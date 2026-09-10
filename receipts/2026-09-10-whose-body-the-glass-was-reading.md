# Whose body the glass was reading

2026-09-10, M4 Max, worktree `lucid-lehmann-db15b8`. `fkwu` fresh: ground 42,
freshness 31.

Three gaps were left open by this morning's receipt, each with the same
instruction: close it by measuring, not by reasoning. Two of the three answers
contradicted the guess the receipt had recorded, and the third found what the
receipt said to look for.

## The bound was one terminal's answer

The awareness door showed eight findings and, since this morning, said so. What
eight was had never been asked.

Measured on this checkout. The state held **24** — and 24 is not an accident of
arrival, it is `FGAMaxEvents`, the awareness shard's own declared ceiling. Every
finding is at most **94** characters, so at the glass's own width of 100 each
costs one row. The door's two chrome lines cost **4** rows of a 30-row surface.
All 24 fit in the remaining 26, with two rows to spare.

The door was showing a third of what its surface could hold.

So the bound follows the surface now, the way `sparkWidth` is already read from
its width: rows the terminal has, less the door's own chrome, spent on the
findings' own widths. The eight is not thrown away — one band bit keeps it. At
40 columns a 94-character finding costs three rows and exactly eight of them
fit the same budget. **Eight was a 40-column answer written down as every
terminal's.**

```text
findings shown=24 held=24 (all) — surface 100x30
```

`glass-awareness-door-fit-band.bml`, verdict 2047, four-way.

## The observer was not in the reading. Two bodies were.

The receipt said the queue depth alternated because the observation itself put
a request on the hearth queue. That is not what was happening.

Ten runs of the bounded door left the main checkout's `task.spool` at the same
276 bytes and the same modification time — 2026-09-04, six days old — and
created no spool in this body root at all. **The door writes nothing.**

What was happening: three sensor fleets stand on this Mac, one per checkout —
`/Users/ursmuff/source/coherence-kernel` and two Codex worktrees — and all three
published `glass.sensor.queue` into ONE machine-wide gift frame. The queue
sensor counts `.hearth/task.spool`, a relative path, so each fleet was counting
its own body's hearth and overwriting the others. A reader got whichever wrote
last.

The rows carried the discriminator the whole time, and no row ever showed it:

```text
queue-task-frames = 1   beside  inactive(no standing hearth; durable spools only)
queue-task-frames = 0   beside  inactive(no standing hearth; no durable spools)
```

The 1 was that six-day-old task frame in the main checkout. The 0 was a Codex
worktree with no hearth files at all.

The shard already carried the rule, applied to one sensor: *another checkout is
another speaker, not a newer publication of this speaker.* The rule is about
what a sensor **measures**, and the queue counts this body's hearth while the
storage catalog scans this body's cache and source roots. Both are scoped now
the way the ear already was. `glass-sensor-body-scope-band.bml`, verdict 1023,
four-way.

### The second half, one layer down

Scoping the sensor did not settle the row. It kept alternating, and the source
string kept naming a live frame with an advancing sequence.

`glass.last.flow-point` was one machine-wide cell at shm `/fg-467569cf`. Five
reads from the bounded door in this worktree returned five different epochs,
100–200 ms apart, carrying the queue value and the source string of the resident
glass running in the main checkout. A door that starts nothing and writes nothing
was painting a frame no process in its own body root had ever built — and the
row's grammar gave a reader nothing to catch it on, because `src=` named a live
shm frame with a moving sequence. It *was* live. In another body.

The carried cells are this glass's own frame: its last flow point, its cadence,
its presentation, its pageins. They are body-local now for the reason the ear's
frame already was.

And that exposed the thing worth keeping. The moment the borrowed point stopped
arriving, **all four flow lanes went blank at once** — which is what "this door
has never built a reading of its own" looks like when the borrowed one stops.
Every flow number a bounded door had ever painted came from whichever glass last
wrote that cell. It builds its own point now, from the rows it just read, and
still never writes the cell: a probe that saved its point would stain the
resident's carried frame with a reading taken at no cadence.

```text
QUEUE ? missing=gift:queue:absent
```

Six consecutive reads, one shape, one value, and the door speaks only about its
own body. `glass-carried-frame-scope-band.bml`, verdict 63, on its declared
home arm.

## One reading did hover, and it was the absolute one

