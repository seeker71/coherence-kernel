# The local current-answer program now keeps its type contract in every arm

**Witnessed:** 2026-08-28  
**Signed:** Codex

The model-free current-answer route was executable on `fkwu`, yet two record
contract mistakes kept it from a four-way witness.  Both appeared where a Form
list crosses from scannerless recipe data into a helper written as though every
value were an integer or string.

First, `program-image-symbol-entry.fk` used `pise-list?` to discover whether a
request record was a list by calling `str_len` and integer conversion on it.
The fourth arm carries the request as a Form record/list; Go, Rust, and
TypeScript rightly refused that as a string.  Request and resolution inputs are
already established by their constructors or a scannerless parser, so the
repair names the truthful boundary: a present record may be shape-checked, and
its fields are kind-checked only after their record seat is admitted.  `nothing`
remains refusal, not a list.

Second, the symbol walker compared a structured program input with integer-only
`eq`.  Its receipt input is a list carrying held/current source and strict
lookup observations.  Replacing that comparison with existing polymorphic
`value_eq` preserves equality for both scalar and structured image inputs
without converting or flattening them.

The current-answer band also now resolves its named source witnesses from both
legitimate body roots: repository root and `form/`.  A missing `read_file`
result is zero bytes on this carrier, so the helper distinguishes a nonempty
source file before choosing the alternate root.  The witness now checks the
same source, recipe, and grammar bytes regardless of which proof arm invokes
it.

## Witnesses

- `program-image-symbol-entry-band.fk`: `31457279`, four-way, `1 ok, 0
  divergent`;
- `form-cli-current-answer-in-process-band.fk`: `32767`, four-way, `1 ok, 0
  divergent`;
- direct runtime execution: current-answer returned the strict one-bucket
  source result with one lookup, `model-executed=0`, resident value `1`, and
  `choice,cut,release`; and
- fresh preflight for symbol entry, walker, direct current-answer cell, and
  its band: balanced with zero errors, warnings, and unresolved calls.

This restores the necessary floor for a typed direct-source peer action.  The
next action may keep filesystem root/capability caller-born, execute this
already-admitted current-answer program image, pass Qwen/KV through untouched,
and make only the terminal receipt available to source-world admission.

I kept the exchange alive by treating every cross-arm type refusal as a request
for a more truthful Form boundary.  The surprising teaching was that a
four-way proof did not need a new kind primitive: it needed fewer false
guesses about values whose schema was already known.  Discomfort became gold
when the same change revealed a body-root witness that no longer depends on a
particular shell cwd.

; witnessed: 2026-08-28 -> symbol-entry 31457279 four-way; current-answer 32767 four-way
