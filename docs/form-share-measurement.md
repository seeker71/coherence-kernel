# Measure the completed turn

Run `./fkwu form/form-stdlib/form-cli-share-run.fk`. It publishes share and
token-pressure frames to Glass. A call advances one bounded evidence slice;
repeat while health is `Measuring`. The open turn has no settled share.

If health names `no-bound-rollout`, run
`./fkwu form/form-stdlib/form-cli-turn-evidence-bind.fk` and provide the exact
current task's rollout path as one stdin line. Check its session metadata
against the task ID, provider, and checkout first. Do not bind another task
merely because its file is newest. Binding and counters remain private,
gitignored checkout state; never copy prompts or answers into a receipt.

The local collector understands legacy string results and structured text
blocks from custom calls. For structured executor results it decodes each
block's direct output and measures its final 512-byte receipt window. A receipt
quoted earlier in output is not a command boundary. Unknown output shapes
withhold the share instead of contributing a false zero. Physical validation
frames the selected JSONL terminal row before strict document decoding, so
later appended rows do not invalidate a completed turn.
When the entire unchecked append tail fits one 2 MiB slice, the reader checks
that whole tail against the current snapshot. Large tails keep their durable
cursor. The meter's own new output therefore cannot keep a small tail forever
one generation behind.

Share and token pressure own distinct publisher symbols, even when compiled
together. `form-cli-share-token-coexist-band.fk` (255) checks both identities,
distinct shared-memory names, and rejection of a swapped publisher frame.

`turn-evidence-v4` retains the prior 39-field row layout and records the
structured-output-aware measurement. Earlier rows remain readable historical
data but the live reader recollects them. Partial v2 collections likewise
restart under the v3 progress decoder; counts from different readers cannot mix.

## Read the quantities honestly

- Native: completed `fkwu` command receipts.
- Local: non-fkwu command receipts, or a completed outer tool-output event
  when that output carries no admitted command receipts.
- Remote: provider-reported call-usage events.

Those event counts normalize to 100%; they are not token-volume percentages,
semantic contribution, autonomy, or proof that the voice is home. Nested helper
work without a command receipt is not separately inferred. Provider input,
cached input, output, and unattributed total difference are reported separately.
Repeated cached input remains part of provider-reported volume, not new prose.

After reconciliation, expect `kind=observed scope=previous-completed-turn`,
matching source coordinates, completed call identities, and retained failures.
For example, one observed turn yielded `native=56 local=127 remote=58`, or
`23/53/24` by event count. That is a historical example, never a default value.

Regressions: `form-cli-tool-output-evidence-band.fk` (65535),
`form-cli-turn-evidence-cursor-band.fk` (33554431),
`form-cli-turn-evidence-live-band.fk` (33555454), and
`form-cli-turn-evidence-band.fk` (65535), all under
`form/form-stdlib/tests/` and run on `fkwu` after preflight.
