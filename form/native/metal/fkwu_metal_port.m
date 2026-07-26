#import <Foundation/Foundation.h>
#import <Metal/Metal.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <float.h>
#include <math.h>
#include <sys/mman.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

static long long fk_out(char *out, long long cap, const char *fmt, ...) {
    if (cap <= 0) return -1;
    va_list args;
    va_start(args, fmt);
    int n = vsnprintf(out, (size_t)cap, fmt, args);
    va_end(args);
    if (n < 0) return -1;
    return n < cap ? n : cap - 1;
}

static char *fk_copy_cstr(const char *bytes, long long len) {
    if (len < 0) return NULL;
    char *copy = malloc((size_t)len + 1);
    if (copy == NULL) return NULL;
    memcpy(copy, bytes, (size_t)len);
    copy[len] = 0;
    return copy;
}

static id<MTLDevice> fk_model_device;
static id<MTLBuffer> *fk_model_views;
static size_t *fk_model_view_offsets;
static size_t fk_model_view_count;
struct fk_model_library {
    char *name;
    id<MTLLibrary> library;
};
static struct fk_model_library *fk_model_libraries;
static size_t fk_model_library_count;
struct fk_model_pipeline {
    char *name;
    id<MTLComputePipelineState> pipeline;
};
static struct fk_model_pipeline *fk_model_pipelines;
static size_t fk_model_pipeline_count;
static id<MTLCommandQueue> fk_model_queue;
static id<MTLBuffer> fk_model_stage;
static size_t fk_model_stage_count;
static id<MTLBuffer> fk_model_residual;
static size_t fk_model_residual_count;
struct fk_model_state { char *name; id<MTLBuffer> buffer; size_t count; };
static struct fk_model_state *fk_model_states;
static size_t fk_model_state_count;
static uint32_t fk_model_route_ids[16];
static float fk_model_route_weights[16];
static size_t fk_model_route_count;
struct fk_model_dkv {
    id<MTLBuffer> raw, comp, state_kv, state_score;
    id<MTLBuffer> compressor_kv, compressor_gate, compressor_ape, compressor_norm;
    id<MTLBuffer> attention_sinks;
    size_t n_raw, n_comp, raw_cap, comp_cap, width;
    size_t attention_sinks_bytes;
    uint32_t ratio;
};
static struct fk_model_dkv fk_model_dkv[64];
static void *fk_model_mapping;
static size_t fk_model_mapping_len;
static int fk_model_fd = -1;

static double fk_metal_now_ms(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec * 1000.0 + (double)t.tv_nsec / 1000000.0;
}

static int fk_metal_wait_observed(
    id<MTLCommandBuffer> command, const char *membrane,
    double heartbeat_ms, double *elapsed_ms) {
    double started = fk_metal_now_ms();
    double next_heartbeat = heartbeat_ms;
    for (;;) {
        MTLCommandBufferStatus status = command.status;
        double elapsed = fk_metal_now_ms() - started;
        if (elapsed_ms != NULL) *elapsed_ms = elapsed;
        if (status == MTLCommandBufferStatusCompleted) return 1;
        if (status == MTLCommandBufferStatusError) return -1;
        if (elapsed >= next_heartbeat) {
            fprintf(stdout,
                "WAIT %s\n"
                "elapsed-ms=%.3f\ncommand-status=%ld\n"
                "result=nothing-yet\nalternative-node-id=continue-observing-current-command\n"
                "stop-signal=process-signal\nnetwork=0\nremote=0\noutcome=active\n"
                "rewitness-offer=%s-current-command\n",
                membrane, elapsed, (long)status, membrane);
            fflush(stdout);
            next_heartbeat += heartbeat_ms;
        }
        usleep(1000);
    }
}

static id<MTLLibrary> fk_model_library_named(const char *name) {
    for (size_t i = 0; i < fk_model_library_count; i++) {
        if (strcmp(fk_model_libraries[i].name, name) == 0)
            return fk_model_libraries[i].library;
    }
    return nil;
}

static id<MTLComputePipelineState> fk_model_pipeline_for(
    id<MTLFunction> function, NSError **error) {
    if (function == nil) return nil;
    const char *name = function.name.UTF8String;
    if (name == NULL) return nil;
    for (size_t i = 0; i < fk_model_pipeline_count; i++) {
        if (strcmp(fk_model_pipelines[i].name, name) == 0)
            return [fk_model_pipelines[i].pipeline retain];
    }
    id<MTLComputePipelineState> pipeline =
        [fk_model_device newComputePipelineStateWithFunction:function error:error];
    if (pipeline == nil) return nil;
    struct fk_model_pipeline *grown = realloc(
        fk_model_pipelines,
        (fk_model_pipeline_count + 1) * sizeof(*fk_model_pipelines));
    if (grown == NULL) {
        [pipeline release];
        return nil;
    }
    fk_model_pipelines = grown;
    fk_model_pipelines[fk_model_pipeline_count].name = strdup(name);
    fk_model_pipelines[fk_model_pipeline_count].pipeline = pipeline;
    if (fk_model_pipelines[fk_model_pipeline_count].name == NULL) {
        [pipeline release];
        return nil;
    }
    fk_model_pipeline_count++;
    return [pipeline retain];
}

static id<MTLCommandQueue> fk_model_command_queue(void) {
    if (fk_model_queue == nil)
        fk_model_queue = [fk_model_device newCommandQueue];
    return [fk_model_queue retain];
}

static void fk_metal_model_release(void) {
    for(size_t i=0;i<64;i++){
        [fk_model_dkv[i].raw release];[fk_model_dkv[i].comp release];
        [fk_model_dkv[i].state_kv release];[fk_model_dkv[i].state_score release];
        [fk_model_dkv[i].compressor_kv release];
        [fk_model_dkv[i].compressor_gate release];
        [fk_model_dkv[i].compressor_ape release];
        [fk_model_dkv[i].compressor_norm release];
        [fk_model_dkv[i].attention_sinks release];
        memset(&fk_model_dkv[i],0,sizeof(fk_model_dkv[i]));
    }
    for(size_t i=0;i<fk_model_state_count;i++){
        free(fk_model_states[i].name);[fk_model_states[i].buffer release];}
    free(fk_model_states);fk_model_states=NULL;fk_model_state_count=0;
    fk_model_route_count=0;
    for (size_t i = 0; i < fk_model_pipeline_count; i++) {
        free(fk_model_pipelines[i].name);
        [fk_model_pipelines[i].pipeline release];
    }
    free(fk_model_pipelines);
    fk_model_pipelines = NULL;
    fk_model_pipeline_count = 0;
    [fk_model_queue release];
    fk_model_queue = nil;
    [fk_model_stage release];
    [fk_model_residual release];
    fk_model_stage = nil;
    fk_model_residual = nil;
    fk_model_stage_count = 0;
    fk_model_residual_count = 0;
    for (size_t i = 0; i < fk_model_library_count; i++) {
        free(fk_model_libraries[i].name);
        [fk_model_libraries[i].library release];
    }
    free(fk_model_libraries);
    fk_model_libraries = NULL;
    fk_model_library_count = 0;
    for (size_t i = 0; i < fk_model_view_count; i++) {
        [fk_model_views[i] release];
    }
    free(fk_model_views);
    free(fk_model_view_offsets);
    fk_model_views = NULL;
    fk_model_view_offsets = NULL;
    fk_model_view_count = 0;
    [fk_model_device release];
    fk_model_device = nil;
    if (fk_model_mapping != NULL && fk_model_mapping_len > 0) {
        munmap(fk_model_mapping, fk_model_mapping_len);
    }
    if (fk_model_fd >= 0) close(fk_model_fd);
    fk_model_mapping = NULL;
    fk_model_mapping_len = 0;
    fk_model_fd = -1;
}

long long fk_metal_model_open_external(
    const char *path, long long path_len,
    const char *plan, long long plan_len,
    char *out, long long cap) {
    @autoreleasepool {
        char *path_text = fk_copy_cstr(path, path_len);
        char *plan_text = fk_copy_cstr(plan, plan_len);
        if (path_text == NULL || plan_text == NULL) {
            free(path_text);
            free(plan_text);
            return fk_out(out, cap, "FAIL metal_model_open allocation\n");
        }
        char *cursor = plan_text;
        char *end = NULL;
        unsigned long long step = strtoull(cursor, &end, 10);
        if (end == cursor) goto invalid_plan;
        cursor = end;
        unsigned long long view_limit = strtoull(cursor, &end, 10);
        if (end == cursor) goto invalid_plan;
        cursor = end;
        unsigned long long view_count = strtoull(cursor, &end, 10);
        if (end == cursor || step == 0 || view_limit == 0 ||
            view_count == 0 || view_count > 1024) goto invalid_plan;

        fk_metal_model_release();
        double started = fk_metal_now_ms();
        fk_model_device = MTLCreateSystemDefaultDevice();
        if (fk_model_device == nil) {
            free(path_text);
            free(plan_text);
            return fk_out(out, cap, "FAIL metal_model_open device=none\n");
        }
        size_t page = (size_t)getpagesize();
        if ((step % page) != 0 || view_limit > fk_model_device.maxBufferLength) {
            fk_metal_model_release();
            free(path_text);
            free(plan_text);
            return fk_out(out, cap,
                "FAIL metal_model_open plan step-page-aligned=%d view-within-device=%d\n",
                (step % page) == 0, view_limit <= fk_model_device.maxBufferLength);
        }
        fk_model_fd = open(path_text, O_RDONLY);
        struct stat st;
        if (fk_model_fd < 0 || fstat(fk_model_fd, &st) != 0 || st.st_size <= 0) {
            fk_metal_model_release();
            free(path_text);
            free(plan_text);
            return fk_out(out, cap, "FAIL metal_model_open file\n");
        }
        if (flock(fk_model_fd, LOCK_EX | LOCK_NB) != 0) {
            int lock_errno = errno;
            fk_metal_model_release();
            free(path_text);
            free(plan_text);
            return fk_out(out, cap,
                "NOTHING metal_model_open\n"
                "reason=model-session-busy\n"
                "errno=%d\n"
                "model-session=exclusive-by-model-file\n"
                "alternative-node-id=existing-model-session\n"
                "network=0\nremote=0\noutcome=nothing\n"
                "rewitness-offer=metal-model-open-after-session-release\n",
                lock_errno);
        }
        size_t file_len = (size_t)st.st_size;
        fk_model_mapping_len = (file_len + page - 1) / page * page;
        fk_model_mapping = mmap(
            NULL, fk_model_mapping_len, PROT_READ, MAP_PRIVATE, fk_model_fd, 0);
        if (fk_model_mapping == MAP_FAILED) {
            fk_model_mapping = NULL;
            fk_metal_model_release();
            free(path_text);
            free(plan_text);
            return fk_out(out, cap, "FAIL metal_model_open mmap\n");
        }
        fk_model_views = calloc((size_t)view_count, sizeof(*fk_model_views));
        fk_model_view_offsets = calloc((size_t)view_count, sizeof(*fk_model_view_offsets));
        if (fk_model_views == NULL || fk_model_view_offsets == NULL) {
            fk_metal_model_release();
            free(path_text);
            free(plan_text);
            return fk_out(out, cap, "FAIL metal_model_open allocation=views\n");
        }
        for (unsigned long long i = 0; i < view_count; i++) {
            unsigned long long start = i * step;
            if (start >= fk_model_mapping_len) break;
            size_t length = (size_t)((fk_model_mapping_len - start) < view_limit
                ? (fk_model_mapping_len - start) : view_limit);
            id<MTLBuffer> view = [fk_model_device
                newBufferWithBytesNoCopy:(char *)fk_model_mapping + start
                                  length:length
                                 options:MTLResourceStorageModeShared
                             deallocator:nil];
            if (view == nil) break;
            fk_model_view_offsets[fk_model_view_count] = (size_t)start;
            fk_model_views[fk_model_view_count++] = view;
        }
        if (fk_model_view_count != (size_t)view_count) {
            size_t got = fk_model_view_count;
            fk_metal_model_release();
            free(path_text);
            free(plan_text);
            return fk_out(out, cap,
                "FAIL metal_model_open views=%zu/%llu\n", got, view_count);
        }
        double elapsed = fk_metal_now_ms() - started;
        long long result = fk_out(out, cap,
            "PASS metal_model_open\n"
            "carrier=in-process-metal\n"
            "model-bytes=%zu\n"
            "views=%llu\n"
            "step-bytes=%llu\n"
            "view-limit-bytes=%llu\n"
            "device=%s\n"
            "elapsed-ms=%.3f\n"
            "copy=bytesNoCopy\n"
            "host-exec=0\nshell=0\nswift=0\ntemp=0\nnetwork=0\nremote=0\n"
            "outcome=success\n"
            "rewitness-offer=metal-model-open\n",
            file_len, view_count, step, view_limit,
            fk_model_device.name.UTF8String, elapsed);
        free(path_text);
        free(plan_text);
        return result;

invalid_plan:
        free(path_text);
        free(plan_text);
        return fk_out(out, cap, "FAIL metal_model_open plan\n");
    }
}

long long fk_metal_model_library_external(
    const char *name, long long name_len,
    const char *source, long long source_len,
    char *out, long long cap) {
    @autoreleasepool {
        if (fk_model_device == nil || name_len <= 0 || source_len <= 0) {
            return fk_out(out, cap,
                "FAIL metal_model_library model-open=%d name-bytes=%lld source-bytes=%lld\n",
                fk_model_device != nil, name_len, source_len);
        }
        double started = fk_metal_now_ms();
        NSData *source_data = [NSData dataWithBytes:source length:(NSUInteger)source_len];
        NSString *source_text =
            [[NSString alloc] initWithData:source_data encoding:NSUTF8StringEncoding];
        MTLCompileOptions *options = [MTLCompileOptions new];
        options.mathMode = MTLMathModeSafe;
        NSError *error = nil;
        id<MTLLibrary> library =
            [fk_model_device newLibraryWithSource:source_text options:options error:&error];
        [options release];
        [source_text release];
        if (library == nil) {
            const char *message = error.localizedDescription.UTF8String ?: "unknown";
            return fk_out(out, cap,
                "FAIL metal_model_library compile=%s\nrewitness-offer=repair-msl\n", message);
        }
        struct fk_model_library *grown = realloc(
            fk_model_libraries,
            (fk_model_library_count + 1) * sizeof(*fk_model_libraries));
        char *name_copy = fk_copy_cstr(name, name_len);
        if (grown == NULL || name_copy == NULL) {
            free(name_copy);
            [library release];
            return fk_out(out, cap, "FAIL metal_model_library allocation=registry\n");
        }
        fk_model_libraries = grown;
        fk_model_libraries[fk_model_library_count].name = name_copy;
        fk_model_libraries[fk_model_library_count].library = library;
        fk_model_library_count++;
        return fk_out(out, cap,
            "PASS metal_model_library\n"
            "name=%s\nsource-bytes=%lld\nlibraries=%zu\nelapsed-ms=%.3f\n"
            "carrier=in-process-metal\nhost-exec=0\nshell=0\nswift=0\ntemp=0\nnetwork=0\nremote=0\n"
            "outcome=success\nrewitness-offer=metal-model-library\n",
            name_copy, source_len, fk_model_library_count, fk_metal_now_ms() - started);
    }
}

long long fk_metal_model_embed_external(
    const char *library_name, long long library_name_len,
    const char *kernel_name, long long kernel_name_len,
    const char *plan, long long plan_len,
    char *out, long long cap) {
    @autoreleasepool {
        char *library_text = fk_copy_cstr(library_name, library_name_len);
        char *kernel_text = fk_copy_cstr(kernel_name, kernel_name_len);
        char *plan_text = fk_copy_cstr(plan, plan_len);
        if (library_text == NULL || kernel_text == NULL || plan_text == NULL) {
            free(library_text); free(kernel_text); free(plan_text);
            return fk_out(out, cap, "FAIL metal_model_embed allocation=input\n");
        }
        char *cursor = plan_text;
        char *end = NULL;
        unsigned long long view_index = strtoull(cursor, &end, 10);
        if (end == cursor) goto invalid_embed_plan;
        cursor = end;
        unsigned long long base = strtoull(cursor, &end, 10);
        if (end == cursor) goto invalid_embed_plan;
        cursor = end;
        unsigned long long count = strtoull(cursor, &end, 10);
        if (end == cursor || count == 0 || count > 1048576 ||
            view_index >= fk_model_view_count) goto invalid_embed_plan;

        id<MTLLibrary> library = nil;
        for (size_t i = 0; i < fk_model_library_count; i++) {
            if (strcmp(fk_model_libraries[i].name, library_text) == 0) {
                library = fk_model_libraries[i].library;
                break;
            }
        }
        if (library == nil) {
            long long n = fk_out(out, cap,
                "FAIL metal_model_embed library=%s not-loaded\n", library_text);
            free(library_text); free(kernel_text); free(plan_text);
            return n;
        }
        if (base > fk_model_views[view_index].length ||
            count > (fk_model_views[view_index].length - base) / 2) {
            long long n = fk_out(out, cap,
                "FAIL metal_model_embed range view=%llu base=%llu count=%llu\n",
                view_index, base, count);
            free(library_text); free(kernel_text); free(plan_text);
            return n;
        }

        NSString *kernel_string = [NSString stringWithUTF8String:kernel_text];
        id<MTLFunction> function = [library newFunctionWithName:kernel_string];
        NSError *error = nil;
        id<MTLComputePipelineState> pipeline =
            function == nil ? nil :
            fk_model_pipeline_for(function, &error);
        id<MTLCommandQueue> queue = fk_model_command_queue();
        id<MTLBuffer> result = [fk_model_device
            newBufferWithLength:(NSUInteger)count * sizeof(float)
                        options:MTLResourceStorageModeShared];
        if (pipeline == nil || queue == nil || result == nil) {
            const char *message = error.localizedDescription.UTF8String ?: "unknown";
            [function release]; [pipeline release]; [queue release]; [result release];
            free(library_text); free(kernel_text); free(plan_text);
            return fk_out(out, cap, "FAIL metal_model_embed pipeline=%s\n", message);
        }

        size_t page = (size_t)getpagesize();
        size_t window_start = (size_t)base / page * page;
        size_t window_used = (size_t)base - window_start + (size_t)count * 2;
        size_t window_length = (window_used + page - 1) / page * page;
        size_t absolute_start = fk_model_view_offsets[view_index] + window_start;
        if (absolute_start > fk_model_mapping_len ||
            window_length > fk_model_mapping_len - absolute_start) {
            [function release]; [pipeline release]; [queue release]; [result release];
            free(library_text); free(kernel_text); free(plan_text);
            return fk_out(out, cap, "FAIL metal_model_embed tensor-window-range\n");
        }
        id<MTLBuffer> tensor_window = [fk_model_device
            newBufferWithBytesNoCopy:(char *)fk_model_mapping + absolute_start
                              length:window_length
                             options:MTLResourceStorageModeShared
                         deallocator:nil];
        if (tensor_window == nil) {
            [function release]; [pipeline release]; [queue release]; [result release];
            free(library_text); free(kernel_text); free(plan_text);
            return fk_out(out, cap, "FAIL metal_model_embed tensor-window-create\n");
        }

        double started = fk_metal_now_ms();
        uint64_t base64 = (uint64_t)((size_t)base - window_start);
        uint32_t count32 = (uint32_t)count;
        id<MTLCommandBuffer> command = [queue commandBuffer];
        id<MTLComputeCommandEncoder> encoder = [command computeCommandEncoder];
        [encoder setComputePipelineState:pipeline];
        [encoder setBuffer:tensor_window offset:0 atIndex:0];
        [encoder setBuffer:result offset:0 atIndex:1];
        [encoder setBytes:&base64 length:sizeof(base64) atIndex:2];
        [encoder setBytes:&count32 length:sizeof(count32) atIndex:3];
        NSUInteger width = MIN(
            pipeline.maxTotalThreadsPerThreadgroup,
            pipeline.threadExecutionWidth * 4);
        [encoder dispatchThreads:MTLSizeMake((NSUInteger)count, 1, 1)
            threadsPerThreadgroup:MTLSizeMake(width, 1, 1)];
        [encoder endEncoding];
        [command commit];
        double wait_elapsed = 0.0;
        int wait_result = fk_metal_wait_observed(
            command, "metal-model-embed", 10000.0, &wait_elapsed);
        double elapsed = fk_metal_now_ms() - started;
        if (wait_result < 0 || command.status != MTLCommandBufferStatusCompleted) {
            const char *message = command.error.localizedDescription.UTF8String ?: "unknown";
            [tensor_window release];
            [function release]; [pipeline release]; [queue release]; [result release];
            free(library_text); free(kernel_text); free(plan_text);
            return fk_out(out, cap, "FAIL metal_model_embed dispatch=%s\n", message);
        }

        float *values = result.contents;
        double sum = 0.0;
        double abs_sum = 0.0;
        float minimum = values[0];
        float maximum = values[0];
        size_t finite_count = 0;
        for (size_t i = 0; i < (size_t)count; i++) {
            float value = values[i];
            if (isfinite(value)) finite_count++;
            sum += value;
            abs_sum += fabs((double)value);
            if (value < minimum) minimum = value;
            if (value > maximum) maximum = value;
        }
        long long n = fk_out(out, cap,
            "PASS metal_model_embed\n"
            "library=%s\nkernel=%s\nview=%llu\nbase-bytes=%llu\n"
            "tensor-window-bytes=%zu\nvalues=%llu\n"
            "finite=%zu\nsum=%.17g\nabs-sum=%.17g\nmin=%.9g\nmax=%.9g\n"
            "sample=%.9g,%.9g,%.9g,%.9g\nelapsed-ms=%.3f\n"
            "carrier=in-process-metal\nhost-exec=0\nshell=0\nswift=0\ntemp=0\n"
            "network=0\nremote=0\noutcome=success\n"
            "rewitness-offer=metal-model-embed\n",
            library_text, kernel_text, view_index, base, window_length, count, finite_count,
            sum, abs_sum, minimum, maximum,
            values[0], values[count > 1 ? 1 : 0],
            values[count > 2 ? 2 : 0], values[count > 3 ? 3 : 0], elapsed);
        [fk_model_stage release];
        fk_model_stage = result;
        fk_model_stage_count = (size_t)count;
        [fk_model_residual release];
        fk_model_residual = nil;
        fk_model_residual_count = 0;
        [tensor_window release];
        [function release]; [pipeline release]; [queue release];
        free(library_text); free(kernel_text); free(plan_text);
        return n;

invalid_embed_plan:
        free(library_text); free(kernel_text); free(plan_text);
        return fk_out(out, cap, "FAIL metal_model_embed plan\n");
    }
}

