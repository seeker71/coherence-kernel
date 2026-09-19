# Measure the response gap

Both approaches carry the same task, documents and caller-owned checks. Keep
the task and checks before either answer exists. Preserve every attempt,
including setup failures. Evaluation answers stay outside weight training.

The local coding door is `observe/form-cli-code-run.fk`. A review request uses
`mode=review`, `evaluation=1`, empty `writable`, source `checks` and nonempty
`report_checks`. The returned documents must equal the supplied documents.
Its Qwen session has no provider fallback. Parent orchestration costs remain
separate from this local execution boundary.

Caller report assertions can expose a concrete unfinished-draft condition to
the existing native repair loop. For an application where square brackets
denote unfilled message slots, an additional assertion is:

```json
{"tool":"jq","arguments":[".follow_up | contains(\"[\")"],"stdout":"false\n"}
```

Keep the original assertions alongside it. This detects that delimiter in the
selected field; it does not certify every aspect of a complete, grounded draft.
In the [draft-repair comparison](../receipts/2026-09-17-native-draft-check-and-source-review.md),
the local model repaired a failing placeholder after one native observation,
preserving every other report field. A separate indexed native review removed
an unsupported reason for another person's silence. Remaining wording and
deadline-scope concerns stayed visible, with the native and provider costs
recorded separately.

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

`bml/form-cli-review-enquiry.bml` exposes
`fcrq-prompt(goal,documents,reportText,fields)` for an explicit indexed review.
It returns the existing carrier result shape: success, prompt text and reason.
An invalid or ambiguous report, or invalid editable fields, returns the
original packet failure. The prompt carries the caller's source documents,
original enquiry and report, plus the exact `fcre-packet` base and spans.
Expected verdicts and report-check answers are not part of this interface.

Generate an answer through the native model session, retain its completion
and release evidence, then pass its decoded edit object to
`fcre-apply(reportText,fields,edits)`. That carrier checks complete ordered
coverage and the original byte identity, composes the selected text fields,
and preserves other values. Re-run the original caller checks and read the
actual reasons and revised report. This is an explicit review interface;
its structural guarantees do not establish the model's judgment quality.

The first live use, recorded in
`receipts/2026-09-17-native-span-review.md`, repaired two unsupported claims
in a new mixed-evidence probe while misreading one supported span. It then
kept every span of the retained failed dialogue, including its unsupported
consequence and unprovided commitment. Both edit sets applied successfully.
The interface remains an explicit diagnostic; that result does not support
automatically reviewing ordinary answers or claiming a semantic repair.

The later source-selection comparison preserves that same enquiry and caller
checks. A native repair restores an exact source quotation after Markdown
reflow, but composing from selected excerpts introduces a new confusion between
neighboring source values. Supplying the complete selected JSON record corrects
that particular error while leaving the draft incomplete. Smaller composition
prompts do not establish a faster whole response once source-selection time is
included. Source selection remains experimental; exact quotation membership,
context coverage and response quality have separate evidence in
`receipts/2026-09-17-native-source-selection-and-quote-repair.md`.

The 2026-09-17 retained-answer review used the same caller material through
two profiles. Ordinary generation copied the input after a STORE marker and
exhausted 1,536 tokens. Compact chat produced a complete 828-token review and
revised report. It corrected the speaker and removed a placeholder, while
retaining an unsupported interpretation of human silence as budget expiry.
The original field checks passed on that revised report. They do not cover
this conceptual error. The intervention changed the whole prompt profile;
it does not isolate a particular instruction or establish general superiority.
See `receipts/2026-09-17-native-json-boundary-and-review.md` for costs and scope.

The `knowledge-query-reasoning` profile leaves the model's thought channel
open. Its scaffold is shared by the live tokenizer cursor and the indexed or
reference fallback. Session admission preserves the requested profile when
the live cursor is unavailable. The tokenizer-only witness compares actual
token IDs across these routes, including literal marker text in the prompt;
matching input IDs establishes profile consistency, not answer quality.
The session carrier retains generated IDs, so final-channel extraction uses
the actual closing token through `fcrt-answer`, rather than a text search.
The 2026-09-17 plain review with that channel open spent all 4096 generated
tokens before a final-channel boundary, then released cleanly. Its final
answer was empty. It does not support enabling open reasoning by default;
the outcome and full costs remain in
`receipts/2026-09-17-native-reasoning-profile-and-review.md`.

### Reserve a final-response stage

`bml/form-cli-reasoning-budget.bml` provides
`frbg-generate(session,initialTokens,finalTokens)` for a session opened with
`knowledge-query-reasoning`. A complete model final answer returns directly.
If the initial stage yields no complete final answer, the native controller
offers a runtime observation in the same resident state and opens an ordinary
assistant turn with its own final allowance. This is a new controller stage,
not a model-generated reasoning close. The initial generation is retained
unchanged; no text from it is promoted by trimming or rewriting.

Read `frbg-answer`, `frbg-complete`, `frbg-origin` and `frbg-reason`. Origins
distinguish `model-final`, `controller-final-stage` and refusal (`none`).
`frbg-first`, `frbg-final` and `frbg-notice` retain both generation records and
the exact controller observation for private evidence. When the first stage
already completes, the second generation is empty. The caller releases
`frbg-session` through `fcms-release-ok?` and retains that result separately.
An incomplete final response stays available as evidence with complete=0.

