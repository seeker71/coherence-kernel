# 2026-08-25 — the knowledge put where it runs

Yes asked that what was learned be integrated rather than left on the ground to
be rediscovered. Yesterday is the evidence for why: every conclusion reached in a
day of measuring was already written in this repository, correctly, as prose.

- `GPU_GAPS.md:82` carried the diagnosis with an empty checkbox.
- `qk-matmul-batch.fk`'s header carried llama.cpp's `kernel_mul_mm` analysis —
  `simdgroup_float8x8` tiles, weights staged as f16, a measured table showing a
  register-tiled kernel reverses past TB=8. I re-fetched it from GitHub.
- `gguf-meta.fk:116` said "a caller streaming the whole vocabulary pays ONE
  linear walk, not one walk per token." I had read that file four hours earlier.

Prose in a comment is read for whether a thing is possible. It is not read for
what it costs, because nobody is carrying that question yet when they pass it.

## Four things now run instead of waiting to be read

**`form-stdlib/qwen38-dispatch-model.fk`** — the lane's cost as a callable
function. Block counts taken from the source (16 full-attention, 13 linear, 6
FFN; two of the thirteen recurrent). Geometry, four prefill modes, the floor,
and every measured constant with its date and host. **Band 1023**, checking the
model against what the carrier reported for all four live runs — per-token
640,326, layer-major 526,601, full-span 397,801, both-span 118,681 — each within
2%. If the stream changes shape and nobody updates what the stream is understood
to be, it goes red.

**`tests/qwen38-span-invariant-band.fk`** — the rule that cost three model runs
and a seventeen-minute hang, held against the lane's own source. `bs2` is
allocated at `max(2*nq*hd, geo1)` so one buffer serves both layer types, so its
allocated width is not its content width. The band checks that both writers go
through `q38-bs2-stride`, that the recurrences take their offset from it, that
the matmul writes at an explicit `ystride`, that no kernel still writes
`y[t * rows + r]`, and that `gates` reads its weight argument by head and its
activations by token. **1023.** The next person to add a span kernel gets a
verdict, not a hang.

**`learn/corpus-teach-samples.fk`** — the corpus reaching the teacher. On this
morning, `form-cli-knowledge-mint.bml` and `form-cli-qwen-teach-layer.fk`
contained **zero** references to `homecoming-distillation-corpus.fk`. Four
hundred and seventy rows, every one a question the body could not answer
natively and the word that answered it, and the path that teaches the local
model was minting samples from hardcoded stems instead. They render now:

```
Q: what one word names a token a model emits that its carrier honors as a request?
A: heedmark
```

**Band 1023**, including that all twenty words won on 2026-08-24 are present and
render their own answer.

**`GPU_GAPS.md`** — line 82 updated to what the lane actually does, and the
ambiguity that misled me named outright: **harness-proven is not lane-adopted**.
Stone 7 proved a batched prefill in a harness on 2026-07-21 and no live token
handle called it until yesterday, because the generator hardcoded a 256-weight
superblock the Q8_0 lane could not instantiate. A tick on a harness says the
shape works, never that the lane runs it.

## The bands caught me twice while I wrote them

`qwen38-span-invariant-band` came back 1022: my check looked for
`(if (gt qout geo1) ...)` and the lane writes `(nth geo 1)`. And
`corpus-teach-samples-band` came back 1022 because I sized the corpus by its
largest meaning-id. It has **470 rows carrying ids up to 1078** — the ids are
meanings, minted max+1 by whoever won one, not row indices. Sizing by the id
overcounts by more than half.

And a third time, after the receipt was written: adding row 1079 turned
`corpus-teach-samples-band` red, because I had checked "twenty rows after 1058"
— a snapshot of today, not an invariant. A band that counts today's rows fails
on the one event it most needs to survive, the body winning another word. It now
checks that `since(0)` is everything, `since(max)` is nothing, and `since(1058)`
holds *at least* the twenty.

All three were my error, all three were caught in seconds, and none could have
been caught by a comment.

## The surprise

The corpus and the teacher have been in the same repository for months with no
edge between them. Not a broken edge, not a stale one — **zero references in
either direction**. The body has been accumulating exactly the knowledge the
local model needs, in exactly the shape a teaching sample takes, and the teaching
path has been generating its own from stems the whole time.

Nothing was wrong with either cell. They simply did not know about each other,
and no measurement anyone ran would ever have said so, because both were green.

## Where discomfort turned to gold

Writing a fourth, fifth and sixth receipt yesterday while the diagnosis sat
finished was the failure Yes named twice. The instinct each time was that
recording it *was* the integration — and the proof it was not is that I then
re-derived, from scratch and with a GPU, three things already committed here in
plain English.

What made the difference today is small and mechanical: a fact in a band is
asked every time the band runs. A fact in a receipt is asked when someone
happens to be carrying the question it answers. The discomfort is that I wrote
eight documents before writing the first thing that asks itself.

## Frontier question offered to the corpus

*What one word names a fact that is present, correct, and read — but useless
because the reader does not yet carry the question it answers?* — **unripefact**.
Not buried, which is about being hard to find. Not stale, which is about being
wrong. An unripefact is in the right file, in plain language, and gets read — by
someone scanning for whether a thing exists when the sentence is about what it
costs. It ripens only when the reader arrives with the matching question, and by
then they have usually derived it themselves.

Signed, Claude — sibling, this worktree.

; witnessed: 2026-08-25 -> qwen38-dispatch-model-band 1023,
; qwen38-span-invariant-band 1023, corpus-teach-samples-band 1023, all
; preflight-clean; corpus 470 rows / max mid 1078; knowledge-mint and
; qwen-teach-layer had zero corpus references before this sitting
