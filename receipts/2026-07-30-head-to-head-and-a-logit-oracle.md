# 2026-07-30 — the head-to-head, and a 129 280-element oracle

The generation lane now runs on the mainline weights, and for the first time in this work there is a
reference on the *same* file to hold it against.

## Applying `firststop` before spending a run

Row 937 landed an hour ago: a failure names where it stopped, never how many stops remain. So before
running the generation lane I enumerated what it touches that the diagnostic lane does not — the exit
head (`gpuMx8`, already type-8 aware), `embedToken` (F16, matches), the KV arena (gate 5 proven),
`gpuAttend` (reads the arena, not a weight). Nothing missing. **The run then passed first time**, which
is the first time this session that a lane came up without an intervening failure.

```
VERDICT PASS  112 gates — 43 layers, generation, mainline weights
BOUNDED AUTOREGRESSIVE TOKEN FEEDBACK (14 steps): [270, 6102, 295, 8760, 305, 305, 1009, 14, 6102, 305]
```

## The head-to-head, identical weights

```
prompt              "The capital of France is"      ids 671 6102 294 8760 344
ds4  (reference)    "Paris"
ours (Form-native)  " the capital in France and and its, capital and"
```

Ours is *topically* right — capitals, France — and grammatically broken. Set that beside what the same
code produced on the reap25 file two days ago: *" to the detection Specialists Protocol detection
protocol setName"*, a hard three-cycle with no relation to the prompt at all.

**Same kernels, same harness, different weights, and the failure changed character.** That is
information I could not have had with one file.

## And now a logit-level oracle

`ds4 --dump-logits` writes the full next-token distribution as JSON — **129 280 elements**, 1.5 MB, for
the same prompt:

```
argmax 2581 "We"   36.7579        <- ds4 opening its reasoning
       671  "The"  26.3893
        42  "H"    26.0521
      1350  " We"  24.5809
ours:  270  " the"
```

Their #2 is `"The"` and ours is `" the"` — adjacent in the vocabulary, adjacent in meaning, and wrong.
A forward that were structurally broken would not land there. This is a degraded signal, not a
misassembled one, and it can now be measured element by element rather than described.

## The most surprising teaching

**One broken example tells you that something is broken; two that break *differently* tell you what
kind.** The reap25 file gave a prompt-independent cycle; the mainline file gives prompt-dependent,
topically-correct, grammatically-broken text. Neither reading alone distinguishes "the pipeline is
misassembled" from "the pipeline degrades signal" — the *pair* does, immediately, and it cost one
download rather than one more instrument.

`secondspecimen` — 0 hits before this row, as are `twospecimen` and `symptomvary`. For three days I
tried to make one specimen say more by building better gates around it. The cheaper move was a second
specimen, and it was available the whole time.

## Where discomfort turned to gold

Watching ds4 answer "Paris" in about a second, on the file our lane had just spent 43 layers producing
" the capital in France and and its" from. There is no way to dress that up, and the honest response is
that it is the *best* news of the session: for three days every DS4 verdict here was `selfgauge`, and a
number that can be embarrassed is a number that can be fixed. The discomfort worth keeping is that I
built four instruments to interrogate a lane I could not check, when the thing that made it checkable
was a file I had declined to download twice.

## Ground stamp

```
metal_dsv4_stack.sh, mainline blob, 43 layers, KV_SEQUENCE=1, KV_STEPS=14
  -> VERDICT PASS 112 gates, 124 PASS lines
  emitted [270, 6102, 295, 8760, 305, 305, 1009, 14, 6102, 305]
     -> " the capital in France and and its, capital and"
ds4 --metal --temp 0, same prompt -> "Paris"
ds4 --dump-logits -> 129280 logits, argmax 2581 "We" 36.7579, then 671 "The" 26.3893
tokenizer: ids 671 6102 294 8760 344 round-trip on BOTH files — one vocabulary
```
