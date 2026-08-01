// fk-metal-carrier.m — the strong symbol behind the kernel's Metal door.
//
// WHAT WAS MISSING. runtime/fkwu-uni.c has declared two Metal primitives for a long time —
// metal_matvec_fixture (tag 203) and metal_matvec_f32 (tag 204) — and form-cli-repl.fk:82 has
// been CALLING metal_matvec_f32 with (msl-text, kernel-name, model-bytes) the whole time. Both
// primitives are __attribute__((weak)) stubs that return FK_METAL_*_UNLINKED, which the kernel
// turns into "SKIP ... metal_linked=false". So the door was built, wired, and called, and no one
// ever wrote the thing on the other side of it. Every call to it has answered SKIP. This file is
// that missing side. Nothing in the kernel changes; linking this translation unit in overrides
// the weak stubs, and the SAME Form cell that printed SKIP yesterday dispatches on the GPU.
//
//   cc -O2 -o fkwu-metal runtime/fkwu-uni.c form/native/metal/fk-metal-carrier.m \
//      -framework Metal -framework Foundation -fobjc-arc
//
// WHY THIS IS NATIVE AND THAT IS NOT A COMPROMISE. Metal is reachable only through an Objective-C
// API; there is no C-only and no Form-only path to a GPU on this machine. That makes this an ORGAN,
// the same category as sense_cam_grab and sense_mic_capture, which are native in the kernel for the
// identical reason. The standing instruction this body works under is that the RECIPE must be the
// body's — no bash deciding, no python computing. An organ that opens a device is not a recipe. The
// arithmetic here is zero: this file compiles text it was handed, binds bytes it was handed, and
// dispatches a kernel it was named. Every number belongs to the MSL, and the MSL is emitted by Form.
//
// THE CONTRACT, stated here because nothing else states it. The door is three strings in, one
// string out. Byte layout of the model argument, little-endian:
//
//     u32   rows                       output length, one thread each
//     u32   cols                       shared inner dimension
//     f32   w[rows * cols]             row-major
//     f32   x[cols]                    the vector
//
// and the named kernel is called with that exact binding:
//
//     kernel void NAME(device const float *w      [[buffer(0)]],
//                      device const float *x      [[buffer(1)]],
//                      device float       *y      [[buffer(2)]],
//                      constant uint      &rows   [[buffer(3)]],
//                      constant uint      &cols   [[buffer(4)]],
//                      uint gid [[thread_position_in_grid]]);
//
// The reply is key=value lines, the shape the kernel's own SKIP text already uses, so a reader that
// handled SKIP handles this without learning a second format.
//
// WHAT THIS DOOR IS NOT. One call is one dispatch that returns TEXT, capped by the kernel at 8192
// bytes. That fits a witness — prove a Form cell can drive this GPU, and prove the number is right.
// It does NOT fit a 43-layer decode, which is ~2600 dispatches per token carrying activations that
// exceed this cap by themselves and must stay resident on the device between calls. Saying so here
// so that no one reads a working matvec as a working model. The decode loop needs a door that
// passes HANDLES rather than text; this one earns the right to ask for that door.

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#include <string.h>

#define FK_METAL_FIXTURE_UNLINKED (0 - 4611686018427387903LL)
#define FK_METAL_MATVEC_UNLINKED (0 - 4611686018427387902LL)

// One device and one pipeline cache for the process. Compiling MSL costs tens of milliseconds, and
// a caller stepping a model would otherwise pay it on every single dispatch. Keyed by the MSL text
// itself, so a cell that changes one line gets a fresh pipeline and a cell that does not, does not.
static id<MTLDevice> fk_dev = nil;
static id<MTLCommandQueue> fk_q = nil;
static NSMutableDictionary *fk_pipes = nil;

static long long fk_emit(char *out, long long cap, NSString *s) {
    const char *u = [s UTF8String];
    if (u == NULL) { return -1; }
    long long n = (long long)strlen(u);
    if (n > cap) { n = cap; }
    memcpy(out, u, (size_t)n);
    return n;
}

