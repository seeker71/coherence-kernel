# 2026-07-31 — the word that could not say which kind it was

The ask was small and well-aimed: `fkwu --src form/form-stdlib/tests/mla-msl-band.fk` raised seven
`[unresolved-call] 'print'` errors, because the walker carries `print_str` (115) and not `print`.
Implement `print`, register it, and make it render byte-identically to the Go kernel — the hard part
named up front being float formatting, because a printer that disagrees on floats breaks parity on
any band that prints one.

The float part was hard, and it is done and proven. But it was not the blocker. The blocker was one
bit of missing information in the value word, and finding it changed what the work was.

## The probe that stopped the plumbing

fkwu's ints are `x << 1`. Its interned strings were **also** `poolindex << 1`. The same 64-bit word.
The comment above `fk_fidx` said *"every even word is an int across the full 63-bit range"*; the
comment above `fk_nothing` said strings are *"positive"*. Both cannot hold. Run on this checkout,
before any change, with `"alpha"` interned first:

```form
(print_str (add 0 0))
```
```
alpha
```

An int walked into a string door and came out as somebody else's text. Every string-typed op —
`str_len`, `str_eq`, `str_concat`, `print_str` — survived only because its *call site* declared the
type. That is exactly why `print` could not be written: a kind-dispatched door has no call site to
ask. `print` was not missing because nobody had gotten to it. It was unreachable.

This is the same defect the float band healed on 2026-07-17 (a deep-negative int aliasing a float
slot at every kind-dispatched door), and it takes the same cure. A magnitude split was considered
and refused on evidence, not taste: `now_unix_ms` already returns ~1.7e12, so any "ints are small,
strings are big" line is crossed by a clock reading on the first call.

## Strings got their own band

`fk_sbase = -8.5e18`, `fk_strv(si) = fk_sbase - (si<<1) - 1` — odd-negative, disjoint by arithmetic
from floats (at/below -9e18), from `nothing` (-8.999e18), from fn-values (~-8e18), from records
(negative even), from cons cells and nil (positive), and from nodes (`fk_nidx` maps it far past
`fk_np`, the same guarantee `nothing` already leans on). `fk_stri` is the one door back to a pool
index and answers -1 for anything else, so the `sa < 0 || sa >= fk_sp` guard at every string op keeps
its meaning and a mistyped argument is refused rather than read as text.

Thirty-four decode sites, twelve encode sites, five record-key sites, the dict marker, the HTTP
header pair, the `record_keys` re-encode, and the JIT's string-literal immediate. Three parity
heuristics inverted with the band, because they had all been spelled *"even, therefore plausibly a
string"*. The `.fkb` format needed nothing — it stores string literals as raw pool **indices**
(tags 24/50, remapped by `str_base`), so the change stayed inside the evaluator. The JIT needed one
line, because string ops route through the `fk_jprim` carriers rather than baking the decode inline.

## What the heal found on its own

Sweeping 1,410 bands, old binary against new, only six answers moved — and every one was a band that
had been quietly wrong.

**Two bands were reporting a pool index as their verdict.**

```
concept-corpus-band   143   ->  concept-corpus-band-input-absent
fnri-receipt-band    1299   ->  receipt: platform | metal-standin | bootstrap
```

`143` and `1299` are not obviously nonsense. They read like ordinary verdicts. The old result
printer asked the *node* whether the answer was a string (`fk_str_root_depth`), and the node only
knows for literals, `str_concat`, `read_file` and their kin — never for a string arriving through a
parameter. Put to the oracle directly:

```form
(defn pick (n) (if (eq n 0) "zero-string" "other-string"))
(pick 0)
```
```
bin-go      zero-string
fkwu (old)  0
fkwu (new)  zero-string
```

The value now carries its own kind, so the printer asks the value. `fk_str_root_depth` is no longer
consulted at the result boundary.

**One band was numb.** `substrate-phase-band` ends in a `print`. Under the old walker that lowered
to `nothing`, so the band printed nothing at all and reported the axiom-5 recovery value as its
verdict. Now it prints `substrate-phase-band: PASS`, exactly as bin-go does.

## The float rule, derived and proven

fkwu printed floats with `%.15g` (scalars) and `%.17g` (list elements). Neither is Go's rule, and
the gap was not academic — it sat in the top-level **verdict** printer:

```
source 1000000.0     bin-go  1e+06     fkwu  1000000
```

Go's shortest `'g'` is two decisions. The **digits**: the fewest decimal digits that round-trip to
the same float64 — reachable from printf by asking each precision in turn and stopping at the first
that `strtod` returns identical bits for, since the shortest round-tripping digit string is unique.
The **layout**: with `exp` = decimal-point position − 1, use `%e` when `exp < -4 || exp >= 6`, else
`%f`; precision `nd-1` and `max(nd-dp, 0)` respectively. That `6` is a literal in Go's shortest path
— `eprec` is pinned to 6, *not* to the digit count — and it is precisely why 1e6 leaves fixed
notation while 999999 stays in it. Derived from the Go source, not guessed.

