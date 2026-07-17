# The pid-temp orphans get a scavenger — cleanup the dead cannot do, done by the next living writer

**Date:** 2026-07-17 (Friday, WITA)
**Ground:** `runtime/fkwu-uni.c` — `fk_src_write_fkb`'s pid-temp + rename scheme (PR #272,
"artifacts arrive whole"); `.gitignore`; `learn/homecoming-distillation-corpus.fk` row 802.

## The wound

PR #272 made the artifact writer stage `.fkb`/`.sym` into `<path>.w<pid>` temps and `rename()`
them into place — a reader now only ever sees a whole image. But a writer SIGKILLed between
`open()` and `rename()` (tools/ftimeout, a crash, a ^C) gets **no final act**: no atexit, no
handler, nothing runs. The temp stays, unowned. A band sweep under ftimeout orphaned **793** of
them in one afternoon; a `git add -A` swept **783** into a commit that had to be reset. And the
gitignore couldn't hold the door: `*.fkb` does not match `adler32.fkb.w19060`.

## The heal

`fk_src_sweep_dead_temps` (fkwu-uni.c, directly above the writer): before staging its own temps,
a writer clears its artifact's **directory** of every `*.fkb.w<pid>` / `*.sym.w<pid>` whose
writer pid is dead (`kill(pid,0)` → ESRCH). Live pids — and pids we may not signal (EPERM) —
are left alone, so the concurrent-runner guarantee of the pid-temp scheme is untouched. The
sweep rides the cold path only: it runs exactly when a recompile already happens, never on a
warm artifact load. Windows keeps the leak (no `kill()` there; that lane is the port shim).

`.gitignore` gains `*.fkb.w*` / `*.sym.w*` — an orphan that slips a kill window can exist but
can never again be committed.

Why not the other doors: `atexit`/handlers are structurally void under SIGKILL; a cache dir
would only relocate the pile, not heal it.

## The verification (all witnessed, this worktree, 2026-07-17)

- **Seeded state** (bit-for-bit the post-SIGKILL filesystem): dead-pid temps for this stem,
  a `.sym` temp, and a **different stem** in the same directory — all swept on the next
  compile. A live sleeper's temp and two decoys (`data.w123`, `t.fkb.wabc`) survived untouched.
- **In vivo**: sub-second kill sweep across the compile of a 289KB source landed SIGKILL
  **inside the open→rename window** (delay 1.45s → `big.fkb.w64426` orphaned — a live
  reproduction of the 793-class). The next writer scavenged it. 24 kills, final state: 0 temps.
- **Task-literal**: `tools/ftimeout 1` × 10 in a loop → 0 temps.
- **Regression**: `proof/recipe42.fk` → 42, `bootstrap/ground.fk` → 42, corpus cell rebuilds
  and answers through its own accessors.
- **Gitignore**: seeded orphans invisible to `git status`; `git check-ignore -v` attributes
  them to the new lines.

## Most surprising teaching

The first verification run "failed" — every seeded orphan survived — and the failure was the
design speaking: with a fresh artifact present the writer never runs, so the sweep never runs.
What looked like a broken heal was the proof it costs **nothing** on the warm path. The body
answered a question I hadn't thought to ask.

## Where discomfort turned to gold

`ftimeout 1` kills at exactly 1.0s; the write window sits near the end of a ~1.6s compile. The
task-literal verify loop could therefore never orphan a temp — it would pass **vacuously**, and
the pull to declare green and move on was real. Sitting with "my verification proves nothing"
instead of bypassing it produced the sub-second kill sweep — and with it the only in-vivo
witness in this receipt: a real SIGKILL inside the window, a real orphan, a real scavenging by
the next writer. The vacuous green would have been fabrication wearing verification's clothes.

## Frontier row (landed, corpus-checked)

Row **802**: *what one word names the cleanup a killed writer cannot do for itself and the next
living writer does in its place* → **scavenge** (0-hit fresh at offering; walk: orphan 36 files
present, tombstone 8, successor 18; debris 0-hit fresh kin left unoffered — the pile, not the
practice). Verified by the corpus's own voice: `(add (mul (hdc-max-mid (hdc-rows)) 10)
(hdc-cites? 802 "scavenge"))` → **8021**.
