# Learning executable BML

`learn/bml-execution-practice.bml` carries three related practice examples and
three distinct transfer tests. The native runtime executes every reference
before any target can enter session learning. This distinguishes a correct
execution target from a plausible explanation of BML.

The practice covers absence versus present zero, a complete recursive
declaration, and an arithmetic repair. The transfer tests compose selection
with recovery, request the last present value, and repair summation across
absent entries. These small cases measure those contracts only. They do not
establish mastery of BML or the repository's broader concepts.

Prepare a fresh private evidence home, with its path on stdin:

```sh
form-run ./fkwu observe/bml-execution-practice-prepare.bml
```

The preparation executes all six references, retaining their exact source,
stdout, stderr, exit status, elapsed time and source hash. Its manifest freezes
both practice and test prompts before model generation. Prediction targets
come from actual reference output; code targets are the declarations that ran.
Changes to the curriculum require a fresh preparation and comparison.

`observe/bml-execution-observe.bml` accepts one JSON line. Its actions are:

- `generate`: supply `home`, a fresh `condition` label, and an explicit
  `adapter` generation (an empty string measures the base without LoRA).
  One native model residence answers the three test
  prompts with independent contexts. Exact prompts, responses, token streams,
  model and adapter identity, stop reasons, timings and events stay private.
- `ground`: the same inputs supply the three executed practice examples as
  context. Their bytes are checked again. This is assisted practice, reported
  separately from unassisted transfer; no test reference answer is supplied.
- `feedback`: also supply the previously graded `parent` condition. Its
  unsuccessful code attempts and the first compiler diagnostic join the
  verified practice context. Complete diagnostics remain in the source
  reading. A compiled behavioral failure supplies its failure status, without
  revealing the expected test outputs. This keeps the adapter unchanged and
  measures the native model's response to observed feedback.
- `focus`: the same parent inputs select related practice by the existing
  native RAG semantic features and shared-feature/union overlap. Each case
  receives its strongest reference matches and only its own feedback.
  `retrieval.json` retains every offered score and selected source. This
  tests focused retrieval separately from providing all available examples.
- `grade`: supply `home` and `condition` after inspecting the proposals.
  This executes generated code with the frozen caller checks. The runner is
  not a sandbox and does not impose an execution timeout. It is intended for
  these inspected, finite, local reference exercises. No generated text is
  silently repaired or stripped of Markdown. Prediction format and individual
  values are reported separately. Full compiler diagnostics remain available.
- `enqueue`: supply `home`. Only the three practice rows are offered, each
  bound again to its source, expected execution output and exact target.
  Stable session/event identities make repetition idempotent. This writes
  `embody.request.json` without launching training itself.
- `status`: supply `home`. Read actual worker state, candidate and serving
  selections, each practice row's result and latest training event. Missing
  results mean pending or interrupted work; they are not zero-cost successes.
- `report`: supply `home`. Retain `learning-report.json` with the per-row
  before/after losses, promotion choices, input and supervised token counts,
  native GPU timings, optimizer updates and checkpoint stages from their
  actual source events.

Start and retain the native learner through its normal session door:

```sh
form-run ./fkwu observe/form-cli-session-home-embody-run.fk < HOME/embody.request.json
```

The learner retrieves related verified examples, rehearses prior knowledge,
continues Adam and performs one full-gradient update for each new practice
event. Its per-row observations decide whether the candidate becomes serving.
Test attempts are retained as evaluation experience, outside gradient targets.
Learning and inference use the native Llama 3B pipeline; this does not update
the Qwen coding model or the external authoring agent.

After the worker completes, use `generate` with the resulting candidate and a
new condition label, then `grade` against the same frozen manifest. Compare
the candidate before training with its descendant candidate. A separately
retained serving adapter must be identified as such. Lower training loss,
successful syntax, correct execution and generalization are separate findings.

The preparation and grading boundaries emit `organ-health-v1` observations,
needs and applied actions. Failures remain available for related practice and
later observation. These exercises are an explicit learning movement, not a
new mandatory regression tally for every organ. See
[native session learning](native-session-learning.md) for the shared learner
and [live diagnostics](live-dynamic-diagnostics.md) for its event flow.

## A local teacher and practice that checks its own behavior

`observe/bml-native-teacher-run.bml` accepts `home`, a new `condition`,
`model` (for example `qwen38-q8`) and `context_capacity`. It supplies the
executed practice references to native Qwen and uses the original transfer
contracts. Weights stay admitted between tasks; each task receives fresh
recurrent and KV state. No LoRA is applied to this teacher. Generation runs
until a model stop, cancellation, an actual context boundary or a native
failure. The capacity is an explicit physical allocation, not a claim of
unbounded context. Inspection and the original `grade` action remain separate.

The teacher retains original token IDs, prompts, streams, stop reasons,
per-task context preparation times and decoding events. Each new token is
decoded once; previous IDs stay in a persistent list until the final ordered
receipt is written. The generator reports model release and live buffers.
A retained response is not necessarily complete: read its stopping reason.

`learn/bml-scalar-practice.bml` is a second, frozen curriculum. Its practice
asks for present-entry counting, first-positive selection and sum of squares.
Its separate transfer tests ask for zero counting, first-negative selection
and sum of absolute values. Every reference has six actual execution checks.
The native JSON door is `observe/bml-scalar-observe.bml`:

- `prepare` takes a fresh `home` and executes all six references.
- `test` takes `home`, a new `condition` and an `adapter`; it generates the
  three transfer responses without practice examples in the prompt.
- `practice` takes `home` and a new `condition`; local Qwen receives the three
  practice contracts, each in its own stream, and a different BML example.
- `teacher-test` takes `home` and a new `condition`; local Qwen receives the
  frozen transfer contracts and verified practice references. This is assisted
  local transfer, with no test answers in context and no training.
- `grade` takes `home`, `condition` and `kind` (`test` or `practice`), after
  proposal inspection. It executes exact response bytes and retains failures.
- `extract` takes `home`, a new `condition` and a previously graded
  `parent`. It can extract the unchanged interior of one complete BML fence
  that occupies the whole response. Prose and ambiguous or incomplete fences
  are preserved unchanged. This is explicit transport assistance, followed by
  another `grade`; it does not rewrite code. Original responses and their
  failed readings remain available. Inherited token counts belong to the
  source generation; extraction reports zero new model calls and tokens.
  The parent's practice or evaluation role stays attached; extracting a test
  response never makes it a training target.

After the teacher's actual code passes, `observe/bml-practice-fit-run.bml`
accepts a `prepare` request with `home`, the verified `teacher` condition,
an explicit parent adapter generation in `parent`, and `learning_rate`.
It binds the prompts, code, execution receipts, training rows, request and
parent identity before writing `home/fit/request.json`. Only practice enters
this dataset. The `run` action then takes `home`.
It requires an available explicit adapter and checks the admitted LoRA tensors;
a missing selection cannot silently become a base-model fit.

This loop owns one native Llama admission and one resident Adam state. It
generates practice responses, checks them, makes one native LoRA update for
the currently unmet practice, publishes a complete checkpoint and observes
every contract again. A passing function receives no further gradient unless
its generated behavior fails again. Each selection retains the observation,
selected row identities, dataset hash and actual request under `fit/attention`;
dataset traversal is rebound when the selected data changes while Adam age
continues from the checkpoint. When a previously passing function fails,
the next update's learning rate is divided by one plus the number of those
new failures. The pressure observation retains both execution readings,
their hashes, the affected functions, offered responses and selected rate.
A resumed fit restores the checkpoint's actual rate and the bound observation
that selected its training data. Model
and optimizer tensors stay resident across rounds. Generated practice
success ends the loop; a nonempty `home/fit/cancel` retains the last completed
checkpoint, and real runtime failures remain failures. There is no fixed
round count. Every completed update carries its actual row losses, token
weights, gradient health, GPU times and checkpoint identity through the
existing native trainer events. Generation names the checkpoint represented
by the live adapter tensors.

While the owner runs, send the evidence home as one stdin line to
`observe/bml-practice-fit-status.bml`. It reads complete events without
interrupting the model. The reading names model admission, LoRA pairs, the
last offered and selected care, event age, completed updates, the latest
generated practice and cumulative token and timing records. An in-flight
row is not counted as completed, and a missing final result remains null.
Row GPU/host times and round totals overlap; do not add the two views together.

Automatic execution here is limited to the caller's pure scalar-list
exercise: one declaration, a nonrecursive empty-list base case, known pure
operations and recursive calls on `tail(xs)`. An unsupported proposal is
reported as not executed. Supported proposals still have to compile and
satisfy the caller's six checks. This is not a general BML interpreter or a
substitute for a richer caller-bound checker.

`fit/result.json` identifies the resulting candidate and the actual number
of updates in that admission; `fit/admissions` retains each completed
admission, and the event stream retains all completed updates across resumes.
A practice pass can be memorization. Run the frozen transfer
`test` and `grade` with that candidate to measure what carries over. The fit
loop does not read transfer answers, train on failed model proposals, or
change the shared session learner's serving pointer. Ordinary session
teachings and serving evaluation continue through the session door above.

The fit door's `enqueue` action takes `home` and `teacher`. It rechecks the
teacher's exact prompt, payload and successful execution, then retains those
three practice targets as `native-code` / `verified-answer` session records.
Their identity binds the manifest and teacher generation, so replay is
idempotent. `teacher-session-queued.json` names the records. Drain them through
the normal session embodiment door; only its measured serving decision can
promote them. This makes verified local teacher practice available to the
existing cross-worktree experience and rehearsal paths without promoting a
fit checkpoint or admitting transfer answers as training targets.

`observe/bml-token-observe.bml` accepts `home`, `teacher`, a fresh `condition`,
an explicit `adapter`, and the practice `case` id. It rechecks the teacher's
execution, performs one native forward pass, and compares each supervised
target token with the highest-logit token under the verified preceding text.
Private `token-observations/CONDITION/reading.json` retains positions, token
IDs and decoded pieces. Stdout reports counts and the evidence path. Binding,
admission, preparation, forward, target reading and release times remain
separate. A missing adapter fails before generation, and admitted tensors are
checked again. This diagnostic emits no generated answer and performs no
gradient update. Correct teacher-forced decisions and working independently
generated code remain separate observations.
