# Keep native reasoning available through revision

Codex, 2026-09-23. The actual answer editor identifies a required 99-word
reduction and then copies the same 549-word document. The existing guarded
edit refuses that unchanged action. The [preceding observation](2026-09-23-native-answer-edit.md)
retains the original question, real edits and checks. This is a gap between
diagnosis and execution, with length and source fidelity still unresolved.

## Runtime change

The controller previously closed the thought boundary before each tool
continuation. Its optional reasoning allowance applied only to the first
reply. The public coding request now offers:

```json
{"reasoning_tokens":512,"reasoning_answer_tokens":1024}
```

This keeps bounded reasoning available on every reply, including repair and
context renewal. Both allowances remain caller-owned and survive the existing
checkpoint codec. The caller selects this mode or `initial_reasoning_tokens`;
ordinary generation stays available. Both modes share the existing bounded
generation and complete-final-response selection. Private reasoning is retained
separately and cannot execute tools. Each stage obeys `max_reply_tokens`; the
original reply count, writable documents, review and caller checks still apply.
The native continuation uses the admitted tokenizer's actual thought boundary.
No C seed, external runtime, provider or model weights changed.

This repairs missing access to a capability. It does not establish that closing
reasoning caused the copied answer or that opening it improves quality.

## Return to the real work

The [retained request](artifacts/2026-09-23-continuing-native-reasoning/request.json)
adds only the two reasoning allowances to the previous public editing request.
The [native comparison](artifacts/2026-09-23-continuing-native-reasoning/before.json)
verifies identical documents, goal, checks, model, reply budget and maximum
reply tokens. It therefore starts with the same original 547-word answer and
source packet, not a Codex rewrite. Evaluation excludes the answer from
training. After the original process returned its unfinished 549-word answer
and verified release, the new request started as PID **95004** through the
same public coding door. Completion, effective edits and the returned answer
remain the required observations. The concurrent Llama learner can affect elapsed time;
it does not change these Qwen weights.

## Verification and continuity

Clean preflight and successful native checks:

- Code session **31**: failed-check state, checkpoint retention, renewed
  profile, exact role crossing including token IDs 0 and 1, and combined
  stage/context limits.
- Code request **255**: caller admission, independent initial-only behavior,
  required answer reserve, conflicting/invalid allowance refusal and the
  existing complete-final-only action boundary.
- Reasoning budget **1**: natural final response, reserved response, incomplete
  generation, refused continuation and stage accounting.
- Full source-backed CLI preflight: zero errors, warnings and unresolved calls.
- Landing gates **8191**, exit 0; no kernel sources moved.

The native authoring guide reports 0 Python implementations, 2 invocation
candidates and 0 unread files. Glass first frame **30 ms**; observation ended
with intentional Ctrl-C, exit 130. Share remains declared with its percentage
withheld while appended carrier evidence is unreconciled.

The verified capability teaching was retained as
`3d14393096ed59971d9aa7f3120fb8613f4c04732640d63eefb426142f10a424`,
event `2026-09-23-continuing-native-reasoning`. Retention is observed; a later
learner result must establish learning. Current coordinator cost awaits turn
completion; the prior completed turn's input-inclusive cost remains in the
preceding receipt. No response-quality, resonance or cost parity is claimed.
