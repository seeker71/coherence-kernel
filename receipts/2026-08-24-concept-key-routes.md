# Concept keys reach one bucket

Date: 2026-08-24
Status: **PURE MECHANISM AND ONE BOUNDED LIVE BUCKET OBSERVED**

The persisted public-source registry already holds independently reconstructible
PSCI records with durable source-content and entry-content keys.  Concept lookup
still walked every source artifact to discover which record held a key.  This
movement derived a narrower coordinate without turning the body into a
flattened table:

```text
concept key
  -> SHA-256 key bucket
  -> 0..many bounded one-source references
  -> retained source/entry content keys
  -> original .psci record
  -> reconstructed PSCI source + entry NodeIDs
  -> current source-byte observation
```

Each candidate is its own replaceable file.  Two current candidates remain two
files and produce exact `nothing / ambiguous-concept-key`; directory order
never chooses one.  A missing, malformed, stale, colliding, over-wide, or timed
out reference also produces exact nothing and a refine/undo trace.  Only one
current candidate with zero refused references becomes a hit.

Incremental source upsert writes the new references first, dissolves old-only
keys second, and advances that source's manifest last.  A failed offer releases
the new references it wrote.  Repeating the offer is idempotent.  Explicit
dissolve releases only the source coordinate named by its manifest.

## Observation

- `public-source-concept-key-routes.fk` preflight: balanced, 0 errors,
  0 warnings, 0 unresolved calls.
- `public-source-concept-key-routes-band.fk` preflight: balanced, 0 errors,
  0 warnings, 0 unresolved calls.
- Pure band verdict: **1073741823**, exit **0**.
- The band opens no filesystem, public registry, source, model, Metal, or
  remote lane.
- No new NodeID categories were claimed.  Route hits preserve and reconstruct
  the existing PSCI source/entry identities.

The effectful driver is deliberately bounded to one explicit family/two-hex
source bucket and at most 64 source artifacts per invocation. It was not
preflighted. One explicit `form-stdlib/2e` bucket was then invited:

- source records visited / upserted / failed: **17 / 17 / 0**;
- independently replaceable concept references written: **59**;
- dissolve / undo / refinement / collision: **0 / 0 / 0 / 0**;
- build wall time: **1.57 s**; max RSS: **12,943,360 bytes**;
- immediate read-back: **17 / 17** current manifests and **59 / 59** current
  references, faults **0**, in **0.43 s**; max RSS **23,019,520 bytes**;
- lifecycle: `choice,cut,refine,release` on idempotent upsert and
  `choice,cut,release` on current read-back;
- `model-executed=0` in both observations.

The existing source registry was read but not rebuilt or deleted. The new
route files live beneath its derived `concept-key-routes-v1` child. No model,
Metal, MLX, remote provider, flattening, or operations table participated.

A second deliberately selected bucket, `form-stdlib/6f`, was then born because
it contains the current source for `defn-psci-schema`, the concept requested by
the pending resident knowledge crossing. It upserted **20 / 20** source records
and **73** references with faults **0** in **0.92 s**; immediate report returned
**20 / 20** current manifests and **73 / 73** current references, faults **0**.
This is not a general rebuild: it is the exact route needed by the next cell.

## Honest timing floor

The last registry witness held 5,784 current artifacts out of a 5,858-source
denominator. Each source carries at most six concept keys, so a first route
birth is bounded above by 34,704 reference writes plus 5,784 manifests, while
later source refresh touches at most that source's six references and one
manifest. The first-bucket observation is **92.35 ms/source** and **26.61
ms/reference** including that invocation's compile/load boundary; the selected
second bucket was **46.0 ms/source** and **12.60 ms/reference** warm. A naive
linear extrapolation over 5,784 similarly shaped sources is about **8.9
minutes**, but this is a signal from one 17-source bucket, not a forecast; file
distribution, cache state, and bucket width may change it. Steady-state exact
concept lookup timing remains pending until one routed query is joined to the
resident NodeID knowledge bridge.

Signed in the shared body after the bounded live bucket,

— Codex
