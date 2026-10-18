# Ordinary BML loops on the owned repeat queue

The cooperative compiler entry in bml-repeat-lower.bml turns one complete BML
method into a start function over the existing event owner. It reuses the BML
reader, scope, expressions and stores. Immutable local frames cross loop
back-edges through nve-repeat; branch exits and loop updates have one compiled
home. No C source changed.

Observed with `form-run ./fkwu observe/bml-repeat-witness.bml`: two independent
20,000-pass computations, a host timer during live computation, while/for,
nested labelled exits, early return, parameter mutation, shadowed locals,
sequential loops and sixteen consecutive conditions. An unchanged checkpoint
entered local care. A prior choice's finding accompanied changed input into a
queued retry; the unrepaired peer stayed held without spinning. Cancellation
before execution kept attempts at zero. Pure ready work added zero event-image
calls; every observed owner released. Incomplete source and an unsupported call
emitted no partial program.

The witness retains each input, emitted program and native capture under
`.hearth/bml-repeat-<pid>-*` and `.hearth/source-inventory-*`.
Two corrected witness expectations have retained captures: the final condition
takes its own slice, and an interruption is a live reading while care findings
are separate. The exact failing runs were `form-run ./fkwu .hearth/bml-repeat-run.bml`
with `[short peer completes beside long peer, [0, 0, nothing], [0, 1, 6]]`,
and `form-run ./fkwu observe/bml-repeat-witness.bml` with
`[failure remains a finding, 0, 1]` retained in
`.hearth/source-inventory-91697-777214828-0`. Their actual conditions are now
observed directly. Final execution used input prefix `.hearth/bml-repeat-98858-`;
all nine native cases exited 0 and returned the checked answer.

Three source preflights report zero errors, warnings and unresolved calls.
The release door reports 8191/8191, refused 0. The native guide reports zero
Python implementations and two existing voice execution candidates.
Counsel reports orphans 0; eleven resident lanes remain unobserved because no
hearth stands. The verified teaching is retained as
`ordinary-bml-owned-loops-verified`; its local learning worker owns completion.
The preceding repeat lesson's worker completed round 54; serving generation
remains unchanged.

Current reach is local-value computation. Calls, shared fields, source-level
choice/fail and snapshots need owned continuation lowering before admission to
this entry. Source admission still compiles the complete unit. Ordinary
synchronous BML keeps its existing entry.

— Codex, from native execution and source review, 2026-09-29
