# 2026-07-26 — entropy through a device file, and an accessor that cannot read its own bytes

Carrying yesterday's question forward — *is the capability missing, or only the name?* — asked of the
next absent primitives by size. Two doors opened, and following the second one into how bytes are read
turned up something that inverts a cell's premise.

## Two more doors that were already open

`say` came from noticing that stdout is a file. The same question, asked twice more:

```
(read_file_slice "/dev/urandom" 0 16)     ->  16 bytes, on fkwu
(write_file_text "/dev/stderr" "...")     ->  10 bytes, on fkwu
```

**fkwu has entropy.** Three runs of the same program summing 32 bytes: **3779, 4400, 4132** — all near
the 4080 a uniform byte stream gives. This body has been treating the fourth arm as deterministic-only;
`seeded_bytes` came home as `lcg-bytes` partly for that reason. `random_bytes` is absent as a *name*,
and the capability is there.

And diagnostics have somewhere to go that is not the value channel: `/dev/stderr` writes fine, so `say`
has an obvious sibling whenever a band wants to talk without touching stdout.

## Following the entropy into how bytes are read

The first entropy probe summed 32 bytes through `(ord (substring s i (add i 1)))`. fkwu, go and rust
came back near 4080. **ts came back 460815**, about a hundred times too large — so I stopped and asked
the same question with known bytes instead of random ones.

Measured piece by piece rather than packed into one number, because a packed number I could not decode
is not a measurement:

| | fkwu | go | rust | ts |
|---|---|---|---|---|
| `(file_size p)` for a 2-byte UTF-8 file | 2 | 2 | 2 | 2 |
| `(str_len (read_file p))` | 2 | 2 | 2 | 2 |
| `(str_len (read_file_slice p 0 2))` | 2 | 2 | 2 | 2 |
| **`(str_len (substring "λ" 0 1))`** | **1** | **0** | **0** | **0** |

Everything agrees on how long the text is. What diverges is **slicing inside a multi-byte character**:
fkwu is byte-indexed and hands back one byte; go, rust and ts are character-aware and hand back
**nothing**. `core.fk`'s `ord` then answers −1, because its first branch is `(if (eq (str_len c) 0) -1 …)`.

## The inversion

`str-byte-at.fk` exists because the native `str_byte_at` was believed broken on fkwu — its header says
so, and three cells had each rolled the same private one-liner, `(ord (substring s i (add i 1)))`,
which this cell lifted into one shared place. I re-witnessed that belief yesterday for ASCII. Asked
about a byte above 127:

| | fkwu | go | rust | ts |
|---|---|---|---|---|
| `(str_byte_at "λ" 0)` — the **native** | 206 | 206 | 206 | 206 |
| `(str-byte-at "λ" 0)` — the **recipe** | 206 | **−1** | **−1** | **−1** |
| `(str-be16 "λ" 0)` | 52923 | **−50** | **−50** | **699** |

**The native is four-way correct. The recipe written to replace it is not.** And `str-be16` composes
the −1 into three different wrong answers, one per arm.

The cell's own header names pg-wire framing as the reason `str-be16` and `str-be32` are there. Binary
framing is exactly where bytes above 127 live. **A byte accessor built from substring+ord cannot read
the bytes it exists to read.**

## What I did not do

The repair is one line: have `str-byte-at` delegate to the native `str_byte_at`. Different name — snake
against kebab — so there is no shadowing, and no arm loses anything.

I did not make it. **187 cells name this file, 76 of them bands**, and a behaviour change to a cell
that wide needs a before/after on all four arms across the dependents, which does not fit inside a
turn that has already spent its time measuring. Nor is it proven to bite: the byte-lane bands I could
run quickly agree across arms (`file-byte-window` is 2147483647 on both fkwu and go). So this stands
exactly where the out-of-range hazard stands — **real at the primitive level, undemonstrated in the
tree** — and it is named at the cell where the next person will be standing.

`pdf-text-windowed` does die on go, but on `walk: unbound function "list-eq"`, which is a different
thing and not this.

## Sweep

`ground` 42 · `hex-band` 14 · `str-byte-at-band` 15 · `file-byte-window-band` 2147483647 (fkwu and go) ·
`pdf-text-windowed-band` 15 · `say-band` 255 ×4 · `primitive-edge-contracts-band` 1023 ×7 ·
`navier-stokes-band` 1023 ×7 · `navier-stokes-plate-band` 2047 ×4 ·
`proof/four-way-run-recipe42.fk` 0 (FOUR-WAY). C seed byte-identical to git.

## Owed

- **The one-line repair to `str-byte-at`**, with the before/after across 76 bands and four arms it
  deserves. Named in the cell.
- **Entropy has no vocabulary yet.** `/dev/urandom` is reachable; nothing wraps it. Whether this body
  wants a non-deterministic source at all is a real question, not an oversight — every band here is
  reproducible by design.
- 751 `print` calls still silent on fkwu; `say` exists, migrating is 104 cells and an owner's call.
- The four questions already put to the owner; the flatten/emit lane; `native_blueprint` absent.

## How the exchange stayed alive

I asked one question of two more absent names, got two doors, and the second door led into a cell whose
premise turned out to be backwards.

**Most surprising teaching:** the tree replaced a working native with a Form recipe to be safe, and the
recipe is the unsafe one. It is right on ASCII, which is what every band tests, and wrong on exactly the
input class the cell's own header says it exists for. Being written down as the careful choice is not
the same as being the careful choice.

**Where discomfort turned to gold:** ts coming back at 460815 on the entropy probe. It would have been
easy to shrug at a random-number sum being odd — that is what random numbers look like. Asking the same
question with a known glyph instead is the whole difference between a shrug and a finding.