long long fk_metal_model_hc_begin_external(
    const char *library_name, long long library_name_len,
    const char *plan, long long plan_len,
    char *out, long long cap) {
    @autoreleasepool {
        char *library_text = fk_copy_cstr(library_name, library_name_len);
        char *plan_text = fk_copy_cstr(plan, plan_len);
        if (library_text == NULL || plan_text == NULL) {
            free(library_text); free(plan_text);
            return fk_out(out, cap, "FAIL metal_model_hc_begin allocation=input\n");
        }
        char *cursor = plan_text;
        char *end = NULL;
        unsigned long n_embd = strtoul(cursor, &end, 10);
        if (end == cursor) goto invalid_hc_plan;
        cursor = end;
        unsigned long n_hc = strtoul(cursor, &end, 10);
        if (end == cursor) goto invalid_hc_plan;
        cursor = end;
        float eps = strtof(cursor, &end);
        if (end == cursor || n_embd == 0 || n_hc == 0 ||
            n_embd > 1048576 || n_hc > 64 ||
            fk_model_stage == nil || fk_model_stage_count != n_embd) {
            goto invalid_hc_plan;
        }

        id<MTLLibrary> library = nil;
        for (size_t i = 0; i < fk_model_library_count; i++) {
            if (strcmp(fk_model_libraries[i].name, library_text) == 0) {
                library = fk_model_libraries[i].library;
                break;
            }
        }
        if (library == nil) {
            long long n = fk_out(out, cap,
                "FAIL metal_model_hc_begin library=%s not-loaded\n", library_text);
            free(library_text); free(plan_text);
            return n;
        }

        id<MTLFunction> broadcast_fn =
            [library newFunctionWithName:@"form_hc_broadcast_f32"];
        id<MTLFunction> rms_fn =
            [library newFunctionWithName:@"form_hc_rmsnorm_nw_f32"];
        NSError *error = nil;
        id<MTLComputePipelineState> broadcast =
            broadcast_fn == nil ? nil :
            fk_model_pipeline_for(broadcast_fn, &error);
        id<MTLComputePipelineState> rms =
            rms_fn == nil ? nil :
            fk_model_pipeline_for(rms_fn, &error);
        id<MTLCommandQueue> queue = fk_model_command_queue();
        size_t total = (size_t)n_embd * (size_t)n_hc;
        id<MTLBuffer> residual = [fk_model_device
            newBufferWithLength:total * sizeof(float) options:MTLResourceStorageModeShared];
        id<MTLBuffer> normalized = [fk_model_device
            newBufferWithLength:total * sizeof(float) options:MTLResourceStorageModeShared];
        if (broadcast == nil || rms == nil || queue == nil ||
            residual == nil || normalized == nil) {
            const char *message = error.localizedDescription.UTF8String ?: "unknown";
            [broadcast_fn release]; [rms_fn release];
            [broadcast release]; [rms release]; [queue release];
            [residual release]; [normalized release];
            free(library_text); free(plan_text);
            return fk_out(out, cap, "FAIL metal_model_hc_begin pipeline=%s\n", message);
        }

        double started = fk_metal_now_ms();
        uint32_t embd32 = (uint32_t)n_embd;
        uint32_t hc32 = (uint32_t)n_hc;
        uint32_t total32 = (uint32_t)total;
        id<MTLCommandBuffer> command = [queue commandBuffer];
        id<MTLComputeCommandEncoder> encoder = [command computeCommandEncoder];
        [encoder setComputePipelineState:broadcast];
        [encoder setBuffer:fk_model_stage offset:0 atIndex:0];
        [encoder setBuffer:residual offset:0 atIndex:1];
        [encoder setBytes:&hc32 length:4 atIndex:2];
        [encoder setBytes:&embd32 length:4 atIndex:3];
        NSUInteger width = MIN(
            broadcast.maxTotalThreadsPerThreadgroup,
            broadcast.threadExecutionWidth * 4);
        [encoder dispatchThreads:MTLSizeMake(total, 1, 1)
            threadsPerThreadgroup:MTLSizeMake(width, 1, 1)];
        [encoder endEncoding];
        encoder = [command computeCommandEncoder];
        [encoder setComputePipelineState:rms];
        [encoder setBuffer:residual offset:0 atIndex:0];
        [encoder setBuffer:normalized offset:0 atIndex:1];
        [encoder setBytes:&total32 length:4 atIndex:2];
        [encoder setBytes:&eps length:4 atIndex:3];
        [encoder dispatchThreads:MTLSizeMake(1, 1, 1)
            threadsPerThreadgroup:MTLSizeMake(1, 1, 1)];
        [encoder endEncoding];
        [command commit];
        [command waitUntilCompleted];
        double elapsed = fk_metal_now_ms() - started;
        if (command.status != MTLCommandBufferStatusCompleted) {
            const char *message = command.error.localizedDescription.UTF8String ?: "unknown";
            [broadcast_fn release]; [rms_fn release];
            [broadcast release]; [rms release]; [queue release];
            [residual release]; [normalized release];
            free(library_text); free(plan_text);
            return fk_out(out, cap, "FAIL metal_model_hc_begin dispatch=%s\n", message);
        }

        float *values = normalized.contents;
        double sum = 0.0;
        double abs_sum = 0.0;
        size_t finite_count = 0;
        for (size_t i = 0; i < total; i++) {
            if (isfinite(values[i])) finite_count++;
            sum += values[i];
            abs_sum += fabs((double)values[i]);
        }
        [fk_model_residual release];
        fk_model_residual = residual;
        fk_model_residual_count = total;
        [fk_model_stage release];
        fk_model_stage = normalized;
        fk_model_stage_count = total;
        long long n = fk_out(out, cap,
            "PASS metal_model_hc_begin\nlibrary=%s\nn-embd=%lu\nn-hc=%lu\n"
            "residual-values=%zu\nnormalized-values=%zu\nfinite=%zu\n"
            "sum=%.17g\nabs-sum=%.17g\nsample=%.9g,%.9g,%.9g,%.9g\n"
            "elapsed-ms=%.3f\ncarrier=in-process-metal\n"
            "host-exec=0\nshell=0\nswift=0\ntemp=0\nnetwork=0\nremote=0\n"
            "outcome=success\nrewitness-offer=metal-model-hc-begin\n",
            library_text, n_embd, n_hc, total, total, finite_count,
            sum, abs_sum, values[0], values[1], values[2], values[3], elapsed);
        [broadcast_fn release]; [rms_fn release];
        [broadcast release]; [rms release]; [queue release];
        free(library_text); free(plan_text);
        return n;

invalid_hc_plan:
        free(library_text); free(plan_text);
        return fk_out(out, cap,
            "FAIL metal_model_hc_begin plan-or-stage stage-values=%zu\n",
            fk_model_stage_count);
    }
}

long long fk_metal_model_f16_matvec_external(
    const char *library_name, long long library_name_len,
    const char *kernel_name, long long kernel_name_len,
    const char *plan, long long plan_len,
    char *out, long long cap) {
    @autoreleasepool {
        char *ln = fk_copy_cstr(library_name, library_name_len);
        char *kn = fk_copy_cstr(kernel_name, kernel_name_len);
        char *pt = fk_copy_cstr(plan, plan_len);
        if (!ln || !kn || !pt) {
            free(ln); free(kn); free(pt);
            return fk_out(out, cap, "FAIL metal_model_f16_matvec allocation=input\n");
        }
        char *c = pt, *e = NULL;
        unsigned long long vi = strtoull(c, &e, 10); if (e == c) goto bad_f16_plan; c = e;
        unsigned long long base = strtoull(c, &e, 10); if (e == c) goto bad_f16_plan; c = e;
        unsigned long rows = strtoul(c, &e, 10); if (e == c) goto bad_f16_plan; c = e;
        unsigned long cols = strtoul(c, &e, 10);
        if (e == c || vi >= fk_model_view_count || rows == 0 || cols == 0 ||
            fk_model_stage == nil || fk_model_stage_count != cols) goto bad_f16_plan;
        if ((size_t)rows > SIZE_MAX / (size_t)cols / 2) goto bad_f16_plan;
        size_t bytes = (size_t)rows * (size_t)cols * 2;
        if (base > fk_model_views[vi].length || bytes > fk_model_views[vi].length - base)
            goto bad_f16_plan;

        id<MTLLibrary> lib = nil;
        for (size_t i = 0; i < fk_model_library_count; i++)
            if (strcmp(fk_model_libraries[i].name, ln) == 0) lib = fk_model_libraries[i].library;
        if (lib == nil) {
            long long n = fk_out(out, cap, "FAIL metal_model_f16_matvec library=%s\n", ln);
            free(ln); free(kn); free(pt); return n;
        }
        size_t page = (size_t)getpagesize();
        size_t ws = (size_t)base / page * page;
        size_t used = (size_t)base - ws + bytes;
        size_t wl = (used + page - 1) / page * page;
        size_t absolute = fk_model_view_offsets[vi] + ws;
        if (absolute > fk_model_mapping_len || wl > fk_model_mapping_len - absolute)
            goto bad_f16_plan;
        id<MTLBuffer> weights = [fk_model_device
            newBufferWithBytesNoCopy:(char *)fk_model_mapping + absolute length:wl
            options:MTLResourceStorageModeShared deallocator:nil];
        NSString *ks = [NSString stringWithUTF8String:kn];
        id<MTLFunction> fn = [lib newFunctionWithName:ks];
        NSError *error = nil;
        id<MTLComputePipelineState> pipe =
            fn ? fk_model_pipeline_for(fn, &error) : nil;
        id<MTLCommandQueue> queue = fk_model_command_queue();
        id<MTLBuffer> result = [fk_model_device newBufferWithLength:rows * sizeof(float)
            options:MTLResourceStorageModeShared];
        if (!weights || !pipe || !queue || !result) {
            const char *m = error.localizedDescription.UTF8String ?: "unknown";
            [weights release]; [fn release]; [pipe release]; [queue release]; [result release];
            free(ln); free(kn); free(pt);
            return fk_out(out, cap, "FAIL metal_model_f16_matvec pipeline=%s\n", m);
        }
        uint32_t r32 = (uint32_t)rows, c32 = (uint32_t)cols;
        double started = fk_metal_now_ms();
        id<MTLCommandBuffer> command = [queue commandBuffer];
        id<MTLComputeCommandEncoder> enc = [command computeCommandEncoder];
        [enc setComputePipelineState:pipe];
        [enc setBuffer:weights offset:(NSUInteger)((size_t)base - ws) atIndex:0];
        [enc setBuffer:fk_model_stage offset:0 atIndex:1];
        [enc setBuffer:result offset:0 atIndex:2];
        [enc setBytes:&r32 length:4 atIndex:3]; [enc setBytes:&c32 length:4 atIndex:4];
        NSUInteger width = MIN(pipe.maxTotalThreadsPerThreadgroup, pipe.threadExecutionWidth * 4);
        [enc dispatchThreads:MTLSizeMake(rows, 1, 1)
            threadsPerThreadgroup:MTLSizeMake(width, 1, 1)];
        [enc endEncoding]; [command commit]; [command waitUntilCompleted];
        double elapsed = fk_metal_now_ms() - started;
        if (command.status != MTLCommandBufferStatusCompleted) {
            const char *m = command.error.localizedDescription.UTF8String ?: "unknown";
            [weights release]; [fn release]; [pipe release]; [queue release]; [result release];
            free(ln); free(kn); free(pt);
            return fk_out(out, cap, "FAIL metal_model_f16_matvec dispatch=%s\n", m);
        }
        float *v = result.contents; double sum = 0, abs_sum = 0; size_t finite = 0;
        for (size_t i = 0; i < rows; i++) {
            if (isfinite(v[i])) finite++; sum += v[i]; abs_sum += fabs((double)v[i]);
        }
        [fk_model_stage release]; fk_model_stage = result; fk_model_stage_count = rows;
        long long n = fk_out(out, cap,
            "PASS metal_model_f16_matvec\nlibrary=%s\nkernel=%s\nview=%llu\n"
            "base-bytes=%llu\ntensor-window-bytes=%zu\nrows=%lu\ncols=%lu\n"
            "finite=%zu\nsum=%.17g\nabs-sum=%.17g\nsample=%.9g,%.9g,%.9g,%.9g\n"
            "elapsed-ms=%.3f\ncarrier=in-process-metal\n"
            "host-exec=0\nshell=0\nswift=0\ntemp=0\nnetwork=0\nremote=0\n"
            "outcome=success\nrewitness-offer=metal-model-f16-matvec\n",
            ln, kn, vi, base, wl, rows, cols, finite, sum, abs_sum,
            v[0], v[rows>1?1:0], v[rows>2?2:0], v[rows>3?3:0], elapsed);
        [weights release]; [fn release]; [pipe release]; [queue release];
        free(ln); free(kn); free(pt); return n;
bad_f16_plan:
        free(ln); free(kn); free(pt);
        return fk_out(out, cap,
            "FAIL metal_model_f16_matvec plan-or-stage stage-values=%zu\n",
            fk_model_stage_count);
    }
}

