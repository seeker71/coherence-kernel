# Two reasoning limits and a shared-namespace repair

Codex, 2026-09-16. Continues `2026-09-16-native-review-call-trace.md`.

## Native interface repair

The preceding movement introduced `fcrt-run` for expression tracing, but the
resident turnwheel already exports that name. Trace exports now use
`fcre-trace-*`; the turnwheel keeps its existing interface. Callers and the
current guide use `fcre-trace-run`.

The execution band now loads both modules and exercises both entry points in
the same process. Its 23 checks return 8388607; the turnwheel's own band returns
65535. Both preflights and executions exit 0. The combined check addresses what
the earlier isolated bands did not exercise. Drift gates return 8191, and
`git diff --check` is clean. No C seed or runtime dependency changed.

## Bounded initial reasoning

The same frozen controller question, documents and caller checks were offered
to Qwen Q8 with initial reasoning enabled. The first admission allowed 2048
generated IDs. After its incomplete result, a correlated native control selected
one re-observation with a 4096 ceiling. The original request hash matches across
both; the requested resource bound is explicit in each effective request.

| Observation | 2048 ceiling | 4096 ceiling |
| --- | ---: | ---: |
| Wall time, ms | 582851 | 1022293 |
| Generated IDs | 2048 | 4096 |
| Final-channel boundary present | 0 | 0 |
| Completion observed | 0 | 0 |
| Final answer bytes | 0 | 0 |
| Release | 1 | 1 |
| Provider calls | 0 | 0 |

Both ended at attention with empty reports and failing report checks. The
measurement drivers exited 0 because they retained their results; that is not
an assessment pass. Source checks passed and documents remained unchanged.
Neither result supports an answer-quality claim. Private generation text was
not used as a report or training target.

The native audit totals both attempts: 1605144 ms, 6144 generated IDs, zero
feedback IDs and zero provider calls. The first attempt is not removed from
cost. Increasing the ceiling did not produce a usable answer in these trials;
the production default remains unchanged. These observations do not establish
that reasoning can never help another request or configuration.

Evidence is retained under `.hearth/response-parity/reasoning-first-live/` and
`reasoning-first-4096/`, with their adjacent drivers, logs and native audit.
The first driver was admitted before the trace namespace repair; the second
uses the corrected export. The trace callback did not run in either admission
because neither produced a report.

The next response experiment should supply native execution evidence before the
first answer, avoiding both an already-written incorrect explanation and another
larger reasoning allowance. The new trace primitive makes that attempt available.
Overall quality and throughput parity remain open.

## Cost and embodiment

Coordinator cost is separate from the zero-provider local trials. The active-goal
counter moved from 4862144 to 4946857 at an intermediate boundary, a delta of
84713 before landing and closing. The transcript meter separately reports
1158323 cumulative output tokens; these scopes are not added together.
Counsel still has 11 of 12 performance lanes unobserved with no standing hearth.
Share remains unmeasured with its percentage withheld.

A verified teaching about shared namespaces, combined checks and incomplete
generation was retained through the native session-learning door after Qwen
released. Worker launch was observed. This uses the existing Llama learner,
not Qwen, and does not establish a model-quality improvement.

The useful surprise is that twice the reasoning allowance still yielded no
answer. The discomfort became two concrete corrections: protect the shared
namespace, and stop treating a larger allowance as the next default quality fix.
