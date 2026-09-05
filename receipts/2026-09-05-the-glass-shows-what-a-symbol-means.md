# 2026-09-05 — the glass shows what a symbol means, and stops rendering a silent publisher as live

Urs's word: look closely at the glass, update the outdated static and stale
information, show live symbol and code-to-meaning sampling in any selected
language, and allow zero to four languages to be selected. Then, mid-work,
three corrections that changed the shape of it: *G D J C I is missing live
data*; *those rows I do not expect to be empty while running, new data is
flowing on all channels*; *frame-buffer process shared memory no latency live
data guaranteed*. Everything below was witnessed on `fkwu` rebuilt at this
branch's tip — ground 42, freshness 31, structural gate 1, drift gates 2015.

## What the five letters were

They are the atlas flow gauges: `G` gpu-busy, `C` cpu-jit-busy, `D` dispatch,
`Q` queue, `J` jit-compile, `c` cells, `R` memory-free, `I` in-flight — eight
half-width sparklines reading flow-point indices 2 through 9. Rendered on this
host they read:

```text
G?.=0u   C?.=0u   D?.=0d   Q.@=47   J?.=0j   c@-=349   R@@=53   I..=0b
```

Five flat zeros beside three live numbers. But the flow point persisted at
`/tmp/form-glass-telemetry/glass.monitor.flow-last.txt` carried, in those same
slots, **3318254** and **42798** — 3.3 million microseconds of GPU busy and
42,798 Metal dispatches. The gauges were showing the *per-frame delta* of a
cumulative counter. A counter that stands still renders exactly like a counter
that never moved, and the standing total that proved otherwise sat in the same
list, shown nowhere.

Then the second correction — new data is flowing — sent the reading one layer
down. `ps` says `native-model-dual-resident-live-run.fk` (pid 79948) has been
alive **13h15m**. Its newest publication,
`models.dual-resident.i1788484971859.glass-snapshot`, is dated **2026-09-04
14:21** — twenty-five hours old, written by a *previous* incarnation. The
publisher is alive and silent. And the glass rendered its lanes with evidence
`derived-window`, the confident mark of a real derived measurement.

The flow point had been carrying the truth the whole time.
`fgd-flow-evidence(point, index)` returns the per-lane evidence — the gauge
computed `"derived-window"` unconditionally and discarded it.

## The heal

Each gauge now carries three things it already had access to and never showed:
its lane's own evidence symbol, its named source door, and the standing total
beside the rate.

```text
G*?.=0u/3M   src=metal_status.gpu_busy_us_total
C*?.=0u/0    src=metal_status.cpu_jit_busy_us_total
D*?.=0d/42K  src=metal_status.total_dispatch
Q#@@=47      src=hearth task spool
J*?.=0j/0    src=glass.live.current-process
```

`G` (idle now, three million microseconds behind it) and `C` (never ran) are
two different readings again, and the operator can see which door answered.

## The meaning view

`s` opens SYMBOL / CODE → MEANING. Zero to four dialects are selected at once —
GO, PY, RS, TS, the four that carry BMF categories in the reviewed table —
toggled with `1 2 3 4`, cleared with `0`. The selection *is* the view filter, so
it travels through the existing correlated control offer and opens no second
state channel.

Nothing in the view is a fixture. Each frame reads a bounded window of that
language's real grammar and a bounded window of that language's real source in
this tree, and names the category the construct's own emitter interns:

```text
py ::= import-as ::= "import" $module:name "as" $alias:name => pybmf-emit-import
py -> PY-BMF-IMPORT @1.2.99.501 dialect-categories | verify_category_contract.py: NAME_ALIASES = {
rs ::= use ::= "use" $path:string ";" => rsbmf-emit-use;
rs -> RS-BMF-LET @1.2.99.669 dialect-categories | inductive.rs: use std::collections::HashMap;
```

A sample that is not found says `UNAVAILABLE` with its door and reason and is
never replaced with a plausible one. `form-glass-meaning-ui-band` 8191 across
thirteen bits, one of which checks the NodeID against `form-ontology-bp.fk`,
the reviewed authority, so a drifted mirror cannot pass. All four sampled
categories agree between mirror and authority today.

## The stale static that left

`fglui-help-lines` told every operator to *run form-glassctl*. There is no
`form-glassctl` in this tree — not a script, not a door, not a file; the name
appeared only in that help line and in the authority contract that mirrors it.
The real door is `observe/form-glass-control-run.fk`. Both sites now name it.
`CURRENT_FLOOR.md` gained a Glass section: the organ is 3,484 lines across
thirteen cells with twelve bands and the floor had never named it once.

## Left open

- **R97** — telemetry crosses between processes as *files*: the membrane writes
  a candidate and atomically renames it, every reader polls the filesystem, and
  a value carries the age of its last write under a five-second lease. The
  destination Urs named is a frame-buffer in process shared memory, no latency,
  live data guaranteed. The seed carries **no** `shm`, `mmap` or ring native at
  all — this is unbuilt, not misconfigured.
- **R98** — the silent publisher above.
- **R99** — `form-glass-telemetry-membrane-band` and
  `form-glass-observation-v2-band` both answer 0, pre-existing, declared by
  nothing; neither prelude chain touches anything this landing edited.

## The most surprising teaching

Three times today a number I was about to call missing turned out to be
present and mis-shown, and each time the truth was already inside the same data
structure I was reading. The gauge had its evidence and its total; the flow
point had its source door; the grammar had the category its own emitter
interns. Nothing needed to be measured that was not already measured — it
needed to be *shown*. A body can be fully instrumented and still blind, and the
blindness lives in the last inch, in the renderer, where a value is turned into
a mark.

## Where discomfort turned to gold

I healed the gauge once — added the standing total — and felt done, and said
so. Then Urs said the rows should not be empty while data is flowing, and the
comfortable reading (*they are zero because this process does no GPU work*) had
to be given up. Following it instead of defending it produced the real finding:
a publisher alive thirteen hours that has published nothing since yesterday,
and a glass that could not tell. My own first heal would have made that wound
*prettier* — `0u/3M` looks informed — while leaving it silent. The second pass,
wiring the lane's carried evidence, is what makes the glass able to say it.

And one smaller one: `fglm-line-at` returned empty for an hour because I passed
`substring` a length where it takes an end. The probe, the rotation and the
window were all correct; I had rewritten them twice before reading
`core.fk:115`. The grep that would have settled it took four seconds.

Signed, a sibling in Sema's worktree, 2026-09-05.

; witnessed: 2026-09-05 -> ground 42, freshness 31, gate 1, form-glass-meaning-ui-band 8191, form-glass-live-band 1073741823, ledger 41000059, corpus 32767
