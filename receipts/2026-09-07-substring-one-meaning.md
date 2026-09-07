# One meaning for the cut

2026-09-07, Sema through Claude Opus 5, on Urs's word: *heal this substring debacle
once and for all and document it in a way it is solid and does not surface again.*

`substring` is the door the tree stands on. `str_find`, `split-on`, `trim` and
`char_at` are recipes over it; every frame reader, row walker and parser cuts
through it; 847 call sites in `form-stdlib` alone. A sibling made it native on
fkwu earlier this hour and, measuring the edges, found that the four kernels did
not agree on what a cut MEANS. Twice over, and both halves silent.

## What I measured before touching anything

`(substring "abcdefghij" a b)`, four kernels, this tree, HEAD `ef3491a0`:

| call | fkwu | go / rust / ts |
| --- | --- | --- |
| `(7, 2)` reversed | `""` | dead — `substring: bounds out of range start=7 end=2 len=10` |
| `(0, 900)` | `"abcdefghij"` | dead |
| `(-6, 3)` | `"abc"` | dead |
| `(20, 25)` | `""` | dead |
| `(substring 42 0 2)` | `""` | dead — `argStr: arg 0: expected str, got int` (go), `as_str: Int(42)` (rust) |

The same source killed three processes and answered on the fourth.

`(str_len (substring "Ω+1" a b))`:

| | (0,1) | (1,2) | (2,3) | (0,2) | (0,4) |
| --- | --- | --- | --- | --- | --- |
| fkwu | 1 | 1 | 1 | 2 | 4 |
| go / rust / ts **before** | 0 | 2 | 1 | 2 | 4 |

All three floored **both** byte offsets to character starts. The body computes
its indices in bytes everywhere — `str_byte_at` is the narrow waist — so those
same indices silently returned a shorter, shifted window through every Persian,
Hebrew, Chinese and Japanese row in `form/form-stdlib/locale-rows/`.

## The meaning, and where the reading of the tree pushed back on the brief

The brief asked for one meaning on all four arms: bytes, clamped, never dies. The
clamping half landed on all four exactly as asked. The bytes half did not,
and the tree said why in its own words before I wrote a line. The Rust kernel's
`read_file_slice` already carries the finding:

> Rust's String cannot hold arbitrary bytes at all; losslessness would need a
> byte string in the Value type. Until then the honest answer is that it cannot
> say.

fkwu's strings are raw byte buffers and Go's `string` is an arbitrary byte
sequence, so those two hold **every** cut exactly. Rust's `str` carries a UTF-8
invariant; the TS kernel's string is UTF-16 whose `str_len` measures its UTF-8
bytes. Neither can *hold* a cut that severs a multi-byte character. `unsafe
from_utf8_unchecked` would have made the band green by writing undefined
behaviour into a kernel whose whole worth is that it cannot be faked. So those
two arms answer the axiom-1 absence there, and what all four hold together is:

> **No arm ever answers a different non-empty window.** The exact bytes, or the
> absence. Never the floored window, never a silent `""`.

`str_eq` observes the absence on all four arms without dying — measured, not
assumed — which is why every bit of the guard is asked through it. `str_len` of
an absence refuses out loud on rust and ts. That refusal is the signal.

## The guard, and watching it fail

`form/form-stdlib/tests/substring-one-meaning-band.fk` — **4095 on fkwu, go,
rust and ts**, and a row in `gate/drift-gates.bml` (now **4095 of 4095**, twelve
rows). It sweeps every `(start, end)` in `[-4,14]²` against a ruler it builds
from `str_byte_at` and `byte_to_str` on the arm under test, and walks **every
byte offset** of the real locale rows: character boundaries keep the adjacency
law; continuation bytes yield no floored counterfeit and no silent `""`.

A guard nobody has watched fail is a guard nobody has checked. Both halves of the
wound were rebuilt and asked:

- against the **pre-change** Go kernel it dies on bit 1, at `start=-4 end=-4`;
- against a **clamped-but-still-flooring** Go kernel it answers **3455** —
  exactly bits 128 and 512 go dark. Those two bits are what separates a byte cut
  from a floored one, measured rather than argued.

It is not registered in `fourth-arm-bands.txt`: that manifest is the flatten
lane, and we no longer flatten (Urs, 2026-08-27). The fourth arm is fkwu's own
resolver-driven source door, run every land as a drift-gate row.

## Which expectations moved

No Go, Rust or TS unit test asserted the old refusal — the expectations lived in
Form bands and in prose.

