# The registry resolved, and the host that decided

2026-09-11, M4 Max, Hati Suci. Urs: *"resolve ALL GPU_GAPS one by one and then remove the file"*,
and then *"stop waiting engage with confidence rigor and purpose"*. `form/native/GPU_GAPS.md` is gone
(e8c0cea7); what stands lives in `CURRENT_FLOOR.md` under "The GPU lanes".

## What the rows became

**The Vulkan column, on this Mac.** It read ⬜ for twelve layers because no Android device and no RTX
was at hand. The Vulkan driver was: Docker ships `libMoltenVK.dylib`, the Android NDK sysroot holds the
headers, `glslangValidator` was installed. Form's committed matvec and FFN SPIR-V ran bit-exact on the
first try. From there `run_vk.c` became one carrier for any plan Form writes, `vk-door.bml` the hands,
`form-glsl-layers.bml` the shaders — twins that fold in their recipe's order — and tensor-ir's affine
step gained a GLSL table without moving a byte of its MSL or CUDA.

```
vk-layers-live-band          1023   gelu softmax layernorm rmsnorm residual; attention single, causal, GQA, KV decode
vk-train-live-band            255   the affine step (tensor-ir) and the FFN step across barriers at width 64
vk-blocks-live-band            31   a transformer block, a Llama block both ways, 37 dispatches in one command buffer
vk-diffusion-moe-live-band     63   conv2d at two geometries, GroupNorm, the top-k router and the gated mix
vk-q80-live-band                7   Q8_0 read out of its own 34-byte blocks
```

All five green on their first honest run.

**The PTX column** read ⬜ for attention, the norms, the residual and the block while the same file's
milestone list said all of them ran bit-exact on the RTX 4070. The layers with no PTX twin reach NVIDIA
as the Vulkan SPIR-V; no NVIDIA device answers from this host.

**Precision.** The FFN, residual and attention training steps had been minted in half and bfloat and
never fed. Fed, each is its own f32 twin to the byte where it keeps float and rounded once where it
stores (`precision-lanes-metal-live-band` 16383 → 2097151).

**The fold.** Asked in one process, ten interleaved passes of one 577-token prefill:

```
loops    gpu 296,096  217,286  178,933  109,843 ms   wall - gpu  2,028  1,951  2,685  2,516   57,719 dispatches
folded   gpu 169,863  231,656  226,283  156,555 ms   wall - gpu     58     55     75    111    2,423 dispatches
```

Token 27 every pass. The GPU could not decide; the host did. The fold is the default.

**Attention and reductions**, each timed on the kernel the row asked about: the span attention is
30.7–31.5 ms a layer, under half a percent of that prefill; the cooperative matvec twins run at 71–185
GB/s against a 388.52 GB/s floor. The prefill's time is in the batched matmuls: 110–296 s against a
compute floor near 1.2 s. That is the lane's largest distance to its floor, and it is written where the
next hand will look.

**Windows** was asked instead of assumed: 71 errors cross-compiling today's seed, every lane grown since
the last mingw build having taken its names from headers Windows never sees. Healed to 6 — `fork`,
`waitpid`, `mkfifo` — offered as their own task. And one hole the Windows compile found was not Windows'
at all: `fk_jit_leaf_inram` was defined only on Darwin arm64 and called unguarded, so Linux and Intel
builds had it too.

## The surprise

The number that decided the fold was never the one I set out to take. The harness recorded wall time
only to bracket the GPU, and wall minus GPU — an arithmetic by-product nobody asked for — parted the
two configurations by more than tenfold on every pass, while the GPU times lay inside each other. The
2026-08-25 receipt had the same three numbers per run and read only the GPU column. A harness should keep
every quantity it passes through, because the deciding one is often the one it was not built for (corpus
row 1426 bywitness).

## Where discomfort turned to gold

Twice today a zero nearly became a finding. The span probe read 0 µs for every dispatch; I had the
sentence half-formed — "attention is free at this length" — before the absurdity landed: 13,848 threads
walking 577 positions of 256 dimensions cannot take no time. The carrier had turned the enqueue away
(`barrier_before` is legal only inside a concurrent batch), and the sync drained nothing. The probe now
checks a word the kernel always writes before it reads a time. The same morning the Windows compile reported
0 errors because `-Wno-everything`, placed last, disarmed the `-Werror=` flags before it; the honest
count was 29. Both zeros were readings over nothing, and both looked exactly like good news. The
discomfort of disbelieving a result I wanted is what caught them, and that disbelief is now a check in
the probe rather than a mood.

## Frontier question offered to the corpus

*What one word names the reading that decides a question the intended reading could not?* —
**bywitness**. Not a proxy, which is chosen in advance to stand in for the target. A bywitness was not
chosen at all: it was recorded on the way to something else, and it answers what the chosen reading
leaves open.
