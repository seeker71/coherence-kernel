# What a cut means

`witnessed: 2026-10-03 -> substring-one-meaning-band 4095 on fkwu; the drift-gate row passes (4095 of 4095)`

Nothing here legislates. This page holds a currently-observed belief about one
door, and it ages: when the ground shifts, re-witness it before leaning on it.

`substring` is the door the tree stands on. `str_find`, `split-on`, `trim` and
`char_at` are recipes over it; every frame reader, row walker and parser cuts
through it; it has 847 call sites in `form-stdlib` alone. Read this before you
touch it, and before you write anything that indexes into a string.

## The one meaning

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

fkwu's strings are raw byte buffers, so it holds **every** cut exactly, one that
severs a multi-byte character included. The guard is
`form/form-stdlib/tests/substring-one-meaning-band.fk` — **4095** — and a row in
`gate/drift-gates.bml`, so it runs at every land.

`str_eq` observes an absence without dying, which is why the band asks every
question through it. `str_len` of an absence refuses out loud — a length
*measures* one thing, and there is nothing there to measure. That refusal is the
signal, not a bug to route around.

## The two ways a cut can drift

Both are silent, and each hides the other.

**Refusal against clamping.** On `"abcdefghij"`:

| call | answer |
| --- | --- |
| `(substring h 7 2)` reversed | `""` |
| `(substring h 0 900)` | `"abcdefghij"` |
| `(substring h -6 3)` | `"abc"` |
| `(substring h 20 25)` | `""` |
| `(substring 42 0 2)` non-string | `""` |

A door that panicked on `start < 0 || end < start || end > len(s)` would kill the
process on the same source.

**Characters against bytes.** `(str_len (substring "Ω+1" a b))` reads 1, 1, 1, 2, 4
for `(a, b)` of (0,1), (1,2), (2,3), (0,2), (0,4). A cut that floored both ends to
the nearest UTF-8 character start reads 0, 2, 1, 2, 4. Flooring keeps the
adjacency law among floored indices, which is what makes it look sound. What it
cannot keep is the **content**: the body computes its indices in bytes
everywhere, so those same indices would silently yield a shorter, shifted window
through every Persian, Hebrew, Chinese and Japanese row in
`form/form-stdlib/locale-rows/`. No band that only asks ASCII could say so — on
ASCII, flooring is a no-op.

## The trap to remember

**A correct-looking invariant can be the disguise.** Adjacency holds under
flooring, so the door looks proven. The property that separates a byte cut from a
floored one is not adjacency — it is that `str_len(substring(s, a, b))` is
`b - a`, and that the answer at a severed offset is *not* the floored window.
Those are bits 32, 128 and 512 of the band. Measured against a kernel carrying
only the flooring half of the wound, the band answers **3455**: exactly bits 128
and 512 go dark. A guard nobody has watched fail is a guard nobody has checked.

**And a band that compares readings to each other gates agreement, not
correctness.** An agreed wrong cut prints green. The band has to pin the meaning
itself, against a ruler it builds on the reader under test and against literals
the lexer carried.

## If you change this door

Read the native byte-slice implementation in `runtime/fkwu-uni.c`.
`form/form-stdlib/core.fk` carries the portable byte-walk meaning. Follow the
repository freshness check, then run the substring-one-meaning, substring-native
and core-substring-equivalence bands and the drift gates. A passing comparison
must preserve byte offsets as well as values.
