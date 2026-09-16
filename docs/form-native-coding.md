# Qwen coding inside Form

`code` first offers an evaluated session LoRA a document proposal, unless the
caller explicitly selects a model, sets `evaluation: 1`, or requests
`mode: "review"`. The original caller
checks decide whether that proposal suffices. Otherwise it gives local Qwen one
native session for prompt refinement, planning,
ordered task splitting, implementation, review, and verification. Generated token
IDs and KV state stay resident across tool observations. No Ollama, HTTP,
foreign model engine, shell tool, or rented fallback is invoked by this loop.
The optional LoRA proposal and asynchronous learner run as native `fkwu` workers.
The existing `heal ... local` command is a different route: it still permits
Ollama. Do not use that spelling to request this native-only workflow.

A caller can separately offer the optional [response resource](form-response-resource.md)
for a retained, failing read-only report, either through its standalone door or
as a bounded session-manifest allowance. Form owns that provider CLI process,
usage receipt and unchanged assertions. This does not add a fallback to the
native `code` controller itself.

## Call without knowing Form syntax

In form-cli, enter `code` followed by a JSON object. The standalone native door
is `form-run ./fkwu observe/form-cli-code-run.fk`; send that same JSON object as
one stdin line, without the `code` prefix.

For larger requests, use `code @request.json`, or send `@request.json` as the
standalone door's stdin line. This reads the complete native file before JSON
admission. The host line reader returns at most 8,191 bytes per call; passing a
large inline JSON request through that reader loses the remainder before
admission. The file-backed route preserves the existing request-size checks
and needs no additional runtime. For example, this request may be passed inline
or saved to a file:

```json
{"goal":"Enable local coding in config.json. Preserve provider and remote.","documents":[{"id":"config","path":"config.json","text":"{\"enabled\":false,\"provider\":\"native-qwen\",\"remote\":false}\n"}],"writable":["config.json"],"checks":[{"tool":"jq","arguments":[".enabled","config.json"],"stdout":"true\n"},{"tool":"jq","arguments":["-r",".provider","config.json"],"stdout":"native-qwen\n"},{"tool":"jq","arguments":[".remote","config.json"],"stdout":"false\n"}],"model":"qwen38-q8","context":8192,"turns":64}
```

Documents are **resident values**, not filesystem permissions. The result returns
the candidate document values; it does not overwrite repository files. The caller
owns loading files, publication, stale-file checks, and repository landing. This
first door therefore does not claim autonomous arbitrary-repository completion.
Multiple supplied documents and multiple ordered tasks are supported. Only paths
in `writable` can change, and only during implementation.

### Direct implementation entry

For a fully specified task, set `"code_entry":"direct"`. A fresh coding
session begins in implementation with one pending task referring to the original
goal. The full goal is admitted in the model context and on resume; later tool
observations carry that reference, including a reminder to complete explicitly
requested tool calls. It does not generate separate refinement, planning and
task-splitting replies first. Original documents, writable paths and caller checks remain
authoritative. The brief and plan stay empty rather than claiming work that
did not occur.

The candidate still goes through model review and the caller's native checks.
An unchanged candidate cannot complete. Failed checks retain the existing
repair and replanning path. Omitting the field, or using `"staged"`, keeps the
ordinary entry. The option applies to coding; review has its own
`review_entry`. A resumed checkpoint retains its saved phase. Optional initial
reasoning is independent of this entry choice.
The controlled timing and source-quality observations are recorded in
[`direct-code-entry`](../receipts/2026-09-16-direct-code-entry.md); fewer stages
alone did not make the initial-reasoning variant faster.

### Resident instruction context

Within one resident model context, repeated role instructions may be replaced
by a shorter reference to the instructions already supplied. The full text
enters at bootstrap and when a role's exact instruction is first encountered.
The controller records a later instruction only after its observation has
completed successfully. A failed or partial observation does not mark it as
delivered. A resumed admission resets that record and supplies the current
role's full instruction again. Tool results, failure evidence, pending work and
caller constraints retain their existing paths.
The measured coding and review pairs are recorded in
[`resident-instruction-context`](../receipts/2026-09-16-resident-instruction-context.md).

