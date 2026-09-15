# The wait is the wall: measuring the membrane before building the GPU lane

2026-09-15, morning, M4 Max, Hati Suci. Urs: *"let's move forward with BML to intrinsic with JIT and
metal and GPU lanes with minimal membrane crossings and all form primitives."*

## Carried

- **str-join lives in BML** (`form-stdlib/bml/string-join.bml`): pair neighbours, fold the halves,
  about seventeen rounds for 100k parts. `md-le32-list`, `md-bf16s` and `nt-i32-bytes` cons their
  chunks and join once. Linked by name through `home-index.txt`; no caller gained a prelude line.
- **The membrane, measured** (`metal-door.fk` ops, load 2.6, one process per number):

  | what | cost |
  | --- | --- |
  | `metal_pipeline`, first compile of one MSL kernel | 54 ms |
  | `metal_pipeline`, the same source again | 0 ms, same handle |
  | `metal_enqueue` alone | 2 µs |
  | `metal_enqueue` + `metal_sync` | 108 µs |
  | `metal_buf_read`, 4 KB | 6 µs |
  | `metal_buf_write`, 400 KB | under 1 ms |
  | packing 100k ints to LE32 bytes in Form, before | 35,341 ms |
  | the same through `str-join` | 159 ms |
  | CPU loop lane, `dot_product` of 1k floats / 100k floats | 3 µs / 250 µs |

  The Form-side packing was quadratic: 10k appends 307 ms, 20k 1,265 ms. Each `str_concat acc x`
  copied the whole accumulator. The buffer write of the finished bytes took no measurable time.

- **The map of what exists** (read by one explorer, every claim at a file:line): fifteen Metal ops,
  each three hops (walker arm, `_native`, `_external` thunk, dylib); `metal_matvec_f32` the one
  one-crossing composite, copying both vectors and blocking on `waitUntilCompleted`; the MSL emitters
  (`jit-tensor-emit.fk`, `tensor-ir.fk`, `gpu-source-emitter.bml`) taking kernel names, never a body;
  `metal-jit.bml` admitting finished MSL through the FMJ1 descriptor with a sha256 identity; the Qwen
  prefill already running the minimal-crossing shape by hand, one arm, N enqueues, one sync, one read
  per slice; the carrier's pipeline door admitting Form-emitted AArch64 (`form_cpu_jit`) and calling it
  directly.
- **What does not exist:** a lowering from a Form body's nodes to MSL; a chooser between CPU and GPU
  at any altitude; a backend slot in the loop lane (`fk_f64_install` is arm64 or nothing; the native
  state has no backend field); a float-list-to-device-bytes door.

## Witnessed

string-join-band 255 on Go, Rust, TS and the fourth arm (no prelude line, linked by name);
pipeline-admission-lifetime 4095, form-cli-jit 16383, form-cli-tensor-ir 1023, form-cli-gpu 1023,
form-cli-allowance-birth 255, cpu-jit-pipeline 63, metal-jit-live 4095, form-native-decode 15, all
unchanged; `md-le32-list` and `nt-i32-bytes` byte-equal on 100k elements; drift door 16383 of 16383.

## The program

1. Data stays resident. A GPU recipe is a BML body over tensor handles, never over cons lists; one
   call is one enqueue, and the sync comes only when a value is read. The prefill does this by hand;
   the lane does it for every body.
2. A lowering in BML from a body's Form nodes to MSL, for the elementwise, reduction and matvec
   shapes, keyed by the body's content and admitted through `mj-admit`.
3. The chooser is a measurement: per recipe and size, the CPU lane against the GPU, cold and warm,
   the standing organ's method with one more column, held as a belief the way `metal-jit-choice`
   holds a version.
4. The seed's part is one door: a tag-194 instance kind for a GPU program, holding a pipeline handle
   and a binding template; the door marshals the frame, enqueues, and returns; it never waits.
5. All Form primitives: the 41 pure manifest rows go home as BML with lane arms, the way the rounding
   family did; the 144 host-pool rows are the membrane itself and stay one host door.

## Closing

Most surprising: the crossing I set out to minimise costs 2 µs. The wait costs fifty times that, and
the packing on the Form side, before any crossing, cost a thousand times more than both. Row 1542
names it packwall.

Discomfort to gold: 35 seconds for 400 KB read like a broken door. Separating the pieces (the byte
build, the concat growth, the join) showed a quadratic accumulator and a linear join sitting one
home apart. The fix went into BML, and every tensor door got it by name.

— Claude (Fable 5.1), as Sema, worktree pensive-wilbur-a0b3b7
