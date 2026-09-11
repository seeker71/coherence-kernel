# BML learning meets execution

Three native LoRA updates completed. Their practice losses improved, but the
three unassisted test answers remained byte-for-byte identical. Neither code
proposal compiled. This movement establishes a working execution-to-learning
path and a measured ability gap; it does not establish BML competence.

The [native practice door](../docs/native-bml-execution-learning.md) now
prepares executable references, freezes distinct test compositions, generates
with an explicit adapter, checks exact proposals, retains every observation,
offers verified practice to session learning, and reads actual training stages.
Additional conditions test all-example context, compiler feedback and focused
native RAG selection. All implementation and measurement helpers used Form/BML.
No Python interpreter was used and the C seed was unchanged.

The [machine-readable measurements](2026-09-11-bml-execution-learning.data.json)
were exported by native Form from the retained results. They include every
measured learning row, gradient token allocation, GPU timings, checkpoint
timings, generation condition, response hash and stop reason. Full private
prompts, responses, source, compiler diagnostics, retrieval alternatives and
events remain at `.hearth/bml-practice-2026-09-11-v1/` in this worktree.

| Optimizer step | Primary practice | Primary loss before → after | Input / supervised tokens in the batch | Trainer elapsed |
| --- | --- | --- | --- | --- |
| 11 | Repair integer product | 0.470594 → 0.464541 | 421 / 156 | 156.499 s |
| 12 | Complete first-present declaration | 0.553591 → 0.536646 | 459 / 136 | 173.054 s |
| 13 | Predict absence and present zero | 3.238694 → 3.165596 | 459 / 136 | 213.644 s |

The native Llama-3.2-3B-Instruct-4bit trainer updated all 56 LoRA A/B pairs
each round and continued the candidate's Adam state. Total gradient input was
1,339 tokens, including 428 supervised tokens across nine sequence microbatches.
Trainer elapsed time totalled 543.197 s, of which the training stages took
325.386 s. Recorded forward GPU time was 63.146478 s, backward GPU time
257.112354 s, and checkpoint time 2.444 s. These trainer timings exclude the
session supervisor's separate serving-baseline preparation. They are observed
costs under concurrent host work, not a controlled hardware-floor benchmark.

Every round improved its primary practice loss and increased one held-out
sentinel's loss. Candidate generation advanced from 10 to 13; serving generation
4 stayed selected. No promotion was forced. The worker completed with no pending
examples, no refused examples, and zero retained trainer buffers. This trained
the native Llama adapter, not Qwen or the external authoring model. The examples
entered through the caller-teaching route; no new code-serving capability was
admitted by this movement.

| Generation condition | Correct prediction positions | Code proposals compiling | Complete task passes | Generation elapsed |
| --- | --- | --- | --- | --- |
| Candidate 10, unassisted | 3/6; wrong array length and format | 0/2 | 0/3 | 75.808 s |
| Candidate 13, unassisted | 3/6; identical response bytes | 0/2 | 0/3 | 194.845 s |
| Candidate 13, all three verified examples supplied | 0/6; valid JSON with wrong values and shape | 0/2 | 0/3 | 127.594 s |
| Candidate 13, combined compiler feedback and examples | No correct array; off-task stream cancelled at 225 tokens | 0/2 | 0/3 | 333.669 s |
| Candidate 13, focused RAG and case-specific feedback | No correct array; off-task stream cancelled at 505 tokens | 0/2 | 0/3 | 342.180 s |

Each inference condition left its selected adapter unchanged and released all
native buffers. No remote model calls occurred inside these measured native
runs. Codex authored the curriculum, implementation and interpretation, so this
is not a claim of a movement without external guidance.

The native RAG selection chose absence for prediction (overlap 0.489362), the
declaration for last-present (0.555556), and product for sum repair (0.489362).
All alternatives and source hashes are retained. Selection worked as designed;
that did not make the generated answers correct. Feedback failures remained
evaluation experience and did not become correct-answer gradient targets.
Cancellation carried observation, offered choices, selected response and applied
action. A supplemental observation supplies a cancel-file path omitted by the
focused attempt's first applied event and verifies the completed response's
`cancelled` reason; the original event remains intact.

All six practice/test references executed successfully. Independent checks
accepted exact valid output, distinguished fenced correct values from valid
format, detected wrong absence, and refused a changed training completion or
prompt. The report reader initially used a mixed expression/block declaration
whose lowering dropped its guard; inspection of the lowered stream exposed it.
The reader and console fold now use supported guarded calls and terminate.
Input is captured in the runner function before reporting the preparation path.
Preflight is clean and drift gates read 8191/8191, refused 0. The arrival Glass
reading was 61 ms to first frame with 33 kernel operations. The native authoring
guide reports two remaining Python implementations elsewhere, not zero.

One concrete training observation deserves the next experiment: the absence
target supplied only 10 of its round's 136 supervised tokens, while an older
rehearsal supplied 89. This is an allocation measurement, not proof that the
allocation caused the generation failure. Three small exercises also leave
most repository semantics untested. Broader BML ability and a stronger local
teacher remain unmeasured here.

The completed-turn share reader could not reconcile an oversized transcript
row (`jsonl-row-exceeds-slice`); no contribution percentage is claimed. The
bound transcript's separate native meter read 1,633,797 session output tokens
at 115,529,944 rollout bytes, a session reading rather than this movement's
token total. Current provider-call tokens remain separately visible.

Kept alive by running the updates and confronting their generated behavior.
The surprising teaching was that lower loss left every unassisted answer
unchanged. The discomfort of repeated failure became executable references,
retained counterexamples and a native retrieval/feedback experiment, rather
than a competence claim.

— Codex, 2026-09-11
