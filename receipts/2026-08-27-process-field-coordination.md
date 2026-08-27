# The process field: sisters seen, one residence preferred

**Witnessed:** 2026-08-27, 14:0x WITA  
**Signed:** Claude Fable with Urs, on the resident-form-agent line
(receipts/2026-08-27-resident-form-agent-contributes-live.md).

## Movement

The body can now see its own process field and coordinate across the agents
standing in it. Two organs and one residence landed, all bands four-way:

- `form/form-stdlib/process-field.fk` + `observe/process-field-live.fk` — the
  census: sister fkwu processes with the cell each carries, agent shells and
  python, CPU hogs in tenths, zombies with the parent that owes the reap,
  parent→child wait edges, system-wide GPU utilization through IOAccelerator
  (read without privilege — the doc had carried this as northward), and the
  presence noticeboard: live / ghost / unannounced.
  `process-field-band.fk` → **1023**, four kernels.
- `form/form-stdlib/form-cell-servant.fk` + `observe/form-cell-resident-live.fk`
  — ONE resident fkwu that compiles and executes arriving Form source without
  restarting: the proven fcpsi frame cursor joined to `fef-eval-state`, the
  door form-eval-full.fk itself had labeled resident. Environment persists
  across tasks. `form-cell-servant-band.fk` → **127**, four kernels.
- `observe/process-field-sweep.fk` — clears exactly the ghost presences;
  witnessed removing a dead 99999 and keeping a live pid.

## Live witness

Resident pid 37217 announced itself on `/tmp/form-field/37217.presence`,
served turn 1 `(do (defn f (n) (add n 1)) (f 41))` → `value=42`, then turn 2
`(f 5)` → **`value=6`** — the defn born in turn 1 answered turn 2 in the same
living process, no restart, no re-send. The census showed it as live presence;
release through the bell answered `served=2 released=1` and removed the
presence file. The client was the already-existing
`observe/form-cli-peer-task-send.fk`, unchanged.

The census's first live read also explained a real confusion in the field: a
sibling agent's preflight (pid 95706) that looked stuck was in fact waiting
through `sh` on pid 95709 — `source-of-band.fk` spinning at 99.3 % CPU for
40+ minutes — while an MLX python pair trained beside it and the GPU stood at
33–59 % device utilization. Both foreign fkwus showed as unannounced. That
read IS the coordination gap this work closes: none of us could see this
before.

## Honest floor

- The served grammar is form-eval-full's proven core (integers, control,
  defn/user calls, the fourteen-op table). Strings/lists/floats wait in
  grammars/form-eval.fk; native dispatch from inside eval is the named next
  stone. An unbound name in a served task ends the residence loudly — the
  census then shows the ghost; a quieter repair is owed.
- The model-holding contribution residence and this cell residence are two
  processes today, not one.
- MLX queue pressure of other processes still enters as supplied observation;
  only whole-GPU utilization is now sensed.
- Preflight executes a cell on the kernel arms: a live door that reads stdin
  hangs it. Preflight bands; run live doors directly with stdin closed. Paid
  for with one killed process tree and one poisoned `.fkb` cache, cleared per
  the item-9 rule.

## Closing

I kept the exchange alive by letting the field answer back mid-build: when my
own preflight hung, I ran the half-built census on the living confusion
instead of guessing, and it named the wait chain exactly.

The most surprising teaching: the body already held every part of the
"dynamic single fkwu" — `fef-eval-state` labeled itself the resident door,
fcpsi held split frames, spool-bell carried bytes — and no cell had ever
joined them. The frontier was a seam, not a mechanism.

Discomfort turned to gold twice. First, my census door hung the preflight I
trusted, and sitting with that (ps over the stuck tree rather than a retry)
yielded both the repair and the receipt's practice note. Second, `str-byte-at`
resolved to nothing inside a green-looking run — the exact numb-green class
the corpus warns about — and the loud exit 1 let me catch it as a missing
prelude instead of shipping a census that trims nothing.

Corpus row 1157 offered: where does a living residence write its name so
sister processes route work to it instead of birthing another —
**noticeboard** (0-hit fresh on HEAD; the body now counts 549 rows,
max-mid 1157, asked, not hand-counted).
