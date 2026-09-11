# The roster speaks

2026-09-11, afternoon. Urs, at midday: *release bands and turn them into live organ events instead.* The
pages spoke first (`2026-09-11-the-pages-speak.md`); the roster follows their shape.

## Released and kept

Released, staged as deletions, with their rows gone from `form/fourth-arm-bands.txt`: roster-census-band
(63), roster-adopt-band (63), roster-register-band (15), zombie-alive-band (15). No band or fixture tally
takes their place.

Kept: host-process-band (127) and born-under-band (31). host-process-band carries clauses no lane I may
run shows live: a kernel started as `./fkwu --src a.fk` names a.fk, a bare `./fkwu` names no cell, and
`rce-base "fkwu"` reads a path with no slash. born-under-band's fourth bit, a kernel that ends by its own
reading, lives only in the test watcher's `.said` file: the glass organs end silently when born-under
releases them (`form-glass-organs.bml:210-214`).

## Where their meaning lives now

`form/form-stdlib/kernel-roster.bml` (organ `kernel-roster`, flow `host`) speaks what the census
(`roster-census.bml`) finds, beside the pages (`kernel-pages.bml`), in `observe/roster-census-run.fk`:

- `untracked`: kernels the roster lacks. Need `roster-slot`; offers `adopt`, `keep`.
- `stale`: roster pids that are no living kernel. Need `kernel-identity`.
- `orphans`: kernels whose owner walk ends at launchd. Need `waiting-owner`; offers only `keep`. A stop
  is Urs's word, and no provider here answers it.
- `errands`: kernels whose row the host told short: no cell, no binary, or an owner walk that ends
  unread, gone or at an ancestor with no binary.
- `unpaged`: processes named fkwu that are no living kernel. Offers evidence, never a slot.

A kernel is its page. Every kernel opens `/fg-k<pid>` as it starts (`fk_nodes_init` calls
`fk_live_open`), whatever its binary is named, so the census asks every pid's page name once and counts
as a kernel a living process whose page names it, says alive, and was opened as or after the process
started. A kernel run from a copy named `vk2` is a kernel; a page left on a pid another process holds now
is not. The name read, `host_processes "fkwu*"`, only finds the processes that call themselves kernels.

Each door now says what it did not see:

- `kernel_page_ended pid` answers 1 for an ended kernel's page, 2 for a page it read that is not one, 0
  for no page, -1 for a page the host will not let this kernel read. A sandbox answers EPERM for a page
  that stands and ENOENT for one that does not, so the two stay apart.
- `host_process pid` adds when the process started and which fields the host withheld (1 exe, 2 cwd,
  4 argv). A binary removed under its process reads empty with nothing withheld (ENOENT); a sandbox's
  EPERM reads empty with 1 withheld.
- `host_processes "fkwu*"` names a process of this user whose name `proc_name` withholds by the process
  table's 16-byte name, answers a pid alone for a process neither will name, and answers -1 when the host
  will not list its processes. A bare `*` answers nothing. The exact `fkwu` the glass asks keeps its
  meaning.
- `kernel_live_pids` and the roster's dead-slot sweep ask whether a pid has gone (ESRCH or a corpse),
  not whether this kernel may signal it.

A reading stands on reads. untracked and stale stand on the pages and the roster; orphans on the pages,
the kernels' rows and every owner walk; errands on the pages and the rows; unpaged on the name read and
the pages; exit-burial and killed-pages on the withheld pages of processes that have left; living-pages
on those of living ones and on the roster. Over a read that did not answer, a reading's health and
observation are null, its need names the read (`page-read`, `host-process-read`, `roster-read`,
`owner-walk`) with the pids it missed, and its evidence names the reads that did not answer: the roster
organ's beside what each read saw (`"unanswered":["page-read"]`), the pages organ's as that list alone.
The census's own line says `unseen` for the same lists. The pages
organ takes two reads half a second apart, as the census does.

The owner walk passes the shells this host lists in `/etc/shells` beside the common ones and login, and
passes an ancestor as a kernel only when the census found its page living: a process named fkwu with no
living page is no kernel, and the walk stops at it as the owner. `rce-kernel-of`, called with no page walk
taken, asks each ancestor's own page as the walk does, and an ancestor whose page the host will not show
reads `withheld`. A kernel whose kernel-parent runs the same cell is its
worker, not a duplicate. Care takes a word as a pid only when it is a positive number the kernel writes
back the same.
## Witnessed on real execution

