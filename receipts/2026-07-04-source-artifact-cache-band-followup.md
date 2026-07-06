# 2026-07-04 -- Source artifact cache band follow-up

## Ground

This follow-up hardens Layer 8's source artifact cache policy after the
2026-07-03 receipt named one remaining band gap:

- `form/form-stdlib/source-artifact-cache.fk`
- `form/form-stdlib/tests/source-artifact-cache-band.fk`
- `receipts/2026-07-03-source-artifact-cache-layer-review.md`
- `receipts/2026-07-03-core-layer-architecture-map.md`

The layer remains synthetic route policy. It does not read disk artifacts, run
program-image `.fkb`, load `.dylib`, or grow the temporary C seed.

## Pre-Review

Two independent read-only reviewers checked the planned patch before edits.
The available tool did not expose literal Grok or Claude endpoints, so the
reviews were run as two independent agents with those review stances.

Noether, the Claude-lineage reviewer, conditionally accepted the patch:

- fix the band prelude from repo-root paths to `form/`-relative paths
- add `; Expected: 2097151`
- add an otherwise fresh/ok/proven/callable state with `sig-ok = 0`
- assert that state routes to source compile
- keep real disk selection, program-image loading, native calls, and real
  signature metadata deferred

Carver, the adversarial Grok-style reviewer, reached the same verdict and added
one receipt requirement: do not leave stale `1048575` witness text or
negative-signature-deferred language behind.

## Implementation

`source-artifact-cache-band.fk` now declares harness-correct preludes:

```text
form-stdlib/core.fk form-stdlib/source-artifact-cache.fk
```

It adds bit `1048576`:

```text
policy: sig-ok=0 invalidates otherwise fresh artifacts
```

The fixture is intentionally narrow:

```text
(sac-state 100 101 102 1 1 1 1 1 0)
```

Every artifact field is otherwise fresh, ok, callable, proven, and lowerable;
only `sig-ok` is false. The public route contract must therefore return
`sac-run-source-compile`.

## Verification

Before the patch, the harness failed before execution because the band prelude
resolved to missing paths from inside `form/`:

```text
form/form-stdlib/core.fk
form/form-stdlib/source-artifact-cache.fk
```

After the patch:

```sh
cd form
./validate.sh form-stdlib/tests/source-artifact-cache-band.fk
```

```text
core.fk+core.fk+source-artifact-cache.fk+source-artifact-cache-band.fk  -> 2097151
1 ok, 0 divergent
```

No OOM, killed process, stall, or kernel divergence occurred.

## Achieved

- The band is now admitted through `validate.sh`, not only through an ad-hoc
  direct `./fkwu --src <(cat ...)` bundle.
- The policy now proves that `sig-ok = 0` invalidates otherwise mtime-fresh
  `.fkb` and `.dylib` artifacts.
- The layer receipt was updated from `1048575` to `2097151` and no longer
  lists the negative signature witness as deferred.

## Deferred

- Real disk-backed selector: still deferred because this layer is only route
  policy over synthetic rows.
- Program-image `.fkb` load/walk/parse-skip: still deferred because no runtime
  loader is installed by this patch.
- Native `.dylib` load/call: still deferred because this patch only models
  readiness policy.
- Strong artifact identity: still deferred because `sig-ok` remains a policy
  flag, not real hash/signature/version/source-map/proof verification.

## Post-Review

Kant, the Claude-lineage post-reviewer, returned PASS. It verified the current
band preludes, expected value, `1048576` bit, old receipt update, and deferred
items. It also reproduced the validation result:

```text
cd form && ./validate.sh form-stdlib/tests/source-artifact-cache-band.fk -> 2097151
1 ok, 0 divergent
```

Linnaeus, the adversarial Grok-style post-reviewer, also returned PASS. It
verified the same file-level facts and named only the intended residual risks:
this remains synthetic route policy only, and `sig-ok` is still a policy flag
rather than real hash/signature verification.

No blockers remained after post-review.