The initial fit check reserves both allowances; the transition checks the
actual observation IDs and the remaining final allowance before any injection.
Generation failure does not authorize continuation. Token counters keep
generated IDs separate from observation-prefill IDs; the latter include
committing the pending generated token as well as the new role/observation
IDs. Mechanical word counts are available through native `sh-count-words`;
callers still apply their original field checks and assess the actual answer.
This API establishes bounded stage control, not semantic correctness.

The 2026-09-20 source-review trial completed with the same evidence packet
through this staged route. Its answer retained an unsupported subset finding.
Making the missing relation explicit in native context removed that finding,
while the next answer misstated the triggering event sequence. Replaying the
claimed sequence through the actual reader exposed the discrepancy. The
[comparison receipt](../receipts/2026-09-20-native-review-relation-grounding.md)
keeps the raw answers, changed context, stage observations and cost boundaries.
These observations support targeted development; they leave general response
quality open.

Response decoding uses `bml/form-token-decode-batch.bml`. For repeated token
lookups it walks the source vocabulary once, retains only requested pieces,
and reconstructs their original order. Short sequences keep scalar lookup
when the estimated traversal savings do not justify a batch. IDs must be
integers inside the source vocabulary; zero and one are ordinary values.
The reference byte alphabet and text rendering remain the same. No model
generation, persistent index or additional runtime is involved. Decode-only
timings and live integration evidence are recorded separately from total
session latency and answer quality.

The first retained-review observation uses 512 initial tokens and a separate
1536-token final allowance. It completes with 512 + 605 generated tokens,
98 observation-prefill IDs, one controller transition and release=1. The
112-word revised answer passes the original field checks and removes the
earlier budget-expiry assertion. Its review still misjudges that analogy.
It takes 456153 ms versus the compact review's 372536 ms; this is a bounded
quality/cost observation, not general parity or a default-policy recommendation.
See `receipts/2026-09-17-native-final-response-reserve.md`.

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

For the coordinating session, run:

```sh
form-run ./fkwu observe/form-cli-turn-cost-run.bml
```

This completes up to eight pending steps in the existing explicitly bound
turn collector, checking a five-second attention allowance between steps.
A single step retains the collector's own bounded reads and can cross that
time allowance. The result names the selected completed turn, its model,
timestamps, call count and provider-reported input, cached input, uncached
input, output, reasoning output and total tokens. Input includes cached input;
output includes reasoning. An unfinished or unreconciled reading returns null
quantities and the collector's reason. It reports neither the current open
turn nor separately launched provider processes. Repeated readings of the
same turn are one usage record, not additional spend. The first live record
and its substantial coordination cost are in
`receipts/2026-09-17-native-span-review.md`.

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

The fresh direct comparison in
`receipts/2026-09-17-native-direct-transfer.md` completes two original tasks
with no provider generation and verifies the proposed edit by applying it.
Its dialogue still transfers a technical property into an unsupported human
consequence. Adding a generic evidence-domain instruction did not repair that
error and introduced an unprovided commitment in the follow-up draft. That
candidate instruction was removed from the default review path. A successful
check or a better-sounding instruction does not establish a semantic repair;
retain the actual failed answer before choosing another intervention.

Review improvements also need a sound control. The retained-pair experiment in
`receipts/2026-09-17-native-intent-review-boundaries.md` separates source support
from intent preservation, but both reviews keep the original draft, including
the missed concession. Asking instead for a concrete recipient counterexample
creates an unsupported objection to the sound control and still misses the
concession in the other draft. Neither prompt is promoted. Exact quote matching
proves where words came from; it does not prove the permission or consequence
a model attributes to those words. Keep completion, format recovery, edit
application, source support and actual usefulness as separate observations.

Passing a few structured assertions establishes only that task's checked
behavior. Read the actual answers for unsupported claims, omitted constraints,
warmth, initiative and usefulness. Numeric text-valence readings, where used,
are instrument readings; they do not establish the quality of a relationship.
Overall session quality, frequency and throughput require a broader set of
tasks and actual session outcomes. Keep those conclusions open until observed.

The first measured pair and its local revision are recorded in
`receipts/2026-09-16-response-cost-and-review.md`.

## Three routes on one enquiry

`receipts/2026-09-18-direct-vs-guided-form-cli.md` runs the same enquiry
three ways: one process-organ call with an unbounded provider loop inside,
one process-organ call bounded to a single provider turn carrying a native
`enrich` reading, and a guided session where the arriving mind chooses each
native door. It reads rented tokens, crossings and what each answer could
see, and it lands `observe/form-cli-native-call-census-run.fk`, which wraps
one in-process `enrich` call in a runtime membrane window and reports the
planes it crossed. The bounded shape cost 7% of the unbounded one and under
2% of the guided floor; only the guided route produced a measurement. A
provider launched from inside Form inherited the coordinating session's
identity; giving it its own is the named next repair.

`observe/form-cli-grounded-synthesis-run.bml` runs that shape as one door:
native `enrich` plus a dated receipt index, then one provider turn with its
own session identity and no tools. On the same enquiry it spent 13,222 tokens
and cited four receipts by path; the receipt above records the measurement.

The goal that walks this ladder toward the north star, with its pinned
measure, its steps and its exit proof, is `docs/rent-to-zero-goal.form`.
The per-enquiry contest it is walked on, one form-cli call against a guided
flow at the least rent with the fewest calls that leave the body, is
`docs/single-call-parity-goal.form`; its structural floor is
`observe/answer-parity-check-run.fk`.
