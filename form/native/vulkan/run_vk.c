/* run_vk.c — the Vulkan lane's carrier for any kernel Form writes: it runs a PLAN.
 *
 * The plan names the SPIR-V modules, the buffers with their initial 32-bit words, the dispatches in
 * order (bindings, push constants, workgroup counts) and the buffers to read back. Every dispatch is
 * recorded into ONE command buffer with a compute-to-compute memory barrier between neighbours, and
 * submitted once, so a kernel graph (a whole block) runs the way the Metal door runs one.
 *
 * Words cross as unsigned decimals in both directions, so a float travels as its bits: Form encodes
 * the inputs (md-f32-bits) and decodes the answer (ln-f32-decode), and nothing is rounded through text.
 * The oracle is the Form recipe, never this file.
 *
 * Plan (whitespace-separated words):
 *   spv  <path>                                        kernel k, numbered in order of appearance
 *   buf  <n> <w0> ... <w(n-1)>                         buffer b, numbered in order of appearance
 *   zero <n>                                           buffer b, n zero words
 *   run  <k> <nb> <b0> .. <np> <p0> .. <gx> <gy> <gz>  binding i of the dispatch is buffer b_i
 *   read <b>
 * Output: "device=<name>", "vklib=<path>", "elapsed-us <n>", then per read "buf <b> <n> <w0> ...".
 *
 * Driver-only, like matvec_vk.c: dlopen of the host's Vulkan implementation, links no Vulkan.
 * Build (macOS, MoltenVK): clang -O2 -ffp-contract=off -I <dir holding vulkan/ and vk_video/> run_vk.c -o run_vk
 *   (the Android NDK sysroot's usr/include holds both directories)
 * Build (Android, NDK arm64): aarch64-linux-android24-clang -O2 -ffp-contract=off run_vk.c -o run_vk -ldl
 * Run:  run_vk plan.txt
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#if defined(_WIN32)
  #include <windows.h>
  static const char *const VKLIBS[] = { "vulkan-1.dll", 0 };
  #define DLOPEN(n)  ((void*)LoadLibraryA(n))
  #define DLSYM(h,n) ((void*)GetProcAddress((HMODULE)(h),(n)))
  #define DLCLOSE(h) FreeLibrary((HMODULE)(h))
#else
  #include <dlfcn.h>
  #if defined(__APPLE__)
  static const char *const VKLIBS[] = { "/opt/homebrew/lib/libMoltenVK.dylib", "/usr/local/lib/libMoltenVK.dylib",
      "/Applications/Docker.app/Contents/Resources/linuxkit/libMoltenVK.dylib", "libMoltenVK.dylib",
      "libvulkan.1.dylib", 0 };
  #else
  static const char *const VKLIBS[] = { "libvulkan.so", "libvulkan.so.1", 0 };
  #endif
  #define DLOPEN(n)  dlopen((n), RTLD_NOW|RTLD_LOCAL)
  #define DLSYM(h,n) dlsym((h),(n))
  #define DLCLOSE(h) dlclose(h)
#endif

#define VKCHECK(expr) do { VkResult _r=(expr); if (_r!=VK_SUCCESS){ \
  fprintf(stderr,"%s failed: VkResult=%d (line %d)\n",#expr,(int)_r,__LINE__); exit(2);} } while(0)
#define DIE(msg) do { fprintf(stderr,"%s\n",(msg)); exit(2);} while(0)

static PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
static PFN_vkCreateInstance vkCreateInstance;
static PFN_vkDestroyInstance vkDestroyInstance;
static PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
static PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
static PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
static PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties;
static PFN_vkCreateDevice vkCreateDevice;
static PFN_vkDestroyDevice vkDestroyDevice;
static PFN_vkGetDeviceQueue vkGetDeviceQueue;
static PFN_vkCreateBuffer vkCreateBuffer;
static PFN_vkDestroyBuffer vkDestroyBuffer;
static PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements;
static PFN_vkAllocateMemory vkAllocateMemory;
static PFN_vkFreeMemory vkFreeMemory;
static PFN_vkBindBufferMemory vkBindBufferMemory;
static PFN_vkMapMemory vkMapMemory;
static PFN_vkUnmapMemory vkUnmapMemory;
static PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout;
static PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout;
static PFN_vkCreateDescriptorPool vkCreateDescriptorPool;
static PFN_vkDestroyDescriptorPool vkDestroyDescriptorPool;
static PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets;
static PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets;
static PFN_vkCreatePipelineLayout vkCreatePipelineLayout;
static PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout;
static PFN_vkCreateShaderModule vkCreateShaderModule;
static PFN_vkDestroyShaderModule vkDestroyShaderModule;
static PFN_vkCreateComputePipelines vkCreateComputePipelines;
static PFN_vkDestroyPipeline vkDestroyPipeline;
static PFN_vkCreateCommandPool vkCreateCommandPool;
static PFN_vkDestroyCommandPool vkDestroyCommandPool;
static PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers;
static PFN_vkBeginCommandBuffer vkBeginCommandBuffer;
static PFN_vkEndCommandBuffer vkEndCommandBuffer;
static PFN_vkCmdBindPipeline vkCmdBindPipeline;
static PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets;
static PFN_vkCmdPushConstants vkCmdPushConstants;
static PFN_vkCmdDispatch vkCmdDispatch;
static PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier;
static PFN_vkQueueSubmit vkQueueSubmit;
static PFN_vkQueueWaitIdle vkQueueWaitIdle;

#define LOAD_I(inst,name) do { name=(PFN_##name)vkGetInstanceProcAddr((inst),#name); \
  if(!name) DIE("missing entry point: " #name); } while(0)

#define MAXK 64
#define MAXB 256
#define MAXR 512
#define MAXBIND 16
#define MAXPUSH 32

typedef struct { const char *path; VkShaderModule mod; } Kern;
typedef struct { uint32_t n; uint32_t *w; VkBuffer buf; VkDeviceMemory mem; } Buf;
typedef struct {
    int k, nb, np; int b[MAXBIND]; uint32_t p[MAXPUSH]; uint32_t g[3];
    VkDescriptorSetLayout dsl; VkPipelineLayout pl; VkPipeline pipe; VkDescriptorSet set;
} Run;

static Kern K[MAXK]; static int nK;
static Buf B[MAXB]; static int nB;
static Run R[MAXR]; static int nR;
static int RD[MAXB]; static int nRD;

static char *slurp(const char *path) {
    FILE *f = fopen(path, "rb"); if (!f) DIE("cannot open plan");
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    if (n <= 0) DIE("plan is empty");
    char *s = malloc((size_t)n + 1);
    if (fread(s, 1, (size_t)n, f) != (size_t)n) DIE("plan read short");
    fclose(f); s[n] = 0; return s;
}
static char *word(char **cur) {
    char *p = *cur;
    while (*p == ' ' || *p == '\n' || *p == '\t' || *p == '\r') p++;
    if (!*p) { *cur = p; return NULL; }
    char *start = p;
    while (*p && *p != ' ' && *p != '\n' && *p != '\t' && *p != '\r') p++;
    if (*p) *p++ = 0;
    *cur = p; return start;
}
static uint32_t num(char **cur, const char *what) {
    char *t = word(cur);
    if (!t) { fprintf(stderr, "plan ends inside %s\n", what); exit(2); }
    char *e; unsigned long long v = strtoull(t, &e, 10);
    if (*e || v > 0xFFFFFFFFull) { fprintf(stderr, "plan: '%s' is not a 32-bit word (%s)\n", t, what); exit(2); }
    return (uint32_t)v;
}
static void parse(char *cur) {
    for (;;) {
        char *t = word(&cur); if (!t) break;
        if (!strcmp(t, "spv")) {
            if (nK >= MAXK) DIE("plan: too many kernels");
            char *p = word(&cur); if (!p) DIE("plan ends inside spv");
            K[nK++].path = p;
        } else if (!strcmp(t, "buf") || !strcmp(t, "zero")) {
            if (nB >= MAXB) DIE("plan: too many buffers");
            uint32_t n = num(&cur, "buffer size"); if (n == 0) DIE("plan: a buffer holds at least one word");
            Buf *b = &B[nB++]; b->n = n; b->w = calloc(n, 4);
            if (t[0] == 'b') for (uint32_t i = 0; i < n; ++i) b->w[i] = num(&cur, "buffer word");
        } else if (!strcmp(t, "run")) {
            if (nR >= MAXR) DIE("plan: too many dispatches");
            Run *r = &R[nR++];
            r->k = (int)num(&cur, "run kernel"); if (r->k >= nK) DIE("plan: run names a kernel not yet declared");
            r->nb = (int)num(&cur, "run bindings"); if (r->nb < 1 || r->nb > MAXBIND) DIE("plan: bindings out of range");
            for (int i = 0; i < r->nb; ++i) { r->b[i] = (int)num(&cur, "run buffer"); if (r->b[i] >= nB) DIE("plan: run binds a buffer not yet declared"); }
            r->np = (int)num(&cur, "run push count"); if (r->np > MAXPUSH) DIE("plan: push constants out of range");
            for (int i = 0; i < r->np; ++i) r->p[i] = num(&cur, "push word");
            for (int i = 0; i < 3; ++i) { r->g[i] = num(&cur, "workgroups"); if (!r->g[i]) DIE("plan: a dispatch needs at least one workgroup per axis"); }
        } else if (!strcmp(t, "read")) {
            int b = (int)num(&cur, "read buffer"); if (b >= nB) DIE("plan: read names a buffer not yet declared");
            if (nRD >= MAXB) DIE("plan: too many reads");
            RD[nRD++] = b;
        } else { fprintf(stderr, "plan: unknown word '%s'\n", t); exit(2); }
    }
    if (nR == 0) DIE("plan dispatches nothing");
}

static uint32_t find_mem_type(const VkPhysicalDeviceMemoryProperties *mp, uint32_t typeBits, VkMemoryPropertyFlags flags) {
    for (uint32_t i = 0; i < mp->memoryTypeCount; ++i)
        if ((typeBits & (1u << i)) && (mp->memoryTypes[i].propertyFlags & flags) == flags) return i;
    DIE("no HOST_VISIBLE|HOST_COHERENT memory type"); return 0;
}
static uint32_t *read_spv(const char *path, size_t *out_bytes) {
    FILE *f = fopen(path, "rb"); if (!f) { fprintf(stderr, "cannot open %s\n", path); exit(2); }
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    if (n <= 0 || (n & 3)) DIE(".spv size invalid");
    uint32_t *code = malloc((size_t)n);
    if (fread(code, 1, (size_t)n, f) != (size_t)n) DIE(".spv read short");
    fclose(f);
    if (code[0] != 0x07230203u) DIE(".spv bad magic");
    *out_bytes = (size_t)n; return code;
}

int main(int argc, char **argv) {
    if (argc < 2) DIE("usage: run_vk plan.txt");
    parse(slurp(argv[1]));

#if defined(__APPLE__)
    /* MoltenVK compiles its MSL with fast math unless told otherwise; fast math may reassociate and
     * approximate division, which `precise` alone does not prevent (ffn_vk.c measured it). */
    setenv("MVK_CONFIG_FAST_MATH_ENABLED", "0", 1);
