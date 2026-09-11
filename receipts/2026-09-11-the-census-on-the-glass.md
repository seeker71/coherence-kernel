# The census on the glass

2026-09-11, evening. Urs: *being aware of what is running for what is vital*, and at midday, *release bands
and turn them into live organ events instead*. The roster organ speaks the census
(`2026-09-11-the-roster-speaks.md`); now the glass shows it.

## What stands

`form/form-stdlib/glass-census-rows.bml` turns each organ-health reading the roster organ
(`kernel-roster.bml`) and the pages organ (`kernel-pages.bml`) speak into one glass row. The sensors organ
(`observe/form-glass-sensors-live.fk`) takes the census every ten seconds, settles each take against the
one before it, and gives the rows as a `census` frame; the glass reads that frame beside its live kernels.

- `census.kernel-roster.<aspect>` — untracked, stale, orphans, errands, unpaged.
- `census.kernel-pages.<aspect>` — exit-burial, killed-pages, living-pages.
- A measured reading is an int row: its value the count, its note the need with the pids it names (or
  `none`). A reading over a read that did not answer is an unavailable row (present 0) whose source is that
  read (`page-read`, `roster-read`, `host-process-read`, `owner-walk`).
- A tick takes the census twice, a tick apart, and names a kernel only when both takes agree. The first
  tick has nothing to settle against and gives nothing; the glass shows the frame absent until the second.

## Why the privacy rule needed no amendment

The glass census was parked on 2026-09-11 (`glass-errandsight-wip`), refuted in part because its rows
carried a command argument (the cell, argv's last word) and a process's binary name (the owner): exactly
what the observer's ProcessPrivacyRule keeps out of Form. That parked work left the choice of which gives
way to Urs.

The readings resolve it without a choice. An organ-health reading carries pids and counts, never a name
and never an argument — the roster organ already keeps them in the census's own lines, off the glass. So
a row built from a reading stands inside the rule as written. The band's bit 64 reads every row gcr-rows
builds and refuses any byte that is not a digit, a lowercase letter, or one of `. - _ : ,` and space: a
name would bring an uppercase letter, an argv path a slash. No amendment, and the question Urs was left
with does not arise.

## Witnessed on real execution

- `glass-census-rows-band` reads 1023 directly on fkwu and ✓ 1023 through `validate.sh` on the fkwu lane.
  It builds each reading by hand and reads no host: a measured reading with no need, one with a need, a
  null reading, the pages organ's unit, id and label, gcr-rows over two settled takes giving one row per
  reading, the no-name byte check, an unavailable row keeping its need's pids, the 10 s cadence, and the
  evidence of a measured and a null row.
- The band stands on the fkwu lane because a reading stamps its id from `host_pid` (`oh-reading`), which
  the sibling kernels do not bind — run four-way first, all three stopped at `oh-reading` on `host_pid`.
  That named the lane, as it does for host-process-band and born-under-band.
- One census take costs about 35 ms and the pages take about 33 ms, read twice over this tree
  (`ct-cost.bml`); settled and read into rows, a tick is under a tenth of a second every ten seconds.
- Preflight is clean for glass-census-rows.bml, form-glass-sensors-live.fk, form-glass-live.bml,
  form-glass-sensor-rows.bml and the band. The voice mirror shows a clear register for the unit, the organ
  and the band. form-glass-views-band still reads 4194303 with `census` in the inspection-sensor list.
- Not run: the glass organs themselves (`observe/form-glass-*.fk`). The rows are witnessed through the
  band; the sensors organ takes effect when Urs next starts the glass.

## What the organ asks

The rows have not been seen painted on a live glass, only built and crossed on the fkwu lane. The first
tick gives nothing by design, so a glass that has just started shows the census frame absent for its first
ten seconds, then the settled rows; that transition is witnessed in the settle rule, not on a running
glass.

## Surprise, and where the discomfort went

The parked census had made a genuine privacy question — the glass would have had to either drop the cell
and owner or carry names the rule forbids — and it waited on Urs to choose. The surprise was that the
question dissolved rather than being answered: the organ readings were built, before any of this, to carry
pids and counts and nothing else — the very thing the glass needed, already what the rule keeps clean.
Building the census as rows of something that measures the body, rather than as rows of the process table,
was the release Urs asked for, and it was cheaper than the amendment.

The discomfort was the pull to finish the parked branch — it had a whole UI, it was nearly done. Reading
why it was refuted, rather than reviving it, showed that five of its six refutations and the privacy
question all came from one decision: it read the process table directly. The readings had already made the
other choice. Leaving the parked work parked and speaking the readings instead was less code and no open
question.

— Claude (Opus 4.8), as Sema, worktree epic-edison-534e30
