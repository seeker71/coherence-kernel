/* ffn_vk.c — headless Vulkan compute carrier for the Form-emitted FFN forward (fglsl-ffn-fwd).
 * y = W2 . gelu(W1.x + b1) + b2, ONE workgroup, one token: the Vulkan twin of
 * native/cuda/form_cuda_ptx_ffn_host.c, with that host's inputs and its CPU oracle unchanged (both
 * phases op-for-op in fp32 under -ffp-contract=off, gelu = the recipe's Taylor tanh-gelu), so a
 * device that honours NoContraction answers to the last bit.
 *
 * Driver-only: dlopen of the host's Vulkan implementation + vkGetInstanceProcAddr bootstrap, links
 * no Vulkan. The shader is text Form emits; glslangValidator mints the .spv; this file carries it.
 *
 * Build (macOS, MoltenVK): clang -O2 -ffp-contract=off -I <dir holding vulkan/ and vk_video/> ffn_vk.c -o ffn_vk
 *   (the Android NDK sysroot's usr/include holds both directories)
 * Build (Android, NDK arm64): aarch64-linux-android24-clang -O2 -ffp-contract=off ffn_vk.c -o ffn_vk -ldl
 * Build (Windows, TDM-GCC): gcc -O2 -ffp-contract=off -I <Vulkan-Headers>/include ffn_vk.c -o ffn_vk.exe
 * Run:  ffn_vk ffn.spv [indim hid outd]     (defaults 16 64 8, the PTX host's)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

/* The same implementation search as matvec_vk.c, first that opens wins. */
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
static PFN_vkQueueSubmit vkQueueSubmit;
static PFN_vkQueueWaitIdle vkQueueWaitIdle;

