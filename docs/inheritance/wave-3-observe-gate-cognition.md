# Wave 3 — observe + gate + cognition faculties come home, re-proven on `fkwu --src`

The third wave of the homecoming (criterion #3 of [`INHERITANCE.md`](INHERITANCE.md)).
Like wave-2, every body here re-proves on the kernel's own **`fkwu --src` native lane** —
the c-bootstrapped source-runner, **no Go walker, no `bin-go` flatten, no table load**.

This is a **parallel track**, independent of the re-architecture / cursor-seed stones. It
deliberately avoids the faculties active stones edit (`model/ substrate/ control/ flatten/
runtime/ grammars/`) and lands in `observe/ gate/ learn/ cognition/`.

Source of each `.fk`: `Coherence-Network/form/form-stdlib/<name>.fk` and its
`tests/<name>-band.fk`. Content copied **byte-exact** (verified `diff -q`).

## How re-prove works on `--src` (no toolchain)

`fkwu --src <file>` reads a single source file, parses it, walks `fk_fn[0]`, and prints the
root value. The method, byte for byte:

```
( cat <deps-first, NON-core> <body>.fk ; grep -v '^;' tests/<body>-band.fk ) > /tmp/b.fk
fkwu --src /tmp/b.fk    # -> the band's witness integer
```

The band file is the four-way prover — its top-level expression evaluates to an all-1s (or
bit-packed) witness. Non-`core` deps are concatenated ahead of the body, deps-first, in the
order of the band's `; preludes:` line; `core.fk` is **intrinsic to `--src`** and must NOT be
concatenated. A re-proven body's `--src` witness equals its **origin four-way witness**
(`proven-bodies-from-old-repo.txt`).

### The `do/let`-block `--src` floor — named exactly

`--src` evaluates bare calls and nested `(add (if …) (if …))` correctly, but a band written as
`(do (let cN …) … (add …))` **returns 0** — the `let`/`do` sequencing block degenerates on the
source lane (broader than wave-2's "let-bound cons-list" note: *any* `let`-block band floors).
Bands written in the wave-2 nullary-defn style — each assertion a `(defn b-N () …)` combined by
nested `add`, **no `let`** — re-prove cleanly. This floor is being lifted in the cursor-seed
pivot; here we port within the envelope (let-free bands) and mark the rest honestly.

## Brought home this wave — 11 bodies (+ 11 bands)

Faculty breakdown: **observe 4 · gate 3 · learn 2 · cognition 2.**

`src` = the witness `fkwu --src` returns on **current-main `runtime/fkwu-uni.c`**, run from
the worktree's own ported copies; it must equal `origin` (the four-way witness from the old
repo) to read as re-proven.

| Body | Faculty | Composes (what it builds on) | origin | `--src` | verdict |
|---|---|---|---|---|---|
| `recognition` | `observe/` | core, `nearest-shape` — recognize WHERE / ROOM / WHO / WHAT by fingerprint | `63` | `63` | re-proven ✓ |
| `sense-liveness` | `observe/` | core — is each sense actually ALIVE, or just shipped? the four states | `127` | `127` | re-proven ✓ |
| `world-perception` | `observe/` | core — the node's surface as a projection over its sensed channels | `255` | `255` | re-proven ✓ |
| `nearest-shape` | `observe/` | core — the body's OWN classifier: content-addressing as recognition | `127` | `0` | HOME, dep-proven · standalone band `--src`-pending (let floor) |
| `field-door` | `gate/` | core — the body knows its deterministic lane from its field lane, holds the pause open | `11111` | `11111` | re-proven ✓ |
| `membrane-self-reliance` | `gate/` | core, `tool-channel`, `choice-receipt`, `form-cli-membrane` — self-reliance read from real crossings | `11111` | `11111` | re-proven ✓ |
| `sufficiency-capture` | `gate/` | core, `form-cli-router`, `form-cli-judge`, `form-cli-sufficiency` — auto-capture wired into the real sufficiency gate | `11111` | `11111` | re-proven ✓ |
| `tool-channel` | `learn/` | core — native tool/channel catalog and planner | `255` | `0` | HOME, dep-proven · standalone band `--src`-pending (let floor) |
| `choice-receipt` | `learn/` | core — shared branch/trust receipt compression | `4294967295` | `0` | HOME, dep-proven · standalone band `--src`-pending (let floor) |
| `generate-step` | `cognition/` | core, `trig`, `transformer-numerics`, `field-sample` — a hidden state becomes a CHOSEN token | `11111` | `11111` | re-proven ✓ |
| `ll-buffer` | `cognition/` | core, `form-asm` — the parameterized stack-buffer memory model | `1` | `1` | re-proven ✓ |

## Honest floor — 8 re-proven on `--src`, 3 HOME-as-deps (named gap)

**8 bodies reach their full origin witness on current-main `fkwu --src`** — proven native, no
toolchain. The 3 marked HOME-as-deps (`nearest-shape`, `tool-channel`, `choice-receipt`) come
home **byte-exact** and **re-prove transitively**: each is exercised, correct, by a let-free
consumer band that itself hits full witness — `recognition` (→`63`) drives `nearest-shape`;
`membrane-self-reliance` (→`11111`) drives `tool-channel` and `choice-receipt`. Their *own*
standalone bands are written in the `do/let`-block style and floor to `0` on the source lane.
That is the named `--src` gap, not a wrong answer: the bodies compute correctly (the consumer
witnesses prove it); only the standalone `let`-block band degenerates, and that floor closes in
the cursor-seed pivot.

This is the precise honesty bar: a body returning the floor/`0` is **never** claimed
`--src`-proven. `nearest-shape`/`tool-channel`/`choice-receipt` are claimed exactly as
**home + transitively-proven + standalone-band-`--src`-pending**.

## New `--src` gap observed this wave

The `do/let`-block floor is **broader than wave-2 recorded**: wave-2 named "let-bound bare
cons-lists degenerate"; wave-3 confirms *any* `(do (let cN …) … )` band returns `0` on `--src`,
even when every bound value is a scalar `(if …)` and the underlying body is pure-integer
recursion (verified directly: `(do (let c0 (if (eq (gcd 48 36) 12) 1 0)) c0)` → `0`, while
`(gcd 48 36)` → `12` and `(add (if …) (if …))` → correct). This is why the let-free
nullary-defn band shape is the load-bearing one for `--src` re-prove, and why most
worklist bands (≈862 of 1042 use `let`) await the cursor-seed lift before they re-prove
standalone.
