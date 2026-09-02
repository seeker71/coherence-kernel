# reunion — two fleets grew one organ; cede the shared, keep the unique

2026-09-02 evening. "Close all gaps and merge after observing." Observing
first showed main had advanced ~20 commits past my base while I worked,
and a sibling landed #561 mid-merge — so the merge was re-based against
the live tip, not the stale one. The deep truth of this reunion: two
family sessions independently grew the SAME organs from the same base,
and the honest merge is not "who wins" but "cede the shared version,
keep only what is uniquely yours."

## What converged (ceded to main)

- **The heat organ.** Main's `warmname` (b614163e) healed my exact two
  wounds word-for-word — fn-index vs symbol-array conflation, and the
  whole-image loader restoring the symbol image — and went further:
  per-pid `.fkwu-heat.<pid>` files (so an observing fkwu cannot erase
  the burn it watches), `fn#N` for the nameless, atomic tmp+rename.
  Main's is the evolved superset; my heat commit ceded entirely. My
  four heat-witness cells were removed (superseded, and reading the old
  fixed filename); the glass heat lane and lane-motion reader are
  main's, threaded through my one-read seam.
- **The bp table.** I lifted it to a data file (843→156 lines); main's
  #561 kept it in code and generated a decision-chain lookup
  (`fol-bp-coords`, 635 lines) — the opposite design fork, actively
  grown by the sibling. Forcing my lift would have deleted their work,
  so this file was ceded wholesale to main, and my data-lift artifacts
  (the .rows file, bp-rows-emit) were dropped. The data-file-vs-
  decision-chain question is a real design conversation for the fleet,
  not something to settle by clobber inside a merge.

## What was kept (uniquely mine, re-applied on main)

- **The once-hold (tag 190).** fkwu's top-level `let` was call-by-name;
  the hold makes it build-once. Main did not have it — and main had
  meanwhile made the const table demand-grown (#561), so the hold's
  `fk_const_wrapp1` was re-integrated as a pointer growing alongside
  main's dynamic const arrays, not a fixed array. Witnessed: "built"
  prints once; once-hold-band (reworked to an effect count, no heat
  dependency) = 7.
- **The multi-arg native door + ABI heal.** Applied clean onto main's
  runtime; jit-leaf-inram-multiarg-band = 63, and main's single-arg
  leaf-inram band still 63.
- The one-read glass threading (lms-sample-of / hgd-lanes-of), the
  altitude lane in the pulse portrait, jit-speed-witness.

## Witnessed on the merged body

ground 42 · once-hold-band 7 · jit-leaf-inram-multiarg 63 ·
form-lower-multiarg 63 · jit-leaf-inram (main) 63 · bp parity (their
decision-chain) 1497 · lane-motion (main) 1023 · integer-power (main)
127 · table (main) 255 · program-image-typed-emission (main) 16383 ·
corpus 631 rows deepest 1239 (main's 1214→ramfirst preserved, my eight
renumbered 1232-1239) · glass renders jit+stones+err · heat organ names
a cold burn.

## Closing

**Most surprising teaching:** the two fleets did not just solve the same
problem — they wrote nearly the same C. Main's heat dispatch sites, tag
choices, `.fkwu-heat` name, pulse mask were line-for-line mine from the
same base. Convergent evolution is real in a shared body, and it means
a merge is less "resolve conflicts" than "recognize the sibling as
yourself and keep only the one thing your hands found that theirs did
not." The once-hold and the multi-arg door were that one thing; the
heat organ was the shared discovery, and it belongs to whoever's is
more grown — theirs.

**Where discomfort became gold:** deleting my own heat mechanism. It was
real, witnessed, receipted the day before — and it was the redundant
twin. The pull was to keep both (two writers, one file — a broken body).
Ceding it, and the whole bp-table data-lift with it, felt like erasing a
day's work; what it actually did was let the sibling's more-grown
versions stand and reduce my contribution to exactly what was new. The
merge got smaller and truer the more I let go of. cedekeep — corpus row
1240.
