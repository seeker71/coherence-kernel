# 2026-07-26 — eight copies of the same blind spot, and a cipher that could not carry an accent

Last turn repaired `str-byte-at` and left one thing owed in plain words: *the three private copies —
`base64`, `hex`, `url-encode` each rolled the same one-liner before it was lifted; whether any still
carries one is uncounted.* Counted. The three named cells are clean. **Eight others are not.**

## The count

Searching for `(ord (substring …))` with comments and string literals stripped:

| cell | |
|---|---|
| `form/form-stdlib/uuid.fk` | `uuid-byte-at` |
| `form/form-stdlib/room-cipher.fk` | inline in `rc-sb` |
| `form/form-stdlib/bmf-core.fk` | `bmf-str-byte-at` |
| `grammars/bmf-core.fk` | the same, second copy |
| `model/pl-grammar-induction.fk` | `plgi-byte` |
| three `form-samples/cross-modal/` cells | url-encode, external-uri-verify, cell-query |

**Eight.** `base64.fk`, `hex.fk` and `url-encode.fk` — the three I was worried about — already call the
shared recipe, so the worry was pointed at the wrong cells and the count found the right ones.

Each carries the identical fault, measured before the repair:

```
(uuid-byte-at    "λ" 0)   fkwu 206 · go -1 · rust -1 · ts -1
(bmf-str-byte-at "λ" 0)   fkwu 206 · go -1 · rust -1 · ts -1
```

## None of their bands could see it

`uuid-band` 11, `room-cipher-band` 15, `bmf-core-band` 600 — and **all three read the same on every
arm**, because every one of them uses ASCII inputs. Exactly as untested as the shared recipe was, for
exactly the same reason.

`uuid-band` does crash on go and rust, and I checked before attributing it: the message is
`bp: unreviewed bootstrap name: UUID`. The blueprint registry, not this.

## The cipher is the one that mattered

`room-cipher.fk` is the room dead-drop's authenticated cipher — encrypt-then-MAC over an HMAC-SHA256
keystream. Its plaintext goes through `rc-str-bytes`, which went through the broken one-liner.

Its band's plaintext is *"Irina morning brain fog; three hours of practice and love"* — ASCII. So the
band was 15 on four arms while **the cipher could not carry a name with an accent in it.**

Two claims added. Bit 16 reads the two bytes of a multi-byte glyph. Bit 32 round-trips
`"Irína — 練習と愛 🌊"` through encrypt and decrypt. **Verdict 15 → 63 on all four arms.**

Perturbation, with the old accessor swapped back into the closure:

```
repaired   fkwu 63 · go 63 · rust 63 · ts 63
old        fkwu 63 · go 47 · rust 47 · ts 15
```

The new bits catch it and **locate it differently per arm**: go and rust lose bit 16, ts loses 16 and 32
— the round-trip fails there too. And fkwu reads 63 either way, which is the same shape as every other
finding this week: the arm this body develops on cannot see the difference.

## The repair

Eight one-token replacements, `(ord (substring s i (add i 1)))` → `(str_byte_at s i)`, each behind a
private name so nothing shadows anything. After:

```
(uuid-byte-at    "λ" 0)   206 · 206 · 206 · 206
(bmf-str-byte-at "λ" 0)   206 · 206 · 206 · 206
(rc-str-bytes    "λ")[0]  206 · 206 · 206 · 206
```

Before and after on fkwu across every affected band: `uuid` 11, `room-cipher` 15→63 (its two new
claims), `bmf-core` 600, `url-encode` 13, `hex` 14, `base64` 12, `str-byte-at` 511. Only the band that
gained claims moved.

## A mis-attribution, caught

My first reading of `rc-str-bytes` said it **crashed** on go, rust and ts, and I nearly wrote that the
cipher was worse than the others. It was my probe: I passed `core.fk` plus the cell, and `room-cipher`
preludes `sha256` and `hmac-sha256`, so the crash was `walk: unbound function "append-list"` — a
missing prelude in my closure, not a fault in the cell. With the real closure it read 206 like the rest.

The closure resolver exists precisely so this does not happen, and I did not use it for a one-line
probe. That is the cost of a shortcut on the step that decides what a result means.

## Sweep

`ground` 42 · `room-cipher-band` **63 ×4** · `str-byte-at-band` 511 · `uuid` 11 · `bmf-core` 600 ·
`hex` 14 · `base64` 12 · `url-encode` 13 · `say` 255 · `primitive-edge-contracts` 1023 ·
`navier-stokes` 1023 · `navier-stokes-plate` 2047 · `benchbench` 4095 · `structural-gate` 63 ·
`json` 1023 · `primitive-registry` 45 · `keyed-map` 4095 · `proof/four-way-run-recipe42.fk` 0
(FOUR-WAY). C seed byte-identical to git.

## Owed

- **`uuid-band` crashes on go and rust** — `bp: unreviewed bootstrap name: UUID`. The blueprint
  registry question, already on the owner's list, now with a band behind it.
- **`url-encode` 13 on fkwu against 16 on the walkers** — still unexplained, and now confirmed not to be
  the byte fault, since url-encode calls the shared recipe and its verdict did not move.
- The seven other repaired cells have no non-ASCII claim yet; only the cipher got one. `uuid` and
  `bmf-core` would each want one, and `bmf-core` scans source, where a UTF-8 identifier is plausible.

> **CORRECTED 2026-07-26, later the same day, on two counts.**
>
> **The scope of `63 ×4`.** This body has seven evaluators, and the room-cipher band was measured on
> four of them: fkwu and the three heavy kernels. The three minimal walkers cannot run it at all —
> `bxor` is unbound there. True of a set, and the set was not named.
>
> **The census counted one spelling.** Searching `(ord (substring …))` found eight copies and walked
> past `(ord (char_at …))`, which is the same fault written differently. `cur-peek` in
> `form/form-stdlib/bmf-core.fk` and `grammars/bmf-core.fk` carried it, and it was holding the whole
> tongue shelf — fourteen tongues, six scripts — off three of four arms, where those bands did not
> disagree but crashed. Counted after: 60 cells, 193 sites of that spelling, which is a count of source
> reads and not of faults. Also: `bmf-core-band`'s new claim was NOT failing because of the accessor
> repaired here. The perturbation cleared it. See receipts/2026-07-26-the-tongue-shelf-crosses.md.
- 751 `print` calls silent on fkwu; the four questions with the owner; the flatten/emit lane.

## How the exchange stayed alive

I owed a count of three cells, found eight different ones, and the one that mattered was a cipher whose
band proved it worked on a sentence with no accents in it.

**Most surprising teaching:** the same blind spot was copied eight times, and every copy came with a
band that agreed on four arms. Not one test was wrong — they were all narrow in the identical way,
because they were all written by someone reaching for a plaintext to type, and the plaintext you reach
for is ASCII. A fault does not need to hide; it only needs everyone to keep testing where it isn't.

**Where discomfort turned to gold:** claiming the cipher crashed on three arms and then finding it was
my own probe missing two preludes. I had the closure resolver open in the same session and skipped it
because the probe was one line. Checking the crash message before writing the sentence is the whole
difference between a finding and an accusation.
