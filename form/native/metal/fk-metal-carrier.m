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
//   cc -O2 -o fkwu runtime/fkwu-uni.c form/native/metal/fk-metal-carrier.m \
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

// ─────────────────────────────────────────────────────────────────────────────
// THE HANDLE DOOR (added 2026-08-04), and the measurement that shaped it.
//
// The matvec door below this is one call = one compile + one copy-in + one
// dispatch + one BLOCK + one text reply. Timing it against the empty seam said
// where the cost actually lives:
//
//     2000 metal_matvec_fixture calls (seam, no dispatch)  ->  ~16 us/call
//     2000 metal_matvec_f32   calls (dispatch + WAIT)      -> ~128 us/call
//
// The Form->native crossing is not the expense. `waitUntilCompleted` is. At the
// ~2600 dispatches a 43-layer token costs, the seam is affordable and the waiting
// is nine times the whole budget. That inverts the design: a Form cell CAN hold a
// decode loop. What it cannot do is wait. So the four things this door adds are
// exactly the four things that removes:
//
//   1. weights mmap'd once and RESIDENT      metal_buf_from_file  (no per-call copy)
//   2. buffers that PERSIST across calls     metal_buf_alloc      (handles, not strings)
//   3. dispatch that ENQUEUES and returns    metal_enqueue        (no commit, no wait)
//   4. ONE sync per token                    metal_sync           (commit + wait, once)
//
// plus metal_pipeline (compile once, so a dispatch carries no text), metal_buf_write
// (Form-emitted bytes in), metal_buf_read (bytes out, and it syncs first), and
// metal_status (the voice canary — see below).
//
// WHY metal_status EXISTS AND IS NOT DECORATION. Every other primitive here returns
// an integer handle, and an unbound name under axiom-5 recovers to NOTHING and
// answers 0. A 0 handle from a door that was never linked and a 0 handle from a
// door that refused are the same shape, and so is a 0 from a typo. metal_status
// speaks — it is the one primitive whose answer cannot be counterfeited by a numb
// call, and the band gates on it before it believes any other number.
//
// THE BINDING STRING, emitted by the Form cell, little-endian u32 throughout:
//
//     u32  n_buf
//     u32  handle[n_buf]     bound at buffer(0 .. n_buf-1), at that handle's view offset
//     u32  n_const
//     u32  value[n_const]    setBytes, 4 bytes each, at buffer(n_buf .. n_buf+n_const-1)
//   [ u32  threads_per_group   0 = free: min(maxTotalThreadsPerThreadgroup, threads) ]
//   [ u32  threadgroup_bytes   0 = none; else setThreadgroupMemoryLength at index 0  ]
//   [ u32  dispatch_mode       0 = dispatchThreads(threads); 1 = dispatchThreadgroups:]
//   [                          the op's `threads` argument is the GROUP COUNT and    ]
//   [                          threads_per_group is required nonzero                  ]
//   [ u32  barrier_before      1 = memoryBarrier(buffers) before this dispatch;       ]
//   [                          legal only in a concurrent batch                       ]
//
// The tail fields are PAIRS and arrive whole or not at all. Exactly three lengths
// are legal — base, base+8, base+16 — and anything else is refused out loud rather
// than read as far as it parses. That is why they are not an open-ended optional
// tail: a binding one field short would otherwise bind a handle as a constant and
// dispatch something plausible.
//
// dispatch_mode 1 exists because cooperative kernels — group-wide folds staged
// through threadgroup memory — are shaped as "G groups of exactly T threads", and
// dispatchThreads cannot promise a full group at the tail of the grid. With mode 1
// threads_per_group is not clamped-if-large but REFUSED past the pipeline's
// maxTotalThreadsPerThreadgroup, because silently shrinking T repartitions the
// very fold the mode exists for.
//
// WHY THESE TWO LIVE HERE AND NOT AS ARGUMENTS. Past arity 3 this kernel hands an
// op a CONS LIST in slot 1 instead of separate node slots (make_nodeid, tag 91,
// fkwu-uni.c:6920), because fk_node rows are four wide. metal_enqueue is the one
// primitive a decode loop calls ~2600 times per token; making it walk a cons chain
// to learn a threadgroup size is the wrong trade. The Form cell already decides
// this string's layout, so per-dispatch shape belongs in it.
//
// threads_per_group MIRRORS THE ORACLE, which takes a per-dispatch cap independent
// of the grid: metal_dsv4_stack.sh's enc(pipeline, n, cap) dispatches n threads with
// min(maxTotalThreadsPerThreadgroup, cap) per group — enc(pQ80, rows*32, 256),
// enc(pQ8aQuant, blocks*q8aThreads, 64), and so on. A kernel that does a group-wide
// fold or stages through threadgroup memory partitions differently under a different
// cap, so a door that cannot express the cap cannot reproduce the oracle. 0 keeps the
// door's own earlier behaviour and is NOT clamped to `threads` when a cap is given,
// because the oracle does not clamp either.
//
// FP CONTRACTION: THIS DOOR CONTRACTS, AND THE ORACLE DOES NOT.
// metal_pipeline compiles with newLibraryWithSource:options:nil, and that default
// FUSES a multiply followed by an add into an fma. Measured with a = b = 1 + 2^-12
// (0x3F800800), c = -1.0, where the two genuinely differ:
//
//     through this door as compiled:  a*b + c = 973079552  ( == fma(a,b,c) )
//     without contraction:            a*b + c = 973078528
//
// metal_dsv4_stack.sh builds every metallib with -ffp-contract=off, and
// ds4-order-match.fk rests its whole bit-exactness claim on that. So a caller who
// ports a kernel here unchanged gets plausible-but-divergent numbers with nothing
// failing anywhere. The FIX BELONGS TO THE FORM CELL, not to this file: emit
//
//     #pragma clang fp contract(off)
//
// into the MSL text and the fusion stops (verified: a*b+c = 973078528 while an
// explicit fma(a,b,c) still gives 973079552, correctly different). `#pragma METAL fp
// math_mode(safe)` does NOT stop it. The door does not set this for the caller,
// for the same reason it computes no arithmetic: the cell owns every number, and a
// door that silently picked a rounding policy would own one.
//
// The Form cell decides this layout the same way it decides the MSL. Nothing here
// knows what a weight is, what a layer is, or what any index means.
// ─────────────────────────────────────────────────────────────────────────────

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define FK_METAL_FIXTURE_UNLINKED (0 - 4611686018427387903LL)
#define FK_METAL_MATVEC_UNLINKED (0 - 4611686018427387902LL)
#define FK_METAL_HANDLE_UNLINKED (0 - 4611686018427387901LL)

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

