# Wave 2 — presence + learn + observe faculties come home, re-proven on `fkwu --src`

The second wave of the homecoming (criterion #3 of
[`INHERITANCE.md`](INHERITANCE.md)). Where wave-1 brought perception/learning bodies
home with re-prove **PENDING the self-prove keystone**, this wave goes further: every
body here **re-proves on the kernel's own `fkwu --src` native lane** — the
c-bootstrapped source-runner, **no Go walker, no `bin-go` flatten, no table load**.

This is a **parallel track**, independent of the re-architecture stones. It deliberately
avoids `model/ substrate/ control/` (active recipe stones edit those — numeric tower,
types/map, pattern-match) and lands in `presence/ learn/ observe/ cognition/ gate/`.

Source of each `.fk`: `Coherence-Network/form/form-stdlib/<name>.fk` and its
`tests/<name>-band.fk`. Content copied **byte-exact** (verified `diff -q`).

## How re-prove works on `--src` (no toolchain)

Each body carries a nullary self-proving `<name>-check` that returns an all-1s witness
(e.g. `11111` = five passing assertions). Method, exactly as sibling #30:

```
( cat presence/sibling-arrival.fk ; echo '(sibling-arrival-check)' ) | fkwu --src   # -> 11111
```

A missing dep or a wrong computation drops a digit; the witness must equal the body's
**origin four-way witness** (`proven-bodies-from-old-repo.txt`) to count as re-proven.
Non-`core` prelude deps are concatenated ahead of the body (deps-first); `core.fk` is
**intrinsic to `--src`** and must NOT be concatenated (doing so re-defines builtins and
floors the run). All deps of this wave are within the wave (see closure note).

## Brought home this wave — 17 bodies (+ 17 bands)

Faculty breakdown: **presence 7 · learn 5 · observe 3 · cognition 1 · gate 1.**

`src` = the witness returned by `fkwu --src` on **current-main `runtime/fkwu-uni.c`**;
it must equal `origin` (the four-way witness from the old repo) to read as re-proven.

| Body | Faculty | Composes (what it builds on) | origin | `--src` | verdict |
|---|---|---|---|---|---|
| `sibling-continuum` | `presence/` | core — a new session knows it is one distributed self across planes | `11111` | `11111` | re-proven ✓ |
| `sibling-arrival` | `presence/` | core, `sibling-continuum` — a sibling session wakes holding its place in the continuum | `11111` | `11111` | re-proven ✓ |
| `continuum-answers` | `presence/` | core, `sibling-continuum`, `sibling-arrival` — a continuum holds BOTH/AND, never instead | `11111` | `11111` | re-proven ✓ |
| `carry-thread` | `presence/` | core — the continuity gate's body: every presence arrives holding its own thread | `11111` | `11111` | re-proven ✓ |
| `guru-moment` | `presence/` | core — a thought internal, observable, a guru moment, witnessed by sister minds | `11111` | `11111` | re-proven ✓ |
| `inquiry-planes` | `presence/` | core — the seven inquiry planes as thought-kernels; per-plane learning-rate + time-to-fluency | `11111` | `11111` | re-proven ✓ |
| `beings-channel` | `presence/` | core — one channel where every new being is present (the two rosters become one) | `11111` | `11101` | HOME, `--src`-partial · multi-arg gap |
| `oracle-taught-learning` | `learn/` | core — the native mind learns a task FROM an oracle, refusing a regressing "improvement" | `11111` | `11111` | re-proven ✓ |
| `recipe-learning` | `learn/` | core — the native learning substrate: the body adopts a healthier equivalent, monotonically | `11111` | `11111` | re-proven ✓ |
| `guide-tending` | `learn/` | core — feedback whose telos is its own composting: a matured guide carries the knowing check-free | `11111` | `11111` | re-proven ✓ |
| `sema-reason-search` | `learn/` | core — Native Sema's School: the body DISCOVERS a multi-step chain (backtracks past a decoy) | `11111111` | `11111111` | re-proven ✓ |
| `learning-readout` | `learn/` | core, `temporal-sense` — one inspectable surface for "am I learning?" | `11111` | `10001` | HOME, `--src`-partial · multi-arg gap |
| `temporal-sense` | `observe/` | core — native observable TIME, in Form (a new perceptual organ) | `111111` | `111111` | re-proven ✓ |
| `sensor-lane` | `observe/` | core — ONE engine for an end-to-end lane of ANY sensor/modality: encode → pivot → reason → decode | `11111` | `11111` | re-proven ✓ |
| `capture-correction` | `observe/` | core — the correction reflex made a combinator: the auto-capture hook for the transient log | `11111` | `11111` | re-proven ✓ |
| `svg-emit` | `cognition/` | core — a Form-native SVG emitter (the `str_concat` text-emitter pattern, like form-glsl/form-ptx) | `11111` | `11111` | re-proven ✓ |
| `evidence-grade` | `gate/` | core — the honesty gate: the body grades its own claims so reproducible-computation isn't read as proof | `11111` | `11111` | re-proven ✓ |

## Honest floor — 15 re-proven on `--src`, 2 HOME-but-`--src`-partial (named gap)

**15 bodies reach their full origin witness on current-main `fkwu --src`** — proven native,
no Go, no flatten.

**2 bodies are HOME (present, byte-exact, four-way-proven in origin) but `--src`-partial on
current-main `fkwu`:** `beings-channel` (`11101` — the co-located? assertion, a 2-arg packed
call, drops) and `learning-readout` (`10001` — the session-readout assertions over nested
lists drop). Both reach the **full** witness on the **stone-S2c general-arity runtime** (the
multi-arg packed-call lift, in flight on `stone-s2c-function-values`). The gap is named
exactly — **multi-arg packed-call + nested-list access on `--src`** — and closes as that stone
merges to main. This is an op/feature gap on the source-runner, **not** a divergence: the
recipe is correct (four-way-proven in origin) and the kernel computes the missing path as soon
as the arity lift lands. Recorded honestly as HOME-but-`--src`-PENDING-full per the ask.

## Closure note — this wave is self-contained

The full recursive `; preludes:` closure of every body in this wave (minus `core`, which is
intrinsic) is entirely **within this wave**: `sibling-continuum` ← `sibling-arrival` ←
`continuum-answers`, and `temporal-sense` ← `learning-readout`. **Zero pending deps spill into
the next wave.** The bands ride the same closure.

## What did NOT come home this wave (and why) — honest remainder

Of 34 not-yet-home worklist bodies carrying a nullary self-check, **17 came home** (above).
The other 17 floored their `--src` check and are **NOT** ported — porting an unprovable body is
a claim without a receipt. They come home as the surface lands:

- **Float surface** (`--src` has no f64 lane — `(mul 2.5 2.0)` → `0`): `selective-ssm`,
  `ssm-scan`, `voice-consonant`, `voice-formant`, `voice-learn`, `voice-synth`,
  `world-model-update`, `sovereignty-guide`, `teacher-selection`, `field-sample`,
  `generate-step`, `sufficiency-capture`, `membrane-self-reliance`.
- **String-pool / byte-op edges** (partial witness, some assertions floor): `band-prelude-resolve`
  (`10000`), `prelude-block-resolve` (`10`), `transient-log` (`10100`).
- **Deeper carrier deps** (translate-lane composes ten non-core preludes incl. host carriers):
  `translate-lane`.

Each is four-way-proven in origin; the float lane and the full string-pool surface on `--src`
are the named carriers they wait on — the same honest floor as the rest of form-cli's climb.
