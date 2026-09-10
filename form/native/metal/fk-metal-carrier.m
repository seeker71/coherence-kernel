// fk-metal-carrier.m — optional dynamic carrier behind the kernel's Metal door.
//
// WHAT WAS MISSING. runtime/fkwu-uni.c has declared two Metal primitives for a long time —
// metal_matvec_fixture (tag 203) and metal_matvec_f32 (tag 204) — and form-cli-repl.fk:82 has
// been CALLING metal_matvec_f32 with (msl-text, kernel-name, model-bytes) the whole time. Both
// primitives now route through a small dynamic resolver in runtime/fkwu-uni.c. Without a carrier
// artifact the kernel answers "metal_loaded=false". When this file is compiled as a .dylib and
// found by that resolver, the SAME Form cell that printed SKIP yesterday dispatches on the GPU.
//
// BUILD. fkwu is plain C. This carrier is an optional dynamic artifact loaded by the running
// process, not a second executable and not a source file linked into the seed. validate.sh builds
// both artifacts on Darwin. For direct experimentation:
//
//   cc -O2 -o fkwu runtime/fkwu-uni.c
//   cc -O2 -dynamiclib -o form/native/metal/fk-metal-carrier.dylib \
//      form/native/metal/fk-metal-carrier.m \
//      -framework Metal -framework Foundation -fobjc-arc
//
// FKWU_METAL_CARRIER selects an adapter when starting a process. Form program versions are
// admitted and compared live through bml/metal-jit.bml, using this same loaded adapter.
//
// This temporary adapter calls Metal's Objective-C ABI. Form owns the generated programs,
// their identities, cache policy and version selection. Source and pipeline creation happen
// in RAM; a caller can capture/reuse a compiled binary archive. Form-emitted host-ABI calls
// can replace this adapter as that narrower native call surface is witnessed.
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
// answers 0. A 0 handle from a door whose dynamic carrier was not admitted and a 0 handle from a
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
#import <objc/runtime.h>
#include <dispatch/dispatch.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

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
                    @"SKIP fkwu-form-cli-metal-direct: %@\nmetal_owner=fkwu-form-cli\nmetal_carrier=dynamic\nmetal_loaded=true\nmetal_device=false\n", err]);
        }
        return fk_emit(out, cap,
            [NSString stringWithFormat:
                @"PASS fkwu-form-cli-metal-direct\nmetal_owner=fkwu-form-cli\nmetal_carrier=dynamic\nmetal_loaded=true\nmetal_device=true\n"
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
                    @"SKIP fkwu-form-cli-metal-matvec-f32: %@\nmetal_owner=fkwu-form-cli\nmetal_carrier=dynamic\nmetal_loaded=true\nmetal_device=false\n", err]);
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
        [r appendString:@"metal_owner=fkwu-form-cli\nmetal_carrier=dynamic\nmetal_loaded=true\nmetal_device=true\n"];
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

// Buffer and pipeline tables grow with the work instead of asking model shape to
// fit a carrier policy. Buffer handles remain the u32 words the Form-owned binding
// format states. Their low 16 bits name a slot and their high 16 bits name that
// slot's generation, so 65535 live/retired slots is the protocol wall. A slot at
// generation 65535 is retired when freed rather than wrapping and making an
// ancient stale handle live again. Pipelines are append-only identities and have
// no carrier-authored count wall.
#define FK_BUF_SLOT_MAX 65535
#define FK_TABLE_INITIAL_CAP 64

typedef struct {
    void *map_base;        // non-NULL when this buffer is an mmap of a file
    size_t map_len;
    unsigned long long view_off;  // where the caller's region starts inside the MTLBuffer
    unsigned long long view_len;  // how much of it is the caller's
    int nocopy;            // 0 = copied, 1 = private file pages, 2 = shared writable file pages
} fk_bufmeta;

// A buffer handle is (generation << 16) | slot, slot 1-based. The generation is
// what lets metal_buf_free exist without inviting the worst failure a handle
// system has: a freed slot reused by a NEW buffer, silently read through an OLD
// handle a Form cell is still holding. Freeing bumps the slot's generation, so
// every outstanding handle to it stops matching and is refused at the door rather
// than answering a stranger's bytes. Generation 0 makes every pre-free handle
// value numerically identical to the plain slot index. Generation never wraps:
// after generation 65535 that slot retires, while the table grows another seat.
static NSMutableArray *fk_buf_objs = nil;   // slot s -> element s-1 (NSNull when freed)
static fk_bufmeta *fk_buf_meta = NULL;
static unsigned int *fk_buf_gen = NULL;
static long long *fk_free_slots = NULL;
static size_t fk_buf_cap = 0;
static long long fk_free_top = 0;
static long long fk_retired_slots = 0;
static NSMutableArray *fk_pipe_objs = nil;  // handle h -> element h-1
static NSMutableDictionary *fk_pipe_by_key = nil;  // source, entry, archive mode/path -> handle
static NSMutableArray *fk_cpu_pipe_images = nil;   // NSData for CPU JIT, NSNull for Metal
static void **fk_cpu_pipe_mem = NULL;
static size_t fk_pipe_cap = 0;
static long long fk_total_cpu_jit_dispatch = 0;
static double fk_total_cpu_jit_busy_s = 0.0;
static long long fk_metal_jit_compiles = 0;
static long long fk_metal_jit_ram_hits = 0;
static long long fk_metal_jit_archive_reads = 0;
static long long fk_metal_jit_archive_writes = 0;

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
// Real GPU device-busy time, not host wall-clock. MTLCommandBuffer.GPUStartTime/
// GPUEndTime are the host-clock timestamps Metal itself records for when the
// device actually started and finished a command buffer's work; they read as
// zero until the buffer completes, so this is only ever added AFTER
// waitUntilCompleted returns for that buffer. This is a running total across
// every command buffer this process has drained (inflight fences and the open
// batch alike), the same accumulation style as fk_total_dispatch/fk_total_sync,
// so metal_status's callers get the same "since process start" semantics for
// all three. It answers a different question than sync-ms (Form's own
// now_unix_ms bracketing of metal_sync): this is what the DEVICE reports about
// its own busy span, sync-ms is what the HOST clock saw waiting for it.
static double fk_total_gpu_busy_s = 0.0;
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

