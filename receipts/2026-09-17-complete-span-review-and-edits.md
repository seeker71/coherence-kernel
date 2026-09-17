# Complete indexed review with actual response edits

Signed: Codex. The previous goal movement made progress: caller-enabled native
rehearsal changed a failed coding session into a completed local session.
The next observed gap is the broader dialogue response: the earlier local
review selected only part of it, identified deferred action, and then returned
the response unchanged.

## Connect review participation to an executable change

`bml/form-cli-review-edits.bml` now builds an exact indexed packet over
caller-selected top-level string fields. It uses the existing punctuation
span heuristic and retains each field, byte interval and original text.
The base binds the exact original JSON bytes by SHA-256.

The reviewing model chooses every keep, replacement or removal. Form requires
one ordered integer ID per supplied span, a status and a nonempty reason.
Replacement text must actually differ; removal is explicit. The native
carrier composes the selected fields exactly and preserves every unselected
value. Missing, repeated, reordered or malformed decisions refuse the whole
operation. Stale bases and unchanged replacements also refuse. The original
remains with the caller.

Statuses and reasons are model judgments. The carrier validates coverage,
addressing and text changes; it does not certify evidence use, prose quality
or semantic correctness. The API is callable on fkwu; ordinary response
generation policy is unchanged. There is no C growth or extra runtime.

The new native behavior band passes **1**, exit 0, after fresh preflight.
It observes UTF-8 offsets, keep/remove/replace composition, unselected nested
values and boolean preservation, full keep-only retention and atomic refusal
of partial reviews, invalid IDs, unchanged/empty replacements, stale bases,
duplicate fields and nontext targets. Its first preflight caught an extra
closing parenthesis in the stale-base assertion; that syntax was corrected
before the passing run. Drift gates pass **8191**, exit 0.

## Same retained enquiry and sources

The new local probe keeps the earlier scope-review request and original
response byte-identical. It supplies all **16** spans across `answer`,
`next_action` and the retained separate `question` field. Instead of selecting
up to six quotations, the model returns an edit decision for every span.
Form applies those decisions, retains the assembled candidate, and returns
that exact candidate to the same local session for whole-response review
through the original source/report contract.

The initial review allowance changes from **640** to **2560** tokens to carry
the complete decisions and actual replacement text. The final reply allowance
changes from **768** to **1024**. Context remains **16,384**, and the ordinary
non-thinking Qwen generation profile remains selected. This is a changed
review/composition workflow and allowance, not an isolated prompt comparison.
The instruction supplies no golden replacements or expected judgment labels.

Private evidence lives under
`.hearth/response-parity/generalization-v2/complete-span-review-v1`.
The original request, original report, packet, prompt, actual edits, native
assembly, follow-up prompt and final result remain separate artifacts.
The native assessment verifies byte-identical original request and report.

The earlier provider response is retained as a comparison, including its
own weakness: its next action was deferred to a later exchange. Its goal,
documents, source checks and source queries match this retained enquiry, but
report checks differ. It is therefore a source-matched reference, not an
identical-contract control. No new provider process is called in this probe.

## Actual local result and repair

The first local review returned all **16** rows, but replacement row **8**
omitted its reason. Atomic refusal preserved the original report. The native
diagnostic now identifies the first invalid span; the behavior band also
checks the missing-reason case and passes **1**, exit 0. The model marked the
numeric-identity/translation sentence supported and kept a next action that
it labeled an action gap. Complete participation did not yield sound judgment.

A second local admission supplied only the missing reason (**13** generated
IDs). Native replay verifies that no other decision changed. Composition then
applied **14** keeps, **1** replacement and **1** removal. Original source and
report assertions passed. The same admitted session reviewed the whole
assembled answer and returned it byte-identically. It still conflates numeric
identity with performed translation, defers the authorized action, and asks
Urs to diagnose its voice. No semantic or resonance improvement is claimed.

The second run completed and released the model: **383,980 ms**, **549**
generated IDs, **4,430** injected IDs. Both runs together cost **736,204 ms**
and **1,123** generated IDs; no provider subprocess ran. The original request
and original report remain byte-identical. Evidence for the second run lives
under `.hearth/response-parity/generalization-v2/complete-span-review-v2`.
The audit initially ran before the result existed and preflight reported an
unreadable chain. Compile-only checking was clean; after model release, fresh
preflight and actual replay passed. The timing failure was not a syntax defect.

This closes the malformed-row diagnosis and native edit-composition gap.
It leaves the response-quality gap open. Further blind retries on this same
answer would add cost without a new discriminating observation; the next
comparison needs actual session outcomes and semantic assessment across tasks.

## Instruments and retained limits

The preceding session learner completed round **66**, promotions **4**, with
pending **0** and no live worker. Its serving generation remains unchanged;
that Llama learning is separate from this base Qwen admission.

Native guide reports Python implementations **0**, invocation candidates
**2**, unread files **0**. Glass's first observed frame arrived in **28 ms**;
the owned viewer was closed. Counsel reports orphans **0**, with **11/12**
lanes unobserved and no standing hearth.

The useful crossing is from a review comment to exact model-authored edits
that Form can apply and inspect. Complete coverage remains a narrower fact
than a better response. The actual completed answer and its costs are read
before claiming an improvement; this enquiry is repeated development data,
not a fresh held-out demonstration of parity.