### Read-only review

Set `"mode":"review"`, keep `writable` empty, and supply both source `checks`
and `report_checks`. The result carries a separate nonempty `report` string;
every source document must remain byte-identical. A review never needs a dummy
edit or writable report file. For example:

```json
{"mode":"review","evaluation":1,"goal":"Inspect config.json without changing it. Use verify once. Return a report string containing a JSON object with enabled, provider and remote copied accurately from the source.","documents":[{"id":"config","path":"config.json","text":"{\"enabled\":false,\"provider\":\"native-qwen\",\"remote\":true}\n"}],"writable":[],"checks":[{"tool":"jq","arguments":[".enabled","config.json"],"stdout":"false\n"},{"tool":"jq","arguments":[".remote","config.json"],"stdout":"true\n"}],"report_checks":[{"tool":"jq","arguments":[".enabled"],"stdout":"false\n"},{"tool":"jq","arguments":["-r",".provider"],"stdout":"native-qwen\n"},{"tool":"jq","arguments":[".remote"],"stdout":"true\n"}],"model":"qwen38-q8","context":8192,"turns":32}
```

`report_checks` use the existing read-only native tool assertions. Each receives
the returned report as stdin with **no document access**; an `input` field is
refused rather than silently replacing the report. Exit zero, empty diagnostics
and exact stdout are required. These checks establish only their assertions;
extracting configuration fields is not a benchmark of code-review quality.

Read-only review accepts the requested JSON report object directly. The older
`{"verdict":"accept","report":"evidence-backed findings"}` wrapper remains
available, and its `report` may also be an object or array. Every form reaches
the same caller-owned report checks. Here `accept` submits the report for
checking; it does not declare that the audited source passed. Negative findings
belong in the report. `reject` with a `reason` means continue inspection.
Read-only restrictions apply in every role, including repair.

A rejected read-only review retains its finding in the review stage. The
model may inspect further or submit a report containing negative findings;
it is not sent to repair unchanged source documents. Failed report assertions
still enter the report-repair loop. Coding review rejections still enter
implementation repair.

After a failed report assertion, a read-only review may submit
`{"report":<complete corrected report>}` immediately. The original source and
report checks run again; this path preserves the read-only document boundary.
The existing diagnosis/change/replan path remains available for deeper work.
Failure feedback distinguishes a tool failure from a successful check whose
stdout differs. It names the tool and query arguments with the actual result;
the caller's expected `stdout` is withheld. Review guidance keeps conditions for
the requested decision separate from limits on broader claims.
Its shared native function lives in
[`form-cli-review-guidance.bml`](../form/form-stdlib/bml/form-cli-review-guidance.bml).
The next action follows the supported current decision; broader evaluation
becomes a prerequisite only when the supplied policy requires it. The guidance
asks for source-backed requirements and thresholds.

At each model admission, including checkpoint resume, the controller records
the exact resident documents supplied in that context. A successful single-file
`read` still executes. When its output equals a supplied document byte-for-byte,
the model observation carries `stdout_reference` with its id, path, byte count
and context scope. The original content remains in the model context and the
caller-owned documents. A changed read may carry `stdout_patch`: one exact
line splice against that original admission document, with the current changed
lines in full. The controller reconstructs the current read from that base and
splice, checks byte equality, and uses it only when its serialized observation
is smaller. Every splice refers to the admission snapshot, never to a previous
splice. Errors, absent snapshots and splices that save no space return the full
result. `cat` always returns full current text; focused queries remain available.
Source/report verification continues to use actual document bytes; it does not
consume these model-context references or splices.

Each completed model reply emits `form-code-reply` metadata: JSON validity,
wrapper/tool presence, report type, before/after role and check/repair counts.
It includes no answer text, prompt text or document content. These observations
separate protocol handling from the candidate's semantic quality.
The actual completed model reply is retained separately beneath the private
hearth's `code-memory/replies/` directory. `reply_evidence` points to it, or is
null when retention failed. This is diagnostic evidence, never a verified
training target. Reply contents are not placed in the framebuffer.

