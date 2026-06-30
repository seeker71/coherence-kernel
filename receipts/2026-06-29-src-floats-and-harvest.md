# Receipt — floats on fkwu --src: the model/SSM/voice lane crosses to observed-on-metal (2026-06-29)

**The walk (Urs):** walk all the not-yet-observed recipes. First and biggest surface: **floats on `--src`.**

## The surface (one focused change)

`runtime/fkwu-uni.c` `fk_sparse` number leaf: a `.` or a valid `e`/`E` exponent makes the literal a **float** —
intern the whole text (incl `1.5e-05`) and wrap it in `str_to_float` (tag 53 = `strtod`), the exact float value
the flattener's `flt-float-lit` produces. Integers stay tag 1. The float arithmetic ops were already float-aware
(`fk_num`), and `eq` compares by `fk_num` — so float compute + comparison work end to end:

```
(add 0.5 0.25)        -> 0.75
(mul 1.5e-2 100.0)    -> 1.5
(eq (add 0.5 0.25) 0.75) -> 1      (float equality, fresh-box-safe via fk_num)
(add 40 2) -> 42 ; native-vs-rented -> 11111   (integers + regression intact)
```

## The lift — 0 → 26 of 32 float recipes now observe on fkwu --src

A re-sweep of the 32 float-flagged checked recipes: **26 now return their all-1s witness** (`->11111`) on the
c-bootstrapped kernel; before this surface, every one returned empty/`nothing`. The model/SSM/voice/learning lane
crossed from "four-way-proven, not observed" to "observed running on metal."

## Ported this commit — 16 missing float-observers (each -> 11111 in-place)

```
observe/: carry-thread field-sample temporal-sense world-model-update selective-ssm ssm-scan sovereignty-guide
learn/:   recipe-learning teacher-selection teacher-selection-school teach-sema-units sema-reason-search
          voice-consonant voice-formant voice-learn voice-synth
```

Notably the **voice lane** (`voice-consonant/formant/learn/synth`) and the **active-inference / state-space lane**
(`selective-ssm`, `ssm-scan`, `world-model-update`, `temporal-sense`) observe natively now — earlier filed under
the deferred climb, but their float numerics run on `fkwu --src` today.

## Still blocked (6 of 32) — the next walk: dependency closures

`attention`, `softmax`, and ~4 others return `nothing` not for floats but because they call **multi-file
dependencies** (`attention` → `embedding-distance`, …) that aren't in the new repo yet — the same form-cli-shell
pattern. They observe once their closure is ported. That is the next step of the walk, alongside the depth-wall
(`form-asm` lowering) and host-io rows.

## Tally update (checked-recipe metric)

Was 35 observed / 62 blocked. Floats move ~26 across; this commit ports 16 of them. The frontier shrank from
"floats + deps + depth + io" to "deps + depth + io" — floats are done.
