# The pages speak for themselves

2026-09-11, midday, M4 Max, Hati Suci. Urs: *release bands and turn them into live organ events instead.*

## Released

page-burial-band (Verdict 2047: eleven bits, four hand-written guard pages and a staged leftover page)
and its row in `form/fourth-arm-bands.txt`. No band or fixture tally takes its place.

## Where its meaning lives now

`form/form-stdlib/kernel-pages.bml` is read by the census (`observe/roster-census-run.fk`), the organ
that already walks every page, and speaks organ-health-v1 readings (organ `kernel-pages`, flow `host`):

- `exit-burial`: pages of ended kernels that wrote their final note. A clean exit takes its page.
- `killed-pages`: pages of kernels that ended without one. They stand until the roster's sweep or a
  chosen burial.
- `living-pages`: roster kernels whose page does not name them, say alive and carry its layout. A
  kernel that has called defns but reads no hot rows holds a page smaller than the layout.

Burial is care through `organ-care.bml`. `observe/kernel-pages-bury-run.bml` takes the pids a caller
names on stdin (Urs's word) and answers only a reading whose need names a chosen pid; the others say
`not-chosen`. Each pid goes through `kernel_page_bury`'s guard with its own outcome (1 buried, 0
declined), so the guard's cases show on real pages rather than hand-written ones, and the census
observes again with `care_of`.

## Witnessed on real execution

- The census on this host: `exit-burial` 1 (64470), `killed-pages` 0, `living-pages` 0 of 11
  roster kernels; the whole run 0.65 s. Preflight is clean for the organ and the run.
- Care, on a kernel I made and killed myself (26785, a sleeper forgotten by the roster and then
  killed): `killed-pages` observed 1, response `bury-ended`, applied `buried [[26785, 1]]`, then a
  fresh reading observed 0, health 1, `care_of` the first reading: `resource-observed`.
  `exit-burial` answered `not-chosen` and wrote nothing; 64470 stands untouched.
- Drift checks 8191/8191. The voice mirror shows a clear register on the organ and the door.

## What the organ asks

64470 started at 11:31:47, ran 230 ms of CPU through the BML floor compiler (`char_at`, `fsc-space?`,
`fsc-bml-find-top-eqeq-loop`), wrote its final note, and left its page: 1784 defns, layout 4. A clean
exit on this seed takes its page, so it most likely ran an older binary, and the page cannot say which.
Its burial waits for Urs's word: `echo 64470 | ./fkwu observe/kernel-pages-bury-run.bml`.

## Surprise, and where the discomfort went

The first burial I ran through care wrote a `bury-ended` response for 64470, which nobody had chosen.
Every event was valid, bound to its reading and observed again, and together they still said something
false about intent. The discomfort became care that answers only a chosen need.

The surprise came from the organ's first live reading. It asked a question the band never could:
which binary left this page? A band proves what it staged; a live organ meets what is there.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