static int fk_metal_up(NSString **err) {
    if (fk_dev != nil) { return 1; }
    fk_dev = MTLCreateSystemDefaultDevice();
    if (fk_dev == nil) { *err = @"no Metal device"; return 0; }
    fk_q = [fk_dev newCommandQueue];
    if (fk_q == nil) { *err = @"no command queue"; return 0; }
    fk_pipes = [NSMutableDictionary dictionary];
    return 1;
}

long long fk_metal_matvec_fixture_external(char *out, long long cap) {
    @autoreleasepool {
        NSString *err = nil;
        if (!fk_metal_up(&err)) {
            return fk_emit(out, cap,
                [NSString stringWithFormat:
                    @"SKIP fkwu-form-cli-metal-direct: %@\nmetal_owner=fkwu-form-cli\nmetal_linked=false\n", err]);
        }
        return fk_emit(out, cap,
            [NSString stringWithFormat:
                @"PASS fkwu-form-cli-metal-direct\nmetal_owner=fkwu-form-cli\nmetal_linked=true\n"
                 "device=%@\nunified_memory=%d\nmax_threadgroup=%lu\n",
                [fk_dev name], (int)[fk_dev hasUnifiedMemory],
                (unsigned long)[fk_dev maxThreadsPerThreadgroup].width]);
    }
}

