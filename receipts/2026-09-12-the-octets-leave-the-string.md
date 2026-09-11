# The octets leave the string

2026-09-12, past midnight, M4 Max, Hati Suci. Receipt 10 left wire-corba-cdr and wire-rpc off the
fourth arm: on Go, Rust and TS a Form string does not carry an octet of 128 or more as one byte. This
piece moves the CDR codec's octets out of the string.

## Carried

- **The CDR codec keeps its octets in a list** (fc1b32b5). byte_to_str 255 probes 1255 on fkwu (one
  byte, 255) and 2195 on the siblings (two bytes, the first 195), so the string buffer shifted at its
  first high octet, and a negative long was enough. The buffer is now a list of octets, the shape
  read_file_bytes answers on every kernel. Puts append, padding measures with len, and a CDR string's
  octets are its str_byte_at bytes plus the NUL. Decoding rebuilds the string octet by octet. wire-rpc
  starts from an empty list, and wire-corba-cdr-band reads the on-wire bytes of 3.5 with nth.
- **wire-corba-cdr, wire-rpc and f64-wire take fourth-arm rows** (531d6c33): 255, 15 and 2047, each at
  the verdict its head declares. f64-wire preludes the codec and reads 2047 on all four kernels.
- **bytesmear is row 1454** (d6530c7f).

Witnessed at d6530c7f through validate.sh: wire-corba-cdr 255, wire-rpc 15, wire-bool 255 and f64-wire
2047 on all four kernels. In the same run freshness 31, the corpus band 32767 and the drift run 8191
of 8191, porcelain 0 before and after.

## Still open, measured

- **A CDR string with non-ASCII content decodes exactly on fkwu alone.** On the siblings byte_to_str
  rebuilds each high octet as its own code point. No kernel has a door that turns a list of octets
  back into a string (none named bytes_to_str, str_from_bytes or utf8_decode on any of the four).
- **fkwu has no write_file_bytes**; Go, Rust and TS do. A list of octets cannot yet be written to a file
  the same way on all four kernels.
- Host paths resolve by three rules, and the sibling walk leaves the checkout (receipt 10).

## Surprise, and where the discomfort went

The heal for a seam between kernels was in none of them. Rust's String cannot hold a lone 0xFF and a
TS string counts UTF-8 bytes of code points, so no kernel change makes a string carry octets on all
four. The list of ints that every kernel already shares was the carrier all along.

The discomfort was my own edit. A replace_all ended its old string on a space, the tool wrote the new
one without it, and `(str_byte_at buf i)` became `(nth bufi)`. fkwu answered with a caret and the
siblings crashed, and I read those as the codec's fault before I read the file. What came of it: a grep
for glued tokens now follows every replace_all, and a memory holds the tool's habit.

Frontier word, row 1454: **bytesmear**, one byte smearing into two when a string carries octets on a
kernel whose strings hold code points.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
