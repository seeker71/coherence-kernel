# Observe provider usage during a task

The optional Codex App Server protocol exposes `thread/tokenUsage/updated`
notifications with a thread ID, turn ID, latest usage and cumulative usage.
The [official protocol documentation](https://learn.chatgpt.com/docs/app-server)
names that notification. The installed CLI can export its precise schema:

```sh
form-run codex app-server generate-json-schema --out <owned-schema-directory>
```

This is a provider-interface investigation. The existing response resource
still calls `codex exec --json`; it does not yet consume App Server events.
Native local generation continues to run on the C-bootstrap.

## Native reading

`form/form-stdlib/bml/form-cli-provider-notification.bml` reads the exact
notification position. Start with `fpn-empty(thread, turn)`, offer complete JSONL
rows through `fpn-line`, and read `fpn-report`. `fpn-lines` preserves an unfinished
row between chunks. `fpn-read` reads a bounded prefix of a captured event file.
The command door takes three stdin lines: owned thread ID, owned turn ID and
captured App Server stdout path.

```sh
form-run ./fkwu observe/form-cli-provider-usage-run.bml
```

The reader selects the latest validated cumulative snapshot; it never adds
snapshots together. Another thread or turn, or a usage-shaped value nested in
tool output, cannot update that state. A regression or malformed matching
snapshot retains the preceding observation and its new error status; later
events do not silently restore validity. Missing quantities remain null.

The reported scope is cumulative thread usage observed during the selected
turn. It may include earlier turns of that thread. `whole_session_tokens` and
`model_calls` remain null: notification count is not request count, and a
snapshot cannot account for unfinished or unreported work. The report includes
cached, uncached, reasoning and unattributed quantities separately. A caller
must retain the source and lifecycle evidence before using it in a comparison.

## Native transport observation

`observe/form-cli-provider-protocol-probe.bml` takes one fresh owned directory
on stdin. On Darwin ARM64 it uses the existing Form-generated pipe machinery
to start `codex app-server --stdio`, send one `initialize` request and await its
correlated response. It then closes and reaps its owned process and pipes,
retaining the request, stdout, stderr and compact result in that directory.
This probe sends neither `thread/start` nor `turn/start`.

The observed initialization response establishes that this installed interface
is reachable through native Form. It does not establish live usage delivery,
generation quality, task completion, or full process-tree supervision for a
tool-using provider. Those remain required observations before this transport
can replace the current owned response resource.