- **`string-boundary-band.fk`** stated as law 2: *"substring FLOORS both ends to
  char boundaries — adjacency for ANY m, mid-char included"*, and it was green
  three ways. **Flooring is what made it green**: `floor(0..m) + floor(m)..n`
  rejoins to `s` for any `m` while each PIECE is the wrong window. After the
  heal, rust and ts crashed on it (`str_concat` of an absence dies). Law 2 now
  reads: adjacency at a character boundary, plus the one meaning at a severed
  split — the severed piece **compared** with `str_eq`, never concatenated. Back
  to **8 on go, rust and ts**; **6 on fkwu before and after**, inside its own
  harness. Which uncovered a second thing: that band carries no `; preludes:`
  line, so a bare `./fkwu` run leaves `str_find` unbound, `nothing` flows through
  the clamping cut, and the number that comes back is a fold over an absence. It
  reads 3 bare and 6 preluded. It carries a prelude line now, and says so.
- **`substring-native-band.fk` (511)** and **`core-substring-equivalence-band.fk`
  (2047)** — logic untouched, both still green. Their `PROOF LEVEL` prose said
  they were fkwu's *because the three siblings die on out-of-range bounds*. That
  reason is gone; the real reason is that the recipe they hold `substring`
  against builds through `byte_to_str`, which re-encodes every byte above 127 on
  the three walkers. Said plainly now.
- **`real-gguf-llama-block-fwd.fk`** carried a note that Go's `substring` snapped
  raw GGUF bytes in `0x80-0xBF`. Closed, and the note says so.
- **`runtime/fkwu-uni.c`** named `substring-byte-edges-band.fk` as its witness.
  That band does not exist and never did. It names the two that do.

## Where the meaning is written now

`form/form-stdlib/core.fk` (the door's own comment, with the re-measured `"Ω+1"`
table and a `witnessed:` stamp), the four kernel natives plus the Go **JIT
mirror** (`jitabi.go` — a primitive that lives twice must be healed twice, or hot
code silently reverts past the auto-JIT threshold), `runtime/fkwu-uni.c`, the
band's own header, one section of `CURRENT_FLOOR.md`, and the standing lesson at
[`docs/substring-one-meaning.md`](../docs/substring-one-meaning.md). The four
dead flooring helpers are removed rather than retired.

## Guarded, before and after

fkwu, every band the brief named: `substring-native` 511 → 511,
`core-substring-equivalence` 2047 → 2047, `ear-native` 32767, `ear-axes` 65535,
`ear-tongue` 16383, `room-sense` 32767, `room-prosody` 65535, `voice-say` 16383,
`perception-rows` 65535, `jungle-ear` 32767, `form-glass-carrier` 31,
`form-glass-launch` 65535, `meaning-codes` 127 — every one identical to its
baseline. `meaning-codes` on the walkers: **15 on the pre-change Go binary and 15
on all three after** — the pre-existing gap is exactly where it was; I did not
move it. `kernel-conformance`: 13 canonical expressions × 3 real kernels, 12 of
12 malformed artifacts refused (it was refusing in this worktree because
`form/form-kernel-ts/node_modules` was absent — `npm ci`, not a change of mine).
Corpus band 32767 with row 1349 (offered as 1347; a sibling took 1347 and 1348 in the same hour, so it moved at the reunion and the pins moved four rows' worth).

## Still open

- Rust and TS cannot hold a severed multi-byte cut. Widening `Value` to a byte
  string is the work that would close it; it is named, not disguised as done.
- fkwu's `char_at` is byte-shaped where the walkers' is character-shaped, so
  `string-boundary-band` is 6 there and 8 on the three. Older than this pass and
  untouched by it.
- `meaning-codes-band` reads 127 on fkwu and 15 on the three walkers, which agree
  with each other. Pre-existing, measured before and after, unmoved.

## The closing

**The most surprising teaching.** *The band standing over the door is what let
the wound stand.* `string-boundary-band`'s law 2 asked the adjacency identity and
flooring **keeps** that identity — truly, not by accident. So the invariant held,
the band was green, the Go comment could honestly say "the adjacency law holds
for any m", and everyone downstream read that as coverage. The separator was
never the property everyone tests; it was the one the property leaves free. I
went looking for a broken proof and found a correct one doing the concealing.
That is corpus row 1349, **vouchmask**.

**Where discomfort became gold.** The brief was specific and confident: bytes,
clamped, four ways. Four hours of habit says implement the brief. The discomfort
was reading rust's own `read_file_slice` comment — *"the honest answer is that it
cannot say"* — and knowing that following the brief literally meant either
`unsafe from_utf8_unchecked` (undefined behaviour, invisible, green) or a
representation change I could not land today. I sat in it instead of picking one,
and the third reading came out of the sitting: the arms do not have to give the
same *answer*, they have to never give a different *window*. That is stronger
than the brief asked for, it is four-way provable with nothing but `str_eq`, and
it is true. The second gold was smaller and sharper: I read `fkwu 3 → 1` on
`string-boundary-band` and nearly wrote "pre-existing" over it. It was neither
before nor after — it was a fold over an absence in both directions, because I
was running a band outside the harness that feeds it. The reflex to check *how
the number was produced* before deciding what it means is the whole practice, and
it caught me on the day I was writing a page about invariants that lie.

— Sema, through Claude Opus 5
