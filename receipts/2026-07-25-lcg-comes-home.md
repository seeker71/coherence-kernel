# 2026-07-25 — a native that was never a host boundary, and the 2^53 wall it hit on the way home

Third item off the list I had been carrying: *"`seeded_bytes` is a deterministic byte generator — a
recipe, not a host boundary, and it could come home to Form."* I had written that twice and left it.

It came home. And on the way it found a real four-way divergence.

## It was never a host boundary

`primitive-registry.fk` names the whole contract: **"deterministic glibc-LCG byte stream: same
(seed,count) everywhere."** A linear congruential generator is multiply, add, remainder. It touches
no device, no clock, no entropy pool. There was never anything for a host to provide.

Calling it an entropy door is what disguised it — it sits beside `random_bytes`, which *is* a host
boundary and stays native. Two doors in one room, one of them arithmetic wearing the other's name.

Because fkwu carries no `seeded_bytes`, `host-kernel-carrier.fk`'s entropy door could not run on the
kernel the body actually runs on.

## The witnesses were already written

I did not get to choose what "correct" means. Two expectations predate this cell, both authored
against the native:

- `primitive-registry.fk`'s probe row: `(nth (seeded_bytes 1 2) 0)` → **166**
- `tests/host-kernel-metal-band.fk`'s bit-256: seed 7 opens **52 93 210 163**

The recurrence that produces them, glibc's: `x <- (x*1103515245 + 12345) mod 2^31`, one byte per step
as `x mod 256`, first byte from x1 rather than the seed. Derived and checked by hand before writing
any code — seed 1 gives x1 = 1103527590, and 1103527590 mod 256 = 166; seed 7 gives x1 = 1282168116,
mod 256 = 52. A recipe that meets both is the same function, not a plausible one.

`form/form-stdlib/lcg-bytes.fk`, with `tests/lcg-bytes-band.fk` → **63**.

The band pins the two inherited witnesses byte for byte, and adds one they did not: calling
`(lcg-bytes 7 4)` twice and demanding the same bytes. "Same everywhere" begins with same-here-twice,
and without that bit the band would pass on a generator that drifted per call.

## The divergence — and why three agreeing arms were not the measurement

Written the obvious way, `(mod (add (mul x (lcg-mult)) (lcg-inc)) (lcg-mod))`:

| kernel | verdict |
|---|---|
| fkwu | **63** |
| Go | **63** |
| Rust | **63** |
| TS | **35** |

35 = 63 − 28, so bits 4, 8 and 16 fell: bytes 2, 3 and 4 of the seed-7 stream diverged while byte 1
agreed. Not a transcription slip, and not random. `x` reaches 2^31 and the multiplier is ~2^30, so the
product reaches **2^61** — and the TS walker's numbers are IEEE doubles, exact only to **2^53**. The
first step stayed small enough to agree; every step after it silently lost low bits.

Three of four kernels agreeing on the wrong shape of arithmetic, with the fourth quietly right to
object. This branch has spent the day on exactly that sentence — `proof/README.md`'s reason the
four-way is a diagnosis and not a tally — and here it arrived as a live case in a cell I had just
written.

## The repair: split the multiply, keep the function

`x` splits at 16 bits and each half is reduced before recombining:

```
x = hi*2^16 + lo,        hi < 2^15,  lo < 2^16
(hi*m*2^16) mod 2^31  =  ((hi*m) mod 2^15) * 2^16      -- hi*m < 2^45
(lo*m)                                                  -- lo*m < 2^46
```

Every intermediate now stays under 2^46, comfortably inside a double's exact range. This is ordinary
modular arithmetic, not an approximation — the result is bit-identical to the wide form on the 64-bit
arms, and the identity is why that is guaranteed rather than hoped.

**fkwu 63 · Go 63 · Rust 63 · TS 63 — FOUR-WAY.**

The cell keeps the derivation in its header, including the 35, because the next person to write
integer arithmetic that crosses four kernels needs the wall's height (2^53) more than they need my
answer.

## Wired, and a duplicate released

`host-kernel-carrier.fk`'s entropy door now calls `lcg-bytes`, so it runs on fkwu for the first time.
The real entropy door beside it — `random_bytes` — is untouched and stays native, because that one is
a host boundary.

Two byte-identical copies of that carrier existed: `form/form-stdlib/host-kernel-carrier.fk` and
`substrate/host-kernel-carrier.fk`. `diff` reports them identical, and **nothing in the tree preludes
the substrate copy**. Proven byte-identical and unreferenced is precisely the one-home rule's release
condition (`MANIFEST.md`, the same way the former top-level `http/` room and a drifted
`tool-channel.fk` went), so the dead copy is released. Git history holds it.

Before/after on the three carrier-dependent bands, run on cleared caches against the pre-change tree:
`fnri-receipt-band` 1299 → 1299, `fnri-gpu-standin-band` 15 → 15, `fnri-cli-band` 331 → 331. The value
does not move, which is what "same function" has to mean.

## Sweep, cold

`ground` 42 · `ground-recursive 10` 55 · `binary-freshness` 15 · **`lcg-bytes-band` 63 (four-way)** ·
`hex-band` 14 · `biography-band` 5 · `form-cli-band` 524287 · `form-cli-ask-band` 262143 ·
`form-cli-membrane-band` 1023 · `membrane-lane-band` 31 · `ask-lane-floor-band` 31 ·
`benchbench-band` 4095 · `frontier-ingest-benchbenchbench-band` 127 · `pdf-text-windowed-band` 15 ·
`homecoming-distillation-corpus-band` 32767.

## What I tried and could not finish

A tree-wide survey of all 1239 `-band.fk` files, to replace the bounded 25-of-120 count with a real
number. Started it, watched it manage 2 rows in the time the first 120 had taken, and **stopped it**.
A survey that cannot finish is not evidence, and letting it run in the background while reporting its
partial output as a tree-wide figure would have been the same laundering this branch keeps finding.
The honest number remains 25 of 120, alphabetical prefix, counted not estimated.

## Owed, observed, still open

- **22 of the 25 surveyed bands remain refused**, and 1119 in that directory are unsurveyed. Bounding
  a real survey — parallel, shorter per-band timeout — is itself a piece of work.
- **`native_blueprint` and `walk_recipe_here`** are still absent from fkwu. Unlike `seeded_bytes`,
  neither is obviously pure: `native_blueprint` resolves a native's name to a NodeID (the `bp` family
  this branch already opened), `walk_recipe_here` walks a recipe in place. Each needs its contract
  read before anyone claims it is a recipe.
- **304 column-0 ALL-CAPS top-level `let`s across 36 cells**, plus the nested form no grep counted.

## How the exchange stayed alive

I took the third thing off my own list instead of re-arming a timer over it.

**Most surprising teaching:** the divergence was the gift. If TS had agreed, I would have shipped a cell
that computes the right answer on three kernels and drifts on the fourth for anyone who ever raises the
seed — and the band would have said 63 four times over. The arm that disagreed is the only reason the
cell is now correct by construction instead of by luck.

**Where discomfort turned to gold:** killing the background survey. It was running, it was producing
rows, and leaving it going would have let me report a bigger number later. Stopping something that is
visibly working, because what it produces would not be true, is a harder call than fixing something
broken.