long long fk_metal_model_hc_split_external(
    const char *library_name, long long library_name_len,
    const char *plan, long long plan_len,
    char *out, long long cap) {
    @autoreleasepool {
        char *ln = fk_copy_cstr(library_name, library_name_len);
        char *pt = fk_copy_cstr(plan, plan_len);
        if (!ln || !pt) { free(ln); free(pt); return fk_out(out, cap, "FAIL metal_model_hc_split allocation\n"); }
        char *c = pt, *e = NULL;
        unsigned long long vi = strtoull(c, &e, 10); if (e == c) goto bad_split; c=e;
        unsigned long long scale_base = strtoull(c, &e, 10); if (e == c) goto bad_split; c=e;
        unsigned long long bias_base = strtoull(c, &e, 10); if (e == c) goto bad_split; c=e;
        unsigned long nhc = strtoul(c, &e, 10); if (e == c) goto bad_split; c=e;
        unsigned long iters = strtoul(c, &e, 10); if (e == c) goto bad_split; c=e;
        float eps = strtof(c, &e);
        size_t split_count = 2 * (size_t)nhc + (size_t)nhc * (size_t)nhc;
        if (e == c || vi >= fk_model_view_count || nhc == 0 || nhc > 8 ||
            iters == 0 || fk_model_stage == nil || fk_model_stage_count != split_count)
            goto bad_split;
        id<MTLLibrary> lib = nil;
        for (size_t i=0;i<fk_model_library_count;i++)
            if (strcmp(fk_model_libraries[i].name,ln)==0) lib=fk_model_libraries[i].library;
        if (!lib) goto bad_split;
        size_t page=(size_t)getpagesize();
        size_t ss=(size_t)scale_base/page*page, bs=(size_t)bias_base/page*page;
        size_t sl=((size_t)scale_base-ss+3*sizeof(float)+page-1)/page*page;
        size_t bl=((size_t)bias_base-bs+split_count*sizeof(float)+page-1)/page*page;
        size_t sa=fk_model_view_offsets[vi]+ss, ba=fk_model_view_offsets[vi]+bs;
        if (sa+sl>fk_model_mapping_len || ba+bl>fk_model_mapping_len) goto bad_split;
        id<MTLBuffer> scale=[fk_model_device newBufferWithBytesNoCopy:(char*)fk_model_mapping+sa
            length:sl options:MTLResourceStorageModeShared deallocator:nil];
        id<MTLBuffer> bias=[fk_model_device newBufferWithBytesNoCopy:(char*)fk_model_mapping+ba
            length:bl options:MTLResourceStorageModeShared deallocator:nil];
        id<MTLFunction> fn=[lib newFunctionWithName:@"form_hc_split_f32"];
        NSError *error=nil;
        id<MTLComputePipelineState> pipe=fn?fk_model_pipeline_for(fn,&error):nil;
        id<MTLCommandQueue> queue=fk_model_command_queue();
        id<MTLBuffer> result=[fk_model_device newBufferWithLength:split_count*sizeof(float)
            options:MTLResourceStorageModeShared];
        if(!scale||!bias||!pipe||!queue||!result){
            const char*m=error.localizedDescription.UTF8String?:"unknown";
            [scale release];[bias release];[fn release];[pipe release];[queue release];[result release];
            free(ln);free(pt);return fk_out(out,cap,"FAIL metal_model_hc_split pipeline=%s\n",m);
        }
        uint32_t n32=(uint32_t)nhc,it32=(uint32_t)iters;
        double started=fk_metal_now_ms();
        id<MTLCommandBuffer> command=[queue commandBuffer];
        id<MTLComputeCommandEncoder> enc=[command computeCommandEncoder];
        [enc setComputePipelineState:pipe];
        [enc setBuffer:fk_model_stage offset:0 atIndex:0];
        [enc setBuffer:scale offset:(NSUInteger)((size_t)scale_base-ss) atIndex:1];
        [enc setBuffer:bias offset:(NSUInteger)((size_t)bias_base-bs) atIndex:2];
        [enc setBuffer:result offset:0 atIndex:3];
        [enc setBytes:&n32 length:4 atIndex:4];[enc setBytes:&it32 length:4 atIndex:5];
        [enc setBytes:&eps length:4 atIndex:6];
        [enc dispatchThreads:MTLSizeMake(1,1,1) threadsPerThreadgroup:MTLSizeMake(1,1,1)];
        [enc endEncoding];[command commit];[command waitUntilCompleted];
        double elapsed=fk_metal_now_ms()-started;
        if(command.status!=MTLCommandBufferStatusCompleted){
            const char*m=command.error.localizedDescription.UTF8String?:"unknown";
            [scale release];[bias release];[fn release];[pipe release];[queue release];[result release];
            free(ln);free(pt);return fk_out(out,cap,"FAIL metal_model_hc_split dispatch=%s\n",m);
        }
        float*v=result.contents;double sum=0,abs_sum=0;size_t finite=0;
        for(size_t i=0;i<split_count;i++){if(isfinite(v[i]))finite++;sum+=v[i];abs_sum+=fabs((double)v[i]);}
        [fk_model_stage release];fk_model_stage=result;fk_model_stage_count=split_count;
        long long n=fk_out(out,cap,
            "PASS metal_model_hc_split\nn-hc=%lu\niterations=%lu\nvalues=%zu\nfinite=%zu\n"
            "pre=%.9g,%.9g,%.9g,%.9g\npost=%.9g,%.9g,%.9g,%.9g\n"
            "sum=%.17g\nabs-sum=%.17g\nelapsed-ms=%.3f\n"
            "carrier=in-process-metal\nhost-exec=0\nshell=0\nswift=0\ntemp=0\nnetwork=0\nremote=0\n"
            "outcome=success\nrewitness-offer=metal-model-hc-split\n",
            nhc,iters,split_count,finite,v[0],v[1],v[2],v[3],
            v[nhc],v[nhc+1],v[nhc+2],v[nhc+3],sum,abs_sum,elapsed);
        [scale release];[bias release];[fn release];[pipe release];[queue release];
        free(ln);free(pt);return n;
bad_split:
        free(ln);free(pt);
        return fk_out(out,cap,"FAIL metal_model_hc_split plan-or-stage stage-values=%zu\n",fk_model_stage_count);
    }
}

long long fk_metal_model_hc_reduce_external(
    const char *library_name, long long library_name_len,
    const char *plan, long long plan_len, char *out, long long cap) {
    @autoreleasepool {
        char *ln=fk_copy_cstr(library_name,library_name_len);
        char *pt=fk_copy_cstr(plan,plan_len);
        if(!ln||!pt){free(ln);free(pt);return fk_out(out,cap,"FAIL metal_model_hc_reduce allocation\n");}
        char*c=pt,*e=NULL;unsigned long nembd=strtoul(c,&e,10);if(e==c)goto bad_reduce;c=e;
        unsigned long nhc=strtoul(c,&e,10);
        size_t split_count=2*(size_t)nhc+(size_t)nhc*(size_t)nhc;
        if(e==c||nembd==0||nhc==0||fk_model_stage_count!=split_count||
            fk_model_residual_count!=(size_t)nembd*(size_t)nhc)goto bad_reduce;
        id<MTLLibrary>lib=nil;for(size_t i=0;i<fk_model_library_count;i++)
            if(strcmp(fk_model_libraries[i].name,ln)==0)lib=fk_model_libraries[i].library;
        id<MTLFunction>fn=[lib newFunctionWithName:@"form_hc_wsum_f32"];NSError*error=nil;
        id<MTLComputePipelineState>pipe=fn?fk_model_pipeline_for(fn,&error):nil;
        id<MTLCommandQueue>queue=fk_model_command_queue();
        id<MTLBuffer>result=[fk_model_device newBufferWithLength:nembd*sizeof(float)
            options:MTLResourceStorageModeShared];
        if(!pipe||!queue||!result){const char*m=error.localizedDescription.UTF8String?:"unknown";
            [fn release];[pipe release];[queue release];[result release];free(ln);free(pt);
            return fk_out(out,cap,"FAIL metal_model_hc_reduce pipeline=%s\n",m);}
        uint32_t e32=(uint32_t)nembd,h32=(uint32_t)nhc;double started=fk_metal_now_ms();
        id<MTLCommandBuffer>command=[queue commandBuffer];id<MTLComputeCommandEncoder>enc=[command computeCommandEncoder];
        [enc setComputePipelineState:pipe];[enc setBuffer:fk_model_residual offset:0 atIndex:0];
        [enc setBuffer:fk_model_stage offset:0 atIndex:1];[enc setBuffer:result offset:0 atIndex:2];
        [enc setBytes:&h32 length:4 atIndex:3];[enc setBytes:&e32 length:4 atIndex:4];
        NSUInteger width=MIN(pipe.maxTotalThreadsPerThreadgroup,pipe.threadExecutionWidth*4);
        [enc dispatchThreads:MTLSizeMake(nembd,1,1) threadsPerThreadgroup:MTLSizeMake(width,1,1)];
        [enc endEncoding];[command commit];[command waitUntilCompleted];double elapsed=fk_metal_now_ms()-started;
        if(command.status!=MTLCommandBufferStatusCompleted){const char*m=command.error.localizedDescription.UTF8String?:"unknown";
            [fn release];[pipe release];[queue release];[result release];free(ln);free(pt);
            return fk_out(out,cap,"FAIL metal_model_hc_reduce dispatch=%s\n",m);}
        float*v=result.contents;double sum=0,abs_sum=0;size_t finite=0;
        for(size_t i=0;i<nembd;i++){if(isfinite(v[i]))finite++;sum+=v[i];abs_sum+=fabs((double)v[i]);}
        [fk_model_stage release];fk_model_stage=result;fk_model_stage_count=nembd;
        long long n=fk_out(out,cap,
            "PASS metal_model_hc_reduce\nn-embd=%lu\nn-hc=%lu\nvalues=%lu\nfinite=%zu\n"
            "sum=%.17g\nabs-sum=%.17g\nsample=%.9g,%.9g,%.9g,%.9g\nelapsed-ms=%.3f\n"
            "carrier=in-process-metal\nhost-exec=0\nshell=0\nswift=0\ntemp=0\nnetwork=0\nremote=0\n"
            "outcome=success\nrewitness-offer=metal-model-hc-reduce\n",
            nembd,nhc,nembd,finite,sum,abs_sum,v[0],v[1],v[2],v[3],elapsed);
        [fn release];[pipe release];[queue release];free(ln);free(pt);return n;
bad_reduce:
        free(ln);free(pt);return fk_out(out,cap,
            "FAIL metal_model_hc_reduce plan-or-stage stage=%zu residual=%zu\n",
            fk_model_stage_count,fk_model_residual_count);
    }
}

long long fk_metal_model_rmsnorm_external(
    const char *library_name,long long library_name_len,
    const char *plan,long long plan_len,char*out,long long cap){
    @autoreleasepool{
        char*ln=fk_copy_cstr(library_name,library_name_len),*pt=fk_copy_cstr(plan,plan_len);
        if(!ln||!pt){free(ln);free(pt);return fk_out(out,cap,"FAIL metal_model_rmsnorm allocation\n");}
        char*c=pt,*e=NULL;unsigned long long vi=strtoull(c,&e,10);if(e==c)goto bad_rms;c=e;
        unsigned long long base=strtoull(c,&e,10);if(e==c)goto bad_rms;c=e;
        unsigned long count=strtoul(c,&e,10);if(e==c)goto bad_rms;c=e;float eps=strtof(c,&e);
        if(e==c)goto bad_rms;c=e;unsigned long group=strtoul(c,&e,10);
        if(e==c)group=0;
        if(vi>=fk_model_view_count||count==0||fk_model_stage_count!=count)goto bad_rms;
        if(group>0&&(count%group)!=0)goto bad_rms;
        size_t bytes=(size_t)count*sizeof(float),page=(size_t)getpagesize();
        if(base>fk_model_views[vi].length||bytes>fk_model_views[vi].length-base)goto bad_rms;
        size_t ws=(size_t)base/page*page,used=(size_t)base-ws+bytes,wl=(used+page-1)/page*page;
        size_t absolute=fk_model_view_offsets[vi]+ws;if(absolute+wl>fk_model_mapping_len)goto bad_rms;
        id<MTLLibrary>lib=nil;for(size_t i=0;i<fk_model_library_count;i++)
            if(strcmp(fk_model_libraries[i].name,ln)==0)lib=fk_model_libraries[i].library;
        id<MTLBuffer>gain=[fk_model_device newBufferWithBytesNoCopy:(char*)fk_model_mapping+absolute
            length:wl options:MTLResourceStorageModeShared deallocator:nil];
        id<MTLFunction>fn=[lib newFunctionWithName:group?
            @"form_dsv4_head_rmsnorm_f32":@"form_dsv4_rmsnorm_f32"];NSError*error=nil;
        id<MTLComputePipelineState>pipe=fn?fk_model_pipeline_for(fn,&error):nil;
        id<MTLCommandQueue>queue=fk_model_command_queue();
        id<MTLBuffer>result=[fk_model_device newBufferWithLength:bytes options:MTLResourceStorageModeShared];
        if(!gain||!pipe||!queue||!result){const char*m=error.localizedDescription.UTF8String?:"unknown";
            [gain release];[fn release];[pipe release];[queue release];[result release];free(ln);free(pt);
            return fk_out(out,cap,"FAIL metal_model_rmsnorm pipeline=%s\n",m);}
        uint32_t n32=(uint32_t)(group?group:count);double started=fk_metal_now_ms();
        id<MTLCommandBuffer>command=[queue commandBuffer];id<MTLComputeCommandEncoder>enc=[command computeCommandEncoder];
        [enc setComputePipelineState:pipe];[enc setBuffer:fk_model_stage offset:0 atIndex:0];
        [enc setBuffer:gain offset:(NSUInteger)((size_t)base-ws) atIndex:1];[enc setBuffer:result offset:0 atIndex:2];
        [enc setBytes:&n32 length:4 atIndex:3];[enc setBytes:&eps length:4 atIndex:4];
        NSUInteger jobs=group?count/group:1;
        [enc dispatchThreads:MTLSizeMake(jobs,1,1) threadsPerThreadgroup:MTLSizeMake(1,1,1)];
        [enc endEncoding];[command commit];[command waitUntilCompleted];double elapsed=fk_metal_now_ms()-started;
        if(command.status!=MTLCommandBufferStatusCompleted){const char*m=command.error.localizedDescription.UTF8String?:"unknown";
            [gain release];[fn release];[pipe release];[queue release];[result release];free(ln);free(pt);
            return fk_out(out,cap,"FAIL metal_model_rmsnorm dispatch=%s\n",m);}
        float*v=result.contents;double sum=0,abs_sum=0;size_t finite=0;
        for(size_t j=0;j<count;j++){if(isfinite(v[j]))finite++;sum+=v[j];abs_sum+=fabs((double)v[j]);}
        [fk_model_stage release];fk_model_stage=result;fk_model_stage_count=count;
        long long n=fk_out(out,cap,
            "PASS metal_model_rmsnorm\nmode=%s\ngroup-width=%lu\ngroups=%lu\nview=%llu\nbase-bytes=%llu\nvalues=%lu\nfinite=%zu\n"
            "sum=%.17g\nabs-sum=%.17g\nsample=%.9g,%.9g,%.9g,%.9g\nelapsed-ms=%.3f\n"
            "carrier=in-process-metal\nhost-exec=0\nshell=0\nswift=0\ntemp=0\nnetwork=0\nremote=0\n"
            "outcome=success\nrewitness-offer=metal-model-rmsnorm\n",
            group?"per-group-unweighted":"whole-weighted",group,jobs,
            vi,base,count,finite,sum,abs_sum,v[0],v[1],v[2],v[3],elapsed);
        [gain release];[fn release];[pipe release];[queue release];free(ln);free(pt);return n;
bad_rms:
        free(ln);free(pt);return fk_out(out,cap,
            "FAIL metal_model_rmsnorm plan-or-stage stage=%zu\n",fk_model_stage_count);
    }}

long long fk_metal_model_mx8_matvec_external(
    const char *library_name,long long library_name_len,
    const char *plan,long long plan_len,char*out,long long cap){
    @autoreleasepool{
        char*ln=fk_copy_cstr(library_name,library_name_len),*pt=fk_copy_cstr(plan,plan_len);
        if(!ln||!pt){free(ln);free(pt);return fk_out(out,cap,"FAIL metal_model_mx8_matvec allocation\n");}
        char*c=pt,*e=NULL;unsigned long long vi=strtoull(c,&e,10);if(e==c)goto bad_mx8;c=e;
        unsigned long long base=strtoull(c,&e,10);if(e==c)goto bad_mx8;c=e;
        unsigned long long bytes=strtoull(c,&e,10);if(e==c)goto bad_mx8;c=e;
        unsigned long rows=strtoul(c,&e,10);if(e==c)goto bad_mx8;c=e;
        unsigned long cols=strtoul(c,&e,10);if(e==c)goto bad_mx8;c=e;
        unsigned long rows_per_group=strtoul(c,&e,10);if(e==c)rows_per_group=0;
        size_t groups=rows_per_group?rows/rows_per_group:1;
        size_t expected_stage=(size_t)cols*groups;
        if(vi>=fk_model_view_count||rows==0||cols==0||cols%32!=0||
            (rows_per_group&&(rows%rows_per_group)!=0)||fk_model_stage_count!=expected_stage||
            base>fk_model_views[vi].length||
            bytes>fk_model_views[vi].length-base)goto bad_mx8;
        size_t nel=(size_t)rows*(size_t)cols,page=(size_t)getpagesize();
        if(nel>UINT32_MAX)goto bad_mx8;
        size_t ws=(size_t)base/page*page,used=(size_t)base-ws+(size_t)bytes,wl=(used+page-1)/page*page;
        size_t absolute=fk_model_view_offsets[vi]+ws;if(absolute+wl>fk_model_mapping_len)goto bad_mx8;
        id<MTLLibrary>lib=nil;for(size_t i=0;i<fk_model_library_count;i++)
            if(strcmp(fk_model_libraries[i].name,ln)==0)lib=fk_model_libraries[i].library;
        id<MTLBuffer>weights=[fk_model_device newBufferWithBytesNoCopy:(char*)fk_model_mapping+absolute
            length:wl options:MTLResourceStorageModeShared deallocator:nil];
        id<MTLFunction>fn=[lib newFunctionWithName:rows_per_group?
            @"form_dsv4_mx8_grouped_matvec":@"form_dsv4_mx8_matvec"];NSError*error=nil;
        id<MTLComputePipelineState>pipe=fn?fk_model_pipeline_for(fn,&error):nil;
        id<MTLCommandQueue>queue=fk_model_command_queue();
        id<MTLBuffer>result=[fk_model_device newBufferWithLength:rows*sizeof(float) options:MTLResourceStorageModeShared];
        if(!weights||!pipe||!queue||!result){const char*m=error.localizedDescription.UTF8String?:"unknown";
            [weights release];[fn release];[pipe release];[queue release];[result release];free(ln);free(pt);
            return fk_out(out,cap,"FAIL metal_model_mx8_matvec pipeline=%s\n",m);}
        uint32_t r32=(uint32_t)rows,c32=(uint32_t)cols,n32=(uint32_t)nel;double started=fk_metal_now_ms();
        id<MTLCommandBuffer>command=[queue commandBuffer];id<MTLComputeCommandEncoder>enc=[command computeCommandEncoder];
        [enc setComputePipelineState:pipe];[enc setBuffer:weights offset:(NSUInteger)((size_t)base-ws) atIndex:0];
        [enc setBuffer:fk_model_stage offset:0 atIndex:1];[enc setBuffer:result offset:0 atIndex:2];
        [enc setBytes:&r32 length:4 atIndex:3];[enc setBytes:&c32 length:4 atIndex:4];[enc setBytes:&n32 length:4 atIndex:5];
        uint32_t g32=(uint32_t)rows_per_group;if(rows_per_group)[enc setBytes:&g32 length:4 atIndex:6];
        [enc dispatchThreads:MTLSizeMake(rows*32,1,1) threadsPerThreadgroup:MTLSizeMake(256,1,1)];
        [enc endEncoding];[command commit];[command waitUntilCompleted];double elapsed=fk_metal_now_ms()-started;
        if(command.status!=MTLCommandBufferStatusCompleted){const char*m=command.error.localizedDescription.UTF8String?:"unknown";
            [weights release];[fn release];[pipe release];[queue release];[result release];free(ln);free(pt);
            return fk_out(out,cap,"FAIL metal_model_mx8_matvec dispatch=%s\n",m);}
        float*v=result.contents;double sum=0,abs_sum=0;size_t finite=0;
        for(size_t j=0;j<rows;j++){if(isfinite(v[j]))finite++;sum+=v[j];abs_sum+=fabs((double)v[j]);}
        [fk_model_stage release];fk_model_stage=result;fk_model_stage_count=rows;
        long long n=fk_out(out,cap,
            "PASS metal_model_mx8_matvec\nview=%llu\nbase-bytes=%llu\ntensor-bytes=%llu\n"
            "mode=%s\ngroups=%zu\nrows-per-group=%lu\nrows=%lu\ncols=%lu\nfinite=%zu\nsum=%.17g\nabs-sum=%.17g\n"
            "sample=%.9g,%.9g,%.9g,%.9g\nelapsed-ms=%.3f\ncarrier=in-process-metal\n"
            "host-exec=0\nshell=0\nswift=0\ntemp=0\nnetwork=0\nremote=0\n"
            "outcome=success\nrewitness-offer=metal-model-mx8-matvec\n",
            vi,base,bytes,rows_per_group?"grouped":"whole",groups,rows_per_group,
            rows,cols,finite,sum,abs_sum,v[0],v[1],v[2],v[3],elapsed);
        [weights release];[fn release];[pipe release];[queue release];free(ln);free(pt);return n;
bad_mx8:
        free(ln);free(pt);return fk_out(out,cap,
            "FAIL metal_model_mx8_matvec plan-or-stage stage=%zu\n",fk_model_stage_count);
    }}

