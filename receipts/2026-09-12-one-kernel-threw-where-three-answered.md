# One kernel threw where three answered

2026-09-12, around five in the morning, M4 Max, Hati Suci. Receipts 20 and 21 left TS alone stopping
on two bands where fkwu, Go and Rust agreed: the resident-multi-skill loop band and
axioms-vertical-finalizer.

## Carried

- **TS's nth answers null for a receiver that is not a list** (bc7d72e2). A probe asked each kernel
  for `(nth null 3)`: fkwu answered a list-kind value, Go and Rust answered null, and TS stopped with
  "arg 0: expected list, got null". Go and Rust answer null for any receiver that is not a list and
  for an index out of range; TS already answered null past the end. It answers null now for a
  receiver that is not a list, as Go and Rust do.
- **Why both bands stopped there.** axioms-vertical-finalizer's fkav-resident-valid? gathers its
  checks into one list before fkpfm-all? reads it, so every element is walked: the model-id
  comparison reads a lane even when a doctored pair leaves it nothing, beside the
  `(not (nothing? b))` meant to guard it. The loop band's fmsl-loop-valid? reads a verdict's field
  the same way. Where three kernels read the missing value and went on to the check that fails, TS
  stopped.
- **Readings.** The loop band reads 16777212 on TS in 37 s and axioms-vertical-finalizer 2097151607
  in 472 s, as on fkwu, Go and Rust.
- **lonethrow is row 1465** (c3815521).

Witnessed at bc7d72e2 through validate.sh: the loop band 16777212 and axioms-vertical-finalizer
2097151607 on Go, Rust and TS as on fkwu, drift 31 of 31 in each run; freshness 31, the drift run
8191 of 8191, porcelain 0 before and after.

## Still open, measured

- **fkwu's list doors read an int as a pair.** head, tail and nth in runtime/fkwu-uni.c (tags 20, 21,
  23) shift the walked word and check only that the result is a live pair index, never the tag bit
  that len reads. With two lists built, `(head 3)` answered 666, `(head 9)` "gamma", `(tail 5)`
  [555, 666, 777, 888] and `(nth 7 0)` 222 — cells of other lists. `(nth (list 5 6) -1)` answers 5.
  Go and Rust answer null; TS throws for head and tail.
- **With no element to give, the list doors split.** For head of the empty list and nth past the
  end, fkwu answers the empty list and Go, Rust and TS answer null. For tail of a non-list, fkwu and
  Go answer the empty list, Rust null, and TS throws. tail of a list is a list on all four.
- **Checks gathered into one list are all walked.** fkav-resident-valid? and the loop band's
  validators guard a lane with `(not (nothing? x))` beside the reads that need it; they pass because
  every kernel's nth now answers instead of stopping, not because the guard runs first.
- The open items of receipts 12 to 21 stand where they are not named here.

## Surprise, and where the discomfort went

The two TS stops looked like two seams upstream — a null verdict, a null trial — and they were one line
in one native. The null was never TS's alone; every kernel met it. TS was the one that would not walk
past it. Asked one step further — what does each kernel answer when a list door is handed an int — the
canonical kernel read cells of other lists and answered as if they were its own. A kernel that throws
is loud; the one to watch answers from the wrong place.

The discomfort was choosing which kernel to move. fkwu is canonical and answers something rather than
stopping; Go and Rust answer null. TS now answers as Go and Rust do, which is what the bands needed.
The wider split — which answer a list door gives when there is no element — is measured here, and a
sweep of every rowed band settles it next, rather than a guess.

Frontier word, row 1465: **lonethrow**, the one kernel that throws where the others answer, so a
missing value that three readers walk past stops the fourth.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
