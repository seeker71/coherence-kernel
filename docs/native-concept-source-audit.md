# Native concept source audit

`form-run ./fkwu observe/concept-source-audit-run.bml </dev/null` checks the
current canonical concept files entirely in Form. It observes 130,000 language
cells, all 10,000 English labels against their stable ranked IDs, 111 repaired
concepts, 111 alias routes, semantic coverage, provenance and eleven exact
manifest files. The 128,557 unaffected cells retain their pinned SHA-256
identity; the 1,443 overlay cells agree byte for byte with the primary surface.

The default hash policy, `form-sha256`, uses the portable Form SHA recipe. An
explicit JSON request selects the Form-emitted ARM64 CPU image:

```json
{"root":".","hash":"arm64-sha256"}
```

Send that object as one stdin line to the same door. Admission requires a
complete object with unique string `root` and `hash` fields; unknown fields or
hash policies refuse. Numeric source fields must contain complete ASCII
integers. Fixed-width labels discard only their trailing ASCII space padding;
leading spaces and trailing tabs remain meaningful bytes.

The ARM64 policy observes the existing guarded structural CPU capability
before admitting raw code. Its context belongs to the current process. An
unknown capability, unsupported observation or stale process identity refuses
before buffer or raw-image admission. The small structural probe remains in
the seed's compiled-leaf cache. Each SHA operation owns its buffer set and
program admission, performs a bulk copy from held immutable bytes, and releases
its resources without retiring another caller's admission of the same program.
This path currently uses the dynamically admitted platform adapter's owned
buffer and CPU-program doors; it does not make Metal a linked root dependency.

Every check reads held bytes. Sixteen distinct input files are read with exact
extent checks and read again before success. This detects an observed change
during the audit; it is not an atomic snapshot of multiple files. Primary
metadata and manifest bindings are content checks, not execution of their Form
definitions. Provenance markers remain source claims: `G` cells are unreviewed.

`form-run ./fkwu observe/concept-source-audit-witness.bml` exercises the real
door through retained child executions. Eleven grouped checks cover exact
portable/CPU SHA agreement through 2 MiB, padding boundaries, UTF-8 and binary
bytes, independent image survival, capability refusal, typed request refusal,
and missing/corrupted/restored private source copies. A refusal emits an
observation, correlated response and applied action before exiting nonzero.
Restoration is followed by a fresh successful full audit. The witness prints
the evidence directory and returns `11` with exit zero.

The current successful full audit emits 32 observations with portable hashing
or 33 with ARM64 hashing, including the CPU resource-count observation.
Independent review also executes malformed numeric and significant-whitespace
cases through the actual door. The audit never edits its input files.

Concept builders and materializers in JavaScript remain active migration
targets. Native verification establishes current source agreement; it does not
establish Form ownership of those builders. The north star is the same held-byte
identity and observed publication contract through Form-owned construction.