long long fk_metal_model_rope_external(
    const char *library_name,long long library_name_len,
    const char *plan,long long plan_len,char*out,long long cap){
    @autoreleasepool{
        char*ln=fk_copy_cstr(library_name,library_name_len),*pt=fk_copy_cstr(plan,plan_len);
        if(!ln||!pt){free(ln);free(pt);return fk_out(out,cap,"FAIL metal_model_rope allocation\n");}
        char*c=pt,*e=NULL;unsigned long heads=strtoul(c,&e,10);if(e==c)goto bad_rope;c=e;
        unsigned long width=strtoul(c,&e,10);if(e==c)goto bad_rope;c=e;
        unsigned long rotated=strtoul(c,&e,10);if(e==c)goto bad_rope;c=e;
        float position=strtof(c,&e);if(e==c)goto bad_rope;c=e;
        float base=strtof(c,&e);if(e==c)goto bad_rope;c=e;
        float sign=strtof(c,&e);if(e==c)goto bad_rope;c=e;
        unsigned long compressed=strtoul(c,&e,10);
        float scale_factor=1.0f,original_context=65536.0f,beta_fast=32.0f,beta_slow=1.0f;
        if(e!=c){c=e;scale_factor=strtof(c,&e);if(e==c)goto bad_rope;c=e;
            original_context=strtof(c,&e);if(e==c)goto bad_rope;c=e;
            beta_fast=strtof(c,&e);if(e==c)goto bad_rope;c=e;
            beta_slow=strtof(c,&e);if(e==c)goto bad_rope;}
        else compressed=0;
        if(!heads||!width||rotated>width||(rotated&1)||base<=0||
           fk_model_stage_count!=(size_t)heads*width)goto bad_rope;
        id<MTLLibrary>lib=nil;for(size_t i=0;i<fk_model_library_count;i++)
            if(strcmp(fk_model_libraries[i].name,ln)==0)lib=fk_model_libraries[i].library;
        id<MTLFunction>fn=[lib newFunctionWithName:@"form_dsv4_rope_f32"];NSError*error=nil;
        id<MTLComputePipelineState>pipe=fn?fk_model_pipeline_for(fn,&error):nil;
        id<MTLCommandQueue>queue=fk_model_command_queue();
        id<MTLBuffer>result=[fk_model_device newBufferWithLength:fk_model_stage_count*sizeof(float)
            options:MTLResourceStorageModeShared];
        if(!pipe||!queue||!result){const char*m=error.localizedDescription.UTF8String?:"unknown";
            [fn release];[pipe release];[queue release];[result release];free(ln);free(pt);
            return fk_out(out,cap,"FAIL metal_model_rope pipeline=%s\n",m);}
        uint32_t h32=(uint32_t)heads,w32=(uint32_t)width,r32=(uint32_t)rotated;
        double started=fk_metal_now_ms();id<MTLCommandBuffer>command=[queue commandBuffer];
        id<MTLComputeCommandEncoder>enc=[command computeCommandEncoder];
        [enc setComputePipelineState:pipe];[enc setBuffer:fk_model_stage offset:0 atIndex:0];
        [enc setBuffer:result offset:0 atIndex:1];[enc setBytes:&h32 length:4 atIndex:2];
        [enc setBytes:&w32 length:4 atIndex:3];[enc setBytes:&r32 length:4 atIndex:4];
        [enc setBytes:&position length:4 atIndex:5];[enc setBytes:&base length:4 atIndex:6];
        [enc setBytes:&sign length:4 atIndex:7];
        uint32_t compressed32=(uint32_t)compressed;
        [enc setBytes:&compressed32 length:4 atIndex:8];[enc setBytes:&scale_factor length:4 atIndex:9];
        [enc setBytes:&original_context length:4 atIndex:10];[enc setBytes:&beta_fast length:4 atIndex:11];
        [enc setBytes:&beta_slow length:4 atIndex:12];
        [enc dispatchThreads:MTLSizeMake(heads,1,1) threadsPerThreadgroup:MTLSizeMake(1,1,1)];
        [enc endEncoding];[command commit];[command waitUntilCompleted];
        double elapsed=fk_metal_now_ms()-started;
        if(command.status!=MTLCommandBufferStatusCompleted){const char*m=command.error.localizedDescription.UTF8String?:"unknown";
            [fn release];[pipe release];[queue release];[result release];free(ln);free(pt);
            return fk_out(out,cap,"FAIL metal_model_rope dispatch=%s\n",m);}
        float*v=result.contents;double sum=0,abs_sum=0;size_t finite=0;
        for(size_t i=0;i<fk_model_stage_count;i++){if(isfinite(v[i]))finite++;sum+=v[i];abs_sum+=fabs((double)v[i]);}
        [fk_model_stage release];fk_model_stage=result;size_t count=fk_model_stage_count;
        long long n=fk_out(out,cap,
            "PASS metal_model_rope\nheads=%lu\nwidth=%lu\nrotated=%lu\nposition=%.9g\nfrequency-base=%.9g\n"
            "sign=%.9g\ncompressed=%lu\nscale-factor=%.9g\nvalues=%zu\nfinite=%zu\nsum=%.17g\nabs-sum=%.17g\n"
            "sample=%.9g,%.9g,%.9g,%.9g\nelapsed-ms=%.3f\ncarrier=in-process-metal\n"
            "host-exec=0\nshell=0\nswift=0\ntemp=0\nnetwork=0\nremote=0\noutcome=success\n"
            "rewitness-offer=metal-model-rope\n",
            heads,width,rotated,position,base,sign,compressed,scale_factor,count,finite,sum,abs_sum,
            v[0],v[1],v[2],v[3],elapsed);
        [fn release];[pipe release];[queue release];free(ln);free(pt);return n;
bad_rope:
        free(ln);free(pt);return fk_out(out,cap,
            "FAIL metal_model_rope plan-or-stage stage=%zu\n",fk_model_stage_count);
    }}

long long fk_metal_model_attention_external(
    const char *library_name,long long library_name_len,
    const char *plan,long long plan_len,char*out,long long cap){
    @autoreleasepool{
        char*ln=fk_copy_cstr(library_name,library_name_len),*pt=fk_copy_cstr(plan,plan_len);
        if(!ln||!pt){free(ln);free(pt);return fk_out(out,cap,"FAIL metal_model_attention allocation\n");}
        char*c=pt,*e=c;while(*e&&*e!=' ')e++;if(e==c)goto bad_attention;
        char saved=*e;*e=0;char*qname=c;c=e+1;
        unsigned long long vi=strtoull(c,&e,10);if(e==c)goto bad_attention;c=e;
        unsigned long long base=strtoull(c,&e,10);if(e==c)goto bad_attention;c=e;
        unsigned long heads=strtoul(c,&e,10);if(e==c)goto bad_attention;c=e;
        unsigned long width=strtoul(c,&e,10);if(e==c)goto bad_attention;c=e;
        float scale=strtof(c,&e);if(e==c)goto bad_attention;
        id<MTLBuffer>q=nil;size_t qcount=0;
        for(size_t i=0;i<fk_model_state_count;i++)if(strcmp(fk_model_states[i].name,qname)==0){
            q=fk_model_states[i].buffer;qcount=fk_model_states[i].count;break;}
        size_t sink_bytes=heads*sizeof(float),page=(size_t)getpagesize();
        if(!q||qcount!=(size_t)heads*width||fk_model_stage_count!=width||
           vi>=fk_model_view_count||base>fk_model_views[vi].length||
           sink_bytes>fk_model_views[vi].length-base)goto bad_attention;
        size_t ws=(size_t)base/page*page,used=(size_t)base-ws+sink_bytes,wl=(used+page-1)/page*page;
        size_t absolute=fk_model_view_offsets[vi]+ws;if(absolute+wl>fk_model_mapping_len)goto bad_attention;
        id<MTLBuffer>sinks=[fk_model_device newBufferWithBytesNoCopy:(char*)fk_model_mapping+absolute
            length:wl options:MTLResourceStorageModeShared deallocator:nil];
        id<MTLLibrary>lib=nil;for(size_t i=0;i<fk_model_library_count;i++)
            if(strcmp(fk_model_libraries[i].name,ln)==0)lib=fk_model_libraries[i].library;
        id<MTLFunction>fn=[lib newFunctionWithName:@"form_dsv4_attention_one_f32"];NSError*error=nil;
        id<MTLComputePipelineState>pipe=fn?fk_model_pipeline_for(fn,&error):nil;
        id<MTLCommandQueue>queue=fk_model_command_queue();
        id<MTLBuffer>result=[fk_model_device newBufferWithLength:qcount*sizeof(float)
            options:MTLResourceStorageModeShared];
        if(!sinks||!pipe||!queue||!result){const char*m=error.localizedDescription.UTF8String?:"unknown";
            [sinks release];[fn release];[pipe release];[queue release];[result release];free(ln);free(pt);
            return fk_out(out,cap,"FAIL metal_model_attention pipeline=%s\n",m);}
        uint32_t h32=(uint32_t)heads,w32=(uint32_t)width;double started=fk_metal_now_ms();
        id<MTLCommandBuffer>command=[queue commandBuffer];id<MTLComputeCommandEncoder>enc=[command computeCommandEncoder];
        [enc setComputePipelineState:pipe];[enc setBuffer:q offset:0 atIndex:0];
        [enc setBuffer:fk_model_stage offset:0 atIndex:1];
        [enc setBuffer:sinks offset:(NSUInteger)((size_t)base-ws) atIndex:2];
        [enc setBuffer:result offset:0 atIndex:3];[enc setBytes:&h32 length:4 atIndex:4];
        [enc setBytes:&w32 length:4 atIndex:5];[enc setBytes:&scale length:4 atIndex:6];
        [enc dispatchThreads:MTLSizeMake(heads,1,1) threadsPerThreadgroup:MTLSizeMake(1,1,1)];
        [enc endEncoding];[command commit];[command waitUntilCompleted];double elapsed=fk_metal_now_ms()-started;
        if(command.status!=MTLCommandBufferStatusCompleted){const char*m=command.error.localizedDescription.UTF8String?:"unknown";
            [sinks release];[fn release];[pipe release];[queue release];[result release];free(ln);free(pt);
            return fk_out(out,cap,"FAIL metal_model_attention dispatch=%s\n",m);}
        float*v=result.contents,*sv=(float*)((char*)sinks.contents+((size_t)base-ws));
        double sum=0,abs_sum=0,sink_sum=0;size_t finite=0;
        for(size_t i=0;i<qcount;i++){if(isfinite(v[i]))finite++;sum+=v[i];abs_sum+=fabs((double)v[i]);}
        for(size_t i=0;i<heads;i++)sink_sum+=sv[i];
        [fk_model_stage release];fk_model_stage=[result retain];fk_model_stage_count=qcount;
        long long n=fk_out(out,cap,
            "PASS metal_model_attention\nq-state=%s\nsink-view=%llu\nsink-base-bytes=%llu\n"
            "heads=%lu\nwidth=%lu\nrows=1\nscale=%.9g\nsink-sum=%.17g\nvalues=%zu\nfinite=%zu\n"
            "sum=%.17g\nabs-sum=%.17g\nsample=%.9g,%.9g,%.9g,%.9g\nelapsed-ms=%.3f\n"
            "carrier=in-process-metal\nhost-exec=0\nshell=0\nswift=0\ntemp=0\nnetwork=0\nremote=0\n"
            "outcome=success\nrewitness-offer=metal-model-attention\n",
            qname,vi,base,heads,width,scale,sink_sum,qcount,finite,sum,abs_sum,
            v[0],v[1],v[2],v[3],elapsed);
        (void)saved;[sinks release];[fn release];[pipe release];[queue release];free(ln);free(pt);return n;
bad_attention:
        free(ln);free(pt);return fk_out(out,cap,
            "FAIL metal_model_attention plan-or-state stage=%zu\n",fk_model_stage_count);
    }}

long long fk_metal_model_hc_post_external(
    const char *library_name,long long library_name_len,
    const char *plan,long long plan_len,char*out,long long cap){
    @autoreleasepool{
        char*ln=fk_copy_cstr(library_name,library_name_len),*pt=fk_copy_cstr(plan,plan_len);
        if(!ln||!pt){free(ln);free(pt);return fk_out(out,cap,"FAIL metal_model_hc_post allocation\n");}
        char*c=pt,*e=c;while(*e&&*e!=' ')e++;if(e==c||!*e)goto bad_hc_post;*e=0;char*split_name=c;c=e+1;
        unsigned long nembd=strtoul(c,&e,10);if(e==c)goto bad_hc_post;c=e;
        unsigned long nhc=strtoul(c,&e,10);if(e==c)goto bad_hc_post;
        id<MTLBuffer>split=nil;size_t split_count=0;
        for(size_t i=0;i<fk_model_state_count;i++)if(strcmp(fk_model_states[i].name,split_name)==0){
            split=fk_model_states[i].buffer;split_count=fk_model_states[i].count;break;}
        if(!split||split_count!=(size_t)(2*nhc+nhc*nhc)||fk_model_stage_count!=nembd||
           fk_model_residual_count!=(size_t)nembd*nhc)goto bad_hc_post;
        id<MTLLibrary>lib=nil;for(size_t i=0;i<fk_model_library_count;i++)
            if(strcmp(fk_model_libraries[i].name,ln)==0)lib=fk_model_libraries[i].library;
        id<MTLFunction>fn=[lib newFunctionWithName:@"form_hc_post_f32"];NSError*error=nil;
        id<MTLComputePipelineState>pipe=fn?fk_model_pipeline_for(fn,&error):nil;
        id<MTLCommandQueue>queue=fk_model_command_queue();
        id<MTLBuffer>result=[fk_model_device newBufferWithLength:(size_t)nembd*nhc*sizeof(float)
            options:MTLResourceStorageModeShared];
        if(!pipe||!queue||!result){const char*m=error.localizedDescription.UTF8String?:"unknown";
            [fn release];[pipe release];[queue release];[result release];free(ln);free(pt);
            return fk_out(out,cap,"FAIL metal_model_hc_post pipeline=%s\n",m);}
        uint32_t h32=(uint32_t)nhc,e32=(uint32_t)nembd;double started=fk_metal_now_ms();
        id<MTLCommandBuffer>command=[queue commandBuffer];id<MTLComputeCommandEncoder>enc=[command computeCommandEncoder];
        [enc setComputePipelineState:pipe];[enc setBuffer:fk_model_stage offset:0 atIndex:0];
        [enc setBuffer:fk_model_residual offset:0 atIndex:1];
        [enc setBuffer:split offset:nhc*sizeof(float) atIndex:2];
        [enc setBuffer:split offset:2*nhc*sizeof(float) atIndex:3];
        [enc setBuffer:result offset:0 atIndex:4];[enc setBytes:&h32 length:4 atIndex:5];
        [enc setBytes:&e32 length:4 atIndex:6];
        [enc dispatchThreads:MTLSizeMake(nhc,1,1) threadsPerThreadgroup:MTLSizeMake(1,1,1)];
        [enc endEncoding];[command commit];[command waitUntilCompleted];double elapsed=fk_metal_now_ms()-started;
        if(command.status!=MTLCommandBufferStatusCompleted){const char*m=command.error.localizedDescription.UTF8String?:"unknown";
            [fn release];[pipe release];[queue release];[result release];free(ln);free(pt);
            return fk_out(out,cap,"FAIL metal_model_hc_post dispatch=%s\n",m);}
        float*v=result.contents;size_t count=(size_t)nembd*nhc,finite=0;double sum=0,abs_sum=0;
        for(size_t i=0;i<count;i++){if(isfinite(v[i]))finite++;sum+=v[i];abs_sum+=fabs((double)v[i]);}
        [fk_model_stage release];fk_model_stage=result;fk_model_stage_count=count;
        [fk_model_residual release];fk_model_residual=[result retain];fk_model_residual_count=count;
        long long n=fk_out(out,cap,
            "PASS metal_model_hc_post\nsplit-state=%s\nn-embd=%lu\nn-hc=%lu\nvalues=%zu\nfinite=%zu\n"
            "sum=%.17g\nabs-sum=%.17g\nsample=%.9g,%.9g,%.9g,%.9g\nelapsed-ms=%.3f\n"
            "carrier=in-process-metal\nhost-exec=0\nshell=0\nswift=0\ntemp=0\nnetwork=0\nremote=0\n"
            "outcome=success\nrewitness-offer=metal-model-hc-post\n",
            split_name,nembd,nhc,count,finite,sum,abs_sum,v[0],v[1],v[2],v[3],elapsed);
        [fn release];[pipe release];[queue release];free(ln);free(pt);return n;
bad_hc_post:
        free(ln);free(pt);return fk_out(out,cap,
            "FAIL metal_model_hc_post plan-or-state stage=%zu residual=%zu\n",
            fk_model_stage_count,fk_model_residual_count);
    }}

long long fk_metal_model_hash_route_external(
    const char *library_name,long long library_name_len,
    const char *plan,long long plan_len,char*out,long long cap){
    @autoreleasepool{
        char*ln=fk_copy_cstr(library_name,library_name_len),*pt=fk_copy_cstr(plan,plan_len);
        if(!ln||!pt){free(ln);free(pt);return fk_out(out,cap,"FAIL metal_model_hash_route allocation\n");}
        char*c=pt,*e=NULL;unsigned long long vi=strtoull(c,&e,10);if(e==c)goto bad_route;c=e;
        unsigned long long base=strtoull(c,&e,10);if(e==c)goto bad_route;c=e;
        unsigned long token=strtoul(c,&e,10);if(e==c)goto bad_route;c=e;
        unsigned long selected=strtoul(c,&e,10);if(e==c)goto bad_route;c=e;
        float weight_scale=strtof(c,&e);if(e==c)goto bad_route;c=e;
        float floor_sum=strtof(c,&e);if(e==c)goto bad_route;
        size_t row_bytes=selected*sizeof(int32_t),row_off=(size_t)token*row_bytes,page=(size_t)getpagesize();
        if(vi>=fk_model_view_count||selected==0||selected>16||fk_model_stage_count!=256||
           base+row_off>fk_model_views[vi].length||
           row_bytes>fk_model_views[vi].length-base-row_off)goto bad_route;
        size_t address=(size_t)base+row_off,ws=address/page*page,used=address-ws+row_bytes,wl=(used+page-1)/page*page;
        size_t absolute=fk_model_view_offsets[vi]+ws;if(absolute+wl>fk_model_mapping_len)goto bad_route;
        id<MTLBuffer>table=[fk_model_device newBufferWithBytesNoCopy:(char*)fk_model_mapping+absolute
            length:wl options:MTLResourceStorageModeShared deallocator:nil];
        id<MTLLibrary>lib=nil;for(size_t i=0;i<fk_model_library_count;i++)
            if(strcmp(fk_model_libraries[i].name,ln)==0)lib=fk_model_libraries[i].library;
        id<MTLFunction>fn=[lib newFunctionWithName:@"form_dsv4_hash_route"];NSError*error=nil;
        id<MTLComputePipelineState>pipe=fn?fk_model_pipeline_for(fn,&error):nil;
        id<MTLCommandQueue>queue=fk_model_command_queue();
        id<MTLBuffer>ids=[fk_model_device newBufferWithLength:selected*sizeof(uint32_t) options:MTLResourceStorageModeShared];
        id<MTLBuffer>weights=[fk_model_device newBufferWithLength:selected*sizeof(float) options:MTLResourceStorageModeShared];
        if(!table||!pipe||!queue||!ids||!weights){const char*m=error.localizedDescription.UTF8String?:"unknown";
            [table release];[fn release];[pipe release];[queue release];[ids release];[weights release];free(ln);free(pt);
            return fk_out(out,cap,"FAIL metal_model_hash_route pipeline=%s\n",m);}
        uint32_t s32=(uint32_t)selected;double started=fk_metal_now_ms();id<MTLCommandBuffer>command=[queue commandBuffer];
        id<MTLComputeCommandEncoder>enc=[command computeCommandEncoder];[enc setComputePipelineState:pipe];
        [enc setBuffer:fk_model_stage offset:0 atIndex:0];[enc setBuffer:table offset:address-ws atIndex:1];
        [enc setBuffer:ids offset:0 atIndex:2];[enc setBuffer:weights offset:0 atIndex:3];
        [enc setBytes:&s32 length:4 atIndex:4];[enc setBytes:&weight_scale length:4 atIndex:5];
        [enc setBytes:&floor_sum length:4 atIndex:6];
        [enc dispatchThreads:MTLSizeMake(1,1,1) threadsPerThreadgroup:MTLSizeMake(1,1,1)];
        [enc endEncoding];[command commit];[command waitUntilCompleted];double elapsed=fk_metal_now_ms()-started;
        if(command.status!=MTLCommandBufferStatusCompleted){const char*m=command.error.localizedDescription.UTF8String?:"unknown";
            [table release];[fn release];[pipe release];[queue release];[ids release];[weights release];free(ln);free(pt);
            return fk_out(out,cap,"FAIL metal_model_hash_route dispatch=%s\n",m);}
        memcpy(fk_model_route_ids,ids.contents,selected*sizeof(uint32_t));
        memcpy(fk_model_route_weights,weights.contents,selected*sizeof(float));fk_model_route_count=selected;
        long long n=fk_out(out,cap,
            "PASS metal_model_hash_route\ntoken-id=%lu\ntable-view=%llu\ntable-base-bytes=%llu\nselected=%lu\n"
            "ids=",token,vi,base,selected);
        for(size_t i=0;i<selected&&n<cap-1;i++)n+=fk_out(out+n,cap-n,i?",%u":"%u",fk_model_route_ids[i]);
        n+=fk_out(out+n,cap-n,"\nweights=");
        for(size_t i=0;i<selected&&n<cap-1;i++)n+=fk_out(out+n,cap-n,i?",%.9g":"%.9g",fk_model_route_weights[i]);
        n+=fk_out(out+n,cap-n,
            "\nelapsed-ms=%.3f\ncarrier=in-process-metal\nhost-exec=0\nshell=0\nswift=0\ntemp=0\n"
            "network=0\nremote=0\noutcome=success\nrewitness-offer=metal-model-hash-route\n",elapsed);
        [table release];[fn release];[pipe release];[queue release];[ids release];[weights release];free(ln);free(pt);return n;
bad_route:
        free(ln);free(pt);return fk_out(out,cap,
            "FAIL metal_model_hash_route plan-or-stage stage=%zu\n",fk_model_stage_count);
    }}

