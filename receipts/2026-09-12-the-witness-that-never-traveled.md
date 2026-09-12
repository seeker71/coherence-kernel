# The witness that never traveled

2026-09-12, around half past eight in the morning, M4 Max, Hati Suci. Receipt 30 carried six rowed
bands reaching pg_exec, pg_query and pg_connect, doors fkwu does not carry.

## Carried

- **pg-wire has a live witness** (d5ebcfaf). form-stdlib/pg-wire.fk speaks the Postgres v3 wire
  protocol in Form over the socket doors every kernel carries. Since 2026-07-22 its header had cited
  its one live claim, that fkwu reads a row back from Postgres, to a script never committed here, and
  two sibling cells carried the same citation. observe/pg-wire-live-witness-run.fk makes a scratch
  cluster of its own under .hearth/ (initdb, trust auth, a port from its pid, no unix socket), writes
  pg-wire's composition as a cell, runs it as a child on each built kernel, stops the server and
  removes only the data. It needs initdb and pg_ctl on PATH.
- **Readings.** Against PostgreSQL 14.24 the cluster took 360 ms and the server 121 ms; fkwu and Go
  read `witness:42`; Rust and TS read the row with the frames around it; the server stopped. The
  verdict is 79 of 127.
- **Where Rust and TS part.** A byte dump of the query response on each kernel: fkwu and Go receive
  75 bytes, Rust and TS 87. RowDescription's two -1 fields are six 0xFF bytes. Rust's socket_recv
  decodes what it receives with `String::from_utf8_lossy` and TS's kernel decodes it as UTF-8, so
  each 0xFF arrives as U+FFFD, three bytes, and every frame after it is read twelve bytes off. Their
  strings are Unicode text and cannot hold a lone 0xFF; bytes cross those two kernels as int lists,
  the way read_file_bytes and write_file_bytes already carry them.
- **The notes** in pg-wire.fk, mesh-sensings-store-pg.fk and storage-port-db.fk say what is: Go
  (pgx) and Rust (the postgres crate) carry the pg natives, fkwu and TS none, and pg-wire reads a live
  row on fkwu and Go. They had called the natives libpq, Rust-only and three-kernel.
- **textsieve is row 1474** (8407520a).

Witnessed at d5ebcfaf: the live witness 79 again after the commit; freshness 31, the drift run 8191
of 8191, porcelain 0 before and after. validate.sh reads mesh-sensings-store-pg-band and
mesh-sensings-route-band ✗: go, rust, typescript and fkwu each read 4095 and 63, and fkwu's leg exits
1 on 3 and 5 diagnostics, the unresolved pg_exec, pg_query and pg_connect in carrier code this commit
did not change.

## Still open, measured

- **Socket doors that carry bytes as bytes on Rust and TS.** socket_recv and socket_send hand over
  strings, and those two kernels' strings cannot hold bytes that are not UTF-8, so pg-wire reads a
  row on fkwu and Go only. fkwu's tags run to 255, so a new door there reclaims one.
- **The six rowed bands** still load units whose carriers call pg_exec, pg_query and pg_connect.
  validate.sh reads two of them ✗ for it: every kernel on the registered verdict, and fkwu's leg
  exiting 1 on the names it does not carry.
- **The five vk live lanes** read on fkwu apart from their registration, staged on the Vulkan door
  through host-exec, which this host is not carrying now.
- The open items of receipts 12 to 30 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was that the claim was true. Three files had carried it for seven weeks as cited to a
script no one here could run, and the first live run here read witness:42 on fkwu, and on Go, which
the claim never named. What the claim never said was Rust and TS, where the same recipe reads the
frames around the row.

The discomfort was the witness's first run, which put nothing on the screen: rc 0 in about a second
and no line after it. It read like a cell that had never started. The raw file held all of it, the
cluster, the four kernels, the stop and the verdict. grep had taken it for binary, because the Rust
and TS lines carry 52 NUL bytes from the frames they misread, and printed nothing. What turned it was
reading the file rather than the filter, and then the bytes rather than the answer: the dump set 75
against 87 and 255 against 239, 191, 189, and the cause was one decode in each of two kernels.

Frontier word, row 1474: **textsieve**, a door that passes bytes through a text decoder, so the bytes
that were not text come out replaced and everything measured after them moves.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