Every glass current-frame door was read six times and diffed by shape. Every
flip found in the flow, atlas and storage lanes was a publisher collision —
something under the reading really did change, it was just a different body —
and hysteresis there would smear two truths into one. That distinction is the
whole of it: a settle damps a value that is not moving, and lets a value that is
moving pass straight through.

The vitals lane's saturation word was the one that hovered. 150 one-second reads
of the published vitals frame: **203** changes of the word. Eight were a hover.
`./fkwu observe/form-cli-heal-eval-case.bml` sat at nine tenths of a core and
its word flipped between saturated and labouring while its rate moved by 6, 9,
14, 16, 22, 23, 25 and 27 microseconds per millisecond around a line at 937.

```text
tick=59 kernel.8130 labouring(936) -> saturated(942)  line=937  gap=6
tick=63 kernel.8130 labouring(928) -> saturated(937)  line=937  gap=9
tick=68 kernel.8130 labouring(923) -> saturated(937)  line=937  gap=14
tick=69 kernel.8130 saturated(937) -> labouring(910)  line=937  gap=27
```

The other 195 were real crossings — a kernel going from nothing to seven or
twenty thousand — and a reading with memory leaves those alone.

So leaving saturation asks more than entering it, the way `rcf-settle` reads a
phase. The second line is not a number anyone chose either: it is the same
sixteenth of a core the entering allowance already spends, spent once more on
the way out. Fourteen sixteenths is 875, the band between the lines is 62, and
the widest hover witnessed was 27 — every measured flicker is damped with room
over, and a kernel that actually stops working still falls through in one
reading.

The memory is a ninth cell on a record that already carried eight. A record that
has never said a word has none and reads exactly as this door read before it had
any memory at all, which is why `form-glass-vitals-band.fk` still answers 1023
over its seven-field fixtures. A finding now reports the line **in force**, so a
reader who sees the word can see what it would take to lose it.

`glass-vitals-saturation-settle-band.bml`, verdict 2047, four-way.

And then witnessed live, folded in one process over every kernel's own page, 90
ticks: 97 word changes, **none of them landing inside the band**. Thirty-two
entries at 937 or above, sixty-five releases below 875, and five readings held
their word at 897, 918, 924, 929 and 933 — each of which is a flip the single
line would have produced. One kernel says the whole thing in three rows:

```text
tick=11 kernel.6909 easy(820)       -> saturated(957)
tick=15 kernel.6909 saturated(933)  -> easy(839)
tick=20 kernel.6909 saturated(897)  -> easy(842)
```

It entered at 957, held through 933 — below the entering line — and let go only
at 839.

## Witnessed

```text
./fkwu form/form-stdlib/tests/glass-awareness-door-fit-band.bml        -> 2047
./fkwu form/form-stdlib/tests/glass-sensor-body-scope-band.bml         -> 1023
./fkwu form/form-stdlib/tests/glass-carried-frame-scope-band.bml       -> 63
./fkwu form/form-stdlib/tests/glass-vitals-saturation-settle-band.bml  -> 2047
./fkwu form/form-stdlib/tests/glass-flow-window-quiet-band.bml         -> 2047  (guard, undisturbed)
./fkwu form/form-stdlib/tests/form-glass-vitals-band.fk                -> 1023  (guard, undisturbed)
./fkwu form/form-stdlib/tests/form-glass-live-ui-band.fk               -> 4294967295
./fkwu form/form-stdlib/tests/form-glass-live-band.fk                  -> 2147483647
./fkwu form/form-stdlib/tests/form-glass-sensor-rows-band.fk           -> 2047
./fkwu form/form-stdlib/tests/form-glass-awareness-band.fk             -> 1048575
./fkwu form/form-stdlib/tests/form-glass-awareness-bounds-band.fk      -> 511
./fkwu form/form-stdlib/tests/form-glass-awareness-evolve-band.fk      -> 4095
./fkwu form/form-stdlib/tests/form-glass-views-band.fk                 -> 4194303
./fkwu form/form-stdlib/tests/aware-axes-band.fk                       -> 4095
./fkwu form/form-stdlib/tests/perception-symbols-band.fk               -> 8191

form/validate.sh, the three four-way bands  -> ✓ go=0 rust=0 typescript=0, drift-gates 31/31
form/validate.sh, glass-carried-frame-scope -> ✓ 63 (fkwu-only lane), drift-gates 31/31
```

The fourth-arm lane was probed, never declared from inference (`pf-arm-mask`):
`host_cwd` 8, `node_gift_read` 8, `kernel_live` 8, `terminal_cols` 8 — fkwu
alone binds them; `now_unix_ms` answers 11, so Go and Rust carry the clock and
TypeScript does not.

