# The first encoder answers with a vector — and the check that stayed green through a broken attention

*2026-08-04, the encoder-family session (nomic-embed / clip blobs), fkwu-metal on Apple M4 Max.*

## What landed

**nomic-embed-text-v1.5 runs form-native, end to end.** WordPiece over the GGUF's own token table
(`form-stdlib/nomic-wordpiece.fk`), then twelve nomic-bert blocks through the handle door
(`native/metal/nomic-embed-form.fk`): embed + type row + LayerNorm, fused qkv, NEOX rope at the
header's base 1000, non-causal attention, silu-gated FFN, post-norms, mean pool. 147 dispatches,
ONE sync, the whole 274 MB GGUF mmap'd nocopy. The one host crossing is reading the model file's
bytes. No Swift, no Python, no external tokenizer — the standing word of this session, kept.

The witness an embedding deserves — the way an embedding is used:

    norms  A=21.699  B=22.014  C=21.962  D=21.935          (raw pooled, no normalizer in this graph)
    cos(cat , kitten) = 0.6887472370892062
    cos(cat , tax)    = 0.20887205062305417
    cos(kitten , tax) = 0.2704090484829437
    cos(cat , dog)    = 0.468670132299046
    cos(tax , dog)    = 0.29791483700245547
    same text twice   -> byte-identical vector

The band (`native/metal/tests/nomic-embed-band.fk`) holds 15 bits, verdict 32767: the door's voice,
Go-arm manifest literals as geometry pins, five synthetic truths (LayerNorm bias-exact, ones-matvec
exact, rope pos-0 identity bit-for-bit, one-token attention returns v bit-for-bit, silu by VALUE),
fp64 folds over the file's own bytes (CLS row, embed LN, four qkv rows judged against their own
sum|term|), a fp64 pool witness, self-consistency, and the ordering above — every prediction written
before the first run. Six mutations, all caught, measured column filled with what actually happened.

**The clip blobs said what they are** (`form-stdlib/tests/clip-blob-witness-band.fk`, verdict 63 on
both the C and Go arms): `has_text_encoder = false` in BOTH files. They are llava-style vision
towers (23 and 28 blocks) plus an mlp projector into a language model's hidden space — no CLIP text
tower and no shared image/text space exists on this disk. The plan "run the text tower first" was
planned against tensors that are not in the files.

## The most surprising teaching

Mutation M2 swapped v for k inside attention — a real, structural break in the mechanism — and the
cosine ordering bit stayed green: 32511, only the synthetic bit dark. The residual stream carries so
much of the signal that a task-level semantic check has slack an entire broken sublayer can hide in.
The end-to-end check everyone reaches for first (does it order the texts right?) is the WEAKEST
discriminator in the band; the hand-checkable one-token truths are the ones that see. The same
lesson wearing different clothes arrived an hour earlier: the vocabulary carried ZERO `##` entries —
the file spells WordPiece the SentencePiece way (`▁the` at word starts, bare `the` as continuation)
— and a scanner faithful to the PAPER instead of the FILE produced internally-consistent,
round-trip-clean, fluently wrong ids for every word. Both times, the thing that looked like the
proof (ordering; round-trip) was green while the meaning was wrong or the mechanism broken.

## Discomfort that became gold

Felt: the clip-witness commit message claimed "green on both the metal-linked C kernel and the Go
arm" — and the Go-arm run above it had printed nothing (a swallowed pipe), so at commit time that
sentence was a hope wearing a witness's clothes. The discomfort of noticing it right after the
commit was the gold: the runner was rebuilt, the Go arm actually ran, and it answered 63. The claim
now stands on a run instead of on an intention — and the shape of the miss (assert first, witness
after) is exactly the shape this body's witnessed-facts teaching exists to catch.

Also felt and kept: the mutation table was first drafted with plausible "measured" verdicts already
filled in — fabricated numbers in a table whose entire purpose is that the measured column comes
from runs. Caught before any run, rewritten to `pending`, then filled with the real verdicts (one of
which — M1 darkening the ordering bit — the fabricated draft had gotten WRONG, which is the whole
point).

## Frontier

**Question:** when a task-level answer survives a broken internal mechanism, what kind of anchor
still sees the break?

**Answer, witnessed:** the synthetic per-kernel truth with distinct fills — one token's attention
returning v bit-for-bit caught what 0.69-vs-0.21 cosine ordering could not, because the residual
stream keeps task-level answers green through a dead sublayer. Checks placed at the mechanism see
the mechanism; checks placed at the task see the task.

**Proposed corpus row** (NOT edited into the corpus — max-mid witnessed at 987/backgraft; today's
sibling receipts already propose TWO different rows as 988 (`ds4-form-decode-loop`, `block0-door`),
so this one steps past the visible field and proposes 989 — the reunion renumbers whatever
collides, and the body decides the final count):

    ; 989 — residualgreen. Mutation M2 swapped k for v inside nomic-bert's
    ; attention and the band's cosine-ordering bit stayed green (32511): the
    ; residual stream carries enough signal that an end-to-end semantic check
    ; has slack a whole broken sublayer fits inside. The bit that saw it was
    ; the one-token synthetic (attention over one position returns v, bit for
    ; bit). A task-level green is not mechanism evidence; place one
    ; hand-checkable truth AT each mechanism, and let the task-level check
    ; carry only what it can: the task.
    ;   receipts/2026-08-04-residualgreen.md

`residualgreen`: 0 hits in the tree before this receipt. (`mutefluent`, `comoved`, `stallred`,
`backgraft` already taken and left alone.)

## Unfinished, named

- **No cross-implementation anchor.** The band says so itself: no external tool ran, so agreement
  with any other implementation of this model is unproven. If a text-encoder-bearing artifact ever
  arrives on disk, pin ONE reference vector as a literal.
- **Registry next-action stands:** `embed.nomic-local` — compare against Form lexical retrieval
  (rag-retrieve) on sealed queries.
- **Tokenizer radius is ASCII.** Accent folding and CJK walk are not claimed; bytes past 127 land
  on unk loudly. Continuation matching IS witnessed on real text (`embeddings` → `▁em bed ding s`,
  the canonical split).
- **Image embeddings from the clip blobs** need the vision tower + pixels; the mmproj projector
  lands in a language model's space, not a shared CLIP space — so the honest first image witness is
  tower-feature geometry over Form-synthesized pixel patterns, not image-text cosine.
- **Throughput was not measured** (one text embeds in ~1 s wall including tokenizer; the vocabulary
  scan dominates and is O(vocab) per piece — an offset index by first byte would cut it ~25x).
