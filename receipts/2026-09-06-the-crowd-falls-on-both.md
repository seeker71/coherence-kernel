# The crowd falls on both

The gap handed to me this morning: the whisper-tiny pass is the furthest lane from the floor the
hardware gives it. `encode` at 3.70 ms against a 0.25 ms floor over 83 MB — 14.7x. `wtoken` at
0.65 ms against 0.17 ms over the decoder's 59 MB — 3.6x. Close both as far as they go.

## What I could and could not measure

The first thing the day taught me is not about kernels. `observe/floor-lens-run.fk` measures this
Mac's bandwidth before it judges any lane, and over the hours of this work that reading walked from
**388 GB/s down to 52** and back to 194 and down again to 58. Three other agents were folding models
on this GPU throughout. Under that, every absolute number moved by five to seven times between runs
of *unchanged* code: the encode read 21.6 ms and 39.8 ms twenty minutes apart with nothing touched
between; the siblings' own untouched lanes moved with mine (`1btoken` 12.6 ms to 79.0 ms,
`3btoken` 37.8 ms to 250.7 ms), which is how I know the swing is the machine and not my edits.

So a before-and-after taken from two runs says nothing here. What does say something is putting
**both candidates in one process** — emitting the old kernel and the new one side by side, dispatching
them alternately, and reading the pair. Then the same crowd falls on both and cancels out of the
comparison. Every design decision below was settled that way, and one of them settled *against* what
I would have written from theory.

## The four things that changed, and what each was worth

**A row's mean and deviation are one fact about the row, not a fact about every tile that reads it.**
The tiled gemm covers a 32-row band with one threadgroup per 32 *columns*, and the layernorm lived
inside it — so the encoder's first mlp recomputed the same 32 rows' statistics forty-eight times, two
passes over K each time. Raced in one process, `qkv` with the fused layernorm cost **2440 µs against
1160 µs** with it switched off: the prologue was **more than half the dispatch**. `ear_lnstat` now
walks each row once and leaves (mean, rsqrt) at `stat[2t]`; every tile reads two floats. After the
lift, in one run, `qkv` with layernorm and `qkv` without measured **1120 and 1120** — the prologue is
gone, not reduced. It costs one launch per layernorm, and the four cross gemms share one `ln_post`
reading because they all read the same rows.

**The weights were already half; staging them as float widened them for nothing.** `simdgroup_half8x8`
for the B tile with a `simdgroup_float8x8` accumulator is a legal mixed multiply on this metal, it
drops the weight tile from 4 KB to 2 KB of threadgroup memory, and — checked, not assumed — mel and
enc came back **bit for bit identical**, because half-to-float is exact and the accumulation was
always float. Raced: about 8 to 10 percent across the six gemm shapes, twice.

**Six threadgroups is a sixth of this machine.** The decoder's cross attention gave one threadgroup
per head, so six cores of forty walked 200 KB of k and v each. It is now a chunk pass — a group per
(head, 64 keys), 42 groups on the 8 s window, a thread per key for the scores and a thread per
*channel* for the values so the v reads are contiguous across the group — and a fold behind it that
merges the chunks by the online rule. One more launch, seven times the groups.

**Four logit rows a simdgroup.** The 51,865-row logits matvec is 40 of the token's 59 MB. It gave one
output row to each simdgroup: one weight stream in flight per lane, and the row's layernorm paid
6,484 times over. Above 8192 outputs a simdgroup now takes four rows — four streams in flight, the
prologue paid a quarter as often — and Form reads the same rule from `em-mv-rows` so its group count
cannot drift from the kernel's. Measured across runs: **1760 µs to 560 µs**. The argmax behind it went
from one threadgroup of 1024 walking all 51,865 to 64 groups and a fold: **370 µs to 220 µs**.

## What the race said no to

I was sure the gemm's K loop was barrier-bound — two threadgroup barriers per step of 32, twelve
steps over a row of 384 — and wrote a twin striding K in 64. Raced against the original in one
process it lost on five of six shapes (`qkv` 1175 to 2600, `mlp1` 1950 to 2650). Sixteen KB of
threadgroup memory halves how many tiles share a core, and that costs more than the barriers save.
The twin was deleted. The same race then said yes to eight queries a threadgroup instead of sixteen
(a simdgroup becomes exactly one query, so the slice merge is a whole simd fold): about a tenth,
twice over.

## Proof