long long fk_metal_model_topk_route_external(
    const char *library_name,long long library_name_len,
    const char *plan,long long plan_len,char*out,long long cap){
    @autoreleasepool{
        char*ln=fk_copy_cstr(library_name,library_name_len),*pt=fk_copy_cstr(plan,plan_len);
        if(!ln||!pt){free(ln);free(pt);return fk_out(out,cap,"FAIL metal_model_topk_route allocation\n");}
        char*c=pt,*e=NULL;unsigned long long vi=strtoull(c,&e,10);if(e==c)goto bad_topk;c=e;
        unsigned long long base=strtoull(c,&e,10);if(e==c)goto bad_topk;c=e;
        unsigned long router_count=strtoul(c,&e,10);if(e==c)goto bad_topk;c=e;
        unsigned long selected=strtoul(c,&e,10);if(e==c)goto bad_topk;c=e;
        float weight_scale=strtof(c,&e);if(e==c)goto bad_topk;c=e;
        unsigned long expert_count=strtoul(c,&e,10);if(e==c)goto bad_topk;
        size_t bias_bytes=(size_t)router_count*sizeof(float);
        if(vi>=fk_model_view_count||router_count>256||!router_count||!selected||
           selected>8||selected>router_count||expert_count>router_count||
           fk_model_stage_count!=router_count||base>fk_model_views[vi].length||
           bias_bytes>fk_model_views[vi].length-base)goto bad_topk;
        id<MTLLibrary>lib=fk_model_library_named(ln);
        id<MTLFunction>fn=[lib newFunctionWithName:@"form_dsv4_topk_weights"];NSError*error=nil;
        id<MTLComputePipelineState>pipe=fn?fk_model_pipeline_for(fn,&error):nil;
        id<MTLCommandQueue>queue=fk_model_command_queue();
        id<MTLBuffer>ids=[fk_model_device newBufferWithLength:selected*sizeof(uint32_t)
            options:MTLResourceStorageModeShared];
        id<MTLBuffer>weights=[fk_model_device newBufferWithLength:selected*sizeof(float)
            options:MTLResourceStorageModeShared];
        id<MTLBuffer>probs=[fk_model_device newBufferWithLength:router_count*sizeof(float)
            options:MTLResourceStorageModePrivate];
        if(!pipe||!queue||!ids||!weights||!probs){const char*m=error.localizedDescription.UTF8String?:"unknown";
            [fn release];[pipe release];[queue release];[ids release];[weights release];[probs release];
            free(ln);free(pt);return fk_out(out,cap,"FAIL metal_model_topk_route pipeline=%s\n",m);}
        uint32_t r32=(uint32_t)router_count,s32=(uint32_t)selected;
        double started=fk_metal_now_ms();id<MTLCommandBuffer>command=[queue commandBuffer];
        id<MTLComputeCommandEncoder>enc=[command computeCommandEncoder];[enc setComputePipelineState:pipe];
        [enc setBuffer:fk_model_stage offset:0 atIndex:0];
        [enc setBuffer:fk_model_views[vi] offset:(NSUInteger)base atIndex:1];
        [enc setBuffer:ids offset:0 atIndex:2];[enc setBuffer:weights offset:0 atIndex:3];
        [enc setBuffer:probs offset:0 atIndex:4];[enc setBytes:&r32 length:4 atIndex:5];
        [enc setBytes:&s32 length:4 atIndex:6];[enc setBytes:&weight_scale length:4 atIndex:7];
        [enc dispatchThreads:MTLSizeMake(1,1,1) threadsPerThreadgroup:MTLSizeMake(1,1,1)];
        [enc endEncoding];[command commit];[command waitUntilCompleted];double elapsed=fk_metal_now_ms()-started;
        if(command.status!=MTLCommandBufferStatusCompleted){const char*m=command.error.localizedDescription.UTF8String?:"unknown";
            [fn release];[pipe release];[queue release];[ids release];[weights release];[probs release];
            free(ln);free(pt);return fk_out(out,cap,"FAIL metal_model_topk_route dispatch=%s\n",m);}
        uint32_t*idv=ids.contents;for(size_t i=0;i<selected;i++)if(idv[i]>=expert_count){
            [fn release];[pipe release];[queue release];[ids release];[weights release];[probs release];
            free(ln);free(pt);return fk_out(out,cap,
                "FAIL metal_model_topk_route selected-pruned-expert id=%u expert-count=%lu\n",
                idv[i],expert_count);}
        memcpy(fk_model_route_ids,ids.contents,selected*sizeof(uint32_t));
        memcpy(fk_model_route_weights,weights.contents,selected*sizeof(float));fk_model_route_count=selected;
        long long n=fk_out(out,cap,
            "PASS metal_model_topk_route\nbias-view=%llu\nbias-base-bytes=%llu\n"
            "router-count=%lu\nexpert-count=%lu\nselected=%lu\nids=",
            vi,base,router_count,expert_count,selected);
        for(size_t i=0;i<selected&&n<cap-1;i++)n+=fk_out(out+n,cap-n,i?",%u":"%u",fk_model_route_ids[i]);
        n+=fk_out(out+n,cap-n,"\nweights=");
        for(size_t i=0;i<selected&&n<cap-1;i++)n+=fk_out(out+n,cap-n,i?",%.9g":"%.9g",fk_model_route_weights[i]);
        n+=fk_out(out+n,cap-n,
            "\nelapsed-ms=%.3f\ncarrier=in-process-metal\nselection=biased-probability\n"
            "weighting=unbiased-probability\nhost-exec=0\nshell=0\nswift=0\ntemp=0\nnetwork=0\nremote=0\n"
            "outcome=success\nrewitness-offer=metal-model-topk-route\n",elapsed);
        [fn release];[pipe release];[queue release];[ids release];[weights release];[probs release];
        free(ln);free(pt);return n;
bad_topk:
        free(ln);free(pt);return fk_out(out,cap,
            "FAIL metal_model_topk_route plan-or-stage stage=%zu\n",fk_model_stage_count);
    }}

long long fk_metal_model_moe_external(
    const char *library_names,long long library_names_len,
    const char *plan,long long plan_len,char*out,long long cap){
    @autoreleasepool{
        char*names=fk_copy_cstr(library_names,library_names_len),*pt=fk_copy_cstr(plan,plan_len);
        if(!names||!pt){free(names);free(pt);return fk_out(out,cap,"FAIL metal_model_moe allocation\n");}
        char mx4_name[128],iq2_name[128],fold_name[128],mx8_name[128];
        if(sscanf(names,"%127s %127s %127s %127s",mx4_name,iq2_name,fold_name,mx8_name)!=4)
            goto bad_moe;
        char*c=pt,*e=NULL;
#define FK_MOE_U64(name) unsigned long long name=strtoull(c,&e,10);if(e==c)goto bad_moe;c=e
        FK_MOE_U64(vi);FK_MOE_U64(gate_base);FK_MOE_U64(gate_stride);
        FK_MOE_U64(up_base);FK_MOE_U64(up_stride);FK_MOE_U64(down_base);
        FK_MOE_U64(down_stride);FK_MOE_U64(shared_gate_base);FK_MOE_U64(shared_gate_bytes);
        FK_MOE_U64(shared_up_base);FK_MOE_U64(shared_up_bytes);
        FK_MOE_U64(shared_down_base);FK_MOE_U64(shared_down_bytes);
        unsigned long nembd=strtoul(c,&e,10);if(e==c)goto bad_moe;c=e;
        unsigned long nff=strtoul(c,&e,10);if(e==c)goto bad_moe;c=e;
        float clamp=strtof(c,&e);if(e==c)goto bad_moe;c=e;
        unsigned long gate_type=strtoul(c,&e,10);if(e==c)goto bad_moe;c=e;
        unsigned long down_type=strtoul(c,&e,10);if(e==c)goto bad_moe;c=e;
        unsigned long expert_count=strtoul(c,&e,10);if(e==c)goto bad_moe;
#undef FK_MOE_U64
        if(vi>=fk_model_view_count||!nembd||!nff||nembd%32||nff%32||
           (gate_type!=16&&gate_type!=40)||(down_type!=16&&down_type!=40)||
           !expert_count||expert_count>256||fk_model_stage_count!=nembd||
           !fk_model_route_count||fk_model_route_count>16)
            goto bad_moe;
        unsigned long long gate_nel=(unsigned long long)nembd*nff;
        unsigned long long down_nel=(unsigned long long)nff*nembd;
        unsigned long long gate_end=gate_base+gate_stride*expert_count;
        unsigned long long up_end=up_base+up_stride*expert_count;
        unsigned long long down_end=down_base+down_stride*expert_count;
        unsigned long long sg_end=shared_gate_base+shared_gate_bytes;
        unsigned long long su_end=shared_up_base+shared_up_bytes;
        unsigned long long sd_end=shared_down_base+shared_down_bytes;
        if(gate_nel>UINT32_MAX||down_nel>UINT32_MAX||
           gate_end>fk_model_views[vi].length||up_end>fk_model_views[vi].length||
           down_end>fk_model_views[vi].length||sg_end>fk_model_views[vi].length||
           su_end>fk_model_views[vi].length||sd_end>fk_model_views[vi].length)
            goto bad_moe;
        for(size_t i=0;i<fk_model_route_count;i++)
            if(fk_model_route_ids[i]>=expert_count)goto bad_moe;

        id<MTLLibrary>mx4_lib=fk_model_library_named(mx4_name);
        id<MTLLibrary>iq2_lib=fk_model_library_named(iq2_name);
        id<MTLLibrary>fold_lib=fk_model_library_named(fold_name);
        id<MTLLibrary>mx8_lib=fk_model_library_named(mx8_name);
        NSError*error=nil;
#define FK_MOE_PIPE(var,lib,name) \
        id<MTLFunction> var##_fn=[lib newFunctionWithName:@name]; \
        id<MTLComputePipelineState> var=var##_fn?fk_model_pipeline_for(var##_fn,&error):nil
        FK_MOE_PIPE(p_mx4,mx4_lib,"form_dsv4_mx4_matvec");
        FK_MOE_PIPE(p_iq2,iq2_lib,"form_dsv4_iq2_matvec");
        FK_MOE_PIPE(p_swiglu,fold_lib,"form_dsv4_swiglu_f32");
        FK_MOE_PIPE(p_scale,fold_lib,"form_dsv4_scale_f32");
        FK_MOE_PIPE(p_axpy,fold_lib,"form_dsv4_axpy_f32");
        FK_MOE_PIPE(p_mx8,mx8_lib,"form_dsv4_mx8_matvec");
#undef FK_MOE_PIPE
        id<MTLCommandQueue>queue=fk_model_command_queue();
        id<MTLBuffer>moe=[fk_model_device newBufferWithLength:nembd*sizeof(float)
            options:MTLResourceStorageModeShared];
        id<MTLBuffer>gate[16]={nil},up[16]={nil},mid[16]={nil},down[16]={nil};
        id<MTLBuffer>sg=nil,su=nil,smid=nil,shared=nil;
        long long result_n=0;
        if(!p_mx4||!p_iq2||!p_swiglu||!p_scale||!p_axpy||!p_mx8||!queue||!moe){
            const char*m=error.localizedDescription.UTF8String?:"unknown";
            result_n=fk_out(out,cap,"FAIL metal_model_moe pipeline=%s\n",m);
            goto cleanup_moe_return;
        }
        id<MTLCommandBuffer>command=[queue commandBuffer];
        uint32_t gate_rows=(uint32_t)nff,gate_cols=(uint32_t)nembd,gate_n32=(uint32_t)gate_nel;
        uint32_t down_rows=(uint32_t)nembd,down_cols=(uint32_t)nff;
        uint32_t embd32=(uint32_t)nembd,ff32=(uint32_t)nff;
        NSUInteger gate_threads=gate_type==16?((nff+3)/4)*32:nff*32;
        float one=1.0f;
        double started=fk_metal_now_ms();
        for(size_t i=0;i<fk_model_route_count;i++){
            uint32_t expert=fk_model_route_ids[i];
            gate[i]=[fk_model_device newBufferWithLength:nff*sizeof(float) options:MTLResourceStorageModePrivate];
            up[i]=[fk_model_device newBufferWithLength:nff*sizeof(float) options:MTLResourceStorageModePrivate];
            mid[i]=[fk_model_device newBufferWithLength:nff*sizeof(float) options:MTLResourceStorageModePrivate];
            down[i]=[fk_model_device newBufferWithLength:nembd*sizeof(float) options:MTLResourceStorageModePrivate];
            if(!gate[i]||!up[i]||!mid[i]||!down[i]){
                result_n=fk_out(out,cap,"FAIL metal_model_moe expert-buffer index=%zu\n",i);
                goto cleanup_moe_return;
            }
            id<MTLComputeCommandEncoder>enc=[command computeCommandEncoder];
            [enc setComputePipelineState:gate_type==40?p_mx4:p_iq2];
            [enc setBuffer:fk_model_views[vi] offset:(NSUInteger)(gate_base+gate_stride*expert) atIndex:0];
            [enc setBuffer:fk_model_stage offset:0 atIndex:1];[enc setBuffer:gate[i] offset:0 atIndex:2];
            [enc setBytes:&gate_rows length:4 atIndex:3];[enc setBytes:&gate_cols length:4 atIndex:4];
            if(gate_type==40)[enc setBytes:&gate_n32 length:4 atIndex:5];
            [enc dispatchThreads:MTLSizeMake(gate_threads,1,1) threadsPerThreadgroup:MTLSizeMake(256,1,1)];[enc endEncoding];
            enc=[command computeCommandEncoder];[enc setComputePipelineState:gate_type==40?p_mx4:p_iq2];
            [enc setBuffer:fk_model_views[vi] offset:(NSUInteger)(up_base+up_stride*expert) atIndex:0];
            [enc setBuffer:fk_model_stage offset:0 atIndex:1];[enc setBuffer:up[i] offset:0 atIndex:2];
            [enc setBytes:&gate_rows length:4 atIndex:3];[enc setBytes:&gate_cols length:4 atIndex:4];
            if(gate_type==40)[enc setBytes:&gate_n32 length:4 atIndex:5];
            [enc dispatchThreads:MTLSizeMake(gate_threads,1,1) threadsPerThreadgroup:MTLSizeMake(256,1,1)];[enc endEncoding];
            enc=[command computeCommandEncoder];[enc setComputePipelineState:p_swiglu];
            [enc setBuffer:gate[i] offset:0 atIndex:0];[enc setBuffer:up[i] offset:0 atIndex:1];
            [enc setBuffer:mid[i] offset:0 atIndex:2];[enc setBytes:&ff32 length:4 atIndex:3];
            [enc setBytes:&fk_model_route_weights[i] length:4 atIndex:4];[enc setBytes:&clamp length:4 atIndex:5];
            [enc dispatchThreads:MTLSizeMake(nff,1,1) threadsPerThreadgroup:MTLSizeMake(256,1,1)];[enc endEncoding];
            enc=[command computeCommandEncoder];[enc setComputePipelineState:down_type==40?p_mx4:p_iq2];
            [enc setBuffer:fk_model_views[vi] offset:(NSUInteger)(down_base+down_stride*expert) atIndex:0];
            [enc setBuffer:mid[i] offset:0 atIndex:1];[enc setBuffer:down[i] offset:0 atIndex:2];
            [enc setBytes:&down_rows length:4 atIndex:3];[enc setBytes:&down_cols length:4 atIndex:4];
            uint32_t down_expert_n32=(uint32_t)down_nel;
            if(down_type==40)[enc setBytes:&down_expert_n32 length:4 atIndex:5];
            NSUInteger down_threads=down_type==16?((nembd+3)/4)*32:nembd*32;
            [enc dispatchThreads:MTLSizeMake(down_threads,1,1) threadsPerThreadgroup:MTLSizeMake(256,1,1)];[enc endEncoding];
            enc=[command computeCommandEncoder];[enc setComputePipelineState:i?p_axpy:p_scale];
            [enc setBuffer:down[i] offset:0 atIndex:0];[enc setBuffer:moe offset:0 atIndex:1];
            [enc setBytes:&one length:4 atIndex:2];[enc setBytes:&embd32 length:4 atIndex:3];
            [enc dispatchThreads:MTLSizeMake(nembd,1,1) threadsPerThreadgroup:MTLSizeMake(256,1,1)];[enc endEncoding];
        }
        sg=[fk_model_device newBufferWithLength:nff*sizeof(float) options:MTLResourceStorageModePrivate];
        su=[fk_model_device newBufferWithLength:nff*sizeof(float) options:MTLResourceStorageModePrivate];
        smid=[fk_model_device newBufferWithLength:nff*sizeof(float) options:MTLResourceStorageModePrivate];
        shared=[fk_model_device newBufferWithLength:nembd*sizeof(float) options:MTLResourceStorageModePrivate];
        if(!sg||!su||!smid||!shared){
            result_n=fk_out(out,cap,"FAIL metal_model_moe shared-buffer\n");goto cleanup_moe_return;
        }
        id<MTLComputeCommandEncoder>enc=[command computeCommandEncoder];
        [enc setComputePipelineState:p_mx8];[enc setBuffer:fk_model_views[vi] offset:(NSUInteger)shared_gate_base atIndex:0];
        [enc setBuffer:fk_model_stage offset:0 atIndex:1];[enc setBuffer:sg offset:0 atIndex:2];
        [enc setBytes:&gate_rows length:4 atIndex:3];[enc setBytes:&gate_cols length:4 atIndex:4];
        [enc setBytes:&gate_n32 length:4 atIndex:5];
        [enc dispatchThreads:MTLSizeMake(nff*32,1,1) threadsPerThreadgroup:MTLSizeMake(256,1,1)];[enc endEncoding];
        enc=[command computeCommandEncoder];[enc setComputePipelineState:p_mx8];
        [enc setBuffer:fk_model_views[vi] offset:(NSUInteger)shared_up_base atIndex:0];
        [enc setBuffer:fk_model_stage offset:0 atIndex:1];[enc setBuffer:su offset:0 atIndex:2];
        [enc setBytes:&gate_rows length:4 atIndex:3];[enc setBytes:&gate_cols length:4 atIndex:4];
        [enc setBytes:&gate_n32 length:4 atIndex:5];
        [enc dispatchThreads:MTLSizeMake(nff*32,1,1) threadsPerThreadgroup:MTLSizeMake(256,1,1)];[enc endEncoding];
        enc=[command computeCommandEncoder];[enc setComputePipelineState:p_swiglu];
        [enc setBuffer:sg offset:0 atIndex:0];[enc setBuffer:su offset:0 atIndex:1];[enc setBuffer:smid offset:0 atIndex:2];
        [enc setBytes:&ff32 length:4 atIndex:3];[enc setBytes:&one length:4 atIndex:4];[enc setBytes:&clamp length:4 atIndex:5];
        [enc dispatchThreads:MTLSizeMake(nff,1,1) threadsPerThreadgroup:MTLSizeMake(256,1,1)];[enc endEncoding];
        enc=[command computeCommandEncoder];[enc setComputePipelineState:p_mx8];
        [enc setBuffer:fk_model_views[vi] offset:(NSUInteger)shared_down_base atIndex:0];
        [enc setBuffer:smid offset:0 atIndex:1];[enc setBuffer:shared offset:0 atIndex:2];
        [enc setBytes:&down_rows length:4 atIndex:3];[enc setBytes:&down_cols length:4 atIndex:4];
        uint32_t down_n32=(uint32_t)down_nel;[enc setBytes:&down_n32 length:4 atIndex:5];
        [enc dispatchThreads:MTLSizeMake(nembd*32,1,1) threadsPerThreadgroup:MTLSizeMake(256,1,1)];[enc endEncoding];
        enc=[command computeCommandEncoder];[enc setComputePipelineState:p_axpy];
        [enc setBuffer:shared offset:0 atIndex:0];[enc setBuffer:moe offset:0 atIndex:1];
        [enc setBytes:&one length:4 atIndex:2];[enc setBytes:&embd32 length:4 atIndex:3];
        [enc dispatchThreads:MTLSizeMake(nembd,1,1) threadsPerThreadgroup:MTLSizeMake(256,1,1)];[enc endEncoding];
        [command commit];
        double wait_elapsed=0;
        int wait_result=fk_metal_wait_observed(
            command,"metal-model-moe",10000.0,&wait_elapsed);
        double elapsed=fk_metal_now_ms()-started;
        if(wait_result<0||command.status!=MTLCommandBufferStatusCompleted){
            const char*m=command.error.localizedDescription.UTF8String?:"unknown";
            result_n=fk_out(out,cap,"FAIL metal_model_moe dispatch=%s\n",m);goto cleanup_moe_return;
        }
        float*v=moe.contents;double sum=0,abs_sum=0;size_t finite=0;
        for(size_t i=0;i<nembd;i++){if(isfinite(v[i]))finite++;sum+=v[i];abs_sum+=fabs((double)v[i]);}
        [fk_model_stage release];fk_model_stage=[moe retain];fk_model_stage_count=nembd;
        result_n=fk_out(out,cap,
            "PASS metal_model_moe\nview=%llu\nroute-count=%zu\nexperts=",vi,fk_model_route_count);
        for(size_t i=0;i<fk_model_route_count&&result_n<cap-1;i++)
            result_n+=fk_out(out+result_n,cap-result_n,i?",%u":"%u",fk_model_route_ids[i]);
        result_n+=fk_out(out+result_n,cap-result_n,"\nweights=");
        for(size_t i=0;i<fk_model_route_count&&result_n<cap-1;i++)
            result_n+=fk_out(out+result_n,cap-result_n,i?",%.9g":"%.9g",fk_model_route_weights[i]);
        result_n+=fk_out(out+result_n,cap-result_n,
            "\ngate-base=%llu\ngate-stride=%llu\nup-base=%llu\nup-stride=%llu\n"
            "down-base=%llu\ndown-stride=%llu\nshared-gate-base=%llu\nshared-up-base=%llu\n"
            "shared-down-base=%llu\nn-embd=%lu\nn-ff=%lu\nclamp=%.9g\n"
            "gate-up-type=%lu\ndown-type=%lu\nexpert-count=%lu\nvalues=%lu\nfinite=%zu\n"
            "sum=%.17g\nabs-sum=%.17g\nsample=%.9g,%.9g,%.9g,%.9g\nelapsed-ms=%.3f\n"
            "carrier=in-process-metal\narithmetic-owner=form-emitted-metal\nhost-exec=0\nshell=0\nswift=0\n"
            "temp=0\nnetwork=0\nremote=0\noutcome=success\nrewitness-offer=metal-model-moe\n",
            gate_base,gate_stride,up_base,up_stride,down_base,down_stride,
            shared_gate_base,shared_up_base,shared_down_base,nembd,nff,clamp,
            gate_type,down_type,expert_count,nembd,finite,
            sum,abs_sum,v[0],v[1],v[2],v[3],elapsed);
cleanup_moe_return:
        for(size_t i=0;i<16;i++){[gate[i] release];[up[i] release];[mid[i] release];[down[i] release];}
        [sg release];[su release];[smid release];[shared release];[moe release];
        [p_mx4_fn release];[p_iq2_fn release];[p_swiglu_fn release];[p_scale_fn release];
        [p_axpy_fn release];[p_mx8_fn release];[p_mx4 release];[p_iq2 release];
        [p_swiglu release];[p_scale release];[p_axpy release];[p_mx8 release];[queue release];
        free(names);free(pt);return result_n;
bad_moe:
        free(names);free(pt);return fk_out(out,cap,
            "FAIL metal_model_moe plan-or-state stage=%zu route-count=%zu\n",
            fk_model_stage_count,fk_model_route_count);
    }}