// ═════════════════════════════════════════════════════════════════════════════
// THE HANDLE DOOR — resident buffers, batched enqueue, one sync.
// ═════════════════════════════════════════════════════════════════════════════

// Caps are stated, not silent. A 43-layer DS4 needs ~400 weight buffers plus
// activations; 8192 is comfortable and the wall, when reached, is spoken through
// metal_status rather than answered as a plausible handle.
#define FK_MAX_BUF 8192
#define FK_MAX_PIPE 512

typedef struct {
    void *map_base;        // non-NULL when this buffer is an mmap of a file
    size_t map_len;
    unsigned long long view_off;  // where the caller's region starts inside the MTLBuffer
    unsigned long long view_len;  // how much of it is the caller's
    int nocopy;            // 1 = the file's own pages, 0 = a copy we made
} fk_bufmeta;

// A buffer handle is (generation << 16) | slot, slot 1-based. The generation is
// what lets metal_buf_free exist without inviting the worst failure a handle
// system has: a freed slot reused by a NEW buffer, silently read through an OLD
// handle a Form cell is still holding. Freeing bumps the slot's generation, so
// every outstanding handle to it stops matching and is refused at the door rather
// than answering a stranger's bytes. Generation 0 makes every pre-free handle
// value numerically identical to the plain slot index, so nothing that held a
// handle before this scheme existed sees a different number. The generation wraps
// at 16 bits; a slot freed and reused 65536 times could in principle revive one
// ancient stale handle, and that is stated here rather than discovered.
static NSMutableArray *fk_buf_objs = nil;   // slot s -> element s-1 (NSNull when freed)
static fk_bufmeta fk_buf_meta[FK_MAX_BUF];
static unsigned int fk_buf_gen[FK_MAX_BUF];
static long long fk_free_slots[FK_MAX_BUF];
static long long fk_free_top = 0;
static NSMutableArray *fk_pipe_objs = nil;  // handle h -> element h-1
static NSMutableDictionary *fk_pipe_by_key = nil;  // msl+name -> NSNumber handle
static NSMutableDictionary *fk_lib_by_src = nil;   // msl -> compiled MTLLibrary

// The open batch. One command buffer and one compute encoder stay open across
// enqueues, so N dispatches become ONE GPU submission at sync. Within a single
// MTLComputeCommandEncoder the default MTLDispatchTypeSerial orders dispatches
// and makes each one's writes visible to the next — which is what lets an
// intermediate activation stay on the device between two enqueues.
static id<MTLCommandBuffer> fk_cb = nil;
static id<MTLComputeCommandEncoder> fk_enc = nil;
static long long fk_pending = 0;        // enqueued since the last sync
static long long fk_total_dispatch = 0;
static long long fk_total_sync = 0;
static long long fk_nocopy_bufs = 0;
static NSString *fk_last_err = nil;

