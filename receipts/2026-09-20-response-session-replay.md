# One logical native session can retain and replay its work

Codex, 2026-09-20. Continued from `22a2d133c`. The previous movement improved
native attribution and measured a stronger Form-owned provider answer. This
movement addresses repeated execution and coordinator overhead. Overall quality,
human resonance and minimum whole-session rent remain open.

## Runtime change

The existing response-session controller accepts an optional nonblank `run_id`.
The JSON-encoded pair of session and run identity determines its atomic claim;
labels never become path components. The complete manifest is frozen. Identical
input can replay a terminal result; different bytes under that identity report
a conflict. Unkeyed assessments retain their fresh execution behavior.

Replay checks the terminal seal, exact requested case identities and order,
request/result/report bytes, case receipts, native assertions and referenced
provider request/process/lifecycle evidence. It reports zero new case executions
and provider processes while retaining historical attempts and usage events.
Case execution counts are not model-admission counts. Incomplete claims remain
unresolved; unknown history is null. A saved owner PID is admission evidence,
not a statement that the process is alive. No interrupted request silently
starts again.

The provider eligibility and native coding loop remain unchanged. This is
retained execution with explicit attribution, not a fresh quality evaluation.
The [native coding guide](../docs/form-native-coding.md) documents the entry point
and the byte-identity contract.

## Actual attempt, failure and repair

One provider tool session was admitted through native
`frr-execute-tool-session`, with an isolated checkout under the caller-owned
evidence directory. Its task was bounded to this implementation and native
checks. The [terminal process reading](artifacts/2026-09-20-owned-session-timeout.json)
records **480,217 ms**, deadline **480,000 ms**, process status **124**, reason
`timeout`, and cleanup `released`. It left a partial controller change and
checks. The outer Form driver exited 0 after retaining the failure; that exit
does not make the provider task successful. No second provider was started.

The partial patch had a public return-type mismatch: initial execution returned
JSON text, while replay returned a JSON node. Terminal verification also needed
to bind retained rows to the requested cases. Local review repaired these,
strengthened provider record sealing, represented interrupted history explicitly
as unknown, and kept label identity separate from path syntax. A bare
`json-node-null` in the unfinished test was corrected to the native call.
The original partial patch remains retained in the owned work directory.

The timeout entered a correlated native care exchange: inspect the retained
patch, apply the repair, and re-observe the public boundary. No provider output
was treated as a correct learning target. This attempt does not establish that
delegating a whole work session is cheaper or equally capable.

## Re-observation and its limits

Both response-session bands have clean fresh preflight and return **1**, exit
**0**. The retained fixtures cover terminal reuse, changed request bytes,
interrupted ownership, malformed and modified summaries, missing requested
cases, changed result artifacts, and new-versus-historical accounting. Fixtures
are explicit and isolated under `/tmp`; they are not live model results.

The [public entry-point witness](artifacts/2026-09-20-response-session-replay-witness.json)
executes a request with intentionally invalid context. The actual controller
retains its `positive-context-and-turns-required` failure before model admission.
The repeated request is valid JSON text, returns `terminal-replay`, preserves
the failed outcome and **1 historical case execution**, and adds **0** new case
or provider executions. Changed bytes conflict. Two unkeyed invocations create
distinct roots and execute fresh. This verifies the real admission/replay path;
it is not a successful answer task or a measured saving on live inference.

## Cost and next gap

The provider attempt has **1 started turn, 0 completed turns**. Input, cached
input, output and total token quantities are **unknown**, not zero: its CLI
stream did not emit a completed-turn usage event before termination. Its native
usage event remains `.hearth/work-sessions/replay-v1`, with 646,411 retained
event-stream bytes. Failed work is counted as one timed-out process. Whole-token
comparison is incomplete until interrupted usage can be recovered or retained
at a finer boundary. A future work-session deadline also needs to be visible to
the worker early enough to return a partial report before forced termination.

The [previous completed coordinator turn](artifacts/2026-09-20-session-replay-coordinator-cost.json)
used **39 calls**, **4,736,055 input tokens** (**4,657,152 cached**, **78,903
uncached**), **30,573 output** including **13,279 reasoning**, **0 unattributed**:
**4,766,628 total**. This open turn and the timed-out provider are separate.
Cached tokens are identified; no price equivalence is assumed. Repeated context
and overly broad diagnostic output still add cost around the native work.

Panel: **0 orphans**, **11/12 lanes unobserved**, no standing hearth. Bounded
glass first frame **36 ms**. Native guide: **0 Python implementations, 2
execution candidates, 0 unread files**. The C seed and runtime dependencies did
not grow. The verified replay procedure is returned through native session
learning under `response-session-keyed-replay-2026-09-20`; its worker was
launched. This does not train the evaluated provider output or promote Qwen.
Landing checks returned **8191**, exit **0**. A second invocation of the public
witness returned the retained failure with **0** new case executions again.