#endif
    const char *vklib = 0; void *lib = 0;
    for (int k = 0; VKLIBS[k] && !lib; ++k) { lib = DLOPEN(VKLIBS[k]); if (lib) vklib = VKLIBS[k]; }
    if (!lib) DIE("no Vulkan implementation opened on this host");
    vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)DLSYM(lib, "vkGetInstanceProcAddr");
    if (!vkGetInstanceProcAddr) DIE("no vkGetInstanceProcAddr");
    vkCreateInstance = (PFN_vkCreateInstance)vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkCreateInstance");
    if (!vkCreateInstance) DIE("no vkCreateInstance");

    VkApplicationInfo app = {0};
    app.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO; app.pApplicationName = "form-run";
    app.apiVersion = VK_API_VERSION_1_1;
    VkInstanceCreateInfo ici = {0};
    ici.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; ici.pApplicationInfo = &app;
    VkInstance inst = VK_NULL_HANDLE; VKCHECK(vkCreateInstance(&ici, NULL, &inst));

    LOAD_I(inst, vkDestroyInstance); LOAD_I(inst, vkEnumeratePhysicalDevices);
    LOAD_I(inst, vkGetPhysicalDeviceProperties);
    LOAD_I(inst, vkGetPhysicalDeviceQueueFamilyProperties);
    LOAD_I(inst, vkGetPhysicalDeviceMemoryProperties);
    LOAD_I(inst, vkCreateDevice); LOAD_I(inst, vkDestroyDevice); LOAD_I(inst, vkGetDeviceQueue);
    LOAD_I(inst, vkCreateBuffer); LOAD_I(inst, vkDestroyBuffer); LOAD_I(inst, vkGetBufferMemoryRequirements);
    LOAD_I(inst, vkAllocateMemory); LOAD_I(inst, vkFreeMemory); LOAD_I(inst, vkBindBufferMemory);
    LOAD_I(inst, vkMapMemory); LOAD_I(inst, vkUnmapMemory);
    LOAD_I(inst, vkCreateDescriptorSetLayout); LOAD_I(inst, vkDestroyDescriptorSetLayout);
    LOAD_I(inst, vkCreateDescriptorPool); LOAD_I(inst, vkDestroyDescriptorPool);
    LOAD_I(inst, vkAllocateDescriptorSets); LOAD_I(inst, vkUpdateDescriptorSets);
    LOAD_I(inst, vkCreatePipelineLayout); LOAD_I(inst, vkDestroyPipelineLayout);
    LOAD_I(inst, vkCreateShaderModule); LOAD_I(inst, vkDestroyShaderModule);
    LOAD_I(inst, vkCreateComputePipelines); LOAD_I(inst, vkDestroyPipeline);
    LOAD_I(inst, vkCreateCommandPool); LOAD_I(inst, vkDestroyCommandPool);
    LOAD_I(inst, vkAllocateCommandBuffers); LOAD_I(inst, vkBeginCommandBuffer); LOAD_I(inst, vkEndCommandBuffer);
    LOAD_I(inst, vkCmdBindPipeline); LOAD_I(inst, vkCmdBindDescriptorSets);
    LOAD_I(inst, vkCmdPushConstants); LOAD_I(inst, vkCmdDispatch); LOAD_I(inst, vkCmdPipelineBarrier);
    LOAD_I(inst, vkQueueSubmit); LOAD_I(inst, vkQueueWaitIdle);

    uint32_t pdCount = 0; VKCHECK(vkEnumeratePhysicalDevices(inst, &pdCount, NULL));
    if (!pdCount) DIE("no Vulkan devices");
    VkPhysicalDevice *pds = malloc(pdCount * sizeof(*pds));
    VKCHECK(vkEnumeratePhysicalDevices(inst, &pdCount, pds));
    VkPhysicalDevice phys = VK_NULL_HANDLE; uint32_t qfam = UINT32_MAX;
    for (uint32_t d = 0; d < pdCount && phys == VK_NULL_HANDLE; ++d) {
        uint32_t qfc = 0; vkGetPhysicalDeviceQueueFamilyProperties(pds[d], &qfc, NULL);
        VkQueueFamilyProperties *qfp = malloc(qfc * sizeof(*qfp));
        vkGetPhysicalDeviceQueueFamilyProperties(pds[d], &qfc, qfp);
        for (uint32_t q = 0; q < qfc; ++q)
            if (qfp[q].queueFlags & VK_QUEUE_COMPUTE_BIT) { phys = pds[d]; qfam = q; break; }
        free(qfp);
    }
    free(pds); if (phys == VK_NULL_HANDLE) DIE("no compute queue family");
    VkPhysicalDeviceProperties props; vkGetPhysicalDeviceProperties(phys, &props);
    VkPhysicalDeviceMemoryProperties memProps; vkGetPhysicalDeviceMemoryProperties(phys, &memProps);

    float prio = 1.0f;
    VkDeviceQueueCreateInfo qci = {0};
    qci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO; qci.queueFamilyIndex = qfam;
    qci.queueCount = 1; qci.pQueuePriorities = &prio;
    VkDeviceCreateInfo dci = {0};
    dci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO; dci.queueCreateInfoCount = 1; dci.pQueueCreateInfos = &qci;
    VkDevice dev = VK_NULL_HANDLE; VKCHECK(vkCreateDevice(phys, &dci, NULL, &dev));
    VkQueue queue = VK_NULL_HANDLE; vkGetDeviceQueue(dev, qfam, 0, &queue);

    for (int i = 0; i < nB; ++i) {
        Buf *b = &B[i];
        VkBufferCreateInfo bci = {0};
        bci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO; bci.size = (VkDeviceSize)b->n * 4;
        bci.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT; bci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        VKCHECK(vkCreateBuffer(dev, &bci, NULL, &b->buf));
        VkMemoryRequirements req; vkGetBufferMemoryRequirements(dev, b->buf, &req);
        VkMemoryAllocateInfo mai = {0};
        mai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO; mai.allocationSize = req.size;
        mai.memoryTypeIndex = find_mem_type(&memProps, req.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        VKCHECK(vkAllocateMemory(dev, &mai, NULL, &b->mem));
        VKCHECK(vkBindBufferMemory(dev, b->buf, b->mem, 0));
        void *p; VKCHECK(vkMapMemory(dev, b->mem, 0, (VkDeviceSize)b->n * 4, 0, &p));
        memcpy(p, b->w, (size_t)b->n * 4); vkUnmapMemory(dev, b->mem);
    }
    for (int k = 0; k < nK; ++k) {
        size_t bytes = 0; uint32_t *code = read_spv(K[k].path, &bytes);
        VkShaderModuleCreateInfo smci = {0};
        smci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO; smci.codeSize = bytes; smci.pCode = code;
        VKCHECK(vkCreateShaderModule(dev, &smci, NULL, &K[k].mod));
        free(code);
    }

    uint32_t totalBinds = 0;
    for (int r = 0; r < nR; ++r) totalBinds += (uint32_t)R[r].nb;
    VkDescriptorPoolSize psize = {0};
    psize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; psize.descriptorCount = totalBinds;
    VkDescriptorPoolCreateInfo dpci = {0};
    dpci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO; dpci.maxSets = (uint32_t)nR;
    dpci.poolSizeCount = 1; dpci.pPoolSizes = &psize;
    VkDescriptorPool pool = VK_NULL_HANDLE; VKCHECK(vkCreateDescriptorPool(dev, &dpci, NULL, &pool));

    for (int ri = 0; ri < nR; ++ri) {
        Run *r = &R[ri];
        VkDescriptorSetLayoutBinding binds[MAXBIND]; memset(binds, 0, sizeof binds);
        for (int i = 0; i < r->nb; ++i) {
            binds[i].binding = (uint32_t)i; binds[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            binds[i].descriptorCount = 1; binds[i].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        }
        VkDescriptorSetLayoutCreateInfo dslci = {0};
        dslci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        dslci.bindingCount = (uint32_t)r->nb; dslci.pBindings = binds;
        VKCHECK(vkCreateDescriptorSetLayout(dev, &dslci, NULL, &r->dsl));

        VkPushConstantRange pcr = {0};
        pcr.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT; pcr.offset = 0; pcr.size = (uint32_t)r->np * 4;
        VkPipelineLayoutCreateInfo plci = {0};
        plci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO; plci.setLayoutCount = 1; plci.pSetLayouts = &r->dsl;
        plci.pushConstantRangeCount = r->np ? 1 : 0; plci.pPushConstantRanges = r->np ? &pcr : NULL;
        VKCHECK(vkCreatePipelineLayout(dev, &plci, NULL, &r->pl));

        VkPipelineShaderStageCreateInfo stage = {0};
        stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO; stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        stage.module = K[r->k].mod; stage.pName = "main";
        VkComputePipelineCreateInfo cpci = {0};
        cpci.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO; cpci.stage = stage; cpci.layout = r->pl;
        cpci.basePipelineIndex = -1;
        VKCHECK(vkCreateComputePipelines(dev, VK_NULL_HANDLE, 1, &cpci, NULL, &r->pipe));

        VkDescriptorSetAllocateInfo dsai = {0};
        dsai.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO; dsai.descriptorPool = pool;
        dsai.descriptorSetCount = 1; dsai.pSetLayouts = &r->dsl;
        VKCHECK(vkAllocateDescriptorSets(dev, &dsai, &r->set));
        VkDescriptorBufferInfo dbi[MAXBIND]; VkWriteDescriptorSet writes[MAXBIND]; memset(writes, 0, sizeof writes);
        for (int i = 0; i < r->nb; ++i) {
            dbi[i].buffer = B[r->b[i]].buf; dbi[i].offset = 0; dbi[i].range = VK_WHOLE_SIZE;
            writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET; writes[i].dstSet = r->set;
            writes[i].dstBinding = (uint32_t)i; writes[i].descriptorCount = 1;
            writes[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; writes[i].pBufferInfo = &dbi[i];
        }
        vkUpdateDescriptorSets(dev, (uint32_t)r->nb, writes, 0, NULL);
    }

    VkCommandPoolCreateInfo cpoolci = {0};
    cpoolci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO; cpoolci.queueFamilyIndex = qfam;
    VkCommandPool cpool = VK_NULL_HANDLE; VKCHECK(vkCreateCommandPool(dev, &cpoolci, NULL, &cpool));
    VkCommandBufferAllocateInfo cbai = {0};
    cbai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO; cbai.commandPool = cpool;
    cbai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; cbai.commandBufferCount = 1;
    VkCommandBuffer cmd = VK_NULL_HANDLE; VKCHECK(vkAllocateCommandBuffers(dev, &cbai, &cmd));
    VkCommandBufferBeginInfo cbbi = {0};
    cbbi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO; cbbi.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VKCHECK(vkBeginCommandBuffer(cmd, &cbbi));
    for (int ri = 0; ri < nR; ++ri) {
        Run *r = &R[ri];
        if (ri > 0) {   /* the previous dispatch's writes are visible to this one's reads and writes */
            VkMemoryBarrier mb = {0};
            mb.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
            mb.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
            mb.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
            vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                0, 1, &mb, 0, NULL, 0, NULL);
        }
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, r->pipe);
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, r->pl, 0, 1, &r->set, 0, NULL);
        if (r->np) vkCmdPushConstants(cmd, r->pl, VK_SHADER_STAGE_COMPUTE_BIT, 0, (uint32_t)r->np * 4, r->p);
        vkCmdDispatch(cmd, r->g[0], r->g[1], r->g[2]);
    }
    VKCHECK(vkEndCommandBuffer(cmd));

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    VkSubmitInfo si = {0};
    si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO; si.commandBufferCount = 1; si.pCommandBuffers = &cmd;
    VKCHECK(vkQueueSubmit(queue, 1, &si, VK_NULL_HANDLE));
    VKCHECK(vkQueueWaitIdle(queue));
    clock_gettime(CLOCK_MONOTONIC, &t1);
    long long us = (long long)(t1.tv_sec - t0.tv_sec) * 1000000LL + (t1.tv_nsec - t0.tv_nsec) / 1000;

    printf("device=%s\n", props.deviceName);
    printf("vklib=%s\n", vklib);
    printf("elapsed-us %lld\n", us);
    for (int i = 0; i < nRD; ++i) {
        Buf *b = &B[RD[i]];
        void *p; VKCHECK(vkMapMemory(dev, b->mem, 0, (VkDeviceSize)b->n * 4, 0, &p));
        const uint32_t *w = (const uint32_t *)p;
        printf("buf %d %u", RD[i], b->n);
        for (uint32_t j = 0; j < b->n; ++j) printf(" %u", w[j]);
        printf("\n");
        vkUnmapMemory(dev, b->mem);
    }

    vkDestroyCommandPool(dev, cpool, NULL);
    for (int ri = 0; ri < nR; ++ri) {
        vkDestroyPipeline(dev, R[ri].pipe, NULL); vkDestroyPipelineLayout(dev, R[ri].pl, NULL);
        vkDestroyDescriptorSetLayout(dev, R[ri].dsl, NULL);
    }
    vkDestroyDescriptorPool(dev, pool, NULL);
    for (int k = 0; k < nK; ++k) vkDestroyShaderModule(dev, K[k].mod, NULL);
    for (int i = 0; i < nB; ++i) { vkDestroyBuffer(dev, B[i].buf, NULL); vkFreeMemory(dev, B[i].mem, NULL); }
    vkDestroyDevice(dev, NULL); vkDestroyInstance(inst, NULL); DLCLOSE(lib);
    return 0;
}
