# Continue the model's unfinished command within its answer allowance

Codex, 2026-09-23. The gap remains useful native completion, answer quality and
total session cost. The two landed controller repairs now have live evidence:
Qwen used each updated document hash and received the actual caller check after
each changed edit. Its edits removed only fourteen words.

## What the real continuation returned

The [completed local run](artifacts/2026-09-23-native-json-completion/report.json)
began from the previous retained 492-word candidate with the original enquiry,
source packet and 350–450-word check. It used a fresh context, six new replies,
512 initial tokens and 1,024 final tokens per reply. It returned **478 words**
after **2,083,078 ms**, with **5,096 generated IDs**, **2,639 injected IDs**,
four tool calls, three checks and verified release. It made zero provider calls.
The coordinating work is accounted separately below.

| Actual reply | Observed change |
|---|---|
| 1 | Correct current hash; 492 → 490 words; immediate check failed. |
| 2 | Correct updated hash; 490 → 478 words; immediate check failed. |
| 3, 4, 6 | The same unfinished 549-byte JSON command; no edit applied. |

The [retained answer](artifacts/2026-09-23-native-json-completion/answer.txt)
still mostly lists definitions. Its closing claim that traceability changes
resonance remains unsupported by an observed comparison. Those are my readings
of the answer, not measurements of Urs's felt resonance. The previously checked
427-word Form-owned assisted answer remains a separate result; no new provider
answer was requested here.

The partial command stops inside a string after “GGUF token”. Public metadata
reports 170 final generated IDs and a natural completion for replies 3, 4 and 6.
The model's [stop metadata](artifacts/2026-09-23-native-json-completion/stop-metadata.json)
maps both configured stops to ID 248046, `<|im_end|>`. This establishes the
configured identity; the old event did not record its pending stop ID.
Private reasoning files were not read. Reply 5's action is not included in this
per-reply comparison because its public path was lost in truncated output.

## Repair and its boundary

`form-cli-json-completion.bml` handles this observed unfinished-string boundary
inside the reserved coding final stage. The stop prediction has not entered
the model's state. Form selects the highest finite non-stop score from that
same head and lets the model continue with the remaining original allowance.
It preserves the generated IDs, position and prefix. It supplies no answer
content. Each selection records the stop, selected token, prefix hash and token
counts; the original partial response is retained separately.

The original syntax admission, tool permissions, review and caller checks
remain. Complete JSON and other syntax failures retain their prior handling.
The change does not affect ordinary dialogue or private reasoning. Non-finite
scores refuse continuation. An exhausted allowance retains incomplete output
without acting on it. Native organ health records observation, care and the
fresh result; a valid command still needs task verification.

The boundary checks pass with exit 0. They cover prefix and state preservation,
unchanged token allowance, exhaustion, refusal, complete JSON, multiple stops,
negative logits, tie selection and non-finite heads. Existing reasoning checks
return **1**, model-session checks **4095**, and the full CLI preflight is clean.
No C seed or additional runtime changed.
Drift gates return **8191**, exit 0.

The [next actual continuation](artifacts/2026-09-23-native-json-completion/after-started.json)
is running from the retained 478-word candidate, with the same original goal,
sources and checks. It has six replies and uses the repaired implementation.
This changes both the current candidate and implementation relative to the
earlier admission. At this receipt's writing it is prefilling; there is no
claim yet that the repair improved its answer or completed the task. The live
owner is PID 28854; evidence belongs under
`.hearth/answer-edit-json-continued-2026-09-23`. Continue that owner until its
terminal result; do not admit a duplicate.

## Failures retained and cost

The earlier public-action observation helper first failed preflight because
its stdin/write effects lacked the effectful marker. It was mistakenly run
before that failure was inspected. Adding the marker produced a clean compiled
preflight and the repeated observation reproduced 490/478 words.

The stop-metadata helper initially passed the model alias as a file path:
`form-run ./fkwu .hearth/answer-edit-stop-metadata.bml` exited 1 with
`fkwu: str_byte_at: only a string has bytes -- ask value_kind first`.
Resolving the registry path and checking that it exists corrected this reading.
The new boundary band's first preflight reported 5 errors and one unresolved
`md-u32le`; using the existing `md-le32` removed that defect and the checks pass.
The continuation's first launch exited before admission with
`native generation write failed: .../answer-edit-json-continued-2026-09-23/started.json`.
Creating its missing evidence directory allowed the single model admission.
Several source searches also used nonexistent guessed paths; they were corrected
to observed paths. None of these failures establishes native answer improvement.

The [previous completed coordinating turn](artifacts/2026-09-23-native-json-completion/previous-turn-cost.json)
used **4,110,082 rented tokens**, including **3,872,640 cached input tokens**:
4,084,743 input, 25,339 output, zero unattributed, 31 model calls and 29 tool
calls reconciled. This excludes the current open turn and separate provider
processes. Cached input remains in the total. The scale of coordinating cost
is part of the gap; local execution's zero provider calls is not a zero-cost
movement.

The native guide reports 0 Python implementations, 2 invocation candidates and
0 unread. Glass first frame is **27 ms**; its viewer was intentionally stopped
with Ctrl-C (exit 1). Counsel reports 0 orphans and 11/12 unobserved lanes with
no standing hearth. This reading's share is declared, percentage withheld while
the carrier append range is still being checked. Semantic contribution is not
measured by that meter. Evaluated answers remain excluded from training.

The verified boundary teaching is retained as
`cddd794b0b4e8a6da951960342966fceae9160fc86eab94afc894c47bebe887f`,
event `2026-09-23-bounded-json-completion-contract`. Retention is observed;
learned use and improved model answers remain to be observed.
