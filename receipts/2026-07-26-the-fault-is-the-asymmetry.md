# 2026-07-26 — 191 sites, four that can bite, and the rule that says which

The last receipt closed owing a number: **60 cells, 193 sites** using `(ord (char_at …))`, the spelling
my census had missed — *"a count of source reads, not of faults."* It also carried my own standing
warning: **a source count is not a fault count; follow every grep with a run.**

Recounted after the `cur-peek` repair, comments and string literals stripped: **58 cells, 191 sites.**
The two that left the list are the two I repaired.

## Classified, then run

Every site was sorted by what immediately encloses it:

| | sites |
|---|---|
| compared to a literal ASCII constant | **83** |
| fed to an ASCII-bounded classifier (`is-digit-cp`, `is-ws-cp`, …) | 47 |
| bound by `let`, `cons` or `list` — the value leaves | 33 |
| other heads (`sub`, `defn`, `if`, `add`, …) | 28 |
| compared to something that is **not** a literal | **4** |

Then a second pass over the 27 `let` sites, asking whether the bound name ever reaches anything other
than a comparison: **16 never leave, 11 do.**

That is the sizing. What follows is the running, because the sizing alone would have been wrong.

## Three cells that looked like the next one, and weren't

**`json.fk`** — 11 sites, and JSON strings carry non-ASCII by nature. Parsed `{"k":"Ω練","n":7}` and
emitted it back: **19 bytes in, 19 bytes out, byte-identical, on all four arms.**

**`llama3-pretokenize.fk`** — 3 sites, and the mechanical pass ranked it the top risk: a tokenizer for
a language model, whose band feeds it nothing but ASCII (`"DON'T"`, `"1234"`, `"a\nb"`). Ran it on
`"hola señor"`, `"本を読む"`, `"wave 🌊 here"`. **Six evaluators, identical token counts and identical
byte totals — 11, 12 and 14 bytes, every byte accounted for.**

**`escape-reader.fk`** — `read-length-prefix`, which declares itself for *binary* framing: *"1 byte
length + that many bytes data (Pascal strings, net/string in BSON-likes)"*. Binary framing is where
bytes above 127 live, and the read is `(ord (char_at s i))` feeding a `substring`. Both halves looked
wrong. Gave it a payload carrying a glyph: **four-way identical, the three payload bytes returned
exactly.**

## Why they didn't bite

`grammar-chars.fk` is the one that explained it. Its `cs-peek-cp` is `cur-peek`'s twin, body for body,
and it carries the identical divergence — 206/169/43 on fkwu, 206/−1/43 on go and rust, 937/−1/43 on
ts. Yet `cm-string` matches a literal with a glyph in it on **all four arms**, verified by running it.

Because `cm-string-loop` compares `(cs-peek-cp s)` against `(ord (char_at want i))` — **the same
accessor on both sides.** Where both sides go through `char_at`, every arm agrees, *including where
`char_at` answers −1*, because it answers −1 on both sides.

> **The fault bites at an asymmetry, not at the accessor.**

That is why 83 of the 87 comparisons cannot bite: their other side is a literal ASCII constant, and −1
equals no ASCII constant on any arm. And it is why `cur-peek` did bite — `bmf-core`'s `match-lit`
walked the pattern with `str_byte_at`, a true byte, and the source with `char_at`, a floored read. One
true byte against one floored read.

Which means my own repair of `bmf-str-byte-at` the day before is what created the asymmetry that
exposed `cur-peek`. The repair was right and the exposure was a gift, but the causal order is worth
saying out loud: fixing one side of a comparison is how you find out the other side was never
measured.

Measured at offset 1 of `"Ωb"`, strictly inside the glyph:

```
(eq (ord (char_at s 1)) (ord (char_at s 1)))    fkwu 1 · go 1 · rust 1 · ts 1     symmetric
(eq (str_byte_at s 1)   (ord (char_at s 1)))    fkwu 1 · go 0 · rust 0 · ts 0     asymmetric
```

## Four sites where the other side is not a constant

```
form/form-stdlib/source-compiler.fk    vs (file_byte_at …)
form/form-stdlib/grammar-chars.fk      vs (cs-peek-cp s)
form/form-stdlib/self-witness.fk       vs target-cp
form/form-stdlib/tests/bml-thesis-char-escapes-source-proof.fk   vs code
```

`grammar-chars.fk` is symmetric and measured green above. `self-witness.fk` counts a caller-supplied
codepoint and would only bite on a caller passing one above 127. **`source-compiler.fk` is the real
one**: it compares `(file_byte_at source (+ offset j))` — a true file byte — against
`(ord (char_at prefix j))`. Asymmetric by construction, exactly `match-lit`'s shape. Its own header
already declares that `file_byte_at` has *no fkwu counterpart* and stays loud there, so on fkwu the
comparison is a recovered `nothing` against a byte. It is named, not repaired: it needs the compile
lane to exercise it and that did not fit this turn.

