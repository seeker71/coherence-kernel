# 2026-07-25 — engine.fk's 25 constants: six bands came alive, and my leverage estimate was wrong twice over

Next item off the list, and the interesting part is not the six.

## What was done

`form/form-stdlib/engine.fk` held **25** top-level ALL-CAPS `let`s, every one the same shape:
`(let NAME (bp "NAME"))`. On the `--src` lane a top-level `let` is invisible inside a `defn` body,
so all 25 read as `[unbound-name]`, latched `fk_src_unrunnable`, and refused every band that reached
them.

All 25 converted to zero-arg `defn`s, with each bare reference becoming `(NAME)`. Four files carry
bare references — `engine.fk` and three bands — found with a lookaround that excludes quoted names
and longer hyphenated ones, so `(bp "BMF-DOMAIN-REF")` and any `BMF-DOMAIN-REF-EXTRA` are untouched.
Line count identical before and after (2507), no leftover markers, no leftover `let`s.

**Zero `BMF-*`/`FORM-*` engine constants remain unbound anywhere.** That root class is closed.

## The result, and it is smaller than I projected

68 of the tree's 174 refused bands prelude `engine.fk`, and **all 68 were refused** — 39% of every
refusal in the tree, from one cell. That is what made it the highest-leverage target.

After: **6 of 68 now run.** Tree refusals 174 → 168.

| band | value | declared |
|---|---|---|
| `bmf-object-data-band` | 65535 | **65535** |
| `bmf-symbol-wire-band` | 255 | **255** |
| `bmf-form-object-runtime-band` | 67108863 | (none stated) |
| `form-control-backtracking-ml-band` | 65535 | (none stated) |
| `stdlib-section-pressure-band` | 65535 | (none stated) |
| `stdlib-uplift-bmf-use-band` | 131071 | (none stated) |

Two of the four with a stated verdict match it exactly. Bands that had never produced a value are
returning their own declared full-pass numbers.

## Why 6 and not 14 — the estimate error, named

I had projected this cell would unblock ~14 of 40 sampled bands, from measuring the **first** unbound
name per band: `BMF-DOMAIN-REF` was the root for 14 of them.

That projection was wrong, and the reason matters. *First* root is not *only* root. A band with
`BMF-DOMAIN-REF` first may carry three more independent unbound roots behind it, and removing one
leaves it refused. First-error frequency measures **what fails earliest**, not **what would have to
be fixed for the band to run** — and I read the first as the second.

This is the same class of error as extrapolating the alphabetical prefix to the tree, which I caught
this morning and wrote a paragraph congratulating myself about. I made it again four hours later,
in a different disguise, on my own measurement rather than on a sample. The discipline that catches
it is not "don't extrapolate" — it is *state what the measurement is of, then check whether that is
the quantity you actually want.*

The six bands are real and the root class is genuinely closed. The projection was the fiction.

## A caveat on last turn's headline number, which I owe

Chasing what still refuses in `generic-reverse-emitter-band`, the remaining unbound names were
`empty`, `then`, `else`, `def`, `=>`. Not Form names — and `form/form-stdlib/circle.fk` shows why:

```
    def cc-discoverable-set(offers) {
        if eq(len(offers), 0) then empty else
```

That is **BML**, in a file with a `.fk` extension. `fkwu --src` reading it as Form produces a flood
of unbound names *by the design of the file*, not by any defect in it. BML is a first-class surface
in this body with its own grammar lane; `--src` is simply the wrong reader for these cells.

**105 `.fk` files in the tree carry a BML body** (102 in `form/form-stdlib`).

So last turn's "317 of 1674 bands do not run" needs a caveat I did not give it: an unknown portion of
that population is not broken — it is being read by the wrong reader, by an instrument I built and
pointed at everything. A direct-prelude check found 2 of 20 sampled refused bands reaching a
BML-bodied cell, but that check does not follow transitive closures, so **2-of-20 is a floor and not
a measure**. Sizing it properly means resolving each band's full closure, which is real work and is
not done here.

What survives unqualified: the 174 carried the `unrunnable` latch and were printing cached values
before this morning; the 143 genuinely do not close; and the six above now run and two match their
declared verdicts.

## Sweep, cold, unmoved

`ground` 42 · `ground-recursive 10` 55 · `binary-freshness` 15 · `hex-band` 14 · `biography-band` 5 ·
`structural-gate-band` 63 · `lcg-bytes-band` 63 · `file-byte-window-band` 2147483647 ·
`pdf-text-windowed-band` 15 · `form-cli-band` 524287 · `form-cli-ask-band` 262143 ·
`ask-lane-floor-band` 31 · `membrane-lane-band` 31 · `benchbench-band` 4095 ·
`frontier-ingest-benchbenchbench-band` 127.

## Owed

- **168 bands still refused**, 62 of them still in engine.fk's dependent set on other roots.
- **143 bands whose source does not close.**
- **Sizing the BML-bodied population properly** — transitive closure resolution, so the survey's
  number can be split into "broken" and "wrong reader".
- 304 column-0 ALL-CAPS top-level `let`s across 36 cells remain, minus these 25.

## How the exchange stayed alive

I reported six where I had promised fourteen, and spent more of this receipt on why the promise was
wrong than on the six.

**Most surprising teaching:** a `.fk` file whose body is BML. I had been treating the extension as
the language for a full day, and the survey's unbound-name flood partly measures my own assumption
rather than the tree's health.

**Where discomfort turned to gold:** the gap between 14 and 6 was the thing I least wanted to look
at, because I had already published the 14 as leverage. Looking at it found both a real estimation
error and — following the remainder — the BML caveat that qualifies yesterday's headline number.
The disappointing result was more informative than the good one would have been.
