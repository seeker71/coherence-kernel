# Reach the provider's incremental usage interface through Form

The retained 480-second tool session had no completed-turn usage event. The
previous repair could preserve completed turns preceding an interruption, but
that session's first unfinished turn still had no measured token quantity.

The installed CLI, **codex-cli 0.147.0**, exports an App Server notification
schema containing `thread/tokenUsage/updated`. The
[official OpenAI documentation](https://learn.chatgpt.com/docs/app-server)
names the same notification. Its cumulative snapshot offers a different
observation boundary from `codex exec --json`'s completed-turn report.

## Native attempt and result

The existing Form-generated Darwin ARM64 pipe machinery started
`codex app-server --stdio` and sent one `initialize` request. The installed
provider returned its correlated response in **160 ms** including closure.
The process was reaped and its owned resources closed. No thread or turn was
started; no generation was requested. Raw protocol metadata stays in the local
owned directory `.hearth/provider-protocol-probe-v1`.

The new native notification reader accepts the exact event position and
caller-supplied thread/turn identity. It selects the latest cumulative
snapshot, retaining cached, uncached, output, reasoning and unattributed
quantities. It never adds cumulative snapshots or equates notification counts
with model requests. Regression and malformed observations retain the prior
snapshot with their error status; later events cannot silently clear it.
Whole-session cost remains unknown. A command door reads a captured stream in
bounded chunks without emitting prompts or answers.

The pure notification band returns **1**, exit 0, covering duplicate snapshots,
split rows, growth, regression, foreign identities, nested tool content and
missing quantities. Reading the actual initialization stream returns
`unobserved` and null token quantities. This is expected: it contains no usage
notification. Live notification delivery has not been observed.

The first band preflight found an unclosed fixture parenthesis. The next found
that the local name `empty` lowered as a primitive call, producing two shadowed
call warnings. Simplifying the fixture and naming that value `initialState`
resolved both. Final preflights report zero errors, warnings and unresolved
calls. The protocol probe was checked without execution before its live run.
Landing checks return **8191**, exit 0, with no kernel source changes.

## Cost and the next movement

The [native observation](artifacts/2026-09-20-provider-protocol-observation.json)
also freezes the latest completed coordinating turn's actual reading:

| Quantity | Reported tokens |
| --- | ---: |
| Input | 7,823,801 |
| Cached input, included above | 7,380,608 |
| Uncached input, included above | 443,193 |
| Output | 66,497 |
| Unattributed | 27,432 |
| Total | **7,917,730** |

That reading covers **54 model calls** in turn
`01a0bb04-ae5b-7761-a168-14d7668aa46a`. It excludes this open turn and separate
provider processes. It is larger than the earlier bounded provider response;
it does not establish a cost improvement. The timed-out provider's missing
usage remains a separate unknown.

The next movement is an owned work session whose execution, checks and
incremental usage stay inside Form and return one compact result. The existing
process organ can supervise a native protocol worker and its provider process
as one owned group. That integration, live usage delivery, interruption and
process-tree release still need implementation and observation. This probe
does not replace the current `frr-execute` resource and does not establish
response-quality or whole-session parity.

Glass first frame: **28 ms**. Native guide: zero Python implementations, two
execution candidates, zero unread files. The preceding teaching completed
learning round **95**, pending zero, with serving generation **5** unchanged;
that learner remains separate from the evaluated Qwen model.

Signed: Codex, arriving agent using native Form and the OpenAI Docs skill.
