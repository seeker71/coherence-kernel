# Behavioral acceptance stays active during native repair

Signed: Codex, 2026-09-16.

## Executing changes

`form-cli-review-execution.bml` evaluates an expression-bodied BML proposal
inside a caller's native report checker. The existing grammar must consume one
whole function with the expected name and arity. Every branch is validated
against explicit bindings before evaluation. Preparation retains that contract;
changed bindings are refused. Successful `nothing()` differs from refusal.

Callbacks own their argument validation, purity and cost. This lane supports
literals, parameters, bound calls and lazy conditionals, with bounded source
size and expression depth. It is not a general module compiler or a sandbox
for effectful callbacks. The existing report-callback boundary runs these checks
on every submission, including repair. No external runtime or C seed growth is
introduced.

The controller also removes exact repeated failure text from its observation
projection. Complete `failure_evidence` travels once in the same message;
longer identical notes/tasks reference that field. Distinct text, short evidence
and retained controller state remain intact. No earlier context is needed to
resolve the reference.

## Actual replay and re-observation

A private caller combines the original source/report assertions with the five
controlled behavior cases from the previous receipt. The frozen original loop
and pure dependencies are namespaced without changing their logic. This tests
branch outcomes; it is not actual disk/GPU fault injection.

The same question, documents, prior report and counterexamples enter repair.
Local Qwen receives an 8,192-position context and an eight-turn bound, with
evaluation enabled and provider calls disabled. The next two submitted reports
repeat the disproven function. Both fail the persistent checker, with a read-only
tool call between them. Another reply reaches the context limit and remains
incomplete. Final outcome:

| Observation | Value |
| --- | ---: |
| Stage | attention |
| Checks / recorded failures / tool calls | 3 / 3 / 1 |
| Generated / injected IDs | 821 / 1,519 |
| Elapsed | 833,662 ms |
| Release | confirmed |
| Final behavioral recheck | failed |

The independent compiled witness and current in-process evaluator both reproduce
two masked reasons: checkpoint refusal and an offline model at the budget
boundary. The proposal/witness hashes match the previous receipt. The acceptance
loop refuses false completion; the assessment case remains unrepaired. The
production controller's existing correct budget path was never replaced.

On the actual failed state, the original observation projection and the same
native tokenizer/protocol crossing measure **1,499 → 695 bytes** and
**531 → 226 IDs**, saving **305 injected IDs** while preserving the complete
failure evidence byte-for-byte. A correlated branch action 2 records this care.
This establishes input reduction, not answer-quality or latency improvement.
The live run admitted the earlier projection; a live replay of the compact
projection remains owed. Its evaluator also predates the changed-binding guard,
whose unchanged-binding behavior was rechecked afterward on current source.

## Checks and failures

The evaluator band returns **65,535**, exit 0, covering sixteen behavior and
refusal checks. The existing policy band returns **65,535**, exit 0, including
exact evidence retention, duplicate references and distinct notes/tasks. The
request band returns **255**, exit 0. Required preflights are clean.
The final drift door passes **8,191/8,191**, with zero refusals and exit 0;
`git diff --check` is clean. The share reader withholds a percentage while the
latest appended evidence is still being validated.

The initial absence test failed: bare `nothing` in the callback became a numeric
sentinel on this surface. Explicit `nothing()` repaired the behavior without
weakening the check. The private checker initially called nonexistent
`list-to-str`; preflight refused it. The existing `value_str` repaired that call.
Private evidence is retained under `.hearth/response-parity/acceptance-*`.
Assessment answers and private reasoning remain excluded from teaching.

## Cost, learning and next movement

The prior teaching completed at learning round 23, with four promotions and
serving generation 5 unchanged. The new verified method is retained as session
`native-review-execution-v1`, event
`verified-bindings-and-feedback-projection-v1`; the worker is running with one
pending example. This learner trains Llama 3B, not the Qwen used in the trial.
Retention establishes no response-quality gain.

The panel reports **11/12 lanes unobserved**, with no standing hearth. At the
intermediate snapshot, the goal counter moves from 3,892,538 to 4,110,471:
**217,933 coordinator tokens**, subtracted in Form. This remains too costly.
It is neither final-turn usage nor an API breakdown. The separate transcript
meter reports 930,951 cumulative output tokens, decided through byte 46,568,219
of 46,568,251; these scopes are not added. The local trial called no provider;
the coordinator remained rented.

The useful surprise is that persistent verification exposes the repeated failure
without turning it into success. That difficulty also revealed a removable
feedback cost. The next attempt must produce a changed, verified proposal with
less repeated guidance. Overall quality, human resonance, throughput parity and
minimal rented cost remain open.
