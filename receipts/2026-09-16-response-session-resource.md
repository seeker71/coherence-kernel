# Response sessions own their optional repair resource

Signed: Codex, 2026-09-16.

## The movement

The response-session runner now offers its existing report-repair resource
after a native read-only review fails. The manifest supplies permission and a
shared process allowance. A passing native report, failed source check,
changed source, missing report, unreleased model or code-edit case consumes
no provider process. The native-only manifest remains sufficient.

The native result and timing remain intact. Assistance has its own source,
checks, timing, usage event and process count. Session outcomes distinguish
native success, assisted success and unresolved cases. Stable usage events
are retained once; replay adds zero new processes. Shared assessment/evidence
helpers remove the dependency cycle between session and resource.

The implementation came from one Form-owned provider worker, followed by
coordinator review. Its observed completed-turn usage was 2,339,689 input
tokens, including 2,219,520 cached and 120,169 uncached, plus 22,332 output
tokens. Elapsed time was 601,125 ms. Coordinator usage is separate. This is
implementation cost, not a matched response-session comparison or proof of
token savings.

Private evidence: `.hearth/response-parity/session-integration-worker-result.json`.

## Observation

The session and resource bands pass after clean preflight. Native validation
passes for both; no kernel source changed. These checks establish selection,
attribution, permission, budget and replay behavior. They leave answer quality
and whole-session parity unassessed.

The fresh native session completed in **219,593 ms** with **87 generated IDs**,
**437 injected IDs**, two replies, one repair and `release_ok=1`. The first
answer incorrectly treated the missing translation as rejection. The second
corrected that field while retaining concept 17, language `id` and a null
translation. Source/report assertions passed, documents stayed unchanged,
and **zero provider processes** were used; its allowance of one stayed intact.

Evidence: `.hearth/response-parity/session-resource-live-summary.json`,
`session-resource-live-process.log`, and the frozen manifest in that directory.
Private actual replies remain at `.hearth/code-memory/replies/76691-1789539752397.txt`
and `76691-1789539871597.txt`. These assessment answers were not teaching targets.
The experiment used the retained translation question with the production
direct review controller, four turns and 4,096 context positions. Its context
and controller differ from the earlier low-level continuation experiment;
the different outcome does not isolate a single causal improvement.

An explicitly bounded **one-turn** integration exercise then used the same
question and assertions. Its native report still failed, with source preserved
and model released. Form selected the optional resource, consumed exactly one
process, and retained `native_status=attention`, `checked_behavior_passed=0`
alongside `case_status=assisted-success`. The provider's corrected report
passed the original assertions. This exercise limits native opportunity on
purpose to witness the handoff; it is not evidence that the fuller native
session needed assistance or that assistance saves time in matched sessions.

The bounded session took **176,622 ms**, including **167,594 ms** native time
and **8,912 ms** provider process time. Provider usage was **16,811 input**,
including **10,624 cached** and **6,187 uncached**, plus **27 output** tokens.
Remaining process budget was zero. Exact resource replay passed with **zero
new processes** and **one usage event** after deduplication.

Private evidence: `session-resource-handoff-summary.json`, its manifest and
process log, and `session-resource-replay-result.json`, all under
`.hearth/response-parity/`. Both actual answers were inspected outside the
framebuffer. The original failed answer and the provider correction remain
separate files. Semantic quality, resonance and whole-session parity remain
unassessed by these protocol witnesses.

Final checks: clean preflight, session band **1**, resource band **1**, hearth
band **32767**, native validation, clean diff, and drift gates **8191**.

Panel: counsel reports **0 orphans**, with **11/12 lanes unobserved** because
no hearth stands. This does not establish healthy performance in those lanes.

## Teaching and remaining work

The useful teaching is that repair and attribution can share one execution
path while preserving the original failure. The integration removes a manual
coordinator step; measured throughput must still show whether this helps a
whole session. The difficulty was keeping a successful repair from rewriting
native failure as native success. Separate retained results make that
distinction inspectable.

The next comparison still needs matched complete sessions and an assessment
of their actual answers. Neither field assertions nor this implementation
worker establish equivalent reasoning, warmth, frequency or minimum cost.

The verified protocol teaching was retained as
`verified-session-resource-boundary-v1` in session
`response-parity-session-resource`. Local learning round **14** completed;
promotions stayed at **4** and serving generation stayed at **5**. This is
the Llama 3B learning lane, not a Qwen weight update. Retention and candidate
training do not establish a promoted capability.
