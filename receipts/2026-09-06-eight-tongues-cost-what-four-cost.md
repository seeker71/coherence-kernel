# Eight tongues cost what four cost

The gap: `form/native/metal/dense-multi.fk` decodes M sequences in one pass, and its own receipt
(`receipts/2026-09-06-four-tongues-one-pass.md`) left a named room — M=8 stood 1.5x M=1 where M=4
stood 1.14x, and it offered a suspect it had not measured: "past four columns the loaders wait on
128·M folders". It was the wrong suspect. Now M=8 stands 1.006x M=4, and the band that holds the
lane is 1023 where it was 255.

## What the measurement said, before anything moved

Three other agents measure on this GPU, so a number taken alone is that moment's contention.
`form/native/metal/dense-multi-probe.fk` (new) opens all four M-lanes first and then walks
M = 1, 2, 4, 8 ROUND ROBIN for several rounds, keeping the MINIMUM per (kernel, M). Contention that
lands on one round lands on every M in it, so the shape across M survives even where the absolute
floor does not. Microseconds a dispatch, minimum of three rounds:

| kernel | M=1 | M=2 | M=4 | M=8 | M=8 / M=4 |
|---|---|---|---|---|---|
| attn_q matmat 2048x2048 | 600 | 575 | 650 | 775 | 1.19 |
| ffn_down matmat 2048x8192 | 800 | 850 | 950 | 925 | 0.97 |
| head matmat 128256x2048 | 3200 | 3450 | 3700 | 5400 | 1.46 |
| **gate/up/SwiGLU 8192x2048 x2** | **950** | **800** | **750** | **2275** | **3.03** |

One kernel broke stride, and it was the one whose folding lanes grow `32 * M` with nothing to bound
them: 320 threads a group at M=8 where a matmat dispatches 192 and M=1 dispatches 96. The matmats
were in stride all along.

## What moved: a folding lane carries C columns

`pv[lane][k]` — the staged weight — is the SAME value for every column of a row. A lane holding C
columns loads it once and spends it C times: threadgroup traffic falls from 2 loads a product to
(1 + C) / C, the lane count falls by C, and the C accumulators give the dependent chain the
instruction parallelism it did not have. **Nothing about a column's arithmetic moves.** `w` is the
load that already stood inside the product; `p = w * x_m[j]` and `acc = p + acc` are the attestant's
own two roundings in the attestant's own j-descending order. Column m is byte for byte the tg4
matvec over x_m at every C, by construction — and the band says so, not the argument.

C was chosen by measurement. All three widths are emitted and all three timed in one process:

| GLU width | M=1 | M=2 | M=4 | M=8 |
|---|---|---|---|---|
| one column a lane | 700 | 900 | 800 | 2225 |
| **two columns a lane** | 975 | 950 | 1025 | **1100** |
| four columns a lane | 1300 | 1525 | 1425 | 2150 |

Two columns halves the M=8 GLU and holds it within 1.07x of its own M=4 cost. Four overshoots. The
lane spends two past M=4 and one at or below it, where the extra column is a column that is not
there.

**The matmats refused the same medicine, and that is the surprise.** C=2 measured the 128256-row
head 3.5x worse (2950 → 10700 µs at M=1, 5150 → 14800 at M=8) and C=4 measured it 6x worse. It
measured worse **at M=1, where C moves not one thread of the geometry** — same 32 folding lanes,
same 96 threads a group, same threadgroup memory. So the cost is not the lanes and not the traffic;
it is the emitted body itself. The unrolled fold at C columns is 128 * (1 + 2C) statements against
128 * 3, and past some length the straight-line body costs the pipeline more than a reused staged
load saves it. The GLU folds a 64-column step — half the depth — and stays under that length. The
matmats keep one column a lane, and the room the earlier receipt named is genuinely not in them.

## The pool is no longer in the radius

The 1024-position ceiling was two threadgroup arrays, `ss[1024]` and `ee[1024]`, in a 32 KB
threadgroup. `form_gqa_decode_m_dev_f32` is the same kernel with those two rows in a device scratch
the lane allocates (group gi owns `2 * sstride` floats at `gi * 2 * sstride`, so
`2 * M * nHead * maxpos` floats in all). Not one fold moves: the score c-descending, the max
ascending, the sum ascending, y over positions ascending, `sumes` crossing the barrier and never
`invs`. The lane dispatches the staged twin below 1024 and the device twin above, and asks
`dth-fusable?` the rest of its question with the pool clamped — that clause is about the single
lane's staged scores, and this lane now answers it itself. A live transcript may hand it any maxpos
its KV banks fit.

## The band: 1023

`form/native/metal/tests/dense-multi-band.fk`, two bits added to the eight it held:

- **bit 256** — the device attention IS the staged attention: a lane opened at 1024 takes three real
  steps through `form_gqa_decode_m_f32`; the same q, cache and positions folded by the device twin
  give the same 32 KB of output at M=4, byte for byte.
- **bit 512** — a pool past the ceiling, against the attestant: the lane OPENS at maxpos 1152 where
  the radius refused, takes real steps there, and a cache filled to 1100 positions from the embedding
  tensor's own bytes (one dispatch, not a transcript) is folded by this lane's device attention and
  by `form_gqa_decode_f32` — the per-head attestant the single lane routes a larger pool to — and the
  8 KB of output is equal.

