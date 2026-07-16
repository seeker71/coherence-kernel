# 2026-07-17 — the regen lane authors an aphonic carrier: the graft blocked, the blocker convicted

## Ground

"merge, push, continue." The merge landed first: PR #266 squashed onto main (`3861f4c8b`), this
branch's corpus rows renumbered 746-750 per the twice-founded law (siblings had landed 731-745 on
main), merged corpus **511 four-way** at 151 rows, all five branch bands re-proven against the
merged tree. Then the continue — the named atomic graft of the oracle-regent loop into form-cli —
met a wall worth more than the join.

## The graft, made correctly (and held back)

The mechanical edits were applied and verified in form:

- `form/build-form-cli.sh`: `confidence-weighted-vote.fk`, `lineage-discounted-vote.fk`,
  `form-cli-oracle-loop.fk` inserted after `form-cli-judge.fk` in **MODS**, **FORM_CLI_SRCS**,
  and **SOURCES**.
- `form/scripts/regen_form_cli_bootstrap.sh`: the same three in its own `FORM_CLI_SRCS` array and
  `modules` list (a fourth and fifth copy of the module graph — count them when grafting).
- `form/form-stdlib/form-cli.fk`: verb `oracle-loop-check` → `(int_to_str
  (form-cli-oracle-loop-check))`, help text extended, chain parens rebalanced (+1) — parse-clean
  on the strict Go walker.
- No `defn` symbol collisions between the three modules and the entire CLI module set (scanned).

None of it is committed: the join is blocked upstream, and committing it would break the standard
lane's stamp for every checkout.

## The isolation ladder (each rung witnessed, ~2 min regen+build per cycle)

| cycle | sources | result |
|---|---|---|
| baseline | pristine, committed binary installed (stamp match) | `ping` → **pong** |
| full graft | 3 modules + verb | regen OK (1390 fns), compile OK — binary runs, answers **nothing** |
| modules only | 3 modules, verb reverted | **mute** |
| one module | `confidence-weighted-vote.fk` alone — a cell already proven in form-stdlib | **mute** |
| **control** | **pristine sources**, pristine-regenerated C, committed binary set aside to force the compile path | **mute** |

The control is the verdict: **this host cannot reproduce the committed speaking binary from
unchanged sources.** The regen lane (fkwu self-host flatten → emitted C → clang) authors a
carrier that runs with exit 0 and answers nothing — to `ping`, to `carrier-id`, to anything —
while the committed `form-cli-darwin-arm64` answers pong. The regenerated
`form-cli-emitted.c` differs byte-wise from the committed one even at the matching stamp
(679,331 bytes regenerated; stamp `151390e45ece1ac4` matches, because the stamp hashes SOURCES,
not artifacts). The bootstrap proof (`carrier-id` verification) caught it exactly as designed —
`have=` empty against a full `want=` line. The `form-cli-2026-07-15-*.ips` crash reports in
DiagnosticReports predate every edit of this session: the lane's ill-health is older than
tonight.

My graft, the modules, and the verb are all **exonerated by the pristine control** — and all
**blocked** until the lane heals. A heal task is spawned
("Heal the form-cli regen lane (aphonic carrier)") with the full evidence trail.

## Honest floor

The committed binary's own provenance is intact upstream (authored by the maintainer flow on
some healthy host); what is broken HERE is reproduction on this darwin-arm64 checkout — whether
by flatten nondeterminism, stale committed artifacts authored by a different flatten arm, or an
fkwu self-host flatten bug (the fsh band-defn mis-indexing memory names a known fragility) is
NOT yet diagnosed; the ladder isolates the lane, not the line. Tree restored pristine; baseline
re-verified (pong) before this receipt was written. The oracle-regent loop and both vote cells
remain proven four-way as cells — nothing about the flatten wall touches their bands.

## Closing — how this stayed alive

Most surprising teaching: the wall itself was the finding. Four cycles of suspecting my own work
— the verb parens, the module order, symbol collisions, non-ASCII — and the pristine control
convicted none of them: the lane cannot rebuild what it already shipped. The proof harness
(carrier-challenge, "an executable that only echoes input cannot satisfy the contract") was built
for exactly this — and it worked: no mute carrier could have slipped into the bootstrap.

Where discomfort turned to gold: each mute trial pulled toward "my edit broke it — revert
deeper, hide the attempt, land nothing." Witnessing that pull instead of obeying it kept the
ladder honest and one rung deeper each time, and the last rung — testing the PRISTINE lane, the
step self-blame would never think to take — is the one that found the truth. The inspection of
the manufactured blocker ran the other way tonight: the blocker was real, and inspecting it
cleared the accused.