Every pid below is a process I started; each ended on its own. The sandboxes are `sandbox-exec` profiles
over my own census: `pages` denies other kernels' pages, `both` adds other processes' info, `s5` denies
every page but `/fg-kernels` and its own, `s2` other processes' info, `s4` every shared-memory read, `s1`
the process list, `s6` the process table (`kern.proc`) and other processes' info, `sig` signals to other
processes, `infosig` both of those, `s3` reading `/fg-kernels`.

- Pages withheld (`pages`, `both`, `s5`): the census found only its own page; all five roster readings,
  exit-burial, killed-pages and living-pages read null with `page-read` naming the 12 other kernels or the
  standing ended page 73204. Before this, the same shapes read health 1 on six or seven readings, and
  unpaged named 13 living kernels at health 0.
- Info withheld, pages readable (`s2`, `infosig`): all 13 kernels found by their pages; untracked,
  stale and unpaged measured; orphans and errands null with `host-process-read` on the withheld rows.
- `s1`: only unpaged null. `s4`: all eight null, with `roster-read`. `s6`: 564 unnamed processes and the
  standing pages whose start the host would not tell leave every roster reading null. `s3`: untracked,
  stale and living-pages null with `roster-read`; the rest measured.
- Signals withheld (`sig`, `infosig`): the roster held all 13 kernels, and the roster read afterwards
  still held the nine other sessions' kernels it held before (68089, 55457, 68091, 68374, 68093, 68092,
  68373, 2175, 3596).
- A copy not named fkwu: 74663, run from `vk2` and forgotten, read untracked at health 0. Care with
  `74663S` answered `not-chosen` and the roster still lacked it; care with its pid adopted it
  (`[[74663,1]]`), and the fresh reading read health 1 with `care_of`. 59246, run from `vk3` and held,
  is a kernel, not stale.
- A removed binary: 74664, whose copy I removed under it, read errands at health 0 with nothing
  withheld. A C program named fkwu, 74665, read unpaged at health 0.
- Orphans: kernels two `sh`, two `ksh` and two `tcsh` shells under pid 1 (63190, 63194, 63196) were each
  owned by launchd and named.
- A waiter named fkwu: 97309 ran under 97305, a C program of mine named fkwu that waits for it, opens no
  page, and whose parent is pid 1. The census read 97309's owner as `fkwu` 97305 and no orphan, and in
  the same census unpaged named 97305 at health 0; its control, 97310 under the same program named
  `waiter`, read owner `waiter`. Before the owner walk asked for a page, the same shape read the kernel
  an orphan while unpaged named its waiter.
- A null reading's reason: under `pages` all eight readings read null, the roster organ's evidence
  `{"pids":[],"host":11,"roster":1,"kernels":1,"unanswered":["page-read"]}` and the pages organ's
  `["page-read"]`; under `s3` untracked, stale and living-pages read null with `roster-read` named the
  same way, and the rest were measured. With no sandbox the census read 11 named, 11 in the roster and
  11 kernels, every reading measured.
- The rules on built takes (`h-syn.bml`, fifteen checks: readings over each missing read, settling,
  rows, the owner walk, lines and shells, page identity, the name read, the pages organ's two takes, a
  pageless ancestor named fkwu that owns and a paged one that is walked past) read 32767.
- The name read: a bare `*` answered 0 rows (339 before); under `s2` `fkwu*` named all 10 kernels (1
  before); under `s1` it answered -1 (an empty list before).
- Bands on this tree, each read directly on fkwu at rc 0: host-process-band 127, born-under-band 31,
  form-glass-organ-care-band 4095, gift-bookkeeping-band 1023, form-glass-observer-band 67108863,
  own-word-band 65535, perception-rows-band 65535, form-glass-heal-band 262143, carrier-mass-band 16383
  (its sources read whole once the release was staged), and binary-freshness-band 31 after the rebuild.
  Through `validate.sh`, host-process-band and born-under-band read ✓ at rc 0. Ten bands print ✗ over a
  reference arm this diff does not touch, and leave with 1 now that validate.sh keeps the run's status:
  carrier-mass, gift-bookkeeping, form-glass-heal, form-glass-observer, field, cell-store, jit-lens,
  native-session-memory, form-local-autonomy-eval and qwen35-form-cli. binary-freshness-band is fkwu's
  own: through `validate.sh` the three reference arms stop in `bfb-bool-distinct`, which compares the
  interned true and false nodes.
