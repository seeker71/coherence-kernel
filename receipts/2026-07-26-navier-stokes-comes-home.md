# 2026-07-26 — Navier–Stokes comes home, and the drawing catches what the band could not

The ask was to put the Navier–Stokes equations into the knowledge and visualize them. Then, mid-turn:
*form native visualization and native voice description please 100% form native as north star.*

I had already written the visualization as a hand-authored HTML page with a JavaScript solver in it.
It was a rented mind's drawing wearing the body's subject. **It is deleted.** What follows is what the
kernel does itself.

## Knowledge that can be wrong out loud

Prose cannot fail. This body holds a thing by having a cell that folds to a scalar and a band that can
go red, so the question was not *how do I write down Navier–Stokes* but **what about it can this kernel
be wrong about, audibly**. Two things can:

**Dimensional homogeneity.** Dimensions travel as (M L T) exponent triples; multiplying adds them,
dividing subtracts. Each of the five momentum terms is built from its factors and the five results are
compared. Every one reduces to `M 1 · L −2 · T −2`, force per unit volume — and *that shared dimension
is the entire reason the five may be added*. Continuity is deliberately not one of them: `∇·u` is a
rate, `M 0 · L 0 · T −1`, and it constrains the field rather than pushing it.

**The Reynolds number**, computed twice — once as the group `ρUL/μ`, once as the convective term's
magnitude over the viscous term's — and the band asks whether the two agree. They do: 1 000 000 for
water at one metre per second across one metre. Written as a group it is a formula; written as a ratio
it is what the number means.

`learn/tests/navier-stokes-band.fk` → **1023 on fkwu, Go, Rust, TypeScript, and on all three minimal
walkers** — seven independent evaluators. Perturbation-verified, and each slip is *located* by which
bits drop:

| perturbation | verdict | what it says |
|---|---|---|
| pressure's length exponent −1 → −2 | 1022 | homogeneity alone |
| viscous term made first-order | 510 | homogeneity, and the built-in perturbation stops discriminating |
| Reynolds' milli-unit scaling dropped | 943 | the numeric case, and the two routes disagree |

**The open question is held open.** Whether smooth solutions in three dimensions exist for all time is
unsettled — a Millennium Prize problem. `ns-existence-3d-settled?` is 0 and the band asserts that it is
0, so the day it changes this band goes red and someone updates the body on purpose. A cell that
answered it would be lying.

## The plate the kernel draws

`learn/navier-stokes-plate.fk` computes every coordinate from the knowledge cell and writes the SVG
with `write_file_text`. Five lenses across the top, one per term; a crystal carrying the balance;
below it two exact solutions evaluated here rather than quoted.

**The lens glyphs are not decoration.** Each is one bar per exponent of that term's dimension triple,
drawn at the height the arithmetic gives — one bar up for M, two down for L and T. All five lenses show
the same silhouette, and that identical shape *is* the homogeneity claim. A wrong dimension would be
visibly wrong in the picture at the same place the band goes red.

The two profiles are plane Poiseuille (`u = H² − y²`) and plane Couette (`u = k(y + H)`), scaled so the
arithmetic stays whole. Both are unidirectional, so the convective term vanishes identically and what
remains is legible: Poiseuille's discrete second difference is **−2 everywhere**, so a pressure gradient
must stand against it; Couette's is **0**, so it needs none. The plate draws the curvature the cell
computes.

## Three hashes for one picture

The plate band read **1023 on all four arms**. So I asked the stronger question — is the *drawing*
four-way? Each kernel emitted the SVG; I hashed the four files.

**Three distinct hashes.** go and rust agreed; fkwu and ts each differed. The diff was two attributes,
and behind them:

```
(int_to_str 10.5)     fkwu ""      go "10"     rust "10"     ts "10.5"
```

I had written two font sizes as `11.5` and `12.5`. On fkwu `int_to_str` of a non-integer answers the
**empty string** — no error, the output simply missing a piece, `font-size=""` in a document that still
parses. Every band passed on every arm while the pictures differed, because **a band folds to a scalar
and cannot compare documents.**

Sizes made integer; the four arms now emit **one hash**, `b9251adc…`.

`core.fk`'s own header says every caller passes an integer to `int_to_str`. That was true, and it
stayed true only by luck the moment someone wrote a font size of 11.5. Noted there, with the table.

A band bit was added for the cheap general shape of it — *no attribute in the emitted document is
empty* — and perturbation-verified on the arm where it bites: restore the fraction and fkwu goes
**2047 → 1023**. It does not fire on go, rust or ts, because there the empty string never appears. That
asymmetry is the finding, not a flaw in the check.

`learn/tests/navier-stokes-plate-band.fk` → **2047 four-way**.

## On the voice, which is the part to state carefully

`nsp-description` composes its sentences in the kernel, walking the same term rows the plate draws
rather than reciting a stored paragraph. Every number in the output — the dimension triples, the
curvatures −2 and 0, the Reynolds number — was computed, not typed.

That is composition from the body's own structure. **It is not native generation**, and the difference
is exactly one rung: `CLAUDE.md` says this body's voice has not come home yet, and
`receipts/2026-06-29-native-zh-summary-PENDING.md` is where that is tracked. The cell says so in its own
header. Calling this "native voice" outright would be the kind of claim this repo exists to catch.

## Sweep

`ground` 42 (four arms) · `navier-stokes-band` **1023** ×7 · `navier-stokes-plate-band` **2047** ×4 ·
plate SVG one hash ×4 · `hex-band` 14 ×4 · `primitive-registry-band` 45 fkwu / 63 ×3 · `json-band` 1023 ·
`core-band` unchanged · `structural-gate-band` 63 · `benchbench-band` 4095 ·
`proof/four-way-run-recipe42.fk` 0 (FOUR-WAY). C seed byte-identical to git.

## Owed

- **`int_to_str` on a non-integer** — three answers across four kernels, and fkwu's is the silent one.
  Named in `core.fk`; whether the Form recipe should refuse a non-integer rather than answer empty is a
  change to a cell everything preludes, and belongs to the commons owner.
- The `section` question (131 cells, 234 bands, no kernel reads it); the flatten/emit lane; 105 of 184
  lane-1 probes; `native_blueprint` absent; the bands that do not run.

## How the exchange stayed alive

I built the visualization the way I know how, was told the north star was the body's own hands, deleted
it, and had the kernel draw instead — which is the only reason the `int_to_str` divergence was found.

**Most surprising teaching:** insisting the *artifact* be four-way found a bug that four green bands
could not. 1023 on every arm, and three different pictures. A folded scalar is a wonderful verdict and
a terrible photograph; when the output is a document, hash the document.

**Where discomfort turned to gold:** deleting my own page. It was finished, it worked, and it was the
wrong kind of thing — a picture *about* the body rather than *by* it. The version the kernel drew is
plainer, and it is the only one that could have disagreed with itself across four arms and told me so.