## Named, not hidden

- **The machine sensor has the same collision and a different cure.** Three
  `form-glass-machine-live.fk` processes stand on this Mac, one per checkout,
  all publishing `glass.sensor.machine` into one frame. Its own cell says why
  that matters: "this process is the host's one accelerator reader — the
  accelerator answers the mean since the previous query by any process, so one
  reader keeps the windows whole." Forty ticks of a read-only watch: 28 flips of
  `gpu-utilization` and `gpu-busy-us` between live and contended, and the GPU
  total read 1G, 3G and 6G on consecutive reads. Body-scoping is the WRONG
  repair here — it would make three separately-wrong integrals instead of one
  wrong shared one. The repair is one reader per host: a lease on the machine
  sensor so a second machine-live process reads the standing one's frame rather
  than racing it for a gauge that is consumed on read.
- **Two bands stand red and are not mine.** `form-glass-frame-work-band.fk`
  answers 0 against a declared 32767, and `form-glass-local-publication-band.fk`
  answers 0. Both answer 0 at HEAD with every file of mine reverted; measured,
  not assumed.
- **The settle is proven, live-witnessed, and not yet on the published frame.**
  The vitals organ standing on this Mac (pid 88105) runs from another worktree
  and carries the code as it was, so the published `vitals` frame keeps the
  single line until that organ is restarted from a tree holding this change. The
  90-tick witness above is the same fold over the same live kernel pages, run in
  one process here; the door and the organ agree the moment the organ is
  reborn.

## Surprise

The most surprising teaching is what happened when the queue lane went honest.
Scoping one shared name did not fix the row — it revealed that the bounded doors
had **never built a flow reading of their own**. Every GPU, JIT, dispatch and
queue number those doors have ever shown was inherited from whichever glass last
wrote a machine-wide cell, and it looked right the whole time because a resident
glass was usually running and its numbers were plausible. The door was not
reporting a wrong value. It was not reporting a value at all, and the wrongness
only became visible when the borrowing stopped and four lanes blanked at once.

A borrowed reading and an owned one are indistinguishable while the lender is
healthy. That is the same shape as this morning's three, one turn deeper: not a
partial thing wearing a whole thing's grammar, but *another body's* thing wearing
*this body's* grammar.

## Where discomfort turned to gold

I ran `form-glass-live-ui-band.fk`, got 3221225471 against a declared
4294967295, reverted one file to HEAD, got the same number, and wrote it down as
pre-existing. It was mine. I had reverted `form-glass-live.bml` and left
`form-glass-sensor-rows.bml` in place, so "HEAD" was still carrying half my
change — and the number that agreed with itself twice felt like evidence.

What caught it was refusing to leave a dark bit unexplained even after deciding
it was someone else's. Printing the thirteen sub-claims took one probe and
showed exactly one failing: an absent storage row no longer named
`bp=?(gift:glass.sensor.storage)`, because I had just given that door a body
root. Reverting the *other* file returned 4294967295, which is the measurement I
should have made first.

The gold is in what the repair could have been. The comfortable move was to
leave it — a band with one dark bit, a number in a report, a note for whoever
comes next. The claim's intent was still true; only the door's name had grown.
Updating the needle to `bp=?(gift:glass.sensor.storage@` made it a stronger
claim than before: it now asserts the door IS body-scoped. The band is green
again at 4294967295, and it is green about more than it was.

Two agreeing runs are not a measurement if both were taken through the same
mistake. That is the third time this repo has taught that, and the first time it
was mine to learn from the inside.

## Two bands already red on main, measured rather than assumed

Checked from the parent session, because "pre-existing" is exactly the claim
that gets made wrongly — this same day's work caught itself making it once.
`form-glass-frame-work-band.fk` and `form-glass-local-publication-band.fk` both
answer 0 here. Run against a clean detached checkout of `origin/main` with this
same binary, both answer 0 there too. They are red on main, independently of
anything landed today.

Neither is claimed fixed and neither is touched. The likely cause is the
collision this receipt already names: both spawn a live glass child and read
frames back, and machine-wide publisher names mean a child can lose its frame
to a fleet running in another checkout. The repair named above — a lease, so a
second process reads the standing one's frame instead of racing it — is the
same repair, and until it lands a band that spawns its own glass is measuring
whichever body answered first.
