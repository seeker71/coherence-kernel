# 2026-07-26 — the tongue shelf crosses, and the waist is only half a dual

Yesterday's turn repaired `str-byte-at` and eight private copies of the same one-liner, then added a
non-ASCII claim to `bmf-core-band` and watched it read **700 on fkwu and 600 on go, rust and ts**. I
wrote in the band's own comment that the cause was the byte accessor I had just repaired.

It was not. The perturbation said so: with the old accessor swapped back in, the band still read 600.
That cleared the accessor, and the finding had to move.

## The same fault in a different spelling

`cur-peek` in `bmf-core.fk`, and its second copy in `grammars/bmf-core.fk`:

```
(if (ge p (surf-len sf)) (sub 0 1) (ord (char_at (surf-payload sf) p)))
```

`(ord (char_at …))`, not `(ord (substring …))` — which is why my eight-copy census walked straight past
it. I had counted one spelling of a fault and called it the fault.

Over `"Ω+1"`, whose Ω is the two bytes CE A9:

| | pos 0 | pos 1 | pos 2 |
|---|---|---|---|
| fkwu | 206 | 169 | 43 |
| go / rust | 206 | **−1** | 43 |
| ts | **937** | **−1** | 43 |

Three arms, three answers. And the function's own comment said *"codepoint at the cursor"* while the
branch six lines below it returned a raw byte from `bbc-byte` — so one function promised two different
things depending on which surface it was reading.

It reads through `str_byte_at` now, on both copies, and the comment says byte. Every consumer already
treated it as one: `bmf-grammar` compares it against 34, 39, 40, 46, 59, 91, and every `is-*-cp`
classifier is ASCII-bounded, so a continuation byte — always ≥ 128 — can never be mistaken for a
delimiter. Byte semantics is what a UTF-8 scanner wants. The word "codepoint" was the wrong promise.

## What that one line was holding shut

`bmf-core-band` went 600 → **700 on fkwu, go, rust and ts**. That was the claim I was chasing. It was
not the finding.

97 bands transitively prelude `bmf-core.fk`. On fkwu, captured entirely before and entirely after, all
97 read **identically** — the repair is a no-op there by construction, because fkwu's `char_at` is
byte-indexed. Third time this week that the whole value of a change is invisible from the arm this body
develops on. (Two of the 97 differed only in cache state, 2085 and 2659 errors either way; re-run cold,
identical. Both are red for their own reasons and were red before.)

On the three heavy kernels, three bands moved. One was `bmf-core-band`. The other two had not been
disagreeing. **They had been crashing.**

```
neutral-symbol-grammar-band   CRASH (as_nid: null) -> 15
nl-many-band                  CRASH (as_nid: null) -> 67108863
```

`as_nid: null` out of `nsg-kid` and `nlt-kidv` — a parse that walked into the middle of a Cyrillic or
Devanagari character got −1 and handed back a null node. Verified against the file as it stands in git,
not only against my perturbation.

## The door was named, and the name was exact

Neither band was hiding. `nl-many-band`'s header:

> PROOF LEVEL: FOURTH-ARM ONLY (fkwu). Not rostered four-way: the non-Latin scripts are blocked by the
> cross-kernel string-unit seam (fkwu char_at walks bytes; go/rust/ts return the whole char at a start
> and nothing inside it). **Cursor unit parity is the next door; when it opens, this band's roster row
> is the acceptance.**

`nl-many-latin-band` put it plainer still: *"Named, not hidden — cursor unit parity is the tongue
shelf's next door."* NORTH_STAR carried the same sentence.

Whoever wrote those lines measured the seam correctly, wrote down what would count as opening it, and
left the acceptance criteria sitting there for whoever arrived with the right line. The door was one
line wide. Fourteen tongues across six scripts — Cyrillic, Han, Arabic, Devanagari, kana, Latin — now
intern to one pivot node on all four arms, **67108863**, and the Latin slice stays at 1023 beside it as
the narrower claim.

I did not find this. Someone left me a note saying exactly where to push.

## Then the floor gave way

Having repaired a byte accessor, I went to annotate `substring` in `core.fk` and read its header:

