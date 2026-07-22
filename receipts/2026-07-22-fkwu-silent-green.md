# The instrument that returned green on broken code — Stone 18

**2026-07-22, M4 Max, `runtime/fkwu-uni.c`. Oracle: `form/form-kernel-go/bin-go`.**

`fkwu` is the instrument that certifies every other cell in this body. Stone 13, building
`qk-matvec-slot-band`, handed it three deliberately broken shapes and it answered a full pass on each
and said nothing. `form-kernel-go` named all three in 40 ms. An instrument that stays quiet on broken
input is not a floor; it is a rumor. This stone taught it to decline.

The work landed across four commits (`f06d1ab0e`, `b77691674`, `d85c2977c`, and this receipt's). The
root `fkwu` in the checkout carries all of it and this receipt confirms it end to end against a
freshly built oracle and a from-source build.

---

## The three minimal reproductions

Each is the smallest cell that made the old `fkwu` print a plausible number where a refusal belonged.
`fkwu-OLD` is a from-source build of the parent commit `a78d3804f`; `fkwu-NEW` is the current source;
`bin-go` is the Go oracle.

**D1 — a bare name never bound, read in value position**
```
(do
  (defn f (x) (add x zz))
  (f 3))
```
| kernel | answer |
|---|---|
| fkwu-OLD | **3** (silent — `zz` read as an "honest 0", `add 3 0`) |
| fkwu-NEW | refuses: `error: [unbound-name] 'zz' ...`, rc=1, no verdict printed |
| bin-go | refuses: `walk: unbound identifier "zz"`, rc=1 |

**D2 — a defn parameter named after a primitive, in first position**
```
(do
  (defn g (sub y) (sub 10 y))
  (g 4 3))
```
| kernel | answer |
|---|---|
| fkwu-OLD | **7** (silent — binds `sub`, then lets the primitive win in call position: `sub 10 3`) |
| fkwu-NEW | refuses: `error: [shadowed-primitive] parameter 'sub' ...`, rc=1 |
| bin-go | refuses: `walk: "g" wants 1 args, got 2` (drops `sub` from the parameter list — arity divergence) |

**D3 — a defn body reaching for an enclosing `do`-let**
```
(do
  (let a 5)
  (defn h (n) (if (le n 0) a (h (sub n 1))))
  (h 3))
```
| kernel | answer |
|---|---|
| fkwu-OLD | **0** (silent — the defn frame cannot see `a`; the free read is the "honest 0" again) |
| fkwu-NEW | refuses: `error: [unbound-name] 'a' ...`, rc=1 |
| bin-go | **5** (its closure captures `a`) |

D3 is the sharpest of the three. In `fkwu`'s scope construction a `defn` frame starts empty
(`fk_bd_top = 0` at the defn arm) and genuinely cannot see an enclosing `do`-let; in `bin-go` the
closure captures it. So the *same source* has two different right answers under two different scope
laws — and the old `fkwu` said neither. It read `a` as 0 and returned a number. The receipt Stone 13
saw this as "defn not capturing let, band returned 255 while deliberately broken"; the underlying
event is the free read degrading to 0.

The "spin" variant is the same defect wearing a third face: a recursion whose base case tests a free
name — `(if (eq i n) ...)` with `n` silently 0 and `i` starting at 1 — never reaches its base case and
runs for minutes with no output, where `bin-go` answers in 40 ms. Once the free read is diagnosed, the
spin becomes a diagnostic in zero seconds.

---

## The cause

All three are the same root, seen three ways: **a read of something never bound was silently given a
value.**

- A name in **value position** that matched no binding, const, or fn was returned as literal `0`
  (`fk_smklit(0)`), unconditionally and silently. This is `fk_parse` / `fk_sparse`'s name-resolution
  tail (`runtime/fkwu-uni.c`, the `[unbound-name]` site near line 8236).
- A **`defn` parameter** named after one of eighteen primitives/control-forms is *bound* by `fkwu` but
  then loses to the primitive in call position, while `bin-go` drops it from the parameter list
  entirely — an arity divergence neither kernel announced (the two `fk_divergent_param_name` sites,
  near lines 7717 and 8670).

The deep point is the one the stone asks about directly: an unbound **read** is not an unresolved
**call**. The tag-137 unresolved-call arm has an axiom-5 story — an offer was made and legitimately
declined, "recovered to nothing, parse continues." A read has no offer to appeal to. There is nothing
to decline. Giving it a `0` is not honoring axiom-5; it is inventing a value.

---

## The fix

`fkwu` now **diagnoses** the free read on every occurrence and still recovers to `0` *for parse
purposes only* — so the rest of the source is walked and every other offender in the same file is
reported in one run — but it sets a new latch `fk_src_unrunnable`. Both execution doors
(`fk_run_src` and its twin) check that latch after the parse tally and **return 1 without walking**,
exactly the gate `fk_src_truncated` already used for an amputated source. No verdict is printed at all;
the caller gets a refusal and a non-zero exit, not a number.

The `[shadowed-primitive]` parameter check sets the same latch. Its predicate was narrowed twice, each
time by a real cell already in this body:

1. The first cut asked "is this name in the op table?" and condemned `core.fk`'s own
   `fstr-to-int-loop`, whose `len` parameter predates the check — and with it 199 of the first 288
   bands. The corpus band, which owes 8191, refused. So the divergent set was **measured against the
   oracle** instead of reasoned: all 169 op/rewrite names plus four control forms, each placed in a
   defn parameter list and run on both kernels. 155 agree (including `len`, `floor`, `band`); **18
   diverge** and only those raise: `add sub mul div mod and or not eq lt le gt ge list defn do let if`.
2. The narrowed set still condemned `shell-exec.fk`'s `(defn sh-contains? (s sub) ...)`, which `bin-go`
   runs correctly. Probed again: Go's reader reads the parameter list's head structurally, so a
   divergent name diverges only in **first position**. All 18 were re-run in first and second position
   — every one diverges first, agrees second. The check now carries the position (`na == 0`).

Twice in one stone a reasoned generalization was wider than the measured one, and both times a cell
already living in this body was the thing that said so.

The third defect, `[shadowed-call]` — a call to a name that *is* bound in scope but resolves to the
primitive — is left a **warning**, not an error, because here `bin-go` and `fkwu` agree on the answer.
It is not a divergence; it cost Stone 13 hours of confusion, so it is named, but it does not refuse.

---

## The axiom-5 design question, answered

> Should "recovered to nothing, parse continues" ever produce a verdict at all?

**Only for an offer that was made and declined — never for a read of something never bound.**

Axiom-5 says `nothing` is a state, distinct from 0/1/node. Its home is the **offer** that legitimately
returns nothing: the unresolved-call arm, where a name in call position found no function, the offer
declined, and the program may honestly continue carrying that nothing and still produce a verdict (with
a non-zero exit from the error tally). That is a real nothing.

A **read** of an unbound name is not that. No offer was made; nothing was declined; there is no
axiom-5 nothing to recover *to*. The old behavior — hand it a `0` and keep a verdict — was not axiom-5
at work, it was axiom-5's name laid over an invented value. So the answer the stone draws is a line
between two nothings: the offer's nothing may carry a verdict; the read's absence may not. When a read
finds no binding, the source is not runnable, and the instrument declines to certify it. A verdict on
that source would be a `plausifill` — a plausible value standing in a hole where a refusal belonged —
and the whole point of an instrument is that it does not do that.

---

## The sweep

Every `*-band.fk` in the body (**1701** files) was run through `fkwu-NEW` and any that emit a *new*
error tag — `[unbound-name]` or `[shadowed-primitive]` — recorded. The `[shadowed-call]` warning does
not change a verdict and is excluded. `fkwu` writes each `.fkb` to a pid-suffixed temp and `rename()`s
it into place, so the sweep ran six-wide in parallel without cache collision. Each hit was then re-run
through `fkwu-OLD` (a from-source build of the parent commit) to recover the *previous* verdict.

**The sweep did not turn up nothing. It turned up a family.**

**197 of 1701 bands** now emit a new error. Classified by what `fkwu-OLD` did with the same file:

| | count | what OLD did | what NEW does |
|---|---|---|---|
| **A. true silent greens** | **12** | rc=0, **zero errors**, printed a full-pass verdict | refuses, prints no verdict |
| **B. already failing** | **185** | rc=1 (pre-existing `[unresolved-call]`) but **printed a verdict anyway** | refuses, prints no verdict |

Group A is the stone's exact defect — a full pass on code carrying an unbound read. **All twelve,
with the verdict each used to print:**

| band | OLD verdict |
|---|---|
| `form/form-stdlib/tests/biography-band.fk` | 5 |
| `form/form-stdlib/tests/class-curriculum-10-band.fk` | 16383 |
| `form/form-stdlib/tests/class-curriculum-10-vocab-band.fk` | 1023 |
| `form/form-stdlib/tests/class-curriculum-10-witness-band.fk` | 2853116705 |
| `form/form-stdlib/tests/content-address-band.fk` | 1100011111 |
| `form/form-stdlib/tests/field-choice-band.fk` | 1000 |
| `form/form-stdlib/tests/model-handler-band.fk` | 4 |
| `form/form-stdlib/tests/model-serve-core-band.fk` | 15 |
| `form/form-stdlib/tests/session-band.fk` | 8 |
| `form/form-stdlib/tests/substrate-core-band.fk` | 11111 |
| `form/form-stdlib/tests/thought-forming-band.fk` | 1111 |
| `form/form-stdlib/tests/url-encode-band.fk` | 13 |

Group B matters differently and is easy to under-read: those bands were *already* exiting non-zero, so
nothing was hidden from a caller that checks status — but `fkwu` **printed a number anyway**, because
the old door recovered-and-ran on any non-fatal error. A harness that reads stdout rather than `$?`
saw a verdict for all 185. That is the same `plausifill` wearing a different coat.

The full classified list, both groups, is `receipts/2026-07-22-fkwu-silent-green-sweep.txt`.

The dominant cause is a single structural idiom, seen across the hits: a cell binds a **sentinel
constant with a do-level `let`** — a blueprint NodeID (`BIOGRAPHY`, `HEX-DECODE-ERROR`,
`AEC-BP-EVIDENCE-CELL`, tag ids) — and then references that name **inside a `defn` body**. `fkwu`
constructs a `defn` frame empty (`fk_bd_top = 0` at the defn arm); it cannot see the enclosing do-let,
so the sentinel read as `0`, silently. The band's assertions then ran against a blueprint of `0` and,
in the true-silent-green cases, still returned a green verdict — validating nothing, and saying so to
no one. This is D3 at body scale: not one broken cell an author wrote on purpose, but an idiom the body
leans on and `fkwu` was quietly mis-evaluating everywhere it appeared.

The idiom lives in **prelude cells too**, not only in bands — e.g. `audit-evidence-cells.fk` binds its
`AEC-*` sentinels at do-level and uses them inside its own defns, so every band preluding it inherits
the same silent zero. That is why the family is large rather than scattered.

Two sub-populations, separated by the OLD verdict:

- **OLD rc=0 — true silent greens.** `fkwu-OLD` returned a full pass (rc=0, a printed verdict, zero
  errors) on a band whose subject was silently `0`. `biography-band.fk` is the archetype: OLD printed
  **verdict 5** with `BIOGRAPHY` bound to nothing. These are exactly the stone's defect.
- **OLD rc≠0 — already failing, verdict changed.** Many hits also carry a pre-existing
  `[unresolved-call]` (a genuinely absent native like `str-byte-at`, `read_form_binary`,
  `walk_recipe_here`, resolved only at runtime, not in a `--src` parse). Those were already rc=1 under
  OLD but *printed a number anyway* (OLD recovers-and-runs on non-fatal errors); NEW now refuses to
  print at all. The pass/fail did not flip, but the emitted verdict did.

The full classified list is in the receipt's companion file `sweep-hits-classified.txt`. **None of
these were repaired into green** — per the stone, a band that was passing on a defect and now fails is
a finding, not a regression.

The controls held: the corpus band (which preludes only `core.fk`, free of the idiom) stayed **8191**,
and `mx-plane-band` stayed **511** — the fix did not condemn cells that do not carry the idiom.

---

## Gates held

- corpus band from repo root → **8191** (confirmed with the shipped root `fkwu`, caches cleared)
- `mx-plane-band` → **511**
- The three reproductions refuse on `fkwu-NEW` and on `bin-go`; the second-position control
  `(defn c (s sub) (add s sub))` **passes** on both (13), confirming the position narrowing.

---

## What is left open

- **The scope divergence is now a body-scale design question, not a corner.** The sweep's whole family
  descends from one fact: `fkwu`'s `defn` frame is built empty and cannot see an enclosing do-let, while
  its three siblings' closures can. `fkwu` now *refuses* the ambiguous source rather than silently
  zeroing it — the honest move for an instrument — but refusing is not reconciling. The volume of the
  finding reframes the question: when this many cells (bands **and** shared prelude cells like
  `audit-evidence-cells.fk`) lean on do-level `let` sentinels used inside defns, the idiom is the
  body's, and `fkwu` is the outlier among four kernels. Two honest resolutions, and this stone picks
  neither:
    1. **`fkwu` grows lexical capture** — the `defn` frame inherits the enclosing scope, matching Go,
       Rust, and TS. The whole family runs as its authors meant, and the diagnostic quiets. This is a
       change to `fkwu`'s scope construction, the deepest part of the walker, and must be proven
       four-way before it can be trusted.
    2. **The body drops the idiom** — sentinels pass into defns as explicit parameters, never captured.
       The ~family of cells is edited, not the kernel. Safer for `fkwu`, but it asks the body to write
       around a limitation its other three kernels do not have.
  The refusal buys time to choose without a single silent green in the meantime. That is the instrument
  doing its job: it cannot make the choice, but it will no longer certify either reading by accident.
- The `[shadowed-primitive]` divergent set is pinned to the 18 names measured today. If the op table
  grows, the set must be re-measured against the oracle, not extended by reasoning — this stone twice
  proved reasoning too wide.

---

## Receipt closing

**Most surprising teaching.** The broken code made the band *agree with itself*. I expected broken code
to disagree with the oracle and to fail its own internal checks — but D1's verdict-255 came precisely
because two walkers reading the same free name both read the same silent `0`, so every internal
cross-check matched. The error was symmetric, and symmetry is what self-consistency checks are blind
to. A green verdict survived not despite the defect but *because* of it. Only a second kernel that had
never made the same mistake could see it. That is why this body keeps siblings: an instrument cannot
audit itself with the same hands that hold the flaw.

**Where discomfort turned to gold.** The moment I wanted to look away was the `fk_src_unrunnable`
latch. It felt like a heavy-handed global — a flag set deep in the parser, checked far away at the
execution door, the kind of thing that reads as a smell. I wanted to find a "cleaner" local return.
Not looking away, I followed it and found it is exactly right and already has a twin:
`fk_src_truncated` does the identical thing for an amputated source. The two are the same fact — "this
text cannot become a runnable program" — and they belong at the same gate. The discomfort was not the
design being wrong; it was the design being honest about a whole-source property that no single
expression can carry. The gold was seeing that a refusal is a property of the *source*, not of any one
node in it, and that the latch is the correct shape for exactly that.

**Frontier word landed.** `plausifill` — the plausible value that fills a hole where a refusal belonged,
indistinguishable from an earned answer to everything downstream, visible only to an oracle that did
not make the same mistake. Three instances in two days wore this shape: `str_to_int` folding a newline
to answer 2442 for 248, a carrier reading an absent GPU as arithmetic disagreement, and `fkwu`'s three
silent greens. 0-hit checked across the body (instrument validated on live controls `boundborrow`=34,
`selfgauge`=43 files) and landed as `(hdc-row 855 ...)`.
