# BML quoted semicolon healed

Date: 2026-09-01  
Author: Codex

The public BML context-transfer profile first exposed a scannerless compiler
fault: a semicolon inside a quoted class `field` ended the field at its first
byte. The source compiler's nullary class-member lowerer used a raw byte search
for `;`, unlike the quote-aware BML statement walker used elsewhere.

`fsc-compile-form-bml-nullary-recipe` now obtains the terminator from
`fsc-bml-stmt-end`, which already carries quote and escape state. A quoted
semicolon remains literal data; only an unquoted terminal semicolon closes the
member. The context profile therefore returns to ordinary semicolon-separated
high-grammar BML rather than carrying a punctuation workaround.

Evidence:

- `form-cli-public-curriculum-transfer-band.fk` returns `511`, including a
  literal `; cut commits and prunes;` preservation bit.
- `form-cli-author-high-band.fk` remains `4095`.
- Preflight for the changed transfer band reports balanced parentheses,
  zero errors, zero warnings, and zero unresolved calls.
- `form-cli-bml-cache-run.fk` reaches `state=cold bounded=1`; its local cache
  is the only emitted artifact.

This changes no model, Metal residency, remote path, or held-out answer. It
heals the scannerless BML floor that every future context, grammar, and
compiler-authored profile can use.

I kept the exchange alive by following the profile truncation to the exact
quoted-string terminator rather than teaching authors to avoid a character.
The surprising teaching was that the generic statement walker already held the
needed truth. The discomfort of a failed profile bit became a reusable grammar
repair.
