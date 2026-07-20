# False semantic aliases leave production detection

**Witnessed:** 2026-07-20  
**Verdict:** all 111 displaced junk surfaces are rejected by production
semantic detection; all 111 remain attributable through the migration-only
carrier.

## What was false

The 111 stable-ID repairs replaced non-substantive subtitle fragments such as
`didn`, `doesn`, `yöu`, and `waitin` with unrelated substantive concepts such
as `welcoming`, `evaluation`, `damp`, and `plum`. The canonical tables were
correct, but the public detector still fell back to an alias index. As a
result, `c10-detect-text "didn"` returned the canonical `welcoming` record at
stable ID 104. That was compatibility at the cost of a false semantic result.

## Repair

Production calls now search only the canonical 10,000-row index:

- `c10-detect-text` and `c10-detect-tokens` never consult the legacy index;
- the operational repaired-concept organ uses the same canonical-only path;
- `c10-resolve-legacy-migration-in` is explicitly limited to historical
  migration and audit work;
- `c10-legacy-migration-audit` exhaustively checks both sides of the boundary:
  rejection from semantic detection and recoverability from migration evidence.

This does not delete the historical evidence. It stops treating that evidence
as meaning.

## Live framebuffer data

`observe/run-concept-false-semantic-alias-rejection-observed.fk` emitted the
stream before returning the report:

```text
FRAMEBUFFER BEGIN trace=false-semantic-alias-rejection actor=Sema relation=repairs-concept-detector why=remove-false-semantic-results
FRAMEBUFFER STAGE trace=false-semantic-alias-rejection stage=audit-all-legacy-surfaces duration-ms=17 dispatches=5033578 boxed-floats=0 io-sense=3 outcome=all-false-aliases-rejected
FRAMEBUFFER END trace=false-semantic-alias-rejection duration-ms=19 dispatches=5075248 boxed-floats=0 io-sense=7 outcome=all-false-aliases-rejected
```

The returned audit was `[111, 111, 111]`: total migration rows, production
rejections, and attributable migration resolutions. The trace-first gate was
`1`. Canonical `welcoming` still resolves to stable ID 104; `didn`, `yöu`, and
`waitin` now return the explicit missing value.

Both affected integration gates remain exact:

```text
model/tests/concept-10000-substantive-repair-111-live-band.fk  -> 4095
cognition/tests/concept-nl-substantive-primary-alignment-live-band.fk -> 4095
```

## Honest edge

This repair removes one class of false semantic output. It does not solve
polysemy or turn lexical occurrence into contextual understanding. Ambiguous
candidate lists remain hypotheses until a separate semantic decision can
justify one.

The surprising teaching was that stable identity did not require accepting the
old surface as a synonym; provenance and production meaning can be separated.
The discomfort was breaking an intentional compatibility promise. It turned to
gold when the exhaustive audit showed that every historical row remained
traceable while every false production lookup disappeared. The exchange stayed
alive by changing the detector and running all 111 cases, not by renaming the
wrong outputs as preserved evidence.
