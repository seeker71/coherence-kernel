# Native loss observations improve a local decision

Signed: Codex. Observed 2026-09-16.

The promotion review previously exhausted eight local turns and chose to retain
the serving model despite every supplied decision condition passing. It treated
unproven general quality as an extra prerequisite. The original request already
contained the complete supplied policy and numeric evidence.

## Repair and re-observation

`nsl-loss-evidence(before, after, tolerance)` now exposes arithmetic observations
from the native learner: supplied values, allowed after value, strict improvement
and tolerance comparison. `nsl-row-health` uses the same comparator and retains
its separate row-identity check. This function makes no promotion decision and
changes no learning state.

The local review received those computed observations in a separate document
beside its original documents. The original effective request identity matches
the retained Q8 run. Its question, policy, checks and model stayed the same; the
effective request changed by adding the derived evidence. No reference answer
or recommended decision entered that document.

| Observation | Wall ms | Generated IDs | Injected IDs | Result |
| --- | ---: | ---: | ---: | --- |
| Prior Q8 review | 492677 | 829 | 1452 | Wrong decision after eight turns |
| With native comparisons | 106545 | 194 | 0 | Correct promotion, first answer |
| Changed held-out row | 111491 | 173 | 0 | Correct retention, first answer |

The changed-evidence case increased the greeting row's after loss from 2.04 to
2.15, against a before value of 2.05. Its expected decision and regression row
were frozen before generation. The model identified that row and applied the
policy in the opposite direction. Both completed runs passed the unchanged
checks for their respective requests, preserved source documents and released
their model. Neither required a native report repair or provider call.

The two completed trials total **218036 ms, 367 generated IDs, zero injected
IDs and zero provider calls**. The matched successful trial took 386132 ms less
than the retained unsuccessful trial. These are individual observations, not a
general speed or quality guarantee.

Both answers still added broader evaluation or usage requirements to their next
action without support from the supplied policy. Correct decisions have improved;
follow-through remains a response gap. Neither report proves overall session
quality or human resonance. The retained provider comparison remains available;
no new provider baseline was purchased for this movement.

## Failed attempts and context boundaries

The first driver omitted the learner prelude. The compiler reported unresolved
`nsl-loss-evidence`, then continued toward model admission. That run was stopped
through a correlated diagnostic control and SIGTERM, exit 143. It has no accepted
result or completed token/release counters. Its time is additional to the table.
The repaired driver passed compile-only preflight before execution. The audit
also needed one delimiter repair before it ran successfully; no model was invoked
by that failed audit.

The source inventory found another boundary: the frozen controller review had
its caller source but neither `fcacs-observe` nor `fcms-observe-ids` definitions.
That comparison does not establish equal dependency coverage with a repository-
equipped baseline. The next controller comparison needs explicit source coverage.

Private evidence: `.hearth/response-parity/loss-grounded-bound/`,
`loss-regression-live/`, `loss-grounded-live.log`, `loss-grounded-audit.log` and
`source-coverage.log`. Private source and answer text remain outside this receipt.

## Embodiment and cost

The numeric boundary band passed **63**, exit 0: improvement, equality, exact
tolerance boundary, regression, row identity and the learner's existing tolerance.
Preflight reported zero errors, warnings or unresolved calls. Drift panel passed
**8191/8191**. Native guide: zero Python implementations; two existing voice
invocation candidates. Counsel still has **11/12 unobserved lanes**, with no
standing hearth, so it supplies no all-good service verdict.

The verified interface teaching was retained under event
`2026-09-16-native-loss-evidence-v1`; the local learner was launched. This is the
Llama learner, separate from the trial's Qwen model. Training completion and
promotion require the subsequent worker observation. Assessment answers were
excluded from that teaching.

Coordinator goal usage rose from **5516956** to **5596183** at the intermediate
post-trial reading: **79227 tokens**. This excludes the later closing work and is
separate from the native model counts. Zero provider calls inside these trials
does not mean zero rented cost for their development. Minimum rented expenditure
and full-session parity remain unproven.

The surprise was how a small native arithmetic document changed a repeated wrong
decision into a correct first answer. The useful movement through the discomfort
was checking the opposite decision too, then reading the answer closely enough
to keep its unsupported next-action conditions visible.
