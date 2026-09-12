# A floor where no native stands

2026-09-12, around nine in the morning, M4 Max, Hati Suci. Receipt 31 left pg-wire reading a row on
fkwu and Go, and validate.sh reading two mesh bands failed on the pg names fkwu does not carry.

## Carried

- **The pg doors in Form, where no native carries them** (9ce6a26c).
  form-stdlib/bml/pg-floor.bml defines pg_connect, pg_exec, pg_query, pg_ping, pg_close and
  pg_last_error. A present native answers its name on every kernel, so Go's pgx and Rust's postgres
  crate keep answering, and the floor stands in on fkwu and TS. It speaks the Postgres v3 wire
  protocol over the socket doors every kernel carries: a startup asking for UTC and UTF8; trust,
  password and SCRAM-SHA-256 authentication, built on the body's own sha256, hmac-sha256,
  pbkdf2-sha256 and base64; the simple Query protocol, read to ReadyForQuery. Cells render as Go
  renders them, bool as true or false, NULL empty, timestamps and dates as RFC3339 in UTC. The last
  error lives in a section-level record, the one form of unit state all four kernels honour, and
  random_bytes has a stand-in over /dev/urandom for fkwu, which carries no such native.
- **Every unit that calls a pg door preludes the floor**: fifteen of them, the carriers, the mutation
  runners and the live integration cells. pg-wire.fk, whose six definitions are a subset of the
  floor, is gone, and so is the witness that composed it.
- **Readings.** Against a scratch PostgreSQL 14.24, the floor on fkwu answers the contract page that
  Go's native answers, line for line: `true false 42 7 x  1.5 0.1 12.30 2026-09-12T00:00:00Z`, the
  row counts 0, 3 and 2, the rows, and the server's own error text after -1 and ERR. It connects
  through SCRAM-SHA-256 in under 4 s, PBKDF2 over 4096 iterations taking 1.9 s in Form on fkwu, and
  a wrong password answers -1 with `password authentication failed for user "form"`. RFC 7677's
  exchange comes out exact on fkwu, Go and Rust.
- **Bands.** validate.sh reads five of the six rowed pg-unit bands whole, where fkwu's leg exited 1
  on the pg names before: application-graph-node-port 1111, native-idea-valuation-audit-ledger
  111111, native-mutation-public-gate 11111111, native-mutation-route-side-effects 11111 and
  mesh-sensings-store-pg 4095. pg-floor-band reads 4095 on all four kernels and is rowed.
- **The live witness** is observe/pg-live-witness-run.fk: two scratch clusters of its own, one
  trusting and one asking SCRAM, a contract cell and a SCRAM cell on each built kernel, each page
  held to Go's. It reads 1487 of 2047: every contract and SCRAM page but Rust's contract page and
  both of TS's.
- **The notes** in mesh-sensings-store-pg.fk, storage-port-db.fk and str-byte-at.fk name the floor
  where they named pg-wire.
- **gapstand is row 1475** (978d31c4e).

Witnessed at 9ce6a26c: validate.sh on pg-floor-band 4095, application-graph-node-port 1111,
native-idea-valuation-audit-ledger 111111, native-mutation-public-gate 11111111,
native-mutation-route-side-effects 11111, mesh-sensings-store-pg 4095 and str-byte-at 1023, go, rust
and typescript exiting 0 and drift 31 of 31 in each; mesh-sensings-route failed on its two
config_database_url calls; the live witness 1487 on the floor this commit carries; freshness 31, the
drift run 8191 of 8191, porcelain 0 before and after.

## Still open, measured

- **Rust renders pg_query its own way**: bool as t and f, float, numeric and timestamptz as `?`,
  jsonb as empty, where Go and the floor read `true false … 1.5 0.1 12.30 2026-09-12T00:00:00Z`.
  Rust's pg_cell_to_string has arms for text, the ints and bool only, and its query binds
  parameters, so the heal decodes each type's binary form rather than moving to the text protocol.
- **TS reads no frame.** Its socket_recv decodes bytes as UTF-8 (textsieve, row 1474). In the
  witness its contract cell ends at str_len on a null the garbled frames left in its buffer, and its
  SCRAM cell printed `refused` and then ran until the 300-second alarm stopped it. Socket doors that
  carry bytes as bytes on Rust and TS are the heal.
- **mesh-sensings-route still reads failed.** Its handlers call config_database_url, which Go and
  Rust carry as a door over the kernel config, api/config/api.json with
  ~/.coherence-network/config.json laid over it. A Form reading of those layers is the next door.
- api.bml calls the pg doors and runs on Go's natives; it does not load the floor.
- **The committed form-cli bootstrap trails its sources.** Its stamp was last committed on
  2026-08-25 (b2007049); the 185 sources it bundles hash to 8103dbf088ebc0b4 now against the
  committed 4193d5b167904e36, moved by many commits since, str-byte-at.fk's notes among them. A
  regen through form/scripts/regen_form_cli_bootstrap.sh republishes it.
- **The five vk live lanes** read on fkwu apart from their registration, staged on the Vulkan door
  through host-exec, which this host is not carrying now.
- The open items of receipts 12 to 31 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was which side told the truth. A floor built on four socket doors and the body's own
hashes answered the live contract byte for byte as Go's library does, and Rust's library, a real
driver compiled in, answered `?` for a float, a numeric and a timestamp, and t for true. The native
was the one that lost data. A present native answers its name, so a native that answers wrong is
answered wrong everywhere it stands.

The discomfort was a probe that printed nothing on fkwu while three kernels printed `boom again`:
fkwu's defn cannot see a let from the enclosing do, where Go, Rust and TS can, and my design for the
last error needed exactly that. What turned it was asking fkwu where state can live, rather than
bending the floor around the miss. A let at the top of the unit, which fkwu holds once and every
defn shares, reads `boom again` on all four, and BML's section-level let lowers to the same. The
floor keeps its last error there.

Frontier word, row 1475: **gapstand**, a definition that stands only in the gap a native leaves:
where the native is present it answers its own name, and where it is absent the gapstand answers.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
