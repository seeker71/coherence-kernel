# The let that was bound twice

2026-09-12, late afternoon, M4 Max, Hati Suci. I set out to learn why a clock stood still across sixteen
million conses. The walk led to two heals in the kernel's memory: a top-level let that ran its maker again
after a melt, and a table grown through a mapping it had just been moved out of.

## Carried

- **A top-level let is built once through a melt** (ecc54a661). The once-hold kept a let's value in its
  hold node, stamped with the melt generation. After any melt, the next read walked the initializer
  again: a clock let read the time of its next read, a printing let printed twice, a list came back as
  new pairs. The held value is now a melt root. fk_melt counts and copies it, fk_smelt marks it, and
  the hold returns it once held. once-hold-band gains bit 8, a walked churn of 20,000 conses between two
  reads. Its mark file is named by the kernel's pid, so a second build would land in the same file. It
  reads 15 on fkwu and Go, and 5 on the old hold.
- **A table grows from its home** (the same commit). Past a table's reservation, fk_store_grow sent the
  store private, which copies every table out and unmaps the mappings. It then reallocated the pointer
  it had been handed, which pointed into the mapping just let go. The process stopped (rc 134, pointer
  being freed was not allocated), or it answered a table of NUL bytes of the right length. It now takes
  the table's home and grows what is there. Off-field, lowering native-table-compile.bml crosses the
  2 GiB string reservation. That was receipt 45's regen that answered an empty table inside the script,
  and it now compiles.
- **The form-cli bootstrap follows** (3f3f31417).
- **twicebound is row 1502** (c0bbf611b).

Witnessed at c0bbf611b:
- **The let:** a let that prints as it binds prints once across a churn that melts. On e86210dc0's
  kernel and on rung 9 it prints twice. A clock let read before the churn keeps its time. The
  float-let probe's clock now brackets the churn at 74 ms, where it read 0.
- **The table:** with the table compiler's cache moved aside, the forced lowering off-field answers rc 0
  and the same 3,391,758 bytes twice, lowered and cached. Before the heal it stopped with rc 134 on both
  kernels, and the earlier by-hand "success" of that size was all NUL bytes. The regen, off-field, rc
  0; TestFkwu.
- **Bands:** every loop-lane band reads as receipt 46 records it, and every JIT band as rung 9 reads it.
  value-str 255, str-to-int-reading 127, once-hold 15, born-under 31, host-process 127; freshness 31,
  the corpus band 32767, drift 16383 of 16383, porcelain 0.

## Still open, measured

- **The shared field is full** at 2^26 node cells. Three validate.sh runs in another worktree have
  ground an fkwu child for over an hour; epic-edison traced them, and whether to clear them is with Urs.
  The field reset runs only when no other kernel is alive, so these runs used FK_FIELD_OFF.
- **Lowering native-table-compile.bml interns more than 2 GiB of strings.** It now goes private and
  finishes in 68 s; what builds that many bytes is not measured.
- **A single pass that needs more than 4,194,304 pairs** still falls to the walker (receipt 46).
- **value_str is still leaf mode 25 in C** (receipt 45).
- The open items of receipts 12 to 46 stand where they are not named here. Receipt 45's empty in-script
  table is closed.

## Surprise, and where the discomfort went

The surprise was that the clock had never stopped; the let was read twice. A memo that forgets on every
melt turns a binding back into a recipe, and a recipe that reads the clock tells the time of whoever
asks last. The same shape waited under the regen: a table asked to grow from an address it had already
been moved away from.

The discomfort was the timing table I had put in front of epic-edison, with "0 ms" for main. I had built
on those readings, and they were the kernel's error, not a measurement. The pull was to route around it,
time one process at a time and move on. Staying with it found the once-hold. Staying with the regen's
empty table, the open item receipt 45 carried unexplained, found the stale pointer: a backtrace of a
debug build, one realloc of an address fk_store_go_private had just unmapped. The gold is two silent
corruptions closed, one of them a table of NUL bytes that passed for success by its size.

Frontier word, row 1502: **twicebound**, a binding whose maker runs again when its memo forgets, so what
was bound once is bound twice.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