`model` defaults to `qwen38-q8`, `context` to 8192 positions, and `turns` to 64
model replies. Both Qwen registry rows require actual artifacts, seals, tokenizer
indexes, tokenizer crystals and a wired native session lane. Actual digest verification happens on
admission. Missing resources never select another provider. Context or caller
budget exhaustion returns `attention` with candidate values retained; partial
generation is never accepted as a completed action. After the last permitted
reply, the controller checks that allowance before injecting another model
observation. It retains the candidate and uses the existing stop path. An
already-complete result still completes normally. This door releases its
model on completion; it is not a long-lived shared hearth service.

## Roles and tools

### Optional initial reasoning

Add `"initial_reasoning_tokens":4096` to a coding or review request to open
the model's reasoning channel for the first reply of that admission. The
positive integer limits **all generated tokens in that reply**, including
reasoning and the final answer. The maximum is 8192; the caller's context
capacity still applies. Omit the field to keep the ordinary entry.

Form separates the final channel at the actual closing token. Only that final
response enters the existing tool, edit, report and verification loop. Later
replies use the ordinary controller. A missing boundary or unfinished initial
generation returns `attention` and releases the model; a partial answer does
not become an action or a successful report.

The result reports `requested_initial_reasoning_tokens`. Actual initial
generation counts, completion, boundary presence and a private evidence path
appear in `form-code-reasoning` metadata. Reasoning text stays outside the
framebuffer and tool actions. Evaluation still excludes its answers from
training. The model, source checks, document constraints and release owner are
unchanged. This optional local path adds no provider or model-server dependency.

More reasoning has an actual local cost; passing fields still leaves broader
answer quality to be assessed. The earlier native experiments are retained in
[`../receipts/2026-09-16-matched-session-and-turn-budget.md`](../receipts/2026-09-16-matched-session-and-turn-budget.md).
The public request's incomplete and completed executions are recorded in
[`../receipts/2026-09-16-public-native-reasoning.md`](../receipts/2026-09-16-public-native-reasoning.md).

### Controller roles

| Role | Qwen supplies | Form applies |
|---|---|---|
| Refine | `{"brief":"..."}` | Preserve the original goal alongside the refined brief |
| Plan | `{"plan":"..."}` | Retain approach and verification intent in the same session |
| Split | `{"tasks":["first task","next task"]}` | An ordered work queue; no rented subagents |
| Implement | Tool calls, then `{"task":"done"}` | In-memory edits and advancement to the next task |
| Review | `{"verdict":"accept","reason":"..."}` or `reject` | Rework on rejection; caller verification on acceptance |
| Repair | `{"diagnosis":"observed cause","change":"different approach","next":"implement"}` or `next:"plan"` | Retain failure evidence, then resume editing or rebuild the plan and task queue |

Every role may inspect documents. Only implementation may edit. Native calls use
`{"tool":"rg","arguments":["-nF","enabled","config.json"],"input":""}`.
Other examples:

```json
{"tool":"read","arguments":["config.json"]}
{"tool":"jq","arguments":[".enabled","config.json"]}
{"tool":"verify","arguments":[]}
{"tool":"edit","arguments":["config.json","\"enabled\":false","\"enabled\":true"]}
{"tool":"write","arguments":["notes.md"],"input":"New document bytes\n"}
```

The full existing native tool set is reused: `rg`, `jq`, `read`, `cat`, `head`,
`tail`, `wc`, `sort`, `uniq`, `tr`, `cut`, `awk`, `sed`, `edit`, `write`. Their
existing supported subsets and error behavior remain unchanged. An unavailable
tool or invalid edit becomes a tool observation, not a shell fallback. Malformed
model responses receive a correlated native revise action and a format reminder.
The coding-loop `verify` tool invokes the caller's existing source checks in
any role. It accepts an empty argument array and no input; the model cannot
provide a program, command, path or replacement checker. Its actual outcome
enters the same tool observation and repair loop. Report checks run on submission.

