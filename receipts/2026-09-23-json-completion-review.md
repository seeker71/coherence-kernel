# Check the execution boundary before changing the completion boundary

Codex, 2026-09-23. A Form-owned review of the landed JSON continuation returned
two findings. Native source restoration and executed counterevidence resolved
both. No runtime change followed an unsupported finding. The local answer-edit
task continues independently; its quality and useful progress remain open.

## Actual review, correction and result

The [first packet](artifacts/2026-09-23-json-completion-review/manifest.json)
contained nine sources, including the generator, model stream, callers,
JSON parser and boundary checks. It omitted the definition of `fcap-step`,
the final action-admission boundary. The provider also returned `summary`
instead of the requested `answer`, so the original report check failed.
The [failed observation](artifacts/2026-09-23-json-completion-review/first-observation.json)
and [answer](artifacts/2026-09-23-json-completion-review/first-answer.json)
remain intact.

The first finding treated a stopped but invalid continuation reaching
`fcac-replied` as defective completion. The second treated the strict context
guard as refusing a reachable exactly fitting continuation. Neither finding
had followed the complete caller contract.

The native [care program](artifacts/2026-09-23-json-completion-review/care.bml)
uses `frcc-prepare` and `oc-hear` to supply the omitted action-admission source
and the reservation source, then re-read their coverage. It also executes
the actual retained 549-byte unfinished command through `fcap-step`:
**zero tool calls, zero document changes**, syntax repair at end of input.
The reservation arithmetic returns false for an exactly full context. That
guard precedes the final generation stage. The
[counterevidence](artifacts/2026-09-23-json-completion-review/counterevidence.json)
distinguishes the actual action reconstruction from this boundary arithmetic.
No local model was admitted for either reading.

The [informed review](artifacts/2026-09-23-json-completion-review/after-answer.json)
returns the required report shape and no supported correctness findings. Form
reran the unchanged source and report assertions; both pass, and release is
verified. This supports leaving the implementation intact. It does not prove
all runtime behavior or improved model answers. The useful correction was
supplying the actual action-admission boundary before interpreting a generation
completion flag.

The provider calls the unchanged live result an unobserved quality result.
That wording is too broad: failure to change the answer and failure to meet
the word range are observed. The review resolves the implementation findings;
it does not erase those adverse task results.

## Cost includes the failed call

The [native aggregation](artifacts/2026-09-23-json-completion-review/cost.json)
records **87,800 provider tokens**, including **21,248 cached input tokens**,
across two Form-owned calls and **87,958 ms** of provider execution windows.
The first call used 43,465 tokens; the second used 44,335. Both released,
neither used tools, and neither entered training. The synthesis compiler's
stale-cache warning was retained; its own health flow rebuilt the image.

The [previous completed coordinating turn](artifacts/2026-09-23-json-completion-review/previous-turn-cost.json)
used **3,826,558 tokens**, including **3,758,976 cached input tokens**;
24 model calls and 23 tool calls reconcile, with zero unattributed tokens.
It is a different movement from this review. The open coordinating turn is
excluded from both figures; these execution-window costs establish no overall
saving or quality parity against a matched baseline.

## The native response gap remains

The same live PID 28854 remains owned by the answer-edit continuation. Its first
two public actions are byte-identical unchanged edits. Its third action changes
the actual answer from **478 to 477 words** and triggers the original failing
word-range check. These complete JSON replies have not exercised the new
unfinished-string continuation. Later replies remain in progress at this
receipt. No new local admission or restart was made for the review.

Clean preflights and successful native execution cover preparation, context
care and cost aggregation. Drift gates return **8191**, exit 0.
The native guide reports 0 Python implementations,
2 invocation candidates and 0 unread. Glass first frame is **28 ms**; its
viewer was intentionally interrupted with Ctrl-C, exit 1. Counsel reports
0 orphans and 11/12 unobserved lanes. The previous completed turn's observed
boundary-event share is native 6, local 49 and remote 45 percent; this counts
events, not semantic contribution or the open turn.

Verified admission-boundary teaching
`449f25c6ab56ecc5cd657d48616eabf6d2a3097f56d394c0522fa1f744ac8976`
is retained under event `2026-09-23-json-review-admission-boundary`. Its later
learned use remains unobserved. Provider review prose was not used as a target.
