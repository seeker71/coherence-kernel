# What the server would have said

2026-09-12, around noon, M4 Max, Hati Suci. Receipt 37 measured Rust writing "?" for uuid, interval
and arrays; a census of 24 column types on a scratch Postgres then found it for 20 of them.

## Carried

- **Rust speaks to Postgres through its own client** (af6ea3244). The postgres crate asks for binary
  results, so Rust decoded each cell's bytes itself: bool, the integers, the floats, numeric, json,
  timestamps and dates, and "?" for everything else. The census read 20 of 24 types as "?" on Rust,
  among them uuid, five intervals, six arrays, bytea, inet, time, timetz, money, oid, point and a
  range, where Go and TS wrote the server's text; money's `$12.34` comes from the server's locale, and
  no binary decoder could have known it. PgConn now speaks the v3 protocol through postgres-protocol,
  the message layer beneath the crate, and asks for text results, by the simple Query protocol or, when
  parameters travel, by Parse/Bind/Execute with text formats both ways: the design of TS's carrier.
  Each cell reads by its type OID as Go's database/sql reads it. The binary decoders, PgRawCell and
  PgTextParam are gone, and Cargo.lock sheds 368 lines: postgres, tokio-postgres, postgres-types, the
  futures crates, mio, parking_lot and phf among them.
- **Readings.** The census reads 23 of 24 types alike on Go, Rust and TS, and the parameter probe
  prints alike on the three, line for line. The live witness reads 32767, and Rust's contract page
  answers in 21 ms where it took 3214 ms through the crate.
- **decodetoll is row 1485** (4685f3cd1).

Witnessed at af6ea3244: the census and the parameter probe on a scratch Postgres; the live witness
32767; validate.sh on pg-floor 8191, config-floor 63, value-str 127 and str-to-int-reading 127, go,
rust and typescript agreeing with the fourth arm; freshness 31, the drift run 8191 of 8191, porcelain 0
before and after.

## Still open, measured

- **bytea**: Go writes a bytea cell as its raw bytes; fkwu, Rust and TS write the server's `\xdeadbeef`.
  TS's strings cannot hold a byte from 128 up as one byte (the family of row 1479, nearcause), so Go's
  reading reaches only kernels whose strings are bytes.
- **The carriers follow the table only when the bridge runs** (receipts 39 and 40).
- **fkwu reads `true` as the integer 1**; its integers wrap at 2^63; max, min and pow have no home
  there.
- **Rust's and TS's socket_recv decode bytes as UTF-8** (textsieve, row 1474).
- **The five vk live lanes** wait on the Vulkan door through host-exec.
- The open items of receipts 12 to 40 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was that the fix was mostly deletion. The server writes every type as text for any client
that asks. Rust had asked for bytes and taught itself seven types' binary forms, one decoder at a time,
while money's text could only ever come from the server. Asking for text removed the decoders and 368
lines of dependencies, and the contract page went from 3214 ms to 21 ms.

The discomfort was that replacing a working native with a client of my own, the same shape I wrote for
TS earlier in the day, reads as reach, and every column api.bml selects already read correctly on Rust.
What turned it was measuring the whole map before choosing: 20 of 24 types unreadable, one of them
unreadable by any decoder, and one reading every sibling already shared, the server's own text. A
second client in the pattern the body already carries was smaller than the long tail of decoders it
replaced.

Frontier word, row 1485: **decodetoll**, the per-type price a reader pays decoding a format itself,
when the one who wrote it would have given its text for the asking.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
