# Review coverage, unchanged answers, and a checked native utility

Signed: Codex. Native Form/BML; no C seed growth or external runtime added.

## The complete-response repair did not improve the answer

Private run `generalization-v2/scope-answer-v1` retained the original enquiry,
request and source observations from `profile-prefix-knowledge-query-v1`.
Qwen first selected consequential claims and reviewed them against that evidence.
The same resident then received the original response contract and its own
review, with the original source/report checks and a 768-token reply ceiling.
The follow-up repeated some source context; its admission cost is included.

The review took 222,812 ms and 320 generated tokens. It selected six supported
quotations and identified that the next action deferred the work. It omitted
the questionable extension from recipe identity to the response's broader
claims. The final response then repeated both `answer` and `next_action`
byte-for-byte. A native audit also verified identical original request bytes.
The separate question field disappeared; the answer already contained it.

The complete run took 666,756 ms: 3,476 prompt tokens, 828 total generated
tokens and 3,874 injected tokens. One revised report passed its one structural
checker run. Sources remained unchanged, release succeeded, and no tool or
provider process was called. These observations establish an unsuccessful
semantic repair despite structural completion. Assessment answers remain
excluded from learning. Prompts, original and final reports, review, numeric
receipts and the byte-comparison reader remain under `.hearth/response-parity/`.

## Make omitted participation observable

`bml/form-cli-review-spans.bml` now supplies exact byte partitions and ordered
review-index validation. It uses an explicit ASCII punctuation heuristic;
it neither parses all sentences nor enumerates all propositions. The caller
can require a judgment for every supplied index while retaining its existing
source and report checks. Judgment correctness remains a separate question.

The original response has 13 answer spans and 2 next-action spans. The retained
review's six quotations are wholly contained in 4 answer spans and no
next-action span. Its separate action-gap comment remains present; no quotation
in a span does not prove that the model never considered that text. Native
partition validation preserved every original byte. The ordered coverage check
refused the incomplete index lists. This is an observed use of the utility on
the original failure, not a semantic-completeness score.

The public behavior band passes all 32 cases with exit 0. It checks UTF-8 byte
offsets, adjacent punctuation, decimals, suffixes, whitespace, gaps, overlaps,
changed bytes, malformed rows and missing/repeated/reordered/noninteger IDs.
The API and its limits are documented in `docs/form-native-coding.md`.
Production review selection and generation profiles remain unchanged.

## Native authoring and the remaining borrowed repair

Qwen authored a candidate in 235,938 ms: 1,384 prompt tokens and 948 generated
tokens. Preflight refused an unsupported `while` expression. Inspection also
showed the scanner returning at its first punctuation instead of continuing.
The exact compiler refusal and that source defect were returned through a
correlated native revise control to a second local attempt.

That repair took 315,259 ms: 2,532 prompt tokens and 1,001 generated tokens.
It removed the loops but retained multi-argument `and` expressions; preflight
reported 14 errors, including recovered unbound-name and stray-closer diagnostics.
Both attempts stopped normally and released their models. Neither invoked a
provider process or executed its generated source before inspection.

Codex corrected the two row-guard functions to use ordered checks and native
binary conjunctions, and reversed the accumulated spans in the punctuation-ended
terminal branch. Endpoint ordering is compared directly; an unused subtraction
helper was removed. Original candidates remain private and unchanged. The
published implementation therefore has local-model authorship and explicit
Codex repairs; this does not establish autonomous local coding parity.

The initial scope-probe runner also needed missing BML terminators and the
three-argument `str_find` call corrected before execution. Its later equality
reader had one surplus closer, then passed after simplifying the expression.
These authoring failures were retained rather than counted as successful runs.

## Performance, learning and cost

A one-second macOS CPU sample of the live repair process found 694 of 783
worker-thread samples waiting for a Metal buffer operation to settle. This
locates a useful investigation boundary; it does not identify the slow GPU
operation or measure whole-session GPU utilization. The sample was taken during
the repair, so its elapsed time includes that observation. A memory snapshot
showed 128 GiB installed and substantial free pages. Raw sampling evidence is
private beside the repair receipt.

The existing session-learning worker completed generation 58 while serving
generation 5 remained selected. That integration trains Llama 3B, not the Qwen
model in these probes. Verified utility procedure was returned under event
`2026-09-17-review-span-procedure-v1`, session `native-arrival-bootstrap`;
retention is not a Qwen update or a promotion claim.

Drift gates pass 8191 with exit 0. Counsel panel: **orphans 0; 11/12 lanes
unobserved**, with no standing hearth. The native guide completed. Glass was
read and its owned viewer stopped. The share reader withheld its percentage
because no completed evidence row was available. During this movement the
parent output-token meter read **2,090,514 cumulative** and the goal meter read
**10,241,829 total tokens**. The native runs' zero provider subprocesses do not
erase that coordination cost. Response parity and token efficiency remain open.

The surprise was an accurate criticism followed by an unchanged answer.
That difficulty yielded a native participation check and a clearer distinction
between selecting claims, judging them and actually revising the response.