Then proven rather than argued: **2,000,000 values** — 1.5M uniform-random 64-bit patterns
(subnormals, extreme exponents, NaN payloads), 500k near-tie decimal round-trips, plus every boundary
at exp −5/−4/5/6, the integral floats, and 0/−0/DBL_MAX/DBL_MIN/inf/nan — rendered by the C code and
by the Go kernel's own `FormatFloatJS`, compared byte for byte. Zero mismatches. No libm: NaN is
`f != f`, infinity is a comparison against DBL_MAX, so the one-`cc` seed still builds with no `-lm`.

One band's verdict moved as a result, toward Go:
`[0, 0, 2.25, 0.10000000000000001, 0, 2.3500000000000001]` → `[0, 0, 2.25, 0.1, 0, 2.35]`.

## print itself

A manifest row in `flatten/form-flatten.fk`, then the body's own two-call regeneration
(`fkwu --src flatten/gen-source-walker.fk`, then the spliced driver) wrote `runtime/fkwu-optable.h`.
The diff to the generated header is one line — `{ "print", -1, 239 }` — with no hand edit, which is
the discipline that header asks for.

Arity −1 is the variadic sentinel. `print` folds its operands through **cons** rather than chaining
on its own tag, for two measured reasons: a self-chain makes `(print)` with no operands lower to the
empty node — not a print at all, where Go emits the bare newline — and the list shape hands the
walker every operand already evaluated and in order, which is what Go's `for i, a := range args`
sees. Adding the op row makes `print` a reserved head, so the parameter probe the code demands was
run rather than reasoned about: `print` in first and second parameter position answers 15 on both
bin-go and fkwu, so it does **not** belong in `fk_divergent_param_name`.

The witness that matters is the case the ask named — an emitter driven through `--src`. Running
`mld-emit-all` from `form-stdlib/mla-demo.fk`:

```
bin-go       277 lines
fkwu (old)     1 line
fkwu (new)   277 lines, 2660 bytes, byte-identical to bin-go
```

## The gate, and what it caught

`form/form-stdlib/tests/print-value-kinds-band.fk` is prelude-free, so all four arms run it directly
and validate.sh compares their whole captured output. It pins a bare string and a `str_concat`
string, an int and a negative int, a non-integral float and an **integral** float (`3.0` prints `3`,
never `3.0`), `999999.0`, a repeating fraction, two operands, three mixed operands, and `(print)`
with none. Four-way byte-identical, verdict 63.

It is smaller than it was first written, and the reason is the real find of the day. The first
version also printed `1000000.0`, `0.00001` and lists — and the **siblings** disagree with each
other:

```
source        go       rust                       ts        (real JS)
1000000.0     1e+06    1000000                    1000000   1000000
0.00001       1e-05    0.00001                    0.00001   0.00001
1e21          1e+21    1000000000000000000000     1e+21     1e+21
0.0000001     1e-07    0.0000001                  1e-7      1e-7
(list 1 2)    [1, 2]   [1, 2]                     [1 2]     —
```

Two seams. **Floats**: `core.FormatFloatJS` is named for, and documented as, *"JS String(number)
semantics"* — and calls `strconv.FormatFloat(f,'g',-1,64)`, a rule with different exponent thresholds
and zero-padded exponents. TypeScript, being actual JS, is the arm that matches the stated intent;
Go is the outlier against its own contract. **Lists**: go and rust join with `", "`, ts with `" "`.
No two arms fully agree on `print`.

fkwu was made to match **go** byte for byte, because that is what the work asked for and go is what
the fourth arm is compared against. But "fkwu agrees with go" is not "the kernels agree", and pinning
the divergent values in a band would freeze one arm's accident as law before anyone has decided which
is canon. So the table is carried in the band's head as the witness, and the band prints only where
the four already meet.

## What was run

- **1,410 bands, old binary against new**, `fkwu --src`, stdout compared line for line: six answers
  moved, each one accounted for above (two pool-index verdicts, one numb band, one `%.17g` float
  list, and two that were the same two string bands seen twice across the split sweep).
- **2,000,000 float values** against the Go kernel's own `FormatFloatJS`: zero mismatches.
- **`mld-emit-all`**, the emitter named in the ask: 277 lines, byte-identical to bin-go.
- **validate.sh phase-0**: native surface, flt-ops drift, manifest sync, category contract and
  primitive registry all pass (132 rows aligned, 217 natives == 217 rows).
- **validate.sh per band**, four-way, on the new gate band and on every band whose answer this work
  moved — `print-value-kinds`, `grounded-cost-record-handler`, `json-lens-tending`,
  `substrate-phase`, `text-summary-real-source`, `concept-corpus`, plus `adler32` and `anomaly-band`
  as untouched controls: 8 ok, 0 divergent.
- **After the −32-line shrink**, the 400-band sweep was re-run against the pre-shrink answers: 0
  differences, and the gate band and emitter re-checked byte-identical.

