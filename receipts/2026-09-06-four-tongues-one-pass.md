# Four tongues, one pass

The gap: the live transcript lane offers a heard line in four tongues through the dense
llama3.2:1b lane one after another, about 300 ms a tongue (receipts/2026-09-06-overlap-and-
tongues-validated.md). A token is 13 ms and a decode step reads every weight once, so M
sequences decoded together should cost about one sequence's time. Now they do: `form/native/
metal/dense-multi.fk` runs M sequences (1..8) through the fused block in one step, and
`form/native/metal/tests/dense-multi-band.fk` = 255 holds every sequence byte for byte against the
single lane.

**The matmat is the matvec, column for column.** `form/form-stdlib/q8-0-matmat-msl.fk`: the
sixteen-rows-a-group twin's structure (`q80-matvec-tg4-body`) with M input columns riding on it.
The loaders stage the WEIGHT `d * q` (exact in f32: an f16 significand times an int8 needs 19
bits) and the step's 128-column slice of every column of x beside it; a folding lane (row, m)
computes `p = w * x_m[j]` and folds `acc = p + acc`, k from 127 down to 0, steps descending — the
attestant's own j-descending order and the same two roundings, so column m of the matmat is byte
for byte the tg4 matvec over x_m. Folding lanes: 32 * ceil(M / 2); loaders: 64, a step ahead in
registers as before. Threadgroup memory is what shaped it: products for four columns
(4 x 16.5 KB) do not fit the 32 KB a group holds; weights plus the x slices (`pv[2][16][129]` +
`xs[2][8][129]`) are 24.8 KB. The same shape carries the residual add (`y = r + acc`), the q/k/v
triple with k and v landing in each sequence's cache bank at that sequence's own position, and the
gate/up/SwiGLU group (32 * M folding lanes, the up fold crossing threadgroup memory per column).
rmsnorm, the pair rope, the cooperative attention, the two-stage argmax and the embedding read run
their single-sequence text once per sequence on its own rows: group m, bank m, `pos[m]`.

Two variants were measured and set down, not guessed at: x read from device memory inside the
fold (down 2048x8192: 17 ms per forty against 6), and the products staged for M columns at eight
rows a group (28 ms). Staged weights and staged x won on every width.

**What the band asserts** (255, power-of-two bits): the door; the ten M-kernels compiled and the
file inside the radius; the matmat against tg4 on four synthetic columns, q and ffn_down, eight
read-backs equal of eight; sequence 0 of the M=4 run answering the sixteen ids dense-family-band
pins; four prompts of four lengths (6, 4, 9, 10 ids) each run alone through dth-prefill/dth-generate
and the batch's sixteen ids per sequence equal; the 513 KB of logits at each sequence's sixteenth
answer, batch against single lane, four equal of four; M=1 answering the pinned sixteen; M=8 with
the four prompts twice answering the M=4 ids per sequence — columns do not cross.

**Measured, warm** (`form/native/metal/dense-multi-run-llama1b.fk`, twenty steps after five, this
M4 Max, two runs agreeing within 0.2 ms):

| M | step | per sequence |
|---|---|---|
| 1 | 13.0 ms | 13.0 |
| 2 | 13.4 ms | 6.7 |
| 4 | 14.8 ms | 3.7 |
| 8 | 19.5 ms | 2.4 |

Four tongues as one batched decode: 14.8 ms a step against 52 ms for four sequential — 3.5x, and
a per-sequence step of 3.7 ms under the 4 ms single-sequence hardware floor, because the floor is
per pass and the pass is shared. The dispatch count a step is the fused lane's own 133, paid once
for M sequences. The four prompts through the batch: " Paris. The Eiffel Tower is located in Paris.
The Louvre Museum" / " 100 degrees Celsius at standard atmospheric pressure. At standard
atmospheric pressure, water boils" / " Jupiter. It is a gas giant, meaning it is primarily composed
of hydrogen and" / " nestled in the rolling hills of Tuscany, there lived a young girl named".

**Entry points.** `(dm-open path maxpos M)` — dth-open's geometry, views and pipes, this cell's
unit and M-row buffers, M-bank caches; refuses loudly outside the fused radius (dm-ok?).
`(dm-step st ids poss)` — one batched step, M ids in, M argmax ids out. `(dm-run st prompts ngen
rec)` — M prompts of any lengths in lock-step, each feeding its s-th prompt id while it has one and
its last answer after; answers the first ngen ids per sequence and, when asked, each sequence's
logits bytes at its ngen-th answer. `(dm-logits st m)` reads a sequence's logits after any step.

**Still open.** M=8 at 19.5 ms is 1.5x M=1 where M=4 is 1.14x: the fold's serial chain is now
128 * M lanes a group against 64 loaders, and past four columns the loaders wait on the folders;
a second loader simdgroup, or a 64-column step at M=8, is the untried room. The tongue lane itself
(`observe/ear-tongue-native.fk`, a sibling's) still offers its tongues one after another — the
door here is the step; wiring four tongue prompts through `dm-run` is the next movement. The
attention kernel stages 1024 positions; a pool past it routes nowhere yet in this lane (the
single lane falls back to the per-head attestant; this one refuses at open by maxpos).

The surprise: the M=1 matmat on ffn_down ran faster than tg4 itself (150 µs against 300 in the
same process) — staging x once per step instead of reading `x[j]` from device memory per product
was worth more than the column count, and the batching's gain came partly from a cost the single
lane had been paying without a name. Discomfort turned gold: the first timing said the matmat was
eight times slower than tg4 and the honest move was to put tg4 beside it in one process before
believing either number — cold, tg4 was four times its own receipt; warm, the ratio was 1.0-1.2
and the design stood.