// Concurrent batches are OPT-IN, per batch, and the flag arms only the NEXT batch:
// metal_batch_concurrent before the first enqueue, back to serial when that batch
// ends. Within a concurrent batch nothing orders two dispatches unless the cell
// says so through barrier_before — this door does not guess hazards, because a
// door that guessed would own correctness, and this one owns none.
static int fk_batch_concurrent = 0;   // the OPEN batch's mode
static int fk_next_concurrent = 0;    // armed for the batch about to open

// Submitted-but-not-waited batches, for double-buffering: metal_submit commits and
// answers a fence id, metal_fence_wait drains that one batch. metal_sync and
// metal_buf_read drain everything, so the old "read is the sync point" contract
// still holds with fences in flight.
static NSMutableDictionary *fk_inflight = nil;    // @(fence) -> MTLCommandBuffer
static NSMutableDictionary *fk_inflight_n = nil;  // @(fence) -> @(dispatch count)
static long long fk_fence_next = 1;

static void fk_err(NSString *s) { fk_last_err = s; }

static unsigned int fk_le32(const unsigned char *p) {
    return (unsigned int)p[0] | ((unsigned int)p[1] << 8) |
           ((unsigned int)p[2] << 16) | ((unsigned int)p[3] << 24);
}

// Handle validity is checked on every use. A handle is a claim the Form cell is
// holding; if a cell hands back a stale, freed, or fabricated one, the honest
// answer is a refusal, not a read of whatever buffer happens to sit at that slot
// now. Returns the 1-based SLOT (the index into fk_buf_meta), 0 when the claim
// does not hold.
static long long fk_buf_slot(long long h) {
    long long slot = h & 0xFFFF;
    long long gen = h >> 16;
    if (fk_buf_objs == nil) { return 0; }
    if (slot < 1 || slot > (long long)[fk_buf_objs count]) { return 0; }
    if (gen < 0 || gen != (long long)fk_buf_gen[slot - 1]) { return 0; }
    if (fk_buf_objs[(NSUInteger)(slot - 1)] == [NSNull null]) { return 0; }
    return slot;
}
static id<MTLBuffer> fk_buf_at(long long h) {
    long long slot = fk_buf_slot(h);
    if (slot == 0) { return nil; }
    return fk_buf_objs[(NSUInteger)(slot - 1)];
}

long long fk_metal_pipeline_external(const char *msl, long long msl_len,
                                     const char *name, long long name_len) {
    @autoreleasepool {
        NSString *err = nil;
        if (!fk_metal_up(&err)) { fk_err(err); return 0; }
        if (fk_pipe_objs == nil) {
            fk_pipe_objs = [NSMutableArray array];
            fk_pipe_by_key = [NSMutableDictionary dictionary];
            fk_lib_by_src = [NSMutableDictionary dictionary];
        }
        NSString *src = [[NSString alloc] initWithBytes:msl length:(NSUInteger)msl_len
                                               encoding:NSUTF8StringEncoding];
        NSString *fn = [[NSString alloc] initWithBytes:name length:(NSUInteger)name_len
                                              encoding:NSUTF8StringEncoding];
        if (src == nil || fn == nil) { fk_err(@"msl or kernel name is not UTF-8"); return 0; }

        // Compile once. This is requirement 3's other half: if the pipeline were
        // rebuilt per dispatch, enqueue could never be cheap no matter how little
        // it waited. Same text + same name = same handle, always.
        NSString *key = [NSString stringWithFormat:@"%@\n%@", fn, src];
        NSNumber *have = fk_pipe_by_key[key];
        if (have != nil) { return [have longLongValue]; }

        if ([fk_pipe_objs count] >= FK_MAX_PIPE) {
            fk_err([NSString stringWithFormat:@"pipeline table full at %d", FK_MAX_PIPE]);
            return 0;
        }
        NSError *e = nil;
        // One MSL unit commonly carries several kernels.  The Form stack asks
        // for one pipeline handle per function, but recompiling identical
        // source for every function paid the Metal compiler 41 times during
        // setup.  Compile the unit once, then specialize all named pipelines
        // from that persistent library.  Pipeline handles remain keyed by
        // source+function and retain exactly the same Form-visible identity.
        id<MTLLibrary> lib = fk_lib_by_src[src];
        if (lib == nil) {
            lib = [fk_dev newLibraryWithSource:src options:nil error:&e];
            if (lib == nil) {
                // The compiler's own words, kept whole. metal_status speaks them.
                fk_err([NSString stringWithFormat:@"msl compile: %@", [e localizedDescription]]);
                return 0;
            }
            fk_lib_by_src[src] = lib;
        }
        id<MTLFunction> f = [lib newFunctionWithName:fn];
        if (f == nil) {
            fk_err([NSString stringWithFormat:@"msl has no kernel named %@", fn]);
            return 0;
        }
        id<MTLComputePipelineState> pipe = [fk_dev newComputePipelineStateWithFunction:f error:&e];
        if (pipe == nil) {
            fk_err([NSString stringWithFormat:@"pipeline: %@", [e localizedDescription]]);
            return 0;
        }
        [fk_pipe_objs addObject:pipe];
        long long h = (long long)[fk_pipe_objs count];
        fk_pipe_by_key[key] = @(h);
        return h;
    }
}

