# Retained source-symbol index

The body could already scan every native Form definition and answer one,
choice, or nothing.  That answer was truthful but not resident: each question
walked the 33k-row source list, making a purportedly hot lookup take more than
a second.  The fresh join is a computed `sof-snapshot-v2`: one cold source scan
creates both its inspectable rows and a Form-native keyed map from exact symbol
bytes to all birth paths.  The mapping is not a maintained function table;
both shapes derive from the same local source observation.

At resident birth, `fcpct-live-source-context` owns that snapshot under the
turnwheel meaning epoch.  A `source-symbol` task is selected by the existing
hot policy route and reaches `fcpssa-run-peer` before any model-facing branch.
It preserves one/choice/nothing, returns stale or malformed requests as typed
choices, keeps the input session intact, and reports zero callback, injection,
and mutation contribution.  Its structural answer is durably staged through
the existing turnwheel commit path.

## Observation

On this checkout, the no-model observer built an index with 33,807 rows in
5,861 ms and resolved `fcpct-task-key` from its retained map in 1 ms.  The
cold observation is visible rather than hidden; the hot query no longer
rescans source rows or crosses the host membrane.

The local witnesses are:

- `form/form-stdlib/tests/source-of-band.fk` → `2047`
- `form/form-stdlib/tests/form-cli-peer-source-symbol-action-band.fk` → `255`
- `form/form-stdlib/tests/form-cli-peer-policy-route-band.fk` → `8191`
- `form/form-stdlib/tests/form-cli-peer-contribution-turnwheel-band.fk` →
  `4194303`
- `observe/form-cli-peer-source-symbol-index-live-run.fk` → cold/hot measure
  above, with no Qwen call.

The already-warm resident is not claimed to have acquired this new Form
closure: process images remain honest boundaries.  A successor born from this
source contains the effect hook and snapshot; the old process continues its
existing lease untouched.

Kept alive: I replaced the repeated source walk with a local value whose
invalidation belongs to the same epoch that changes meaning, then made its
route and durable evidence observable.  The surprising teaching is that the
source map did not need a hand-authored table to become fast—its exact bytes
were already enough.  The discomfort was the 1,072 ms “hot” result; measuring
it turned the name into a precise 1 ms map crossing instead of a performance
claim.
