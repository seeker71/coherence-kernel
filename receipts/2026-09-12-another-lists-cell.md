# Another list's cell

2026-09-12, around half past five in the morning, M4 Max, Hati Suci. Receipt 22 closed with TS's nth
answering null, and named what one more probe had shown: the canonical kernel, handed an int where a
list belongs, answered with a cell from some other list.

## Carried

- **fkwu's list doors read the tag bit** (a61bdbb9). head, tail and nth in runtime/fkwu-uni.c
  shifted the walked word to a pair index and checked only that the index was live. An int is an
  even word, so an int that landed on a live pair read that pair: with two lists built, `(head 3)`
  answered 666, `(head 9)` "gamma", `(tail 5)` [555, 666, 777, 888], and `(nth (list 5 6) -1)`
  answered 5. Each door now reads the tag bit first, as len already did.
- **One contract on four kernels** (a61bdbb9). A receiver that is not a list, and an element that
  is not there, answer nothing on fkwu and null on Go, Rust and TS; the tail of a list is a list.
  Eleven probes — int, string and missing receivers, the empty list, an index past the end and
  below zero — answer alike on all four. Go's tail of a non-list answered the empty list and TS's
  head and tail threw; both move to the contract. Rust already answered it.
- **The sweep before the change.** A scratch fkwu with the same code read all 943 rowed fourth-arm
  bands against their registered verdicts. 13 differ from their registration and read the same on
  the current fkwu. Three more differed only because the scratch binary lived elsewhere, and an
  unmodified copy built in the same place read them the same way: host-process asks that the
  running binary be named fkwu and that the child it starts as ./fkwu run the same binary (127
  against 124), and the two q8-0 matmul bands read 0 because the dynamic Metal carrier reports
  metal_loaded=false for a binary outside the root, where ./fkwu reports true. The rebuilt ./fkwu
  reads host-process 127 and both matmul bands 15.
- **strayread is row 1466** (6448308e).

Witnessed at 859c3c74, with a61bdbb9 beneath it, through validate.sh: the loop band 16777212,
prefix-choice 4194303, flow-control 4095, prefix-session 8388607, family-native-exec-teach-check
1073741823, fs-crud 11111111, axioms-vertical-finalizer 2097151607, the heldout band 16371 and
turnwheel 33554431 — four-way where rowed, drift 31 of 31 in every run; freshness 31, the drift run
8191 of 8191, porcelain 0 before and after.

## Still open, measured

- **13 rowed bands read on fkwu apart from their registration**, the same before and after this
  change: xpath, doc-xpath and concept-xpath; framebuffer-viewer and framebuffer-readback;
  transformer-kernel; form-cli (2097151 against 1048575); nanite-mem-parse; and five vk live lanes.
- The open items of receipts 12 to 22 stand where they are not named here.

## Surprise, and where the discomfort went

The kernel every other kernel is measured against was the one answering from the wrong place, and
no band noticed: a band that hands an int to head is already off its path, and fkwu met it with
something shaped like an answer. Three kernels answered null or stopped; the canonical one answered
666.

The discomfort was moving the canonical kernel. fkwu's answers are what the registry holds, and
changing what it gives for a missing element could shift verdicts across the body. The sweep carried
that weight: every rowed band was read before the change was trusted, and the three that moved were
traced to where the scratch binary lived rather than to the change, by building an unmodified
control in the same place and reading what each band asks of its binary.

Frontier word, row 1466: **strayread**, a read that leaves the value it was handed and lands in
another's cells, answering with something that belongs elsewhere.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