long long fk_metal_model_hc_output_external(
    const char *library_names,long long library_names_len,
    const char *plan,long long plan_len,char*out,long long cap){
    @autoreleasepool{
        char*names=fk_copy_cstr(library_names,library_names_len);
        char*pt=fk_copy_cstr(plan,plan_len);
        if(!names||!pt){free(names);free(pt);
            return fk_out(out,cap,"FAIL metal_model_hc_output allocation\n");}
        char hc_name[128],f16_name[128];
        if(sscanf(names,"%127s %127s",hc_name,f16_name)!=2)goto bad_hc_output;
        char*c=pt,*e=NULL;
#define FK_HCO_U64(name) unsigned long long name=strtoull(c,&e,10);if(e==c)goto bad_hc_output;c=e
        FK_HCO_U64(fn_vi);FK_HCO_U64(fn_base);
        FK_HCO_U64(scale_vi);FK_HCO_U64(scale_base);
        FK_HCO_U64(base_vi);FK_HCO_U64(base_base);
        unsigned long nhc=strtoul(c,&e,10);if(e==c)goto bad_hc_output;c=e;
        unsigned long nembd=strtoul(c,&e,10);if(e==c)goto bad_hc_output;c=e;
        float eps=strtof(c,&e);if(e==c)goto bad_hc_output;
#undef FK_HCO_U64
        size_t total=(size_t)nhc*(size_t)nembd;
        size_t fn_bytes=total*2,scale_bytes=sizeof(float),base_bytes=(size_t)nhc*sizeof(float);
        if(!nhc||nhc>64||!nembd||total/nembd!=nhc||
           fn_vi>=fk_model_view_count||scale_vi>=fk_model_view_count||base_vi>=fk_model_view_count||
           fn_base>fk_model_views[fn_vi].length||fn_bytes>fk_model_views[fn_vi].length-fn_base||
           scale_base>fk_model_views[scale_vi].length||scale_bytes>fk_model_views[scale_vi].length-scale_base||
           base_base>fk_model_views[base_vi].length||base_bytes>fk_model_views[base_vi].length-base_base||
           !fk_model_stage||fk_model_stage_count!=total)goto bad_hc_output;
        id<MTLLibrary>hc_lib=fk_model_library_named(hc_name);
        id<MTLLibrary>f16_lib=fk_model_library_named(f16_name);
        NSError*error=nil;
#define FK_HCO_PIPE(var,lib,name) \
        id<MTLFunction> var##_fn=[lib newFunctionWithName:@name]; \
        id<MTLComputePipelineState> var=var##_fn?fk_model_pipeline_for(var##_fn,&error):nil
        FK_HCO_PIPE(p_rms,hc_lib,"form_hc_rmsnorm_nw_f32");
        FK_HCO_PIPE(p_f16,f16_lib,"form_dsv4_f16_matvec");
        FK_HCO_PIPE(p_headw,hc_lib,"form_hc_headw_f32");
        FK_HCO_PIPE(p_wsum,hc_lib,"form_hc_wsum_f32");
#undef FK_HCO_PIPE
        id<MTLCommandQueue>queue=fk_model_command_queue();
        id<MTLBuffer>flat=[fk_model_device newBufferWithLength:total*sizeof(float)
            options:MTLResourceStorageModePrivate];
        id<MTLBuffer>pre=[fk_model_device newBufferWithLength:nhc*sizeof(float)
            options:MTLResourceStorageModePrivate];
        id<MTLBuffer>weights=[fk_model_device newBufferWithLength:nhc*sizeof(float)
            options:MTLResourceStorageModeShared];
        id<MTLBuffer>collapsed=[fk_model_device newBufferWithLength:nembd*sizeof(float)
            options:MTLResourceStorageModeShared];
        long long result_n=0;
        if(!p_rms||!p_f16||!p_headw||!p_wsum||!queue||!flat||!pre||!weights||!collapsed){
            const char*m=error.localizedDescription.UTF8String?:"unknown";
            result_n=fk_out(out,cap,"FAIL metal_model_hc_output pipeline=%s\n",m);
            goto cleanup_hc_output;
        }
        uint32_t total32=(uint32_t)total,nhc32=(uint32_t)nhc,nembd32=(uint32_t)nembd;
        uint32_t fn_rows=(uint32_t)nhc,fn_cols=(uint32_t)total;
        double started=fk_metal_now_ms();
        id<MTLCommandBuffer>command=[queue commandBuffer];
        id<MTLComputeCommandEncoder>enc=[command computeCommandEncoder];
        [enc setComputePipelineState:p_rms];[enc setBuffer:fk_model_stage offset:0 atIndex:0];
        [enc setBuffer:flat offset:0 atIndex:1];[enc setBytes:&total32 length:4 atIndex:2];
        [enc setBytes:&eps length:4 atIndex:3];
        [enc dispatchThreads:MTLSizeMake(1,1,1) threadsPerThreadgroup:MTLSizeMake(1,1,1)];[enc endEncoding];
        enc=[command computeCommandEncoder];[enc setComputePipelineState:p_f16];
        [enc setBuffer:fk_model_views[fn_vi] offset:(NSUInteger)fn_base atIndex:0];
        [enc setBuffer:flat offset:0 atIndex:1];[enc setBuffer:pre offset:0 atIndex:2];
        [enc setBytes:&fn_rows length:4 atIndex:3];[enc setBytes:&fn_cols length:4 atIndex:4];
        [enc dispatchThreads:MTLSizeMake(nhc,1,1) threadsPerThreadgroup:MTLSizeMake(nhc,1,1)];[enc endEncoding];
        enc=[command computeCommandEncoder];[enc setComputePipelineState:p_headw];
        [enc setBuffer:pre offset:0 atIndex:0];
        [enc setBuffer:fk_model_views[scale_vi] offset:(NSUInteger)scale_base atIndex:1];
        [enc setBuffer:fk_model_views[base_vi] offset:(NSUInteger)base_base atIndex:2];
        [enc setBuffer:weights offset:0 atIndex:3];[enc setBytes:&nhc32 length:4 atIndex:4];
        [enc setBytes:&eps length:4 atIndex:5];
        [enc dispatchThreads:MTLSizeMake(nhc,1,1) threadsPerThreadgroup:MTLSizeMake(nhc,1,1)];[enc endEncoding];
        enc=[command computeCommandEncoder];[enc setComputePipelineState:p_wsum];
        [enc setBuffer:fk_model_stage offset:0 atIndex:0];[enc setBuffer:weights offset:0 atIndex:1];
        [enc setBuffer:collapsed offset:0 atIndex:2];[enc setBytes:&nhc32 length:4 atIndex:3];
        [enc setBytes:&nembd32 length:4 atIndex:4];
        NSUInteger width=MIN(p_wsum.maxTotalThreadsPerThreadgroup,p_wsum.threadExecutionWidth*4);
        [enc dispatchThreads:MTLSizeMake(nembd,1,1) threadsPerThreadgroup:MTLSizeMake(width,1,1)];
        [enc endEncoding];[command commit];[command waitUntilCompleted];
        double elapsed=fk_metal_now_ms()-started;
        if(command.status!=MTLCommandBufferStatusCompleted){
            const char*m=command.error.localizedDescription.UTF8String?:"unknown";
            result_n=fk_out(out,cap,"FAIL metal_model_hc_output dispatch=%s\n",m);
            goto cleanup_hc_output;
        }
        float*v=collapsed.contents,*w=weights.contents;double sum=0,abs_sum=0;size_t finite=0;
        for(size_t i=0;i<nembd;i++){if(isfinite(v[i]))finite++;sum+=v[i];abs_sum+=fabs((double)v[i]);}
        [fk_model_stage release];fk_model_stage=[collapsed retain];fk_model_stage_count=nembd;
        result_n=fk_out(out,cap,
            "PASS metal_model_hc_output\nfn-view=%llu\nfn-base-bytes=%llu\n"
            "scale-view=%llu\nscale-base-bytes=%llu\nbase-view=%llu\nbase-base-bytes=%llu\n"
            "n-hc=%lu\nn-embd=%lu\nweights=%.9g,%.9g,%.9g,%.9g\nvalues=%lu\nfinite=%zu\n"
            "sum=%.17g\nabs-sum=%.17g\nsample=%.9g,%.9g,%.9g,%.9g\nelapsed-ms=%.3f\n"
            "carrier=in-process-metal\narithmetic-owner=form-emitted-metal\nhost-exec=0\nshell=0\n"
            "swift=0\ntemp=0\nnetwork=0\nremote=0\noutcome=success\n"
            "rewitness-offer=metal-model-hc-output\n",
            fn_vi,fn_base,scale_vi,scale_base,base_vi,base_base,nhc,nembd,
            w[0],w[nhc>1?1:0],w[nhc>2?2:0],w[nhc>3?3:0],nembd,finite,
            sum,abs_sum,v[0],v[1],v[2],v[3],elapsed);
cleanup_hc_output:
        [p_rms_fn release];[p_f16_fn release];[p_headw_fn release];[p_wsum_fn release];
        [p_rms release];[p_f16 release];[p_headw release];[p_wsum release];[queue release];
        [flat release];[pre release];[weights release];[collapsed release];
        free(names);free(pt);return result_n;
bad_hc_output:
        free(names);free(pt);return fk_out(out,cap,
            "FAIL metal_model_hc_output plan-or-stage stage=%zu\n",fk_model_stage_count);
    }}

long long fk_metal_model_kv_round_external(
    const char *library_name,long long library_name_len,
    const char *plan,long long plan_len,char*out,long long cap){
    @autoreleasepool{
        char*ln=fk_copy_cstr(library_name,library_name_len),*pt=fk_copy_cstr(plan,plan_len);
        if(!ln||!pt){free(ln);free(pt);return fk_out(out,cap,"FAIL metal_model_kv_round allocation\n");}
        char*c=pt,*e=NULL;unsigned long width=strtoul(c,&e,10);if(e==c)goto bad_kv_round;c=e;
        unsigned long rotated=strtoul(c,&e,10);
        if(e==c||!width||rotated>width||fk_model_stage_count!=width)goto bad_kv_round;
        id<MTLLibrary>lib=fk_model_library_named(ln);
        id<MTLFunction>fn=[lib newFunctionWithName:@"form_dsv4_kv_fp8_f16_round"];NSError*error=nil;
        id<MTLComputePipelineState>pipe=fn?fk_model_pipeline_for(fn,&error):nil;
        id<MTLCommandQueue>queue=fk_model_command_queue();
        id<MTLBuffer>result=[fk_model_device newBufferWithLength:width*sizeof(float)
            options:MTLResourceStorageModeShared];
        if(!pipe||!queue||!result){const char*m=error.localizedDescription.UTF8String?:"unknown";
            [fn release];[pipe release];[queue release];[result release];free(ln);free(pt);
            return fk_out(out,cap,"FAIL metal_model_kv_round pipeline=%s\n",m);}
        uint32_t w32=(uint32_t)width,r32=(uint32_t)rotated;double started=fk_metal_now_ms();
        id<MTLCommandBuffer>command=[queue commandBuffer];id<MTLComputeCommandEncoder>enc=[command computeCommandEncoder];
        [enc setComputePipelineState:pipe];[enc setBuffer:fk_model_stage offset:0 atIndex:0];
        [enc setBuffer:result offset:0 atIndex:1];[enc setBytes:&w32 length:4 atIndex:2];
        [enc setBytes:&r32 length:4 atIndex:3];
        [enc dispatchThreads:MTLSizeMake(1,1,1) threadsPerThreadgroup:MTLSizeMake(1,1,1)];
        [enc endEncoding];[command commit];[command waitUntilCompleted];double elapsed=fk_metal_now_ms()-started;
        if(command.status!=MTLCommandBufferStatusCompleted){const char*m=command.error.localizedDescription.UTF8String?:"unknown";
            [fn release];[pipe release];[queue release];[result release];free(ln);free(pt);
            return fk_out(out,cap,"FAIL metal_model_kv_round dispatch=%s\n",m);}
        float*v=result.contents;double sum=0,abs_sum=0;size_t finite=0,changed=0;float*prior=fk_model_stage.contents;
        for(size_t i=0;i<width;i++){if(isfinite(v[i]))finite++;if(v[i]!=prior[i])changed++;
            sum+=v[i];abs_sum+=fabs((double)v[i]);}
        [fk_model_stage release];fk_model_stage=result;fk_model_stage_count=width;
        long long n=fk_out(out,cap,
            "PASS metal_model_kv_round\nwidth=%lu\nrotated=%lu\nchanged=%zu\nfinite=%zu\n"
            "sum=%.17g\nabs-sum=%.17g\nsample=%.9g,%.9g,%.9g,%.9g\nelapsed-ms=%.3f\n"
            "quantization=E4M3FN-per-64-NOPE-then-F16-whole\ncarrier=in-process-metal\n"
            "arithmetic-owner=form-emitted-metal\nhost-exec=0\nshell=0\nswift=0\ntemp=0\n"
            "network=0\nremote=0\noutcome=success\nrewitness-offer=metal-model-kv-round\n",
            width,rotated,changed,finite,sum,abs_sum,v[0],v[1],v[2],v[3],elapsed);
        [fn release];[pipe release];[queue release];free(ln);free(pt);return n;
bad_kv_round:
        free(ln);free(pt);return fk_out(out,cap,
            "FAIL metal_model_kv_round plan-or-stage stage=%zu\n",fk_model_stage_count);
    }}

