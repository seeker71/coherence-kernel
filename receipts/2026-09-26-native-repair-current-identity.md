# Historical failure identity reached a current edit

The resumed local Qwen response supplied `89537cde…` to a guarded whole-document
edit while the current answer was `307755a1…`. The native tool returned
`edit-sha256-mismatch`, exit 2, and preserved the 471-word answer. The supplied
hash belongs to an earlier checked answer, recorded in the preceding public
result. Current document identities were also supplied at admission.

The existing native loop recovered on its next action. The retained answer
became **454 words**, with the automatic new-node-ID claim removed. Its word
check still failed by four words; all four phrase checks passed. This is a
working document, not a released final answer. It then shortened the answer to
**443 words** at `1a15dff2…`; all original checks passed, and turn 28 was in review
after a successful read. Its eventual review and release remain pending. These
changes used the already-admitted context.

## Repair at the context boundary

Admission was carrying earlier document hashes inside historical check and
guard evidence, including a pending repair task retained from an earlier failed
check. The controller now renders those historical failures with their actual
exit, stdout and stderr. It preserves the complete historical evidence in
continuity and the public result, and keeps the current failure's full evidence.
Source documents, edit guards, task state and caller checks are unchanged.

The [native observation](artifacts/2026-09-26-native-identity-usefulness-care/check-admission-history.bml)
reconstructed admission against one frozen, validated real checkpoint at turn
25. It did not admit a model or change the active model's context.

| Observation | Before | Verified change |
| --- | ---: | ---: |
| Admission bytes | 34,011 | 31,216 |
| Current answer identity present | 1 | 1 |
| Earlier checked identity present in admission | 1 | 0 |
| Earlier identity retained in public history | 1 | 1 |

The complete public-history SHA-256 remained `92a41180…`. The reduction is
**2,795 bytes** for this reconstructed admission; token count, response quality
and runtime benefit from a later admission remain unmeasured.

Intermediate observations remain alongside the final one: normalizing prior
verification removed 833 bytes but left the stale identity; normalizing the
historical guard removed more, while the pending task still carried the older
checked identity. Normalizing that historical pending failure removed the
remaining occurrence. Each source change was re-observed against the same
checkpoint. Existing coding-policy and continuity bands returned **65535** with
exit 0 after the final implementation; both preflight chains were clean.
Drift gates passed **8191/8191**. The verified teaching was retained as
`2dbbde82…` and its learner launched; a completed weight update is unobserved.

## Adverse evidence and boundaries

The measurement initially used JSON decoding on a FORMBIN2 checkpoint. Both
initial calls returned `checkpoint contract invalid`, exit 1. Using the existing
`fcam-read` native binary reader repaired that cause. A later diagnostic helper
placed a definition inside a conditional block; compilation returned
`form.bml call requires one complete expression`, exit 2. Moving that definition
to the section's definition surface made the actual observation run cleanly.
No failed measurement was treated as a successful one.

The snapshot observer now reports only the native tool name, exit and error,
plus offered and current hashes for an explicit SHA-guarded edit. It reads
validated caller-owned state. Private model reasoning and reply files were not
read. The native repair and its release-aware comparison remain the same live
owners; this movement started no new provider or model admission.

The last counsel panel showed **0 orphans**, with **11/12** serving lanes
unobserved. The broader gap remains: dependable native completion, actual answer
quality and resonance, and whole-session quality and throughput at minimum
rented-token cost. Removing a stale identity from reconstructed context proves
that context change; its effect on model behavior remains unobserved.

— Codex
