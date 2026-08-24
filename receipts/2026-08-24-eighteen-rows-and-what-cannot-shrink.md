# 2026-08-24 — eighteen rows, and the thing that could not shrink

Asked: fully seat all ops, and reduce the number of ops to the absolute minimum.

Both halves landed on MLX. Only one half was possible on Metal, and the reason
is the teaching.

## The census, counted before anything moved

```
208   distinct form_* MSL kernels across the body
 66   of them matvec variants
 13   dequant variants
 36   kernels in the MSL the qwen35 dense path emits
 31   of those compiled into pipelines
  5   MLX carrier ops:  add mul sub max sum   — int32 only
```

## MLX: eighteen rows, and sixteen ops that need none

The carrier's own header already carried the law — *"New shapes are new tokens
in the program, not new opcodes"* — but it had five ops and no floats, so a
transformer could not be said in it at all.

It now holds **eighteen forms and no more**, each with its reason written at
the site:

```
<int>  vN     f32 i32     dup swap
add mul div max      exp rsqrt      sum rmax
matmul   rN   take   argmax
```

`div` is not reachable from add and mul. `swap` is there because `div` needs
its operands the other way round and a stack cannot say a DAG without a
shuffle — `silu` needs x twice. `argmax` stays because deriving it wants `eq`
plus `where`, two rows to save one. A program lands ONE int32 and says `i32`
before it ends, so the carrier owns no float return path and needs no float
parser.

**`sub` was retired**, to prove the law cuts both ways. It is now a Form line:
`-1 mul add`.

Sixteen ops that are Form-emitted graphs over those eighteen and cost the
carrier nothing — `form/form-stdlib/mlx-derived.fk`, band **65535**:

| | | |
|---|---|---|
| sub | `-1 mul add` | 4 |
| neg | `-1 mul` | -5 |
| square | `dup mul` | 36 |
| recip | `1 swap div` | 0.250 |
| sigmoid | `-1 mul exp 1 add 1 swap div` | 0.500 |
| silu | `dup <sigmoid> mul` | 0 |
| tanh | `2 mul exp dup 1 -1 mul add swap 1 add div` | 0 |
| swiglu | `<silu> mul` | 0 |
| mean | `sum n div` | 2.500 |
| l2norm | `dup dup mul sum rsqrt mul` | 1.400 |
| rmsnorm | `dup dup mul <mean> rsqrt mul` | 2.000 |
| softmax | `dup rmax -1 mul add exp dup sum div` | 1.000 |
| matmul | `f32 r2 2 2 dup matmul sum` | 54.000 |
| take | `v4 10 20 30 40 v1 2 take` | 30 |
| argmax | `v4 1 9 3 4 argmax` | 1 |
| gated | `<sigmoid> mul` | 3.000 |

Floats land through a round-half-up ending, so every reading above is exact
rather than nearly so. `swiglu`'s zero is load-bearing: `silu(0)` is exactly 0,
so a derivation that dropped the gate would answer 3000 there, not 0.

## MLX was also not seated where it matters

`fkwu` has linked the MLX carrier since 2026-08-20. **`form-cli` never did.**
The binary a session actually speaks through answered `mlx_linked=false` on a
host whose MLX was live — the exact shape the build script's own paragraph
names: *"how a body ends up not knowing what it is capable of."* That paragraph
was about Metal. MLX sat one `if` below it, unwritten.

`build-form-cli.sh` now links it when `libmlxc` is present, no flag. Witnessed:
`otool -L` shows `/opt/homebrew/opt/mlx-c/lib/libmlxc.dylib`, and
`fk_mlx_run_external` / `fk_mlx_add_external` / `fk_mlx_status_external` are
**T** in the binary rather than weak stubs. The repl still has no verb that
speaks `mlx_status` — named, not claimed.

## Metal: the reduction that measurement refused

Two reductions looked obvious and both died against `observe/msl-dump-probe.fk`.

**One.** `form_q4k_matvec_f32` and `form_q4k_matvec_off_f32` differ by exactly
one `ebase +` and one `constant uint&`. A strict generalization: pass zero and
the pair is one. But normalising every emitted body under that transform merges
**zero** classes — the two are not both emitted into the same library, so there
is no pair to collapse.

**Two.** Four kernels are in the qwen35 MSL and never compiled into a pipeline:
`form_argmax_f32`, `form_q3k_dequant_f32`, `form_rope_f32`, `form_rope_pair_f32`.
Dead weight, plainly — until asking who else uses them: llama-token-handle,
dense-token-handle, kat-exit-handle, kat-block0. `kth-msl` is one **shared**
library and those are its other tenants. Deleting them is deleting other paths'
organs, on a host that holds one of the models needed to re-witness them.

What is genuinely repeated: the sigmoid line

```
1.0f / (1.0f + fexp(-1.0f * v))
```

appears verbatim in `form_silu_f32`, `form_sig_mul_f32`, `form_swiglu_f32`, and
`form_axpy_sig_f32`. Four kernels, one arithmetic, differing only in how many
buffers the operands arrive in and where the answer lands. That is the one true
duplication in the 31, and collapsing it is dispatch-site surgery across paths
this host cannot re-witness. **Named, queued, not attempted.**

## Re-witnessed, nothing moved

ground **42**, freshness **31**, metal-door **15**, native-vs-rented **11111**,
mlx **63**, mlx-ir **1023**, live **255**, mlx-derived **65535**,
jit-metal-lanes **8191**, metal-handle-door **65535**, emitted-table-capacity
**63**, hati-os **15**, qwen35-dense-token-handle **131071**. And through the
rebuilt form-cli with MLX seated:

```
generate Reply with exactly: LOCAL FORM ALIVE
  backend=form-native-metal-jit
  text: LOCAL FORM ALIVE
```

## The most surprising teaching

**The minimum is a property of the basis, not of the implementation.** MLX went
from five ops to eighteen and got *smaller*, because eighteen chosen forms span
what five arbitrary ones could not reach — sixteen more ops arrived costing no
C at all. Metal's 31 look like the same kind of sprawl and are not: they are
that basis already fused and specialised, and every one removed is memory
traffic added. Counting rows measures the wrong thing on one side of the seam
and the right thing on the other.

## Where discomfort turned to gold

Twice the measurement said my reasoning was wrong, and both times the
comfortable move was to trust the reasoning.

The `ebase` collapse was clean enough to write from memory — a strict
generalization, one parameter, obviously one kernel too many. Running the
normaliser instead of asserting it returned zero merged classes.

Then the four never-piped kernels: 36 present, 31 compiled, five names left
over, and a tidy story about dead weight. One grep for their other callers
turned "dead" into "shared." Both errors had the same root — reading the
qwen35 path as if it owned a library that four other paths live in — and that
root is worth more than either reduction would have been.

; witnessed: 2026-08-24 -> mlx-derived 65535, mlx 63, mlx-ir 1023, live 255,
; qwen35-dense-token-handle 131071, ground 42, freshness 31, metal-door 15,
; form-cli links libmlxc with fk_mlx_*_external defined, generate answers
