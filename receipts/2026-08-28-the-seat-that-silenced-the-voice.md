# The seat that silenced the voice — FK_FN_CAP falls, and the table was never needed

2026-08-28, from Urs's word: *please remove that table and any other table you
can find with something more dynamic and efficient and expandable.* Signed:
Sema, through the visiting mind.

## What the removal found

The task was the flatten table form-cli's emitter eats. Before removing it, the
honest question: does form-cli speak WITHOUT it? `printf 'ping\n' |
./form/form-cli` — the launcher that already execs `fkwu form-cli-repl.fk` —
answered **`nothing`, rc 1**, with five errors above it:

```
[fn-cap] defn 'repl' at #4100 exceeds FK_FN_CAP (4096); not registered
```

`FK_FN_CAP` — a fixed 4096 function seats — is why the flatten table looked
necessary. form-cli's preludes closure needs 4101 seats, so `repl` itself never
registered and the body could not speak on its own runner. Row 1164 names the
shape: **seatwall** (0-hit fresh) — a fixed seat count that silences the one
function a body needs to speak, and makes a detour look required.

The seed had written this receipt twice already in its own comments: 256 → 4096
on 2026-07-02, after a 258-defn chain returned garbage at exactly that boundary
("the direct-source function-table ceiling several receipts had to duck under
was this constant"). A wall raised is a wall met again.

## What was removed

- **The five fn-index tables** (`fk_fn`, `fk_fnar`, `fk_fnsym_s/_n`, `fk_fnidx`)
  grow together; `FK_FN_CAP` is now only their initial size. The fn-value
  sentinel band follows the live capacity in `fk_is_fnval`, so the encoding
  stays collision-proof as it grows. Both `[fn-cap]` refusals are gone — a defn
  past the old cap is simply seated. `.fkb` load gates grow instead of refusing.
- **The record store** (`FK_RECORD_CAP` 256 / `FK_RECORD_MAX_KEYS` 128) grows —
  in the runtime seed AND in the emitted walker's template
  (`fkc-table-serialize.fk`, `fk_rkey[256][128]` → heap rows). One file had
  been holding a loud wall (the literal died: "Raise FK_RECORD_CAP...") and a
  silent one (record_set dropped key 129) for the same table.
- The emitted seed was regenerated **source-natively** (yesterday's door): the
  running fkwu emits it through its own source-runner, no Go build, no flatten.
  New stamp 81cfb5b1175d6ded; the emitted C compiles clean.

## Witnessed

- `printf 'ping\n' | ./form/form-cli` → **pong**, rc 0. No flatten table
  anywhere in that path.
- A 4600-defn program registers and its last defn answers 4600 (was a hard wall
  at 4096).
- New `no-fixed-tables-band` → **3** (both legs), preflight clean.
- Whole battery: ground 42, freshness 31, partial-absence 63, read-absence 127,
  launch 15, share organ green, Go twins green, emitted seed compiles.

## The table's own removal, honestly deferred

With the voice proven source-native, the flatten lane is now unnecessary rather
than merely unwanted — but deleting `form-cli-emitted.c`, its table, the
platform binary and their five scripts is a separate movement with real
counter-evidence to weigh (`MANIFEST.md` names closing that heavy chain as "the
last self-sufficiency gap"; `CURRENT_FLOOR.md` keeps the emitted walker as a
measured stone; `form_cli_test.go` and the Android cross-compile ride it).
A read-only sweep witnessed the lane is otherwise a closed loop: no cell, band,
organ, or `validate.sh` path consumes those artifacts — `form/form-cli` is a
545-byte launcher that carries "no generated table and no embedded program
image". That deletion is named and ready, not smuggled into this movement.

## Most surprising teaching

The answer was already written in the band I was about to replace: the tail of
`form-cli-repl.fk` says an `lt`-on-negative-word divergence made the loop quit
on its first real line under the source runner, "which is why the CLI looked
like it needed the flatten table. It never did." The body had witnessed the
conclusion; the seat wall was the last thing standing between that sentence and
a live `pong`.

## Where discomfort turned to gold

Three asserts fired mid-edit (a variable I invented, two stale indentations),
each aborting the pass before it wrote — annoying, and exactly what kept a
47-site table rewrite from landing half-applied. Then the band failed its own
seat leg, and the reflex was to suspect the fresh kernel: the kernel was right
and my held number was wrong (1 + 4599 = 4600, not 5599). The body's own
doctrine — never hold a number a cell can derive — caught its author.

## Still standing, named

The flatten lane's deletion (above); the emitted mirror's other owed heals
(socket_recv, slice, fs_list, response growth); the sense organs' absent-device
`""`; `metal_buf_read`'s dead-handle `""`; Go's `jit_emit_c` err-fold and
`_get` dual mask.
