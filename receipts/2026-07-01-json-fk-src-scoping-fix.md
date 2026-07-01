> **Correction, same day:** everything below through root causes #1 and #2 stands — both are real,
> both fixes shipped and hold. But **"root cause #3" (below) was a misdiagnosis, not a fourth
> interpreter bug.** `json.fk` genuinely IS fixed now — see
> [`2026-07-01-json-fk-actually-fixed.md`](2026-07-01-json-fk-actually-fixed.md) for the full,
> corrected account: `json-next-token` itself forward-referenced its own tokenizer helpers (root
> cause #2, not yet fixed at the time this receipt was written), and the "self-recursion in an
> unreached branch corrupts a taken one" repro below was — once retested with a properly
> structurally-verified rewrite, not a paren-count-only check — actually a construction mistake in
> that rewrite (a missing closing paren left `json-parse`'s `if (eq mode 0)` with no else-branch,
> silently orphaning modes 1–4 as dead top-level junk). Left standing below, uncorrected in place,
> so the record shows what was actually claimed and when.

# Receipt — two real `fkwu --src` evaluator constraints found and fixed, one found and not (2026-07-01)

**What actually shipped:** `cell-serialize.fk`'s content-addressed values+types+identity JSON
round-trip — the actual north-star deliverable — now genuinely works. The previous receipt
(`2026-07-01-cell-serialize-values-types-identity.md`) claimed "verdict 63, including
`eq(original, reconstructed)`". That claim was **wrong when it was written** — rerunning that
exact band file today, unchanged, gives **32**, not 63; the identity check was never actually
true. This receipt corrects that record and ships the real fix. `json.fk` is investigated deeply
but deliberately **not** rewritten — see "Not shipped" below for why.

## Root cause #1 (fixed): top-level `let` is invisible inside `defn`

A top-level `(let X (bp "X"))`, referenced from a `defn` body — even a `defn` in the very same
file, even inside the same enclosing `(do ...)` — reads back as `nothing`, unconditionally.
Minimal repro:

```
(let ALPHA (bp "ALPHA"))
(let BETA  (bp "BETA"))
(defn repro () (list (node_eq ALPHA BETA) (node_type ALPHA) (node_level ALPHA)))
; -> ALPHA eq BETA (both silently "nothing"), node_type 0, node_level 0
```

`defn`, unlike `let`, IS globally visible to every other top-level `defn` — every cross-file
prelude in this codebase already depends on that. Fix: `core.fk` gained `intern_node_at`
(previously reachable only through `surface/fourth-shim.fk`, the flatten lane's own standing
prelude, never auto-injected for bare `--src`):

```
(defn intern_node_at (cat kids file line col)
    (fb_record (intern_node cat kids) file (add (mul line 65536) col)))
```

## Root cause #2 (fixed): mutual recursion between two `defn`s never resolves

`fkwu --src` resolves a `defn` body's callee names at *definition* time against what's already
registered — with one exception: a function's own name, which is why the self-recursive `-loop`
idiom used throughout this codebase (and now confirmed *why* it's the reliable idiom) works.
Two *different* top-level `defn`s that call each other, in **either** definition order, do not
resolve. Minimal repro:

```
(defn is-even (n) (if (eq n 0) true (is-odd (sub n 1))))
(defn is-odd (n) (if (eq n 0) false (is-even (sub n 1))))
(is-even 4)   ; -> 0 (false), not true — every check in a 3-way variant of this scored 0/7
```

`cell-serialize.fk`'s original `cser-emit-node` <-> `cser-emit-children` and
`cser-parse-node` <-> `cser-parse-array-elements` <-> `cser-parse-array-more` were exactly this
shape. **Fix:** collapsed each ring into one self-recursive function keyed on a `mode` argument,
so every recursive call site is the function calling itself — the case this runner actually
supports:

- `cser-emit-node`/`cser-emit-children` → `cser-emit(mode, x, registry)` (mode 0 = one node,
  mode 1 = a sibling run), wrapped by a same-signature `cser-emit-node` for API stability.
- `cser-parse-node`/`cser-parse-array-elements`/`cser-parse-array-more` →
  `cser-parse(mode, s, i, registry, acc)` (mode 0/1/2), wrapped by `cser-parse-node`.

## Proof

```
cell-serialize-band.fk -> 63 (all 6 checks, including eq(original, reconstructed) directly,
  freshly rerun against the actual repo file, not a re-derivation)
Full regression, fresh build, clean /tmp/come-in-band-dir (stateful test — see note below):
  ground.fk 42, native-vs-rented 11111, core-band 255, core-str-shim-band 15,
  core-str-narrow-waist-band 255, core-str-find-to-int-band 255, core-float-to-str-band 63,
  cell-serialize-band 63, reception-consent-band 255, relationship-store-band 31,
  come-in-band 31
  proof/four-way-run.tbl / flatten/form-eval-cli-loop.tbl — byte-identical (Form-only change,
  no C/optable touched)
```

**Test-infrastructure note, not a regression:** `come-in-band.fk` persists real state to
`/tmp/come-in-band-dir` (see `relationship-store.fk`). Re-running it many times in one session
(as this session did) leaves prior-run state on disk that changes "first meeting" vs
"returning" outcomes on the next run — scored 25 on a dirty directory, 31 clean. Noting this
plainly since it cost real debugging time before the cause was clear; not something this stone
changed.

**Also noted, not investigated further (pre-existing, not touched by this work):**
`arrival-band.fk` scores 895, not the 1023 recorded as its baseline in earlier receipts today —
reproduces identically against the original, unmodified `core.fk`, so it predates this stone.
Out of scope here; flagged so it isn't silently carried forward as "unchanged" again.

## Not shipped: `json.fk` — investigated deeply, a third real bug found, not fixed

`json.fk`'s parser and emitter are each a small ring of exactly root-cause #2's shape
(`json-parse-value` <-> `json-parse-array`/`json-parse-object` <-> `json-parse-array-elements`/
`json-parse-object-pairs`; `json-emit` <-> `json-emit-pair-list`/`json-emit-array-items` <->
`json-emit-pair-node`). Applying the exact same self-recursive-`mode`-function merge that fixed
`cell-serialize.fk` — same technique, written out in full, paren-balance verified line by line —
surfaced a **third, distinct, genuinely reproducible evaluator bug**, not a mistake in the
rewrite:

**A self-recursive call anywhere in a nested `if`-chain corrupts sibling branches that never
take it.** Minimal repro, isolated by bisection down from the full rewrite:

```
; Version A — an LBRACE/LBRACK branch recurses into itself:
(defn jp (s i source-path)
    (do (let tok (json-next-token s i)) (let kind (json-tok-kind tok))
    (if (str_eq kind "NUMBER") (json-mk-pair-r (json-number-node (json-tok-val tok)) ...)
    (if (str_eq kind "LBRACE") (jp 3 ...)          ; <- self-recursive branch, present but NOT taken
        ...))))
(jp "1" 0 "test")   ; -> node_value 0, WRONG (should be 1) — NUMBER branch never touches LBRACE

; Version B — identical, LBRACE/LBRACK branches deleted entirely:
(jp "1" 0 "test")   ; -> node_value 1, correct
```

Confirmed via direct call to `json-number-node("1")` alone (correct, `1`) and to the NUMBER
branch in isolation with no sibling recursive branches present (correct) — the only variable
between "correct" and "wrong" is whether an *unrelated, unreached* sibling branch of the same
`if`-chain happens to call itself. This is very likely a defect in `fk_walk`'s handling of
self-recursion / tail-call structure at the C level (`runtime/fkwu-uni.c`), not something fixable
by rearranging Form code — every Form-level workaround tried (renaming, reordering, isolating
into a helper) either sidesteps the recursive branch entirely (not a real parser) or reproduces
the bug once the recursive branch is restored.

Given this is a new, C-level, load-bearing evaluator question — not a quick fix, and risky to
attempt without the same careful `.tbl`-baseline-diff discipline the C-seed-shrink work used all
day — `json.fk` is left at its original committed state, not the in-progress rewrite. Shipping a
"differently broken" `json.fk` under time pressure was judged worse than an honest, clearly-named
gap. `cell-serialize.fk` remains fully independent of `json.fk` by design (see its own header
comment) and is unaffected.

## Still open

- `json.fk`'s `--src` breakage: root causes #1 and #2 (both fixed in `core.fk`/
  `cell-serialize.fk` already) apply to it directly and the fix pattern is proven; root cause #3
  (this receipt) blocks completing it and needs its own dedicated C-level investigation.
- `cell-serialize.fk` task #12 (still pending): `null` decodes as int `0`, not a distinct value;
  no string-escape handling in the reader. Both named directly in the file, not silently assumed
  correct.
- `arrival-band.fk`'s 895-vs-1023 discrepancy (pre-existing, not investigated here).
