# BMF repetition carries BML retry

2026-09-29 — Codex

BMF multi-match and cooperative BML work now share the same advancing-checkpoint
decision in `bmf-repeat.bml`. `nve-repeat` runs a body on the existing queue.
A native stop unwinds into local care; `nve-repeat-again` queues a changed
choice without calling the body. Admission commits the proposed cursor.
Declined options, unchanged choices and cancellation preserve the held
checkpoint and findings. Ordinary zero and nothing remain completion values.

Observed with `form-run ./fkwu observe/native-repeat-witness.bml`: 20,000
passes, zero additional event-image calls, peer order `A0B0A1B1A2`, repaired
failures in the original context, accumulated findings, no-progress rest,
cancellation, resource-event resumption and physical release all passed.
The witness deliberately raises five native stops and observes their care.

Existing BMF primitives returned 12; component execution returned 1023.
The previous cooperative-slices observation still passed its 10,000-pass,
native-resource, care-correlation and release observations. Changed-source
preflights reported zero errors, warnings and unresolved calls.
All nine public pipe, HTTP and translation witnesses passed with clean
preflights, exit 0 and zero stderr; source identities and report readback passed
in `docs/evidence/fkwu/native-pipe-translation.json`. Drift gates returned
8191/8191, refused 0 on the rebased checkout. The repeat witness and both
BMF observations passed again; retained public source identities stayed current.

Glass: first frame 35 ms, closed through its native abstain door; counsel
orphans 0, with 11/12 serving lanes unobserved because no hearth stands.
The native guide reports zero Python implementations and two invocation
candidates. The verified teaching is retained by the local learner; its supervised round
is still processing, so no new adapter or answer-quality improvement is claimed.
New implementation, observations and edits are BML; C source is unchanged.
The unused continuation draft and obsolete parser warning were removed.

This closes explicit self-calls and yield plumbing for structured repeat
bodies. BMF's synchronous matcher keeps its tail-call driver. Arbitrary loop
lowering and preemption remain separate work.