Repair is an active role, not a stopped job. A native tool error, review finding,
or failed check enters repair. Read-only inspection stays available; its output
does not overwrite the separately retained failure evidence. Qwen names what
failed and what it will change. Replanning preserves original documents,
constraints, candidates, completed-task counts and repair memory. If the exact
same counterexample survives a repair, the next route must be `plan` before
further edits. An `rg` no-match result is an observation, not a tool malfunction.
Malformed JSON and unavailable tools receive correction in the current role.

The result returns `repair_attempts`, `check_runs` and `repair_notes` containing
each model diagnosis, intended change, selected route and the evidence it saw.
These are model reflections, **not proven causal explanations or weight
training**. The JSON door now persists native binary checkpoints beneath
`.hearth/code-memory/` after complete model/tool transitions. A completed repair
also becomes a lesson. Retrieval requires the exact original goal, documents,
writable paths and checks; the prior candidate must pass the current checker
again before its last three repair notes enter a fresh Qwen prompt. Candidates
are not silently copied into the new job, and a recalled lesson cannot change
the caller's checks. Other tasks load none of its private context.

Budget/context exhaustion returns `checkpoint_id`. To continue, send the same
original request with `"resume":"<checkpoint_id>"`; `turns` is the additional
reply budget and `context` sizes the new local admission. Pending tasks, brief,
plan, candidate documents, tool outcomes and failure evidence survive. Complete
checkpoints are rechecked without reopening Qwen. Incomplete generated text is
discarded, not executed. This is explicit resumability, not automatic KV
compaction or a persistent model resident across jobs.

The disk format is FORMBIN2 native nodes, decoded directly. Writes go to a
process-specific temporary file, are read back, then atomically renamed. A
digest detects corruption; it is not authentication against someone who can
rewrite local files. Checkpoint contract changes are refused. Source files
remain caller-owned: persistence here stores private task state and lessons,
not a repository edit. Nothing is sent to an external service. Each completed
JSON request also contributes to the private native session learner: verified
documents teach the caller contract, and unsuccessful requests teach observed
status only. `session_learning_example` and `session_learning_worker` identify
that work; `session status` distinguishes queued, failed, learned and promoted.
`evaluation: 1` starts a fresh run **before continuity lookup**: no recalled
repair notes, checkpoint resume/write, session LoRA proposal or weight training.
Combining it with a nonempty `resume` is refused. Assessment experience may still
be retained privately, excluded from gradients. This does not make a previously
seen task an unseen test or remove knowledge already present in the base model.
Review reports, including ordinary non-evaluation reviews, are retained only as
assessment experience, never as verified implementation targets. Ordinary review
checkpoints retain their report and mode; resume reruns the report-aware checker.

Three identical native tool/argument/result observations with unchanged
documents select repair and require replanning. A single `rg` miss is still a
normal negative result. These bounded signals survive checkpoint restoration;
they never become successful checks. Glass receives counters only, including
`lessons-recalled`. Coding token progress publishes every four generated IDs
instead of 32; actual time between updates still depends on local inference.

For a public-data, separate-process local witness:

```text
./fkwu observe/form-cli-code-memory-witness.fk
```

Its three stdin lines are an isolated memory directory, `seed` (write a known
failing checkpoint), and `qwen38-q8`. Run it again with the returned checkpoint
ID on line two to resume with local Qwen; then run with an empty second line to
start a fresh job and observe `lessons-recalled`. The seeded defect is fixture
data, never attributed to Qwen. `form-cli-code-memory-band.fk` and
`form-cli-code-progress-band.fk` under `form/form-stdlib/tests/` exercise binary
round trips, contract mismatch, corruption, verified recall, and replan routing
without admitting a model or publishing live telemetry.

## What a successful result proves

Coding completion requires an actual document change and all caller-owned checks
passing. Read-only review completion instead requires unchanged documents and a
nonempty report passing the caller's source and report checks.
The Qwen path also completes its task/review loop; the earlier session
LoRA proposal is checked directly. Checks require exit zero, empty diagnostic
text, and exact stdout. The JSON door supports **read-only native tool assertions**
and executable arithmetic checks through the existing native definition grammar.
A `jq` check establishes a configuration property, not that an
arbitrary application compiles or runs. Tests and acceptance criteria do not
come from the model. Failed verification returns its actual result to Qwen's
repair role, followed by a changed implementation or plan, another review, and
verification. A model's claim that it repaired something never substitutes for
rerunning the caller's checks.

