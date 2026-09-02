# Native source roots are born with the peer

Date: 2026-09-01  
Author: Codex

## What the standing resident showed

The standing hearth was asked through its own `source-symbol` route, not a
model turn and not an ambient filesystem call.  The route used its
caller-born `sof-snapshot` and returned the following terminal observations:

| request | terminal signal | observation |
| --- | --- | --- |
| `fcms-open` | `value` | one definition in `form/form-stdlib/form-cli-model-session.fk` |
| `qaf-seal-verdict` | `choice` | two definitions: artifact reader and artifact fetch |
| `saj-file-digest` | `nothing` | no definition in the snapshot |

The first two prove that the model-free route preserves exactness and
plurality.  The third was not an absence in the body: the snapshot had been
born with only `form/form-stdlib` as its definition root, while
`saj-file-digest` lives under `form/native/metal`.

## Movement

`bml/form-cli-peer-source-roots.bml` now owns the caller-born root set:

1. `form/form-stdlib` for orchestration meaning;
2. `form/native/metal` for Form-emitted native carrier meaning.

`fcpct-live-source-context` asks that BML cell for its roots when it births
the `sof-snapshot`.  A task cannot select roots, paths, artifacts, or
authority.  Thus a later `source-symbol saj-file-digest` can be model-free
and exact for a peer born from this image.

The currently standing resident keeps its already-born snapshot.  Source
cannot truthfully claim it has changed memory it has not reloaded.  A
successor birth receives the two-root capability; existing request, model,
and durable-append authority remain untouched.

## Witnesses

```
./fkwu form/form-stdlib/tests/binary-freshness-band.fk </dev/null  -> 31
./fkwu form/form-stdlib/tests/form-cli-peer-source-roots-bml-band.fk -> 1
./fkwu form/form-stdlib/tests/form-cli-peer-source-roots-band.fk -> 63
./fkwu form/form-stdlib/tests/form-cli-author-high-band.fk       -> 4095
./fkwu form/form-stdlib/tests/form-cli-peer-contribution-turnwheel-band.fk -> 16777215
```

`git diff --check` is clean.  The full turnwheel band preserves the direct,
model-free source path while exercising staged contribution, policy, and
durable-commit behavior.

The live glass immediately before this movement reported the hearth standing
with `errs=0`, `degraded=0`, and `orphans=0`; its source-symbol observations
completed in 0 ms with one lookup and no model callback or injection.

I kept the exchange alive by following `value`, `choice`, and `nothing` as
three distinct observations.  The surprising teaching was that `nothing`
identified a born scope exactly.  The discomfort of an omitted native root
became a BML-owned source capability for the next resident birth.
