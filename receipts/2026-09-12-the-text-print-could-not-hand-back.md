# The text print could not hand back

2026-09-12, around half past eleven in the morning, M4 Max, Hati Suci. Receipt 36 measured the pg
floor writing float cells as the server's text on fkwu, and receipt 34 int_to_str answering "" for a
float there. Both asked for one door fkwu did not carry.

## Carried

- **value_str on every kernel** (35bfb1583). fkwu writes a float one way, fk_fmt_float_js, the
  rendering Go's FormatFloatJS matches byte for byte, but only print reached it: no door handed that
  text back to Form. value_str is now mode 25 of fkwu's leaf door and writes a word as Go's
  formValueString writes it: a string as its bytes, an integer in decimal, a float as print writes it,
  a bool as true or false, a list as `[a, b]` with its items likewise, and nothing as "". TS gains
  value_str as a native. Go answered "" for a null and Rust "null"; Rust's now answers "", as Go's does.
- **int_to_str writes through value_str on fkwu.** core.fk's int_to_str was a digit loop that knew only
  integers, so a float came back as "". It now answers nothing as "nothing" and anything else as
  value_str writes it, and the two digit helpers only it used are gone. Its 6372 call sites read the
  same text for every integer.
- **The pg floor writes float cells as Go does.** float8 reads through str_to_float into value_str, and
  float4 through make_float32 and float_value first, so 123456789, 1.1::float4 and 2500000 read
  1.23456789e+08, 1.100000023841858 and 2.5e+06 on fkwu as on Go, Rust and TS. The live witness's
  contract page now asks for both floats, and pg-floor-band gains a claim for them.
- **Readings.** A value_str probe of floats, integers past 2^53, strings, nested lists, null,
  int_to_str and a widened float4 prints alike on all four kernels. value-str-band reads 127 four-way
  and is rowed for the fourth arm; pg-floor-band reads 8191.
- **printbound is row 1481** (b5d942671). Receipt 37 rewraps one long line.

Witnessed at 35bfb1583: the value_str probe alike on fkwu, Go, Rust and TS; validate.sh on value-str
127, pg-floor 8191, config-floor 63, str-to-int-reading 127, format-arith 2047, uuid 15, http-serve
1023 and float-parity 255, go, rust and typescript agreeing and exiting 0 with the fourth arm; the
live witness 32767 with its two new float cells; freshness 31, the drift run 8191 of 8191, porcelain 0
before and after.

## Still open, measured

- **Rust writes "?" for types outside its set**: uuid, interval and arrays (receipt 37).
- **fkwu reads `true` as the integer 1**: `print true` writes 1 on fkwu, and value_str answers true on
  Go and Rust.
- **fkwu's integers wrap at 2^63**, and **max, min and pow have no home on fkwu** (receipt 35).
- **Rust's and TS's socket_recv decode bytes as UTF-8** (textsieve, row 1474).
- **The committed form-cli bootstrap trails its sources**, its stamp from 2026-08-25.
- **The five vk live lanes** wait on the Vulkan door through host-exec.
- The open items of receipts 12 to 37 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was that the canonical kernel owned the one float rendering, the one the siblings were
brought to match in receipt 34, and could not give that text to its own Form code. print wrote it to
stdout and nothing returned it as a string, so fkwu's library wrote a float as "" in int_to_str and
the pg floor fell back on the server's text. The rendering was never missing; the way back was.

The discomfort was reach. int_to_str has 6372 call sites, and changing its body on the canonical
kernel touches every one of them. The first build then crashed every fkwu run that reached value_str,
the drift run's BML lowering among them. The buffer's capacity was declared `static long long
fk_vs_cap;`, the name the value stack already gives its own capacity, and C joined the two
declarations into one variable: the buffer read the stack's capacity, never grew, and wrote through a
null pointer. A debug build under lldb named the write in one run, and a count of 15 uses of the name
before mine named the cause. The buffer's names carry their own prefix now. The narrower change, a
float branch beside the digit loop, needed a type test that allocates a string on every call, while
the digit loop and value_str write the same text for every integer; so the wider change stayed the
plainer one, and the bands and the drift run read it before it landed.

Frontier word, row 1481: **printbound**, text a kernel can print but not hold.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