long long fk_metal_sync_external(void);

static void fk_err(NSString *s) { fk_last_err = s; }

// Grow bookkeeping only when a new identity needs a seat. These arrays carry no
// model bytes; their size follows observed use and their allocation failures are
// returned through metal_status instead of becoming a plausible handle.
static int fk_buf_tables_reserve(size_t need) {
    if (need <= fk_buf_cap) { return 1; }
    size_t next = fk_buf_cap == 0 ? FK_TABLE_INITIAL_CAP : fk_buf_cap;
    while (next < need && next < FK_BUF_SLOT_MAX) {
        size_t doubled = next * 2;
        next = doubled > FK_BUF_SLOT_MAX ? FK_BUF_SLOT_MAX : doubled;
    }
    if (next < need) {
        fk_err([NSString stringWithFormat:
            @"buffer handle protocol exhausted at %d slots", FK_BUF_SLOT_MAX]);
        return 0;
    }
    fk_bufmeta *meta = (fk_bufmeta *)calloc(next, sizeof(fk_bufmeta));
    unsigned int *gen = (unsigned int *)calloc(next, sizeof(unsigned int));
    long long *free_slots = (long long *)calloc(next, sizeof(long long));
    if (meta == NULL || gen == NULL || free_slots == NULL) {
        free(meta); free(gen); free(free_slots);
        fk_err(@"buffer bookkeeping allocation failed");
        return 0;
    }
    if (fk_buf_cap > 0) {
        memcpy(meta, fk_buf_meta, fk_buf_cap * sizeof(fk_bufmeta));
        memcpy(gen, fk_buf_gen, fk_buf_cap * sizeof(unsigned int));
        memcpy(free_slots, fk_free_slots, fk_buf_cap * sizeof(long long));
    }
    free(fk_buf_meta); free(fk_buf_gen); free(fk_free_slots);
    fk_buf_meta = meta;
    fk_buf_gen = gen;
    fk_free_slots = free_slots;
    fk_buf_cap = next;
    return 1;
}

static int fk_pipe_table_reserve(size_t need) {
    if (need <= fk_pipe_cap) { return 1; }
    if (need > ((size_t)-1) / sizeof(void *)) {
        fk_err(@"pipeline bookkeeping size overflow");
        return 0;
    }
    size_t next = fk_pipe_cap == 0 ? FK_TABLE_INITIAL_CAP : fk_pipe_cap;
    while (next < need) {
        if (next > ((size_t)-1) / (2 * sizeof(void *))) {
            next = need;
            break;
        }
        if (next > ((size_t)-1) / 2) {
            fk_err(@"pipeline bookkeeping size overflow");
            return 0;
        }
        next *= 2;
    }
    void **mem = (void **)realloc(fk_cpu_pipe_mem, next * sizeof(void *));
    if (mem == NULL) {
        fk_err(@"pipeline bookkeeping allocation failed");
        return 0;
    }
    memset(mem + fk_pipe_cap, 0, (next - fk_pipe_cap) * sizeof(void *));
    fk_cpu_pipe_mem = mem;
    fk_pipe_cap = next;
    return 1;
}

// ── the caller's patience ──────────────────────────────────────────────────
// A committed buffer's completion is observed, not presumed, and the wait is the
// kernel's own: at commit every buffer is armed with a completed handler that
// signals a semaphore the buffer carries, and a wait blocks on that semaphore —
// no status poll, no backoff, no sleep. The deadline is the CALLER's: Form hands
// it in as data through metal_deadline (ms), and the one place the default
// number lives is a Form row — form-stdlib/hearth.bml, hearth-metal-deadline-ms —
// where the argument for that number lives beside it. Nothing here argues a
// number. -1 means no caller handed one, and the wait is then unbounded exactly
// as the hearth's bell is (spool-bell-transport: block on the kernel's read,
// never sleep in a poll). What a deadline abandons is SHELVED, not lost: the
// buffer stays owed to the device, the next wait settles it first, and says so.
// Every wait ends in ONE typed frame, readable as data through metal_status:
//   completed  the buffer finished within the deadline
//   error      the buffer finished and Metal reported an error (last_error says)
//   timeout    the deadline passed with the buffer still on the device; shelved
//   released   this wait also settled work an earlier deadline had abandoned
// Witnessed 2026-08-25: two residents of one 27 GB artifact on this host — the
// second walker hung 17+ min on an idle GPU. The current door is typed and
// caller-owned: the caller supplies a deadline or asks for the kernel's own
// blocking wait, and status reports the exact frame that occurred.
enum { FK_WAIT_NONE = 0, FK_WAIT_COMPLETED = 1, FK_WAIT_ERROR = 2,
       FK_WAIT_TIMEOUT = 3, FK_WAIT_RELEASED = 4 };
static long long fk_deadline_ms = -1;      // handed by the caller; -1 = kernel's own wait
static int fk_wait_frame = FK_WAIT_NONE;   // the last wait's typed frame
static double fk_wait_last_s = 0.0;        // host seconds the last wait took
static NSMutableArray *fk_shelf = nil;     // buffers a deadline abandoned, still owed
static NSString *fk_timeout_line = nil;    // the red line a timeout wrote; cleared on release
static const void *fk_sem_key = &fk_sem_key;

// One predicate owns the meaning of quiescence. Open, submitted, and timed-out
// command buffers all retain work which may still read or write shared storage.
// Any host access or release path that needs settled bytes asks this predicate;
// adding another outstanding-work lane therefore cannot silently miss one door.
static int fk_work_outstanding(void) {
    return fk_cb != nil ||
        (fk_inflight != nil && [fk_inflight count] > 0) ||
        (fk_shelf != nil && [fk_shelf count] > 0);
}