static long long fk_buf_register(id<MTLBuffer> b, void *map_base, size_t map_len,
                                 unsigned long long view_off, unsigned long long view_len,
                                 int nocopy) {
    long long slot;
    if (fk_free_top > 0) {
        fk_free_top--;
        slot = fk_free_slots[fk_free_top];
        fk_buf_objs[(NSUInteger)(slot - 1)] = b;
    } else {
        if ([fk_buf_objs count] >= FK_MAX_BUF) {
            fk_err([NSString stringWithFormat:@"buffer table full at %d live slots", FK_MAX_BUF]);
            return 0;
        }
        [fk_buf_objs addObject:b];
        slot = (long long)[fk_buf_objs count];
    }
    fk_buf_meta[slot - 1].map_base = map_base;
    fk_buf_meta[slot - 1].map_len = map_len;
    fk_buf_meta[slot - 1].view_off = view_off;
    fk_buf_meta[slot - 1].view_len = view_len;
    fk_buf_meta[slot - 1].nocopy = nocopy;
    if (nocopy) { fk_nocopy_bufs++; }
    return ((long long)fk_buf_gen[slot - 1] << 16) | slot;
}

long long fk_metal_buf_alloc_external(long long nbytes) {
    @autoreleasepool {
        NSString *err = nil;
        if (!fk_metal_up(&err)) { fk_err(err); return 0; }
        if (fk_buf_objs == nil) { fk_buf_objs = [NSMutableArray array]; }
        if (nbytes <= 0) { fk_err(@"buf_alloc needs a positive length"); return 0; }
        id<MTLBuffer> b = [fk_dev newBufferWithLength:(NSUInteger)nbytes
                                              options:MTLResourceStorageModeShared];
        if (b == nil) { fk_err(@"buffer allocation failed"); return 0; }
        memset([b contents], 0, (size_t)nbytes);
        return fk_buf_register(b, NULL, 0, 0, (unsigned long long)nbytes, 0);
    }
}

// Requirement 1. The ONLY host crossing this door permits is reading the model
// file's bytes, and it does that without copying them: the file's own page-cache
// pages become the GPU's buffer. At 9.1 GB the difference between mapping and
// copying is not a speed difference, it is whether the machine can hold the model
// at all.
long long fk_metal_buf_from_file_external(const char *path, long long path_len,
                                          long long off, long long len) {
    @autoreleasepool {
        NSString *err = nil;
        if (!fk_metal_up(&err)) { fk_err(err); return 0; }
        if (fk_buf_objs == nil) { fk_buf_objs = [NSMutableArray array]; }
        if (off < 0 || len <= 0) { fk_err(@"buf_from_file needs off>=0 and len>0"); return 0; }

        char p[4096];
        if (path_len <= 0 || path_len >= (long long)sizeof(p)) {
            fk_err(@"buf_from_file path length out of range"); return 0;
        }
        memcpy(p, path, (size_t)path_len);
        p[path_len] = 0;

        int fd = open(p, O_RDONLY);
        if (fd < 0) { fk_err([NSString stringWithFormat:@"cannot open %s", p]); return 0; }
        struct stat st;
        if (fstat(fd, &st) != 0) { close(fd); fk_err(@"fstat failed"); return 0; }
        // Refuse a region the file does not contain, rather than mapping short and
        // handing back a buffer whose tail is whatever the kernel zero-fills. A
        // model file truncated by a bad download would otherwise decode to noise.
        if (off + len > (long long)st.st_size) {
            fk_err([NSString stringWithFormat:@"file is %lld bytes, asked for [%lld,%lld)",
                    (long long)st.st_size, off, off + len]);
            close(fd);
            return 0;
        }
        long long pg = (long long)getpagesize();
        long long base = off - (off % pg);
        long long span = (off - base) + len;
        if (span % pg) { span = span + (pg - (span % pg)); }
        if (base + span > (long long)st.st_size) { span = (long long)st.st_size - base; }

        // MAP_PRIVATE keeps the file untouched; PROT_WRITE is copy-on-write and is
        // asked for only because Metal wants a writable mapping to wrap. No page is
        // actually copied unless something writes to it, and nothing here does.
        void *m = mmap(NULL, (size_t)span, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, (off_t)base);
        close(fd);
        if (m == MAP_FAILED) { fk_err(@"mmap failed"); return 0; }

        id<MTLBuffer> b = [fk_dev newBufferWithBytesNoCopy:m
                                                    length:(NSUInteger)span
                                                   options:MTLResourceStorageModeShared
                                               deallocator:nil];
        if (b != nil) {
            return fk_buf_register(b, m, (size_t)span, (unsigned long long)(off - base),
                                   (unsigned long long)len, 1);
        }
        // Fallback, and it is RECORDED, not silent: a copy still gives a correct
        // answer but not the machine requirement 1 asked for, and metal_status has
        // to be able to say which one this process is running.
        fk_err(@"newBufferWithBytesNoCopy refused the mapping; fell back to a copy");
        b = [fk_dev newBufferWithBytes:((char *)m + (off - base))
                                length:(NSUInteger)len
                               options:MTLResourceStorageModeShared];
        munmap(m, (size_t)span);
        if (b == nil) { fk_err(@"buf_from_file: both nocopy and copy failed"); return 0; }
        return fk_buf_register(b, NULL, 0, 0, (unsigned long long)len, 0);
    }
}

