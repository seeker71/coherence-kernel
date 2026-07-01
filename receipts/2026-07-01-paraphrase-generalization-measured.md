# 2026-07-01 -- Paraphrase generalization, measured not asserted

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

The 10-sample multilocale pipeline band proves plumbing, not learning (it says so itself: "closed-set Form
learning, not open ASR/translation"). The open question this receipt answers with real numbers instead of
another assertion: given the two locate functions this body now has -- `slb-meaning-for-tokens` (exact match)
and the pre-existing `mlap-text-meaning` (word-overlap best-match) -- how far do they actually generalize past
the exact seed phrases they were handed?

## What Changed

Added `learn/tests/paraphrase-generalization-band.fk`: 20 hand-written English paraphrases of the 4 baseline
meanings (5 each), none copied verbatim from `slb-lines()`. Each case is checked two ways and the band returns
`exact_correct*100 + overlap_correct` (both counts 0..20, non-colliding encoding).

## Witness

```sh
cat observe/stt-wer.fk observe/asr-prompt-id.fk learn/sanskrit-locale-baseline.fk \
    learn/multilocale-nl-audio-pipeline.fk learn/tests/paraphrase-generalization-band.fk \
    > /tmp/paraphrase-generalization-band.fk
./fkwu --src /tmp/paraphrase-generalization-band.fk
```

```text
18
```

Decoded: `exact_correct=0/20`, `overlap_correct=18/20`.

- Exact-match generalizing `0/20` is expected -- it is a lookup table, not a classifier; it only ever
  matched the literal seed tokens.
- Overlap-match at `18/20` (90%) sounds better, but the 2 failures are not noise -- both are diagnosed and
  reproduce a real, explainable failure mode, not "not enough data" in the abstract:
  - Case `("happiness" "for" "everyone")`, true meaning `301`, predicted `304`. The incidental word `"for"`
    happens to appear in row `304`'s canonical tokens (`"peace" "for" "every" "world"`), so it outscores the
    true row's zero overlap. A stopword-blind overlap scorer treats any shared filler word as signal.
  - Case `("existence" "is" "present")`, true meaning `303`, predicted `301`. Zero overlap against every row.
    `mlap-best-text-loop` seeds `best` with the first row in list order and only replaces it on a *strictly
    greater* score, so an all-zero tie silently resolves to whichever meaning happens to be listed first --
    there is no abstention path, no "I don't know."

## Answering "how many samples would a model actually need"

Measured here, not estimated: a same-vocabulary overlap scorer needs zero additional training data to hit
90% on these paraphrases, because it isn't learning weights -- it's counting shared tokens against a fixed
4-row table. That number will not hold against real open text; it holds here only because the 4 meanings
share almost no vocabulary with each other in English (the one collision found, `"for"`, already broke a
case). Scaling the same approach to a realistic open vocabulary needs stopword removal and an abstention
threshold at minimum, and the accuracy figure itself stops meaning anything past a handful of hand-picked
classes -- which is exactly why `form-cli-predict.fk`'s real result (939 turns, 84.8% held-out over **8**
labels) is the more honest reference point for what real generalization costs, not this toy count.