static const char *fk_wait_frame_name(void) {
    switch (fk_wait_frame) {
        case FK_WAIT_COMPLETED: return "completed";
        case FK_WAIT_ERROR:     return "error";
        case FK_WAIT_TIMEOUT:   return "timeout";
        case FK_WAIT_RELEASED:  return "released";
        default:                return "none";
    }
}

// Arm BEFORE commit. The handler runs on Metal's completion thread and only
// signals; the waiter reads the buffer's own status afterwards. A timed-out wait
// leaves the signal unconsumed, so a later wait on the same buffer still wakes.
static void fk_arm(id<MTLCommandBuffer> cb) {
    dispatch_semaphore_t sem = dispatch_semaphore_create(0);
    objc_setAssociatedObject(cb, fk_sem_key, sem, OBJC_ASSOCIATION_RETAIN);
    [cb addCompletedHandler:^(id<MTLCommandBuffer> done) {
        (void)done;
        dispatch_semaphore_signal(sem);
    }];
}

// 0 when the buffer settled (completed, or error — the caller reads [cb error]);
// -1 when the caller's deadline passed first. Sets the frame and the measured wait.
static int fk_wait_observed(id<MTLCommandBuffer> cb) {
    NSDate *t0 = [NSDate date];
    MTLCommandBufferStatus st = [cb status];
    if (st != MTLCommandBufferStatusCompleted && st != MTLCommandBufferStatusError) {
        dispatch_semaphore_t sem = objc_getAssociatedObject(cb, fk_sem_key);
        if (sem == nil) {
            [cb waitUntilCompleted];   // an unarmed buffer: the kernel's own wait
        } else {
            dispatch_time_t until = fk_deadline_ms < 0 ? DISPATCH_TIME_FOREVER
                : dispatch_time(DISPATCH_TIME_NOW, fk_deadline_ms * (long long)NSEC_PER_MSEC);
            (void)dispatch_semaphore_wait(sem, until);
        }
        st = [cb status];
    }
    fk_wait_last_s = -[t0 timeIntervalSinceNow];
    if (st == MTLCommandBufferStatusCompleted) { fk_wait_frame = FK_WAIT_COMPLETED; return 0; }
    if (st == MTLCommandBufferStatusError) { fk_wait_frame = FK_WAIT_ERROR; return 0; }
    fk_wait_frame = FK_WAIT_TIMEOUT;
    fk_timeout_line = [NSString stringWithFormat:
        @"sync: command buffer status=%lu after the caller's %lld ms — timeout, shelved, still owed",
        (unsigned long)st, fk_deadline_ms];
    fk_err(fk_timeout_line);
    return -1;
}

static void fk_shelve(id<MTLCommandBuffer> cb) {
    if (fk_shelf == nil) { fk_shelf = [NSMutableArray array]; }
    [fk_shelf addObject:cb];
}

// Settle what earlier deadlines abandoned, oldest first, within the caller's
// current deadline. Answers how many were released, or -1 while one is still
// owed (frame = timeout; it and everything behind it stay shelved).
static long long fk_shelf_settle(void) {
    long long released = 0;
    NSString *settled_error = nil;
    while (fk_shelf != nil && [fk_shelf count] > 0) {
        id<MTLCommandBuffer> cb = fk_shelf[0];
        if (fk_wait_observed(cb) < 0) { return -1; }
        fk_total_gpu_busy_s += ([cb GPUEndTime] - [cb GPUStartTime]);
        if ([cb error] != nil) {
            settled_error = [NSString stringWithFormat:@"dispatch (released from shelf): %@",
                             [[cb error] localizedDescription]];
        }
        [fk_shelf removeObjectAtIndex:0];
        released++;
    }
    if (settled_error != nil) {
        fk_wait_frame = FK_WAIT_ERROR;
        fk_err(settled_error);
        return -2;
    }
    // Nothing is owed any more: the red line a timeout wrote no longer stands.
    if (fk_last_err != nil && fk_last_err == fk_timeout_line) { fk_last_err = nil; }
    return released;
}

