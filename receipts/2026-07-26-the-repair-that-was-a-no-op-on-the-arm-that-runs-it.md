# 2026-07-26 — the repair that changes nothing on the arm that runs everything

Last turn found it and left it: `str-byte-at` is built from `(ord (substring s i (add i 1)))`, which
reads a byte only on fkwu. On go, rust and ts, slicing inside a multi-byte character gives the **empty
string**, so `core.fk`'s `ord` answers **−1** — and the cell exists for pg-wire framing, where bytes
above 127 live. I named it and did not swing it, because the verification did not fit the turn.

This is that turn.

## The blast radius, checked rather than assumed

187 cells name `str-byte-at.fk`. That is the number that made me stop last time, and it is the wrong
number. Counting **actual calls** to `str-byte-at` / `str-be16` / `str-be32`, with comments and string
literals stripped:

**20 cells.** And they are exactly who you would fear: `form-cli-gguf-cell`, `pg-wire`, `base64`,
`hex`, `url-encode`, `pdf-text-windowed`, `whisper-tiny-native-conv1`, `file-byte-window`,
`source-artifact-identity`, `source-artifact-seal`, `light-codes-bootstrap`, and the human-corpus
ingest cells. Binary readers, every one.

The first count also nearly cost the turn a different way. I started a before-capture across all 76
dependent *bands*; it was still running after three minutes, and two things were wrong with it. Some of
those bands print **timestamps**, so a naive before/after would have differed on every run regardless of
my change. And it was still running when I applied the repair — so its rows straddle the edit and are
worthless as a baseline. **It was abandoned, not mined.** The comparison this receipt rests on is the
focused one below: ten byte-lane bands, captured entirely before and entirely after, in sequence.

## The repair

```
-    (defn str-byte-at (s i) (ord (substring s i (add i 1))))
+    (defn str-byte-at (s i) (str_byte_at s i))
```

Snake against kebab — the native's name and the recipe's name are different, so nothing shadows
anything and no arm loses a binding. Verified after, on the tree copy:

| | fkwu | go | rust | ts |
|---|---|---|---|---|
| `(str-byte-at "λ" 0)` | 206 | 206 | 206 | 206 |
| `(str-be16 "λ" 0)` | 52923 | 52923 | 52923 | 52923 |
| `(str-byte-at "€" 2)` | 172 | 172 | 172 | 172 |

Before, those three read 206 / −1 / −1 / −1 and 52923 / −50 / −50 / 699.

## Before and after, on the byte lane

| band | fkwu before | fkwu after |
|---|---|---|
| base64 | 12 | 12 |
| file-byte-window | 2147483647 | 2147483647 |
| hex | 14 | 14 |
| light-codes-bootstrap | 255 | 255 |
| pdf-text-windowed | 15 | 15 |
| situated-witness-continuity | 2147483647 | 2147483647 |
| source-artifact-identity | 1073741823 | 1073741823 |
| url-encode | 13 | 13 |
| **str-byte-at** | **15** | **511** |

Only one moved, and only because its band grew new claims in the same commit.

**The repair is a no-op on fkwu by construction.** fkwu's own `substring` is byte-indexed, so the old
expression and the new one compute the same thing there. That is not a footnote — **it is why the fault
was invisible.** The arm this body runs everything on could not have told anyone, and the same shape
appeared two turns ago with the bounds clamp: a change with no observable effect on fkwu and a decisive
one on the other three.

Four-way after: `str-byte-at` **511 ×4**, `hex` 14 ×4, `base64` 12 ×4, `file-byte-window` 2147483647 ×4.

> **CORRECTED 2026-07-26, later the same day.** That `511 ×4` is scoped to a set this receipt never
> named. This body has **seven** evaluators — fkwu, the three heavy kernels, and the three minimal
> walkers — and "ts" is two of them. On the minimal ts walker `str-byte-at-band` read **15**, not 511,
> because that walker took its source in as UTF-8 and measured strings in UTF-16 code units, so
> `(str_len "λ")` was 1 there and `(str_byte_at "λ" 0)` was 187 — the low byte of the codepoint, not
> the first byte of the encoding. Six evaluators were at 511 and I wrote "×4" over a set that left out
> the one that disagreed. Its intake is latin1 now and all seven read 511; the walker's own source had
> named the gap and said "every test this walker actually needs to pass today is plain ASCII", which
> stopped being true the moment the non-ASCII claims in this receipt landed. See
> receipts/2026-07-26-the-tongue-shelf-crosses.md.

`url-encode-band` reads 13 on fkwu and 16 on the walkers. I checked whether I caused it by swapping the
original cell back into the closure: **go answers 16 either way.** Pre-existing, not this change.

## The sentence that hid it

`str-byte-at-band.fk` opened with its own explanation:

> *ASCII literals so each byte is its codepoint and the verdict is deterministic across the four walkers.*

A deliberate, sensible-sounding choice — and it made the verdict deterministic **by confining the band
to the one input range where the implementation could not fail.** The cell is for binary framing;
binary framing is above 127; the band tested below it. It read 15 on every arm for as long as it has
existed.

Five claims added with the repair: both bytes of a two-byte glyph, `str-be16` over it, the third byte
of a three-byte glyph, and `str_len` counting bytes rather than characters. **Verdict 15 → 511, on all
four arms.** Those are the claims that would have caught this the day it was written.

## Sweep

`ground` 42 · `str-byte-at-band` **511 ×4** · `hex` 14 ×4 · `base64` 12 ×4 · `file-byte-window`
2147483647 ×4 · `say` 255 · `primitive-edge-contracts` 1023 · `navier-stokes` 1023 ·
`navier-stokes-plate` 2047 · `benchbench` 4095 · `structural-gate` 63 · `json` 1023 ·
`primitive-registry` 45 · `keyed-map` 4095 · `cell-voice-tissue` 511 · `lcg-bytes` 63 ·
`proof/four-way-run-recipe42.fk` 0 (FOUR-WAY). C seed byte-identical to git.

## Owed

- **The three private copies.** `base64`, `hex` and `url-encode` were the cells that each rolled the
  same one-liner before it was lifted here; whether any still carries a private `(ord (substring …))`
  rather than calling the shared recipe is uncounted. Their bands are unchanged and four-way, so
  nothing is broken — but the same fault could be sitting in a copy.
- 751 `print` calls still silent on fkwu; `say` exists, migrating is an owner's call.
- The four questions already put to the owner; the flatten/emit lane; `native_blueprint` absent.

## How the exchange stayed alive

I left a repair unmade last turn because the verification did not fit, came back with the whole turn
for it, and the first thing that changed was the number that had made me stop: 187 became 20.

**Most surprising teaching:** the fix does nothing on fkwu. Nothing at all — same bytes, same verdicts,
by construction. A change whose entire value is invisible from the arm you develop on is a change you
would never make, never notice was needed, and never be able to check. That is now the second one this
week, and it is the strongest argument for the other three kernels I have found: not that they
disagree, but that some things are only *visible* from over there.

**Where discomfort turned to gold:** `url-encode` coming back 13 against 16 right after I changed a
shared cell. The honest move was to assume I had broken it and go prove it — swapping the original file
back into the closure took two minutes and said go answers 16 either way. Being willing to have caused
it is what made the check quick instead of defensive.