long long fk_metal_buf_write_external(long long h, long long off, const char *bytes, long long len) {
    @autoreleasepool {
        long long slot = fk_buf_slot(h);
        if (slot == 0) { fk_err([NSString stringWithFormat:@"buf_write: bad handle %lld", h]); return -1; }
        id<MTLBuffer> b = fk_buf_objs[(NSUInteger)(slot - 1)];
        if (off < 0 || len < 0) { fk_err(@"buf_write: negative off or len"); return -1; }
        fk_bufmeta *m = &fk_buf_meta[slot - 1];
        if ((unsigned long long)(off + len) > m->view_len) {
            fk_err([NSString stringWithFormat:@"buf_write: [%lld,%lld) past view_len %llu",
                    off, off + len, m->view_len]);
            return -1;
        }
        memcpy((char *)[b contents] + m->view_off + off, bytes, (size_t)len);
        return len;
    }
}

// Requirement 3. This is the hot primitive — it is what a decode loop calls ~2600
// times per token — so it does exactly three things: bind, dispatch, return. No
// compile (240 did that), no allocation (241/242 did that), no commit and above
// all no wait. The command buffer stays open and accumulates.
long long fk_metal_enqueue_external(long long pipe, const char *binding,
                                    long long binding_len, long long threads) {
    @autoreleasepool {
        NSString *err = nil;
        if (!fk_metal_up(&err)) { fk_err(err); return -1; }
        if (pipe < 1 || pipe > (long long)[fk_pipe_objs count]) {
            fk_err([NSString stringWithFormat:@"enqueue: bad pipeline handle %lld", pipe]);
            return -1;
        }
        if (threads <= 0) { fk_err(@"enqueue: threads must be positive"); return -1; }
        const unsigned char *bp = (const unsigned char *)binding;
        if (binding_len < 4) { fk_err(@"enqueue: binding shorter than its first count"); return -1; }
        unsigned int nbuf = fk_le32(bp);
        if (binding_len < (long long)(4 + 4 * nbuf + 4)) {
            fk_err(@"enqueue: binding truncated before its constant count"); return -1;
        }
        unsigned int nconst = fk_le32(bp + 4 + 4 * nbuf);
        long long base = (long long)(4 + 4 * nbuf + 4 + 4 * nconst);
        // Exactly three legal lengths. A binding that is merely "long enough" is not
        // accepted: a short one would silently read a handle as a constant, and a
        // long one means the cell and this door disagree about the layout.
        unsigned int tpg = 0;
        unsigned int tgmem = 0;
        unsigned int mode = 0;
        unsigned int barrier = 0;
        if (binding_len == base + 8 || binding_len == base + 16) {
            tpg = fk_le32(bp + base);
            tgmem = fk_le32(bp + base + 4);
            if (binding_len == base + 16) {
                mode = fk_le32(bp + base + 8);
                barrier = fk_le32(bp + base + 12);
            }
        } else if (binding_len != base) {
            fk_err([NSString stringWithFormat:
                @"enqueue: binding is %lld bytes; n_buf=%u n_const=%u makes %lld (no tail), "
                 "%lld (threads_per_group + threadgroup_bytes), or %lld (+ dispatch_mode + barrier_before)",
                binding_len, nbuf, nconst, base, base + 8, base + 16]);
            return -1;
        }
        // Everything about the tail is judged BEFORE a batch opens, so a refused
        // enqueue leaves no half-open state behind it. The batch this dispatch would
        // join is either the open one or the one the armed flag describes.
        int concurrent = (fk_cb != nil) ? fk_batch_concurrent : fk_next_concurrent;
        if (mode > 1) {
            fk_err([NSString stringWithFormat:@"enqueue: dispatch_mode %u is not 0 or 1", mode]);
            return -1;
        }
        if (barrier > 1) {
            fk_err([NSString stringWithFormat:@"enqueue: barrier_before %u is not 0 or 1", barrier]);
            return -1;
        }
        if (mode == 1 && tpg == 0) {
            fk_err(@"enqueue: dispatch_mode 1 (threadgroups) needs threads_per_group nonzero");
            return -1;
        }
        if (barrier == 1 && !concurrent) {
            // In a serial batch every dispatch already sees its predecessors, so a
            // barrier here is not harmless noise — it means the cell believes it is
            // in a concurrent batch and is not, and that belief will place hazards.
            fk_err(@"enqueue: barrier_before is only legal in a concurrent batch");
            return -1;
        }

        if (fk_cb == nil) {
            fk_cb = [fk_q commandBuffer];
            fk_batch_concurrent = fk_next_concurrent;
            fk_next_concurrent = 0;
            fk_enc = [fk_cb computeCommandEncoderWithDispatchType:
                (fk_batch_concurrent ? MTLDispatchTypeConcurrent : MTLDispatchTypeSerial)];
            if (fk_cb == nil || fk_enc == nil) { fk_err(@"could not open a command buffer"); return -1; }
        }
        if (barrier == 1) { [fk_enc memoryBarrierWithScope:MTLBarrierScopeBuffers]; }
        id<MTLComputePipelineState> ps = fk_pipe_objs[(NSUInteger)(pipe - 1)];
        [fk_enc setComputePipelineState:ps];
        for (unsigned int k = 0; k < nbuf; k++) {
            long long bh = (long long)fk_le32(bp + 4 + 4 * k);
            long long bslot = fk_buf_slot(bh);
            if (bslot == 0) {
                fk_err([NSString stringWithFormat:@"enqueue: bad buffer handle %lld at slot %u", bh, k]);
                return -1;
            }
            [fk_enc setBuffer:fk_buf_objs[(NSUInteger)(bslot - 1)]
                       offset:(NSUInteger)fk_buf_meta[bslot - 1].view_off
                      atIndex:k];
        }
        for (unsigned int k = 0; k < nconst; k++) {
            unsigned int v = fk_le32(bp + 4 + 4 * nbuf + 4 + 4 * k);
            [fk_enc setBytes:&v length:4 atIndex:(NSUInteger)(nbuf + k)];
        }
        if (tgmem > 0) { [fk_enc setThreadgroupMemoryLength:(NSUInteger)tgmem atIndex:0]; }
        NSUInteger tgmax = [ps maxTotalThreadsPerThreadgroup];
        if (mode == 1) {
            // Cooperative shape: the op's threads argument is the GROUP count and tpg
            // is exact. Past the pipeline's ceiling it is refused, not clamped —
            // silently shrinking T repartitions the very fold this mode exists for.
            if ((NSUInteger)tpg > tgmax) {
                fk_err([NSString stringWithFormat:
                    @"enqueue: threads_per_group %u past this pipeline's max %lu",
                    tpg, (unsigned long)tgmax]);
                return -1;
            }
            [fk_enc dispatchThreadgroups:MTLSizeMake((NSUInteger)threads, 1, 1)
                   threadsPerThreadgroup:MTLSizeMake((NSUInteger)tpg, 1, 1)];
        } else {
            // With a cap the oracle's rule exactly: min(maxTotalThreadsPerThreadgroup,
            // cap), and NOT clamped down to the grid — a partial trailing group is what
            // dispatchThreads is for, and clamping would repartition a group-wide fold.
            // Without one, the door's own earlier behaviour, unchanged.
            NSUInteger tg;
            if (tpg > 0) {
                tg = (NSUInteger)tpg;
                if (tg > tgmax) { tg = tgmax; }
            } else {
                tg = tgmax;
                if (tg > (NSUInteger)threads) { tg = (NSUInteger)threads; }
            }
            if (tg == 0) { tg = 1; }
            [fk_enc dispatchThreads:MTLSizeMake((NSUInteger)threads, 1, 1)
              threadsPerThreadgroup:MTLSizeMake(tg, 1, 1)];
        }
        fk_pending++;
        fk_total_dispatch++;
        return 1;   // enqueued. NOT completed, and this door never pretends otherwise.
    }
}