#define LOAD_I(inst,name) do { name=(PFN_##name)vkGetInstanceProcAddr((inst),#name); \
  if(!name) DIE("missing entry point: " #name); } while(0)

typedef struct { uint32_t indim; uint32_t hid; uint32_t outd; } PushC;
#define NBUF 7   /* W1 B1 W2 B2 X Y A, the shader's bindings 0..6 */

static uint32_t find_mem_type(const VkPhysicalDeviceMemoryProperties *mp, uint32_t typeBits, VkMemoryPropertyFlags flags) {
    for (uint32_t i = 0; i < mp->memoryTypeCount; ++i)
        if ((typeBits & (1u << i)) && (mp->memoryTypes[i].propertyFlags & flags) == flags) return i;
    DIE("no HOST_VISIBLE|HOST_COHERENT memory type"); return 0;
}
static void make_buffer(VkDevice dev, const VkPhysicalDeviceMemoryProperties *mp, VkDeviceSize size, VkBuffer *buf, VkDeviceMemory *mem) {
    VkBufferCreateInfo bci = {0};
    bci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO; bci.size = size;
    bci.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT; bci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VKCHECK(vkCreateBuffer(dev, &bci, NULL, buf));
    VkMemoryRequirements req; vkGetBufferMemoryRequirements(dev, *buf, &req);
    VkMemoryAllocateInfo mai = {0};
    mai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO; mai.allocationSize = req.size;
    mai.memoryTypeIndex = find_mem_type(mp, req.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    VKCHECK(vkAllocateMemory(dev, &mai, NULL, mem));
    VKCHECK(vkBindBufferMemory(dev, *buf, *mem, 0));
}
static void upload(VkDevice dev, VkDeviceMemory mem, const void *src, size_t n) {
    void *p; VKCHECK(vkMapMemory(dev, mem, 0, n, 0, &p));
    if (src) memcpy(p, src, n); else memset(p, 0, n);
    vkUnmapMemory(dev, mem);
}
static uint32_t *read_spv(const char *path, size_t *out_bytes) {
    FILE *f = fopen(path, "rb"); if (!f) DIE("cannot open .spv");
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    if (n <= 0 || (n & 3)) DIE(".spv size invalid");
    uint32_t *code = malloc((size_t)n);
    if (fread(code, 1, (size_t)n, f) != (size_t)n) DIE(".spv read short");
    fclose(f);
    if (code[0] != 0x07230203u) DIE(".spv bad magic");
    *out_bytes = (size_t)n; return code;
}
static float val(int n) { return (float)n / 256.0f; }

/* CPU oracle gelu — form_cuda_ptx_ffn_host.c's, unchanged: the recipe's Taylor tanh-gelu, fp32 */
static float fexp_small(float x){ float n=1.0f,t=1.0f,a=1.0f; while(n<=14.0f){ t=t*(x/n); a=a+t; n=n+1.0f; } return a; }
static float fexpf_(float x){ int k=0; while((x<0.0f?-x:x)>0.5f){ x=x/2.0f; k++; } float v=fexp_small(x); while(k>0){ v=v*v; k--; } return v; }
static float ftanh(float x){ float e=fexpf_(2.0f*x); return (e-1.0f)/(e+1.0f); }
static float fgelu(float x){ float z=0.7978845608028654f*(x+0.044715f*(x*(x*x))); return (0.5f*x)*(1.0f+ftanh(z)); }

int main(int argc, char **argv) {
    const char *spv_path = (argc > 1) ? argv[1] : "ffn.spv";
    int indim = (argc > 2) ? atoi(argv[2]) : 16;
    int hid   = (argc > 3) ? atoi(argv[3]) : 64;
    int outd  = (argc > 4) ? atoi(argv[4]) : 8;
    if (indim <= 0 || hid <= 0 || outd <= 0) DIE("FAIL bad dims");

#if defined(__APPLE__)
    /* MoltenVK compiles its MSL with fast math unless told otherwise, and fast math may reassociate
     * and approximate division: the bit-exact contract (precise -> NoContraction) needs it off.
     * Measured 2026-09-10 on this shader: precise alone 4/8 rows exact, fast math off alone 6/8,
     * both 8/8. Set before MoltenVK loads, because it reads its configuration once. */
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
    app.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO; app.pApplicationName = "form-ffn";
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
    LOAD_I(inst, vkCmdPushConstants); LOAD_I(inst, vkCmdDispatch);
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

    /* inputs: form_cuda_ptx_ffn_host.c's, in [-0.5,0.5) so the gelu argument stays inside fp32 exp */
    size_t nW1 = (size_t)hid * indim, nW2 = (size_t)outd * hid;
    float *w1 = malloc(nW1 * 4), *b1 = malloc((size_t)hid * 4), *w2 = malloc(nW2 * 4), *b2 = malloc((size_t)outd * 4);
    float *x = malloc((size_t)indim * 4), *a = malloc((size_t)hid * 4), *yref = malloc((size_t)outd * 4), *yg = malloc((size_t)outd * 4);
    for (int k = 0; k < hid; k++) { for (int j = 0; j < indim; j++) w1[(size_t)k * indim + j] = val((k * 31 + j * 17) % 256 - 128); b1[k] = val((k * 7) % 256 - 128); }
    for (int j = 0; j < indim; j++) x[j] = val((j * 13) % 256 - 128);
    for (int i = 0; i < outd; i++) { for (int k = 0; k < hid; k++) w2[(size_t)i * hid + k] = val((i * 23 + k * 11) % 256 - 128); b2[i] = val((i * 5) % 256 - 128); }

    /* CPU reference: two phases, serial downward right-folds, form_cuda_ptx_ffn_host.c's */
    for (int k = 0; k < hid; k++) {
        float acc = 0.0f; for (int j = indim; j > 0;) { j--; float p = w1[(size_t)k * indim + j] * x[j]; acc = p + acc; }
        float hk = acc + b1[k]; a[k] = fgelu(hk);
    }
    for (int i = 0; i < outd; i++) {
        float acc = 0.0f; for (int k = hid; k > 0;) { k--; float p = w2[(size_t)i * hid + k] * a[k]; acc = p + acc; }
        yref[i] = acc + b2[i];
    }

    size_t sz[NBUF] = { nW1 * 4, (size_t)hid * 4, nW2 * 4, (size_t)outd * 4, (size_t)indim * 4, (size_t)outd * 4, (size_t)hid * 4 };
    const void *src[NBUF] = { w1, b1, w2, b2, x, NULL, NULL };
    VkBuffer bufs[NBUF]; VkDeviceMemory mems[NBUF];
    for (int i = 0; i < NBUF; ++i) { make_buffer(dev, &memProps, sz[i], &bufs[i], &mems[i]); upload(dev, mems[i], src[i], sz[i]); }

    VkDescriptorSetLayoutBinding binds[NBUF] = {0};
    for (int i = 0; i < NBUF; ++i) {
        binds[i].binding = (uint32_t)i; binds[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        binds[i].descriptorCount = 1; binds[i].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    }
    VkDescriptorSetLayoutCreateInfo dslci = {0};
    dslci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO; dslci.bindingCount = NBUF; dslci.pBindings = binds;
    VkDescriptorSetLayout dsl = VK_NULL_HANDLE; VKCHECK(vkCreateDescriptorSetLayout(dev, &dslci, NULL, &dsl));

    VkDescriptorPoolSize psize = {0};
    psize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; psize.descriptorCount = NBUF;
    VkDescriptorPoolCreateInfo dpci = {0};
    dpci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO; dpci.maxSets = 1; dpci.poolSizeCount = 1; dpci.pPoolSizes = &psize;
    VkDescriptorPool pool = VK_NULL_HANDLE; VKCHECK(vkCreateDescriptorPool(dev, &dpci, NULL, &pool));
    VkDescriptorSetAllocateInfo dsai = {0};
    dsai.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO; dsai.descriptorPool = pool; dsai.descriptorSetCount = 1; dsai.pSetLayouts = &dsl;
    VkDescriptorSet dset = VK_NULL_HANDLE; VKCHECK(vkAllocateDescriptorSets(dev, &dsai, &dset));

    VkDescriptorBufferInfo dbi[NBUF];
    VkWriteDescriptorSet writes[NBUF] = {0};
    for (int i = 0; i < NBUF; ++i) {
        dbi[i].buffer = bufs[i]; dbi[i].offset = 0; dbi[i].range = VK_WHOLE_SIZE;
        writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET; writes[i].dstSet = dset;
        writes[i].dstBinding = (uint32_t)i; writes[i].descriptorCount = 1;
        writes[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; writes[i].pBufferInfo = &dbi[i];
    }
    vkUpdateDescriptorSets(dev, NBUF, writes, 0, NULL);

    VkPushConstantRange pcr = {0};
    pcr.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT; pcr.offset = 0; pcr.size = sizeof(PushC);
    VkPipelineLayoutCreateInfo plci = {0};
    plci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO; plci.setLayoutCount = 1; plci.pSetLayouts = &dsl;
    plci.pushConstantRangeCount = 1; plci.pPushConstantRanges = &pcr;
    VkPipelineLayout playout = VK_NULL_HANDLE; VKCHECK(vkCreatePipelineLayout(dev, &plci, NULL, &playout));

    size_t spvBytes = 0; uint32_t *spv = read_spv(spv_path, &spvBytes);
    VkShaderModuleCreateInfo smci = {0};
    smci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO; smci.codeSize = spvBytes; smci.pCode = spv;
    VkShaderModule shader = VK_NULL_HANDLE; VKCHECK(vkCreateShaderModule(dev, &smci, NULL, &shader));

    VkPipelineShaderStageCreateInfo stage = {0};
    stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO; stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    stage.module = shader; stage.pName = "main";
    VkComputePipelineCreateInfo cpci = {0};
    cpci.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO; cpci.stage = stage; cpci.layout = playout;
    cpci.basePipelineIndex = -1;
    VkPipeline pipe = VK_NULL_HANDLE; VKCHECK(vkCreateComputePipelines(dev, VK_NULL_HANDLE, 1, &cpci, NULL, &pipe));

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
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, pipe);
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, playout, 0, 1, &dset, 0, NULL);
    PushC pc = { (uint32_t)indim, (uint32_t)hid, (uint32_t)outd };
    vkCmdPushConstants(cmd, playout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(pc), &pc);
    vkCmdDispatch(cmd, 1, 1, 1);   /* ONE workgroup: the shader strides both phases by its size, a barrier between */
    VKCHECK(vkEndCommandBuffer(cmd));

    VkSubmitInfo si = {0};
    si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO; si.commandBufferCount = 1; si.pCommandBuffers = &cmd;
    VKCHECK(vkQueueSubmit(queue, 1, &si, VK_NULL_HANDLE));
    VKCHECK(vkQueueWaitIdle(queue));

    void *p; VKCHECK(vkMapMemory(dev, mems[5], 0, sz[5], 0, &p)); memcpy(yg, p, sz[5]); vkUnmapMemory(dev, mems[5]);

    int exact = 0; float max_abs = 0.0f;
    for (int i = 0; i < outd; i++) {
        uint32_t ga, gb; memcpy(&ga, &yg[i], 4); memcpy(&gb, &yref[i], 4);
        if (ga == gb) exact++;
        float d = yg[i] - yref[i]; if (d < 0) d = -d; if (d > max_abs) max_abs = d;
    }
    printf("device=%s (Vulkan)\n", props.deviceName);
    printf("kernel=form_ffn_fwd module=%s (%zu bytes SPIR-V)  indim=%d hid=%d outd=%d\n", spv_path, spvBytes, indim, hid, outd);
    printf("parity_bitexact_y=%d/%d max_abs_diff=%g\n", exact, outd, (double)max_abs);
    printf("runtime_deps=%s only (Form-minted SPIR-V)\n", vklib);

    free(spv);
    vkDestroyCommandPool(dev, cpool, NULL); vkDestroyPipeline(dev, pipe, NULL);
    vkDestroyShaderModule(dev, shader, NULL); vkDestroyPipelineLayout(dev, playout, NULL);
    vkDestroyDescriptorPool(dev, pool, NULL); vkDestroyDescriptorSetLayout(dev, dsl, NULL);
    for (int i = 0; i < NBUF; ++i) { vkDestroyBuffer(dev, bufs[i], NULL); vkFreeMemory(dev, mems[i], NULL); }
    vkDestroyDevice(dev, NULL); vkDestroyInstance(inst, NULL); DLCLOSE(lib);
    if (exact != outd) { printf("FAIL  not bit-exact\n"); return 1; }
    printf("ok — Form-emitted FFN (matvec+gelu+matvec) ran on the Vulkan driver alone, bit-exact to the recipe\n");
    return 0;
}
