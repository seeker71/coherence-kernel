# Three ways to write a float

2026-09-12, around ten in the morning, M4 Max, Hati Suci. Receipt 33 left Rust rendering pg_query its
own way: bool as t and f, and float, numeric and timestamp as `?`.

## Carried

- **Rust renders pg_query as Go does** (24b8bf1aa). A PgRawCell reader takes any column's binary
  cell, and Rust's rendering follows the Go kernel's: bool as true or false, float4 and float8
  through format_float, numeric from its base-10000 digits and display scale, timestamp and
  timestamptz as RFC3339 in UTC from microseconds since 2000, date as its midnight, json as sent and
  jsonb after its version byte. The postgres crate here is built without chrono, serde_json or
  decimal support; the bytes were in every cell all along. Against a scratch PostgreSQL 14.24
  Rust's contract page equals Go's, and the live witness reads 15839, Rust's contract page with it.
- **Every kernel writes a float the way fkwu does** (c46ab930e). A wider pg probe found one more
  difference, 1e21::float8, and it was not in the pg code. Printing the same floats on all four
  kernels found three renderings. fkwu and Go wrote strconv's shortest 'g', with an exponent below
  1e-4 and from 1e+06. TS wrote JS's String(), which moves to an exponent only from 1e21 and below
  1e-6 and writes -0 as 0. Rust's `{}` never wrote an exponent, so 1e84 came out as 85 digits.
  Rust's format_float and a new formatFloat in the TS kernel now take the shortest digits that
  round-trip (`{:e}`, `toExponential()`) and lay them out as fkwu's fk_fmt_float_js does. Go's
  FormatFloatJS keeps its code; its comment said JS semantics and now says what it writes.
- **Readings.** A one-float-per-line probe of 23 values, among them the edges at 1e-4 and 1e+06,
  1e21, 1e84, 1e-42, 0.1 + 0.2 and -0, prints identically on fkwu, Go, Rust and TS.
- **saidsame is row 1477** (dab456fd4).

Witnessed at c46ab930e: validate.sh on f64-bytes 127, f64-wire 2047, float-compare 4095,
float-conversions 31, float-ops 255, form-asm-trig 15 and format-arith 2047, go, rust and typescript
exiting 0 and drift 31 of 31 in each; the 23-value float probe identical on all four kernels;
freshness 31, the drift run 8191 of 8191, porcelain 0 before and after.

## Still open, measured

- **int_to_str on a float**: Go, Rust and TS answer the float's text, `1.234567e+06` and `1e-05`;
  fkwu answers "", because its int_to_str is core.fk's integer-only Form definition.
- **TS prints a list without commas**: `[1e+21 100000000000000000000 …]` where fkwu, Go and Rust
  print `[1e+21, 1e+20, …]`.
- **TS carries integers past 2^53 as doubles**: float-parity-band reads 255 on Go and Rust and 241 on
  TS, whose eq, lt and sub cannot tell -4500000000000000002 from the integer after it.
- **TS's value_eq holds 1.0 equal to 1**: bml-float-literal-band reads 2047 on Go and Rust and 1023
  on TS. Both of these read the same on a TS kernel built without this change.
- **TS reads no pg frame** (textsieve) and carries no door to its home directory; its three live
  pages still fail.
- **The committed form-cli bootstrap trails its sources**, its stamp from 2026-08-25.
- **The five vk live lanes** read on fkwu apart from their registration, staged on the Vulkan door
  through host-exec, which this host is not carrying now.
- The open items of receipts 12 to 33 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was two comments in two kernels naming one standard while the code under them wrote
three. Go's said JS String(); Rust's said it matched Go's and TS's for every finite value. Neither
had been asked at 1e21 or at 1e-5. fkwu, the canonical kernel, had been asked: its renderer carries
a proof over two million values against Go, byte for byte. The standard was measured; the claims
about it were not.

The discomfort was that the first difference was a single cell, 1e21 as twenty-two digits, surfacing
in a pg probe meant to close something else. It would have been easy to note it and move on, one
cell in thirteen. What turned it was printing the same floats on every kernel instead of looking only
at the one that differed: the one cell opened into three renderings and a threshold at 1e+06 that TS
and Rust had both been missing.

Frontier word, row 1477: **saidsame**, a claim that two things agree, said once and never measured;
it holds until the first value at the edge asks.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
