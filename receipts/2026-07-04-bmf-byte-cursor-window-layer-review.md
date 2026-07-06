# 2026-07-04 -- bmf-byte-cursor window layer review

## Why This Layer Exists

After `file-byte-window` landed, the next red result was the existing
`form/form-stdlib/tests/bmf-byte-cursor-source-band.fk`. It still called the
unavailable raw byte-at path directly and returned `0` on current `fkwu`.

The repair is a BMF file cursor over bounded `fbw` windows:

```text
file-byte-window -> bmf-byte-cursor -> scannerless file cursor
```

This repairs the source byte-cursor witness without growing C and without
claiming every `bmf-core` file surface now streams through this cursor.

## Reviewer Pre-Review

Grok returned `PASS` with required changes:

- make window size constructor-bounded and testable;
- fix the prefix and cursor tag before coding;
- define EOF, window indexing, refresh, and immutability precisely;
- keep range rows source-compiler-free;
- rewrite the band without section syntax or raw byte-at;
- state non-goals in the receipt/map.

Claude returned `PASS` with required changes:

- `bbc-byte` must never reload; reload belongs to refresh/advance;
- EOF refresh must not loop, and advance at EOF must be idempotent;
- constructor window size must be clamped to `[1, fbw-max-window]`;
- binary fixture writing should reuse the already-witnessed Form FS byte path;
- header and band must pin line/column conventions.

All required reviewer changes were implemented.

## Implementation Stall Investigation

The first focused run of the rewritten band produced no output after 30
seconds, then remained silent for another minute. It was interrupted with exit
code `130` and treated as a real stall, not ignored. Load-only probes against
`bmf-byte-cursor.fk` also stalled, which isolated the fault to the new module
shape rather than the band logic.

A delimiter count found one extra close paren in `bbc-advance`. That paren
closed the module-level `do` before the range functions and left the final file
close unmatched. After removing it, load-only, constructor-byte, and advance
probes returned immediately.

The next focused band run returned `3145727`, missing only the binary/EOF bit.
Clause-level probing showed byte/range checks were sound but EOF was no longer
at offset `7` after repeated runs. Root cause: `fs-remove-tree` does not clear a
non-empty temp directory on this floor, so the fixture file survived and each
setup appended another seven bytes. The band now removes its fixture file before
and after the directory operation. The `file-byte-window` band received the same
cleanup hardening because it also used append-backed fixtures.

## Implemented Surface

Files:

- `form/form-stdlib/bmf-byte-cursor.fk`
- `form/form-stdlib/tests/bmf-byte-cursor-source-band.fk`

Prefix: `bbc-`.

Cursor row:

```text
("bmf-byte-cursor" path offset line col window-start window-size window eof)
```

Primary functions:

- `bbc-cursor(path)`
- `bbc-cursor-with-window(path, window-size)`
- `bbc-cursor-at(path, offset, line, col, window-size)`
- `bbc-refresh(cursor)`
- `bbc-refresh-if-needed(cursor)`
- `bbc-byte(cursor)`
- `bbc-advance(cursor)`
- `bbc-range(start, end)`

`bbc-byte` only decodes the carried window; it never reloads. `bbc-advance`
refreshes at window boundaries and carries an EOF flag so repeated EOF advance
is idempotent. Window size defaults to `256` and is clamped to `1..4096`.

Line/column convention:

- initial coordinates are line `1`, column `0`;
- byte `10` advances to the next line and resets column to `0`;
- byte `13` is an ordinary column-advancing byte.

## Band

Command:

```sh
./fkwu --src <(cat form/form-stdlib/core.fk \
  form/form-stdlib/str-byte-at.fk \
  form/form-stdlib/form-fs.fk \
  form/form-stdlib/file-byte-window.fk \
  form/form-stdlib/bmf-byte-cursor.fk \
  form/form-stdlib/tests/bmf-byte-cursor-source-band.fk)
```

Expected:

```text
4194303
```

The band proves:

- manifest boundaries;
- constructor window loading;
- immutable advance;
- CR/LF line and column semantics;
- deliberate boundary refresh with a two-byte test window;
- NUL/ASCII bytes `0`, `127`, and `90`;
- EOF byte sentinel and idempotent EOF advance;
- source span row without source-compiler dependency;
- static scan that the module does not use the raw byte-at path, whole-file
  byte reads/writes, raw file reads, file-size probes, source-compiler, or line
  grammar.

## Post-Implementation Verification

- Focused BMF byte-cursor band returned `4194303` twice in a row after fixture
  cleanup hardening.
- Focused file-byte-window band returned `2147483647` twice in a row after the
  same cleanup hardening.
- Delimiter count over the module and band returned `diff=0`.
- A direct forbidden-route scan over `bmf-byte-cursor.fk` found no raw byte-at,
  whole-file byte read/write, raw file read, file-size, source-compiler, or
  line-grammar dependency names.

## Reviewer Post-Review

Grok returned `PASS`, with no required changes. Residual risks: future bands
can still repeat the temp-fixture cleanup mistake unless a shared cleanup helper
exists, and EOF detection depends on keeping the small-window matrix tight.

Claude returned `PASS`, with no required changes. Residual risks: CRLF editor
columns will not match this LF-only cursor convention, `bbc-byte` returns `-1`
both for EOF and stale windows if callers bypass refresh/advance discipline,
other temp-dir bands may still have fixture accretion bugs, and all-bits-set
band constants must be kept in sync with the bit list.

## Sibling Boundary Correction

A later Layer 1a/1b sibling review found that the shared Go/Rust/TypeScript
proof floor must not claim arbitrary high-byte binary or multibyte UTF-8
transparency through `read_file_slice`. The original direct `fkwu` observation
over bytes `128` and `255` is therefore not a shared-layer BMF proof.

The BMF byte cursor manifest and band were corrected:

- `binary-byte-transparent` was renamed to `nul-ascii-window-byte-indexed`;
- the fixture now uses `65 13 10 66 0 127 90`;
- the validator prelude header is a single parseable line;
- the static module scan reads from either repo root or `form/`;
- the static scan requires nonempty loaded source text before negative route
  checks can pass.

Corrected gate:

```sh
cd form && ./validate.sh form-stdlib/tests/bmf-byte-cursor-source-band.fk
# -> 4194303
```

No OOM or killed process occurred during this correction.

Corrective post-review:

- Grok/Jason returned `PASS`, with no required changes. Grok confirmed the live
  manifest and band now stay within the NUL/ASCII proof surface, and that the
  adjacent BMF core file-window and grammar-band repairs are scoped correctly.
- Claude/Popper returned `PASS`, with no required changes. Claude confirmed the
  old broad terms remain only as historical correction text or explicit deferred
  scope, not as live manifest/test claims.

## Deferred

- Migrating downstream grammar consumers after `bmf-core` `surface-file` moved
  onto the file-window path in
  `receipts/2026-07-04-bmf-core-file-window-surface-layer-review.md`.
- Wiring grammar consumers to this file-backed cursor.
- Arbitrary high-byte binary and multibyte UTF-8 file-cursor proof on the shared
  sibling floor.
- A shared temp-fixture cleanup helper for bands that create non-empty
  directories.
- Streaming/full-file hashing for artifact identity.
- Runtime artifact 9h loader/executor.

The exchange stayed alive by not treating a green byte-window as a finished BMF
file story: the old red cursor witness was repaired at the next honest layer,
with EOF and refresh behavior made observable.
