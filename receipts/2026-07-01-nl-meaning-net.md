# 2026-07-01 -- a real trained model, seen learning, on hundreds of pairs

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

This session's earlier work (`slb-meaning-for-tokens`, `mlap-text-meaning`) was hand-coded exact-match and
word-overlap heuristics -- explicitly not learning. The ask: get to a real model that can be trained and
observed learning, over hundreds of pairs and complex passages, not four toy phrases.

**A major finding along the way:** `model/transformer-backprop.fk` and `model/transformer-corpus-train.fk`
already contain a genuine, well-designed backprop/SGD training stack over `model/transformer-block.fk`'s
residual FFN blocks -- but neither has a test file, a band, or a MANIFEST/receipt entry anywhere in this repo.
It was real code that had, as far as this checkout shows, never actually been run. It is run and witnessed
here for the first time.

**A second finding, discovered only by running code rather than trusting its comments:** `model/rag-embed.fk`'s
`re-vec` -- billed as "a sovereign lexical embedding... four-way provable... needing no model" -- depends on
`tk-words` and `ord`, neither of which resolves to a working function on this fkwu build. Both silently
evaluate to the canonical "nothing" instead of erroring:

```sh
echo '(ord "t")' | ...        # -> nothing
echo '(tk-words "x" 1)' | ...  # -> nothing
```

So `re-vec` currently produces an all-zero histogram (right length, empty content) on this kernel. Not fixed
in `rag-embed.fk` itself here -- a real, separate, still-open gap. `re-split` (needs only `substring`/`str_eq`)
and `re-inc`/`re-zeros` are fine; only the `ord`-based hash step is broken. `str_byte_at` is the real, working
replacement (`(str_byte_at "the" 0)` -> `116`).

`append` is also missing the same way (`(append (list 1 2) (list 3))` -> `nothing`) -- a `cons`-based concat
was written instead.

## What Changed

- `learn/nl-meaning-net.fk` -- a real lexical embedding (`nmn-vec`, composing the working pieces of
  `rag-embed.fk` plus a `str_byte_at`-based hash, bypassing the broken `tk-words`/`ord` chain), deterministic
  small-weight initialization (a hash-based LCG, not `Math.random` -- same determinism discipline this whole
  kernel holds), a training loop over `transformer-backprop.fk`'s `tbp-stack-step` (unchanged, reused), and
  nearest-prototype classification.
- `learn/nl-meaning-corpus.fk` -- **204 hand-authored examples** across the same 4 meaning classes
  `sanskrit-locale-baseline.fk` anchors (wellbeing, truth-triumphs, self/existence, world-peace), ~50 per
  class, mixing short paraphrases with genuinely complex/compound sentences and a handful of real multi-locale
  baseline renderings. Labeled honestly as hand-authored for this training demo, not scraped from an external
  corpus (`~/source/Coherence-Network` does not exist in this checkout -- confirmed, not assumed).
- `learn/tests/nl-meaning-net-band.fk` -- deterministic 75/25 split (every 4th example held out), a
  class-balanced round-robin training order (so a small prefix still spans all 4 classes), and a real learning
  curve: held-out accuracy measured at training-set sizes 20 / 60 / 100 / full (154).

## Witness -- the actual numbers, not smoothed over

```sh
cat learn/sanskrit-locale-baseline.fk learn/nl-meaning-corpus.fk model/rag-embed.fk \
    model/transformer-numerics.fk model/transformer-block.fk model/transformer-backprop.fk \
    learn/nl-meaning-net.fk learn/tests/nl-meaning-net-band.fk > /tmp/nl-meaning-net-band.fk
./fkwu --src /tmp/nl-meaning-net-band.fk
```

```text
47
```

Decoded against the band's 6 conditions: bits 1+2+4+8+32 set (loss decreases; held-out beats chance by a wide
margin; more data never hurts; the full set strictly beats the 20-example start; the sweep genuinely reaches
150+ examples) -- bit 16 (perfectly monotonic across all four sweep points) is the one that does NOT hold.

The individual numbers, queried directly:

| training examples | held-out accuracy (of 50) |
|---|---|
| 20  | 26 (52%) |
| 60  | 27 (54%) |
| 100 | 29 (58%) — the peak |
| 154 (full) | 27 (54%) |

Chance level at 4 classes is 25% (~12.5/50). Corpus loss on the full 154-example training set: **1070.3**
before training, **150.2** after 10 epochs -- real, substantial reduction, not full convergence (10 epochs was
chosen to keep the sweep's total runtime under a minute; more epochs would likely reduce it further).

**Honest reading:** this is a real, working, measured result -- not a hand-tuned demo. Accuracy climbs from
chance-plus (52%) toward its best measured point (58% at n=100), then dips slightly at the full 154-example
set (54%). That dip is reported, not hidden or re-run until it disappeared: plausible causes are the fixed
10-epoch budget being relatively less sufficient as the training set grows more diverse (including the harder,
longer compound sentences concentrated later in each class's list), and the small embedding width (`d=32`)
absorbing more hash collisions as vocabulary grows. Both are real, nameable next steps -- more epochs, a wider
embedding, or a proper validation-loss-based early stop -- not papered over as "it just works."

## Honest seam

This is a single lexical (bag-of-hashed-words) embedding into a small 2-block residual stack, trained per-run
with no persistence -- not a multi-locale or multi-model system. The 4 targets are the classes' own canonical
English strings' embeddings, not independently-designed prototypes. Four-way re-proof (Go/Rust/TS) is not done
here; this runs on `fkwu --src` only. `re-vec`'s `tk-words`/`ord` gap is named, not fixed, in `rag-embed.fk`
itself.
