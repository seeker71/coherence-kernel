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

## Call without knowing Form syntax

In form-cli, enter `code` followed by a JSON object. The standalone native door
is `form-run ./fkwu observe/form-cli-code-run.fk`; send that same JSON object as
one stdin line, without the `code` prefix. For example:

```json
{"goal":"Enable local coding in config.json. Preserve provider and remote.","documents":[{"id":"config","path":"config.json","text":"{\"enabled\":false,\"provider\":\"native-qwen\",\"remote\":false}\n"}],"writable":["config.json"],"checks":[{"tool":"jq","arguments":[".enabled","config.json"],"stdout":"true\n"},{"tool":"jq","arguments":["-r",".provider","config.json"],"stdout":"native-qwen\n"},{"tool":"jq","arguments":[".remote","config.json"],"stdout":"false\n"}],"model":"qwen38-q8","context":8192,"turns":64}
```

Documents are **resident values**, not filesystem permissions. The result returns
the candidate document values; it does not overwrite repository files. The caller
owns loading files, publication, stale-file checks, and repository landing. This
first door therefore does not claim autonomous arbitrary-repository completion.
Multiple supplied documents and multiple ordered tasks are supported. Only paths
in `writable` can change, and only during implementation.

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

The final review-role reply is
`{"verdict":"accept","report":"evidence-backed findings"}`. Here `accept`
submits the report for checking; it does not declare that the audited source
passed. Negative findings belong in the report. `reject` with a `reason` means
continue inspection. Read-only restrictions apply in every role, including repair.

`model` defaults to `qwen38-q8`, `context` to 8192 positions, and `turns` to 64
model replies. Both Qwen registry rows require actual artifacts, seals, tokenizer
indexes, tokenizer crystals and a wired native session lane. Actual digest verification happens on
admission. Missing resources never select another provider. Context or caller
budget exhaustion returns `attention` with candidate values retained; partial
generation is never accepted as a completed action. This door releases its
model on completion; it is not a long-lived shared hearth service.

## Roles and tools

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
