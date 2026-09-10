# Native source inspection

Form owns source classification, byte reading, line counts, call-shape detection
and report publication. Git supplies tracked and unignored paths. The structural
gate uses an OS directory traversal with its own complete status and output.
The current transport is POSIX; portable host admission remains part of the
[north star](fkwu-form-native-north-star.md).

`form/form-stdlib/bml/native-source-inventory.bml` starts an argv child with
explicit stdin, stdout and stderr files, waits for its actual status, then checks
both reads against their file extents. The private evidence directory retains
the bytes and `capture.json`. An empty successful inventory is valid. A failed
child is refused even if its partial stdout ends at a valid NUL boundary.
Capture completion and NUL-list admission are separate checks.

`observe/carrier-mass.bml` reads current source in Form. Its extension catalogue
includes C#, device text and common script languages; unknown extensions are
probed for a shebang. Paths containing spaces or newlines remain whole. Only
complete `node_modules` and `seedbank` path components affect weighted exclusion;
every excluded source row remains in `.hearth/carrier-mass-current.json`.
Each source row retains its path, newline count, class, byte extent and exclusion.
Unclassified paths stay visible. Unread paths refuse a mass and retain a
correlated observation, request for evidence and applied response.

The weighted field encodes `seed-lines * 10^9 + shell-lines * 10^3 +
host-exec-cells`. Its named components and full rows are authoritative for
inspection; the packed number alone does not establish ownership. The source
scope includes proof engines and target-language specimens. Lexical call shapes
can occur inside source examples and do not prove execution. This is a current
file reading, not an atomic snapshot of a changing repository.

`observe/native-authoring-guide-run.fk` retains the Python implementation and
invocation-candidate reading at `.hearth/native-authoring-current.json`. Report
publication verifies the written bytes before atomic rename. The public reading
shows unread paths explicitly. The current guide identifies the active
large-model MLA numerical oracle and the real terminal acceptance test as the
two remaining Python implementations.

The structural gate rejects an absent, non-directory or unreadable root. It
preserves names through NUL-delimited traversal and prunes `.git` and `.claude`.
Its seven result fields are total, unclassified, carrier, oracle, fixture,
proof sibling and tooling. Classification states a declared role; it does not
establish that a script contains only mechanical OS operations.

Actual child and filesystem witnesses:

```sh
form-run ./fkwu observe/native-source-inventory-witness.bml
form-run ./fkwu observe/structural-source-reader-witness.bml
form-run ./fkwu observe/tests/carrier-mass-band.fk
form-run ./fkwu gate/tests/structural-gate-band.fk
form-run ./fkwu form/form-stdlib/tests/native-authoring-guide-band.fk
```

The source witness verifies 2,097,152 captured bytes, an empty Git repository,
newline-containing filenames, unknown-extension scripts, failed partial output,
unterminated output, missing execution, observed read recovery and atomic report
publication/refusal. It returns **5**, exit zero. The directory witness observes
six actual boundaries; the carrier, structural and guide bands each return
**16383**, exit zero. A green number with a nonzero exit is never an accepted
observation.
