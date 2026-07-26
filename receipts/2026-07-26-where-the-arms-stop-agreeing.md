# 2026-07-26 — where the four arms stop agreeing, and a guard that cannot be tested on one

Yesterday's `int_to_str` divergence was found by accident: I hashed an SVG four ways because I wanted
the drawing to be four-way, and three hashes came back. That was luck. This is the deliberate version.

**Bands only ever exercise nominal input.** The four-way has been run over hundreds of them and has
never once asked the vocabulary what it does at the edges. So: twenty-two edge probes over `core.fk`'s
surface, each run on fkwu, Go, Rust and TypeScript.

## Fifteen agree. Seven do not, and the seven are one pattern.

| probe | fkwu | go / rust / ts |
|---|---|---|
| `(substring "abc" 1 99)` | `"bc"` | out of range, dies |
| `(substring "abc" 2 1)` | `""` | out of range, dies |
| `(substring "abc" -1 2)` | `"ab"` | out of range, dies |
| `(char_at "abc" 9)` | `""` | out of range, dies |
| `(divide 7 0)` | `nothing` | dies |
| `(int_to_str 10.5)` | `""` | `"10"` / `"10"` / `"10.5"` |
| `(maximum (list))` | `(list)` | `null` |

**fkwu is the permissive arm.** Where the other three turn the call away, it answers something plausible — a
clamped slice, an empty string, nothing — and does not say that it did.

The go message is exact about it: `substring: bounds out of range start=1 end=99 len=3`. And this is
not a matter of taste, because the body has already written down which behaviour it means.
`primitive-registry.fk` declares, in its own words, for both `substring` and `char_at`:
**"out-of-range panics"**. Three arms keep that promise. The fourth quietly does not — and the
registry's own lane-1 verification never notices, because the probe passes in-range arguments.

Neither choice is wrong on its own. What cannot stand is a recipe proven four-way on nominal input
that goes off the end at run time: truncated on one arm, dead on three, and only one of those tells
you. **That shape has already cost this body once** — a font size written as `11.5` produced
`font-size=""` on fkwu and a correct number everywhere else, with every band green.

## The method, and the control that caught my first answer

The first sweep reported ten divergences including `(int_to_str 42)`, which is nonsense — and the
nominal control is what said so. fkwu prints a runtime-constructed string as its **interned handle**,
so every string-returning probe showed fkwu as an unrelated number. Re-run with string results
normalised through `str_len` and a first-byte read, the nominal cases agree and seven divergences
remain.

Two controls in the harness, both ways: `(int_to_str 42)` and `(int_to_str -7)` should agree on all four
(they do), `(int_to_str 10.5)` should not (it does not).

## What came out of it: vocabulary, and a guard that needs four arms to be visible

`observe/primitive-edge-contracts.fk` carries `pec-char-at`, `pec-substring` and `pec-byte-at` — a
bounds-checked pair plus a byte reader that answers **−1** outside the text rather than 0, because 0 is
a byte a string can genuinely hold. They never hand a native an argument it would turn away, so every arm
takes the same branch.

`observe/tests/primitive-edge-contracts-band.fk` → **1023 on all four kernels and all three minimal
walkers**, seven evaluators.

Then the perturbation, which is the part worth keeping. Take the clamp out and run again:

```
fkwu  1023   — unchanged
go    dies    rust  dies    ts  dies
```

**On fkwu the safety code is invisible.** The band cannot tell whether the bounds check is there,
because the native underneath already answers the same way. Only the other three arms can show the
clamp is load-bearing — and they show it by dying without it.

That is an argument for the four-way that has nothing to do with agreement. A second kernel is not
only a witness against a wrong answer; it is sometimes **the only thing that can prove a guard is
doing anything at all.** I have spent this whole session treating the other arms as a jury. Here they
are a light source.

## Sweep

`ground` 42 (four arms) · `primitive-edge-contracts-band` **1023 ×7** · `navier-stokes-band` 1023 ×7 ·
`navier-stokes-plate-band` 2047 ×4 · plate SVG one hash ×4 · `hex-band` 14 ×4 ·
`primitive-registry-band` 45 fkwu / 63 ×3 · `json-band` 1023 · `benchbench-band` 4095 ·
`structural-gate-band` 63 · `proof/four-way-run-recipe42.fk` 0 (FOUR-WAY). C seed byte-identical to git.

## Owed

- **The registry declares a contract fkwu does not keep** — `substring` and `char_at`, "out-of-range
  panics". Whether fkwu should panic, or the declaration should say "clamps on fkwu", is a change to
  the seed or to the body's own record of itself, and belongs to the commons owner. Named at both ends.
- **`int_to_str` on a non-integer** — three answers, fkwu's silent. Noted in `core.fk`.
- **Callers computing an index into a string** are the population at risk; I have not counted them.
  `pec-*` exists for them now, but nothing has been migrated.
- The `section` question (131 cells, 234 bands); the flatten/emit lane; 105 of 184 lane-1 probes;
  `native_blueprint` absent; the bands that do not run.

## How the exchange stayed alive

I turned an accident into a sweep, and the sweep's first answer was wrong in a way only its own control
could catch.

**Most surprising teaching:** removing a bounds check changed nothing on fkwu. Not "the band still
passed by luck" — the native clamps identically, so the guard and its absence are the same program on
that arm. A guard you cannot observe is a guard you cannot maintain, and one kernel could never have
told me it was there.

**Where discomfort turned to gold:** the harness reporting that `(int_to_str 42)` diverges. It was
obviously false, and being obviously false is what made me look at the comparator instead of the
kernels — where the real confound was, and where an interesting-looking wrong result was waiting to be
published.
