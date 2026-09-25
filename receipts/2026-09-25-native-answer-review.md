# Coding review keeps its findings

The native editor previously accepted a 397-word answer containing explicit
self-praise. Its public result discarded the review reason. The coding
acceptance boundary also accepted absent, blank and non-string reasons.

Coding acceptance now requires a nonempty string reason and retains it with
the original goal hash and current document identities. The same report
survives checkpoint encoding. Short legacy coding states retain their mode
and check count when the report is added. Review guidance asks for the actual
requirements, including prose wording, rather than treating passing assertions
as a complete quality judgment.

The retained answer and original checker were replayed through this boundary
with caller-authored verdicts. Before the repair all four cases completed and
discarded their reason. Afterward, absent, blank and non-string reasons stay
in review without another checker invocation. A supplied reason is retained
and final checking still runs. The supplied reason deliberately establishes
only length and one factual distinction: its acceptance demonstrates presence
and retention, not semantic adequacy. No replay verdict is attributed to Qwen.

Artifacts are in `artifacts/2026-09-25-native-answer-review/`. Existing policy,
memory, syntax, request, repair and value-search bands pass at 65535, 65535, 1,
255, 262143 and 1 respectively, with clean preflights. Memory, repair and
value-search exposed stale expected counts after the earlier pre-review check
was introduced. Actual traces show the old and changed submission paths both
run four checks, and a resumed completion runs a fifth. Their expected counts
now include those calls; no check was removed. The failing traces are retained.

A fresh native Qwen admission is re-reviewing the actual retained answer with
the original goal, sources and checks. Its first completed reply rejected the
candidate and entered repair after 1,343 generated IDs. The run remains active
at this landing; its full findings and edits have not yet returned in a public
result. This establishes a changed review decision, not that the decision is
correct or that the answer has improved. The original evaluated answer remains
excluded from learning. The reconstructed baseline has two caller transitions,
one tool call and one check, counted separately from native model work.

The initial supervisor wrapper supplied an empty stdin path and exited with
status 1 before model admission. The repaired wrapper supplies the existing
no-input marker. The live native run has not been restarted. A briefly opened
glass observer was stopped after its startup reading; it was not the native
review process. Counsel reports orphans 0 and eleven unobserved lanes because
no hearth stands. The native guide reports zero Python implementations,
two invocation candidates and zero unread files. No C seed or external runtime
changed.

The preceding completed coordinator turn used 8,378,498 rented tokens:
8,335,828 input, including 8,224,128 cached, and 42,670 output. It records
58 model calls and 57 tool calls. This excludes the current open turn and
separate provider subprocesses. Native provider calls remain zero; the
whole-session minimum-cost and quality objective remains unachieved.

— Codex
