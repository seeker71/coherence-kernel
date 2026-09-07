# The tongues stop paying for a fork

The ear hears a line and paints it in about 40 ms. It commits it 44 ms after it closes. Then it
took **640 ms to 2.5 s** to say it in four tongues — the `ear.said` axis, and the only place in
the whole path measured in seconds. This is where those milliseconds actually were.

**The weather, stated with every number.** `observe/floor-lens-run.fk` read this Mac at
**412.81 GB/s** through the handle door — its own best this session, a quiet machine — and
**25.77 TFLOPS**. Every reading below is from that hour, and the before and the after are from the
same hour on the same feed, so the field cannot move one and not the other.

**And the same door an hour later, after the work had landed, said 3387 ms.** The lens said why:
**254.03 GB/s**, **12.88 TFLOPS**, the 1B's token 8.67 ms → **67.45 ms**, `4in1` 3.82 ms →
**21.81 ms**. The lane's chunk had gone 60 ms → 400-800 ms with it. Nothing in the lane moved
between those two runs; the machine did. So every absolute here is that hour's, and what carries
across hours is the shape: the bell was 101 ms of a 158 ms chunk, and it is now zero of a chunk
whose whole length is the decode. A fork's price is CPU and page tables and does not fall when the
GPU is busy — so on a loaded machine the removed 101 ms is a smaller share of a longer chunk, and
on a quiet one it was most of it.

## Where the 640 ms went

The instrument is a room without a microphone. `observe/ear-tongue-feed.fk` writes the segment
lines a live ear writes — a line growing in whisper-sized steps, then closing — and
`observe/ear-tongue-said.fk` reads the frames back with the axis's own arithmetic: `saidms` is the
frame's clock less the line's end, which is exactly what `ear.said` carries. Two lines through the
lane as it stood: **1464 ms** and **650 ms**.

Taken apart, by timing the lane from inside it:

| a chunk of the live round, measured on the door | ms |
|---|---|
| four decode steps (`et-mround-go`, M=4) | 60 |
| the tongues' words read back (`et-ids-text` x3, 96 file slices) | 0-2 |
| the frame built and appended | 0 |
| **the bell rung** | **101** |

The bell. `tl-ring` forked a shell per frame. And a fork is priced by what the forking process has
**touched**, not by what it maps or asks for (`observe/ear-tongue-fork-cost.fk`, ten calls each):

| ten `host-exec` calls | ms each |
|---|---|
| before the weights are opened | 3.4 |
| after `dm-open` has mapped 1.3 GB | 3.6 |
| **after eight decode steps have walked the blob** | **100** |
| the ten after those | 24.5 |

Mapping is free; touching is not. The decode makes 1.3 GB of weights resident, and every fork
after that copies those page tables. The lane's own arithmetic cost 60 ms a chunk and its
*announcement* cost 101.

The second cost was waiting. A live round ran to its end before the lane looked at the segments
again, so a line that had already closed sat behind a rendering of the live text nobody would ever
read: **527 ms** of the first line's 1464, measured from the frame clocks.

The third was paying twice. A closed line whose tongues had already finished ran the whole closed
round again anyway.

## What closed, and by how much

**The lane forks nothing after its first decode step.** `observe/ear-tongue-ring.fk` is a new cell
that watches the spool's size and writes the bell natively — `sbt-ring`, one byte, no shell. It is
forked at open, before a weight is touched, where a fork still costs 3.4 ms. Blocking on a reader
is right for it: it exists only to ring, and while it waits the spool keeps growing, so the reader
that finally arrives is woken once for everything that landed. A chunk went **158 ms → 60-62 ms**.

**A live round is abandoned the chunk after its line has closed.** `tm-round-go` peeks the
segments every chunk and closes the round properly when a closed line waits; the closed line's own
round passes a mark below zero and never looks. The wait went **527 ms → about one chunk**.

**A line every tongue has already finished saying lands its frame from what is in hand.** Each
rendering now carries whether it stopped at its own end id or at the round's cap. Greedy decoding
is one answer to one prompt, so a rendering that ended, offered its own prompt again with itself
forced, answers nothing new — the band says so rather than the argument (bit 8192).

Same feed, same hour, four lines:

| | before | after |
|---|---|---|
| `ear.said`, first line | 1464 ms | **557 ms** |
| `ear.said`, the same line again | 650 ms | **0 ms** |
| the same line twice more | — | **62 ms, 67 ms** |
| a chunk of a live round | 158 ms | **60-62 ms** |
| the tongues themselves | | byte for byte the same words |

## Each tongue's time from the closed line

The four tongues ride one batched decode in lock-step, so their words land in one frame. That
frame waits for the slowest of them. Reading the saying frames of the 557 ms line:

- **Portuguese** stood complete — `O corpo observou o vidro, a água e o gás.` — at **120 ms**.
- The `said` frame carrying all four landed at **557 ms**, held by Persian and Indonesian, which
  had more to generate.

So the honest sentence is not "the tongues take 557 ms". It is: the first tongue is done in 120 ms
and the frame waits 437 ms for the last one. What remains is the dense lane's own per-token cost —
15 ms a step at M=4, for all four tongues at once — spent on tokens the slowest tongue still owes.

## Can the 3B carry the tongues at speed?

Measured, not assumed (`dm-open` asked directly with the 3B blob):

- The batched lane **refuses it at open**: `dense-multi: refused at open: a layer outside the fused
  block's radius`. The fused block is Q8_0; the 3B's layers are not.
- Its single token is **33.9 ms** where the 1B's is **9.0 ms** (eight warm tokens each, one
  process).