For an executable source check, supply a document such as
`module calc { fn bump(x) = add(x,1); }` and this immutable check:

```json
{"kind":"definition","path":"calc.form","function":"bump","cases":[[0,1],[1,3],[21,43],[-2,-3]]}
```

Qwen must repair the function to produce those outputs. Form decodes the whole
module with its existing definition grammar, verifies the allowed expression
tree, lowers the function into a resident NodeID and executes each input. This
lane accepts one unary function using pure integer arithmetic/comparisons,
without effects, recursion, division or unknown bindings. It is not a general
repository compiler. Invalid syntax, changed function identity, and failed cases
return concrete observations to Qwen.

For richer executable behavioral tests, an embedding Form cell calls:

```text
fcac-run(model, goal, documents, writable,
         list(native-checker, immutable-contract), context, turns)
```

The callback receives `(immutable-contract, candidate-documents)` and returns
`list(passed, actual-observation)`. It runs in-process. The model cannot replace
that callback or its contract. A caller claiming native-only behavior must keep
its callback native too. No generic shell test runner is implicitly provided.

An embedding cell can run a fresh read-only review with:

```text
fcac-review(model, goal, documents,
            list(report-checker, report-contract, source-checker, source-contract),
            context, turns)
```

`report-checker` receives `(report-contract, list(original-documents, report))`
and returns `list(0-or-1, actual-observation-string)`. The optional last two
checker elements bind `verify` to a native source-only callback, which receives
`(source-contract, original-documents)`. Without them, `verify` reports its
absence; submission still runs the report checker. This lower-level door does
not recall lessons, write checkpoints or offer training examples. The JSON door
provides ordinary continuity unless `evaluation` is 1. Neither door grants an
arbitrary filesystem or compiler surface: richer native tests belong to the
caller-bound callback.

### Execute a proposed function during review

`bml/form-cli-review-execution.bml` lets a caller's report checker evaluate an
expression-bodied BML function against explicit native bindings. Use it when
the report proposes code and a field assertion cannot establish its behavior:

```text
bindings = list(list("combine", 2, native-combine-callback), ...)
prepared = fcre-prepare(proposed-source, expected-function-name,
                        expected-parameter-count, bindings)
result = fcre-run(prepared, arguments, bindings)
```

Each binding callback receives one argument list and returns
`list(ok, value, observation-string)`. `fcre-ok(value)` and
`fcre-no(observation)` construct those results. Successful `nothing()` is
distinct from a refused evaluation. The caller owns each callback's behavior,
argument validation, purity and execution cost. Preparation retains the binding
contract; execution refuses changed bindings.

The existing Form BML grammar must consume the entire source as one expression
definition with the expected name and arity. Every branch is checked before
execution: names must be parameters or explicit bindings; calls must match
their arities. Literals, parameter references, bound calls and lazy
`if … then … else …` expressions are supported. Extra definitions, unknown
names, recursion and dynamic callees are refused. Source is limited to 65,536
bytes and admitted expression depth to 64. This is an expression lane, not a
general BML module compiler or an isolation boundary for effectful callbacks.

Compare the returned values with caller-owned expected behaviors inside the
report callback, retaining the original source and report assertions. Pass that
combined callback to `fcac-review` or `fcac-admit`. It then runs on **every report
submission, including repair submissions**. Supplying a failure once at repair
entry and subsequently checking only report fields leaves the failure outside
the acceptance contract. An explanation of a counterexample is not its repair.

The evaluator makes no filesystem, process or model calls. The normal review
controller carries failed observations into its existing repair and diagnostic
flow. `tests/form-cli-review-execution-band.fk` checks actual return values,
lazy evaluation, successful absence, callback errors and whole-source refusal.

