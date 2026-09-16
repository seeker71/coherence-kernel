# Keep each report beside the check that observed it

Codex, 2026-09-16. Continues `2026-09-16-autonomous-native-review-repair.md`.

## Repair and re-observation

The local explanation had confused a rejected proposal's failure with the
original code's behavior. Its evidence packet held original and repaired code,
but omitted the rejected proposal while retaining that proposal's diagnostics.

Native review events now retain `before` and `after` evidence: the exact report
and the checker result that actually evaluated it. Declines and unchanged
proposals have no after check. Older events retain null evidence. Malformed
checker output is marked without asserting a valid result. Binary continuity
preserves the records. They are returned to the caller, outside automatic model
feedback, so the caller can select what a later explanation needs.

The submission replay completed in 23 ms with no model admission, two whole
checks, one original failure and one accepted native attempt. Re-observation
matched the retained submission and final report to their respective evidence
records, confirmed failed/passed results, and reran the behavioral check. All
five observations passed. The diagnostic exchange applied revise action 2;
its framebuffer contained metadata only.

## What the local answer actually changed

The local Qwen Q8 renderer received the same question, original excerpts, model
and limits as the preceding run. Its native-result document now included both
proposals, each paired with its actual check. No reference explanation was added.
It completed in 186270 ms with 391 generated IDs, zero injected IDs, one format
check and release 1. The preceding run took 154706 ms and generated 351 IDs.
This fuller evidence costs context and time in this single comparison.

The new answer correctly identifies that the rejected submission bypassed the
loop and changed terminal outcomes, while the native repair calls the loop
without injecting unnecessary feedback. It no longer assigns that rejected
proposal's terminal failure to the original feedback path. This is a specific
attribution improvement, not general causal accuracy or response parity.

Two remaining claims prevent a full quality pass. The answer says that passing
the format check establishes correctness for the supplied cases; the format
check only checks unchanged documents and a nonempty explanation string. It also
describes zero counterexamples across three checker runs, while three is the
search's total invocation count and zero describes its final result. Its telemetry
description names preservation for non-exhausted cases without clearly explaining
that the repaired exhausted-budget path still enters the loop's telemetry call.
These weaknesses remain in the unchanged raw answer. The next explanation must
distinguish check scope, total runs and final results precisely.

Private evidence: `.hearth/response-parity/lineage-replay*`, `lineage-close*`,
`lineage-explain*`, including the complete raw answer and inputs in its result.
Evaluation answers remain excluded from training.

## Verification and limits

- Native review repair: 23 assertions, 8388607, exit 0. These cover paired
  accepted/rejected evidence, absent checks, legacy history and malformed checks.
- Existing policy, request and memory bands: 65535, 255 and 65535, exit 0.
- Final preflights: no errors, warnings or unresolved calls.
- Drift gates: 8191/8191, exit 0.
- Counsel panel: orphans 0; 11/12 lanes unobserved without a standing hearth.
- Before-landing coordinator counter: 4322714 to 4356149, delta 33435 tokens;
  this is an intermediate snapshot, not the final turn cost. The separate
  transcript meter read 1006711 cumulative session output tokens. Share was
  declared and withheld because no completed evidence row was available.

Initial preflight rejected the serializer because a conditional block lacked
its terminating semicolon: `source-compile: unconsumed form.bml do suffix`.
The correction passed preflight and the behavioral checks; no gate was weakened.

The previous teaching worker completed round 26, pending 0, with serving
generation 5 unchanged. A verified teaching of evidence retention was submitted
this turn; its worker is pending at this receipt. This is the native session
learner, not a claim that the Qwen weights changed.

All implementation and experiment logic is Form/BML on fkwu. No provider call
was made by these native experiments and no new runtime dependency was added.
Codex coordination remains rented work. Whole-session quality, resonance,
throughput and minimum-rental parity are still unproven.

The useful surprise was that supplying the rejected code beside its own failure
changed the explanation's attribution. The difficult part became visible evidence:
more complete context improved one claim while another unsupported claim survived.
That distinction keeps the next repair concrete.
