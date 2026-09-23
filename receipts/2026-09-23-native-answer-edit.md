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

The process finished prefill of 5,345 IDs and made two effective edits. The
[completed-action reconstruction](artifacts/2026-09-23-native-answer-edit/observed-stages.json)
applies the first eight retained native replies through the same policy and
checker over the original documents, without generating another answer or
writing source documents. Its [native reader](artifacts/2026-09-23-native-answer-edit/answer-edit-observe.bml)
is retained. These are deterministic reconstructions, not snapshots of the
running process's heap.

| Actual action | Result |
| --- | --- |
| Original draft | 547 words; false unconditional lifetime claim |
| First edit | 560 words; referenced cells persist, unreferenced cells compost |
| First verification | Word range refused; repair entered |
| Second edit | 549 words; lifetime correction retained |
| Second verification | Word range refused; repair entered |
| Third edit | Identical 549-word document; `edit-unchanged` refused it |

The [first](artifacts/2026-09-23-native-answer-edit/candidate-after-2.txt) and
[second](artifacts/2026-09-23-native-answer-edit/candidate-after-5.txt) candidates
retain the model-token/graph distinction and French `offrait`. They still read
largely as a glossary, retain redundant detail and end by asserting a resonance
effect that has not been observed in the person. The lifetime repair is real;
these findings leave answer quality open.

The model correctly diagnosed the 99-word excess before its unchanged third
edit. Additional numerical feedback alone is therefore not an established
repair. The hash-guarded edit syntax was valid: Codex initially misread that
action as a tool-shape failure, then corrected the interpretation after reading
the implementation and native replay. The actual gap is carrying a diagnosis
into an effective revision. The ninth reply acknowledges the unchanged edit;
the tenth again returns the identical text, then the model replans.

The [final result](artifacts/2026-09-23-native-answer-edit/result.json) reaches
the original 12-reply limit with `status=attention`, 7 tool calls, 4 repairs and
2 verification runs. Its [compact report](artifacts/2026-09-23-native-answer-edit/report.json)
records **3,970,346 ms** elapsed, **3,642 generated IDs**, **3,706 injected IDs**,
zero offered provider calls and verified model release. The [returned answer](artifacts/2026-09-23-native-answer-edit/answer.txt)
is still **549 words**, with the lifetime correction retained. Process exit 0
establishes a returned result, while the failed word range and unfinished task
remain visible. Replanning consumed the final reply; it did not complete the
requested answer. The [next runtime repair](2026-09-23-continuing-native-reasoning.md)
opens bounded reasoning across the task and returns to the unchanged request.

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

The preceding memory repair now has a completed full-size update. The
[native observation](artifacts/2026-09-23-native-answer-edit/learning-gradient.json)
and its [reader](artifacts/2026-09-23-native-answer-edit/full-gradient-identity.bml)
establish that the original **5,485-token**, 2,502-supervised-token example was
the third row of a real three-row batch. Its parsed example and canonical
serialization hash match the original failed row. Its explicit retry remains
queued; this batch independently selected the same example for replay.

The original whole-tape estimate was **341,213,194,880 bytes**. Layer
recomputation estimated **17,135,995,660 additional bytes**, completed its full
forward pass in **365,554 ms** and backward pass in **266,781 ms**, and contributed
to published optimizer step **149**. The [checkpoint observation](artifacts/2026-09-23-native-answer-edit/learning-checkpoint.json)
verifies positive adapter and optimizer byte sizes against the actual files,
112 adapter tensors, 224 optimizer tensors and all three examples consumed.
The full batch updated 56 LoRA pairs over 11,528 input and 5,292 supervised
tokens. This closes the observed memory refusal for the original example in
this batch.

Context changed: the parent was generation 148 and the update combined three
rows. It is not an isolated comparison at the original parent. Validation loss
rose from **3.37489495575428** to **3.3768423721194267**; full after-assessment and
serving promotion remain pending. The memory estimate is not measured peak
allocation. Training adapts Llama 3.2 3B, separately from native Qwen editing.
The two GPU workloads overlap, so timings are not isolated. Execution progress
does not establish better answers or response parity.

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
