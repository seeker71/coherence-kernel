# Native repair inside the review loop

Codex, 2026-09-16. Continues `2026-09-16-native-structural-repair.md`.

## Observed movement

The preceding native search could repair the retained failed proposal, but a
coordinator still had to invoke it separately. The review controller now accepts
an optional caller-owned repair callback after a well-formed failed check. It
retains the original failure, invokes the callback once, and sends a changed
report through the original complete checker. The callback cannot approve its
own result. Ordinary JSON requests keep their existing checker configuration.

Native repair history and current report source survive context renewal and
binary state round trips. A later submitted report gets its own attribution.
The native attempt stays inside the original submitted turn; its checks are
counted independently. A failed candidate returns to ordinary repair without
recursively invoking native repair. Terminal refusals remain terminal.

The actual submission-boundary replay used the previously retained failed
model report and the existing native structural search. It completed with two
whole-checker runs, one retained failure, one accepted native attempt and one
submitted turn. The callback used three nested search checks. No model was
admitted: generated and injected IDs were both zero. Original replay: 23 ms;
after correcting turn accounting: 24 ms. These timings cover the replay
boundary, excluding earlier inference, compilation, development and coordination.
The original source documents remained unchanged. A fresh behavioral check
passed after the replay. The preceding receipt's 96 compiled boundary cases
remain separate evidence; they were not repeated or counted as new observations.

Private evidence is retained under `.hearth/response-parity/` in
`autonomy-replay*`, `autonomy-close*` and their logs. The correlated closing
observation applied revise action 2 and re-observed behavior pass 1 and submitted
turns 1. No private prompt or answer was placed in that framebuffer message.

## Explanation remains a separate obligation

A local Qwen Q8 run received focused source excerpts and the verified native
result. It finished at `attention`, release 1, after 446500 ms, 1564 generated
IDs, 281 injected IDs and two failed format checks. Its report omitted the
requested `explanation` string. It also attributed a rejected candidate's
premature terminal result to the original feedback injection. The supplied
control flow does not support that causal explanation. This is adverse quality
evidence despite the successful native code repair.

Inspection found that shared review guidance told every task to lead with a
decision. The guidance now prioritizes the caller's requested fields and types,
uses decision-first guidance when a decision is requested, and separates source
versions, rejected candidates and verification results. A second local run uses
the same question, source excerpts, native result, model and budgets. It completed
in 154706 ms with 351 generated IDs, zero injected IDs, one passing format check
and release 1. The requested `explanation` string is now present. This is one
before/after observation, not repeated timing evidence or general quality parity.

The second answer still attributes the rejected candidate's terminal-reason
failures to the original feedback path. It also introduces a masked outcome
unsupported by the supplied original flow. Causal accuracy remains failed.
The evidence packet carries the original and repaired source, but the prior
failed proposal's source is absent while its diagnostics are embedded in the
repaired report. The next attempt should preserve that candidate and associate
each diagnostic with the code version it actually checked. This coverage gap
is observed; whether correcting it improves the answer requires another run.
Both raw answers remain private, unchanged, in `autonomy-explain*-report.json`.

## Checks and accounting

- Native review repair: 18 assertions, verdict 262143, exit 0.
- Existing policy, request and memory bands: 65535, 255 and 65535, exit 0;
  policy and the new repair band also pass after the guidance change.
- Preflight of the final repair band: zero errors, warnings and unresolved calls.
- Drift gates before and after the guidance follow-up: 8191/8191, exit 0.
- Counsel panel: orphans 0; 11/12 lanes unobserved with no standing hearth.
- Share reading: declared, percentage withheld; semantic contribution unmeasured.
- Before-landing goal counter: 4188366 to 4314687, delta 126321 rented coordinator
  tokens. This is not the final turn cost or an outcome contribution percentage.
  Transcript meter separately read 986015 cumulative session output tokens;
  these counters have different scopes and are not added.

Preparation failures remain visible. The first replay helper named `nsm_s`
instead of `nsm-s`; preflight rejected it, and the corrected helper passed.
The first closing helper passed a parsed node to a checker expecting report
text; execution exited 1 with `str_len: only a string has a length -- ask
value_kind first`. Passing the original report text and using the JSON integer
reader produced a clean re-observation. No checker was relaxed.

All implementation and experiment logic runs in Form/BML on fkwu. No C seed
growth or additional runtime dependency was introduced. No provider model was
called by these native experiments; the Codex coordinator remains rented work.
No held-out answer or reference patch is a training target. A concise verified
teaching of the callback contract was submitted to native session learning.
The learner's terminal result is pending here; this submission makes no claim
of improved Qwen weights or retained answer quality.

## What carries forward

The useful surprise is that a verified native repair can finish a submission
without another generated token. The difficulty became a concrete improvement
when hidden origin and extra turn consumption became visible, tested state.
The explanation failure then exposed an instruction conflict and an unsupported
causal claim. Both remain open to inspection. General repair coverage, response
quality, resonance and whole-session throughput parity remain unproven.