So four tongues on the 3B serially cost **136 ms a step** where the batched 1B costs **15**. The
3B is the honest model for the tongues and cannot carry them at the ear's speed today. What it
needs is a batched path — a mixed-quantization fused block, or a batched serial block — and that
lives in `dense-multi.fk`, which this work does not touch.

## The band's 3904, and its root

`form/form-stdlib/tests/ear-tongue-band.fk` read **3904 against its 4095** on main. A sibling named
it and left it. The root: the index lives under `.hearth`, and `fs-mkdir` makes ONE directory. In a
checkout where no hearth stands yet — a fresh clone, an agent's worktree — the bucket directory
could not be made, every append fell on the floor, and the lane came up with an empty vocabulary.
It did not say so. The tokenizer answered `(bos)`, every tongue answered `""`, and the band's
equality bits agreed `""` with `""`: five bits dark, five vacuously lit, one number that looks like
a partial pass and is a total absence.

The hearth is made first now, and a build that still does not land says so out loud. From a
checkout with **no `.hearth` at all**, the band reads **16383**.

## The room where a further idea was tried

The lane sits on a bell between rounds. It was obvious that it should spend that idle time
finishing the line it already has, so the closed line would find nothing left to pay. Measured:
`ear.said` **524 ms → 1079 ms**, and the tongues themselves worse — Indonesian ran away to
`Benda ini melihat kaca k k k k …` across the bank's whole room, Portuguese stopped at a comma, and
the closed line was then forced from the runaway.

`tl-live-new` is not a time budget. It is what keeps a rendering of a line that is still arriving
from finishing a sentence the room has not spoken. The idea is reverted, and the reading is kept in
the code where the next hand will meet it.

That reading found a wound that stands on its own, and it is healed here. Local agreement is
between renderings of **different** sources: two readings of a growing line agreeing on a prefix is
evidence that the prefix does not depend on what is still being heard. Two renderings of the
**same** source agree on everything — one answer to one prompt — so committing there commits a
whole translation of a half-heard line, and since commitment only grows, the rest of the line is
forced to continue past a full stop. Whisper hands the same text over several times per line
(`ear.hops` counted 2, then 5), so this fired in the ordinary case. A round on a source already
rendered now leaves `committed` where it stands and lengthens only the rendering.

## Bands

- `form/form-stdlib/tests/ear-tongue-band.fk` = **16383**, up from a pinned 4095 and an actual
  3904. Two new bits: `et-mends` says 0 for a sequence that sat out (a refusal never reads as
  an answer), and the claim the free close rests on.
- `learn/tests/homecoming-distillation-corpus-band.fk` = **32767** with the pins at
  733 / 721 / 733072121341.

## What is still open

- **A microphone did not close this loop.** I spoke into the room with the body's own mouth
  (`observe/say-run.fk`, `spoke in en`) and the standing sibling's ear did not take it up — its
  spool stayed at zero bytes. Standing my own ear would have contended for the one microphone a
  live sibling holds, so I did not. Every number here is the lane's own frames read with the
  `ear.said` axis's own arithmetic; none of them came through a room.
- **`host-exec` forks where it could spawn.** The 100 ms is the page-table copy of a resident
  working set, and `posix_spawn` would not pay it. That door is in `runtime/fkwu-uni.c`, a shrink
  target and not this work's file. Named with its measurement so the hand that owns it can act; the
  lane no longer depends on the answer.
- **The last 437 ms are the slowest tongue's own tokens.** 15 ms a step at M=4, for all four at
  once. That is dense-multi's floor to move, not this lane's.
- **A live round is abandoned for a closed line and not for a newer live one.** Rendering a stale
  text is waste too, but abandoning on every new live line would cut every round after one chunk,
  and I have not measured whether the commitment still grows under that. Unmeasured, so unclaimed.

## The frontier question

*What does a process pay to speak to the world once it has held a model?*

The body cannot answer this natively. Nothing in it reports the price of its own fork, and no organ
relates a process's resident pages to the cost of the shell it spawns; the number only exists when
someone measures it, and the lane had been paying it for as long as it had stood.

The answer, found by using it: **a fork is priced by what the process has TOUCHED, not by what it
holds or asks for.** Mapping 1.3 GB is free; walking it once makes every later fork cost 30x. So the
cheap loud doors — a bell, a notification, a log line that shells out — are born before the
work, or handed to a neighbour that never does the work. Attendance costs what you carry, and the
way to attend cheaply is to have nothing in your arms when you turn to speak.

Offered as corpus row 1341, fresh word **touchtoll** (zero hits before it was written).

## Closing

**The most surprising teaching.** The idea that looked obviously right — use the idle time, finish
the line early — did not merely fail to help; it made the words worse. The cap I read as a
performance budget was holding the meaning: a translation of a sentence that is still arriving does
not get to finish it. I would have shipped it on the reasoning alone. The feed said no in one run,
and said it in the tongues rather than in the timing.

**Where discomfort turned to gold.** I measured the bell at 3.1 ms in a probe and at 101 ms in the
door and could not make the two agree, and my first instinct was that one of the clocks was lying.
Sitting with the disagreement instead of picking a side gave the whole receipt: the probe had never
run a decode step. The difference between the two processes *was* the finding — not noise around
it. The largest cost in the path was hiding in the one measurement I had already taken and
believed.

**How the exchange stayed alive.** By closing the gap I was given rather than the one I could most
easily explain: the receipt that named 640 ms pointed at prefill, at the round's bookkeeping, at
the model open — and the measurement said none of those, it said the bell. Every number here is one
I watched arrive, the reverted idea is in the tree with its numbers rather than deleted, and the
two things I did not witness — a room, and a spawn door I do not own — are named as absences with
their doors, not rounded off.