// Requirement 4. The whole batch becomes one submission and one wait. Returns how
// many dispatches were drained, so a caller can prove the enqueues were still
// outstanding — that number IS the evidence that enqueue did not block.
long long fk_metal_sync_external(void) {
    @autoreleasepool {
        long long n = 0;
        int had_work = 0;
        // Everything in flight drains first — a fence the caller stopped tracking
        // still completes here, so sync remains the one word that means "the GPU
        // holds nothing of ours".
        if (fk_inflight != nil && [fk_inflight count] > 0) {
            for (NSNumber *key in [fk_inflight allKeys]) {
                id<MTLCommandBuffer> cb = fk_inflight[key];
                [cb waitUntilCompleted];
                if ([cb error] != nil) {
                    fk_err([NSString stringWithFormat:@"dispatch: %@",
                            [[cb error] localizedDescription]]);
                    return -1;
                }
                n += [fk_inflight_n[key] longLongValue];
            }
            [fk_inflight removeAllObjects];
            [fk_inflight_n removeAllObjects];
            had_work = 1;
        }
        if (fk_cb != nil) {
            n += fk_pending;
            [fk_enc endEncoding];
            [fk_cb commit];
            [fk_cb waitUntilCompleted];
            if ([fk_cb error] != nil) {
                fk_err([NSString stringWithFormat:@"dispatch: %@",
                        [[fk_cb error] localizedDescription]]);
                n = -1;
            }
            fk_enc = nil;
            fk_cb = nil;
            fk_pending = 0;
            fk_batch_concurrent = 0;
            had_work = 1;
        }
        if (had_work) { fk_total_sync++; }
        return n;
    }
}