// The door the caller hands its patience through. Answers the deadline that
// stood before, so the hand-over reads back as data (-1: none stood).
long long fk_metal_deadline_external(long long ms) {
    long long before = fk_deadline_ms;
    fk_deadline_ms = ms < 0 ? -1 : ms;
    return before;
}

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
        // Form owns this descriptor and the selected cache mode. All source
        // compilation remains in memory. Mode 0 never touches a cache file;
        // mode 1 records a compiled archive; mode 2 requires an archive hit.
        // FMJ1 | u32 mode | u32 entry bytes | u32 path bytes | entry | path | MSL
        unsigned int cache_mode = 0;
        NSString *cache_path = @"";
        if (name_len == 17 && memcmp(name, "form_metal_jit_v1", 17) == 0) {
            if (msl_len < 16 || memcmp(msl, "FMJ1", 4) != 0) {
                fk_err(@"metal jit descriptor header invalid"); return 0;
            }
            const unsigned char *p = (const unsigned char *)msl;
            unsigned int fields[3];
            for (int k = 0; k < 3; k++) {
                const unsigned char *q = p + 4 + k * 4;
                fields[k] = (unsigned int)q[0] | ((unsigned int)q[1] << 8) |
                    ((unsigned int)q[2] << 16) | ((unsigned int)q[3] << 24);
            }
            cache_mode = fields[0];
            unsigned long long prefix = 16ULL + fields[1] + fields[2];
            if (cache_mode > 2 || fields[1] == 0 || prefix >= (unsigned long long)msl_len ||
                (cache_mode == 0 ? fields[2] != 0 : fields[2] == 0)) {
                fk_err(@"metal jit descriptor lengths or mode invalid"); return 0;
            }
            name = msl + 16;
            name_len = fields[1];
            cache_path = [[NSString alloc] initWithBytes:name + name_len length:fields[2]
                                                encoding:NSUTF8StringEncoding];
            if (cache_path == nil || memchr(name, 0, fields[1] + (size_t)fields[2]) != NULL) {
                fk_err(@"metal jit entry/path is invalid UTF-8 or contains NUL"); return 0;
            }
            msl += prefix;
            msl_len -= (long long)prefix;
        }
        if (fk_pipe_objs == nil) {
            fk_pipe_objs = [NSMutableArray array];
            fk_pipe_by_key = [NSMutableDictionary dictionary];
            fk_cpu_pipe_images = [NSMutableArray array];
        }
        NSString *fn = [[NSString alloc] initWithBytes:name length:(NSUInteger)name_len
                                              encoding:NSUTF8StringEncoding];
        if (fn == nil) { fk_err(@"kernel name is not UTF-8"); return 0; }

        // The same Form-owned pipeline door admits a CPU organ when, and only
        // when, the name says so. The payload is then raw AArch64 emitted by
        // Form, not MSL; the carrier owns only W^X installation and the call.
        if (name_len == 12 && memcmp(name, "form_cpu_jit", 12) == 0) {
            if (msl_len <= 0 || msl_len > 1048576) {
                fk_err(@"cpu jit image length is outside (0,1048576]"); return 0;
            }
            NSData *image = [NSData dataWithBytes:msl length:(NSUInteger)msl_len];
            for (NSUInteger k = 0; k < [fk_cpu_pipe_images count]; k++) {
                id prior = fk_cpu_pipe_images[k];
                if (fk_cpu_pipe_mem[k] != NULL && prior != [NSNull null] &&
                    [prior isEqualToData:image]) {
                    return (long long)k + 1;
                }
            }
            if (!fk_pipe_table_reserve((size_t)[fk_pipe_objs count] + 1)) { return 0; }
            size_t pg = (size_t)getpagesize();
            size_t span = ((size_t)msl_len + pg - 1) & ~(pg - 1);
            void *mem = mmap(NULL, span, PROT_READ | PROT_WRITE | PROT_EXEC,
                             MAP_PRIVATE | MAP_ANON | MAP_JIT, -1, 0);
            if (mem == MAP_FAILED) { fk_err(@"cpu jit MAP_JIT failed"); return 0; }
            pthread_jit_write_protect_np(0);
            memcpy(mem, msl, (size_t)msl_len);
            pthread_jit_write_protect_np(1);
            __builtin___clear_cache((char *)mem, (char *)mem + msl_len);
            [fk_pipe_objs addObject:[NSNull null]];
            [fk_cpu_pipe_images addObject:image];
            NSUInteger slot = [fk_pipe_objs count] - 1;
            fk_cpu_pipe_mem[slot] = mem;
            return (long long)slot + 1;
        }

        NSString *src = [[NSString alloc] initWithBytes:msl length:(NSUInteger)msl_len
                                               encoding:NSUTF8StringEncoding];
        if (src == nil) { fk_err(@"msl is not UTF-8"); return 0; }

        // Compile once. This is requirement 3's other half: if the pipeline were
        // rebuilt per dispatch, enqueue could never be cheap no matter how little
        // it waited. Same text + same name = same handle, always.
        NSArray *key = @[fn, src, @(cache_mode), cache_path];
        NSNumber *have = fk_pipe_by_key[key];
        if (have != nil) { fk_metal_jit_ram_hits++; return [have longLongValue]; }

        if (!fk_pipe_table_reserve((size_t)[fk_pipe_objs count] + 1)) { return 0; }
        NSError *e = nil;
        id<MTLLibrary> lib = [fk_dev newLibraryWithSource:src options:nil error:&e];
        if (lib == nil) {
            // The compiler's own words, kept whole. metal_status speaks them.
            fk_err([NSString stringWithFormat:@"msl compile: %@", [e localizedDescription]]);
            return 0;
        }
        fk_metal_jit_compiles++;
        id<MTLFunction> f = [lib newFunctionWithName:fn];
        if (f == nil) {
            fk_err([NSString stringWithFormat:@"msl has no kernel named %@", fn]);
            return 0;
        }
        MTLComputePipelineDescriptor *descriptor = [MTLComputePipelineDescriptor new];
        descriptor.computeFunction = f;
        id<MTLBinaryArchive> archive = nil;
        NSURL *archive_url = nil;
        if (cache_mode != 0) {
            archive_url = [NSURL fileURLWithPath:cache_path];
            MTLBinaryArchiveDescriptor *ad = [MTLBinaryArchiveDescriptor new];
            if (cache_mode == 2) { ad.url = archive_url; }
            archive = [fk_dev newBinaryArchiveWithDescriptor:ad error:&e];
            if (archive == nil) {
                fk_err([NSString stringWithFormat:@"metal jit archive: %@", e.localizedDescription]);
                return 0;
            }
            if (cache_mode == 1 && ![archive addComputePipelineFunctionsWithDescriptor:descriptor error:&e]) {
                fk_err([NSString stringWithFormat:@"metal jit archive compile: %@", e.localizedDescription]);
                return 0;
            }
            descriptor.binaryArchives = @[archive];
        }
        MTLPipelineOption options = cache_mode == 2 ? MTLPipelineOptionFailOnBinaryArchiveMiss : MTLPipelineOptionNone;
        id<MTLComputePipelineState> pipe = [fk_dev newComputePipelineStateWithDescriptor:descriptor
            options:options reflection:nil error:&e];
        if (pipe == nil) {
            fk_err([NSString stringWithFormat:@"pipeline: %@", [e localizedDescription]]);
            return 0;
        }
        if (cache_mode == 1) {
            if (![archive serializeToURL:archive_url error:&e]) {
                fk_err([NSString stringWithFormat:@"metal jit archive write: %@", e.localizedDescription]);
                return 0;
            }
            fk_metal_jit_archive_writes++;
        } else if (cache_mode == 2) { fk_metal_jit_archive_reads++; }
        [fk_pipe_objs addObject:pipe];
        [fk_cpu_pipe_images addObject:[NSNull null]];
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
        if ([fk_buf_objs count] >= FK_BUF_SLOT_MAX) {
            fk_err([NSString stringWithFormat:
                @"buffer handle protocol exhausted at %d live or retired slots",
                FK_BUF_SLOT_MAX]);
            if (map_base != NULL) { munmap(map_base, map_len); }
            return 0;
        }
        if (!fk_buf_tables_reserve((size_t)[fk_buf_objs count] + 1)) {
            if (map_base != NULL) { munmap(map_base, map_len); }
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
        int shared_file = len < 0;
        if (len == (-9223372036854775807LL - 1LL)) {
            fk_err(@"buf_from_file length is out of range"); return 0;
        }
        if (shared_file) { len = -len; }
        if (off < 0 || len <= 0) { fk_err(@"buf_from_file needs off>=0 and len>0"); return 0; }

        char p[4096];
        if (path_len <= 0 || path_len >= (long long)sizeof(p)) {
            fk_err(@"buf_from_file path length out of range"); return 0;
        }
        memcpy(p, path, (size_t)path_len);
        p[path_len] = 0;

        int fd = open(p, shared_file ? (O_RDWR | O_CREAT) : O_RDONLY, 0644);
        if (fd < 0) { fk_err([NSString stringWithFormat:@"cannot open %s", p]); return 0; }
        struct stat st;
        if (fstat(fd, &st) != 0) { close(fd); fk_err(@"fstat failed"); return 0; }
        if (shared_file && off + len > (long long)st.st_size) {
            if (ftruncate(fd, (off_t)(off + len)) != 0) {
                close(fd); fk_err(@"writable buf_from_file could not extend file"); return 0;
            }
            st.st_size = (off_t)(off + len);
        }
        // Refuse a region the file does not contain, rather than mapping short and
        // handing back a buffer whose tail is whatever the kernel zero-fills. A
        // model file truncated by a bad download would otherwise decode to noise.
        if (!shared_file && off + len > (long long)st.st_size) {
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

        // A positive length keeps the model mapping private. A negative length is
        // an explicit writable-file mode through the same handle door.
        void *m = mmap(NULL, (size_t)span, PROT_READ | PROT_WRITE,
                       shared_file ? MAP_SHARED : MAP_PRIVATE, fd, (off_t)base);
        close(fd);
        if (m == MAP_FAILED) { fk_err(@"mmap failed"); return 0; }

        id<MTLBuffer> b = [fk_dev newBufferWithBytesNoCopy:m
                                                    length:(NSUInteger)span
                                                   options:MTLResourceStorageModeShared
                                               deallocator:nil];
        if (b != nil) {
            return fk_buf_register(b, m, (size_t)span, (unsigned long long)(off - base),
                                   (unsigned long long)len, shared_file ? 2 : 1);
        }
        if (shared_file) {
            munmap(m, (size_t)span);
            fk_err(@"newBufferWithBytesNoCopy refused writable shared mapping");
            return 0;
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
        // Shared storage has one ownership transition: GPU work settles before
        // the host mutates it. This also commits an open batch, so a write used
        // to refresh a pooled buffer cannot run ahead of dispatches already
        // encoded against that buffer. Double-buffering remains available to a
        // Form recipe that wants overlap without this dependency.
        if (fk_work_outstanding() && fk_metal_sync_external() < 0) { return -1; }
        id<MTLBuffer> b = fk_buf_objs[(NSUInteger)(slot - 1)];
        if (off < 0 || len < 0) { fk_err(@"buf_write: negative off or len"); return -1; }
        fk_bufmeta *m = &fk_buf_meta[slot - 1];
        if ((unsigned long long)off > m->view_len ||
            (unsigned long long)len > m->view_len - (unsigned long long)off) {
            fk_err([NSString stringWithFormat:@"buf_write: off=%lld len=%lld past view_len %llu",
                    off, len, m->view_len]);
            return -1;
        }
        memcpy((char *)[b contents] + m->view_off + off, bytes, (size_t)len);
        if (m->nocopy == 2 && len > 0) {
            long long pg = (long long)getpagesize();
            long long delta = (long long)((m->view_off + (unsigned long long)off) %
                                          (unsigned long long)pg);
            void *flush = (char *)m->map_base + m->view_off + off - delta;
            if (msync(flush, (size_t)(delta + len), MS_SYNC) != 0) {
                fk_err(@"buf_write: shared file flush failed"); return -1;
            }
        }
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

        if (fk_cpu_pipe_mem[pipe - 1] != NULL) {
            if (nbuf > 64 || nconst > 64) {
                fk_err(@"cpu jit binding exceeds 64 buffers or constants"); return -1;
            }
            if (tpg != 0 || tgmem != 0 || mode != 0 || barrier != 0) {
                fk_err(@"cpu jit binding accepts no Metal dispatch tail"); return -1;
            }
            if (fk_work_outstanding()) {
                if (fk_metal_sync_external() < 0) { return -1; }
            }
            void *bufs[64];
            unsigned int consts[64];
            for (unsigned int k = 0; k < nbuf; k++) {
                long long bh = (long long)fk_le32(bp + 4 + 4 * k);
                long long bslot = fk_buf_slot(bh);
                if (bslot == 0) {
                    fk_err([NSString stringWithFormat:@"cpu jit: bad buffer handle %lld at slot %u", bh, k]);
                    return -1;
                }
                id<MTLBuffer> b = fk_buf_objs[(NSUInteger)(bslot - 1)];
                bufs[k] = (char *)[b contents] + fk_buf_meta[bslot - 1].view_off;
            }
            for (unsigned int k = 0; k < nconst; k++) {
                consts[k] = fk_le32(bp + 4 + 4 * nbuf + 4 + 4 * k);
            }
            typedef void (*fk_cpu_pipe_fn)(void **, const unsigned int *, unsigned long long);
            double started = [NSDate timeIntervalSinceReferenceDate];
            ((fk_cpu_pipe_fn)fk_cpu_pipe_mem[pipe - 1])(bufs, consts,
                                                        (unsigned long long)threads);
            fk_total_cpu_jit_busy_s += [NSDate timeIntervalSinceReferenceDate] - started;
            fk_total_cpu_jit_dispatch++;
            return 1;
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
        // What an earlier deadline abandoned is settled first: sync remains the
        // one word that means "the GPU holds nothing of ours".
        long long released = fk_shelf_settle();
        if (released < 0) { return -1; }
        // Everything in flight drains first — a fence the caller stopped tracking
        // still completes here, so sync remains the one word that means "the GPU
        // holds nothing of ours".
        if (fk_inflight != nil && [fk_inflight count] > 0) {
            for (NSNumber *key in [fk_inflight allKeys]) {
                id<MTLCommandBuffer> cb = fk_inflight[key];
                if (fk_wait_observed(cb) < 0) { return -1; }
                // GPUStartTime/GPUEndTime are valid now — the buffer just completed.
                // Real device-busy span for THIS command buffer, added to the
                // process-wide running total.
                fk_total_gpu_busy_s += ([cb GPUEndTime] - [cb GPUStartTime]);
                long long drained = [fk_inflight_n[key] longLongValue];
                [fk_inflight removeObjectForKey:key];
                [fk_inflight_n removeObjectForKey:key];
                if ([cb error] != nil) {
                    fk_err([NSString stringWithFormat:@"dispatch: %@",
                            [[cb error] localizedDescription]]);
                    return -1;
                }
                n += drained;
            }
            had_work = 1;
        }
        if (fk_cb != nil) {
            n += fk_pending;
            [fk_enc endEncoding];
            fk_arm(fk_cb);
            [fk_cb commit];
            if (fk_wait_observed(fk_cb) < 0) {
                // The batch is shelved, not awaited: it stays owed to the device
                // and the next wait settles it first; the walker's side is
                // cleared so the next admission starts clean, and the red line
                // is answered with its typed frame (timeout).
                fk_shelve(fk_cb);
                fk_enc = nil;
                fk_cb = nil;
                fk_pending = 0;
                fk_batch_concurrent = 0;
                return -1;
            }
            // Same accumulation as the inflight loop above: this ONE command
            // buffer's real device-busy span, covering every dispatch enqueued
            // into it since the batch opened (all fk_pending of them together —
            // Metal exposes no finer-grained per-dispatch device timestamp).
            fk_total_gpu_busy_s += ([fk_cb GPUEndTime] - [fk_cb GPUStartTime]);
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
        if (released > 0 && fk_wait_frame == FK_WAIT_COMPLETED) { fk_wait_frame = FK_WAIT_RELEASED; }
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
        if (fk_work_outstanding()) {
            if (fk_metal_sync_external() < 0) { return -1; }
        }
        long long slot = fk_buf_slot(h);
        if (slot == 0) { fk_err([NSString stringWithFormat:@"buf_read: bad handle %lld", h]); return -1; }
        id<MTLBuffer> b = fk_buf_objs[(NSUInteger)(slot - 1)];
        if (off < 0 || len < 0 || len > cap) { fk_err(@"buf_read: off/len out of range"); return -1; }
        fk_bufmeta *m = &fk_buf_meta[slot - 1];
        if ((unsigned long long)off > m->view_len ||
            (unsigned long long)len > m->view_len - (unsigned long long)off) {
            fk_err([NSString stringWithFormat:@"buf_read: off=%lld len=%lld past view_len %llu",
                    off, len, m->view_len]);
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
        // fault Metal cannot see coming. One rule for both kinds, stated by
        // fk_work_outstanding: free only when nothing is open, in flight, or
        // shelved after a deadline.
        if (fk_work_outstanding()) {
            fk_err(@"buf_free: work is open, in flight, or shelved; sync first");
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
        if (fk_buf_gen[slot - 1] == 0xFFFFU) {
            // Never revive an ancient handle. The table can grow another slot;
            // this one remains an explicit retired identity.
            fk_retired_slots++;
        } else {
            fk_buf_gen[slot - 1]++;
            fk_free_slots[fk_free_top] = slot;
            fk_free_top++;
        }
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
            fk_arm(fk_cb);
            [fk_cb commit];
            if (fk_wait_observed(fk_cb) < 0) { fk_shelve(fk_cb); }
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
        fk_arm(fk_cb);
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
        long long released = fk_shelf_settle();
        if (released < 0) { return -1; }
        if (fk_wait_observed(cb) < 0) {
            // Shelved, not dropped: the fence is spent, the buffer is still owed.
            fk_shelve(cb);
            [fk_inflight removeObjectForKey:@(fence)];
            [fk_inflight_n removeObjectForKey:@(fence)];
            return -1;
        }
        [fk_inflight removeObjectForKey:@(fence)];
        [fk_inflight_n removeObjectForKey:@(fence)];
        if ([cb error] != nil) {
            fk_err([NSString stringWithFormat:@"dispatch: %@", [[cb error] localizedDescription]]);
            return -1;
        }
        fk_total_sync++;
        if (released > 0 && fk_wait_frame == FK_WAIT_COMPLETED) { fk_wait_frame = FK_WAIT_RELEASED; }
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
                @"metal_owner=fkwu-form-cli\nmetal_carrier=dynamic\nmetal_loaded=true\nmetal_device=false\nmetal_door=handle\nlast_error=%@\n", err]);
        }
        NSMutableString *r = [NSMutableString string];
        [r appendString:@"metal_owner=fkwu-form-cli\nmetal_carrier=dynamic\nmetal_loaded=true\nmetal_device=true\nmetal_door=handle\n"];
        [r appendFormat:@"device=%@\nunified_memory=%d\n", [fk_dev name], (int)[fk_dev hasUnifiedMemory]];
        [r appendFormat:@"metal_jit_target=%llu|%@|FMJ1\n",
            (unsigned long long)[fk_dev registryID], [NSProcessInfo processInfo].operatingSystemVersionString];
        // The device's own memory accounting, asked of the device every time it
        // is spoken. currentAllocatedSize is the device bytes THIS process's
        // Metal resources currently hold; recommendedMaxWorkingSetSize is the
        // ceiling the system suggests this process stay under before it starts
        // paying for eviction. Neither is a handle count -- fk_buf_objs says how
        // MANY buffers stand, these say how LARGE the standing set is.
        [r appendFormat:@"allocated_bytes=%llu\nrecommended_working_set_bytes=%llu\n",
            (unsigned long long)[fk_dev currentAllocatedSize],
            (unsigned long long)[fk_dev recommendedMaxWorkingSetSize]];
        [r appendFormat:@"buffers=%lld\npipelines=%lu\n",
            (long long)[fk_buf_objs count] - fk_free_top - fk_retired_slots,
            (unsigned long)[fk_pipe_objs count]];
        [r appendFormat:@"metal_jit_source_compiles=%lld\nmetal_jit_ram_hits=%lld\n"
            "metal_jit_archive_reads=%lld\nmetal_jit_archive_writes=%lld\n",
            fk_metal_jit_compiles, fk_metal_jit_ram_hits,
            fk_metal_jit_archive_reads, fk_metal_jit_archive_writes];
        [r appendFormat:
            @"buffer_slots=%lu\nfree_slots=%lld\nretired_slots=%lld\n"
             "buffer_table_capacity=%lu\nbuffer_slot_protocol_limit=%d\n"
             "pipeline_table_capacity=%lu\n",
            (unsigned long)[fk_buf_objs count], fk_free_top, fk_retired_slots,
            (unsigned long)fk_buf_cap, FK_BUF_SLOT_MAX, (unsigned long)fk_pipe_cap];
        [r appendFormat:@"mmap_nocopy_buffers=%lld\n", fk_nocopy_bufs];
        [r appendFormat:@"pending=%lld\n", fk_pending];
        [r appendFormat:@"batch=%s\n",
            fk_cb == nil ? "none" : (fk_batch_concurrent ? "concurrent" : "serial")];
        [r appendFormat:@"in_flight=%lu\n",
            (unsigned long)(fk_inflight == nil ? 0 : [fk_inflight count])];
        [r appendFormat:@"total_dispatch=%lld\ntotal_sync=%lld\n", fk_total_dispatch, fk_total_sync];
        [r appendFormat:@"cpu_jit_dispatch=%lld\ncpu_jit_busy_us_total=%lld\n",
            fk_total_cpu_jit_dispatch, (long long)(fk_total_cpu_jit_busy_s * 1000000.0)];
        // Real GPU device-busy time (MTLCommandBuffer.GPUEndTime - GPUStartTime),
        // summed across every command buffer drained so far, converted to whole
        // microseconds to match this file's existing integer-reporting style.
        // This is HOST-CLOCK-STAMPED device occupancy, not a host wall-clock
        // measurement of waitUntilCompleted itself — see the field's own comment
        // at fk_total_gpu_busy_s for what it does and does not mean.
        [r appendFormat:@"gpu_busy_us_total=%lld\n", (long long)(fk_total_gpu_busy_s * 1000000.0)];
        // The last wait's typed frame, the host ms it took, the deadline in force
        // (-1: none handed, the kernel's own wait), and what a deadline shelved.
        [r appendFormat:@"wait_frame=%s\nwait_ms=%lld\nwait_deadline_ms=%lld\nshelf=%lu\n",
            fk_wait_frame_name(), (long long)(fk_wait_last_s * 1000.0), fk_deadline_ms,
            (unsigned long)(fk_shelf == nil ? 0 : [fk_shelf count])];
        [r appendFormat:@"last_error=%@\n", fk_last_err == nil ? @"none" : fk_last_err];
        return fk_emit(out, cap, r);
    }
}

// HOST GPU LEVEL. The accelerator publishes its own PerformanceStatistics through the I/O registry;
// "Device Utilization %" is the whole host's GPU level, every process, no privilege asked. IOKit is
// loaded by name so the build line stays what it is; a host without the door answers -1 and the
// glass names it absent. The kernel integrates this level over its own clock gaps (host_gpu_busy_us).
typedef unsigned int fk_io_object_t;
long long fk_host_gpu_utilization(void) {
    static void *iokit = 0;
    static CFMutableDictionaryRef (*matching)(const char *) = 0;
    static int (*getServices)(unsigned int, CFDictionaryRef, fk_io_object_t *) = 0;
    static fk_io_object_t (*next)(fk_io_object_t) = 0;
    static CFTypeRef (*createProperty)(fk_io_object_t, CFStringRef, CFAllocatorRef, unsigned int) = 0;
    static int (*release)(fk_io_object_t) = 0;
    if (!iokit) {
        iokit = dlopen("/System/Library/Frameworks/IOKit.framework/IOKit", RTLD_LAZY);
        if (!iokit) return -1;
        matching = (CFMutableDictionaryRef (*)(const char *))dlsym(iokit, "IOServiceMatching");
        getServices = (int (*)(unsigned int, CFDictionaryRef, fk_io_object_t *))dlsym(iokit, "IOServiceGetMatchingServices");
        next = (fk_io_object_t (*)(fk_io_object_t))dlsym(iokit, "IOIteratorNext");
        createProperty = (CFTypeRef (*)(fk_io_object_t, CFStringRef, CFAllocatorRef, unsigned int))dlsym(iokit, "IORegistryEntryCreateCFProperty");
        release = (int (*)(fk_io_object_t))dlsym(iokit, "IOObjectRelease");
    }
    if (!matching || !getServices || !next || !createProperty || !release) return -1;
    fk_io_object_t iter = 0;
    if (getServices(0, matching("IOAccelerator"), &iter) != 0) return -1;
    long long answer = -1;
    fk_io_object_t entry = 0;
    while ((entry = next(iter)) != 0) {
        CFTypeRef stats = createProperty(entry, CFSTR("PerformanceStatistics"), kCFAllocatorDefault, 0);
        if (stats && CFGetTypeID(stats) == CFDictionaryGetTypeID()) {
            CFTypeRef v = CFDictionaryGetValue((CFDictionaryRef)stats, CFSTR("Device Utilization %"));
            if (v && CFGetTypeID(v) == CFNumberGetTypeID()) { long long n = 0; CFNumberGetValue((CFNumberRef)v, kCFNumberSInt64Type, &n); if (n > answer) { answer = n; } }
        }
        if (stats) CFRelease(stats);
        release(entry);
    }
    release(iter);
    return answer;
}

// HOST DISK. Every IOBlockStorageDriver publishes cumulative "Statistics": bytes and operations read
// and written since boot. Summed over the drivers; the kernel derives rates from its own deltas. The
// same IOKit-by-name door as the GPU level; a host without it answers -1.
long long fk_host_disk_stat(long long *out) {
    static void *iokit = 0;
    static CFMutableDictionaryRef (*matching)(const char *) = 0;
    static int (*getServices)(unsigned int, CFDictionaryRef, fk_io_object_t *) = 0;
    static fk_io_object_t (*next)(fk_io_object_t) = 0;
    static CFTypeRef (*createProperty)(fk_io_object_t, CFStringRef, CFAllocatorRef, unsigned int) = 0;
    static int (*release)(fk_io_object_t) = 0;
    if (!iokit) {
        iokit = dlopen("/System/Library/Frameworks/IOKit.framework/IOKit", RTLD_LAZY);
        if (!iokit) return -1;
        matching = (CFMutableDictionaryRef (*)(const char *))dlsym(iokit, "IOServiceMatching");
        getServices = (int (*)(unsigned int, CFDictionaryRef, fk_io_object_t *))dlsym(iokit, "IOServiceGetMatchingServices");
        next = (fk_io_object_t (*)(fk_io_object_t))dlsym(iokit, "IOIteratorNext");
        createProperty = (CFTypeRef (*)(fk_io_object_t, CFStringRef, CFAllocatorRef, unsigned int))dlsym(iokit, "IORegistryEntryCreateCFProperty");
        release = (int (*)(fk_io_object_t))dlsym(iokit, "IOObjectRelease");
    }
    if (!matching || !getServices || !next || !createProperty || !release) return -1;
    fk_io_object_t iter = 0;
    if (getServices(0, matching("IOBlockStorageDriver"), &iter) != 0) return -1;
    out[0] = 0; out[1] = 0; out[2] = 0; out[3] = 0;
    long long drivers = 0;
    fk_io_object_t entry = 0;
    while ((entry = next(iter)) != 0) {
        CFTypeRef stats = createProperty(entry, CFSTR("Statistics"), kCFAllocatorDefault, 0);
        if (stats && CFGetTypeID(stats) == CFDictionaryGetTypeID()) {
            CFStringRef keys[4] = { CFSTR("Bytes (Read)"), CFSTR("Bytes (Write)"), CFSTR("Operations (Read)"), CFSTR("Operations (Write)") };
            for (int k = 0; k < 4; k++) {
                CFTypeRef v = CFDictionaryGetValue((CFDictionaryRef)stats, keys[k]);
                if (v && CFGetTypeID(v) == CFNumberGetTypeID()) { long long n = 0; CFNumberGetValue((CFNumberRef)v, kCFNumberSInt64Type, &n); out[k] += n; }
            }
            drivers++;
        }
        if (stats) CFRelease(stats);
        release(entry);
    }
    release(iter);
    return drivers;
}

// METAL LIVE, AS NUMBERS. The same counters the text status carries, handed as words so no reader parses:
// [0] linked [1] unified [2] buffers [3] pipelines [4] nocopy buffers [5] pending [6] in flight
// [7] total dispatch [8] total sync [9] cpu-jit dispatch [10] cpu-jit busy us [11] gpu busy us
// [12] last wait ms [13] deadline ms [14] shelf [15] batch mode (0 none, 1 serial, 2 concurrent)
// [16] buffer slots [17] free slots [18] allocated bytes [19] recommended working-set bytes.
// Answers 20, or 0 when Metal is not up.
long long fk_metal_live_external(long long *out) {
    @autoreleasepool {
        for (int k = 0; k < 20; k++) out[k] = 0;
        NSString *err = nil;
        if (!fk_metal_up(&err)) return 0;
        out[0] = 1;
        out[1] = (long long)[fk_dev hasUnifiedMemory];
        out[2] = (long long)[fk_buf_objs count] - fk_free_top - fk_retired_slots;
        out[3] = (long long)[fk_pipe_objs count];
        out[4] = fk_nocopy_bufs;
        out[5] = fk_pending;
        out[6] = (long long)(fk_inflight == nil ? 0 : [fk_inflight count]);
        out[7] = fk_total_dispatch;
        out[8] = fk_total_sync;
        out[9] = fk_total_cpu_jit_dispatch;
        out[10] = (long long)(fk_total_cpu_jit_busy_s * 1000000.0);
        out[11] = (long long)(fk_total_gpu_busy_s * 1000000.0);
        out[12] = (long long)(fk_wait_last_s * 1000.0);
        out[13] = fk_deadline_ms;
        out[14] = (long long)(fk_shelf == nil ? 0 : [fk_shelf count]);
        out[15] = fk_cb == nil ? 0 : (fk_batch_concurrent ? 2 : 1);
        out[16] = (long long)[fk_buf_objs count];
        out[17] = fk_free_top;
        /* the allocator ledger as WORDS, not text: the byte gauges the glass reads
         * every frame parsed metal_status per byte before this (fstr-find-loop at
         * 559K calls a tick) -- the same two numbers, read by index */
        out[18] = (long long)(unsigned long long)[fk_dev currentAllocatedSize];
        out[19] = (long long)(unsigned long long)[fk_dev recommendedMaxWorkingSetSize];
        return 20;
    }
}
