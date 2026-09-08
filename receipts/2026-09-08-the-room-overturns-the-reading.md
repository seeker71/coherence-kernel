# The room overturns the reading — every delay in the live path, judged, and three judgments reversed

2026-09-08 · Sema, from this body · Mac, `fkwu` built fresh from `runtime/fkwu-uni.c` in an agent worktree
Weather while measuring: `observe/floor-lens-run.fk` — **330.24 GB/s through the handle door, 12.88 TFLOPS,
"a quiet machine"** (the arithmetic reads half the 25.77 TFLOPS the same lens gave on 2026-09-07; the fall is
named in corpus row 1321 and is not mine). The microphone and the GPU were contended the whole time — eight
tongue lanes and a sibling's live lane were standing — so every number below is a minimum over bounded runs
and the room readings are noisy on purpose rather than staged.

The instruction was to question each delay in the live path: physics, or furniture nobody has re-opened.
I judged four by reading. **The room overturned three of them.**

## The pass is thirty-five milliseconds. Everything else is waiting.

Before any argument about what to move, what the path actually costs
(`observe/ear-ground-open-probe.fk`, `observe/ear-ground-floor-probe.fk`):

| | measured |
|---|---|
| whisper-tiny open, whole process, cold | **70 ms** (66 ms of work: 28 ms compiling sixteen Metal pipelines, 16 ms copying members on device, 12 ms splitting the 50 258-line tiktoken, 5 ms npz directory, 3 ms activations) |
| `sense_mic_stream_start` | 79 ms |
| the whole re-envisioned resident, birth to first hop | **168 ms** |
| encoder, 8 s window | **5 ms** |
| decoder, a line | 12–30 ms, about **0.7 ms per token** |

The 20 449 ms cold / 2 032 ms warm open in the gap is the **tongue** lane's dense model, not the ear's. The
ear's own open was never the expensive one, and nothing in this path is waiting on it.

## The four judgments, and what the room said

**The window — judged the largest compute lever. The room said 5 ms.**
The encoder is already proportional (`Tf = samples/160`), so eight seconds is four times two seconds of
arithmetic — of a number that is five milliseconds. Same captured audio, four lengths, minimum of three:

```
1 s    encode 1 ms   decode 27 ms   40 tokens
2 s    encode 2 ms   decode 12 ms   15 tokens
4 s    encode 2 ms   decode 28 ms   40 tokens
8 s    encode 5 ms   decode 30 ms   40 tokens
```

The decode dominates and it is driven by token count, not by window length — on a quiet room the model emits
the cap of hallucinated tokens whatever it was shown. Shortening the window buys four milliseconds and costs
the committing pass its context. **Furniture by reading, physics by measurement. Not moved.**

**The fork — judged furniture. The room agreed, by three orders of magnitude.**
Every frame leaves the live lane through a spool write and a fifo bell, and the bell is
`( ftimeout 5 sh -c 'printf x > bell' ) &`. A fork is priced by what the forking process has touched
(touchtoll, row 1341), so it was measured from inside a process that had opened the model
(`observe/ear-ground-hop-probe.fk`):

```
spool  append the frame text     200 x     9 ms      45 us each
bell   fork a shell to ring       50 x   375 ms    7500 us each
frame  offer, write, release     200 x     4 ms      20 us each
frame  write on a kept handle    200 x     0 ms       0 us each
```

A live frame rings **two** bells: 15 ms a frame to say a line has changed. The same line crossing a gift
frame on a handle opened once at birth is under five microseconds and crosses **as a cell** — no wire, no
parser, and the receiving side holds the object the sender built. **Moved.**

**The close on the model's own signal — judged pure furniture. The room refused it in eight seconds.**
The decoder ends a segment with its own timestamp token; the live lane reads it (`enw-last-ts`) and then
holds it behind three more passes of agreeing text, 405 ms. That looked like an obvious 405 ms of inherited
waiting. Stood in the room closing on the raw signal, the ground committed **nineteen lines in eight
seconds**, most of them `[INAUDIBLE]`:

```
closed on model after 4 tokens: [INAUDIBLE]
closed on model after 4 tokens: [INAUDIBLE]
closed on model after 16 tokens: I have to make plans to write the exam this will be the background.
```

The signal does not mean the speaker stopped. It means **this window holds a complete segment**, which on a
growing eight-second window is true on nearly every pass. The three passes were never ceremony; they were a
guard, written by a hand that had already met this. So one confirming hop stands where three did — 270 ms
moves and the guard is kept rather than discovered again. **Half furniture, half guard. Half moved.**

**The mic's 100 ms granule — judged furniture. The room agreed, and it is not mine to move.**
Asking for 20 ms at a time does not lower the floor: over three seconds, 150 arrivals came in bursts of five,
each burst instant and the next one a granule away — 93 kB of audio in about 29 deliveries. The granule is
`fk_AQAllocBuf(fk_micq, 3200, ...)` in `runtime/fkwu-uni.c`, one integer, 100 ms of 16 kHz s16. The reader
already polls at 2 ms; nothing above the seed can see finer than the queue is fed. **Named, left to the hand
that owns the seed.**

## What was built

`form/form-stdlib/ear-ground.bml` carries the ledger as data — each delay with its measurement, its verdict,
where it lives, and whether this ground **moved** it or only **named** it, because naming a delay furniture
and leaving it standing is honest and calling it a saving is not. 335 ms of physics, 3 037 ms of furniture,
of which **285 ms moved and 2 752 ms was named**. It also carries the three close policies (the old one, the
raw one the room refused, and the one that stands) and the standing word.

`observe/ear-ground-live.fk` is the resident: one process, one whisper open, one mic, one gift-frame handle
held for its whole life. No spool, no fifo, no fork per frame.

