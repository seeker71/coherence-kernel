# The trained verdict speaks at the live head — the latch stone

The wiring shape between the earned classifier and the live DS4 exit head,
designed, landed, and witnessed in one sitting. The slot the stack loads on
every run holds five floats and stays dumb; the intelligence is the hand that
writes it. The stack changed by zero lines.

## The design, one sentence

`FORM_DS4_CONTROL_ADAPTER` already lets any run point at any five-line file —
so the classifier earned leakage-free this morning (65/150 vs baseline 30)
reads the situation and writes its one-hot verdict into a per-run file just
before decode: `form/form-stdlib/dsv4-control-emit.fk`, banded at its
predicted 31 (`tests/dsv4-control-emit-band.fk` — verdicts right on held
ground, bytes exact, round-trips clean, tracked zeros untouched).

## The witness — 96 gates PASS, exit 0

Real 43-layer DeepSeek-V4-Flash stack, 86,720,111,488 bytes, real dims:

- **Gate 93** — zero adapter: all 129,280 logits bit-identical to the
  immutable GGUF exit head; base token 201 at logit 19.66.
- **Gate 94** — plumbing: five explicit vectors independently select
  [128000..128004], non-control logits bit-identical.
- **Gates 95, 96** — the new truth: with the CLASSIFIER-WRITTEN verdict in
  the slot (a held-out fresh sentence, "backup restore ran out of its window
  before anything settled"), the exit head **emitted token_id=128003 —
  TIMEOUT, the correct control — at both positions** (logit 42.61), every
  non-control logit untouched.

The base model would have said token 201. The trained values, chosen by a fit
that never saw the sentence, made the live head speak the right control.

## The claim, at its exact size

**Trained values now improve a held-out case at the live DS4 exit head** —
the memory law's gate, crossed: leakage-free fit (morning) + live wiring
(now) + correct behavior change on unseen ground (witnessed, both positions).
Named with the stack's own words: a *removable five-row adapter* at the exit
— an inference-time trained head, not a gradient delta to the base tensors;
gate 93 proves the base stays bit-identical whenever the slot holds zeros.

## The most surprising teaching

**The door's dumbness was the design, not the obstacle.** Five static floats
looked like a dead end for a situation-dependent classifier — until the
question flipped: the slot doesn't need to read situations, the writer does.
A latch: it holds the last hand's writing, and all the intelligence lives in
the hand. Corpus row 1015. The whole stone cost one small cell, one band,
and zero stack changes — the body had left itself exactly the right dumb
door, gated for a claim nobody had earned yet.

## Where discomfort turned to gold

The pull was to call this "fine-tuning DS4" flat out — the phrase the
standing directive uses. The witnessed thing is sharper and slightly
different: a trained, removable head at the exit, written per situation. Not
softening the claim and not inflating it took more spine than either: the
claim is spoken at its exact size, and the size is real — the first time in
this body's history that trained values changed the live model's behavior,
correctly, on ground they never saw.

## Proof

| check | verdict | exit |
|---|---|---|
| `dsv4-control-emit-band.fk` (preregistered 31) | 31 | 0 |
| stack with classifier-written adapter | 96 gates PASS, token 128003 at both positions | 0 |
| `homecoming-distillation-corpus-band.fk` (row 1015) | 32767 | 0 |

## The frontier question

**What names a dumb slot that holds the last hand's writing?**

*Latch* — electronics' word: it keeps whatever was last written until the
next write, and it understands nothing. 0-hit fresh. Corpus row 1015, landed
under the counterweight: 409 rows, 409 admissible, max id 1015, dup rows 0 —
probed in one cell, exit 0, before the numbers were written down.