> str_len / str_byte_at / byte_to_str (plus str_concat) are the native composition floor … measure,
> decompose (one raw byte, 0-255), construct (**the exact dual**), join. Confirmed present,
> **byte-identical**, on fkwu, Go, Rust, and TS.

Three of those four names are. Measured:

```
(str_len (byte_to_str n))         n=65  127  128  169  206  255
  fkwu                              1    1    1    1    1    1
  go / rust / ts                    1    1    2    2    2    2

(str_byte_at (byte_to_str n) 0)   n=65  127  128  169  206  255
  fkwu                             65  127  128  169  206  255    <- the dual
  go / rust / ts                   65  127  194  194  195  195    <- a lead byte
```

Above 127 the other three read `n` as a codepoint and hand back its two-byte UTF-8 encoding. **The
waist can take a string apart on every arm and put it back together in exactly one place.**

That is the root the whole week has been circling. `str_byte_at` — decompose — is sound everywhere,
which is why every repair that routed *through* it worked. `byte_to_str` — construct — is not, which is
why `substring`, written in Form over the waist, is byte-correct only on fkwu:

```
(str_len (substring "Ω+1" a b))   (0,1) (1,2) (2,3) (0,2) (0,4)
  fkwu                               1     1     1     2     4    raw bytes
  the recipe, on go/rust/ts          2     2     1     4     6    re-encoded
  the NATIVE, on go/rust/ts          0     2     1     2     4    char-floored
```

The recipe row needed a private name to measure at all: `core.fk`'s header says the kernels keep their
own `substring` when a same-named Form definition is loaded, *"so source compilers and cursors keep
byte indexing instead of accidentally routing through slower fallback string reconstruction."* On three
arms that override shadows a Form recipe with a native that floors each byte offset to the start of the
character containing it and then slices whole characters. Which is precisely what *"return the whole
char at a start and nothing inside it"* means, now measured rather than described.

**Is the capability missing, or only the name?** Only the name, on the walkers:
`(write_file_bytes p (list 206 169 65))` then `(read_file_slice p 0 3)` gives a three-byte string
reading 206, 169, 65 on go, rust and ts. On fkwu those two names are unbound and recover to `nothing` —
no file appears. Each side has exactly one door to a raw byte above 127 inside a string, and it is a
different door on each side. **No door is open on all four.**

`byte-waist-band.fk` pins this. It asserts no number above 127 — a band that did would be red on three
arms forever and teach nothing. It asserts the four-way floor plus the invariant that links the halves:
*the construct is the exact dual exactly when it produces one byte.* True on fkwu because both sides
hold, true on the others because both sides fail, red on any arm where they come apart. **63 on seven
evaluators.**

## Seven, not four — and a correction I owe

Running that band on all seven evaluators is what caught the next thing. The **minimal ts walker** read
**20** where the other six read 63. It measures strings in **UTF-16 code units**:

```
                        str_len "λ"   byte 0   str_len "🌊"
  everyone else              2          206         4
  the minimal ts walker      1          187         2      <- surrogate pair, low bytes
```

187 is `0x03BB & 0xFF` — the low byte of the codepoint, not the first byte of the encoding.

This is where I correct myself. Yesterday's receipt says **`str-byte-at` 511 ×4**. On the minimal ts
walker it was **15**. The other six evaluators were at 511 and I wrote "×4" over a set that did not
include the arm that disagreed. The number was not invented — it was scoped and I did not say to what.
Same for `room-cipher-band` **63 ×4**: fkwu and the three heavy kernels, since the minimal walkers
cannot run it at all (`bxor` is unbound there). Both sentences are true of a set I never named. Fixed by
naming: this body has **seven** evaluators, and "ts" is two of them that disagree above 127.

The walker's own source declared the gap, in the comment beside those three natives:

> Source string literals are read as proper UTF-8 … so any literal outside the Latin-1 range will NOT
> byte-count identically to fkwu's raw-byte view here — a real, bounded gap, not silently papered over.
> **Every test this walker actually needs to pass today is plain ASCII, where this is exact.**

Named, not hidden — and the last sentence stopped being true the day a band grew a non-ASCII claim.

