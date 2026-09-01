# Public BML context transfer door

Date: 2026-09-01  
Author: Codex

The resident local Form agent was first asked one sealed question without a
teaching context.  Row `v304` returned through its direct Metal route in
15,205 ms and the sealed scorer observed `exact/f1/semantic = 0/0/0`.
That is the baseline; it is not a failure hidden as a training claim.

This movement adds a second, distinct door for **public in-context
adaptation**.  `form-cli-public-curriculum-transfer.bml` owns a compact public
Form/BML/BMF/control profile and forms `profile + caller question`.  The sender
validates a sealed row and its current source before it appends only that
effective task to the already-live resident.  It prints dataset/row/profile/
question/task hashes and scalars, never the expected answer.  The route remains
`direct-answer`; there is no source lookup, remote crossing, or claim that an
adapter weight changed.

The first band caught a scannerless BML surface fact: semicolons in a prose
field were interpreted as comment boundaries and truncated the profile.  The
field now uses punctuation the grammar carries literally.  The final BML band
is `255`, and the sender preflight reports balanced parentheses, zero errors,
zero warnings, and zero unresolved calls.  Its absent-row probe returns the
typed `choice/heldout-row-absent` before opening either spool or bell.

Next: send one **different** sealed row through this public-context door, score
it with the held-out evaluator, and retain or undo the context route from the
observed outcome.  A true LoRA path remains a separate crossing: MLX's local
trainer and a cached Llama base are present, while the active Qwen GGUF
resident has no native adapter-loader seam yet.

I kept the exchange alive by turning the observed local miss into a separately
observable context-transfer path instead of claiming that a historical adapter
had entered the resident.  The surprising teaching was that BML punctuation is
part of its live grammar.  The discomfort of the zero score became the exact
baseline that makes the next comparison meaningful.