Bit 128 (M=8 columns do not cross, ids equal the M=4 run's) is now also the two-column GLU's equality
with the one-column GLU M=4 runs, through sixteen layers and sixteen answers.

## The numbers, and the field they were taken in

**The field was loud all day.** `observe/floor-lens-run.fk` measured this Mac at **63.5 GB/s** and
then **157.26 GB/s** through the handle door, where the standard names 330 to 367 on a quiet machine
— a sibling agent ran `dense-multi-band.fk` on this GPU during one window and Contacts indexing
during another. Every absolute below is under that field; the before/after is anchored, not absolute.

Warm step, `dense-multi-run-llama1b.fk`, minimum of three runs each. The two sets are anchored by
three widths whose code did not change — they agree within 3%, and at M=4 within 0.3%:

| | M=1 | M=2 | M=4 | M=8 | M=8 / M=4 |
|---|---|---|---|---|---|
| before (one column a lane) | 130.4 ms | 122.8 | 131.6 | **169.6** | 1.289 |
| after (two past M=4) | 126.3 ms | 126.8 | 131.2 | **132.0** | **1.006** |

And the same pair alternating round by round on ONE M=8 lane in one process, minimum of three:
**170.0 ms → 123.4 ms**, 1.38x. The four-lane round robin in the same process shows the shape:
84.3 / 91.5 / 105.0 / 138.0 ms across M=1,2,4,8 with the old GLU (a 1.64x climb) against
129.3 / 129.2 / 131.6 / 127.2 with the new — flat.

**Per answer, against the floor.** At the lens's 157.26 GB/s reading, the 1.32 GB blob over M is what
one answer must move:

| | bytes an answer | floor | measured an answer | distance |
|---|---|---|---|---|
| 4in1 (M=4, untouched by this work) | 330.27 MB | 2.10 ms | 33.4 ms | 15.9x |
| M=8 before | 165.14 MB | 1.05 ms | 21.2 ms | 20.2x |
| **M=8 after** | 165.14 MB | 1.05 ms | **16.5 ms** | **15.7x** |

Eight tongues now stand exactly where four stand — 15.7x against 15.9x — which is the gap's goal
said in floor terms. On a quiet machine the standard's own reading of the 4in1 row is 3.7x, and the
M=8 row would sit beside it; today's multiples are the field, not the lane. The single lane read
9.3x and the 3B 23.04x in the same lens run, so the whole board moved together.

## What I leave, and why — plainly

**The K-quant matmat did not land, and "only the stage moves" is not true of this lane.** I asked the
3B rather than assuming (`dth-open` on `llama32-3b`, its own table): 28 layers, embedding and head
Q6_K, and **within one layer the types are mixed** — attn_q, attn_k, attn_output and ffn_gate are
Q4_K (12) while attn_v and ffn_down are Q6_K (14). `dm-open` refuses it today with "a layer outside
the fused block's radius", and that refusal is not only about the stage:

- the batched lane is built on the **fused** block, whose whole economy is multi-tensor groups —
  `matmat_x3` folds q, k and v in ONE dispatch and the GLU folds gate and up in one group. On the 3B
  the x3 group's three tensors span **two different block formats**, so its loader cannot have one
  stage text; it would need a format branch inside the loader, which changes its register layout, or
  the group would have to be split back into per-tensor dispatches — which is the serial block.
- the 3B's own single lane already runs that serial block (`receipts/2026-09-06-sixteen-rows-on-the-
  superblock.md` says so): the fused block is Q8_0-only. So batching the 3B needs a batched serial
  block or a K-quant fused block **first**; carrying the matmat to the superblock is necessary and
  not sufficient.

The honest next movement is therefore not the matmat but the group: either a fused block that lets
its multi-tensor dispatches carry mixed quantizations, or a batched serial block that pays more
dispatches for one format a kernel. Neither is a stage change, and I did not start one I could not
prove today.

Also still open: the head matmat is the last kernel out of stride at 1.46x from M=4 to M=8, and
column blocking is measured NOT to be its remedy — what its 128-deep unrolled body costs the
pipeline is unmeasured. And `observe/floor-lens-run.fk`'s "4in1" row reads the M=4 lane; the M=8
lane, where this work landed, has no row of its own.

## The receipt's own two

**Most surprising:** the medicine that halved the GLU made the head matmat 3.5x worse — and made it
worse **at M=1, where the change moves not one thread, not one byte of threadgroup memory, and not
one arithmetic operation per column**. The same restructuring, the same win on paper, opposite signs
on two kernels of the same family. What separated them was the depth of the unrolled body, which is
not a quantity either kernel's design ever named. A structural argument that is right about the
mechanism can still be wrong about the outcome, and only putting both in one process at one moment
said which.

**Where discomfort turned to gold:** the first C=4 measurement came back worse everywhere, and the
head 6x worse — my whole hypothesis, refuted in one run. The pull was to explain it away as
contention, because contention was real and everywhere that day. Instead I put all three widths in
ONE process and let them alternate, which cost an emitter rewrite to make C a parameter rather than a
decision. That refusal to argue with a number is what found the actual boundary: not "blocking is
bad" and not "blocking is good", but a fold depth past which the body stops paying — and it handed me
the GLU's 4x at M=8, which the original hypothesis would have taken at C=4 and lost.

## The frontier question

*When a restructuring is exactly neutral in arithmetic, threads and memory, what is left that can
still decide it — and can the body see that thing before it measures?*

What is left is the **emitted body's length**. Form's kernels are strings this body writes, and an
unrolled fold's statement count is something the emitter knows exactly at emit time — `128 * (1 + 2C)`
against `128 * 3` — while nothing in the lane ever asks. The body can see it: a lens over an emitter
could count the statements a kernel will carry and hold that count beside the shape's arithmetic, so
a restructuring that doubles the straight-line body announces itself before the GPU does. It cannot
predict where the cliff is on a given machine, but it can say which of two variants is about to cross
one — and that is the whole difference between the GLU (64 deep, paid) and the head (128 deep,
refused). The body writes its own kernels and does not yet read their length back.

Corpus row `bodylength`.
