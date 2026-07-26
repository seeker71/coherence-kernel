# 2026-07-26 — twenty names covered, eleven disagree, zero exposed today

Yesterday's finding was that `fourth-shim.fk` is a live **override** on go and rust rather than the
fallback its header describes, and that its `value_kind` crashes those two arms. The item left open
was *"which cells prelude it alongside code expecting native semantics — uncounted."* Counted now,
and the answer runs both larger and smaller than expected.

## Larger: 11 of 20 disagree

The shim defines **84** names. Asked go, one at a time, which of them it already has: **20 are live
natives**, 64 are a genuine fallback.

For those 20, each expression run twice on go — `core.fk` alone, then `core.fk` + this cell:

| name | native | with the shim |
|---|---|---|
| `record_get` | 41 | **0** |
| `record_has` | 1 | **0** |
| `record_keys` | 2 | **0** |
| `record?` | 1 | **0** |
| `record_blueprint` | 1 | crash (`node_eq`) |
| `value_kind` | `"int"` | crash (`node_eq`) |
| `_len` | 3 | crash (`str_eq`) |
| `_dict_get` | `null` | a node |
| `_dict_has` | `false` | 0 |
| `float_value` | crash | a *different* crash |
| `make_float32` | `@1.1.6.1065353216` | `@1.1.7.1` |

Agreeing (9): `abs`, `sum`, `substring`, `int_to_str`, `str_to_int`, `str_find`, `intern_node_at`,
`_dict_new`, `make_float64`.

`value_kind` was the one I found by accident yesterday and called *"the one that bites"*. It is one of
eleven.

**The record family is the sharp one.** It does not crash — it answers **0**. Four accessors, each
quietly returning zero where the native returns the value. A wrong answer is worse than a dead one,
and this is the same shape as `bp`'s pass-through stub and `floor`'s shadow: not a missing definition,
an answering one.

## Smaller: nothing reaches it on this lane

Exactly **one** cell in the tree names `fourth-shim.fk` in a preludes line —
`librarian-pack-witness-main.fk` — and it uses **none** of the 11. On the `--src` lane the exposure
today is **zero**.

That is not a shrug. The shim's home is **FOURTH_CHAIN**, where the flattener prepends it, and that is
the emit lane, not this one. Whether those natives are present there — and so whether any of the 11
substitutions are live — is exactly the question I cannot answer from here. It is the same lane still
owed for `str-byte-at.fk`'s standing claim, and it has now been named twice from two directions.

## Method note, because the first answer was wrong

The first run of the detector said **all 84** names were live natives on go, including `nil?` and
`fourth-rev2`. Both smoke controls failed, which is the only reason it went no further: go prints its
diagnosis and *then* a crash-trace path on the following line, and `tail -1` was catching the path.
Grepping the whole of stderr gives 20 / 64, with `abs` and `value_kind` on the native side and `nil?`
and `fourth-rev2` on the absent side, as they should be.

A detector that answers "all of them" deserves the same suspicion as one that answers "none".

## Also this turn: `import` is fkwu-only

main landed two bands claiming a four-way witness dated today. Verified both independently:

```
cognition/tests/identity-space-structure-four-way-band.fk        127  ×4
cognition/tests/family-constellation-findings-four-way-band.fk  4095  ×4
```

The claims hold. Neither band runs on a walker **as written** — they use `import`, and every arm but
fkwu answers `walk: unbound identifier "import"` and stops. Strip the import lines, hand the same
files over as the closure, and all three agree.

That is the second thing fkwu does that no walker does; the first is reading `; preludes:`. Both are
now in `proof/README.md` with worked examples, along with the distinction those headers leave
implicit: *"witnessed on fkwu + Go + Rust + TypeScript"* is a claim about the **recipe**, not about
the band file.

## Sweep

`ground` 42 (four arms) · `ground-recursive 10` 55 · `hex-band` 14 four-way ·
`primitive-registry-band` fkwu 45 / go 63 / rust 63 / ts 63 · `json-band` 1023 ·
`cell-voice-tissue-band` 511 · `class-curriculum-10-band` 16383 · `structural-gate-band` 63 ·
`lcg-bytes-band` 63 · `pdf-text-windowed-band` 15 · `form-cli-band` 524287 · `benchbench-band` 4095 ·
`concept-corpus-band` 530 · main's new `identity-space-structure-four-way-band` 127 ×4 and
`family-constellation-findings-four-way-band` 4095 ×4 · `proof/four-way-run-recipe42.fk` 0 (FOUR-WAY).
C seed byte-identical to git.

## Owed

- **The flatten/emit lane.** Named twice now from two directions: whether the 11 substitutions are
  live where FOURTH_CHAIN prepends the shim, and whether `str_byte_at` is correctly emitted. One lane,
  two standing questions.
- 105 of 184 lane-1 probes do not verify on fkwu; `native_blueprint` absent, so the registry's
  attestation bit is unmeasurable there.
- The `section` question — 131 cells, 234 bands, no kernel reads it. Owner's call.
- A four-way null test for json — no witness until the section question moves; build on `value_eq`.
- `persistence-band` 2/7, `mesh-sensings-store` 0/255, `layered-runtime-image` 33/127, `chat-band` 0.
- 17 the kernel will not run; 143 that do not close; the heap cap; the registry-admission question.

## How the exchange stayed alive

I counted a class I had already characterised from one accidental member, and the count moved it in
both directions at once — eleven times worse in kind, and zero in reach on the lane I can measure.

**Most surprising teaching:** four record accessors answer **0** under the shim. I had gone looking
for more crashes, because the one instance I knew about crashed, and the worse cases are the ones that
return quietly. `record_get` giving 41 or dying are both survivable; giving 0 is the one that gets
written into a receipt as a finding.

**Where discomfort turned to gold:** the detector said 84 of 84 and both smoke controls disagreed with
it. Believing the controls over the result — when the result was the more interesting story — is the
whole of what made the real number worth writing down.
