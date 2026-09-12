Codex · 2026-09-12

The `glass` alias launches `/Users/ursmuff/source/coherence-kernel`, rather than
this agent worktree. Its ignored executable returned freshness 15. The frontier
door then failed with exit 1 at core's `int_to_str`: `value_str` was unresolved.
The committed source already contained that primitive. Rebuilding the checkout
kernel restored freshness 31 and a clean Glass live-library preflight.

`observe/form-glass-run.fk` now checks freshness before admitting core or BML.
The small native bootstrap closes the canary's stdin and requires both exit 0
and exactly `31\n`. Otherwise it compiles the committed seed into a temporary
executable, publishes after compiler success, and rechecks before spawning
`observe/form-glass-supervisor-run.fk`. The existing supervisor retains the fleet
and rendering behavior. This earliest layer stays direct Form because admitting
today's BML/core on the older kernel reproduces the very failure it repairs.
There is no new C implementation or Python helper.

Dependency refresh previously discarded the child status and stderr and always
returned one. It now exposes diagnostics and reports the real exit. Admission or
renderer failure releases the supervisor's owned sensors and propagates failure
to the public door. A failed compilation does not repeat blindly in the carrier.

Observed checks:

- An executable built from the actual older `5e82c324b` seed and opcode header
  ran the new bootstrap. In an isolated checkout, with its binary timestamp
  deliberately older than the current source, freshness read 15, the rebuild
  took 3,643 ms, and freshness then read 31. The public door handed off to a
  probe using `value_str`, which the older kernel could not resolve. Exit 0.
- Repeating that public-door probe on the fresh executable skipped compilation
  and passed. This probe substitutes a bounded supervisor; it is not a claim
  that the entire fleet ran inside the isolated checkout.
- A deliberate compiler failure returned exit 1 and left the existing executable
  SHA-256 unchanged: `e13100f4231b0731350efab54860039b98c4abf6bbbdad09df83c5fd23e6ddaf`.
- A missing refresh unit exposed its compiler diagnostic and child exit 2;
  `fglu-refresh-one` returned process exit 1 rather than success.
- The actual supervisor functions, with a deliberately missing startup unit,
  returned exit 1 and released their owned sleep process, PID 85438. A following
  process lookup found no such process.
- Existing launch, carrier and healing checks returned 131071, 31 and 262143,
  all with exit 0. Their preflights had no errors or unresolved calls.
- The live renderer painted frames. The final bounded panel read 19/20 frames
  under 50 ms, mean 56 ms, first frame 389 ms and maximum warm frame 49 ms.
  Queue and storage sensors were absent; this does not claim all sensors healthy.
  The drift door returned 8191/8191 with zero refusals.

The share meter separately failed with `str_len: nothing has no length`; no
contribution percentage is claimed. The verified bootstrap teaching was returned
through the session-learning door under session `codex-glass-compile-2026-09-12`,
event `native-bootstrap-and-visible-refresh-v1`. Submission and completed learning
remain separate observations. The later completion reported one native update,
candidate step 19, worker completed and zero pending rows; serving stayed at step 4.

Retained experiment evidence is under `/tmp/form-glass-bootstrap-case-20260912`,
`/tmp/form-glass-bootstrap-failed-20260912`,
`/tmp/form-glass-supervisor-case-20260912`, the printed native bootstrap job paths,
and this worktree's `.hearth/glass-*.log` files.

The useful surprise was that repairing this worktree alone would have left the
alias broken. Following that mismatch to its actual checkout made the error
actionable, and the older-kernel execution showed the repair can begin before
the formatting layer that originally prevented it.
