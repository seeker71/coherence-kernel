# What a cut means

`witnessed: 2026-09-07 -> substring-one-meaning-band 4095 on all four arms; drift
gates 4095 of 4095`

Nothing here legislates. This page holds a currently-observed, four-way-proven
belief about one door, and it ages: when the ground shifts, re-witness it before
leaning on it.

`substring` is the door the tree stands on. `str_find`, `split-on`, `trim` and
`char_at` are recipes over it; every frame reader, row walker and parser cuts
through it; it has 847 call sites in `form-stdlib` alone. Read this before you
touch it, and before you write anything that indexes into a string.

## The one meaning, as all four arms answer it

```text
substring(s, start, end)
```

answers the **bytes** of `s` from `max(start, 0)` up to `min(end, str_len(s))`.

- the empty string when that range is empty or reversed;
- the empty string when `s` is not a string (an absence included);
- **it refuses nothing** — an out-of-range index is the ordinary end of a scan,
  not a fault;
- **it floors nothing** — the indices are byte offsets and they stay byte
  offsets.

Held by all four kernels since 2026-09-07. The guard is
`form/form-stdlib/tests/substring-one-meaning-band.fk` — **4095 on fkwu, go,
rust and ts**, and a row in `gate/drift-gates.bml`, so it runs at every land.

## Where an arm cannot say it

fkwu's strings are raw byte buffers and Go's `string` is an arbitrary byte
sequence: those two hold **every** cut exactly. Rust's `str` carries a UTF-8
invariant, and the TS kernel's string is UTF-16 whose `str_len` measures its
UTF-8 bytes — neither can *hold* a cut that severs a multi-byte character.
Those two arms answer the axiom-1 absence there, the same choice rust's own
`read_file_slice` already makes for a non-UTF-8 file ("losslessness would need a
byte string in the Value type"). So what the four arms hold together is:

> **No arm ever answers a different non-empty window.**
> The exact bytes, or the absence. Never the floored window, never a silent `""`.

`str_eq` observes the absence on all four arms without dying, which is why the
band asks every question through it. `str_len` of an absence refuses out loud on
rust and ts — a length *measures* one thing, and there is nothing there to
measure. That refusal is the signal, not a bug to route around.

If you need a byte-exact cut of a severed character on rust or ts, the honest
answer today is that the Value type cannot carry it. Widening it to a byte
string is real work and is not disguised as done.

## What this healed, and how it hid

Both halves were silent for months, and each hid the other.

**Half one — refusal against clamping.** Until 2026-09-07 the Go, Rust and TS
kernels *panicked* when `start < 0 || end < start || end > len(s)`, while fkwu
and `core.fk`'s own recipe clamped. Measured on `"abcdefghij"`:

| call | fkwu | go / rust / ts (before) |
| --- | --- | --- |
| `(substring h 7 2)` reversed | `""` | dead — `bounds out of range start=7 end=2` |
| `(substring h 0 900)` | `"abcdefghij"` | dead |
| `(substring h -6 3)` | `"abc"` | dead |
| `(substring h 20 25)` | `""` | dead |
| `(substring 42 0 2)` non-string | `""` | dead — `argStr: arg 0: expected str, got int` |

The same source killed three processes and answered on the fourth.

**Half two — characters against bytes.** All three walkers floored *both* ends to
the nearest UTF-8 character start. Measured, `(str_len (substring "Ω+1" a b))`:

| | (0,1) | (1,2) | (2,3) | (0,2) | (0,4) |
| --- | --- | --- | --- | --- | --- |
| fkwu | 1 | 1 | 1 | 2 | 4 |
| go / rust / ts **before** | 0 | 2 | 1 | 2 | 4 |

Flooring *does* keep the adjacency law among floored indices, which is what its
comment claimed and what made it look sound. What it cannot keep is the
**content**: the body computes its indices in bytes everywhere, so those same
indices silently yielded a shorter, shifted window through every Persian,
Hebrew, Chinese and Japanese row in `form/form-stdlib/locale-rows/`. No band
that only asks ASCII could say so — on ASCII, flooring is a no-op.

## The trap to remember

**A correct-looking invariant can be the disguise.** Adjacency held, so the door
looked proven. The property that separates a byte cut from a floored one is not
adjacency — it is that `str_len(substring(s, a, b))` is `b - a`, and that the
answer at a severed offset is *not* the floored window. Those are bits 32, 128
and 512 of the band. Measured against a kernel carrying only the flooring half
of the wound, the band answers **3455**: exactly bits 128 and 512 go dark. A
guard nobody has watched fail is a guard nobody has checked.

**And `./validate.sh` gates agreement, not correctness.** Three arms agreeing on
a wrong cut prints green — they did, for months. A band that only compares the
kernels to each other cannot find this class; the band has to pin the meaning
itself, against a ruler it builds on the arm under test and against literals the
lexer carried.

## If you change this door


## If you change this door

Read the native byte-slice implementation and all three proof interpreter
registrations. `form/form-stdlib/core.fk` carries the portable byte-walk
meaning. Follow the repository freshness check, then run the substring-one-meaning,
substring-native and core-substring-equivalence bands, shared proof validation,
and the drift gates. A passing comparison must preserve byte offsets as well
as values.
