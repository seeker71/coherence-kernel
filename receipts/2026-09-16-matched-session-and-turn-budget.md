# Matched review session and turn-budget repair

Signed: Codex, 2026-09-16.

## What was frozen

Three tasks share identical questions, source documents and report assertions
across a native Form session and a context-equipped provider session using
Form tools: promotion-policy reasoning, review of the actual native controller,
and dialogue grounded in the axioms, teachings, vocabulary and codebook.

The native arm has eight replies per case and one optional provider repair for
the entire session. The reference is one provider session, dispatched and
metered through Form. Its expected answers are hidden; native `verify` and
`check-report` tools expose observations. A rubric was written before reading
either answer. The set covers read-only tasks, not completed code-edit sessions
or every workload implied by overall quality parity.

Private frozen evidence is under `.hearth/response-parity/`:
`matched-session-manifest.json`, `matched-session-visible.json`, and
`matched-session-rubric.md`. Manifest digest:
`37f56e3d054e948c59baf0fe8563abea048402e0571430976ba79a562dcb646b`.
Evaluation answers stay outside training.

## Provider reference

The three reports passed the original assertions; process release was
observed. Elapsed time was **258,074 ms**. Actual completed-turn usage was
**454,009 input tokens**, including **432,896 cached** and **21,113 uncached**,
plus **5,726 output tokens**. Setup attempts remain in its event stream and
usage. Coordinator cost is separate.

The numeric decision follows the policy and preserves the evaluation boundary.
The controller proposal identifies the order of operations and preserves the
existing stop path. The dialogue distinguishes source-backed observations from
broader claims and offers a concrete reversible action. Its answer string
contains literal escaped paragraph separators; passing field assertions did
not detect that formatting defect. This is the coordinator's reading, not a
human preference measurement.

Evidence: `matched-session-baseline-result.json`,
`matched-session-baseline-check.json`, and the actual reply named there.

## Native evidence

The promotion case called **2.05 → 2.04 an increase**, repeated the claim,
reread the source, and reached eight replies without correcting it. Native
time was **1,053,721 ms**, with **1,529 generated IDs** and **2,876 injected
IDs**. Original source stayed unchanged and the model released. A direct
native numeric query independently found no row with `after > before`.
The comparison primitive works; this run did not use it to ground the decision.

Form then repaired the report through its offered resource. That process took
**18,619 ms** and used **16,472 input tokens**, including **10,624 cached** and
**5,848 uncached**, plus **276 output tokens**. The original native failure
remains beside `assisted-success`. The corrected report preserves missing
usage as unknown and held-out targets outside training. Fewer rented tokens
alone do not establish throughput parity.

The controller review completed in **220,309 ms**, one reply, **317 generated
IDs**, zero injected IDs and no provider use. It passed the field assertions,
but its explanation confuses `fcac-observe` (telemetry) with `fcacs-observe`
(model feedback). It claims the change prevents telemetry, although its
proposed `fcac-finish` still publishes telemetry. Its source proposal does
remove the model observation call at the budget boundary, but bypasses the
existing loop's other checks. The retained implementation instead follows
the baseline's smaller proposal, reusing that loop. This is a substantive
review gap despite the field pass.

The dialogue completed in **324,391 ms**, one reply, **457 generated IDs**,
zero injected IDs and no provider use. Its explanation is warm and offers an
actionable example, but omits the observed `1785cfc3` anchor and absent German
mapping that make the baseline's translation discussion concrete. Its blanket
claim that a timeout is not an error also blurs the distinction between an
absent answer and a failed transport. These are qualitative findings from the
actual answer, beyond the passing boolean fields.

The complete native arm took **1,617,231 ms**, versus **258,074 ms** for the
provider reference. It used the one provider repair described above. Two
native field passes plus one assisted pass do not establish semantic parity;
the observed runtime also falls short of the requested throughput.

Actual reports are collected privately in
`.hearth/response-parity/matched-session-comparison.md`, with separate native,
baseline and assisted answers. `matched-session-comparison.json` carries the
cost and time projection without answer content.