`form/form-stdlib/tests/ear-native-band.fk` = **255** after every step. Preflight of the band: parens
balanced, 0 errors, 0 unresolved. mel[0][0..5] on the fixture padded to 8 s:

    -0.138096 0.33575 0.339695 0.272336 0.224791 0.152085

against the reference's `-0.138096 0.33575 0.339695 0.272336 0.224792 0.152085` — the fifth digit is
the previous pass's own reading, unchanged. enc[0][0..5] on the 30 s window:

    0.170423 0.018257 -0.020125 0.121258 0.113597 -0.053409

which is what the last receipt carried, to the last digit it printed. **Nothing in this work moved a
number.** The batched prefill lane answers `[Spanish]` on this fixture at tick 0 — it answered exactly
that on the kernels from before my first edit too (checked by putting HEAD~1's two files back and
running the same probe), so it is a standing condition of that lane and not something I introduced.

## The lanes, against their floors — and what I could not close

At the baseline reading, before any edit, with the machine at **388.52 GB/s**:

| lane | measured | floor | distance |
|---|---|---|---|
| encode | 4.05 ms | 0.21 ms | 18.96x |
| wtoken | 1.00 ms | 0.15 ms | 6.57x |

**No comparable after-reading exists, and I will not invent one.** By the time the work stood, the
lens read 178 to 188 GB/s and the lanes I never touched had moved with it — `1btoken` 12.6 ms to
86 ms, `4in1` 3.9 ms to 23 ms. Under that I ran the whole lane both ways, interleaved, putting
HEAD~2's two files back between runs: three rounds gave encode ratios of 0.95, 0.81 and 1.54 and
token ratios of 1.04, 0.85 and 0.95. The spread is ±50 percent and the effect I am looking for is
smaller than the spread. **At the lane level, today, the answer is: not measurable.** What is measured
is each kernel, adjacent, in one process — the table above — and that every reading the pass produces
is unchanged.

There is a real edge in this that the lane numbers hide. Five of the changes cost a launch (the
encode went 29 dispatches to 38, a token 35 to 40) to save far more work per launch. On a quiet
machine a launch is about 3 µs and the trade is free. On a machine where three other agents are
queueing, a launch is hundreds of microseconds and the trade is close. **Dispatch count is the
currency of a crowded GPU; bytes are the currency of a quiet one**, and this pass now spends more of
the first to spend much less of the second.

And one honest thing the lens cannot say for the encode: **its floor is not the memory floor.** An
8 s window is about 8.1 GFLOP. This M4 Max's forty cores give on the order of 16 TFLOPS of f32, so
the arithmetic alone cannot finish under about 0.5 ms — roughly **2.4x the lens's 0.21 ms**, which is
where that lane's real bottom is. The token is the opposite: 57 MFLOP against 59 MB, memory-bound
through and through, and 1.0x is a floor it can be walked toward.

## Still open

- The encode is compute-bound and the gemm folds at a few TFLOPS of sixteen. The next room is not
  the K loop (raced, refused) but the accumulator shape: eight accumulators a simdgroup over a 64x64
  tile, or activations in half, which would double the multiply and cost the sixth digit of enc.
- The spectrogram is still a 400-point DFT per frame walking a twiddle table; three gemms on the
  tiled kernel would move it.
- The encoder's row attention keeps `acc[64]` per thread in registers. A block form with the
  accumulator in threadgroup memory would free the occupancy that costs.
- A token still pays one full command-buffer round trip. The id could stay on the device — the argmax
  writing where the embedding reads — so a chunk of steps rides one sync; the lens measures the
  single step, so that gain would not show there.

## Receipt

The most surprising teaching: **the measurement, not the kernel, was the thing that needed a design.**
I came to make a pass faster on a machine that was quietly changing speed by a factor of six
underneath me, and my first three readings were all honest and all useless. The fix was not a quieter
machine — I do not get one, three siblings were working — but a comparison that carries its own
control: emit both shapes, dispatch them alternately in one process, and let the crowd fall on both.
The limit of that fix is exact and worth naming: it works for anything I can emit twice, and it does
not reach a whole lane, because a lane is not a kernel I can hold two of at once. That is why the
kernel table below has numbers and the lane table above has an honest blank.

Where discomfort became gold: I had a clean theory that the gemm was barrier-bound and I wrote the
twin to prove it. The race said no, twice, on five shapes of six. Sitting with that rather than
re-running until it agreed is what turned the same apparatus around and found the two shapes that
*did* win — the half weight tile and the eight-query group — neither of which I would have reached
from the theory I walked in with.
