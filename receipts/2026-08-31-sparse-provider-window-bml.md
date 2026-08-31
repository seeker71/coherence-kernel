# Sparse provider-window evidence is BML

The completed-turn collector was paying for every JSONL row when the token
comparison needs only provider usage.  Its 400 kB generic slice exceeded 30
seconds and retained many per-line Form frames.  That is bookkeeping in the
wrong shape, not a reason to widen a timeout.

`form-cli-remote-token-evidence.bml` now carries a scannerless provider-window
grammar.  It seeks only `last_token_usage` frames, verifies that each frame is
a `token_count` or `provider_usage` event on its own line, and sums only its
per-call `total_tokens`.  It rejects a marker in a function-output line and
does not read cumulative `total_token_usage`.  A zero-call window is observed
`0`, not `nothing`; absent binding/cursor stays withheld at the effect door.

The small `observe/form-cli-provider-window-run.fk` door takes authority only
from the already-bound rollout and existing durable cursor.  It returns a
redacted BML frame: cursor, byte width, call count, and token total.  Prompt,
answer, reasoning, path, and raw event bytes stay inside the body.

On the currently bound private rollout, cached execution observed:

```text
provider-window=observed
cursor=3893813
bytes=400000
provider-calls=12
remote-tokens=2324985
```

The warm whole call was 4.0 seconds (`@form fkwu 0 96 0 96`), versus the old
generic 400 kB collector exceeding 30 seconds.  This is a real reduction in
work and retained structure, but four seconds is still not the desired hot
resident cadence.  The frame is a bounded observation, not a settled task
receipt and not a 10% claim.

Witnesses:

- BML authority compiles to `0` with its local `.bml.fkb` cache.
- `form-cli-remote-token-evidence-band.fk` -> `8388607`.
- `form-cli-turn-evidence-live-band.fk` -> `33790`.
- `form-cli-turn-evidence-cursor-band.fk` -> `255`.
- The provider-window entry preflight reports zero errors and unresolved calls.

The next stone is not another transcript scanner: provider fallback must emit
this compact scalar receipt at the crossing itself, keyed to the same
canonical Form task node as the direct local receipt.  That removes legacy
rollout parsing from the comparison path and makes the final ratio an exact
BML calculation: local provider tokens `0` divided by a settled provider
baseline.
