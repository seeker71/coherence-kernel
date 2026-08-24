# The Grok cutoff left no teaching behind

; witnessed: 2026-08-24 -> durable workspace and session residue reconciled

The visible checkout was not Grok's writing surface. The live `grok` process
had cwd `/Users/ursmuff/source/coherence-kernel`, but its durable worktree was:

`/Users/ursmuff/.grok/worktrees/source-coherence-kernel/2026-08-14-f4ef26ab`

That worktree held three uncommitted files:

- `form/form-stdlib/form-cli-qwen-teach-layer.fk`
- `form/form-stdlib/form-teach-layer.fk`
- `form/form-stdlib/tests/qwen35-form-cli-band.fk`

The session event log ended without a final model message. Its last transition
entered `waiting_for_model`, then recorded `turn_ended outcome=error` at
`2026-08-24T05:06:58.562Z`. That is consistent with the subscriber usage
cutoff Urs observed. No private prompt or answer text was copied into this
receipt.

The worktree diff contained 24 insertions and 6 deletions. It made Form
teaching default-on when no explicit latch exists and added the local control
glosses:

- `nothing`: vacant attestation is silence without a fork;
- `cut`: unresolved-call has two opposite repairs;
- `stop`: stale fkb means rebuild then stop;
- `undo`: unbalanced-source wants a byte-for-byte restore;
- `timeout`: one-turn budget ended incomplete.

Nothing needed to be copied blindly. `form-teach-layer.fk` and
`tests/qwen35-form-cli-band.fk` are byte-identical between the Grok worktree and
this branch. This branch's Qwen teach layer is newer rather than divergent: it
contains every one of those five lessons and the default-local-turn prefix,
then extends the same surface to 29 concepts covering executable BML,
walker-carried BMF grammars, live raw-byte streaming cursors, choice/lanes/cut,
checkpoint undo, timeout versus exhaustion, strict nothing versus 0/1, thought
kernel routing, JIT evidence, and the current-source knowledge-query token.

The usage wall therefore stopped Grok's voice, not the movement. Its durable
teaching is already present in the local body, and the new held-out evaluator
can test whether the local Q8 model actually uses it.

The surprising teaching was that the active process cwd told only half the
truth; the uncommitted work lived under Grok's own hidden worktree.

Discomfort turned to gold when “the session stopped” became three inspectable
files and one terminal event instead of a story about lost reasoning.

— Codex
