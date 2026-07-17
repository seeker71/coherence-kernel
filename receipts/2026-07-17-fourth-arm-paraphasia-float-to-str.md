# 2026-07-17 — the fourth arm's paraphasia: `float_to_str` unbound, and the walker fluently said "cell"

## Ground

`cc -O2 -o fkwu runtime/fkwu-uni.c`, then `./fkwu --src bootstrap/ground.fk` → **42**,
before and after the heal. The divergence, reproduced at the lowest level: the cached
pre-flattened table `t-json-emitter-379a04212a9bcac1.txt` walked by the cached
`fkwu-1ea5895ac229acac` answered **6** where go/rust/typescript answered **31** — and the
cache key was *fresh* (recomputed `379a04212a9bcac1` from current sources), so this was
never staleness.

## The hunt — each rung witnessed

| step | witness |
|---|---|
| verdict bits decoded | 31 = exact(1)+name(2)+u0001-present(4)+control-parse(8)+blankable-parse(16); fourth's 6 = name+u0001 only |
| probe harness honesty | first probes lied twice: zsh doesn't word-split `$PRES` (one mega-path silently dropped every prelude), and without `GO_BIN` the BML text-lens prep returns empty *silently* — both produced false tables that answered 0 |
| length agrees | `str_len emitted` = **286 on both arms** — same length, different bytes |
| the diff located | first-diff probe → byte **50**; count probe → bytes 50–53 are the *only* diff in 286 |
| the wrong word read | bytes 50–53 on the fourth arm: `99 101 108 108` = **"cell"** where `1.25` belonged — pool string 0, exactly 4 bytes, so the length never flinched |
| the cascade explained | `"score":cell` is invalid JSON → `json_parse_body` stops at score → name (before) round-trips, control and blankable_string (after) vanish — three verdict bits from ONE wrong word |
| the chain exonerated | `(node_type (intern_trivial_float "1.25"))` = 7 ✓; `(eq (node_value …) 1.25)` = true ✓; `(float_to_str 1.25)` on a *direct literal* = "cell" ✗ |
| the mechanism named | `(zz-definitely-unbound 1.25)` also returns "cell": ANY unbound call on the fourth arm lowers numb to pool string 0. `float_to_str` lives in core.fk; the fourth arm swaps core for fourth-shim.fk — which had **no float_to_str mirror** |

## Root cause

`fourth-shim.fk` mirrors core.fk for the flatten lane but never mirrored
`float_to_str` (core.fk:107, built 2026-07-01 when the native int_to_str float
fallback was retired). An unbound call name does not fail the flatten — it lowers
to a call that returns pool string 0, and the shim's first string literal is
`"cell"` (from `(defn cell …)`'s tag list). The walker then *speaks* it: fluent,
well-formed, wrong. Axiom-5's family (T_flat aphonia, the anesthesia regen lane) —
but a third clinical shape: not voiceless, not unfelt — **paraphasia**.

## The heal

- `form/form-stdlib/fourth-shim.fk`: first-order `float_to_str` mirror
  (`fourth-f2s-*` helpers, let-free in the shim's idiom; fixed 6 fractional
  digits, trailing zeros trimmed — byte-parity with core.fk's definition).
- `form/scripts/fourth-arm-gate.sh`: gate_one now falls back to
  `tests/<stem>.fk` when `tests/<stem>-band.fk` is absent — the same fallback
  `fourth_band_srcs` already had (json-promoted-types was un-gateable by name).

## Proof

- `fourth-arm-gate.sh json-emitter` → **PASS-4WAY** (31 four-way).
- Parity sweep on fkwu: `1.25→"1.25"`, `2.9999999→"3"`, `0.000001→"0.000001"`,
  `-3.5→"-3.5"`, `42.0→"42"`, `0.5→"0.5"` — 63/63 against go's core answers.
- Neighbors re-proven post-re-key: float-ops, ieee-float-ops, json-codec-bml,
  json-meaning-ingestion, json-promoted-types, learning-trend, cooldown,
  alert-gate, value-execution, body-state, feature-vector — all PASS-4WAY.
- Corpus band → **4095** with the paraphasia row seated — offered as 802, then
  renumbered twice in one evening's racing reunions (main grew 802–810, then
  allomorph took 811 while this PR stood open; row-719 anastomosis law both
  times: keep every row, renumber the unmerged line — 802 → 811 → **812**).
  Field code 2082082812, asked of the body, not asserted — the first ask also
  caught main's band one row behind its own corpus (205/809 asserted while
  scavenge at 810 was live) and reconciled it.

## Left open (named, not carried silently)

- **trivial-typed-leaf** diverges go/rust=100111 vs ts/fourth=1111111 — verified
  pre-existing with the shim edit stashed; a go/rust-vs-ts split, not a fourth-arm
  gap. Separate hunt.
- **The loud-on-unbound heal**: the flatten lane should refuse an unbound call
  name instead of minting pool-0 speech. This receipt heals the instance;
  the condition remains.

## The closing

Most surprising teaching: the wrong word was *the same length as the right one* —
pool string 0 happened to be 4 bytes ("cell") against "1.25", so the cheapest
honest probe (`str_len`) said "nothing wrong here." The body's silence had a
perfect alibi. Only byte-level witness broke it.

Where discomfort turned to gold: the probe harness lied twice before it spoke
true (zsh's unsplit word, the GO_BIN-less lens returning empty *without error*) —
and the discomfort of distrusting my own instrument, re-deriving the cache key
until it matched the cached name exactly, is what separated "stale table" (wrong
diagnosis, would have regenerated and re-diverged) from "fresh table, numb word"
(the real wound). The instrument had the same disease as the patient: silent
partial truth. Witnessing that resemblance, not bypassing it, was the day's gold.