## Repair in the executing controller

`fcac-advance` previously injected a model observation before `fcac-loop`
noticed that the caller's reply allowance had ended. It now checks the
allowance first and passes the unchanged session to that existing loop.
Already-complete results keep their original path. Candidate state, budget
reason and release remain owned by the existing controller.

The source change follows the baseline's actual proposal. Its preflight and
native request validation pass. A fresh one-turn execution has the identical
request digest, report bytes, document bytes, stop reason, status and generated
count (**39**), with release successful in both runs. Unused injected IDs fall
from **437 to 0**. Native elapsed time changes from **167,594 to 85,134 ms**:
an observed difference of **82,460 ms**. Provider stages are excluded, and no
general timing ratio or answer improvement is inferred from this one pair.
The retained wrong answer still fails its original assertion.

Evidence: `turn-budget-reobserve-summary.json` and `turn-budget-proof.json`
under `.hearth/response-parity/`. The latter rechecks actual retained results,
including candidate preservation and release, rather than a synthetic model.

The running matched session was admitted before this source edit; its task
documents stay frozen. The follow-up observations are separate attempts.

## What this teaches

The useful surprise is how sharply correctness and token cost can separate:
a low-rental session can spend minutes defending a numeric misreading. The
difficulty yields a concrete next experiment: use the existing local reasoning
path on that same frozen request, retaining its answer and total local cost.
Its private reasoning is not exposed as a report or a training target.

The initial reasoning attempt reached its fixed **2,048-token** limit after
**473,628 ms**. The reasoning boundary was present and the visible final
channel had the correct numeric decision, but its next-action sentence was
cut off. Completion and report checks were therefore **0**, with release **1**
and provider calls **0**. The failed command was
`form-run ./fkwu .hearth/response-parity/matched-thinking-session.bml`, exit **1**:
`fkwu: form_error: thinking probe incomplete or failed; actual output retained`.

Its evidence is `.hearth/response-parity/open-thinking-session-99780-1789542375694`.
A separate attempt raises the initial reply ceiling to **4,096 tokens**;
the question and checks stay frozen. This changes the resource bound, not the
recorded verdict of the first attempt. The metadata-only framebuffer exchange
selects continuation of that owned re-observation; answer and reasoning text
remain outside the framebuffer.

That second attempt completed in **519,917 ms**, with **2,094 generated IDs**,
zero injected IDs, one reply, zero repairs, source/report checks **1**, release
**1**, and provider calls **0**. It correctly identifies every supplied loss
change as an improvement. Evidence:
`.hearth/response-parity/open-thinking-session-1258-1789542926242`.
Both attempts count: the earlier **473,628 ms** failed run remains part of
development cost. The successful run alone is not the entire experiment.

The numerical decision improved, while the next-action wording remains a
qualitative concern: it moves to a later promotion window, obtaining usage
and adding "at least two" held-out rows, without clearly taking the currently
supported promotion action. The source does not specify that number. The
baseline more clearly executes the supported decision and keeps further
evaluation separate. This is a coordinator assessment of the actual response;
the boolean field checks do not resolve it.

Initial reasoning remains an explicit native experiment, with its cost and
generation bound visible. No production chat default changed. The result
supports continuing that path; it does not establish whole-session parity.

The verified controller teaching was returned through the native session home
as event `verified-unused-feedback-elision-v1`, session
`matched-session-and-turn-budget`. Its contents concern the observed execution
boundary, not an assessment answer. Retention and any learning/promotion state
are observed separately; this does not claim a Qwen weight update.

Final controller checks: preflight clean, native request validation passed,
actual before/after preservation witness **1**, clean diff, drift **8191**.

Panel: **0 orphans**, **11/12 counsel lanes unobserved** with no standing hearth.
Neither that panel nor these assertions establishes whole-session quality,
resonance, throughput parity or a global minimum in rented tokens.
