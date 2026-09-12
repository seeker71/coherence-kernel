# The kernel that did what the comment said

2026-09-12, around a quarter past ten in the morning, M4 Max, Hati Suci. Receipt 34 left TS printing
lists without commas, holding 1.0 equal to 1, and carrying integers past 2^53 as doubles.

## Carried

- **TS prints and compares as its siblings do** (d7a32ed5c). renderForPrint joins a list's items with
  ", ", so `[1, 2.5, x, [3, 4], 2e+06]` prints alike on all four kernels. value_eq compares integers
  across their widths and floats across theirs, and never an integer against a float, as the one Int
  kind and the one Float kind of Go and Rust do. bml-float-literal-band reads 2047 on TS, where it
  read 1023.
- **TS folds integers in int64.** The bare-width add, sub, mul, div and mod fold in JS numbers while
  every step stays within ±(2^53−1). An operand carried as a bigint, or a step that leaves that range,
  refolds the expression in BigInt, wrapped at 64 bits as Go's and Rust's int64 wraps. float-parity-band
  reads 255 on TS, where it read 241, and abs, pow and int_to_str take a bigint as it is. A probe of
  fourteen big-integer lines, from 2^53 + 1 to the int64 wrap of 10^22 and of 3^40, prints on TS
  exactly as on Go.
- **band, bor and bxor combine the whole int64 word on every kernel.** The comments above them in Go,
  Rust and TS all said 32-bit-unsigned semantics. Go's and Rust's code combined int64; TS's did what the
  comment said, and its `>>> 0` wrote any negative answer as its unsigned 32-bit form. So
  `(bor (add 9007199254740992 1) 4)` read 4 on TS and 9007199254740997 on the other three. TS's
  bitwiseInt now splits a safe integer into a signed high half and an unsigned low half inside JS's
  32-bit operators, and takes BigInt past 2^53. Go's and Rust's comments say what their code does.
- **str_to_int reads one way on four kernels.** fkwu's reading lives in core.fk: leading space, tab,
  LF and CR are skipped, one leading "-" negates, and the digits run to the first non-digit, so shell
  output's trailing newline reads as nothing. TS's parseInt read nearly that way. Go's ParseInt and
  Rust's parse answered 0 for anything but a bare signed integer, so `"  42"` and `"12abc"` read 42 and
  12 on fkwu and TS and 0 on Go and Rust. Go, Rust and TS now carry fkwu's reading as a native
  (leadingInt, leading_int), exact past 2^53, and str-to-int-reading-band holds the four to it. It reads
  127 on each kernel and is rowed for the fourth arm. core.fk's comment pointed to a receipt path that
  does not exist; it now says what the reading is.
- **Three bands gain their fourth arm.** float-parity, bml-float-literal and eq-shape read 255, 2047
  and 524287 on all four kernels, the first two whole on TS only since this piece, and are rowed in
  fourth-arm-bands.txt beside str-to-int-reading.
- **letterkept is row 1478** (5e2435d97).

Witnessed at d7a32ed5c: validate.sh on 30 bands, go, rust and typescript agreeing and exiting 0 on
each, among them str-to-int-reading 127, float-parity 255, bml-float-literal 2047 and eq-shape 524287
with their fourth arm, float-compare 4095, format-arith 2047, int-literal-width 9, sha256-hmac-stream
8388607, pbkdf2-sha256 3, uuid 15, room-cipher 63 and pg-floor 4095; the three integer probes
identical on Go, Rust and TS; freshness 31, the drift run 8191 of 8191, porcelain 0 before and after.

## Still open, measured

- **fkwu's integers wrap at 2^63**: `(mul 3037000499 3037000499)` reads -5928526807 on fkwu and
  9223372030926249001 on Go, Rust and TS, and `(add 9223372036854775807 1)` reads 0 on fkwu and
  -9223372036854775808 on the others.
- **max, min and pow have no home on fkwu**: core.fk defines none of them and fkwu carries no native,
  so a cell calling them resolves on Go, Rust and TS and reads nothing on fkwu.
- **int_to_str on a float** answers "" on fkwu and the float's text on the siblings (receipt 34).
- **TS reads no pg frame** (textsieve) and carries no door to its home directory.
- **The committed form-cli bootstrap trails its sources**, its stamp from 2026-08-25.
- **The five vk live lanes** wait on the Vulkan door through host-exec.
- The open items of receipts 12 to 34 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was which kernel was wrong. Three comments above band, bor and bxor said the same thing.
Two kernels' code had moved past it and agreed with each other; the third did what it said. The
sibling that disagreed was the faithful one, and every check that compares kernels read it as the
wound. A comment that two kernels outgrew became a specification for the third.

The discomfort was str_to_int. It split two against two, with the canonical kernel on the side of the
looser reading, and core.fk's own comment had already filed the split as a known spread, pointing to a
receipt that is not there. The piece was about 2^53; it would have been easy to leave the spread where
it was filed. What turned it was reading the reason written beside fkwu's loop: shell output ends in
a newline, and a reader that takes only a bare integer turns the body's own `grep -c`, `"248\n"`,
into 0. The loose reading was the one with a reason, and it now has one native in three kernels and
a band that asks all four.

Frontier word, row 1478: **letterkept**, a sibling that keeps the letter of a shared comment while
the others keep the code; it diverges by being faithful.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
