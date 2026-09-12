# The kind asked outright

2026-09-12, afternoon, M4 Max, Hati Suci. Rungs 6 to 8 of the goal Urs named for the core recipes
land together: a leaf that builds a string, so int_to_str's digits leave the C seed; a string as a value
in a leaf, with a cold callee warmed for the types its call carries; and the list door, with typefall and
a small defn inlined. They were written in the epic-edison worktree and rebased onto e91bee6d3. The
landing carried one heal.

## Carried

- **Rungs 6 to 8 land** (597bb6e0c, 6c87992dc, ef80b398d, 163b6e98a), with rows 1495 scratchfall, 1496
  impliedheat and 1497 typefall.
- **int_to_str asks value_kind** (f42573e65). The recipe told a float from an integer with fstr-float?,
  which runs (mul n 2) on whatever it is given, and since 03a03f6f4 arithmetic over a non-number stops.
  So int_to_str "abc" stopped on fkwu, where the C arm it replaces, Go, Rust and TS pass a string
  through. It now asks value_kind once: a string, a list or a float reads as value_str writes it, and
  every other kind takes the digit loops. value-str-band gains bit 128, a string through int_to_str on
  all four kernels, and reads 255; loop-lane-string-build asks int_to_str at both tagged limits and of
  a string where it asked fstr-float?.
- **The form-cli bootstrap follows** (e4b6299fe).
- **useprobe is row 1498** (dfbc4230d).

Witnessed: a differential probe of int_to_str on 24 inputs, and one of the list door, nil? and
str_to_int on 19 more, each cold and after 3000 calls, equal line for line between e91bee6d3's kernel
and the landed one. At e4b6299fe: validate.sh on value-str 255 and str-to-int-reading 127 with go,
rust and typescript agreeing, every loop-lane band on the fkwu lane, jit-leaf-inram 63 and its
multiarg 63, jit-native-span 127, jit-lower-emit 63, f64-wire 2047, float-parity 255, born-under 31
and host-process 127; jit-lens 16383, once-hold 7 and float-mint 63 on fkwu directly; TestFkwu;
freshness 31, the corpus band 32767, the drift run 8191 of 8191, porcelain 0 before and after.

## Still open, measured

- **value_str is still leaf mode 25 in C.** The next rung, a leaf that stores the bytes of any value,
  is the one that lets it leave; int_to_str's mode 26 left with rung 6.
- **The inliner hides a small leaf from its own heat.** Every lane band's walker twin now holds
  (lt (host_monotonic_ms) 0) in its compare to stay cold, and a band that claims a small leaf's state
  drives it from a walker.
- **nothing** reads "nothing" through int_to_str on fkwu and "null" on Go, Rust and TS.
- **One regen answered an empty table inside the script** where the same binary and request by hand
  answered a table (receipt 44).
- The open items of receipts 12 to 44 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was that the same test failed twice, in opposite directions. An arithmetic kind test first
let a string through as a number, because the operation answered a number for any word; once arithmetic
stopped on strangers, the same test stopped the string. A test that learns a kind by using the value is
only as sound as the use's manners toward strangers. Asking value_kind outright holds both ways.

The discomfort was meeting the same gap a second time, in a rung that had been rebased and had grown by
two more. The pull was to trust the witness list that came with it; it named value-str at 127, and the
missing bit was the one that mattered. What turned it was the differential probe, the same twenty-four
inputs as before against main's own kernel, which answered in one line where the string stopped, and
then carrying the heal on top of the rungs instead of sending them round again.

Frontier word, row 1498: **useprobe**, a kind test that uses a value and watches what survives, instead
of asking what it is.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
