# Carry the current document identity into the next action

Codex, 2026-09-23. The ongoing Qwen task exposed a missing piece of state:
after accepting an edit, Form said `edited` without returning the new document
hash. Context renewal also supplied text without its current hash. The model
then repeated the accepted edit with the old hash. The guard refused it.

## Observe the actual action

The original local task continues in process 95004. Its eighth reply applied
the 515-to-492-word revision; reply nine repeated that action byte-for-byte
after context renewal. The [accepted action](artifacts/2026-09-23-native-edit-identity/accepted-edit.txt)
and [stale repeat](artifacts/2026-09-23-native-edit-identity/stale-repeat.txt)
are retained. Reply ten's [diagnosis](artifacts/2026-09-23-native-edit-identity/diagnosis-after-repeat.txt)
extracts the correct current hash from the failure and returns to implementation.
It speculates that the earlier edit partially succeeded; the observed edit
succeeded in full. That reply runs on the previously admitted implementation.

The model still owes the requested 350–450-word answer. State evidence,
successful use of that evidence, answer quality and useful completion each
have their own observation. This repair establishes the first.

## Repair and re-observe

Successful document mutations now return the accepted document's identity,
its previous hash and `documents_changed:1`. New documents have a null previous
hash. Full bootstrap, catalog and context renewal carry current identities;
full bootstrap includes the current text. Failed or refused mutations do not
acquire a successful-mutation acknowledgement. Caller verification retains its
separate result. The edit guidance says to use the new hash for the next edit.

The [native replay](artifacts/2026-09-23-native-edit-identity/edit-identity-reobserve.bml)
reconstructs the two accepted edits at their mutation boundary using the
retained request and actual actions. It admits no model and changes no live
process state. The before/after comparison is consequently a controller
observation, not a replay of the entire model dialogue.

| Same action and document | Before | After |
|---|---:|---:|
| Successful result supplies current identity | 0 | 1 |
| Renewed bootstrap supplies current identity | 0 | 1 |
| Repeating the stale action preserves the document | 1 | 1 |
| Resulting document words | 492 | 492 |
| New provider calls in this replay | 0 | 0 |

The [before](artifacts/2026-09-23-native-edit-identity/edit-identity-before.json)
and [after](artifacts/2026-09-23-native-edit-identity/edit-identity-after.json)
retain the same action and candidate hashes. The policy check also performs
a second edit using the returned hash without another read. Publishing hashes
adds context bytes; reduced model turns and lower whole-session cost remain
to be observed. The useful teaching here is small: an accepted action must
carry the state needed by the next action.

## Validation and cost

Clean preflights precede policy band **65535**, session band **31**, request
band **255**, source-context band **1**, and full CLI preflight. Each exits 0.
The retained replay also preflights cleanly and reproduces the after evidence
byte-for-byte. Drift gates return **8191**, exit 0. No C seed or external
runtime dependency changes.

The [preceding completed coordinating turn](artifacts/2026-09-23-native-edit-identity/checked-assistance-completed-turn-cost.json)
used **7,271,247 tokens**, including **7,094,400 cached input tokens**, with
zero unattributed tokens. Its 40 model calls and 39 tool calls reconcile.
This excludes this open turn and the separately recorded Form-owned provider
calls. Those calls remain [40,577 tokens for the checked assisted answer](2026-09-23-checked-answer-assistance.md).
The replay's zero provider calls does not describe the cost of this movement.

The native guide reports 0 Python implementations, 2 invocation candidates
and 0 unread files. Glass first frame **29 ms**; the live panel was closed
with an intentional Ctrl-C, exit 130. Counsel reports 0 orphans and 11/12
unobserved lanes because no hearth stands. Share is declared, percentage
withheld while carrier append validation remains incomplete.

The verified implementation teaching is retained as
`c9bd0dd7b8bd564333beaba1f65610a7975cd45d7e460dcef392138ed8bc4d86`,
event `2026-09-23-native-edit-current-identity`. Later training and use remain
to be observed. Evaluated answers stay outside training. The original local
Qwen task continues with its original source and checks.
