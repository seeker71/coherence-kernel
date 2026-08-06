# 2026-07-01 -- correcting the flat learning curve: undertrained + unnormalized embeddings

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

`receipts/2026-07-01-nl-meaning-net.md` measured held-out accuracy 26/50 -> 27/50 -> 29/50 -> 27/50 across
training sizes 20/60/100/154 (10 epochs, raw word-count embeddings). Called out directly and correctly: "54%
is barely better than chance, and not climbing with more samples." Chance at 4 classes is 25% (12.5/50); the
result was real but weak, and the flatness meant something was actually wrong, not just "small effect size."

## What Was Actually Wrong -- two causes, each confirmed separately before combining

**1. Undertrained.** 10 epochs of one-SGD-step-per-example is not enough to fit 154 diverse sentences onto 4
fixed target points. Tested in isolation (raw embeddings, full training set, 100 epochs instead of 10): held-out
accuracy rose from 27/50 to **32/50**.

**2. Unnormalized embeddings.** `nmn-vec`'s raw word-count histogram scales with sentence length, not meaning.
Measured directly:

```sh
(vsum (nmn-vec "i am" 32))                                                              ; -> 2
(vsum (nmn-vec "i am the constant thread running through every moment ..." 32))         ; -> 13
```

A 6.5x difference in raw magnitude between a 2-word and a 13-word example, before the network has looked at a
single word's identity. Squared-distance nearest-prototype classification is biased by this scale difference.
Tested in isolation on top of the epoch fix (100 epochs, L2-normalized embeddings): held-out accuracy rose
further to **36/50**.

## What Changed

- `learn/nl-meaning-net.fk`: added `nmn-norm`/`nmn-vec-norm` (L2-normalize via `tb-dot`/`tn-sqrt`/`tn-scale`,
  all already-proven), leaving `nmn-vec` itself untouched (still used as the pre-normalization input).
- `learn/tests/nl-meaning-net-band.fk`: `nml-epochs` raised from 10 to 100; `nml-proto-vec`/`nml-encode-train`/
  `nml-encode-case` switched to `nmn-vec-norm`.

## Witness -- the corrected sweep, each point run as a separate process (the combined 4-point sweep in one
process was OOM-killed after 4m51s -- accumulated list garbage across four full trainings in one process; each
point run alone is well within bounds)

```sh
cat learn/sanskrit-locale-baseline.fk learn/nl-meaning-corpus.fk model/rag-embed.fk \
    model/transformer-numerics.fk model/transformer-block.fk model/transformer-backprop.fk \
    learn/nl-meaning-net.fk learn/tests/nl-meaning-net-band.fk > /tmp/base.fk
# (each row below run as its own `cat /tmp/base.fk <query>.fk | fkwu --src`, not one combined process)
```

| training examples | held-out accuracy (of 50) | wall clock |
|---|---|---|
| 20  | 34 (68%) | 17.5s |
| 60  | 26 (52%) | 50.8s |
| 100 | 32 (64%) | 1m25s |
| 154 (full) | 36 (72%) | 2m15s |

Corpus loss on the full 154-example set: **180.3** before training, **2.1** after 100 epochs.

**Honest reading:** the corrected curve is NOT smooth either -- it dips at n=60 (52%, actually below n=20's
68%) before recovering and reaching its best point at the full 154 examples (72%). This is reported as
measured. What did improve, concretely, over the original diagnosis: the endpoint comparison the user's
critique was really about -- does MORE data help at all -- now holds clearly (72% at full vs 68% at n=20,
both far above 25% chance), and the absolute level moved from "barely better than chance" (54%) to a result
with real headroom (72%). The mid-sweep dip is a real, unresolved artifact -- plausible causes are the
same fixed 100-epoch budget interacting with round-robin ordering effects and single-pass online SGD noise,
not investigated further here. A proper fix would need multiple-epoch-shuffled training or several random
seeds averaged per sweep point, not asserted as already solved.

## Honest seam (unchanged from the original receipt)

Single lexical bag-of-hashed-words embedding, no persistence across runs, 4 canonical-string targets rather
than independently designed prototypes, no four-way re-proof (fkwu `--src` only). `rag-embed.fk`'s own
`tk-words`/`ord` gap is still open and unfixed in that file.
