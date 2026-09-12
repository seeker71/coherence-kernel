# Where bytes are bytes

2026-09-12, around eleven in the morning, M4 Max, Hati Suci. Receipt 35 left TS reading no pg frame
and carrying no door to its home directory, so three of the live witness's twelve pages failed there.

## Carried

- **TS carries pg and config as natives** (a83384e5a). The pg floor builds its frames as strings of
  bytes, which fkwu's strings are. On TS a string's length counts its UTF-8 bytes, while
  byte_to_str(200) makes one character that encodes as two, so a frame holding a byte from 128 up
  changes size there before any socket door sees it. TS now speaks the wire protocol in its host's pg
  carrier, a worker holding Buffers. It asks for UTC and UTF8 at startup, answers trust, cleartext, md5
  and SCRAM-SHA-256, and runs SQL by the simple Query protocol, or by Parse/Bind/Execute when
  parameters travel. pg_connect, pg_exec, pg_query, pg_query_rows, pg_ping, pg_close and pg_last_error
  answer as Go's natives do, each cell read as Go's database/sql reads it. config_database_url and
  config_value_or merge the kernel config's layers as Go's and Rust's do, reading $HOME through a new
  homeDirectory door. The pg floor stands in on fkwu alone.
- **Readings.** The live witness reads 32767, where it read 15839: the contract, SCRAM and config pages
  on all four kernels, trust and SCRAM-SHA-256 among them. On a scratch Postgres, a probe of
  parameters, typed rows, pg_ping and a failing cast prints alike on Go and TS, line for line, and
  seven float cells print alike on Go, Rust and TS.
- **The doors say so.** pg-floor.bml, config-floor.bml, pg-live-witness.bml and storage-port-db.fk
  named TS among the kernels the floor serves; they name fkwu. Receipt 35 rewraps one long line.
- **nearcause is row 1479** (83a041801).

Witnessed at a83384e5a: the live witness 32767 on the carrier's own build; validate.sh on pg-floor
4095, config-floor 63 and str-to-int-reading 127, go, rust and typescript agreeing and exiting 0; on a
scratch Postgres the float probe alike on Go, Rust and TS and the parameter probe alike on Go and TS;
freshness 31, the drift run 8191 of 8191, porcelain 0 before and after.

## Still open, measured

- **Rust's pg natives fail wherever a parameter travels**: with 41, "x", 2.5 and true bound to
  `$1::int + 1, $2::text, $3::float8, $4::bool`, Go and TS answer `42 x 2.5 true`, and Rust answers
  ERR. Its pg_exec with parameters answers -1 and its pg_query_rows [], and where Go and TS read the
  server's `invalid input syntax for type integer: "nope"`, Rust's pg_last_error reads
  `error serializing parameter 0`. api.bml hands pg_exec and pg_query_rows their parameters this way.
- **The pg floor writes a float as the server's text**: `SELECT 123456789::float8, 1.1::float4,
  2.5e6::float8` reads `123456789`, `1.1` and `2500000` through the floor on fkwu, and
  `1.23456789e+08`, `1.100000023841858` and `2.5e+06` on Go, Rust and TS; 1e-07, 1e+21, 0.1 and 100000
  read alike on all four. The floor's comment says each cell renders as Go renders it. fkwu carries no
  door that writes a float as text the way its print does, the gap that also leaves int_to_str on a
  float answering "".
- **fkwu's integers wrap at 2^63**, and **max, min and pow have no home on fkwu** (receipt 35).
- **Rust's and TS's socket_recv decode bytes as UTF-8** (textsieve, row 1474), which any Form protocol
  over sockets still meets on those kernels.
- **The committed form-cli bootstrap trails its sources**, its stamp from 2026-08-25.
- **The five vk live lanes** wait on the Vulkan door through host-exec.
- The open items of receipts 12 to 35 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was that the named cause was true and still not the one that decided. Row 1474 named
textsieve: TS decodes the bytes its sockets receive as UTF-8. Reading TS's string doors showed that
byte-faithful sockets would not have carried the floor there either. byte_to_str(200) makes a character
its str_len counts as two bytes, so the floor's frames change size before a socket ever sees them.
Healing the socket door alone would have moved the failure one step inward.

The discomfort was choosing the contained fix over the one that sounds more general. Teaching TS's
strings to hold raw bytes would let one Form floor serve fkwu and TS alike. Measuring what that touches
showed it is not a TS change: Go and Rust share the code-point byte_to_str, and changing TS alone would
break the three-way agreement every band reads. The discomfort was writing a third client for a
protocol the body already speaks in Form. What turned it was the pattern already standing: Go and Rust
carry pg natively, a present native answers its name, and the floor exists for the kernel with no host
to lean on. TS has a host whose Buffers are bytes. The byte model stays one named family, byte_to_str
from 128 on the three siblings, rather than a patch on one of them.

Frontier word, row 1479: **nearcause**, a true cause, named correctly, that stands in front of the one
that decides; removing it alone would not heal.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
