# 2026-07-01 -- satsang-oracle.fk's first live (non-synthetic) voice

## Ground

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk
./fkwu --src bootstrap/ground-recursive.fk 10
```

Witness:

```text
42
55
```

## Source Observation

`receipts/2026-07-01-satsang-oracle.md` proved the folding algorithm against three committed, hand-authored
EXAMPLE proposals (Model A/B/C) -- explicitly marked as not live model output. Asked directly what the next
honest step toward a real council looks like, given no external API credentials are available this session
(confirmed: no credential search was run, and none were offered) and live multi-model wiring over HTTP is a
separate, larger, credential-gated task: the middle step is a genuinely LIVE voice with no network call at
all -- a real parse produced fresh, in this session, by an actual reasoning model (this Claude session,
2026-07-01), not scripted in advance the way Model A/B/C were.

## What Changed

`learn/tests/satsang-oracle-live-band.fk` -- two parses of a NEW sentence ("Truth alone triumphs over every
lie."), both produced live in this turn, not copied from any earlier file. The two parses differ on a genuine,
real linguistic question rather than a manufactured disagreement: English phrasal verbs are real -- is
"triumphs over" one lexical unit, or two separate word-leaves? Both segmentations are defensible; this band
feeds both to `sao-witness-council` and reports whatever it actually decides.

## Witness -- and an honest limitation found by actually running it, not assumed

```sh
cat form/form-stdlib/channel-interface.fk form/form-stdlib/satsang.fk learn/satsang-oracle.fk \
    learn/tests/satsang-oracle-live-band.fk > /tmp/satsang-oracle-live-band.fk
./fkwu --src /tmp/satsang-oracle-live-band.fk
```

```text
7
```

All 3 conditions hold:

- The root does **not** survive: `truth alone triumphs over every lie .` (7 leaves) and
  `truth alone triumphs-over every lie .` (6 leaves) differ in arity at the very top, so
  `sao-shape-eq?` (axiom-3 ctor-tag+arity match) calls them different shapes before anything about
  their shared content is examined.
- The candidate's tag (`"seq"`) is still named in the open-question record -- not dropped, matching
  `satsang-oracle.fk`'s own discipline that an unresolved node stays visible, not silently absent.
- The witness record is exactly `(1 affirm, 1 dissent)` -- a genuine tie, and `sat-survives?` requires
  *strictly* more affirm than dissent, so the tie correctly does NOT crystallize into false confidence.

**The honest finding:** two parses that agree on 5 of 6-or-7 words (`truth`, `alone`, `every`, `lie`, `.`) and
differ on exactly one real segmentation choice show up as a FULL non-survival at the root, with zero credit
for the substantial word-level agreement underneath -- because the fold only recurses into children when the
parent's shape (tag+arity) already matches. This is a real, named limitation of the current algorithm, not a
bug: `sao-fold-tree`'s own design deliberately does not walk children of a non-surviving node, since "there is
no agreed structure to align them against" (the comment already in `satsang-oracle.fk`). Closing this would
need an alignment step (e.g. edit-distance-style matching) before the node-by-node fold, not asserted as
already handled here.

## Honest seam

Still one voice, not a council -- N=2 with no true third party, and both proposals came from the same
session/model producing two candidate parses, not two independently-reasoning parties. No network call was
made; wiring an actual second model over fkwu's native HTTP floor remains separate, real, and still gated on
credentials/scope you'd need to provide.