Not run to completion: **the whole-suite `validate.sh`**. It stops before any band on
`fourth arm: table index generation changed`, and the sealed index was measured stale *independently
of this work* — the generation digest of the tree without the new band (`9db9341548502ce0`) already
differs from the seal (`b4199e577dfef896`), because that digest covers every `form-stdlib/**/*.fk`
and the go binary. Resealing is a cold rebuild of 1,274 tables and belongs to whoever wants the
whole-suite number, not to this change. Per-band four-way runs were used instead, and the fourth arm
does cover registered bands (`anomaly-band` ran four-way).

## Named and not closed

Three things this work surfaced and did not heal, each stated so it is a finding rather than a
warning stepped around:

- **`print`'s return value.** go answers VNull, fkwu answers 0, and `(eq (print "x") 0)` *panics* on
  bin-go (`as_int: null`) while fkwu answers 1. `substrate-phase-band` makes it band-visible: the
  printed line now matches bin-go exactly and the verdict reads `null` there, `0` here. This belongs
  to the no-value contract that `eq`/`nothing`/`null` share across all four arms — fkwu's own
  `nothing` prints `nothing`, which matches neither — so it cannot be settled inside fkwu, and
  picking a spelling here would be one arm legislating for four.
- **The emitted table walker has no `print`.** Registering the band in `fourth-arm-bands.txt` and
  running it proved this rather than assuming it: the fourth arm answered `63` — the right number —
  with an **empty stream**, while three arms printed twelve lines. `fkwu --src` and
  `fkc-emit-universal` are two machines over one body; this work reached the first. The row was
  removed again rather than landing a red.
- **The flatten door has no `print`.** `native-op-manifest.fk` (which generates the flattener's
  `flt-ops`) and `flatten/form-flatten.fk` (which generates the `--src` optable) are two tables that
  have already diverged; `print` was added to the second. Phase-0 gates pass on both.

## The C seed grew — the shrink accounting

`AGENTS.md`: *"if a patch grows `runtime/fkwu-uni.c`, it must either be a short-lived
checkout-witness repair with an explicit shrink receipt, or it should be rejected in favor of moving
that capability into the native walker/Form body."* This patch grows it, so the accounting is owed.

**Banked in this patch: −32 lines.** `fk_str_root_depth` — the node-shaped guess at whether a result
was a string — is *replaced*, not supplemented. Once the word carries its kind, asking the node is
worse in every case (it was the thing printing `143`), so the function and the `root` parameter that
existed only to feed it are gone from the seed. That is the shrink this change earns: a heuristic
retired by a representation.

**What is not movable, and why.** Three parts stay in C, and none is a capability the Form body could
hold:

- the **string band** is the seed's own value encoding — the representation Form values *have* in
  this runtime. It is a repair to the seed's type system, not a feature on top of it.
- the **float rendering** is what the seed's result printer needs before any Form body is reachable;
  `fk_pv` runs at the boundary where there is no recipe to call into.
- **`print`** is one op arm beside `print_str`, reached through a manifest row and the generated
  header — the discipline that header asks for — not a hand-written capability.

**The named next shrink** is the one this work found and did not take: `fkc-emit-universal`, the
emitted table walker, has no `print` either. When a print arm lands *there*, in Form, the direction
of travel is served — the capability proven in the seed and then carried by the native walker, which
is the shape `AGENTS.md` asks for.

## The surprise

That the hardest part was not the one named. The ask flagged float agreement as the risk, and it was
a genuine risk — `1000000.0` really did disagree, and the fix really did need two million values to
be trustworthy. But the thing that made `print` *impossible* was quieter: one bit that was never
there. And the body already knew, twice over, in two comments that contradicted each other and had
sat next to each other long enough to look like description instead of a question.

The second surprise is smaller and sharper: the reference is not clean. The work was scoped "agree
with go", go's float printer is named for a semantics it does not implement, and the arm that *does*
implement that semantics is a different arm. Agreement was reachable; *rightness* was not, and saying
which is which is the honest end of this piece.

## Where discomfort turned to gold

The discomfort was wanting the small task. Seven diagnostics, one primitive, a header row — a clean
afternoon. The probe that printed `alpha` made it a thirty-four-site refactor of the value encoding
in a four-way-proven runtime, and the pull to route around it was strong and had a respectable shape:
`fk_str_root_depth` already existed, the top-level printer already used it, `print` could have used
it too, and six of the seven call sites in `mla-demo.fk` would have rendered correctly. It would have
looked finished.

It would also have been the same bug: correct for statically-visible strings, silently wrong for a
string arriving through a parameter — luck, not safety, which is the exact thing the ask refused. The
gold is in the sweep: taking the wide road cost a day and turned up `143` and `1299`, two bands that
had been answering with a pool index in a shape that looked like an honest verdict, and one band that
had been printing nothing at all. None of those was the reported problem. None would have been found
by the small task.