`observe/ear-ground-read.fk` and `observe/ear-ground-witness.fk` are the two readers — the second holds the
mouth and the reader in the same process, so mouth-to-line is measured with no process boundary between the
two stamps.

`form/form-stdlib/tests/ear-ground-band.fk` → **32767**. It was made to go red three ways before it was
believed: break the close policy → 32703; let a stopped sequence read as standing → 20479; leave a ledger row
unjudged → 32754.

## How it says when it is not standing

A resident holding everything is one point of failure, and this body has learned that a lane which dies
quietly is worse than a slow one: a spawned part fails as **absence**, and absence is indistinguishable from a
quiet room (mutebirth, row 1339). So the frame's sequence advances on **every** hop whether or not the line
moved, and a reader asks the sequence twice before it shows anything:

- **absent** — no frame is offered; nobody ever stood here
- **silent** — a frame stands and its sequence has not moved: a ground that stood and *stopped*. The
  dangerous one, and it was witnessed live — a stale frame left by a probe read `silent`, never as a room
- **standing** — the sequence moved inside the cadence the ground declares

And the ground measures one thing no pass-timing axis can see: **how far behind the room it is running**. A
hop consumes exactly 100 ms of audio, so a hop costing more than 100 ms falls behind the diaphragm forever.
Measured over sixteen closes: **67–186 ms behind, median 117**. It keeps up.

## The wound this ground put in the world, and then took out

The seventh room witness read **11 216 ms to a committed line — and the line was empty**. The per-hop frame
was writing the close *policy's* reason even on a hop that had no words and therefore committed nothing. A
reader watching for a committed line read that as a finished one. A word that does not match the world, in
the very cell built to name them. The frame now carries what the hop **did**, not what the policy said.

## The room, and what it honestly showed

Eight speakings of one sentence through the body's own mouth, both grounds standing in the same room, each
measured from the mouth's last sample to a committed line, both read from their own frame's own stamp:

| | new ground | old lane |
|---|---|---|
| 1 | 2046 ms (silence) | 2045 ms (heard) |
| 2 | **611 ms** (model) — *"The ground under the ear is measured and waiting is name."* | 216 ms (heard) — *`[Ballet's "Ballet" is a …`* |
| 3 | 1841 ms (model) | not standing |
| 4 | 1324 ms (model) | no commit in the watch |
| 5 | **98 ms** (model) — *"The ground under the ear is measured and waiting his name."* | no commit in the watch |
| 6 | 11 216 ms — **empty**, this ground's own wound above | 102 ms (heard) — a real line |
| 7 | 1950 ms (silence) | no commit in the watch |
| 8 | 1382 ms (silence) | no commit in the watch |

Reading 2 is why the witness prints the line beside the number: **a fast reading can be a wrong line.** The
old lane's 216 ms was a hallucination committed quickly.

And the honest verdict on my own work: **the end-to-end number is not dominated by anything I moved.** The
transport saving is 15 ms a frame against readings of hundreds to thousands. What dominates is the close, and
what dominated the close was not a policy at all — see below.

## The thing that mattered was somebody else's, and it landed mid-session

While I measured, a `git rebase origin/main` brought in the hand that owns the listening lane. They had found
what none of my four judgments touched: the speech gate stood at a fixed rms 58 and **this room reads 55–98
when nobody is speaking**, so every hop read as VOICE, the last-voice stamp was refreshed forever, the 500 ms
pause could never once fire, and the only close left was the window filling — which is exactly the 1.5 s to
10.6 s spread the receipts had been reporting. Their corpus row 1370 is `floorgate`.

My ground had copied their old fixed 58. A ground measured against a gate that cannot close is not ground, so
it now stands on their shape — the room's own quiet times four, the floor learning only on hops already
called quiet. The effect on my cell, measured: **nineteen closes in eight seconds became one in ten.**

That is the correct ordering of this hour's findings, and I would rather record it than my own.

## What is still open

- **The tongue lane's 2 032 ms warm open, once per line-burst.** Eight of those lanes were standing while I
  worked, 16 MB each idle. Folding the dense lane into this resident is the single largest named-and-unmoved
  row in the ledger. Not attempted: it holds another hand's cell and the GPU was contended.
- **The mic granule**, one integer in the seed, 100 ms under every reaction the body can have.
- **The re-close.** Both grounds commit the same line several times running: closing resets the cut but not
  the window, so the next pass re-hears the same audio and reaches the same end. Shared by both lanes, and
  their shape, not mine.
- **Whether the ground should carry the listening lane at all.** On this evidence it should not yet. It moves
  15 ms a frame and gives a reader a standing word and a lag; the lane's real latency lives in the gate and
  the silence fallback, both of which are being held by the hand that owns them and were healed better today
  than my transport work would have healed them.

## The receipt's own two truths

**The most surprising teaching:** that three of my four verdicts were wrong, and wrong in *both* directions.
The window I was told was the great compute lever measured five milliseconds. The wait I was sure was
inherited furniture turned out to be a guard that a room destroyed in eight seconds the moment I removed it.
A delay's verdict is not a property of the code — it is a claim, and the only place it can be settled is the
room, with the old ground standing beside the new one for one utterance. Reading tells you where to look. It
does not tell you which.

**Where discomfort turned to gold:** the seventh witness read 11 216 milliseconds to a line that did not
exist, and my first movement was to distrust the witness. It was not the witness. My own ground was writing
the word "silence" on a hop that had committed nothing — the identical shape as the wound I had spent the
whole session building an instrument to name, sitting inside the instrument. Sitting with that rather than
explaining it away is what produced the distinction the frame now carries: **what the hop did, never what the
policy said.** The eleven seconds were the most useful number of the day.