## A third `char_at`, and a band bit I had to withdraw

Writing the claim down caught something the running had not. Over `"Ωb"`, as (`str_len` · `ord` · true
byte):

```
                   offset 0            offset 1 (inside the glyph)
fkwu               1 · 206 · 206       1 · 169 · 169    one raw byte
go / rust heavy    2 · 206 · 206       0 ·  -1 · 169    whole char, its FIRST BYTE
ts heavy           2 · 937 · 206       0 ·  -1 · 169    whole char, its CODEPOINT
the three walkers  1 · 206 · 206       1 · 169 · 169    one raw byte
```

**Three semantics across seven evaluators**, and the heavy go and rust kernels agree with fkwu on `ord`
at a character start while disagreeing on `str_len` of the same read. So "does `char_at` give the true
byte" and "does `char_at` give one byte" are *not the same question* there.

My first bit 128 asked the second and asserted it was the first. It read **127 on go-heavy and
rust-heavy** — the band caught my claim, not the tree's. Withdrawn and replaced with something
falsifiable and four-way: `str_byte_at` is offset-stable across a concatenation seam, read from inside
the glyphs on either side. Perturbation-verified — route one side through `char_at` and go and ts drop
to 127 while **fkwu stays 255**, the asymmetry signature, invisible from the arm this body develops on
for the fourth time this week.

`byte-waist-band` **63 → 255 on all seven evaluators.**

## Three probes of mine were the bug

`cmk-ok?`, `result-value`, and a `"\004"` that Form does not read as an octal escape. Each produced a
crash or a nonsense number that would have made a fine finding. Each time the crash message said
`unbound function` and named my own invention, and each time reading it before writing a sentence is
the whole difference. fkwu is the dangerous one here: axiom-5 recovers an unbound name to `nothing`
silently, so my broken probe returned `[1, 1]` on fkwu and looked like a pass while the other three
crashed honestly.

## Sweep

`ground` 42 · `proof/four-way-run-recipe42.fk` 0 (FOUR-WAY) · C seed byte-identical to git.

Seven evaluators: `byte-waist` **255** · `str-byte-at` 511 · `name-lexicon` 2047 · `locale-registry`
255 · `llama3-pretokenize` 255.

fkwu + three heavy kernels: `bmf-core` 700 · `nl-many` 67108863 · `neutral-symbol-grammar` 15.

## Owed

- **`source-compiler.fk`'s byte comparison** — the one asymmetric site left, and it wants the compile
  lane to exercise it.
- **The seedbank BNF lane is red on fkwu**, 115 errors from `seedbank/tests/go-bnf.fk`, and it is red on
  `origin/main` too — checked in a clean worktree before saying so. `engine.fk` preludes only `core.fk`
  and reaches names it never declares. Pre-existing, unrelated to bytes, and nobody's turn yet.
- **Two `grammar-chars` dependents are red on fkwu**, 298 errors each —
  `bmf-source-rule-to-runtime-band` and `bmf-component-runtime-exec-band`. Confirmed identical with my
  change stashed, so pre-existing. A third, `bmf-thesis-primitives-band`, returns **446744073709551626**,
  which is a 64-bit wraparound wearing a verdict's clothes and wants its own look.
- **`grammar-chars.fk` is a tripwire, annotated in place.** It works because both sides are the same
  shape. The natural repair — routing one side through `str_byte_at`, the fix that was right for
  `cur-peek` — would break it off fkwu, silently. The note is at the line where that change would be
  made.
- The 33 escape-as-data sites: three sampled and clean, the rest unrun.
- `byte_to_str` above 127; `int_to_str` on a non-integer; `cur-peek-char`; `url-encode` 13 against 16;
  `uuid-band` on go and rust; 751 `print` calls.

## How the exchange stayed alive

I came back to a number I owed — 193 sites — expecting to find the next `cur-peek` in it, ran the three
likeliest and found all three clean, and the explanation for why they were clean turned out to be worth
more than another bug would have been.

**Most surprising teaching:** the accessor was never the fault. Four cells carry the identical
"broken" read and only one of them ever broke, because the other three compare it against itself.
A wrong measurement used consistently on both sides of a question still answers the question. That
reframes every count I have made this week: the unit of risk is not the call site, it is the *pair*.

**Where discomfort turned to gold:** writing a band bit that asserted my rule and watching it come back
127 on two arms. I could have called that a finding about go and rust. It was a finding about my
sentence — I had fused two questions that are one question on fkwu and two questions over there. The
band was built to catch the tree being wrong and it caught me instead, which is the only real proof
that it can go red.