`bml/form-cli-review-trace.bml` adds `fcre-trace-run(prepared, arguments, bindings)`.
It returns `[ordinary-result, invoked-binding-names]`. The trace records actual
evaluation order: arguments before their enclosing call, only the selected
branch, and no later calls after an argument fails. Each callback runs once.
Changed bindings and rejected source produce no calls. The ordinary evaluator
remains available without trace allocation.

These are expression call sites. Supplied callback bodies remain opaque; the
trace does not infer their internal calls or effects from their names. A caller
can compare original, rejected and verified candidates using the same inputs
and bindings, then return those observed traces alongside the paired checks.
Use the source to explain internal effects, and preserve the trace's scope.

`fcre-trace-detail(prepared, arguments, bindings)` returns
`[ordinary-result, events]`, where each event is
`[binding-name, evaluated-arguments, execution-result]`. This retains the inputs
and result at each actual call boundary, including a failed call. Calls skipped
after failure have no event. `fcre-trace-run` projects names from the same
execution; it does not invoke callbacks again. The trace itself emits no values
to the framebuffer. The caller owns retained data and any public projection.

When supplying native observations as initial context, keep the original source
documents and place the derived evidence in its own document. Name the controlled
inputs, observed values and scope. A final counter alone does not attribute its
change to a particular call; use the corresponding arguments and results for
that attribution. An accurate observation still requires checking the generated
answer against the source.

### Search a small native repair before another model call

`bml/form-cli-review-search.bml` searches one explicit repair family: replace
one existing call with a call that forwards the function's original parameters
to a compatible caller-owned binding. Compatibility comes from caller-supplied
result roles and matching parameter count. Roles describe the caller's domain;
they are not inferred types or a guarantee that a callback accepts every value.

```text
roles = list(list("finish", "continuation-result"),
             list("continue", "continuation-result"))
result = fcrs-search(source, name, arity, bindings, roles,
                     behavioral-checker, immutable-contract, check-budget)
```

The checker receives `(immutable-contract, prepared-expression)` and returns
`list(passed, actual-observation)`. It can call `fcre-run` for each case. The
search first checks the unchanged function, then reparses and checks each
generated candidate. It never invents expected outputs or changes the contract.
Candidates use existing parameters and bindings; no reference implementation
is required. Callers must supply safe, appropriately validated callbacks.

The result is `[status, source, checker-runs, observation]`. Status is
`unchanged`, `repaired`, `exhausted` or `refused`. Failed or exhausted search
returns the original source. The budget counts actual checker invocations,
including the original. No model, filesystem or process call is made by the
search itself. Attribute its output as native structural repair, separately
from model generation. A passing result establishes the supplied behaviors;
inspect and independently execute the generated source before a broader claim.

This is a targeted search strategy, not unrestricted program synthesis.
`tests/form-cli-review-search-band.fk` exercises a separate two-argument task,
additional input values, budget exhaustion, malformed checks, missing roles,
unchanged source and escaped/unicode source rendering.

### Attach native repair to the review loop

An embedding caller may append a native repair callback and its immutable
contract to the four-entry review checker:

```text
checker = list(report-checker, report-contract, source-checker, source-contract,
               native-repair, repair-contract)
```

After a valid failed report check, the controller retains that failure and calls
`native-repair(repair-contract, list(documents, report, actual-failure))` once.
The callback returns `list(available, proposed-report, method, observation)`:
`available` is 0 or 1; the other entries are strings, with a nonempty method.
It may use `fcrs-search` or another caller-owned native capability. The caller
must update any explanation or related claims made stale by a changed proposal.

When a verified proposal still needs the resident's full explanation, return
`list(available, proposed-report, method, observation, "review")`. After the
unchanged checks pass, this leaves the task in review with event status
`accepted-for-review`. The same resident receives the exact rejected and verified
reports with their respective check results. It must complete the original
caller report; that submission runs the same checks again. A passing native
candidate alone does not finish this continuation. Existing turn and context
budgets still apply, and no additional model admission is requested by this hook.
Other fifth-element values are malformed.

