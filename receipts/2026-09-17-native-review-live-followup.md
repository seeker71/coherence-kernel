# One native admission carries review and correction

Signed: Codex. Witnessed 2026-09-17 on `fkwu`.

`form-cli-review-followup.bml` gives native embedding callers one bounded
follow-up before a review releases its model. The first checked report and
source observations remain in the live context. The original checker, source
boundaries, context capacity and total turn allowance still govern the second
pass. Both reports remain available to the caller. Ordinary review is unchanged.

The caller supplies the feedback. This is same-model continuation, not an
independent reviewer or an automatic semantic-quality verdict. A previously
completed state without its live context cannot satisfy a new follow-up.

## Observed execution

The retained comparison used the same initial prompt as the previous review.
Its first report matched the previous report byte-for-byte. Feedback then asked
the model to apply supported findings in the full answer and next action.

| Route | Elapsed ms | Generated IDs | Injected IDs |
| --- | ---: | ---: | ---: |
| Earlier separate review and revision admissions | 915775 | 1107 | 3519 |
| Review and follow-up in one live context | 757208 | 1158 | 3802 |

Native arithmetic observes **158567 ms saved, 17% rounded down**. This is one
comparison: the second instruction and retained context differ between routes.
It does not isolate a general speed effect. The continued session generated
more tokens; avoiding another admission still reduced this observed total.

The follow-up observation was admitted, check count advanced from 1 to 2,
the final state completed, original source/report checks passed, documents
were unchanged, and model release returned 1. There was one source tool read
and zero provider processes in this experiment. Parent coordination remains
additional rented work; zero provider processes does not mean zero total cost.
The parent goal meter moved from 7,537,451 tokens at movement start to
7,703,696 at the post-check checkpoint. The transcript output meter read
1,604,790 cumulative output tokens. These are different measures; neither
supports calling the overall work a minimal-token result.

## What changed in the answer

The next action changed. The answer removed a deferred source read, the
unsupported fear-costume/nothing/composting mapping, and the phrase claiming
not to pretend to understand. It used the existing arrival observation in its
proposed action.

It still described the architecture defensively, overstated persistence of old
shapes, and relied on abstract declarations of connection. Its concluding
`review_findings` array was empty despite these remaining weaknesses. Checking
the fields and completing another generation did not establish that the
criticism had been fully applied. Response quality and resonance parity remain
open. The new door reduces the cost of an observed correction; it does not
certify the correction's completeness.

## Checks and retained limits

The boundary band preflight was clean; its executed verdict was 1, exit 0.
It covers review-only admission, absent feedback, incomplete first review,
shared turn exhaustion, closed-context refusal and unavailable-model handling.
The live trial separately witnesses successful context retention and release.

The private comparison helper initially failed: preflight executed it before
the result files existed, producing `str_len: nothing has no length`. Adding
its effectful compile-only marker made preflight clean; it ran successfully
after the result existed. The adverse attempt remains in the session trace.

Native authoring guide: 0 Python implementations, 2 existing voice invocation
candidates, 0 unread files. Drift gates returned 8191, exit 0. The verified
procedural teaching was retained under event
`2026-09-17-native-review-live-followup-v1`; assessment answers were excluded.
The worker launched, with completion still pending at this receipt checkpoint.
This learner is Llama, not the measured Qwen model.

Counsel panel: **orphans 0**; 11 of 12 lanes remained unobserved because no hearth
stood. There is no all-good performance claim. Private evidence lives under
`.hearth/response-parity/generalization-v2/resident-followup-v1`, including both
reports, frozen prompt and feedback, result, receipt and comparison audit.

The useful surprise was exact reproduction of the first report followed by an
actual correction in its retained context. The uncomfortable observation was
an empty findings list beside unresolved weaknesses. Keeping those together
makes the next improvement assessable without dressing partial progress as
completion.
