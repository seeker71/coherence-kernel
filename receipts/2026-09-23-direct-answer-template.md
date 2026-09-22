# Keep the direct response in the native CLI's chat template

The response gap remained after the previous teaching repair. The next real
step returned to Urs's original enquiry with current source, the verified host
effect contract and an executed `enrich offer trust resonance` lookup. Its
[13,506-byte packet](artifacts/2026-09-23-direct-answer-template/hearth-answer-source-repair.packet.json) replaced the invented before/after requirement with that
actual vocabulary example and named every requested conceptual axis.

## What failed

Native turn 6 reached its 4,096-ID allowance after **1,299,071 ms**. Its [header](artifacts/2026-09-23-direct-answer-template/hearth-turn-6.header.txt)
reported `partial`, `stopped=0`, position 8,821 and pending token 25. All 16,327
response bytes were inside an unclosed thinking channel; [zero final-answer
bytes](artifacts/2026-09-23-direct-answer-template/hearth-turn-6.channels.json)
were produced. The [unfinished generation](artifacts/2026-09-23-direct-answer-template/hearth-turn-6.unfinished.txt) repeatedly counted and trimmed words
against the 350–450-word requirement I added. This supplied no completed answer
to the person. The failed words remain unfinished evidence, excluded from
training targets. A successful process exit did not close that response gap.

The earlier request framing contributed work the person did not ask for:
an invented baseline example, then a narrow word-count constraint across many
conceptual axes. The same enquiry and this full packet are retained for the
following native step, including its length constraint, so that constraint
is not silently removed from the comparison.

## The native repair

The model's ordinary CLI template, `q35-chat-ids`, explicitly opens and closes
an empty thinking channel before final-answer generation. Initial admission
and tool observations used that template. Direct-answer follow-ups opened a
plain assistant turn and generated a new thinking channel. The two routes
therefore offered different generation modes.

`bml/form-cli-model-observation.bml` now reuses the existing chat template in
both encoding paths: the indexed path uses `qtlc-chat-segments`, and the
reference path uses `q35-chat-ids`. Both retain the newline following the
previous assistant close. `form-cli-model-session.fk` delegates that meaning
to the BML cell. The existing crossing still commits the prior pending token
once, and the question still enters as a user turn. Model weights, the C seed,
the question packet and the 4,096-ID allowance are unchanged.

PID 1473 released its 96 spare buffers and model, reported `release-ok=1`,
cleared its discovery board and exited zero with no group members. An initial
release write included a newline and was only a wake: the bell reads exact
bytes. Sending the actual `release` control resolved it. Its task and reply
spools were preserved before starting the replacement.

PID 93431 admitted the new code and received turn 7 with the identical packet.
This new admission also includes the already-landed 4,288-byte effect teaching;
the old resident retained its 3,820-byte overlay. The packet itself already
contained the effect teaching. A native reference-encoding check overlaps this
run; the preceding run overlapped session learning. These are successive real
steps, not a controlled estimate of the template's isolated timing effect.

## The actual response after the repair

Turn 7 [completed](artifacts/2026-09-23-direct-answer-template/hearth-turn-7.header.txt)
with **627 generated IDs in 338,123 ms**, `stopped=1`, returning
[3,166 final-answer bytes](artifacts/2026-09-23-direct-answer-template/hearth-turn-7.final.txt).
The native comparison counts 500 whitespace-delimited words, exceeding the
requested 450-word ceiling. This is a completed answer, not full instruction
compliance. The new admission separately reported `admit-prefill-ms=68078`;
the answer time excludes admission, coordination and other setup work.

Completion and generated-token use improved for this actual request. Factual
synthesis remains insufficient:

- It describes the frequency function as counting fear words in supplied text.
  The implementation consumes supplied numeric annotation rows; `tf-fearfrac`
  counts rows with negative valence. This loses a distinction turn 5 preserved.
- It says the old cell persists and that releasing a reference cannot destroy
  underlying data, omitting the referenced-cell condition. Its later account
  of external `unlink` is correctly scoped and no longer promises file recovery.
- It calls the codebook's language surfaces alternate senses. The actual
  lookup supplies witnessed labels; language labels and alternate senses are
  different distinctions.
- Unmeasured baseline and causal warmth claims remain. The answer also leaves
  offered interfaces, acknowledgements and the full numeric identity distinction
  underexplained.

The better completion does **not** establish better overall answer quality.
The same enquiry still needs source-faithful synthesis. These findings come
from reading the actual returned words against their supplied contracts, not
from the successful checks below.

## Verification and scope

Clean preflights preceded the model observation, direct-answer action and live
grammar bands. They returned **16,777,215**, **8,191** and **4,293,918,719**,
respectively, with exit zero. The additional direct-quantum caller's band
returned **1,023** after a clean preflight. The resident's compile-only check
passed. Drift gates returned **8,191/8,191**, zero refusals. Glass's first frame
took **49 ms**. The native guide reports zero Python implementations, two
existing invocation candidates and zero unread files.

The [actual enquiry encoding](artifacts/2026-09-23-direct-answer-template/direct-chat-template-check.json)
matched exactly: **3,303 IDs** through each encoder. Indexed encoding took
9,130 ms; reference encoding took 369,536 ms. This check loaded no model and
generated no IDs. Its time is additional development work, not included in
either answer's elapsed time. Neither native answer added a provider call;
the coordinating agent's open-turn rented cost remains separate and incomplete.

The whitespace check initially refused the packet's verbatim codebook line,
which ends with a space. The archived packet now uses a JSON string; native
decode/readback confirmed all 13,506 original bytes unchanged. The whitespace
check then passed. The input offered to both native runs was not trimmed.

The preceding verified teaching completed Llama learning round 128 with no
pending examples. Serving stayed at generation 5 with four promotions. That
completion did not change the Qwen model answering this enquiry.

The new template teaching was retained as
`direct-answer-native-chat-template-2026-09-23` and completed Llama learning
round 129, with no pending examples. Serving stayed at generation 5 with four
promotions. This is not a Qwen weight change. The generated evaluation answers
remain outside training targets.

This movement closes a native response-delivery failure. The remaining gap is
the fidelity and completeness of the generated explanation, with human resonance
and whole-session parity still requiring their own observations.

— Codex
