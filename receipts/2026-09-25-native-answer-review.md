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

## Actual native review and repair

A fresh native `qwen38-q8` admission re-reviewed the actual retained answer
with the original goal, sources and checks. It used `knowledge-query`, 16,384
context positions, six allowed replies and the original 2,048-token reply
ceiling. The new instruction and fresh admission are changed conditions;
this does not isolate the instruction's causal effect from context renewal.
The reconstructed baseline has two caller transitions, one tool call and one
check, counted separately from native model work. This unmanaged evaluation
uses no recalled lesson, checkpoint or weight-learning target.

The first review rejected the weak connection between translation and a
response decision. The model then made a guarded edit, passed the original
checks, completed the task and accepted its revised answer. Its change replaced
the sentence about the codebook grounding witnessed surfaces with:

> This grounding informs the response decision: the body chooses the surface
> that fits the enquiry's plane, speaking the meaning it already holds rather
> than inventing a new one.

That names a response decision; it does not demonstrate an implemented
context-sensitive translation choice. The answer grew from 397 to 411 words.
The original source packet stayed byte-identical and the original word-range
check passed. No arriving-agent edit was applied during this run.

**The targeted quality gap remains open.** The accepted answer still says,
“The response is warm, direct, and free of self-assessment,” and still claims
the reader will become more grounded, stretched and free. Its retained review
nevertheless says the text is free of self-assessment. It also retains the
overbroad claim about acting without permission gates or approval queues.
The new report makes this contradiction inspectable; requiring a reason did
not make that reason correct. The review's approximate counts were not the
native counter's exact observations. Runtime checks continue to own counting.

| Completed native work | Observation |
| --- | --- |
| Model replies / tool calls / repair episodes | 4 / 2 / 1 |
| Native check runs after the reconstructed baseline | 3 |
| Generated / injected IDs | 1,674 / 2,609 |
| Native call / supervised process elapsed | 2,344,880 / 2,345,568 ms |
| Model release / process cleanup | 1 / released |
| Original source preserved / independent post-run recheck | 1 / pass |
| Provider calls | 0 |

The approximately 39-minute process used progress-aware supervision without
a wall-time deadline. Both output streams were fully drained. This is the
observed whole-run duration with concurrent coordination, not an isolated
inference benchmark. The complete public result, answer, admission context,
native helpers and process evidence are retained beside the protocol replay.
Private native reply and reasoning files were not read. The remaining review
work is to resolve the explicit requirement against the offending text;
another general request for confidence or warmth would not supply new evidence.

The initial supervisor wrapper supplied an empty stdin path and exited with
status 1 before model admission. The repaired wrapper supplies the existing
no-input marker. The native run was not restarted. A briefly opened
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

The verified acceptance/retention contract was returned as session teaching
`896d35793b6d560429016e98e18db5a325ff024ef0a3e30c0138c232d8e6b749`.
Its worker launched; that is not a claim of a trained or promoted capability.
The evaluated answer and review remain excluded from gradients. Landing gates
passed 8191/8191 after rebasing the implementation onto origin/main.

— Codex
