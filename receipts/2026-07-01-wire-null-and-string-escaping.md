# Receipt — the last named gap closed: WIRE-NULL + real JSON string escaping (2026-07-01)

The pending item carried since [`2026-07-01-wire-serialization-lane-generic-dialects.md`](2026-07-01-wire-serialization-lane-generic-dialects.md):
"`cell-serialize.fk` task #12: `null` decodes as int `0`... no string-escape handling in the
reader." Root-caused and closed — for all three dialects, not just JSON, since the same gap
was structurally present in each.

## Root cause: no native op produces a "null" trivial value

Verified directly, not assumed: `(node_type (intern_trivial_int 1))` -> `1`,
`intern_trivial_string` -> `2`, `intern_trivial_bool` -> `3`, `intern_trivial_float` -> `7`.
Nothing in this kernel produces `4` or `6`. Every dialect's leaf-emit code carried a dead
`(if (eq ty 4) "null" ...)` branch, inherited from `json.fk`'s design, that NOTHING could ever
reach — so every reader fell back to decoding a literal `null` as `(intern_trivial_int 0)`,
losing the distinction entirely (`eq(null, 0)` was true, which is wrong).

**Fix:** [`wire-registry.fk`](../form/form-stdlib/wire-registry.fk) gained a universal,
dialect-independent null: `WIRE-NULL` (a well-known Blueprint, not a caller-registered one —
null isn't document-specific the way "OUTER"/"INNER" are) plus `wr-make-null`/`wr-null?`. Real,
content-addressed, distinct identity via axiom-3 (same Blueprint, no children -> same NodeID,
always), the same mechanism every other composite already uses — no fake trivial value needed.
Wired into all three dialects' emit (checked before the leaf/composite dispatch) and parse (the
literal-`null`/self-closing-`<n/>`/tag-3 branches now construct `wr-make-null` instead of
`intern_trivial_int 0`). The dead `ty=4`/`ty=6` branches are removed, not left as unreachable
dead code, with a comment explaining why.

## Root cause: the JSON writer never escaped, the reader never unescaped

Two separate, connected bugs in `cell-serialize.fk` specifically (XML's `wx-escape`/`wx-unescape`
were already correct from when it was first built): `cser-emit-leaf`'s string branch wrote
`node_value` directly with no escaping — a string containing `"` or `\` produced literally
invalid JSON, not just JSON this reader couldn't parse back. And `cser-scan-str-end` found the
next `"` byte with no regard for an escaped `\"` before it, so it terminated early on exactly
the input that needed escaping the most.

**Fix:** added `cser-escape`/`cser-unescape` to `cell-serialize.fk`. Escaping covers what JSON
mandates (`"`, `\`) plus the standard short forms (`\b\t\n\f\r`) plus `\u00XX` for other control
chars. Unescaping is the FULL mirror, including `\uXXXX` with surrogate-pair support — reusing
`json.fk`'s own codepoint-to-UTF8 helpers verbatim (those are plain, non-cyclic pure functions,
not part of `json.fk`'s broken mutual-recursion parser/emitter, so reusing them carries none of
that file's actual bug). `cser-scan-str-end` now skips a backslash-escaped byte before checking
for the closing quote.

## Proof

```
cell-serialize-band.fk    -> 1023  (was 63; +4 new checks: null round-trip through literal
                                     "null", null identity, an escaped-string identity round-trip
                                     covering quote/newline/backslash, a composite carrying both)
wire-xml-band.fk          -> 63    (was 15; +2 new checks: null through <n/>, null identity)
wire-corba-cdr-band.fk    -> 63    (was 15; +2 new checks: null through tag 3, a composite
                                     carrying null alongside a real value)

Full regression, fresh build, clean /tmp/come-in-band-dir:
  ground.fk 42, native-vs-rented 11111, core-band 255, core-str-shim-band 15,
  core-str-narrow-waist-band 255, core-str-find-to-int-band 255, core-float-to-str-band 63,
  reception-consent-band 255, relationship-store-band 31, come-in-band 31
  proof/four-way-run.tbl / flatten/form-eval-cli-loop.tbl — byte-identical (Form-only change,
  no C/optable touched)
```

## Not attempted here (real, named, not silently assumed complete)

- Trivial-bool (`node_type` 3) leaf emission was found to be a SEPARATE, pre-existing, unscoped
  gap while checking this one (`node_value` of a trivial bool doesn't behave the way
  `cser-emit-leaf` would need to treat it as JSON `true`/`false`) — not touched, since it wasn't
  part of what was asked and fixing it well needs its own investigation, not a rushed add-on.
- `json.fk` itself remains unfixed (root cause #3 from `2026-07-01-json-fk-src-scoping-fix.md`).
- CDR's IEEE754 `double`, GIOP framing, IIOP transport — unchanged scope limits from the
  previous receipt.