- The seven BML units and cells this diff adds or changes each lower through `bml-floor-compile.fk` at rc
  0 with no decline, under the complete admission main landed the same day. roster-census.bml's class
  had closed on a bare `0;`; main removed it, and the rebase onto main kept that. `cc -fsyntax-only`,
  the Windows cross-compile and the Linux-shaped check read 0 errors. Preflight is clean for every
  changed unit, cell and witness; `flatten/gen-source-walker-table.fk` reads 9 errors and 2 unresolved
  names, the same as its HEAD copy.
- validate.sh: the builder found its exit handler leaving every failed run with 0 (form-glass-heal-band's
  `0 ok, 1 failed` left with 0). Main healed the same line in the same hours
  (`2026-09-11-the-exit-that-always-said-zero.md`), and this tree carries main's copy.
- The voice mirror shows a clear register for this receipt, `roster-census.bml`, `kernel-roster.bml`,
  `kernel-pages.bml`, `organ-care.bml` and `roster-census-run.fk`. `organ-health.bml` shows one count, from a comment
  already at HEAD. `validate.sh`, the seed, the walker table and `CURRENT_FLOOR.md` show whole-file
  counts; the lines added to them carry none of the counted words.

## What the organ asks

Not met live: a broken living page caught by one read and not the other. 1977 kernels started and ended
while the living-pages question was read sixteen times, and no single read named one; the two-read rule
is witnessed on built takes. A stale roster pid, a page left on a pid another process holds now, is
witnessed on built pages only. A kernel started with a flag before its cell (the band keeps the clause).

Off macOS `host_process` answers a start of 0, so a Linux census reads every standing page unread and
every roster reading null; no Linux lane answered here (the docker daemon is not running). A page another
user's kernel opened answers -1 here, so a host where another user runs kernels would read the census null;
not seen. `kernel_roster_adopt` still asks whether this kernel may signal a pid, so under a sandbox that
withholds signals it answers -1 for a living kernel and care there reads `need-open`.

A kernel with no page is seen only by its name. 81371, a copy named `vkp` run under a sandbox that denied
it shared memory, opened no page and ran 150 s in no list and no need; its twin named fkwu read unpaged.
By the page rule it is no kernel, and adoption needs a page.

By code reading, not seen: a page left standing on a pid another process now holds, with no roster slot,
is in no reading while that process lives (`rce-sort-with` drops a standing page whose living test reads
0); once the process leaves, the page reads ended and killed-pages names it. And `host_processes "fkwu*"`
asks the process table's name only of this user's processes (`fkwu-uni.c`, the uid test before the
16-byte name), so another user's pageless process named fkwu is not in unpaged; another user's kernel
reads null, since its page answers -1.

`born-under.bml:30` finds a kernel's own row by the exact name fkwu, so a kernel run from `fkwu-<stamp>`
reads its birth parent as -1 and is held for good. The glass organs ask it every two seconds for hours,
and I may not run them to watch a change there.

## Surprise, and where the discomfort went

The census had been using itself as the canary: did each read see me? A sandbox that lets a kernel read
only itself passes that question perfectly, so six readings claimed a healthy host one wall away from a
forgotten kernel and a standing ended page. The host had been telling the difference all along: EPERM
for a page that stands and ENOENT for one that does not, EPERM for a withheld binary and ENOENT for a
removed one. The doors were folding both into one answer. Once they said it, the organ's rules got
simpler: every reading stands on the page walk.

With no sandbox at all, 204 of 549 processes withhold their name from this user. Written as "any
withheld name leaves the read unfinished", the heal would have left every census null for good; the
probe, run before the rule, found the narrower truth: this user's processes, named by the process table.

The first live witness for the copied kernel showed nothing: `vk2` was back in the roster before any
care. Staying with that instead of calling the heal broken found the roster's own repair: every rest puts
a missing kernel back, so a resting kernel hides for under a second. A kernel in one long rest stayed
forgotten, and the census named it. The synthetic check read 16381 once. The miss was in my own
expectation (I asked unpaged for the wrong need), and the rule was right; the check now asks the rule's
own question.

— Claude (Opus 5), as Sema, worktree epic-edison-534e30
