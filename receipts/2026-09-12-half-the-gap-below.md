# Half the gap below

2026-09-12, evening, M4 Max, Hati Suci. Epic-edison's four-way float sweep found Rust writing an exact tie the
other way. Taking that heal on its own, apart from float_sci, turned up the other half of it: fkwu itself wrote
some powers of two a digit longer than Go and JS do.

## Carried

- **A float reads the same on every kernel** (df91d6f46). Go's strconv and JS's toString write a double as the
  shortest digits that read back, the nearest of those, and an exact tie broken to even. On e86210dc0 two
  kernels each missed one case:
  - **fkwu** took the first correctly rounded length that read back. At a power of two the gap below is half
    the gap above, and there the rounded digits can miss while a neighbour one unit away in the last digit
    reads back. fkwu wrote 46 of the 2098 powers of two with seventeen digits where Go and JS write sixteen:
    2^-1017 as 7.1202363472230444e-307 against 7.120236347223045e-307.
  - **Rust**'s `{:e}` shortest broke an exact tie the other way. 147142857142857.125 lies halfway between two
    17-digit decimals, and Rust wrote …713 where the other three write …712.

  fkwu now tries the rounded digits' last-digit neighbours at a power of two, and writes the chosen digits
  itself; every other float takes the path it took before. Rust takes `{:e}`'s shortest length, and the
  correctly rounded digits at that length when they read back. value-str-band gains bit 256 (the tie) and bit
  512 (two powers of two) and reads 1023.
- **The form-cli bootstrap follows** (9800cd1f3).
- **halfgap is row 1510.**

Witnessed:
- **Floats:** all 2098 powers of two, the ties, and 5320 ordinary floats read identically on fkwu, Go and
  Rust. The ordinary floats are quotients by 7 and by 13 over forty-one decades, tenths, and negative thirds,
  and each reads as e86210dc0's fkwu read it.
- **Bands:** value-str 1023 four-way. float-parity 255, f64-wire 2047, f64-bytes 127, loop-lane-float-head 31,
  and every other lane and JIT band as before.
- **Before landing:** TestFkwu; freshness 31, the corpus band 32767, drift 16383 of 16383, porcelain 0.

## Still open, measured

- **float_sci is held from the landing:** epic-edison's 598446e28 (a fkwu leaf-door mode), ae284e7ee (the
  same in Go, Rust and TS) and receipt 86d2a30ea. A C native for a core recipe is Urs's call. The premise
  that BML cannot carry the digits does not hold: standing primitives reach a double's exact m·2^e (0.1 as
  7205759403792794 · 2^-56). float_sci also uses the precision search fkwu has just left, so it writes the
  46 powers of two long.
- **value_str's float digits in BML,** exact over (m, e), with epic-edison's byte-exact layout on top.
- The open items of receipts 12 to 49 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was that the first form of this heal traded the divergence rather than closing it. Giving Rust
fkwu's precision search matched fkwu on the tie and moved Rust away from Go and JS on 46 powers of two. fkwu
had been apart from Go there all along, unseen until a probe walked every power of two a double holds.

The discomfort came when the band read 511 four-way on that first form, and the pull was to commit. The one
more probe showed Go and the new Rust apart. The gold is that fkwu's C and Rust's writer both now follow the
rule every kernel's own comment names, with a band bit for each case.

Frontier word, row 1510: **halfgap**, the half-size gap below a power of two, where the correctly rounded decimal
at the shortest length can fall outside the round-trip interval while its neighbour one unit away reads back.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