One word: `readFileSync(p, "utf8")` → `readFileSync(p, "latin1")`. The three natives were already
latin1 in both directions; only the intake disagreed with them.

I scoped the check to what the change can reach — the 333 bands whose transitive closure carries a file
with non-ASCII bytes outside comments. 127 of them run to a number on that walker. Before and after:

```
byte-waist-band          20 -> 63
str-byte-at-band         15 -> 511
name-lexicon-band      2005 -> 2047
locale-registry-band    127 -> 255
```

**Four changed, all upward, nothing else moved.** The last two I was not looking for. `name-lexicon.fk`
says its walk *"splits only at ASCII delimiters … so the cross-kernel string-unit seam is not crossed,
and round-trip identity holds on all four arms."* The reasoning is right and the conclusion was right
for six evaluators; on the seventh it was 2005, not 2047, and had been all along. All four now read the
same on all seven.

## Sweep

`ground` 42 · `proof/four-way-run-recipe42.fk` 0 (FOUR-WAY) · C seed byte-identical to git.

Seven evaluators: `byte-waist` **63** · `str-byte-at` **511** · `name-lexicon` **2047** ·
`locale-registry` **255**.

fkwu + three heavy kernels: `bmf-core` **700** · `nl-many` **67108863** ·
`neutral-symbol-grammar` **15** · `nl-many-latin` 1023 · `room-cipher` 63.

fkwu: `bmf-grammar` 2047 · `bmf-cursor-language` 1023 · `json` 1023 · `keyed-map` 4095 ·
`primitive-registry` 45 · `say` 255 · `primitive-edge-contracts` 1023 · `navier-stokes` 1023 ·
`navier-stokes-plate` 2047 · `uuid` 11 · `hex` 14 · `base64` 12 · `url-encode` 13 · and 97 bmf-core
dependents unchanged across the repair.

## Owed

- **`byte_to_str` above 127.** Named in `core.fk` and pinned by a band; the repair is a kernel change in
  three kernels, and the shape of it — raw byte, or codepoint — is the owner's call, not mine. The
  capability exists there under `write_file_bytes` + `read_file_slice`; it is the reverse on fkwu.
- **`cur-peek-char` still crosses the seam.** `(substring s p (add p 1))` reads 1 byte on fkwu and, on
  the others, floors to the character start: lengths `[1,1,1,1]` against `[0,2,1,1]` over `"Ω+1"`. It is
  used in three bands and nowhere in the engine, so nothing depends on it today.
- **`(ord (char_at …))` elsewhere: 60 cells, 193 sites**, counted with comments and strings stripped —
  `seedbank/grammar-bnf.fk` 12, `json.fk` 11, `seedbank/grammars/form.fk` 11. That is a count of source
  reads, not of faults. Two of them bit, and both were found by running, not by grepping. The rest are
  uncounted until something runs them on non-ASCII.
- `come-in-band` stays fourth-arm-only for its own declared reason — `(nothing)` is walker tag 137,
  never registered. Measured today; not this seam.
- `uuid-band` crashes on go and rust with `bp: unreviewed bootstrap name: UUID`. `url-encode` 13 on
  fkwu against 16 on the walkers, still unexplained. 751 `print` calls silent on fkwu.

## How the exchange stayed alive

I wrote a wrong cause into a band comment, and the perturbation I had already promised to run said
otherwise. Everything in this receipt is downstream of believing it over my own sentence.

**Most surprising teaching:** the two things that mattered most were both already written down by
someone who could not reach them. `nl-many-band` said *cursor unit parity is the next door; when it
opens, this band's roster row is the acceptance* — a door, its key, and the acceptance test, left in a
comment. The ts walker's source said *every test this walker actually needs to pass today is plain
ASCII* — an honest scope that quietly expired when someone else added a claim. Neither was hidden.
Both were waiting. What this body needs is not more findings; it is someone willing to run the note
that is already there.

**Where discomfort turned to gold:** finding, three receipts deep, that my own `511 ×4` was scoped to a
set I never named — and that the arm left out was the one that disagreed. I did not invent a number; I
said "four" about seven and let the reader supply the wrong four. That is the sharper failure of the
two, because it looks exactly like a measurement.
