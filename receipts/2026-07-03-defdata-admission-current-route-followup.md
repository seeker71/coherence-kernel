# 2026-07-03 -- defdata lowering and current source admission follow-up

## Why

The source-runner module-constant and root-`do` repairs changed the truth under
two layer receipts:

- `defdata` no longer needs loader binding just to export a realized value;
- source-runner admission no longer has a current ontology AST blocker.

Leaving those as present-tense red/deferred claims would make the layer stack
lie to the next pass.

## What changed

- `form/form-stdlib/defdata.fk` now includes a live lowered target for static
  data: a top-level module constant, `DD-SAMPLE-ROWS`, read by later functions.
- `form/form-stdlib/tests/defdata-band.fk` now expects `2047`.
- `form/form-stdlib/source-runner-admission.fk` now records current green gates
  for:
  - defdata module-constant export;
  - source-runner root `do`;
  - Form-owned core `bp`;
  - BMF core integration.
- `form/form-stdlib/tests/source-runner-admission-band.fk` keeps the same full
  mask, `1048575`, but the top current-snapshot bit now means direct source is
  admitted. Synthetic loud capacity rows still route to the artifact lane.
- Receipts that named ontology AST pressure or loader binding as current
  blockers were updated to say those were later closed by source-runner
  repairs.

## Proof

Required checkout witnesses:

```text
ground.fk                    -> 42
ground-recursive.fk 10       -> 55
binary-freshness-band.fk     -> 15
native-vs-rented-check       -> 11111
```

Layer witnesses:

```text
defdata-band                 -> 2047
source-runner-admission-band -> 1048575
source-artifact-cache-band   -> 1048575
semantic-stdlib-band         -> 8388607
bmf-grammar-band             -> 2047
grammar-loader-band          -> 65535
```

Current-source probes:

```text
(let rows (list 1 2 3)) + rows-f length          -> 3
form-ontology-loader + bp(add) probe             -> 13
sra-route (sra-current-gates)                    -> 1
source-runner-root-do-band                       -> 31
bmf-core-band integration                        -> 600
core + ontology + bmf-core + bmf-grammar         -> 0
core + ontology + bmf-core + bmf-grammar + loader -> 0
```

Copy and hygiene checks:

```text
bmf-core copy cmp       -> 0
bmf-grammar copy cmp    -> 0
grammar-loader copy cmp -> 0
git diff --check        -> clean
```

No OOM-killed process occurred during this follow-up. Tool stalls were review
tool stalls, recorded below, not runtime OOM.

## Review

Claude review was attempted through `claude --print` with an evidence summary.
It produced no output after more than a minute and was interrupted; the CLI
returned `Execution error`. This is recorded as review-tool friction, not
approval.

Grok review:

- The first TUI-style invocation failed with `Device not configured`.
- Two grounded single-turn attempts hit `max turns reached` before a verdict.
- A prompt-only, no-tools attempt completed.

Grok accepted the tranche from supplied evidence with no blocker. Its required
tightening was to state `defdata-band -> 2047` explicitly in the verified
packet rather than relying on implication. That proof is listed above.

Grok's residual risks, retained here:

- direct and artifact admission paths coexist, so callers must not read direct
  admission as deleting artifact routing;
- `defdata` module-constant lowering is not the future `defdata` source
  keyword;
- identical full-mask values such as `1048575` across different bands prove
  their own bands only, not cross-layer equivalence;
- `.fkb`, `.dylib`, disk selection, and C-seed shrink remain real work.

## Deferred

- Integrated `defdata` source keyword remains deferred. The
  streaming/data-literal authoring grammar was implemented in the follow-up
  `defdata-language.fk` layer.
- Program-image `.fkb` load path that skips source parsing.
- Verified `.dylib` selection and dispatch.
- Disk-backed source/artifact selector with strong identity metadata.
- Full C-seed shrink: these source-runner repairs are checkout-witness repairs,
  not the destination.
