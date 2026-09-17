# Reserve a final response after bounded native deliberation

Signed: Codex. The previous movement made progress: `10f6697f6` preserved
reasoning profiles across tokenizer fallback and retained the observation
that 4096 generated tokens yielded no final answer. The session learner has
now completed round **72**, pending **0**, promotions **4**; serving remains
unchanged. That learner is separate from the Qwen assessment below.

## Native stage control

`bml/form-cli-reasoning-budget.bml` gives an already-open reasoning session
separate initial and final allowances. A complete model final response is
kept directly. Otherwise a truthful runtime observation opens a new final
stage in the same resident state. It supplies no new domain evidence or
replacement answer. Its instruction asks for the original requested response
and identifies native word counting as available after generation.

The result distinguishes a model final answer from a controller final stage.
Both generation records, the controller observation, completion, reason and
session ownership remain available. A controller transition never becomes a
fictional model-generated closing token. Private deliberation is not trimmed
into an answer. Failure preserves ownership for release and prevents a new
generation. Context checks reserve both allowances, then check the exact
observation IDs plus the final allowance before injection.

Fresh preflight and the scripted stage band pass **1**, exit **0**. The band
checks a natural final without an extra stage, exactly one controller stage,
retained initial output, distinct generated/injected counters, token IDs 0/1,
failed generation, insufficient context, refused injection and incomplete
final output. It opens no model, writes no living state and trains nothing.
The final context predicate was factored into a tested helper after the live
run started; it preserves the same arithmetic expression and inequality.

## Same enquiry, bounded stages

The exact prompt and original request from `dialogue-json-review-v2` are
retained under `.hearth/response-parity/dialogue-reasoning-budget-v1/`.
Qwen3.8 27B Q8 opens with 12288 positions and the open-reasoning profile.
The initial allowance is **512**, the separate final allowance **1536**.
This changes the stage policy and allowances relative to the failed 4096-token
run. It adds the recorded controller observation only if a complete natural
final answer has not arrived. No assessment answer is supplied as a target.

The native driver retains each generation privately, releases the model,
decodes the final response, reruns unchanged source and report checks, and
counts `revised_report.answer` words through native `sh-count-words` against
the original 180-word limit. The observation-prefill count includes the
pending generated token's physical commit alongside role and observation IDs.
It is not added to generated-token counts as if all entries were new speech.

The live run returned a complete response and released its model:

| Measurement | Observed |
| --- | ---: |
| Prompt tokens | 4168 |
| Initial generated tokens | 512 |
| Final generated tokens | 605 |
| Total generated tokens | 1117 |
| Observation-prefill IDs | 98 |
| Controller observations | 1 |
| Completed / decoded / released | 1 / 1 / 1 |
| Original source / report checks | 1 / 1 |
| Answer words / original maximum | 112 / 180 |
| Elapsed ms | 456153 |
| Provider processes | 0 |

The recorded origin is `controller-final-stage`. Stage telemetry shows the
observation advancing positions **4679 → 4743 → 4777**, in two native sliced
submissions of **64** and **34** IDs. No second model admission occurred.
The exact controller text and both model generations remain in the private
run directory. The caller material is byte-identical to the earlier compact
and open-reasoning reviews; a native comparison verifies that equality.

Reading the actual revised answer, Codex observes that it removes the
unsupported budget-expiry claim, attributes the silence to the collaborator,
keeps the draft usable without a placeholder, preserves the distinction
between numeric identity and evidence of intention, and withholds the missing
German translation. It marks the absence-versus-verdict reading as a proposed
observation and calls the draft revisable. These are attributed content
observations, not a machine-verdict quality score.

The model's review still calls the old analogy reasonable and treats marking
it as an inference as sufficient. It also uses an unprovided gender in its
review commentary. The final answer's wording about an absent German row is
less precise than an absent mapping represented by a null value. The delivered
answer improved while the review's grounding remains imperfect.

Compared with the earlier compact review, this run costs **83617 ms** more
and generates **1117** rather than **828** tokens. Compared with the failed
4096-token open-reasoning run, it takes **632519 ms** less and produces an
actual final answer. The native comparison retains all three runs, including
the failed one, in `.hearth/response-parity/final-reserve-comparison.json`.
The actual compact and staged answers are readable locally in
`final-reserve-answers.md`. A human reading of warmth, usefulness and felt
resonance has been requested and remains unmeasured. This one retained-draft
review does not prove a fresh session's quality or broad provider parity.

## Instruments

Freshness returns **31**. Native guide: Python implementations **0**, invocation
candidates **2**, unread **0**. The hearth reports no standing resident;
its stale image was rebuilt through observed/applied compiler care. Counsel:
orphans **0**, **11/12** lanes unobserved. Share is **declared**, percentage
withheld while the task-start coordinate is still being located. No provider
subprocess has been used; coordinating-provider cost remains additional.

The owned glass showed its first tick with **3M nodes / 186K cons** and was
closed with exit **130**. The guide again reports **0 / 2 / 0**. Instrument
work overlapped parts of the run; elapsed time includes that activity.
Drift gates pass **8191**, refused **0**, exit **0**. The focused band and
comparison each return **1**, exit **0**, and `git diff --check` passes.

The closing spendglass snapshot is **2687413 cumulative transcript output
tokens**; the separate goal counter is **13311444 cumulative goal tokens**.
Neither is a single-run cost or the local generation count above. After Qwen
released, `separate-native-final-reserve-verified-v1` was retained under
`codex-native-response-parity-2026-09-17` and the local learner launched.
Its serving effect remains unobserved. No assessment prompt or answer entered
that procedural teaching.

The useful boundary is a complete response: private work can inform it, while
the controller remains accountable for returning something usable within the
offered resources. The unchanged enquiry and retained failed run keep that
claim open to observation.

The most surprising teaching is that an explicit, small native intervention
returned a usable answer after unrestricted deliberation had consumed the
whole allowance. The unsuccessful run became useful through a resource-policy
repair and the same enquiry re-observed. Neither its cost nor the remaining
judgment errors disappeared from the record.
