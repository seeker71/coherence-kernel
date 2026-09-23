# Put the actual answer's word range inside native editing

Codex, 2026-09-23. The preceding shortening instruction returned all 547 words
unchanged. This movement gives the native editor a numerical completion check
and the actual draft and sources to revise. Answer quality remains a separate
observation from the check.

## The executable change

The public `code` request now accepts a document assertion:

```json
{"kind":"word-range","path":"answer.md","minimum":350,"maximum":450}
```

It reads the current resident document using the ordinary response path's
existing `fal-count` counter. Both bounds are included, headings count, and
the basis is ASCII whitespace. A missing document fails separately from an
empty one. The four-field contract admits ordered, nonnegative integer bounds
and provides no alternate input text. It is a source-document check, not a
read-only report tool.

The existing `verify`, implementation completion and final acceptance paths
run the check. Failure returns the observed count and required range through
the existing repair flow. Source checks after it still run. No answer is cut,
rewritten or marked semantically correct by the counter. The change is BML,
authored by Codex, with no C seed or external runtime change.

## Actual work admitted

The [request](artifacts/2026-09-23-native-answer-edit/request.json) carries the
unchanged original enquiry/source packet and the retained native draft. Only
`answer.md` is writable. It asks native Qwen3.8 27B Q8 to correct the false
“referenced or not” lifetime claim, preserve all six axes and produce a useful
350–450-word answer. It selects the public direct editing path, a 16,384-position
context, at most 12 replies and 1,536 tokens per reply. Evaluation mode excludes
the answer from training.

The new check reproduces the actual failure at **547 words**, as retained in
[the baseline observation](artifacts/2026-09-23-native-answer-edit/before.json).
This is a different response interface: native document tools and numerical
verification are available throughout the editing task. It is not a controlled
comparison of wording alone.

The process finished prefill of 5,345 IDs, read the draft, applied an edit and
called verification. Its first verification failed and it entered repair:
three replies, three tool calls, one check and one repair were observed. The
native task is still running; no completed revised answer or quality result is
claimed in this initial receipt. The process is PID 85407, owned by the native
helper retained beside the request. The final result belongs beside this
observation when it arrives.

A [separate Codex revision](artifacts/2026-09-23-native-answer-edit/codex-answer.txt)
of the same draft is **407 words** and passes the same check. Its
[record](artifacts/2026-09-23-native-answer-edit/codex-report.json) names the
source and hash. Codex also has the full thread context. This answer was not
given to the native editor or offered as a training target. Length alone
establishes no judgment of its meaning or felt resonance.

## Checks and care

`form-cli-code-request-band.fk` has clean preflight and returns **255**, exit 0.
It covers inclusive bounds, UTF-8 words, ASCII whitespace and headings,
missing versus empty source, invalid bounds, retained subsequent assertions
and the actual implementation-completion transition back into repair after an
overlong edit. Full source-backed CLI compilation passes.

The first completion-path check failed because the authored fixture used
`write` against an existing document. Native tools returned
`document-exists-use-guarded-edit`; the fixture now uses the offered exact-text
`edit` operation. Its assertion checks the actual count in the model-facing
failure observation. The production edit and completion boundaries remain
intact. A temporary diagnostic print was removed after resolving that failure.

The preceding memory repair is also advancing through real work: learner PID
85268 completed all 28 forward layers of a **5,868-token** queued row in
**461,038 ms**, using layer recomputation. Its backward pass and update remain
ahead. This is another retained example; the original 5,485-token retry is
still queued. Neither this training progress nor the new assertion establishes
response parity. Training here adapts Llama 3.2 3B, separately from native Qwen
editing. The two GPU workloads overlap, so their timings are not isolated.

Glass first frame: **35 ms**, followed by intentional Ctrl-C, exit 1. Counsel:
**0 orphans**, 11/12 serving lanes unobserved because no hearth stands. The
kernel question returned `no-standing-hearth`; native tools remain available.
The landing checks returned **8191**, exit 0, with no kernel source moved;
`git diff --check` passed. The native guide reads 0 Python implementations,
2 invocation candidates and 0 unread files.

The verified assertion teaching was retained through the public embody door as
`57282dcad1592c6e40fe24695c97437b832878aef3dd9f6c50ff4105c5cf1382`, event
`2026-09-23-native-edit-word-range`. Its text distinguishes numerical
verification from semantic quality. A later learner update must establish
whether it was learned or promoted.

## Cost boundary

The [preceding completed coordinator turn](artifacts/2026-09-23-native-answer-edit/preceding-turn-cost.json)
consumed **9,618,647 rented tokens**: 9,341,312 cached input, 190,935 uncached
input, 59,896 output and 26,504 unattributed tokens. Reasoning is a subset of
output. Its 63 model calls and 61 tool calls reconcile. This includes the
previous implementation and coordination; it is not a price for the 407-word
revision. The current turn, including that revision and ongoing coordination,
can be measured after it completes. The native editing request offers no
provider call. No cost or overall quality parity is established.
The separate output-only session meter reads **5,452,929** and excludes input.