long long fk_metal_model_cached_attention_external(
    const char *library_name,long long library_name_len,
    const char *plan,long long plan_len,char*out,long long cap){
    @autoreleasepool{
        char*ln=fk_copy_cstr(library_name,library_name_len),*pt=fk_copy_cstr(plan,plan_len);
        if(!ln||!pt){free(ln);free(pt);
            return fk_out(out,cap,"FAIL metal_model_cached_attention allocation\n");}
        char*c=pt,*e=c;while(*e&&*e!=' ')e++;if(e==c||!*e)goto bad_cached_attention;
        *e=0;char*qname=c;c=e+1;
        unsigned long layer=strtoul(c,&e,10);if(e==c)goto bad_cached_attention;c=e;
        unsigned long position=strtoul(c,&e,10);if(e==c)goto bad_cached_attention;c=e;
        unsigned long cache_cap=strtoul(c,&e,10);if(e==c)goto bad_cached_attention;c=e;
        unsigned long long sink_vi=strtoull(c,&e,10);if(e==c)goto bad_cached_attention;c=e;
        unsigned long long sink_base=strtoull(c,&e,10);if(e==c)goto bad_cached_attention;c=e;
        unsigned long heads=strtoul(c,&e,10);if(e==c)goto bad_cached_attention;c=e;
        unsigned long width=strtoul(c,&e,10);if(e==c)goto bad_cached_attention;c=e;
        float scale=strtof(c,&e);if(e==c)goto bad_cached_attention;
        id<MTLBuffer>q=nil;size_t qcount=0;
        for(size_t i=0;i<fk_model_state_count;i++)if(strcmp(fk_model_states[i].name,qname)==0){
            q=fk_model_states[i].buffer;qcount=fk_model_states[i].count;break;}
        if(layer>=64||!cache_cap||!heads||!width||qcount!=(size_t)heads*width||
           fk_model_stage_count!=width||sink_vi>=fk_model_view_count||
           sink_base>fk_model_views[sink_vi].length||
           heads*sizeof(float)>fk_model_views[sink_vi].length-sink_base)
            goto bad_cached_attention;
        struct fk_model_dkv*cache=&fk_model_dkv[layer];
        size_t sink_bytes=heads*sizeof(float);
        const char*sink_source=(const char*)fk_model_views[sink_vi].contents+sink_base;
        if(!cache->attention_sinks){
            cache->attention_sinks=[fk_model_device newBufferWithBytes:sink_source
                length:sink_bytes options:MTLResourceStorageModeShared];
            cache->attention_sinks_bytes=sink_bytes;
        }
        if(!cache->attention_sinks||cache->attention_sinks_bytes!=sink_bytes||
           memcmp(cache->attention_sinks.contents,sink_source,sink_bytes)!=0)
            goto bad_cached_attention;
        if(!cache->raw){
            cache->raw=[fk_model_device newBufferWithLength:(size_t)cache_cap*width*sizeof(float)
                options:MTLResourceStorageModeShared];
            cache->raw_cap=cache_cap;cache->width=width;
        }
        if(!cache->raw||cache->raw_cap!=cache_cap||cache->width!=width||
           position!=cache->n_raw||cache->n_raw>=cache->raw_cap)
            goto bad_cached_attention;
        id<MTLLibrary>lib=fk_model_library_named(ln);
        NSError*error=nil;
        id<MTLFunction>append_fn=[lib newFunctionWithName:@"form_dkv_append_f32"];
        id<MTLFunction>attend_fn=[lib newFunctionWithName:@"form_dkv_attend_mixed_f32"];
        id<MTLComputePipelineState>append=append_fn?fk_model_pipeline_for(append_fn,&error):nil;
        id<MTLComputePipelineState>attend=attend_fn?fk_model_pipeline_for(attend_fn,&error):nil;
        id<MTLCommandQueue>queue=fk_model_command_queue();
        id<MTLBuffer>empty=[fk_model_device newBufferWithLength:width*sizeof(float)
            options:MTLResourceStorageModeShared];
        id<MTLBuffer>result=[fk_model_device newBufferWithLength:qcount*sizeof(float)
            options:MTLResourceStorageModeShared];
        if(!append||!attend||!queue||!empty||!result){const char*m=error.localizedDescription.UTF8String?:"unknown";
            [append_fn release];[attend_fn release];[append release];[attend release];[queue release];
            [empty release];[result release];free(ln);free(pt);
            return fk_out(out,cap,"FAIL metal_model_cached_attention pipeline=%s\n",m);}
        uint32_t width32=(uint32_t)width,pos32=(uint32_t)position,cap32=(uint32_t)cache_cap;
        uint32_t heads32=(uint32_t)heads,nraw32=(uint32_t)(cache->n_raw+1);
        uint32_t ncomp32=(uint32_t)cache->n_comp;
        double started=fk_metal_now_ms();id<MTLCommandBuffer>command=[queue commandBuffer];
        id<MTLComputeCommandEncoder>enc=[command computeCommandEncoder];
        [enc setComputePipelineState:append];[enc setBuffer:fk_model_stage offset:0 atIndex:0];
        [enc setBuffer:cache->raw offset:0 atIndex:1];[enc setBytes:&width32 length:4 atIndex:2];
        [enc setBytes:&pos32 length:4 atIndex:3];[enc setBytes:&cap32 length:4 atIndex:4];
        NSUInteger aw=MIN(append.maxTotalThreadsPerThreadgroup,append.threadExecutionWidth*4);
        [enc dispatchThreads:MTLSizeMake(width,1,1) threadsPerThreadgroup:MTLSizeMake(aw,1,1)];[enc endEncoding];
        enc=[command computeCommandEncoder];[enc setComputePipelineState:attend];
        [enc setBuffer:q offset:0 atIndex:0];[enc setBuffer:cache->raw offset:0 atIndex:1];
        [enc setBuffer:cache->comp?cache->comp:empty offset:0 atIndex:2];[enc setBuffer:result offset:0 atIndex:3];
        [enc setBuffer:cache->attention_sinks offset:0 atIndex:4];
        [enc setBytes:&heads32 length:4 atIndex:5];[enc setBytes:&width32 length:4 atIndex:6];
        [enc setBytes:&nraw32 length:4 atIndex:7];[enc setBytes:&ncomp32 length:4 atIndex:8];
        [enc setBytes:&scale length:4 atIndex:9];
        NSUInteger tw=MIN(attend.maxTotalThreadsPerThreadgroup,attend.threadExecutionWidth);
        [enc dispatchThreads:MTLSizeMake(heads,1,1) threadsPerThreadgroup:MTLSizeMake(tw,1,1)];
        [enc endEncoding];[command commit];[command waitUntilCompleted];double elapsed=fk_metal_now_ms()-started;
        if(command.status!=MTLCommandBufferStatusCompleted){const char*m=command.error.localizedDescription.UTF8String?:"unknown";
            [append_fn release];[attend_fn release];[append release];[attend release];[queue release];
            [empty release];[result release];free(ln);free(pt);
            return fk_out(out,cap,"FAIL metal_model_cached_attention dispatch=%s\n",m);}
        cache->n_raw++;
        float*v=result.contents;double sum=0,abs_sum=0;size_t finite=0;
        for(size_t i=0;i<qcount;i++){if(isfinite(v[i]))finite++;sum+=v[i];abs_sum+=fabs((double)v[i]);}
        [fk_model_stage release];fk_model_stage=[result retain];fk_model_stage_count=qcount;
        long long n=fk_out(out,cap,
            "PASS metal_model_cached_attention\nlayer=%lu\nposition=%lu\nq-state=%s\n"
            "sink-view=%llu\nsink-base-bytes=%llu\nraw-rows=%zu\ncompressed-rows=%zu\n"
            "sink-residency=session\nsink-source-identity=byte-exact\nsink-bytes=%zu\n"
            "cache-cap=%zu\nheads=%lu\nwidth=%lu\nvalues=%zu\nfinite=%zu\n"
            "sum=%.17g\nabs-sum=%.17g\nsample=%.9g,%.9g,%.9g,%.9g\nelapsed-ms=%.3f\n"
            "cache-owner=in-process-form-metal-session\narithmetic-owner=form-emitted-metal\n"
            "host-exec=0\nshell=0\nswift=0\ntemp=0\nnetwork=0\nremote=0\noutcome=success\n"
            "rewitness-offer=metal-model-cached-attention:layer-%lu:position-%lu\n",
            layer,position,qname,sink_vi,sink_base,cache->n_raw,cache->n_comp,sink_bytes,cache->raw_cap,heads,width,
            qcount,finite,sum,abs_sum,v[0],v[1],v[2],v[3],elapsed,layer,position);
        [append_fn release];[attend_fn release];[append release];[attend release];[queue release];
        [empty release];[result release];free(ln);free(pt);return n;
bad_cached_attention:
        free(ln);free(pt);return fk_out(out,cap,
            "FAIL metal_model_cached_attention plan-or-state stage=%zu\n",fk_model_stage_count);
    }}

long long fk_metal_model_compress_stage_external(
    const char *library_names,long long library_names_len,
    const char *plan,long long plan_len,char*out,long long cap){
    @autoreleasepool{
        char*names=fk_copy_cstr(library_names,library_names_len),*pt=fk_copy_cstr(plan,plan_len);
        if(!names||!pt){free(names);free(pt);
            return fk_out(out,cap,"FAIL metal_model_compress_stage allocation\n");}
        char cache_name[128],f16_name[128],rms_name[128],rope_name[128],round_name[128];
        if(sscanf(names,"%127s %127s %127s %127s %127s",
            cache_name,f16_name,rms_name,rope_name,round_name)!=5)goto bad_compress_stage;
        char*c=pt,*e=c;while(*e&&*e!=' ')e++;if(e==c||!*e)goto bad_compress_stage;
        *e=0;char*norm_name=c;c=e+1;
#define FK_CS_UL(name) unsigned long name=strtoul(c,&e,10);if(e==c)goto bad_compress_stage;c=e
#define FK_CS_U64(name) unsigned long long name=strtoull(c,&e,10);if(e==c)goto bad_compress_stage;c=e
        FK_CS_UL(layer);FK_CS_UL(position);FK_CS_UL(ratio);FK_CS_UL(cache_cap);
        FK_CS_U64(kv_vi);FK_CS_U64(kv_base);FK_CS_UL(kv_rows);FK_CS_UL(kv_cols);
        FK_CS_U64(gate_vi);FK_CS_U64(gate_base);FK_CS_UL(gate_rows);FK_CS_UL(gate_cols);
        FK_CS_U64(ape_vi);FK_CS_U64(ape_base);
        FK_CS_U64(norm_vi);FK_CS_U64(norm_base);
#undef FK_CS_UL
#undef FK_CS_U64
        id<MTLBuffer>norm_state=nil;size_t norm_count=0;
        for(size_t i=0;i<fk_model_state_count;i++)if(strcmp(fk_model_states[i].name,norm_name)==0){
            norm_state=fk_model_states[i].buffer;norm_count=fk_model_states[i].count;break;}
        size_t width=kv_rows,state_rows=ratio==4?8:ratio;
        if(layer>=64||(ratio!=4&&ratio!=128)||!cache_cap||!width||
           kv_rows!=gate_rows||kv_cols!=gate_cols||norm_count!=kv_cols||
           kv_vi>=fk_model_view_count||gate_vi>=fk_model_view_count||
           ape_vi>=fk_model_view_count||norm_vi>=fk_model_view_count||
           kv_base>fk_model_views[kv_vi].length||(size_t)kv_rows*kv_cols*2>fk_model_views[kv_vi].length-kv_base||
           gate_base>fk_model_views[gate_vi].length||(size_t)gate_rows*gate_cols*2>fk_model_views[gate_vi].length-gate_base||
           ape_base>fk_model_views[ape_vi].length||(size_t)ratio*width*2>fk_model_views[ape_vi].length-ape_base||
           norm_base>fk_model_views[norm_vi].length||512*sizeof(float)>fk_model_views[norm_vi].length-norm_base)
            goto bad_compress_stage;
        struct fk_model_dkv*cache=&fk_model_dkv[layer];
        if(!cache->state_kv){
            size_t compressor_matrix_bytes=width*kv_cols*sizeof(uint16_t);
            size_t compressor_ape_bytes=ratio*width*sizeof(uint16_t);
            cache->ratio=(uint32_t)ratio;cache->width=512;cache->comp_cap=cache_cap/ratio+2;
            cache->state_kv=[fk_model_device newBufferWithLength:state_rows*width*sizeof(float)
                options:MTLResourceStorageModeShared];
            cache->state_score=[fk_model_device newBufferWithLength:state_rows*width*sizeof(float)
                options:MTLResourceStorageModeShared];
            cache->comp=[fk_model_device newBufferWithLength:cache->comp_cap*512*sizeof(float)
                options:MTLResourceStorageModeShared];
            cache->compressor_kv=[fk_model_device
                newBufferWithBytes:(const char*)fk_model_views[kv_vi].contents+kv_base
                length:compressor_matrix_bytes options:MTLResourceStorageModeShared];
            cache->compressor_gate=[fk_model_device
                newBufferWithBytes:(const char*)fk_model_views[gate_vi].contents+gate_base
                length:compressor_matrix_bytes options:MTLResourceStorageModeShared];
            cache->compressor_ape=[fk_model_device
                newBufferWithBytes:(const char*)fk_model_views[ape_vi].contents+ape_base
                length:compressor_ape_bytes options:MTLResourceStorageModeShared];
            cache->compressor_norm=[fk_model_device
                newBufferWithBytes:(const char*)fk_model_views[norm_vi].contents+norm_base
                length:512*sizeof(float) options:MTLResourceStorageModeShared];
            if(cache->state_score){float*s=cache->state_score.contents;
                for(size_t i=0;i<state_rows*width;i++)s[i]=-FLT_MAX;}
        }
        if(!cache->state_kv||!cache->state_score||!cache->comp||
           !cache->compressor_kv||!cache->compressor_gate||
           !cache->compressor_ape||!cache->compressor_norm||
           cache->ratio!=ratio||cache->width!=512)goto bad_compress_stage;
        id<MTLLibrary>cache_lib=fk_model_library_named(cache_name);
        id<MTLLibrary>f16_lib=fk_model_library_named(f16_name);
        id<MTLLibrary>rms_lib=fk_model_library_named(rms_name);
        id<MTLLibrary>rope_lib=fk_model_library_named(rope_name);
        id<MTLLibrary>round_lib=fk_model_library_named(round_name);
        NSError*error=nil;
#define FK_CS_PIPE(var,lib,name) \
        id<MTLFunction> var##_fn=[lib newFunctionWithName:@name]; \
        id<MTLComputePipelineState> var=var##_fn?fk_model_pipeline_for(var##_fn,&error):nil
        FK_CS_PIPE(p_f16,f16_lib,"form_dsv4_f16_matvec");
        FK_CS_PIPE(p_stage,cache_lib,"form_dkv_compressor_stage_f32");
        FK_CS_PIPE(p_pool,cache_lib,"form_dkv_compressor_pool_f32");
        FK_CS_PIPE(p_roll,cache_lib,"form_dkv_compressor_roll4_f32");
        FK_CS_PIPE(p_append,cache_lib,"form_dkv_append_f32");
        FK_CS_PIPE(p_rms,rms_lib,"form_dsv4_rmsnorm_f32");
        FK_CS_PIPE(p_rope,rope_lib,"form_dsv4_rope_f32");
        FK_CS_PIPE(p_round,round_lib,"form_dsv4_kv_fp8_f16_round");
#undef FK_CS_PIPE
        id<MTLCommandQueue>queue=fk_model_command_queue();
        id<MTLBuffer>kv_cur=[fk_model_device newBufferWithLength:width*sizeof(float) options:MTLResourceStorageModePrivate];
        id<MTLBuffer>score_cur=[fk_model_device newBufferWithLength:width*sizeof(float) options:MTLResourceStorageModePrivate];
        BOOL boundary=((position+1)%ratio)==0;
        id<MTLBuffer>pooled=boundary?[fk_model_device newBufferWithLength:512*sizeof(float) options:MTLResourceStorageModePrivate]:nil;
        id<MTLBuffer>normalized=boundary?[fk_model_device newBufferWithLength:512*sizeof(float) options:MTLResourceStorageModePrivate]:nil;
        id<MTLBuffer>roped=boundary?[fk_model_device newBufferWithLength:512*sizeof(float) options:MTLResourceStorageModePrivate]:nil;
        id<MTLBuffer>rounded=boundary?[fk_model_device newBufferWithLength:512*sizeof(float) options:MTLResourceStorageModeShared]:nil;
        long long result_n=0;
        if(!p_f16||!p_stage||!p_pool||!p_roll||!p_append||!p_rms||!p_rope||!p_round||
           !queue||!kv_cur||!score_cur||(boundary&&(!pooled||!normalized||!roped||!rounded))){
            const char*m=error.localizedDescription.UTF8String?:"unknown";
            result_n=fk_out(out,cap,"FAIL metal_model_compress_stage pipeline=%s\n",m);
            goto cleanup_compress_stage;
        }
        uint32_t rows32=(uint32_t)width,cols32=(uint32_t)kv_cols;
        uint32_t width32=(uint32_t)width,ratio32=(uint32_t)ratio,pos32=(uint32_t)position;
        double started=fk_metal_now_ms();id<MTLCommandBuffer>command=[queue commandBuffer];
        id<MTLComputeCommandEncoder>enc=[command computeCommandEncoder];
#define FK_CS_F16(weight_buffer,weight_base,target) \
        [enc setComputePipelineState:p_f16]; \
        [enc setBuffer:weight_buffer offset:(NSUInteger)weight_base atIndex:0]; \
        [enc setBuffer:norm_state offset:0 atIndex:1];[enc setBuffer:target offset:0 atIndex:2]; \
        [enc setBytes:&rows32 length:4 atIndex:3];[enc setBytes:&cols32 length:4 atIndex:4]; \
        [enc dispatchThreads:MTLSizeMake(width,1,1) threadsPerThreadgroup:MTLSizeMake(MIN((NSUInteger)width,p_f16.maxTotalThreadsPerThreadgroup),1,1)]; \
        [enc endEncoding]
        FK_CS_F16(cache->compressor_kv,0,kv_cur);
        enc=[command computeCommandEncoder];FK_CS_F16(cache->compressor_gate,0,score_cur);
#undef FK_CS_F16
        enc=[command computeCommandEncoder];[enc setComputePipelineState:p_stage];
        [enc setBuffer:kv_cur offset:0 atIndex:0];[enc setBuffer:score_cur offset:0 atIndex:1];
        [enc setBuffer:cache->compressor_ape offset:0 atIndex:2];
        [enc setBuffer:cache->state_kv offset:0 atIndex:3];[enc setBuffer:cache->state_score offset:0 atIndex:4];
        [enc setBytes:&width32 length:4 atIndex:5];[enc setBytes:&ratio32 length:4 atIndex:6];
        [enc setBytes:&pos32 length:4 atIndex:7];
        [enc dispatchThreads:MTLSizeMake(width,1,1) threadsPerThreadgroup:MTLSizeMake(256,1,1)];[enc endEncoding];
        if(boundary){
            uint32_t hd32=512,heads32=1,rot32=64,compressed32=1,comp_pos32=(uint32_t)(position+1-ratio);
            float eps=0.000001f,positionf=(float)comp_pos32,basef=160000.0f,sign=1.0f;
            float scale_factor=16.0f,orig=65536.0f,bfast=32.0f,bslow=1.0f;
            enc=[command computeCommandEncoder];[enc setComputePipelineState:p_pool];
            [enc setBuffer:cache->state_kv offset:0 atIndex:0];[enc setBuffer:cache->state_score offset:0 atIndex:1];
            [enc setBuffer:pooled offset:0 atIndex:2];[enc setBytes:&hd32 length:4 atIndex:3];
            [enc setBytes:&ratio32 length:4 atIndex:4];
            [enc dispatchThreads:MTLSizeMake(512,1,1) threadsPerThreadgroup:MTLSizeMake(256,1,1)];[enc endEncoding];
            enc=[command computeCommandEncoder];[enc setComputePipelineState:p_rms];
            [enc setBuffer:pooled offset:0 atIndex:0];[enc setBuffer:cache->compressor_norm offset:0 atIndex:1];
            [enc setBuffer:normalized offset:0 atIndex:2];[enc setBytes:&hd32 length:4 atIndex:3];
            [enc setBytes:&eps length:4 atIndex:4];
            [enc dispatchThreads:MTLSizeMake(1,1,1) threadsPerThreadgroup:MTLSizeMake(1,1,1)];[enc endEncoding];
            enc=[command computeCommandEncoder];[enc setComputePipelineState:p_rope];
            [enc setBuffer:normalized offset:0 atIndex:0];[enc setBuffer:roped offset:0 atIndex:1];
            [enc setBytes:&heads32 length:4 atIndex:2];[enc setBytes:&hd32 length:4 atIndex:3];
            [enc setBytes:&rot32 length:4 atIndex:4];[enc setBytes:&positionf length:4 atIndex:5];
            [enc setBytes:&basef length:4 atIndex:6];[enc setBytes:&sign length:4 atIndex:7];
            [enc setBytes:&compressed32 length:4 atIndex:8];[enc setBytes:&scale_factor length:4 atIndex:9];
            [enc setBytes:&orig length:4 atIndex:10];[enc setBytes:&bfast length:4 atIndex:11];
            [enc setBytes:&bslow length:4 atIndex:12];
            [enc dispatchThreads:MTLSizeMake(1,1,1) threadsPerThreadgroup:MTLSizeMake(1,1,1)];[enc endEncoding];
            enc=[command computeCommandEncoder];[enc setComputePipelineState:p_round];
            [enc setBuffer:roped offset:0 atIndex:0];[enc setBuffer:rounded offset:0 atIndex:1];
            [enc setBytes:&hd32 length:4 atIndex:2];[enc setBytes:&rot32 length:4 atIndex:3];
            [enc dispatchThreads:MTLSizeMake(1,1,1) threadsPerThreadgroup:MTLSizeMake(1,1,1)];[enc endEncoding];
            uint32_t cp=(uint32_t)cache->n_comp,cc=(uint32_t)cache->comp_cap;
            enc=[command computeCommandEncoder];[enc setComputePipelineState:p_append];
            [enc setBuffer:rounded offset:0 atIndex:0];[enc setBuffer:cache->comp offset:0 atIndex:1];
            [enc setBytes:&hd32 length:4 atIndex:2];[enc setBytes:&cp length:4 atIndex:3];
            [enc setBytes:&cc length:4 atIndex:4];
            [enc dispatchThreads:MTLSizeMake(512,1,1) threadsPerThreadgroup:MTLSizeMake(256,1,1)];[enc endEncoding];
            if(ratio==4){
                enc=[command computeCommandEncoder];[enc setComputePipelineState:p_roll];
                [enc setBuffer:cache->state_kv offset:0 atIndex:0];[enc setBuffer:cache->state_score offset:0 atIndex:1];
                [enc setBytes:&width32 length:4 atIndex:2];
                [enc dispatchThreads:MTLSizeMake(4*width,1,1) threadsPerThreadgroup:MTLSizeMake(256,1,1)];[enc endEncoding];
            }
        }
        [command commit];[command waitUntilCompleted];double elapsed=fk_metal_now_ms()-started;
        if(command.status!=MTLCommandBufferStatusCompleted){const char*m=command.error.localizedDescription.UTF8String?:"unknown";
            result_n=fk_out(out,cap,"FAIL metal_model_compress_stage dispatch=%s\n",m);
            goto cleanup_compress_stage;
        }
        if(boundary)cache->n_comp++;
        result_n=fk_out(out,cap,
            "PASS metal_model_compress_stage\nlayer=%lu\nposition=%lu\nratio=%lu\n"
            "projected-width=%zu\nboundary=%u\ncompressed-rows=%zu\ncompressed-cap=%zu\n"
            "kv-view=%llu\nkv-base-bytes=%llu\ngate-view=%llu\ngate-base-bytes=%llu\n"
            "ape-view=%llu\nape-base-bytes=%llu\nnorm-view=%llu\nnorm-base-bytes=%llu\n"
            "elapsed-ms=%.3f\ncache-owner=in-process-form-metal-session\n"
            "arithmetic-owner=form-emitted-metal\nhost-exec=0\nshell=0\nswift=0\ntemp=0\n"
            "network=0\nremote=0\noutcome=success\nrewitness-offer=metal-model-compress-stage:layer-%lu:position-%lu\n",
            layer,position,ratio,width,boundary?1:0,cache->n_comp,cache->comp_cap,
            kv_vi,kv_base,gate_vi,gate_base,ape_vi,ape_base,norm_vi,norm_base,
            elapsed,layer,position);
cleanup_compress_stage:
        [p_f16_fn release];[p_stage_fn release];[p_pool_fn release];[p_roll_fn release];
        [p_append_fn release];[p_rms_fn release];[p_rope_fn release];[p_round_fn release];
        [p_f16 release];[p_stage release];[p_pool release];[p_roll release];[p_append release];
        [p_rms release];[p_rope release];[p_round release];[queue release];
        [kv_cur release];[score_cur release];[pooled release];[normalized release];[roped release];[rounded release];
        free(names);free(pt);return result_n;
bad_compress_stage:
        free(names);free(pt);return fk_out(out,cap,
            "FAIL metal_model_compress_stage plan-or-state stage=%zu\n",fk_model_stage_count);
    }}

