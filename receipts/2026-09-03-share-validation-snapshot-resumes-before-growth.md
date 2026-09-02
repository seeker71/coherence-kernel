# 2026-09-03 — share validation finishes its snapshot before following growth

The completed-turn share cursor had a real liveness defect. Its retained
evidence carrier ended at byte `153,129,399`; validation had frozen snapshot
`169,122,793` and moved its cursor to `162,309,713`:

- append range: 15,993,394 bytes;
- already checked: 6,813,080 bytes;
- still unchecked: 9,180,314 bytes.

When the private rollout grew between invocations, the old
`fctec-validation-grown-step` read the newest tail and replaced both snapshot
and cursor. A continuously active writer could therefore keep moving the
upper boundary while the older unchecked middle was never reached.

## The bounded repair

Validation state is now a sequence of frozen generations. The v2 row retains
the evidence carrier, immutable generation snapshot, descending cursor, and a
checked floor. One validation movement reads at most one 2 MiB discovery slice:

1. resume the saved cursor against the saved snapshot;
2. withhold share while `cursor > floor`;
3. reject the candidate if a newer terminal starts at or above the floor;
4. expose a clear range only if the carrier has not grown;
5. if it grew, roll forward to a new generation whose floor is the completed
   snapshot and whose cursor starts at the new snapshot.

The existing nine-field v1 continuation remains readable with
`floor=carrier`. Its first write migrates to the ten-field v2 row without
discarding already checked bytes. Invalid floors, cursors, snapshots, identity,
provider, or model still restart from current physical size rather than being
trusted.

Share health now reports totals across generations:

`checked = (floor - carrier) + (snapshot - cursor)`

`remaining = cursor - floor`

This keeps append, checked, and remaining byte counts truthful after a
roll-forward.

## Physical continuation

The live v1 row resumed at its original snapshot. The first repaired movement
changed cursor `162,309,713 -> 160,212,561`, exactly 2,097,152 bytes, while
snapshot remained `169,122,793`. It later reached `cursor=153,921,105` with
791,706 bytes remaining, completed that generation, and rolled forward to:

`floor=169,122,793 snapshot=170,903,810 cursor=170,903,810`.

The following bounded slice found a newer terminal in that extension and
expired the stale observed row without exposing its percentage. Form then
located and collected the newer 16,514,840-byte completed turn. Discovery,
start lookup, collection, and the post-collection append check all continued
to print `kind=declared` and withhold share.

Only after carrier identity, start and completion timestamps, source
coordinates, provider usage, provider-token totals, tool call/output pairs,
Form receipt bytes, lane totals, source kind, and completion reconciled did the
runner print:

`kind=observed share native=11 local=40 remote=49 sum=100`

That result came from 62 native, 229 local, and 284 remote boundary events
(575 total). The observed read and atomic publication took 896 ms, inside the
5,000 ms attention boundary. It measured the previous completed turn only;
the open turn remains unmeasured until a later turn can witness its completion.

## Witnesses

- evidence invariants: `65535`;
- live carrier collection: `2098174`;
- cursor generations, v1 migration, append liveness, and newer-terminal
  rejection: `16777215`;
- share health including v2 cross-generation byte math: `8191`;
- fresh preflight of both modified chains: balanced, zero errors, zero
  warnings, zero unresolved calls;
- `git diff --check`: clean.

The read-only Glass frame was:

`LIVE NODE ATLAS 2c-KT m92 s45 o25 drop=0 cap=39/row`

with phase census `gas=3 water=92 ice=39` and resource lane `R@=54`.
No Glass, model, or JIT source was changed by this movement.

The most surprising teaching was that every individual tail read was bounded
and still the composition could fail to make progress: bounded work needs a
stable lower destination, not just a maximum slice size. Discomfort turned to
gold when the tempting shortcut—accepting a frozen snapshot while newer bytes
were present—became an explicit generation boundary that is itself checked
before any percentage appears.

Signed: **Codex / Sol**. I kept the exchange alive by preserving the old
cursor, letting each exact range finish, and carrying growth forward only as a
new witnessed range.

; witnessed: 2026-09-03 -> observed 11/40/49 from 62/229/284,
; cursor 16777215, health 8191, collector 2098174, evidence 65535,
; publication 896ms within5000ms, atlas m92/s45/o25/drop0,
; gas3/water92/ice39, resource54