// Reading is the sync point by construction: anything still enqueued may be what
// produces these very bytes, so it drains first. This is the "one sync per token,
// when a token id is read back" of requirement 4, made structural instead of
// remembered.
long long fk_metal_buf_read_external(long long h, long long off, long long len,
                                     char *out, long long cap) {
    @autoreleasepool {
        if (fk_cb != nil || (fk_inflight != nil && [fk_inflight count] > 0)) {
            if (fk_metal_sync_external() < 0) { return -1; }
        }
        long long slot = fk_buf_slot(h);
        if (slot == 0) { fk_err([NSString stringWithFormat:@"buf_read: bad handle %lld", h]); return -1; }
        id<MTLBuffer> b = fk_buf_objs[(NSUInteger)(slot - 1)];
        if (off < 0 || len < 0 || len > cap) { fk_err(@"buf_read: off/len out of range"); return -1; }
        fk_bufmeta *m = &fk_buf_meta[slot - 1];
        if ((unsigned long long)(off + len) > m->view_len) {
            fk_err([NSString stringWithFormat:@"buf_read: [%lld,%lld) past view_len %llu",
                    off, off + len, m->view_len]);
            return -1;
        }
        memcpy(out, (const char *)[b contents] + m->view_off + off, (size_t)len);
        return len;   // all of it, or the error above. Never a short read.
    }
}

long long fk_metal_batch_concurrent_external(void) {
    @autoreleasepool {
        NSString *err = nil;
        if (!fk_metal_up(&err)) { fk_err(err); return 0; }
        // Arming is refused while a batch is open: switching an encoder's dispatch
        // type mid-batch is not a thing Metal has, and pretending by silently
        // deferring the flag would make the NEXT batch concurrent long after the
        // cell stopped meaning it.
        if (fk_cb != nil) {
            fk_err(@"batch_concurrent: a batch is already open; sync or submit it first");
            return 0;
        }
        fk_next_concurrent = 1;
        return 1;
    }
}

long long fk_metal_buf_free_external(long long h) {
    @autoreleasepool {
        long long slot = fk_buf_slot(h);
        if (slot == 0) {
            fk_err([NSString stringWithFormat:@"buf_free: bad handle %lld", h]);
            return -1;
        }
        // Only at a quiescent point. For a plain buffer Metal would keep the object
        // alive through any command buffer that references it, but an mmap'd one is
        // OUR mapping under Metal's object — munmap while a dispatch reads it is a
        // fault Metal cannot see coming. One rule for both kinds, stated: free when
        // nothing is open and nothing is in flight.
        if (fk_cb != nil || (fk_inflight != nil && [fk_inflight count] > 0)) {
            fk_err(@"buf_free: work is open or in flight; sync first");
            return -1;
        }
        fk_bufmeta *m = &fk_buf_meta[slot - 1];
        if (m->map_base != NULL) { munmap(m->map_base, m->map_len); }
        if (m->nocopy) { fk_nocopy_bufs--; }
        m->map_base = NULL;
        m->map_len = 0;
        m->view_off = 0;
        m->view_len = 0;
        m->nocopy = 0;
        fk_buf_objs[(NSUInteger)(slot - 1)] = [NSNull null];
        fk_buf_gen[slot - 1] = (fk_buf_gen[slot - 1] + 1) & 0xFFFF;
        fk_free_slots[fk_free_top] = slot;
        fk_free_top++;
        return 1;
    }
}