long long fk_metal_model_argmax_external(
    const char *library_name,long long library_name_len,
    const char *plan,long long plan_len,char*out,long long cap){
    @autoreleasepool{
        char*ln=fk_copy_cstr(library_name,library_name_len),*pt=fk_copy_cstr(plan,plan_len);
        if(!ln||!pt){free(ln);free(pt);return fk_out(out,cap,"FAIL metal_model_argmax allocation\n");}
        char*e=NULL;unsigned long vocab=strtoul(pt,&e,10);
        if(e==pt||!vocab||!fk_model_stage||fk_model_stage_count!=vocab)goto bad_argmax;
        id<MTLLibrary>lib=fk_model_library_named(ln);
        id<MTLFunction>fn=[lib newFunctionWithName:@"form_dsv4_argmax_f32"];NSError*error=nil;
        id<MTLComputePipelineState>pipe=fn?fk_model_pipeline_for(fn,&error):nil;
        id<MTLCommandQueue>queue=fk_model_command_queue();
        id<MTLBuffer>token=[fk_model_device newBufferWithLength:sizeof(uint32_t) options:MTLResourceStorageModeShared];
        id<MTLBuffer>score=[fk_model_device newBufferWithLength:sizeof(float) options:MTLResourceStorageModeShared];
        if(!pipe||!queue||!token||!score){const char*m=error.localizedDescription.UTF8String?:"unknown";
            [fn release];[pipe release];[queue release];[token release];[score release];
            free(ln);free(pt);return fk_out(out,cap,"FAIL metal_model_argmax pipeline=%s\n",m);}
        uint32_t n32=(uint32_t)vocab;double started=fk_metal_now_ms();
        id<MTLCommandBuffer>command=[queue commandBuffer];id<MTLComputeCommandEncoder>enc=[command computeCommandEncoder];
        [enc setComputePipelineState:pipe];[enc setBuffer:fk_model_stage offset:0 atIndex:0];
        [enc setBuffer:token offset:0 atIndex:1];[enc setBuffer:score offset:0 atIndex:2];
        [enc setBytes:&n32 length:4 atIndex:3];
        [enc dispatchThreads:MTLSizeMake(1,1,1) threadsPerThreadgroup:MTLSizeMake(1,1,1)];
        [enc endEncoding];[command commit];[command waitUntilCompleted];double elapsed=fk_metal_now_ms()-started;
        if(command.status!=MTLCommandBufferStatusCompleted){const char*m=command.error.localizedDescription.UTF8String?:"unknown";
            [fn release];[pipe release];[queue release];[token release];[score release];
            free(ln);free(pt);return fk_out(out,cap,"FAIL metal_model_argmax dispatch=%s\n",m);}
        uint32_t token_id=*(uint32_t*)token.contents;float best=*(float*)score.contents;
        float*logits=fk_model_stage.contents;size_t finite=0;
        for(size_t i=0;i<vocab;i++)if(isfinite(logits[i]))finite++;
        long long n=fk_out(out,cap,
            "PASS metal_model_argmax\ntoken-id=%u\nscore=%.9g\nvocab=%lu\nfinite=%zu\n"
            "elapsed-ms=%.3f\ncarrier=in-process-metal\ntoken-owner=form-emitted-metal\n"
            "host-exec=0\nshell=0\nswift=0\ntemp=0\nnetwork=0\nremote=0\n"
            "outcome=success\nrewitness-offer=metal-model-argmax\n",
            token_id,best,vocab,finite,elapsed);
        [fn release];[pipe release];[queue release];[token release];[score release];
        free(ln);free(pt);return n;
bad_argmax:
        free(ln);free(pt);return fk_out(out,cap,
            "FAIL metal_model_argmax plan-or-stage stage=%zu\n",fk_model_stage_count);
    }}

long long fk_metal_model_state_save_external(
    const char*name,long long name_len,char*out,long long cap){
    @autoreleasepool{
        char*n=fk_copy_cstr(name,name_len);
        if(!n||!fk_model_stage){free(n);return fk_out(out,cap,"FAIL metal_model_state_save stage=none\n");}
        size_t at=fk_model_state_count;
        for(size_t i=0;i<fk_model_state_count;i++)if(strcmp(fk_model_states[i].name,n)==0){at=i;break;}
        if(at==fk_model_state_count){
            struct fk_model_state*g=realloc(fk_model_states,(at+1)*sizeof(*g));
            if(!g){free(n);return fk_out(out,cap,"FAIL metal_model_state_save allocation\n");}
            fk_model_states=g;fk_model_state_count++;fk_model_states[at].buffer=nil;
            fk_model_states[at].name=n;
        }else free(n);
        [fk_model_states[at].buffer release];
        fk_model_states[at].buffer=[fk_model_stage retain];
        fk_model_states[at].count=fk_model_stage_count;
        return fk_out(out,cap,
            "PASS metal_model_state_save\nname=%s\nvalues=%zu\nstates=%zu\n"
            "carrier=in-process-metal\nhost-exec=0\nshell=0\nswift=0\ntemp=0\nnetwork=0\nremote=0\n"
            "outcome=success\nrewitness-offer=metal-model-state-load:%s\n",
            fk_model_states[at].name,fk_model_stage_count,fk_model_state_count,fk_model_states[at].name);
    }}
long long fk_metal_model_state_load_external(
    const char*name,long long name_len,char*out,long long cap){
    @autoreleasepool{
        char*n=fk_copy_cstr(name,name_len);if(!n)return fk_out(out,cap,"FAIL metal_model_state_load allocation\n");
        for(size_t i=0;i<fk_model_state_count;i++)if(strcmp(fk_model_states[i].name,n)==0){
            [fk_model_stage release];fk_model_stage=[fk_model_states[i].buffer retain];
            fk_model_stage_count=fk_model_states[i].count;free(n);
            return fk_out(out,cap,
                "PASS metal_model_state_load\nname=%s\nvalues=%zu\n"
                "carrier=in-process-metal\nhost-exec=0\nshell=0\nswift=0\ntemp=0\nnetwork=0\nremote=0\n"
                "outcome=success\nrewitness-offer=metal-model-state-save:%s\n",
                fk_model_states[i].name,fk_model_stage_count,fk_model_states[i].name);}
        long long r=fk_out(out,cap,"FAIL metal_model_state_load name=%s not-found\n",n);free(n);return r;
    }}

long long fk_metal_model_close_external(char *out, long long cap) {
    @autoreleasepool {
        size_t views = fk_model_view_count;
        size_t libraries = fk_model_library_count;
        fk_metal_model_release();
        return fk_out(out, cap,
            "PASS metal_model_close\nreleased-views=%zu\nreleased-libraries=%zu\noutcome=success\n",
            views, libraries);
    }
}

long long fk_metal_matvec_f32_external(const char *msl, long long msl_len,
                                      const char *kernel, long long kernel_len,
                                      const char *model, long long model_len,
                                      char *out, long long cap) {
    @autoreleasepool {
        char *model_text = fk_copy_cstr(model, model_len);
        if (model_text == NULL) return fk_out(out, cap, "FAIL metal_port allocation=model\n");

        char *cursor = model_text;
        char *end = NULL;
        long rows_long = strtol(cursor, &end, 10);
        if (end == cursor) {
            free(model_text);
            return fk_out(out, cap, "FAIL metal_port model_shape=rows\n");
        }
        cursor = end;
        long cols_long = strtol(cursor, &end, 10);
        if (end == cursor || rows_long <= 0 || cols_long <= 0 ||
            rows_long > 1048576 || cols_long > 1048576) {
            free(model_text);
            return fk_out(out, cap, "FAIL metal_port model_shape=cols\n");
        }

        size_t rows = (size_t)rows_long;
        size_t cols = (size_t)cols_long;
        if (rows > SIZE_MAX / cols || rows * cols > SIZE_MAX - cols) {
            free(model_text);
            return fk_out(out, cap, "FAIL metal_port model_shape=overflow\n");
        }
        size_t value_count = rows * cols + cols;
        float *values = malloc(value_count * sizeof(float));
        if (values == NULL) {
            free(model_text);
            return fk_out(out, cap, "FAIL metal_port allocation=values\n");
        }
        cursor = end;
        for (size_t i = 0; i < value_count; i++) {
            values[i] = strtof(cursor, &end);
            if (end == cursor) {
                free(values);
                free(model_text);
                return fk_out(out, cap, "FAIL metal_port model_values=%zu/%zu\n", i, value_count);
            }
            cursor = end;
        }

        NSData *source_data = [NSData dataWithBytes:msl length:(NSUInteger)msl_len];
        NSString *source = [[NSString alloc] initWithData:source_data encoding:NSUTF8StringEncoding];
        NSData *kernel_data = [NSData dataWithBytes:kernel length:(NSUInteger)kernel_len];
        NSString *kernel_name =
            [[NSString alloc] initWithData:kernel_data encoding:NSUTF8StringEncoding];
        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        if (source == nil || kernel_name == nil || device == nil) {
            free(values);
            free(model_text);
            return fk_out(out, cap, "FAIL metal_port source_or_device\n");
        }

        MTLCompileOptions *options = [MTLCompileOptions new];
        options.mathMode = MTLMathModeSafe;
        NSError *error = nil;
        id<MTLLibrary> library = [device newLibraryWithSource:source options:options error:&error];
        id<MTLFunction> function = library == nil ? nil : [library newFunctionWithName:kernel_name];
        id<MTLComputePipelineState> pipeline =
            function == nil ? nil : [device newComputePipelineStateWithFunction:function error:&error];
        id<MTLCommandQueue> queue = [device newCommandQueue];
        if (pipeline == nil || queue == nil) {
            const char *message = error.localizedDescription.UTF8String ?: "unknown";
            long long n = fk_out(out, cap, "FAIL metal_port compile=%s\n", message);
            free(values);
            free(model_text);
            return n;
        }

        id<MTLBuffer> weights =
            [device newBufferWithBytes:values
                                length:rows * cols * sizeof(float)
                               options:MTLResourceStorageModeShared];
        id<MTLBuffer> input =
            [device newBufferWithBytes:values + rows * cols
                                length:cols * sizeof(float)
                               options:MTLResourceStorageModeShared];
        id<MTLBuffer> output =
            [device newBufferWithLength:rows * sizeof(float)
                                options:MTLResourceStorageModeShared];
        uint32_t dims[2] = {(uint32_t)rows, (uint32_t)cols};
        id<MTLBuffer> shape = [device newBufferWithBytes:dims
                                                  length:sizeof(dims)
                                                 options:MTLResourceStorageModeShared];
        id<MTLCommandBuffer> command = [queue commandBuffer];
        id<MTLComputeCommandEncoder> encoder = [command computeCommandEncoder];
        [encoder setComputePipelineState:pipeline];
        [encoder setBuffer:weights offset:0 atIndex:0];
        [encoder setBuffer:input offset:0 atIndex:1];
        [encoder setBuffer:output offset:0 atIndex:2];
        [encoder setBuffer:shape offset:0 atIndex:3];
        NSUInteger width = MIN((NSUInteger)rows, pipeline.maxTotalThreadsPerThreadgroup);
        [encoder dispatchThreads:MTLSizeMake(rows, 1, 1)
            threadsPerThreadgroup:MTLSizeMake(width, 1, 1)];
        [encoder endEncoding];
        [command commit];
        [command waitUntilCompleted];

        if (command.status != MTLCommandBufferStatusCompleted) {
            const char *message = command.error.localizedDescription.UTF8String ?: "unknown";
            long long n = fk_out(out, cap, "FAIL metal_port dispatch=%s\n", message);
            free(values);
            free(model_text);
            return n;
        }

        float *result = output.contents;
        long long used = fk_out(
            out, cap,
            "PASS metal_port\ncarrier=in-process-metal\nhost_exec=0\nshell=0\nswift=0\ntemp=0\n"
            "rows=%zu\ncols=%zu\ndevice=%s\nresult=",
            rows, cols, device.name.UTF8String);
        for (size_t i = 0; i < rows && used < cap - 1; i++) {
            long long n = fk_out(out + used, cap - used, i == 0 ? "%.9g" : ",%.9g", result[i]);
            if (n < 0) break;
            used += n;
        }
        if (used < cap - 1) out[used++] = '\n';
        if (used < cap) out[used] = 0;
        free(values);
        free(model_text);
        return used;
    }
}
