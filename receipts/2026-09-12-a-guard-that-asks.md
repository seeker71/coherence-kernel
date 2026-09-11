# A guard that asks

2026-09-12, a little after four in the morning, M4 Max, Hati Suci. Receipt 18 left three
form-knowledge-public bands stopping at one predicate. Following them found one wound in three
shapes: guards that measure a value to learn what kind it is.

## Carried

- **fkpfm-string? and fkpfm-int? ask value_kind** (64c29fcd). form-knowledge-public-family-mastery.fk
  recognized a string by measuring it with str_len and an integer by rendering it with int_to_str,
  under a comment saying fkwu has no value_kind. Every kernel answers value_kind natively: a probe
  reads string, string, list, int, null on fkwu, Go, Rust and TS alike. On Go, Rust and TS, str_len
  on a list stops the kernel; on fkwu it answers. The predicates ask the kind now.
- **Six PARSE-FAIL checks ask node_eq against the sentinel.** A failed parse is
  `(intern_trivial_string "PARSE-FAIL")`; a good one is a composite. Six places asked
  `(str_eq (node_value X) "PARSE-FAIL")`, and node_value of a composite answers 0 on fkwu and stops
  Go, Rust and TS. Interning is content-addressed on all four (a probe reads 1 0 0), so
  `(node_eq X (intern_trivial_string "PARSE-FAIL"))` answers where the old check answered and
  answers 0 where it stopped; fnri-shell.fk already asked this way.
- **Seven evidence guards ask before they measure.** fkpfm-text? measures a value only once it is
  known to be a string. The receipt, report and evidence fields it guards can be missing, and fkwu
  stopped on the report field that was measured unguarded.
- **Readings.** On fkwu after all three, generative-school answers 2034236639 where it stopped;
  finalizer 402653179, query-offer-batch 8354001, resident-eval 59903,
  generative-supervised-exemplar 14, family-challenge-seals 32767 and
  family-native-exec-teach-check 1073741823 read as before. On Go, family-native-exec-teach-check
  reads its declared 1073741823 where it stopped, generative-supervised-exemplar reads 14 as fkwu
  does, and family-challenge-seals keeps 32767. Of the 28 bands that reach the edited files, the
  first 10 read on all four kernels: six agree at their declared values, and in four the siblings
  stop at node_eq handed lists or strings where fkwu answers. The ternary sugar band reads 7 on all
  four.
- **gaugeguard is row 1462** (4af5526a).

Witnessed at 4af5526a through validate.sh: the ternary sugar band 7 and
form-knowledge-bml-temporal-challenge 65535 on Go, Rust and TS, drift 31 of 31 in both runs;
freshness 31, the corpus band 32767, the drift run 8191 of 8191, porcelain 0 before and after.

## Still open, measured

- **family-mastery still stops**, on fkwu at "str_len: nothing has no length", now after 9 s
  rather than 2; on Go it runs past four minutes without reaching that point.
- **node_eq handed lists or strings** stops four bands on Go, Rust and TS: the prefix-choice,
  flow-control and prefix-session bands' own comparisons, and form-cli-resident-multi-skill-loop.fk's
  snapshot step. fkwu's node_eq compares the values; value_eq asks that question on every kernel.
- **The 28-band reading runs on**; 18 of the bands remain to read.
- **generative-school** answers 2034236639 on fkwu against its declared 2147483647; on Go it stops in
  the band itself, where str_concat meets a null (fkg15b-run).
- **query-offer-batch** stops on Go where a null reaches nmfe-sha256, receipt 18's null seam; fkwu
  reads 8354001 against 8388607.
- **generative-supervised-exemplar** reads 14 on fkwu and on Go against its declared 15.
- **node_value of a composite** answers 0 on fkwu and stops Go, Rust and TS. The callers ask first
  now; the kernels still differ.
- The open items of receipts 12 to 18 stand where they are not named here.

## Surprise, and where the discomfort went

The comment was the wound's alibi. "fkwu intentionally has no value_kind primitive" sat above the
predicate that measured, while fkwu answers value_kind natively, as every kernel does. The guard
stood on a fact that had moved, and it stopped the three kernels that measure strictly.

The discomfort was that each heal uncovered the next stop — the predicate, then the parse check,
then the evidence fields — and family-mastery still does not reach its verdict. What held it: each
stop moved later, and every change asks the same one question, so the next one had a shape before
it was found.

Frontier word, row 1462: **gaugeguard**, a guard that measures a value to learn its kind instead of
asking it, so it passes a lenient kernel by accident and stops a strict one.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