long long fk_metal_matvec_f32_external(const char *msl, long long msl_len,
                                       const char *kernel, long long kernel_len,
                                       const char *model, long long model_len,
                                       char *out, long long cap) {
    @autoreleasepool {
        NSString *err = nil;
        if (!fk_metal_up(&err)) {
            return fk_emit(out, cap,
                [NSString stringWithFormat:
                    @"SKIP fkwu-form-cli-metal-matvec-f32: %@\nmetal_owner=fkwu-form-cli\nmetal_linked=false\n", err]);
        }

        // The header must be present before it is trusted. A truncated model argument that we read
        // anyway would produce a plausible number from garbage, which is the failure this body calls
        // a numb green: right-shaped, wrong-meaning, and silent.
        if (model_len < 8) {
            return fk_emit(out, cap, @"FAIL fkwu-form-cli-metal-matvec-f32 model shorter than its 8-byte header\n");
        }
        uint32_t rows = 0, cols = 0;
        memcpy(&rows, model, 4);
        memcpy(&cols, model + 4, 4);
        if (rows == 0 || cols == 0) {
            return fk_emit(out, cap, @"FAIL fkwu-form-cli-metal-matvec-f32 rows or cols is zero\n");
        }
        long long need = 8 + (long long)rows * (long long)cols * 4 + (long long)cols * 4;
        if (model_len < need) {
            return fk_emit(out, cap,
                [NSString stringWithFormat:
                    @"FAIL fkwu-form-cli-metal-matvec-f32 model is %lld bytes, rows=%u cols=%u needs %lld\n",
                    model_len, rows, cols, need]);
        }

        NSString *src = [[NSString alloc] initWithBytes:msl length:(NSUInteger)msl_len
                                               encoding:NSUTF8StringEncoding];
        NSString *fn = [[NSString alloc] initWithBytes:kernel length:(NSUInteger)kernel_len
                                              encoding:NSUTF8StringEncoding];
        if (src == nil || fn == nil) {
            return fk_emit(out, cap, @"FAIL fkwu-form-cli-metal-matvec-f32 msl or kernel name is not UTF-8\n");
        }

        NSString *key = [NSString stringWithFormat:@"%@\n%@", fn, src];
        id<MTLComputePipelineState> pipe = fk_pipes[key];
        if (pipe == nil) {
            NSError *e = nil;
            id<MTLLibrary> lib = [fk_dev newLibraryWithSource:src options:nil error:&e];
            if (lib == nil) {
                // The compiler's own words, not a summary of them. A swallowed MSL diagnostic is how
                // a kernel that silently value-initialises an operand to zero gets shipped.
                return fk_emit(out, cap,
                    [NSString stringWithFormat:@"FAIL fkwu-form-cli-metal-matvec-f32 msl compile\n%@\n",
                        [e localizedDescription]]);
            }
            id<MTLFunction> f = [lib newFunctionWithName:fn];
            if (f == nil) {
                return fk_emit(out, cap,
                    [NSString stringWithFormat:
                        @"FAIL fkwu-form-cli-metal-matvec-f32 msl has no kernel named %@\n", fn]);
            }
            pipe = [fk_dev newComputePipelineStateWithFunction:f error:&e];
            if (pipe == nil) {
                return fk_emit(out, cap,
                    [NSString stringWithFormat:@"FAIL fkwu-form-cli-metal-matvec-f32 pipeline\n%@\n",
                        [e localizedDescription]]);
            }
            fk_pipes[key] = pipe;
        }

        const char *wp = model + 8;
        const char *xp = model + 8 + (long long)rows * (long long)cols * 4;
        id<MTLBuffer> wb = [fk_dev newBufferWithBytes:wp
                                               length:(NSUInteger)rows * cols * 4
                                              options:MTLResourceStorageModeShared];
        id<MTLBuffer> xb = [fk_dev newBufferWithBytes:xp
                                               length:(NSUInteger)cols * 4
                                              options:MTLResourceStorageModeShared];
        id<MTLBuffer> yb = [fk_dev newBufferWithLength:(NSUInteger)rows * 4
                                               options:MTLResourceStorageModeShared];
        if (wb == nil || xb == nil || yb == nil) {
            return fk_emit(out, cap, @"FAIL fkwu-form-cli-metal-matvec-f32 buffer allocation\n");
        }

        id<MTLCommandBuffer> cb = [fk_q commandBuffer];
        id<MTLComputeCommandEncoder> enc = [cb computeCommandEncoder];
        [enc setComputePipelineState:pipe];
        [enc setBuffer:wb offset:0 atIndex:0];
        [enc setBuffer:xb offset:0 atIndex:1];
        [enc setBuffer:yb offset:0 atIndex:2];
        [enc setBytes:&rows length:4 atIndex:3];
        [enc setBytes:&cols length:4 atIndex:4];
        NSUInteger tg = [pipe maxTotalThreadsPerThreadgroup];
        if (tg > rows) { tg = rows; }
        if (tg == 0) { tg = 1; }
        [enc dispatchThreads:MTLSizeMake(rows, 1, 1) threadsPerThreadgroup:MTLSizeMake(tg, 1, 1)];
        [enc endEncoding];
        [cb commit];
        [cb waitUntilCompleted];

        if ([cb error] != nil) {
            return fk_emit(out, cap,
                [NSString stringWithFormat:@"FAIL fkwu-form-cli-metal-matvec-f32 dispatch\n%@\n",
                    [[cb error] localizedDescription]]);
        }

        // The sum travels alongside the elements so a caller who cannot fit rows into the 8192-byte
        // cap still holds one number that changes when any element changes.
        const float *y = (const float *)[yb contents];
        double sum = 0.0;
        for (uint32_t i = 0; i < rows; i++) { sum += (double)y[i]; }

        NSMutableString *r = [NSMutableString string];
        [r appendString:@"PASS fkwu-form-cli-metal-matvec-f32\n"];
        [r appendString:@"metal_owner=fkwu-form-cli\nmetal_linked=true\n"];
        [r appendFormat:@"device=%@\nkernel=%@\n", [fk_dev name], fn];
        [r appendFormat:@"rows=%u\ncols=%u\n", rows, cols];
        [r appendFormat:@"sum=%.9g\n", sum];
        [r appendString:@"y="];
        for (uint32_t i = 0; i < rows; i++) {
            if ([r length] > (NSUInteger)(cap - 64)) { [r appendString:@" ..."]; break; }
            [r appendFormat:@"%s%.9g", i ? " " : "", (double)y[i]];
        }
        [r appendString:@"\n"];
        return fk_emit(out, cap, r);
    }
}
