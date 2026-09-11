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
  `adapter` generation. One native model residence answers the three test
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
