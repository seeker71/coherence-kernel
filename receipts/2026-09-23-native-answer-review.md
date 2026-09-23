# Carry the semantic revision through native continuity

Signed: Codex, 2026-09-23.

The preceding movement was progress: `3edec1754` landed the direct-replanning
repair and applied the actual native edit at the repaired boundary. Its
447-word answer still mostly inventories definitions and claims that
traceability changes felt resonance without supporting that causal claim.

The current work offers those two concrete observations to local Qwen through
the public `code` entry, using `mode=review` and `review_entry=direct`.
The original enquiry, its source packet and the actual 447-word native draft
are supplied in full. The original document checks remain intact; report
checks require an answer string, findings array and 350–450 words. Semantic
quality is assessed from the returned answer separately.

This uses ordinary managed review. Its checkpoint carries unfinished work,
and `nsc-review-observe` retains assessment experience with training excluded.
The earlier fresh evaluation workflow discarded session state between
admissions. Choosing the existing review path supplies continuity without
treating a structurally accepted answer as a correct training target. This
request starts a new review of the retained candidate; it does not claim to
resume the earlier uncheckpointed generation.

## Repair the actual admission failure

Preparation first stopped before model admission:

```
form-run ./fkwu .hearth/native-answer-review-prepare.bml
fkwu: form_error: native review request or original source checks failed
@form fkwu 1 0 73 73
```

Native discrimination returned request-valid=0, reasoning-valid=0 and
source-checks-passed=1. The helper copied `reasoning_tokens` while omitting
its required `reasoning_answer_tokens`. Carrying the original separate
1,024-token answer allowance made request-valid=1, preserving the original
source checks. Both helpers passed clean preflight before execution.

The public request entry now names this missing resource directly:
`reasoning_tokens requires reasoning_answer_tokens`. It asks the caller to
retain the reasoning allowance and supply the separate answer allowance.
Admission rules are unchanged. The existing request band invokes this public
entry with the incomplete request and verifies an attention result before
any model admission. It returned **255**, exit zero, after clean preflight.
This improves the next caller's diagnostic; the original helper's generic
error remains preserved above.

Drift gates returned **8191**, exit zero; no kernel source moved. The native
authoring guide reports zero Python implementations, two existing invocation
candidates and zero unread files. Counsel reports **zero orphans** and 11/12
lanes unobserved with no standing hearth; the independent review process is
observed through its own handle and runtime log.
The verified admission teaching was retained as example
`00348c2a2e492895aac1363e5aa9b80226ada43b381e37089234ba8ddb4cb723`;
the learner reports an existing or launching worker. This establishes retained
evidence, not an observed serving-weight update.

## Current native owner and cost

The prepared request selects the existing **qwen38-q8**, context 16,384 and
the original 12-reply allowance. Its enquiry SHA-256 remains
`c0520855fe91d59e30f913af192843c99e37185f3eaeb917e08deacb63f06574`.
The [request and admission](artifacts/2026-09-23-native-answer-review/request.json)
are retained beside the native helpers. Exec **11092**, PID **50317**, owns
the ongoing run; `write_stdin` confirmed it live. The runtime passed model
admission and advanced through at least 3,136 of 5,853 prompt positions.
Its log is `.hearth/native-answer-review-2026-09-23/runtime.log`.
No returned answer or completed semantic improvement is claimed yet. Continue
that owner; observation expiry is not evidence that it stopped.

The selected previous completed coordinating turn used **9,632,662 tokens**:
9,544,297 input, including **9,180,416 cached input**; 62,809 output; and
25,556 unattributed tokens included in the total. Its 67 model calls and
64 tool calls reconciled. That completed turn, identified in the retained
[cost reading](artifacts/2026-09-23-native-answer-review/previous-turn-cost.json),
excludes this open turn and separate provider subprocesses. The native review
uses zero provider calls. These scopes establish no end-to-end savings ratio.

Coordination cost is itself an open gap. Keep the native owner, checkpoint,
original checks and complete runtime evidence; inspect the actual returned
answer before spending on another inference or claiming parity.
