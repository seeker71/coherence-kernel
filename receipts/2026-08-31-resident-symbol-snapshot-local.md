# Resident source-symbol snapshot enters the local peer

Signed: Codex, 2026-08-31

## Crossing

The resident peer now has two explicit native knowledge routes in its
high-grammar BML authority:

- `source <concept-or-nodeid>` stays the existing caller-born strict
  current-source action. It is never allowed to fall back to the model.
- `symbol <name>` is a distinct source-of action. At peer admission it builds
  one caller-owned Form symbol snapshot; later tasks query that retained keyed
  map. It returns one birth, choice, or nothing without a Qwen forward.

Task bytes select only the typed lookup coordinate. The root, artifact,
snapshot, and meaning epoch are born before the loop and remain outside task
and model authority. The live peer retains its opaque framebuffer lifecycle
frames; neither task, source, nor answer bytes were added to them.

## Live evidence

`./fkwu form/form-stdlib/form-cli-peer-local-source-answer-live-run.fk`
returned:

```text
route=source-symbol
signal=value
reason=symbol-one
lookup-count=1
admission-snapshot-ms=3960
first-hot-lookup-ms=0
second-hot-lookup-ms=0
second-signal=value
second-callback-calls=0
callback-calls=0
knowledge-contribution=1
mutation-contribution=0
```

The millisecond clock rounds both retained lookups to zero; this is not a
claim of zero work. It is evidence that the once-born snapshot served both
queries without a new model callback. The admission snapshot uses the existing
local `source-of` scanner. It has a bounded local host-exec lease for the
initial source scan and release; this receipt does **not** claim a measured
membrane-crossing count for that implementation.

The explicit strict concept route was also run. It returned
`signal=nothing`, `reason=concept-not-routed`, `lookup-count=1`, and zero
callbacks. That refusal remains present: `symbol` is not its fallback.

## Verification

- BML cache: `bml-cache state=ready bounded=1`
- `form-cli-peer-live-grammar-band.fk` -> `3145727`
- `form-cli-peer-local-source-answer-band.fk` -> `255`
- `form-cli-peer-direct-answer-action-band.fk` -> `1023`
- `form-cli-peer-direct-source-action-band.fk` -> `255`
- `form-cli-peer-agent-band.fk` -> `16383`
- `form-cli-author-high-band.fk` -> `4095`
- Preflight clean: `form-cli-peer-local-source-answer.fk` and
  `observe/form-cli-peer-agent-live.fk`
- `git diff --check` clean.

## Next stone

Give direct local model turns a typed, caller-born current-source observation
when the explicit source action returns a valid PIF receipt. The direct model
turn must receive only that attributed observation, never ambient repository
authority; a strict-route miss, stale receipt, choice, timeout, or `nothing`
must remain local evidence rather than trigger a Qwen guess. Separately, make
the concept-key registry's incremental JIT route available at admission so
`source <concept>` can earn a hit without a whole-tree rebuild.
