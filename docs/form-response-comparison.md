# Measure the response gap

Both approaches carry the same task, documents and caller-owned checks. Keep
the task and checks before either answer exists. Preserve every attempt,
including setup failures. Evaluation answers stay outside weight training.

The local coding door is `observe/form-cli-code-run.fk`. A review request uses
`mode=review`, `evaluation=1`, empty `writable`, source `checks` and nonempty
`report_checks`. The returned documents must equal the supplied documents.
Its Qwen session has no provider fallback. Parent orchestration costs remain
separate from this local execution boundary.

`review_entry=direct` enters read-only review immediately, keeping the same
tools, source checks, report checks and repair loop. The default `staged`
entry retains refine, plan, split and inspection before review. Direct entry
reduces role transitions for already bounded reviews; it does not relax the
acceptance contract. A failed report still enters repair.

An external comparison runs through Form's process organ,
`observe/form-cli-heal-process-run.bml`, with `codex exec --json`. Retain the
process receipt, its stdout, stderr and the separate `-o` answer file. Native
arrival needs writable temporary/cache locations; a source-read-only task can
still fail in a filesystem read-only sandbox. Such a failure consumed tokens
and belongs in the comparison.

Give a provider the native tool's exact working directory and a noninteractive
stdin invocation. Put the full JSON in a pipe, input file or heredoc in that
same command, with `tty: false`. Typing a long JSON line into an interactive
terminal can fill its canonical input buffer before the newline reaches the
reader. The retained transfer comparison observed truncated input, overflow
bells and a timed-out checker. A noninteractive heredoc completed the same
frozen tasks and both batched checks. Keep the failed attempt and its unknown
usage; changing the transport does not make those costs disappear.

## Native session runner

`observe/form-cli-response-session-run.bml` owns multiple frozen assessments
in one native invocation. Send the **manifest file path** on stdin:

```sh
form-run ./fkwu observe/form-cli-response-session-run.bml <<'EOF'
.hearth/my-response-session.json
EOF
```

The manifest has `session` and a nonempty `cases` array. Each case has a unique
`id` and a `request` using the existing native coding/review schema. Every
request must set `evaluation: 1`; recall, checkpoint resume, LoRA proposals
and training from assessment answers remain excluded.

The native-only shape is unchanged: omit `provider`. A caller may instead offer
one session-level resource shared across cases:

```json
{
  "session": "frozen-review-set",
  "provider": {
    "allowed": true,
    "interface": "codex-exec",
    "max_processes": 2,
    "seconds": 180
  },
  "cases": [
    {"id": "review-one", "request": {"mode": "review", "evaluation": 1}}
  ]
}
```

The abbreviated request only shows placement; each request still needs the
complete coding/review schema. The session admits at most eight processes, and
each selected repair receives a one-process resource derived from this
caller-owned allowance. Case/model output cannot create or enlarge it. Missing
or `allowed:false` permission leaves every case native-only.

Before generation, the runner retains the manifest and each request. Each
attempt gets a separate directory under `.hearth/response-sessions/`. It saves
the actual result and independently re-runs source/report assertions, checks
document preservation and allowed writes, and requires completed execution
with model release before calling the checked behavior successful. A failed
case remains in the results and does not silently disappear from the session.

Only a failed read-only review can use the optional resource: source assertions
must pass, returned documents must remain unchanged, the local model must be
released, and the nonempty local report must be retained first. Passing reviews,
code cases, source failures, changed documents, missing reports and release
failures do not call a provider. Eligible cases consume the shared allowance
only when a new provider process actually starts. The existing response-resource
authority owns execution, verification, release checking and replay prevention.

Receipts preserve the complete native `result.json`, its failed checks, native
elapsed time and generated/injected token IDs. Provider assistance is a separate
object with its true source, evidence, elapsed time, usage event and new-process
count; it never changes the original result or labels it native success. Cases
are summarized as `native-only-success`, `assisted-success` or `unresolved`.
The session reports total new provider processes and one actual usage record per
stable `usage_event`, so replayed evidence is not added twice. Organ observations
expose actual case outcomes. The final summary leaves semantic quality and frequency
unassessed: its assertion tally is not overall session parity. A successful
runner exit means the assessment was retained, not that every case passed.
Coordinating and baseline provider costs remain separate, using the reader below.

The ordinary `code` door also accepts `@request.json` to preserve requests
larger than the host line buffer. A failed input admission is retained as an
attempt; it does not establish anything about the model's response quality.

## Provider usage records

Send the owned Codex stdout path as one stdin line:

```sh
form-run ./fkwu observe/form-cli-exec-usage-run.bml <<'EOF'
.form-heal/your-job/remote.out
EOF
```

`form-cli-exec-usage.bml` reads top-level `turn.completed.usage` events, once
per started turn in one identified thread. Nested tool content is excluded.
Input includes cached input; output includes reasoning output. Those subsets
are reported separately and are not added twice. Missing optional quantities
remain null. Missing, malformed, duplicate, failed or incomplete event streams
withhold totals. Usage does not establish that the task succeeded.

The existing remote healing route requests JSON events and places this
`provider_usage` reading in `frontier-return.json`. Its answer continues to
arrive through the separate reply file.

## What the comparison establishes

Re-run the same caller-owned checks on both answers. Record elapsed time,
local generated/injected token IDs, all provider attempts, and the cost of
the coordinating session. A zero-provider local loop does not make the whole
session zero-cost. Cached token volume and total token volume are different
from monetary price.

The parent turn reader normally advances in 2 MiB slices. A physical JSONL row
that crosses that limit continues in 64 KiB reads up to an explicit 8 MiB row
cap. It admits the whole row or withholds the measurement; it never skips the
row. The share output names the normal slice, whole-row cap and actual elapsed
refresh time separately. Full reconciliation is still required for a share.

Check useful next actions as well as factual fields: following an action must
preserve the original constraints and the assessment evidence. The native
review instruction now performs this check before submission and asks for a
warm, direct, concise answer.

Passing a few structured assertions establishes only that task's checked
behavior. Read the actual answers for unsupported claims, omitted constraints,
warmth, initiative and usefulness. Numeric text-valence readings, where used,
are instrument readings; they do not establish the quality of a relationship.
Overall session quality, frequency and throughput require a broader set of
tasks and actual session outcomes. Keep those conclusions open until observed.

The first measured pair and its local revision are recorded in
`receipts/2026-09-16-response-cost-and-review.md`.