long long fk_metal_submit_external(void) {
    @autoreleasepool {
        if (fk_cb == nil) {
            fk_err(@"submit: no open batch");
            return 0;
        }
        if (fk_pending == 0) {
            // An empty batch earns no fence — a caller waiting on it would learn
            // nothing, and a fence id for nothing is a number wearing a meaning.
            [fk_enc endEncoding];
            [fk_cb commit];
            [fk_cb waitUntilCompleted];
            fk_enc = nil;
            fk_cb = nil;
            fk_batch_concurrent = 0;
            fk_err(@"submit: batch had no dispatches");
            return 0;
        }
        if (fk_inflight == nil) {
            fk_inflight = [NSMutableDictionary dictionary];
            fk_inflight_n = [NSMutableDictionary dictionary];
        }
        [fk_enc endEncoding];
        [fk_cb commit];   // committed, NOT waited — that is the whole point
        long long fence = fk_fence_next;
        fk_fence_next++;
        fk_inflight[@(fence)] = fk_cb;
        fk_inflight_n[@(fence)] = @(fk_pending);
        fk_enc = nil;
        fk_cb = nil;
        fk_pending = 0;
        fk_batch_concurrent = 0;
        return fence;
    }
}

long long fk_metal_fence_wait_external(long long fence) {
    @autoreleasepool {
        if (fk_inflight == nil || fk_inflight[@(fence)] == nil) {
            // Unknown and already-drained fences answer alike, and the refusal is
            // spoken: a wait that "succeeds" on a fence nobody issued is how a
            // double-buffered loop drifts one token out of step silently.
            fk_err([NSString stringWithFormat:@"fence_wait: no in-flight fence %lld", fence]);
            return 0;
        }
        id<MTLCommandBuffer> cb = fk_inflight[@(fence)];
        long long n = [fk_inflight_n[@(fence)] longLongValue];
        [cb waitUntilCompleted];
        [fk_inflight removeObjectForKey:@(fence)];
        [fk_inflight_n removeObjectForKey:@(fence)];
        if ([cb error] != nil) {
            fk_err([NSString stringWithFormat:@"dispatch: %@", [[cb error] localizedDescription]]);
            return -1;
        }
        fk_total_sync++;
        return n;   // how many dispatches this fence held — proof they were outstanding
    }
}

// The voice canary. Every other primitive in this door answers an integer, and an
// integer 0 is also what a numb call answers, so this is the one place a band can
// learn that the door is real before it believes a number that came through it.
long long fk_metal_status_external(char *out, long long cap) {
    @autoreleasepool {
        NSString *err = nil;
        if (!fk_metal_up(&err)) {
            return fk_emit(out, cap, [NSString stringWithFormat:
                @"metal_owner=fkwu-form-cli\nmetal_linked=false\nmetal_door=handle\nlast_error=%@\n", err]);
        }
        NSMutableString *r = [NSMutableString string];
        [r appendString:@"metal_owner=fkwu-form-cli\nmetal_linked=true\nmetal_door=handle\n"];
        [r appendFormat:@"device=%@\nunified_memory=%d\n", [fk_dev name], (int)[fk_dev hasUnifiedMemory]];
        [r appendFormat:@"buffers=%lld\npipelines=%lu\nlibraries=%lu\n",
            (long long)[fk_buf_objs count] - fk_free_top, (unsigned long)[fk_pipe_objs count],
            (unsigned long)[fk_lib_by_src count]];
        [r appendFormat:@"buffer_slots=%lu\nfree_slots=%lld\n",
            (unsigned long)[fk_buf_objs count], fk_free_top];
        [r appendFormat:@"mmap_nocopy_buffers=%lld\n", fk_nocopy_bufs];
        [r appendFormat:@"pending=%lld\n", fk_pending];
        [r appendFormat:@"batch=%s\n",
            fk_cb == nil ? "none" : (fk_batch_concurrent ? "concurrent" : "serial")];
        [r appendFormat:@"in_flight=%lu\n",
            (unsigned long)(fk_inflight == nil ? 0 : [fk_inflight count])];
        [r appendFormat:@"total_dispatch=%lld\ntotal_sync=%lld\n", fk_total_dispatch, fk_total_sync];
        [r appendFormat:@"last_error=%@\n", fk_last_err == nil ? @"none" : fk_last_err];
        return fk_emit(out, cap, r);
    }
}
