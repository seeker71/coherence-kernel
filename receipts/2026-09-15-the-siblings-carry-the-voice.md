# The siblings carry the voice

2026-09-15 · Claude (Opus 5), embodying Sema

## What closed

The organ-health carrier (`form/form-stdlib/organ-health.bml`) answered only on fkwu. On the
proof siblings `noc-open` in `form/form-stdlib/bml/native-owner-clock.bml` stopped at its first
door: Go `walk: unbound function "kernel_stat"`, Rust `unbound function: kernel_stat`,
TypeScript `call: unbound kernel_stat`. So no organ that voices its health could run four-way,
the BML compiler's own voice (`bml-compiler-health.bml`) among them.

Go, Rust and TypeScript now bind the three doors with fkwu's meaning, read from fkwu's own arms:

- `kernel_stat 164` is fkwu's arm counter for tag 64, `record_new` (`fkwu-uni.c:17887`, counted at
  `9145` and `12503`). Each sibling counts `record_new` where the record is made. Every other key
  answers nothing. fkwu answers 0 for a key it does not know, but a sibling answering 0 for a key
  fkwu measures would claim a counter it never read.
- `kernel_live pid` answers fkwu's page words 0-2 (magic `0x464B4C4956`, pid, start-ms) for the
  process's own pid (`fkwu-uni.c:16588`, layout at `14128`), and the empty list for any other pid.
  That empty list is fkwu's answer where no page can be read.
- `print_str` writes the string and one newline, flushed, and answers 0 (`fkwu-uni.c:18024`).

The same probe cell on all four: fkwu answers `[34, 301911001430, 1, 1, 1, 1, 4, 0, 0]`, and Go,
Rust and TypeScript answer `[3, 301911001430, 1, 1, 1, null, null, null, 0]`. The first five
readings agree. The siblings hold three words of the page and answer nothing where they measure
nothing.

With the doors standing, the next stop was `json-emit-precise`
(`argStr: arg 1: expected str, got null`, sixteenth pair, `at_ms`). `json-emit-leaf` now reads
storage slot 5 as an integer through `node_value`.

The registry took three rows (`kernel_stat` and `kernel_live` witness, `print_str` call), and its
lanes read 176+29. `primitive-registry-band` answers 47 three-way (exits go=0 rust=0
typescript=0, "1 ok, 0 divergent"). `./fkwu gate/primitive-registry.bml` reads
`primitive-registry: OK 205 natives == 205 rows; lanes 176+29; band pins aligned`.

## The witness is the organ voice

These readings come from the rebased tree:

- **The specified cell** (`oh-reading "probe"` voiced with `oh-voice`, 7 when `oh-valid`),
  through `./validate.sh`, exits go=0 rust=0 typescript=0. Each leg prints `7` with an empty
  stderr and one voiced line. fkwu answers 7 with rc 0. validate names the legs divergent
  because each voiced row carries its own process (`id` pid:birth:count, `at_ms`).
- **A read-back cell** parsed every kernel's voiced line on every kernel. Each reads as a valid
  organ-health-v1 row, re-emits to the same bytes, and is followed by the blank line and 7. It
  answers 255 on go, rust and typescript (validate ✓) and 255 on fkwu.
- **The compiler's voice**: `bch-ran` on an answer the Hati lane could not lower
  (`BML-HATI-UNSUPPORTED`) voices observe, response (`abstain`) and applied on go, rust and
  typescript, three rows each, verdict 3, empty stderr. fkwu answers the same.

The throwaway cells are gone; what stays is the voice itself, running on every kernel.

The instruments on the same tree read binary-freshness 31 and
`drift-gates pass=31 full=31 refused=0`. The native authoring guide reads
`Python implementations=1 execution candidates=195 unread=0`. That one file,
`form/scripts/test_glass_keyboard_pty.py`, predates this work.

## Most surprising teaching

The request said fkwu's `print_str` adds no newline. fkwu's arm writes one
(`putchar(10)`, `fkwu-uni.c:18034`), and `oh-voice` appends its own `"\n"`, so every voiced row is
followed by a blank line. That was true on fkwu first, and now on every sibling, because the
siblings follow the arm, not the description of it.

The second surprise: the carrier's next stop was a timestamp, not a missing door. A value past
2^31 is type 1 on fkwu and type 5 on all three siblings, whose 32-bit inst holds a table
position. `json-emit-leaf` read the slot, so every row's `at_ms` emitted null there. The seam
sat in the most ordinary value the voice speaks.

## Where discomfort turned to gold

The quick heal was to make the siblings' `node_type` answer 1 for a wide int: one line per
kernel, and the reading would match fkwu. Sitting with it, `node_inst` on the same node answers 0
or 1 (a table position) where fkwu answers the value. `intern-content-address-band.fk` already
names that table position as a seam it will not assert away. A type of 1 beside a table-position
inst would teach any reader that trusts type 1 to read an index as a value.

The gold was to heal the reader instead. It now asks `node_value`, which reads the value on every
kernel, as `typed-literal-carrier.fk:61` already did. The identity seam stays visible and named.
That band's header now says what the siblings answer: they bind these natives, and each answers a
table position and type 5. It had said they did not bind them.

## Frontier word

**slotleak** (0 hits in the tree before this receipt): a kernel's storage slot answering a
reader's question about kind, as type 5 stood where "integer" was asked.

Which Form readers still dispatch on `node_type` or `node_inst` where `node_value` would answer
the same on every kernel? And once the siblings carry a value-bearing inst past 2^31, would that
one heal retire both the `typed-literal-carrier` arm and the `json-emit-leaf` arm?
