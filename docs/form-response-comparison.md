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

Ordinary conversation uses a different interface: the REPL's `generate` command
calls `fcmg-generate-resident`. It carries Form's teaching overlay and native
knowledge-query capability, without the coding-review bootstrap. Compare these
interfaces explicitly; `review_entry=direct` still uses the review controller.
Preserve the same enquiry and source material, and record differences in
prompt profile, resource allowances and output protocol.

For ordinary generation, the requested token allowance applies to the direct
reply or query phase. A lookup opens a separate answer allowance at least as
large as that request, with the existing default reserve as its floor. The
runtime supplies those actual allowances in the model prompt. Unused query
fuel remains separate; it cannot consume the answer reserve. The specialized
knowledge-query callers retain their explicit budgets. Native admission uses
the same BML scratch-width authority and sliced prefill as model sessions.

Retain the complete generation report: it includes lookup/control text,
generated and injected token counts, phase allowances and stop state. Releasing
the model successfully does not establish that its answer finished. The
[ordinary-generation receipt](../receipts/2026-09-17-dialogue-revision-and-generation.md)
records a real reply cut off by the old 32-token post-lookup reserve.

### Read a declared JSON response

`bml/form-cli-generation-json.bml` provides `fcgj-generate(modelPath,prompt,limit)`
for an ordinary one-shot generation whose requested response is JSON. It uses
the same local generator and releases its model, then decodes the report's
response boundary. `fcgj-generation(report,modelReleaseOk)` reads a retained
report using the separately observed model-release result.

The result keeps `fcgj-body`, `fcgj-prefix`, `fcgj-suffix` and `fcgj-raw`.
On success, prefix + body + suffix reconstruct the raw model output exactly.
The body is the original JSON byte slice, not a rewritten or repaired answer.
`fcgj-report` retains the complete input generation report for diagnostics and
usage accounting, including on refusal.

The reader requires completed generation, stream/model release, no recorded
decode timeout or frame/injection refusal, and a unique observed query count.
It accepts complete leading knowledge-query frames matching that count, an
optional CHOICE marker, one JSON value, and an optional final STOP marker.
The tags come from the existing Form authorities. It refuses unfinished JSON,
unexplained prose, Markdown fences, extra values and other control actions.
Marker text inside JSON strings stays data; JSON `0`, `false` and `null` stay
distinct valid values. The report is trusted local execution evidence, not an
attestation for an arbitrary supplied string.

Use `fcgj-ok` and `fcgj-reason` to inspect the result, then run the original
report checks on `fcgj-body`. Keep raw-format and decoded-format results
separate when comparing interfaces. Decoding proves transport handling; it
does not establish grounding, useful completion or response quality.

### Preserve source-selection authority

Ordinary generation treats paths mentioned in a prompt as hints. A unique
eligible path can supply the source for an unqualified knowledge query;
repeated mentions of that same path remain one hint. Multiple distinct paths
provide no inferred hint. An explicit path in the generated query takes
precedence over a prompt hint. The separately source-bound APIs retain their
caller-supplied binding. `bml/form-cli-source-affinity.bml` carries this
distinction through both fresh and resident generation.

Retained-query replay verifies source selection without another model run.
That establishes the lookup behavior; any effect on a later generated answer
still needs observation.

### Local review evidence

The 2026-09-17 retained-answer review used the same caller material through
two profiles. Ordinary generation copied the input after a STORE marker and
exhausted 1,536 tokens. Compact chat produced a complete 828-token review and
revised report. It corrected the speaker and removed a placeholder, while
retaining an unsupported interpretation of human silence as budget expiry.
The original field checks passed on that revised report. They do not cover
this conceptual error. The intervention changed the whole prompt profile;
it does not isolate a particular instruction or establish general superiority.
See `receipts/2026-09-17-native-json-boundary-and-review.md` for costs and scope.

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

## One admitted model for independent assessments

`observe/form-cli-response-resident-run.bml` accepts a manifest file path on
stdin, like the ordinary session runner. Every case is a read-only evaluation;
all cases use the same model and context capacity, positive reply allowances,
and the non-thinking generation channel. A provider allowance is refused.

```sh
form-run ./fkwu observe/form-cli-response-resident-run.bml <<'EOF'
.hearth/my-response-session.json
EOF
```

The first case admits the model. Later cases replace the conversation stream
while retaining those weights; their prompts contain their own task and source
documents. The existing native controller performs each review and repair with
the original caller checks. Evaluation answers do not enter training or recall.

Each case retains its report and stream boundary with shared release pending.
Only after the final owner is released does the runner publish final results
and recheck every case. `release_ok` in those results refers to the shared
lifecycle: all stream transitions succeeded and the final owner released.
The summary exposes those two observations separately. A failed transition or
release preserves reports with an incomplete lifecycle; it cannot become a
successful case. Intermediate boundaries are evidence of progress, not completed
release. An interrupted run keeps its pending evidence for inspection.

The public Form API is `frsr-run(manifest)` in
`bml/form-cli-response-resident.bml`. Its result remains an assessment receipt:
successful runner execution does not establish answer quality or session parity.
The ordinary session runner continues to own separate model admissions and its
optional provider resource.

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
