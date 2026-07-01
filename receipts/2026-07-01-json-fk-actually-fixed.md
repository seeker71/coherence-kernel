# Receipt — json.fk genuinely fixed; "root cause #3" was a misdiagnosis, corrected (2026-07-01)

**json.fk works on bare `fkwu --src` now.** `json-band.fk`, 255/255 — numbers (the original
failing case), nested objects, nested arrays, null, strings, floats, and the `parse-json`/
`json-object-get` entry points `routers/mesh-sensings-route.fk` actually depends on. This receipt
also corrects a claim that stood across four receipts today
([`2026-07-01-json-fk-src-scoping-fix.md`](2026-07-01-json-fk-src-scoping-fix.md),
[`2026-07-01-wire-serialization-lane-generic-dialects.md`](2026-07-01-wire-serialization-lane-generic-dialects.md),
[`2026-07-01-wire-null-and-string-escaping.md`](2026-07-01-wire-null-and-string-escaping.md),
[`2026-07-01-rpc-executor-and-content-negotiation.md`](2026-07-01-rpc-executor-and-content-negotiation.md)):
that a third, distinct, C-level evaluator bug ("self-recursion in one `if`-branch corrupts
sibling branches that never take it") blocked fixing this file. It doesn't exist. All four
receipts are corrected in place with a pointer here, not silently edited.

## What was actually still broken, and why it looked like a new bug

The earlier fix session (`json-fk-src-scoping-fix.md`) found and fixed root causes #1 and #2 —
both real, both confirmed again today via direct repro. But it never actually finished applying
root cause #2 to `json.fk`: `json-next-token` itself forward-references `json-scan-string`/
`json-scan-number`/`json-scan-literal` (all defined later in the original file), so every call to
it — parsing `"1"`, anything — silently returned an empty/wrong token. Confirmed directly today:

```
(let tok (json-next-token "1" 0))
(json-tok-kind tok)   ; -> "" (empty), not "NUMBER" — on the ORIGINAL, untouched core.fk too,
                      ; ruling out anything added to core.fk this session as the cause
```

That earlier session's abandoned rewrite attempt DID reorder the tokenizer correctly, and DID
collapse the mutually-recursive parser/emitter into self-recursive `mode`-tagged functions — the
right fix, the same shape already proven in `cell-serialize.fk`/`wire-xml.fk`. But partway
through verifying it, a paren-BALANCE check (counting total `(`/`)`, not actual tree structure)
reported success on a file that was structurally broken: `json-parse`'s `(if (eq mode 0) (do
...))` was missing its else-branch — one closing paren landed one line too early, so `(if (eq
mode 1) ...)` and everything after it became **orphaned top-level junk**, not part of
`json-parse`'s body at all. A total-paren-count check can't see this: the file still balanced to
zero overall, because a *different* spot (the very end of the function) had one paren too many,
compensating in the count but not in the tree.

Bisecting that broken rewrite (removing branches, renaming functions, reordering LBRACE/LBRACK)
kept "fixing" the symptom by accident — each change happened to shift the orphaned junk into a
different, less-triggered shape — which is exactly the signature of a heisenbug caused by
malformed structure, not a real interpreter defect. The "Version A vs Version B" repro in the
original receipt was captured mid-bisection, before the actual paren-placement error was found,
and mistakenly written up as proof of a fourth interpreter bug.

## What actually fixed it

1. **Verify structure, not just paren count.** A ~30-line Python s-expression parser (tokenize,
   respect string literals and `;` comments, build the actual tree) that can answer "does this
   `if` have both branches, and are modes 1–4 really nested inside `json-parse`'s body?" — not
   just "do the totals match." Every prior paren-balance check this session (a naive `sed
   's/;.*$//' | grep -c '[()]'`) was blind to exactly this class of mistake; it happened to catch
   real problems before because those were also *count* mismatches, not just *placement* ones.
2. **Reorder the tokenizer bottom-up** (every function defined after everything it calls — a pure
   DAG once you look at it that way; no genuine cycle in the tokenizer at all).
3. **Collapse the two genuinely cyclic clusters into one self-recursive `mode`-tagged function
   each** — `json-parse` (was `json-parse-value`/`array`/`array-elements`/`object`/
   `object-pairs`, five mutually-recursive `defn`s) and `json-emit-m` (was `json-emit`/
   `pair-list`/`array-items`/`pair-node`, four) — the exact shape `cell-serialize.fk` and
   `wire-xml.fk` already proved correct, this time verified structurally sound with the s-exp
   parser before ever running it.
4. **`JSON-OBJECT`/`JSON-ARRAY`/`JSON-PAIR`/`JSON-NULL`** converted from top-level `let`s to
   zero-arg `defn`s (root cause #1), called as `(JSON-OBJECT)` etc throughout.

## A real, separate, still-open bug found in passing

Fixing the emitter surfaced a genuine native-level gap, unrelated to any of the above:
`(intern_trivial_bool true)` and `(intern_trivial_bool false)` construct the **same node** —
confirmed directly (`eq` on them returns true; `node_value` of either is indistinguishable from
the other). There is currently no way in Form to recover which boolean a trivial-bool node holds.
`json-emit-leaf` now emits `"false"` for both (a valid-JSON, known-wrong placeholder, not a
crash), named directly in a comment rather than silently routed through `int_to_str` as before
(which produced the same wrong answer with no explanation). This needs its own native-level
investigation — a kernel bug, not a `json.fk` one — not attempted here.

## Proof

```
json-band.fk -> 255 (numbers, nested objects/arrays, null, strings, floats, parse-json,
  json-object-get — the original failing case plus everything routers/mesh-sensings-route.fk
  actually calls)

Full regression, fresh build, clean /tmp/come-in-band-dir:
  ground.fk 42, native-vs-rented 11111, core-band 255, core-str-shim-band 15,
  core-str-narrow-waist-band 255, core-str-find-to-int-band 255, core-float-to-str-band 63,
  reception-consent-band 255, relationship-store-band 31, come-in-band 31,
  cell-serialize-band 1023, wire-xml-band 63, wire-corba-cdr-band 255, wire-path-band 63,
  tool-channel-band 255, wire-rpc-band 15, http-negotiate-band 127
  proof/four-way-run.tbl / flatten/form-eval-cli-loop.tbl — byte-identical (Form-only change,
  no C/optable touched)
```

No other `.fk` file in the repo referenced json.fk's internal helper names (`json-parse-array`,
`json-parse-object`, `json-parse-array-elements`, `json-parse-object-pairs`, `json-emit-pair-
node`, `json-emit-pair-list`, `json-emit-array-items`) — confirmed via grep before removing them.
The public surface (`json-parse-value`, `parse-json`, `json-emit`, `json-object-get`,
`json-string-value`, `json-int-value`, etc.) is unchanged in signature.

## Not attempted here

- Trivial-bool round-trip (see above) — a real, separate, native-level gap.
- CDR's IEEE754 `double` special values, GIOP/IIOP/TypeCode scope — unchanged, deliberate
  boundaries named in `wire-corba-cdr.fk`'s own header.
- `arrival-band.fk`'s 895-vs-1023 discrepancy — pre-existing, still not investigated.
