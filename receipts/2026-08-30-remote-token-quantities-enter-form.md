# Remote token quantities enter Form

The user corrected the metric: local Qwen token IDs are not a measure of
whether the remote Codex mind used fewer tokens.  The relevant quantity is the
remote provider's own per-call token receipt.

`form/form-stdlib/form-cli-turn-evidence-live.fk` previously retained only the
number of `token_count` events.  It now retains both facts without conflating
them: event count remains the boundary trace, while `last_token_usage` supplies
the additive input, cached-input, cache-write-input, output,
reasoning-output, and total quantities.  The cumulative
`total_token_usage` object is intentionally not summed; it describes a rollout
total and would count earlier calls again.

`form/form-stdlib/form-cli-share-run.fk` now renders those remote quantities
alongside the event share.  During an open task it separately exposes only the
most recently completed provider call as `remote-token-pressure`; that value
is not a settled share or a substitute for a completed-turn total.  The event
share remains an event-share only; it does not claim to measure semantic
contribution or token cost.

The high-grammar authority is
`form/form-stdlib/bml/form-cli-remote-token-evidence.bml`.  It names the
separation explicitly: `event-count-is-not-token-volume`, and it runs directly
through the BML-to-native cache path—no `*-xtal.fk` mirror was made.

Witnesses:

- `./fkwu form/form-stdlib/bml/form-cli-remote-token-evidence.bml` → `0`
- `./fkwu form/form-stdlib/tests/form-cli-remote-token-evidence-band.fk` → `1023`
- `./fkwu form/form-stdlib/tests/form-cli-turn-evidence-live-band.fk` → `5118`
- `./fkwu form/form-stdlib/tests/form-cli-turn-evidence-band.fk` → `4095`

The live binding is the explicit private capability
`.form-cli-turn-rollout`, pointed at this Codex task.  Its native discovery
cursor persists across rollout appends, so a large open stream does not rescan
the same tail.  During this still-open turn the collector withheld a settled
row because no terminal completion frame was at the tail; that is the right
`nothing`, not zero.  The active provider-pressure view also withheld because
no completed provider-call usage event occurred in its native tail.  After
this turn completes, the next refresh can admit a real remote token row without
printing prompt, answer, or reasoning bytes.

The next measurement is not a Qwen token ratio.  It is a paired remote-provider
comparison: a task closed with the Form-native/local route versus a comparable
task that falls through, using their observed uncached input plus output and
reasoning totals.  Qwen only earns a reduction when the task closes locally and
the remote row stays absent.

— Codex