A changed, nonempty report runs the original complete checker again. Passing
is required for completion. A failed native candidate returns to ordinary
repair without recursively invoking the callback; declining or returning the
same report preserves the existing failure. Malformed checker/proposal results
cannot approve a report. The hook does not run for passing reports or coding
mode. Model JSON cannot install or replace it; ordinary JSON requests retain
their existing behavior.

Results expose `report_source` and `native_repairs`. Each native event records
method, outcome, caller observation and controller check count. A later submitted
report resets its own attribution while preserving the native attempt history.
Each new event also pairs the exact prior report with its original failed check
under `before`, and a changed native report with its own check under `after`.
A declined or unchanged proposal has no `after` check. Older events expose null
evidence instead of reconstructing a history they did not retain. Malformed
checker output is explicitly marked, with no valid check asserted. These records
are returned to the caller and preserved in continuity. The four-element callback
does not add them to model feedback. The explicit `"review"` continuation sends
the newest pair under `native_review` once per resident context. Only a completed
observation marks it delivered; later feedback may use `native_review_reference`.
A fresh or resumed context receives the full pair again. Historical failed checks
stay attached to the rejected report instead of appearing as a current failure.
When the live controller matches the exact rejected report to the resident's
just-generated submission, `before.report_reference=preceding-submission` replaces
that duplicate report text in feedback. Its check still travels in full. Without
that match the complete report travels. Bootstrap always includes the full pair,
and renewed contexts clear the submission attestation. Returned provenance keeps
the exact report in every case.
Context renewal and binary continuity preserve those records. Nested search
checks remain the native callback's observations, distinct from the controller's
whole-checker count. Callback execution and its side effects are caller-owned.
Native work remains inside the submitted turn and does not consume another
model-turn allowance; its verification runs and provenance are counted separately.

`tests/form-cli-native-review-repair-band.fk` exercises successful and failed
repair, declines, unchanged proposals, malformed results, source preservation,
attribution, result projection, continuation, observation admission and binary
continuity without loading a model.

Review guidance follows the caller's requested report fields and value types.
Decision-first guidance applies when the task asks for a decision. Originals,
rejected candidates, proposed changes and verification results keep their own
attribution when the model explains the evidence. These instructions guide
generation; inspect the returned answer to establish whether it followed them.
The shared guidance also keeps a format check's scope separate from behavioral
verification, and a total attempt count separate from the final observed result.

Review currently shares the same Qwen and context: **not independent-model
validation**. Qwen weights remain unchanged; the shared native Llama adapter
learns asynchronously from observed outcomes. The loop does not assert rented-model
parity, broad coding quality improvement, or `voice-home=1` from a passing example.

## Observe

Each movement publishes a fresh `qwen.coding.<pid>` Glass snapshot with the
current role, actual native tool-call count, cumulative generated-ID count,
repair attempts and caller-check runs. Check runs count whole callback calls,
not individual assertions. A failed check remains counted after a later pass.
Generation refreshes these counts every four IDs, preserving the pending ID
exactly once and decoding the complete reply only after generation ends.
Terminal metadata adds injected IDs, position and elapsed milliseconds.
Repair observations carry complete current failure evidence once. Notes and
pending tasks exactly equal to that evidence use a shorter reference to the
same message's `failure_evidence` field. Distinct text and retained controller
state stay intact; no earlier context is required to resolve the reference.
Prompt, response and source content stay out of the diagnostic framebuffer.
The JSON result carries candidate document content and, for review, the report
back to its caller. Rechecks on recall/resume count as actual checker calls.

Regression doors (preflight each FK band first):

```text
form/form-stdlib/tests/form-cli-code-policy-band.fk   -> 65535
form/form-stdlib/tests/form-cli-code-request-band.fk  -> 255
form/form-stdlib/tests/form-cli-code-memory-band.fk -> 65535
form/form-stdlib/tests/native-session-code-band.fk -> 511
form/form-stdlib/tests/form-cli-code-session-band.fk  -> 31
form/form-stdlib/tests/form-cli-code-telemetry-band.fk -> 7
form/form-stdlib/tests/form-cli-code-definition-band.fk -> 127
form/form-stdlib/tests/form-cli-code-dispatch-band.fk -> 7
form/form-stdlib/tests/form-cli-code-repair-band.fk -> 262143
form/form-stdlib/tests/form-cli-code-documents-band.fk -> 255
form/form-stdlib/tests/form-cli-agent-tool-wire-band.fk -> 131071
form/form-stdlib/tests/form-cli-agent-tools-examples-band.fk -> 32767
form/form-stdlib/tests/qwen38-sliced-head-band.fk -> 7
```

