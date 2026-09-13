# A record gets its own band

2026-09-13, evening, M4 Max, Hati Suci. Row 1525 (kindshadow) found that fkwu boxed record r as 0-(r<<1), which is
also the word of the integer -r. So once one record existed, value_kind(-1) answered "record", and record? agreed.
That receipt held the repair for Urs, as a value-encoding change across every kernel. Urs's word this evening was to
close every gap. Counting where records live showed the repair is fkwu's alone: Go, Rust and TypeScript hold a record
as its own typed value, so no integer there can share its word, and the form-cli carrier's kernel has no record_new.
So it lands here, and one revert takes it back.

## Carried

- **A record is its own odd band.** fk_rbox(r) is fk_rbase - (r<<1) - 1, with fk_rbase at -7e18. That lies between
  the fn values (at or below -8e18) and every node box. fk_ridx reads only that band, and fk_isrec still bounds r by
  fk_rp. No other code builds a record's word.
- **The three comment blocks that called records even** (beside nothing, the fn values and the strings) now say where
  records live.
- **What changes for a program:**
  - value_kind and record? read a kind from the word alone. -1 is an int however many records exist.
  - A record is not a number. add and lt on a record now stop with the kernel's own words, as they do on a string.
    Before, they computed on the record's index.
  - `(eq rec -1)` is 0. A record still equals itself and differs from another record.
  - The lane types a record 0, so a leaf handed one declines. Before, it read the record as an int.
- **The kind fold loses its exception.** An int in a leaf is never a record now, so "int" and "record" fold like every
  other name.
- **loop-lane-kind-fold-band bit 16 pins the truth.** With two records minted, -1 and -2 answer "int" cold and hot,
  kfb-int? reads state 1, and a record answers "record". The band stays registered at 31.
- **record-word-band asks the same of all four kernels, registered at 15.** With two records minted, -1 and -2 are
  ints, record? tells -1 from a record, a record's kind is "record", and a record equals itself but not -1.
- **ownband is row 1528.**

Witnessed:
- **record-word-band reads 15 on Go, Rust and TypeScript through validate.sh, and 15 on this fkwu.** On the kernels
  of b01bc969d and 53a58d319, fkwu read 4: only a record's own kind held.
- **A probe with two records minted**, on 53a58d319's kernel and on this one:
  - value_kind of -1, -2, -3: record, record, int before; int, int, int now.
  - record? of -1: 1 before, 0 now. `(eq rec -1)`: 1 before, 0 now.
  - `(add rec 1)` computed before. Now it stops: "only numbers add, subtract, multiply and divide -- ask value_kind
    first". `(lt rec 0)` stops too.
  - value_str of a record, and of a list holding one: `<record>` and `[<record>, 1]` on both.
- **Every test that mints a record and runs** reads the same last line as before: record 176, record-fold 31,
  record-blueprint 7, record-field-access 16, method 116, list-of-record-reduction 22, grounded-cost-reduction 14,
  grounded-cost-record-handler, cache-phase 255, form-flatten 97791, frame-metal-failure 255, and the kind-fold
  band 31.
- **Bands:** every lane band and the registered record bands read as registered; value-str 1023, str-to-int-reading
  127, once-hold 15.
- **Before landing:** TestFkwu; freshness 31, the corpus band 32767, drift 16383 of 16383, porcelain 0.

## Still open, measured

- **Nineteen BML class and thesis proofs that mint records are the sibling kernels' to run.** On fkwu they stop at
  `source_scan_file`, an io door that fourth-arm-survey.sh names as fkwu's wall (`[unresolved-call]` at fkwu:13722
  and fkwu:16038), on this kernel and on b01bc969d's alike. So no class program exercised this change on fkwu; the
  record bands above did.
- A kind test over a word-typed value; the value_str float-arm switch; the open items of receipts 12 to 60.

## Surprise, and where the discomfort went

The surprise was the size. kindshadow named the repair a value-encoding change across every kernel. Counted, it is two
functions and three comments in one kernel, because the others keep a record as a typed value, never as a word. And
its note that no four-way band could reach the wound did not hold: Go, Rust and TypeScript do mint records, and
there value_kind(-1) was "int" all along. fkwu was the kernel that disagreed.

The discomfort was lifting a hold another session had placed for Urs. The hold was honest: it was placed before
anyone counted where a record's word is built. The gold is that the counting was the whole cost. Everything reads
records through fk_rbox and fk_ridx. With those two changed, the fold that was held back because the primitive lied
can fold, because the primitive now tells the truth.

Frontier word, row 1528: **ownband**, a kind given its own band of words, so no other kind can wear one and a value's
kind is read from its word alone.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
