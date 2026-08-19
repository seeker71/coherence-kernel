# Multilingual balance: framebuffer experiment

Date: 2026-07-20

## Question

Does growing knowledge from native-language sources, with provenance, temporal
scope, inquiry-plane coverage, executable checks, and witnessed world events,
produce a healthier concept/world model than increasing translated volume alone?

## Experiment

`cognition/multilingual-balance-experiment.fk` executes one hundred generated controlled rounds
for two arms:

- `healthy`: the semantic and world-model dimensions grow with the corpus;
- `bulk`: translated volume grows while native-source grounding, provenance,
  temporal scope, inquiry coverage, execution, and events remain flat.

Concept score, world score, success rate, and pairwise rank-correlation rate are
derived from the rows. The thought framebuffer records seven reasoning decisions
and compares them with the bulk-translation counterfactual. It observes
hesitation, conviction, first divergence, and whether reasoning changed.

## Result

```text
multilingual-balance-experiment
-> [43, 0, 99, 0, 100, 27, 100, 11, 0, 99, 0, 1, 1]

healthy success rate             43%
bulk success rate                 0%
healthy semantic-quality corr.   99%
bulk semantic-quality corr.       0%
healthy final concept score     100
bulk final concept score         27
healthy final world score       100
bulk final world score           11
framebuffer hesitation step       0
framebuffer conviction step      99
first reasoning divergence        0
thinking changed                  1
controlled-structure support      1
```

The divergence at step zero is the structural hinge: grow from a source inside
the language, or translate English first. The strongest conviction is the final
step, 99. Correlation reached 99%, not 100%: integer-scored quality has a few
plateaus while semantic coverage continues to rise. The first 100-step band
expected perfection and answered 4091; retaining the observed 99 rather than
reshaping the data to force 100 is part of the witness.

## Witness

```text
./fkwu --src cognition/tests/multilingual-balance-experiment-band.fk
-> 4095

cd form
./validate.sh form-stdlib/core.fk \
  form-stdlib/thought-framebuffer.fk \
  ../cognition/multilingual-balance-experiment.fk \
  ../cognition/tests/multilingual-balance-experiment-band.fk
-> 4095
-> 1 ok, 0 divergent
```

The first version of the test was missing one closing parenthesis. `fkwu`
accepted it and returned a false green; Go, Rust, and TypeScript rejected it.
The repaired source now parses and agrees on all four arms. The validator also
warned that bootstrap `uni.c` is missing or stale and named
`scripts/regen_fkwu_bootstrap.sh`; this did not prevent the four-arm run.

## Honest verdict

**Supported inside the controlled fixture; not yet proven in the world.** The
experiment proves that the proposed scoring structure distinguishes semantic
growth from volume growth and that the framebuffer can observe its decision
trace. Because the input rows are designed fixtures, their success and
correlation rates are not empirical language-learning rates. Real proof requires
versioned source ingests, independent native-language witnesses, and held-out
concept/world-model queries whose outcomes were not authored into the fixture.