The existing policy, request, memory and session-code bands also contain
fail-fast review guards: immutable sources, report-aware checking, caller-bound
verification, report checkpoint round trips, fresh evaluation before recall,
and exclusion of review reports from supervised implementation examples.

For an explicit real-model recovery exercise, run
`form-run ./fkwu observe/form-cli-code-retry-witness.fk` and send `qwen38-q8`
on stdin. The fixture deliberately supplies a wrong candidate for `x*x+x+1`.
The native checker fails it before Qwen is admitted; Qwen must then diagnose,
repair, review and pass six arithmetic cases plus preservation of a second
document. The initial wrong edit and six initial policy turns are **fixture
inputs, not model-generated behavior**. The witness reports newly generated
model replies separately. It admits real weights, publishes actual Glass
metadata, and releases its model; it is not a simulated model test.

`observe/form-cli-code-first-reply-witness.fk` accepts the same model-name line
and checks a bounded first reply on public two-document data. It requires a
completed refinement reply advancing to `plan` and successful release. Its
128-ID generation bound is a probe bound, not a production reply limit.
The 2026-09-09 probe found that sliced prefill drained its concurrent batch,
then submitted the gather/head with no concurrent batch armed. Re-arming
before the head changed the identical prompt's first response from unrelated
54-ID JavaScript to a valid 70-ID refinement. Generated text was never executed
as host code. The structural band guards this batch boundary; the live probe
checks actual first-role behavior.

The native tokenizer parity witness is
`observe/form-cli-code-tokenizer-witness.fk`. On this host, its 34 observation
IDs were identical across the scanner and indexed paths: 19,328 ms versus
1,124 ms. This is one encoding measurement, not a whole-task speedup claim.

Live local Qwen observations on 2026-09-09:

- Configuration edit: 11 model replies, 3 native tool calls, 3 completed tasks,
  481 generated IDs, all three native `jq` checks passed, release verified.
- Executable Form edit: changed `add(x,1)` to `add(mul(x,2),1)` in a resident
  definition module. Four behavioral cases passed, including zero and a negative
  input. 11 model replies, 3 native tool calls, 3 tasks, 579 generated IDs,
  887 newly injected IDs, 10 same-session observations, release verified.

Both runs recovered from a first reply that did not satisfy the refinement
contract. These are observed small tasks, not a held-out coding benchmark.

On 2026-09-11 the read-only JSON example above completed with real local Qwen:
8 replies, 2 native calls (read and caller-bound verification), 2 check stages,
249 generated IDs, 643 injected IDs, no repairs or recalled lessons, and verified
model release. The source remained unchanged and all three returned report
fields passed. Total admission-to-release time was 287,493 ms under other local
workload. Its assessment record is excluded from training. This is a small
functionality witness, not an unseen review benchmark or latency guarantee.

## What we learned from open-source agents

Qwen-Agent's [function-call loop](https://github.com/QwenLM/Qwen-Agent/blob/main/qwen_agent/agents/fncall_agent.py)
feeds executed tool results back into the model's conversation and continues
when tools were requested. We use that interaction pattern with Form functions,
without importing its Python runtime or provider transports.

Qwen Code's [engineering prompt](https://github.com/QwenLM/qwen-code/blob/main/packages/core/src/core/prompts.ts)
emphasizes inspecting existing project conventions and reporting actual
verification outcomes. Its [role-specific agents](https://github.com/QwenLM/qwen-code/blob/main/docs/users/features/sub-agents.md)
demonstrate restricted tool sets and specialized contexts. This implementation
uses restricted roles but deliberately reports its shared-context review seam.
These are design references, not evidence that this local model has passed a
coding benchmark. No framework source was copied into Form.
