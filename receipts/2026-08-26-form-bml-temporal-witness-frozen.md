# 2026-08-26 — temporal BML witness frozen before transformation

The next semantic family is now preregistered before any prompt, answer,
candidate, repair, model call or gradient exists.  This commit changes no model
and opens no Metal carrier.

The family is `form-bml-temporal-witness-v1`.  Its source-group surface binds,
byte for byte, these two untouched semantic authorities:

- `form/form-stdlib/input-stream-lifecycle.fk`, SHA-256
  `d1d0be0540f4a94827f2f1725967fbd13c85fb7ecc3d48124cec04a1204f077b`,
  provenance `1440772cc07df8b70be65c796a1cdead8fb4245e`;
- `form/form-stdlib/obs-verify.fk`, SHA-256
  `9846e78e9e93d68dfc17ec1750176b0ad82cf0e2b6264098c803860d1590344e`,
  provenance `1c6f456c65c0e35de7527ceb255d3d18e974798e`.

The final-LF-inclusive source-group ID is
`9c01794960753681e199c338780049fbee8fbb8b097d8157240718aa061790ee`.
Both source paths are absent from all 34 canonical-BML adaptation rows.

Ten executable consequences are sealed.  They distinguish timely present 1,
timely present 0, unanswered, late, live silence one tick before the deadline,
timeout at the inclusive deadline, an explicit nothing event, an unacknowledged
stop, handoff and offline.  The family uses the source's explicit `-1` state
code for verified nothing because the present Form evaluator does not yet carry
native nothing through this harness without collapsing it into numeric zero.

The two source files, their hashes, the case table and all later derived
surfaces are denied to training and RAG while this family is called held out.
The preceding `59cb` diagnostic and all three landed corpus hashes are denied as
well.  Shared canonical BMF/BML grammar is infrastructure, explicitly not novel
semantic evidence.

No model candidate has been observed.  The next commit may now derive a prompt
and connect live canonical prefix state to token choice without moving this
frozen denominator.

Signed, **Codex**, embodying Sema with Beauvoir's temporal boundary retained.

Kept alive: the freeze records exactly what may not leak before generation.
Most surprising: one tick, 28 versus 29, creates a sharper semantic boundary
than another broad API vocabulary.  Discomfort turned to gold when the current
evaluator's nothing-to-zero collapse forced the narrower and truthful
`-1/0/1` executable contract.

; witnessed: 2026-08-26 -> source group frozen before prompt/model/gradient
