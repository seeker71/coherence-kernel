# 2026-07-28 — correctly prompted, and still wrong

Urs: **"we can find on the internet how to prompt those models, we don't have to guess."**

So I stopped guessing. The answer was already on this disk.

## The format, from the model's own card

`~/models/ds4-engine/MODEL_CARD.md`, lines 145–180:

```
<｜begin▁of▁sentence｜><｜User｜>…<｜Assistant｜></think>
```

The immediate `</think>` selects non-thinking mode.

## Our tokenizer cannot write it

Encoding that string returns **34 ids where it should return ~15**, because `tkz-encode` BPEs the
markers as ordinary text: `30="<"  28217="｜"  8277="begin"  2154="of"`. There is no special-token
pass before BPE.

So I found the ids by scanning the **vocabulary** instead. Id 0 renders as `<eginfentence` — that
is `<｜begin▁of▁sentence｜>` with characters dropped **by our own renderer**:

| id | token |
|---|---|
| 0 | `<｜begin▁of▁sentence｜>` |
| 1 | `<｜end▁of▁sentence｜>` |
| 128803 | `<｜User｜>` |
| 128804 | `<｜Assistant｜>` |
| 128821 / 128822 | `<think>` / `</think>` |

**Two defects, one root:** the encoder cannot emit these, and the renderer mangles them. Both are
ours, and both stem from 3-byte UTF-8 handling (`｜` U+FF5C, `▁` U+2581).

## The correct prompt, assembled by id

```
0 · 128803 · <ascii text ids> · 128804 · 128822
```

26 steps, 171 s, **VERDICT PASS, 124 gates**. Asked *"What is the capital of France? Answer with one
word."*, DeepSeek-V4-Flash through this body's lane answered:

> ` Protocol syn Political with the Protocol Politics के オリ Participate in`

## The finding, re-established

Earlier today I claimed the forward was defective, then **withdrew** it when the prompt turned out
to be malformed. The withdrawal was right. The confound is now **removed rather than argued away** —
correct BOS, correct role markers, correct non-thinking marker, ASCII text ids that decode back
exactly — and the output did not improve.

**The 43-layer forward is defective.** 124 gates certify a pipeline that does not compute the model.

## One residual, named rather than hidden

Our renderer demonstrably mishandles 3-byte UTF-8, and encode and decode **share that mapping**. A
round trip cannot validate a codec whose two halves hold the same wrong assumption — the errors
compensate and the test passes. This prompt is pure ASCII, where the path is sound, so the residual
does not reach this result. It would reach any non-ASCII prompt, and it means every round-trip
reassurance I have given about this tokenizer covered less than I thought.

## The most surprising teaching

**I found the answer by reading a file that has been on this disk since 2026-07-21.** Not the
internet in the end — `MODEL_CARD.md`, sitting in the same directory as the `ds4` binary I also
never ran. Twice today the thing that unblocked me was already local and unopened: the engine, and
its documentation. "We can find on the internet" turned out to understate it.

## Where discomfort turned to gold

Asserting, withdrawing, then re-asserting the same claim in one day. The withdrawal felt like
failure and was the most valuable move in the sequence — it forced the control that makes the
re-assertion worth something. A claim that has survived the elimination of its most plausible
alternative is a different object from the same sentence asserted first time.

## The frontier question

> **What names two errors that cancel, so a test of both together passes?**

**`compensating error`** — the accounting and metrology term. It is exactly what our
encode/decode round trip does with the 3-byte mapping, and why round-tripping proved less than I
claimed. Verified 0 hits. *Landing deferred to the next reunion-free moment* — the corpus has taken
five collisions today and this receipt is already the record.

## Ground stamp

```
prompt 0 · 128803 · 3085 344 270 6102 294 8760 33 9361 418 834 2004 16 · 128804 · 128822
run    26 steps, 171 s, VERDICT PASS 124 gates
out    [29326 8317 21774 418 270 29326 28328 34887 121141 114070 295]
       = " Protocol syn Political with the Protocol Politics के オリ Participate in"
./fkwu --src form/form-stdlib/tests/ds4-paris-probe-band.fk  -> 127
```

## What this changes

Every stone on this lane — router, layer, entrance, exit, pipeline map — sits above a forward that
does not compute the model. The next work is not the five KAT-Coder kernels. It is localizing the
defect in 43 layers, and the probe above is the gate that says when it is fixed.
