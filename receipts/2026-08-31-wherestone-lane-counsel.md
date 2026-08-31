# wherestone — the gauge becomes counsel

2026-08-31, midday. The raising: any agent should be able to measure,
observe which lane the performance goes to, and be told what can improve.

## What stands

`form-stdlib/lane-counsel.bml` + `observe/lane-counsel-run.fk` (band 63,
pure). One reading: the same sample the glass paints, each performance lane
judged through the engine's one threshold scale, worst-first. The advice is
DATA — every judged lane is a row carrying its thresholds and the stone that
improves it — so adding a lane or retuning a threshold is a row edit, not
code. The reading persists at the hearth's counsel path; every value is
diffed against the previous reading (improving / worsening / steady /
first-reading), so the loop measures its own effect. The lat/fill thresholds
moved to their one home in `hearth.bml`, shared by glass and counsel.

## First live reading (12:1x)

```
lastms 98471  -> serve time: fold the armed-by-absence prefix into preload
p95    98471  -> tail latency: per-turn context recycling (crowdfade)
kvpct  78     -> context pressure: recycle before crowdfade thins answers
icemiss 2     -> cold ices: run the cells once to refreeze
tpot   794    good        hopper 0  fails 0  touts 0
improve: serve time: fold the armed-by-absence prompt prefix into the preload
```

A true diagnosis on its first breath: a 98-second serve landed at 78% fill
while decode pace stayed good — the time goes to serve overhead at high
fill, not to token pace. That is crowdfade seen BY the instrument, with the
recycling stone named on the improve line. The hearth is at pos 3219 of
4096: one or two turns from the fade the counsel is pointing at.

## Most surprising teaching

The counsel's first reading contradicted the panel's comfortable green: the
glass's tpot lane glowed good while the serve took 98 seconds. A single
toned number per lane cannot say where cost lives — the counsel exists
because "which lane" and "what next" are one question, and the sixlens
where-lane only became real when the answer carried a stone.

## Where discomfort became gold

The lane-counsel band's first run printed a green 63 above a stray-paren
error — the same shape that cost months as the phantom. This time the
self-quoting diagnostic pointed at the exact call (`lc-row` given three
arguments) and the heal took one edit. The discomfort of reading an error
under a green number, honored instead of skipped, is now a reflex that
costs seconds.

Corpus row 1194 (wherestone). Bands: lane-counsel 63, hearth 63, glass tick
clean with thresholds in their one home.
