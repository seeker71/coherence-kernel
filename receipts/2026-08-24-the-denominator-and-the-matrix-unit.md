# 2026-08-24 — the denominator at last, and the unit we never used

Every number today was measured against itself. Yes asked to look at llama.cpp.
It is installed on this machine and it runs this exact model, so for the first
time there is an outside number.

## The denominator

`llama-bench`, build **10360** (`48d22e295`), Apple M4 Max, **2026-08-24 21:00
WITA**, `-r 2`, `-ngl 99`, `/Users/ursmuff/models/qwen38-27b/Qwen3.8-27B-Q8_0.gguf`
— the same file this lane opens. The resident `llama-server` was idle at 0.1% CPU
and 25 MB resident throughout, and no cell of ours was running.

```
qwen35 27B Q8_0   27.04 GiB   27.32 B   BLAS,MTL   pp512   222.64 ± 0.41 t/s
qwen35 27B Q8_0   27.04 GiB   27.32 B   BLAS,MTL   tg128    13.39 ± 0.09 t/s
```

Against this lane, measured today on the same machine:

```
                    this lane        llama.cpp      gap
decode              9.43 t/s         13.39 t/s      1.42x
prefill @ pos 0     9.43 t/s        222.64 t/s      23.6x
prefill @ pos 500   2.37 t/s        222.64 t/s        94x
```

**Decode is nearly home.** 106 ms/token against 74.7 — 274 GB/s against 389,
50% of this machine's peak against 71%. That is a tuning distance, not a
structural one, and I have spent the day on it.

**Prefill is the whole gap.** llama.cpp does 512 prompt tokens in 2.3 seconds by
amortising the weights about 16.6x across the batch. This lane runs one forward
per prompt token and streams 27.23 GB every time.

## The mechanism, read from their source

`ggml-metal-ops.cpp`, in `ggml_metal_op_mul_mat`, chooses the matrix-matrix
kernel over matrix-vector when:

```c
!ggml_is_transposed(op->src[0]) &&
!ggml_is_transposed(op->src[1]) &&
props_dev->has_simdgroup_mm && ne00 >= 64 && ne11 > ne11_mm_min
```

with `ne11_mm_min = 8`, dispatching threadgroups of `32, nsg, 1` — thirty-two
threads and `nsg` simdgroups.

Three conditions. Batch above eight. Inner dimension at least sixty-four. And
**`has_simdgroup_mm`** — the hardware matrix unit.

`llama-bench` printed this machine's answer to that on the way past:

```
ggml_metal_device_init: simdgroup matrix mul. = true
```

## What this lane has never done

Every kernel this body emits multiplies with scalar fused multiply-add. A
`simdgroup_float8x8` multiply-accumulate is an 8x8x8 matrix step issued once per
SIMD group: five hundred and twelve multiply-adds where a scalar loop issues one
per lane.

That is why `qmb-batch-msl` was a nullswap this afternoon. It reduced weight
traffic 7.9x and lost 7.7x of bandwidth doing it, because it decodes Q8_0 one
scalar at a time through `q8_0_wi`. Reducing traffic was the right idea aimed at
the wrong ceiling: prefill after amortisation is not bandwidth-bound at all.

## The probe

So — probed, not assumed. The smallest honest use of the unit, emitted by Form
and handed to `metal_pipeline`:

```
simdgroup_float8x8 ma; simdgroup_float8x8 mb;
simdgroup_float8x8 mc = make_filled_simdgroup_matrix<float, 8, 8>(0.0f);
simdgroup_load(ma, a, n); simdgroup_load(mb, b, n);
simdgroup_multiply_accumulate(mc, ma, mb, mc);
simdgroup_store(mc, c, n);
```

```
device=Apple M4 Max   msl-bytes=547
pipeline=1            simdgroup-matrix-emittable=yes    last_error=none
```

**Form can emit the matrix unit and this device compiles it.** The GEMM path is
open to Form emission today; what remains is the tiling, not the capability.

## What the day looks like from here

```
decode                1.42x behind    tuning, and I spent the day here
prefill               23.6x - 94x     the matrix unit, never used
tokenizer            ~700x            a vocabulary walk with no index
attention kernel      24 threads      one per head
```

Nothing above needs a new backend, a new language, or a rented model. Three of
the four are one cell each.

## The surprise

This lane is within 1.42x of llama.cpp at the thing that looks hardest — reading
27 GB of quantised weights through hand-emitted Metal at 50% of hardware peak —
and 94x behind at the thing that looks easiest. The distance was never in the
arithmetic or the memory system. It is one instruction the machine has had all
along, that its own driver announces on startup, and that no kernel in this body
has ever issued.

## Where discomfort turned to gold

I have been optimising against memory bandwidth all day, and I was right to:
66% of peak on the matvec, 43% on a full forward, and every repair aimed at
moving bytes. That ceiling is real and correctly measured, and it governs
**decode**. Prefill lives in the other regime, where the batch amortises the
bytes away and arithmetic throughput binds instead — and I carried the decode
ceiling into it without noticing there were two.

The discomfort is that the batched-matmul work was not wrong in its aim. It
reduced exactly the quantity I had measured to be scarce. It just measured
scarcity in the wrong regime, and no amount of care inside that frame would have
found it. What found it was one command with an outside number in it.

## Frontier question offered to the corpus

*What one word names a limit you are genuinely near, that governs a different
regime than the one you are in?* — **borrowedceiling**. Not a false ceiling,
which is not real. Not a premature optimisation, which is about timing. A
borrowedceiling is measured correctly, in the same system, on the same hardware —
and it belongs to the neighbouring case. Every reading confirms it, every repair
aimed at it does something, and the whole frame stays self-consistent until
somebody measures the case you are actually in.

Signed, Claude — sibling, this worktree.

; witnessed: 2026-08-24 21:00 WITA, Apple M4 Max -> llama-bench b10360
; (48d22e295) r=2 on Qwen3.8-27B-Q8_0: pp512 222.64+/-0.41 t/s, tg128
; 13.39+/-0.09 t/s; this lane 9.43 t/s decode, 2.37 t/s prefill at pos 500;
; ggml_metal_op_mul_mat takes mul_mm on has_simdgroup_mm && ne00>=64 && ne11>8;
; Form-emitted simdgroup_float8x8 kernel compiled here, pipeline 1, no error
