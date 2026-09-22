# Qwen learning reaches the next answer; faithful composition remains open

Signed: Codex. Native Form/BML with the existing Metal carrier; no C seed
change or additional runtime dependency.

The session learner updates Llama 3B while the answering model is Qwen 27B.
This movement adds a reusable, explicit Qwen learning door and returns to the
original enquiry with its learned adapter. The adapter changes the answer,
but the answer retains source errors.
The candidate remains unpromoted. Whole-session quality parity is not achieved.

## What now runs

`observe/qwen-lora-learning-run.bml` accepts verified caller-selected lessons,
captures completion features in one Qwen admission, trains a rank-one head
adapter, retains per-row validation and checkpoints, verifies release, and binds
the selected candidate to the exact sealed base/tokenizer and adapter bytes.
Fresh conversation state separates examples; prompt tokens are not trained.
`qll-open-selected` reopens that retained candidate explicitly for a later ask.
The automatic session worker still selects Llama; this does not silently change
its routing or promote a Qwen adapter.

The native request and ownership checks pass. A repeated invocation refuses
the existing evidence directory with exit 1 and preserves the completed run.
The runner returns ordinary refusals with a nonzero process status. Exceptions
inside Metal/batch helpers remain a process-failure boundary without BML finally
recovery; the retained data does not claim otherwise.

The initial provider proposal failed `--check` with:
`form.bml call requires one complete expression: str_eq(value_kind(value),"float") and gt(value,0.0)`.
Its profile mapping also placed prose targets in an open reasoning span, and it
reloaded weights per row. Local repair preserves the answer profile, shares one
model admission, checks renewal and optimizer outcomes, and verifies ownership
and candidate identity. My integration then introduced an unresolved `nsm-read`
call and a missing-file `str_len: nothing has no length` failure. Both were
repaired and rechecked before model admission. No failed check became a pass.

## Actual learning and return

Five training prompts use exact committed teaching functions. Three different
validation prompts use exact sentences from those teachings. This tests the
same concepts under different prompts, not unseen-concept generalization.
The original enquiry and all generated answers are excluded from learning.

One admission captured **412** completion positions: **365 training**, **47
validation**, width 5,120, **8,437,760 bytes**. Eight optimizer rounds took
**83,031 ms** in total. Validation loss fell from **3.659830885 to 3.319386218**;
all three validation rows improved. Epoch eight was selected by aggregate loss.
The optimizer's six buffers, three adapter buffers, six loss buffers and model
session were released. The sealed base file still verified after learning.

Both subsequent answers used the same original question, supplied sources,
system text, full profile, 4,607 prompt IDs, 12,288-position context and
2,048-token allowance. Neither received an injected observation or correction.
The fresh base answer is byte-identical to the prior ordinary-generation answer.
The candidate's admitted A/B digest matches the retained learning result:
`ec99dfac7f499c1956f4b4032e63fb9a2634c1ea76c33cc04d80562c396b8c26`.

| Observation | Base | Candidate |
| --- | ---: | ---: |
| Generated tokens | 871 | 844 |
| Whitespace-delimited words | 643 | 663 |
| Reached model stop | yes | yes |
| Session release verified | yes | yes |
| Requested 350–450 words | exceeded | exceeded |

The paired execution took **788,339 ms**, including admissions and prefills.
Generation plus release took 172,316 ms and 166,992 ms respectively. Other
observation work ran alongside parts of this execution; these times do not
establish a speed improvement. Learning and comparison made **zero provider calls**.

## What changed in the words

The [candidate](artifacts/2026-09-23-qwen-teaching-learning/candidate-answer.txt)
now states: “A label in another language is a surface, not a different sense.”
The [base answer](artifacts/2026-09-23-qwen-teaching-learning/base-answer.txt)
omitted that distinction. The candidate also removes the direct claim that
the frequency calculation detects the person's contracted or expanded state.
Its replacement still overstates what supplied annotations establish.

The candidate continues to invent unmeasured native-model behavior, promises
that axioms prevent hallucination, and omits the unreferenced-cell reclamation
condition when describing persistence. Trust remains largely vocabulary
reframing and claimed warmth.
No human judgment of felt resonance was supplied.

The original assessment also alleged a Node-ID/content-identity conflation.
That finding is withdrawn: axiom 2 names the four integers, and the native
`nodeid-one-cell-band` returns **7** for coordinate identity and interning.
The candidate's sentence alone does not establish the alleged regression.
The [correction receipt](2026-09-23-teaching-withdrawal.md) carries the retained
lesson's withdrawal and checkpoint repair. The original answer bytes stay intact.

This is Codex's source comparison, not an automatic semantic score. The
[retained assessment](artifacts/2026-09-23-qwen-teaching-learning/assessment.json)
entered a correlated organ-health exchange, applied `abstain-from-promotion`,
and re-read `allow_promotion=0` from disk. Lower loss and fewer generated tokens
did not produce a source-faithful or appropriately sized answer. The next gap
is faithful composition of the supplied distinctions in a complete response.

## Evidence, instruments and spend

The [artifact directory](artifacts/2026-09-23-qwen-teaching-learning/) retains
the exact learning request, target-source identity, capture manifest, training
result, candidate binding, audit, paired answers and comparison. Private feature
bytes and checkpoints remain in `.hearth/qwen-teaching-learning-2026-09-23`.
Native admission, audit, comparison and export sources remain beside it in
`.hearth/`. The audit reconciles all target counts, offsets, masks and hashes.

Drift gates pass **8191/8191**. Native guide reports Python implementations 0,
execution candidates 2, unread 0. Counsel shows **orphans 0** and **11/12 lanes
unobserved** because no hearth stands. The owned glass viewer's first frame
arrived in **25 ms** and was deliberately stopped. Its brief overlap with the
comparison is retained as context, not removed from timing evidence.

One Form-owned implementation call used **38,095 provider tokens**: 30,486
input, including 10,624 cached, and 7,609 output. Its proposal and usage remain
under `.hearth/response-sessions/syntheses/9eb6689b5c4cb8a60dc423037bbd885a2a6218312b954c288396fde687aad4fa`.
The provider's shape/source checks did not establish compilable code.

Coordinator cost is separate and substantial. The preceding completed turn's
retained usage reading was **8,437,004**: input 8,353,622 (cached 8,042,624; uncached
310,998), output 57,490, and 25,892 unattributed. The present turn is still open;
its total is not yet reconciled. The output-only cumulative meter read 5,005,182;
its decided byte advanced beyond its entry extent during a live append, so it
is not an isolated turn-cost or full-token result. The share reading withheld
its percentage while its append range was still being checked. No token-saving
or parity claim follows from these observations.

The procedure assessment was returned through native session learning under
event `2026-09-23-qwen-teaching-learning-procedure-v1`, session
`native-arrival-bootstrap`. It was retained and its Llama worker launched;
The Llama worker subsequently completed optimizer step 142 without promotion.
Because the target included the unsupported identity criticism, it was then
withdrawn through the native correction path above. It did not train Qwen.
