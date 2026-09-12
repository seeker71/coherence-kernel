# The server declares its parameters

2026-09-12, around ten past eleven in the morning, M4 Max, Hati Suci. Receipt 36 measured Rust's pg
natives failing wherever a parameter travels: ERR, -1 and [], with pg_last_error reading
`error serializing parameter 0`.

## Carried

- **Rust sends every SQL parameter as text** (ce3c7348c). Rust bound each parameter by a type it
  guessed: it read casts such as `$1::int4` off the SQL text and boxed a Rust value to match, falling
  back to the Form value's own kind. The guess missed `$1::int`, and where it hit, its int4 arm still
  handed over an i64, which the postgres crate binds only to an int8 parameter. The server declares
  every parameter's type once the statement is prepared. PgTextParam now sends each parameter in the
  text format, as TS's carrier sends them and as Go's pgx sends what it holds no binary encoding for,
  and the server reads it by the declared type. sql_param_cast and the three value_as_* readers that
  served the guess are gone. bytes joins the kernel's dependencies for the buffer ToSql writes into;
  the lock already held it through tokio-postgres.
- **Rust's typed rows read NULL as null** (ce3c7348c). pg_query_rows answered "" for a NULL text,
  numeric, json or timestamp cell, where Go's dbCellToForm and TS answer null. A NULL of any type now
  reads null before the per-type readers run.
- **Readings.** On a scratch Postgres, parameters bound to `$1::int + 1, $2::text, $3::float8, $4::bool`,
  exec counts with parameters, typed rows holding NULLs of five types, pg_ping, a failing cast and a
  syntax error print alike on Go, Rust and TS, line for line, the server's own
  `invalid input syntax for type integer: "nope"` among them.
- **sniffcast is row 1480** (043c7bec8).

Witnessed at ce3c7348c: the parameter probe alike on Go, Rust and TS but for the one line below; the
live witness 32767; validate.sh on pg-floor 4095 and config-floor 63, go, rust and typescript agreeing
and exiting 0; freshness 31, the drift run 8191 of 8191, porcelain 0 before and after.

## Still open, measured

- **Rust writes "?" for types outside its set**: `SELECT '00000000-0000-0000-0000-000000000001'::uuid,
  '1 day'::interval, ARRAY[1,2]` reads `? ? ?` on Rust, and the server's
  `00000000-0000-0000-0000-000000000001`, `1 day` and `{1,2}` on Go and TS. The postgres crate reads
  results in binary, and Rust decodes the binary of bool, the integers, the floats, numeric, json,
  timestamps and dates.
- **The pg floor writes a float as the server's text** on fkwu (receipt 36).
- **fkwu's integers wrap at 2^63**, and **max, min and pow have no home on fkwu** (receipt 35).
- **Rust's and TS's socket_recv decode bytes as UTF-8** (textsieve, row 1474).
- **The committed form-cli bootstrap trails its sources**, its stamp from 2026-08-25.
- **The five vk live lanes** wait on the Vulkan door through host-exec.
- The open items of receipts 12 to 36 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was that the guess failed even where it guessed right. `$1::int4` matched the sniff and
chose the int4 arm, and that arm handed over an i64, so every int4 parameter failed, sniffed or not. A
reader of the SQL text stood in for an answer the server gives exactly, and no query had asked it the
question that would have shown the gap.

The discomfort was a probe written to confirm TS that turned up a wound in Rust. The parameter probe
was meant to witness the new carrier, and Rust's column of ERR, -1 and [] arrived beside it. Receipt
36's first draft said the three agreed; it was wrong before a word of it landed. The gold was writing
what the probe showed instead of what I expected, and then carrying the Rust wound in the next piece
rather than filing it. The probe that found it now reads alike on three kernels.

Frontier word, row 1480: **sniffcast**, a type guessed from the text of a request, where the one who
answers would declare it if asked.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
