# A string op stops on a non-string

2026-09-13, evening, M4 Max, Hati Suci. The timing receipt's first baseline called str_len on an int by mistake.
Every sibling leg stopped on it, and fkwu answered 0. A survey of the string ops found six that answered the wrong
kind where Go, Rust and TypeScript stop on the argument's type:

| op, handed the wrong kind | fkwu answered |
|---|---|
| str_len | 0 |
| str_byte_at | -1 |
| str_concat, either side | -1 |
| str_eq, either side | 0, and `(str_eq 5 6)` 1 |
| str_to_float | 0 |
| byte_to_str, handed a string | "" |

substring answers "" for a non-string on all four kernels, so it stays as it is.

## Carried

- **Each op stops with its own words**, as arithmetic and order already do over a non-number (03a03f6f4):
  - "only a string has a length"
  - "only a string has bytes"
  - "only strings join"
  - "only a string reads as a float"
  - "only an int is a byte"
  - "only strings and nothing compare as strings"
- **The absence stays a value str_eq may ask about.** str-eq-absence-band (#577, four-way) pins that an absence
  equals only an absence. So str_eq stops on an int, a float, a list or a record, and still answers for nothing.
  str_len of nothing already stopped: a length is a measurement, and a comparison is a question.
- **The in-range contracts stand.** str_byte_at past a string's end still answers -1, and byte_to_str past 255
  still answers "".
- **string-op-stops-band, registered at 127** (FOURTH-ARM ONLY). A stop ends the run, so each stopping case is a
  child process, and the six children are committed beside the band.
- **loop-lane-self-recursion-band's header now says what declines.** It is value_kind of a list element, not
  value_kind itself.
- **blankpass is row 1532.**

Witnessed:
- **A trace kernel over every registered fkwu band.** The trace kernel keeps the old answers and writes a line each
  time a string op is handed a non-string.
  - It ran 919 .fk bands and the 46 .bml ones.
  - One band met the case: str-eq-absence, four times, each time with nothing.
  - Nine bands read other than their registered verdict with no trace line: the Vulkan and matrix-unit GPU lanes,
    host-process (125) and teach-sema-pattern. They read the host, not these ops.
  - form-cli-heal-dynamic has no band file under its stem. It is a service: observe/form-cli-heal-dynamic-run.fk
    reads one observation from stdin and answers it. Handed three observations, it answers each the same on the
    old kernel and this one, with no trace line and exit 0.
- **All four kernels now stop on all six ops, and on `(str_eq 5 6)`.** `(str_len "abc")` answers 3 on all four.
- **The six children** exit 0 with the old answers on 53a58d319's kernel, and exit 1 now.
- **Bands:**
  - str-eq-absence 63; loop-lane-str-eq 31 and str-eq-field 15.
  - Every lane band and the record bands read as registered.
  - value-str 1023, str-to-int-reading 127, once-hold 15.
- **Before landing:** TestFkwu; freshness 31, the corpus band 32767, drift 16383 of 16383, porcelain 0.

## Still open, measured

- **The sweep covers the registered bands.** A program outside them that read a blank answer (the live glass, the
  organs, the hearth) now stops with one of these lines. The first run of each after a rebuild will show it.
- The value_str float-arm switch, the wider word window, and a kind test over a word-typed value; the open items of
  receipts 12 to 62.

## Surprise, and where the discomfort went

The surprise was `(str_eq 5 6)` answering 1, "equal". Each int read as "no string", and no string equals no string.
That is the same path that lets nothing equal nothing, which is right and pinned four-way. The int answer rode along
on it.

The discomfort was my first cut. It stopped str_eq on nothing too, and would have broken a contract written on
2026-09-04 out of exactly this kind of divergence. The sweep found it in one band out of nearly a thousand. The gold is
the line between the two cases: the absence keeps its answer because it is a value the axioms name, while an int
handed to str_eq is not an absent string. It is the wrong kind.

Frontier word, row 1532: **blankpass**, an operation that lets a wrong-kind argument through with a blank answer
(0, -1, ""), so the caller reads an answer that was never computed.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
