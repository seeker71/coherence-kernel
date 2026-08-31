# Remote spend meter becomes caller-bound BML delta state

Authored by Codex on 2026-08-31.

The old `rented-turn-meter-run` summed every textual `output_tokens` field in
the private rollout. That included cumulative `total_token_usage` rows and
produced an impossible total of `25,826,352,066`. The number was refused; it
is not retained as evidence.

`form-cli-movement.bml` now names the only countable quantity: the first
`output_tokens` after each `last_token_usage` marker on a JSONL row. It skips
cumulative totals. Its caller-bound state record retains only:

```text
tag | rollout-path | completed-byte-extent | cumulative-call-local-output-tokens
```

The runner stores that private state locally. On an unchanged rollout it
begins a new scan 32 bytes before the retained extent, preserving the scanner
boundary while counting no old row twice. A changed path or shrink starts a
new baseline; `nothing()` state never becomes a forged zero state.

## Receipt

```text
./fkwu form/form-stdlib/tests/form-cli-rented-turn-meter-band.fk
-> 255

./fkwu form/form-stdlib/tests/form-cli-movement-band.fk
-> 15
```

The new meter band preflights with balanced parentheses, zero errors, zero
warnings, and zero unresolved calls.

The first corrected private baseline read `505,698,903` rollout bytes and
returned `session-output-tokens=6,379,578`. The immediate delta read returned
`session-output-tokens=6,379,998`, with `meter-mode=delta` and
`scanned-from-byte=505,698,871` over a `505,711,872`-byte rollout: 13,001
bytes instead of a second full 505 MB scan.

This is the remote provider's aggregated call-local output spend, not a
completed same-task baseline. The direct local Form answer still has zero
provider callbacks, but the exact goal has no settled provider counterpart;
therefore the requested 10% ratio remains `nothing`, not `0` or a percentage.

I kept the exchange alive by listening to the impossible cumulative number
and changing the meter's grammar instead of accepting it. The surprising
teaching is that the useful remote-cost fact was already in the rollout but
hidden behind the wrong aggregation. The discomfort of a 505 MB reread became
one retained BML cursor and a 13 KB follow-up path.
