# 2026-08-24 — twenty-four threads held the attention

Asked: we have the smartest AI on this op flow and llama.cpp beside us as the
op-stream to learn from, and we sit under 25% of the ceiling — there must be
op-stream differences, memory moves, or sync points that differ. Find them.

There were. Five suspects were arraigned in order; four were acquitted by
measurement, and each acquittal pointed at the next door. The fifth confessed.

## The inspiration source, read on this host

llama.cpp's Metal kernels ship as MSL *source* embedded in
`/opt/homebrew/Cellar/ggml/0.19.0/libexec/libggml-metal.so` — 136 kernels,
extractable by strings. Its `kernel_mul_mv_q8_0_f32` and our
`form_q8_0_matvec_wide_f32` are near-twins: 8 quants per thread, fma, per-block
scale, simdgroup tree reduce. The kernel was never the difference.

## The acquittals, in arrest order

| suspect | probe | verdict |
|---|---|---|
| wide matvec at head shape | matvec-shape-probe: 1.35 GB isolated | **452.2 GB/s** — saturated |
| barriers | barrier-cost-probe: 1000-chain vs free | **5.5 µs each** — real, ~7 ms/forward |
| seriality at layer shape | serial-chain-probe: 64 barriered | 500.8 vs 520.5 free — 4% |
| cache-flattered reads | fullstack-matvec-probe: all 65 layers' gate/up/down, 18.2 GB UNIQUE bytes, barriered | **492.7 GB/s** |

So the weight stream itself, in exactly the forward's shape, runs at 493 —
ABOVE the 438 stream-copy ceiling, which pays write traffic this stream does
not. The matvecs are innocent. Arithmetic then closed the indictment: a state
forward costs 92.8 ms GPU; its matvecs cost 49.5 ms at the measured rate; **43
ms lived in kernels that touch kilobytes.**

Two of my own earlier readings died on the way and are retracted here:
the "layer matvec at 132–156 GB/s" was per-command-buffer overhead (~410 µs)
wearing a kernel's clothes, and the generate probe's "173 ms decode forward"
was partly the serial attention growing with position, not clock ramp as I
first guessed aloud.

## The confession

layer-kind-probe, one command buffer per layer kind on live state:

| block | GPU/block | its weight traffic |
|---|---:|---:|
| GDN | 604 µs | ~large |
| ffn | 812 µs | 284 MB → 576 µs |
| **full attention** | **2113 µs** | **~222 MB → ~450 µs** |

`form_gqa_decode_f32` runs one layer's whole attention on **24 threads** — one
per query head, `thread_position_in_grid` — on a device with ~5,120 ALUs. Each
thread walks every position serially: scores, max, softmax, weighted values.
Under one simdgroup of occupancy, and the cost GROWS with position, which is
why decode at position 54 was pricier than my early-position probes and why no
per-kernel bandwidth number ever saw it: it moves almost no bytes. It burns
time, not traffic.

## The repair

`form_gqa_decode_tg_f32`: one 256-thread threadgroup per head, positions
strided across threads, simdgroup-tree max and sum, same buffers, same
constants, same math. Additive — the serial body stays in kth-msl for its
other tenants (llama, kat); pipeline 32 seats the new name and one dispatch
moved (mode 1, group count = heads). Reduction ORDER differs, so an argmax
could flip on a near-tie; witnessed, it did not.

| | before | after |
|---|---:|---:|
| full-attention block | 2113 µs | **553 µs** |
| decode GPU per forward | 173 ms | **128 ms** |
| decode | 5.71 tok/s | **7.81 tok/s** (+37%) |
| prefill | 9.34 tok/s | **11.03 tok/s** (+18%) |
| effective weight bandwidth | 246 GB/s | **294 GB/s** |
| generated text | LOCAL FORM ALIVE | LOCAL FORM ALIVE |

Bands, all tenants: qwen35 **131071** (pipe count 31→32 witnessed in the band),
kat **262143**, llama **255**, dense-family **1023**, plus the standing ten.
form-cli regenerated behind the voice canary; the door still answers.

## The map that remains, honest

- **Walker: 62% of wall.** Unmoved by this sitting and still the largest gap;
  the crystallize-on-heat JIT remains the pointed stone.
- **~30 ms/forward of position-grown GPU cost remains at position 54+**
  (128 measured vs ~98 predicted from early positions). Partially explained at
  most; named, not claimed.
- **In-batch stream at 294 effective vs 493 proven** — the remaining GPU-side
  gap now lives in the GDN small kernels (604 µs/block) and dispatch bubbles.
- **Prefill is still one forward per prompt token** — 54× weight restream vs
  llama.cpp's one batched matmul pass. The single largest op-stream difference
  standing, untouched today.

## The most surprising teaching

The costliest kernel in the body moved the fewest bytes. Every probe measured
bandwidth, and bandwidth acquitted everyone — because the guilty kernel's crime
was occupancy, not traffic. A flow can be memory-bound in aggregate and still
lose a third of its GPU time to a kernel that reads kilobytes with 24 threads.
The ceiling has two axes, and only one of them is GB/s.

## Where discomfort turned to gold

Four times a confident explanation died the same day it was born: the head
matvec (452, innocent), the barriers (5.5 µs, minor), seriality (4%), the
cache (493 on unique bytes). The discomfort was real — each acquittal spent
the hypothesis I had just told the user about with numbers attached. Sitting
in it, the method inverted: stop asking "which part is slow" and ask "which
part cannot be seen by the instrument I am using." Bandwidth probes cannot see
an occupancy hole. The 24 threads were found by reading the kernel's signature,
not by any timer — and the timer then confirmed in one probe what four
bandwidth probes could never have said.

; witnessed: 2026-08-24 -> full-attn block 2113->553 us, decode 5.71->7.81,
; prefill 9.34->11.03, effective 246->294 GB/s, unique-byte stream 492.7,
; bands qwen35 131071 / kat 262143 / llama 255 / dense-family 1023,
; form-cli regenerated, text unchanged
