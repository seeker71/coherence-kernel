# The call that kept everything

2026-09-13, late morning, M4 Max, Hati Suci. Written in chain shape, the digit loop of epic-edison's float-repr
(Dragon4) still read -1 once the three caps followed the page. A bare -1 names no reason, so I built a witness binary
that keeps the last sixteen failing return lines. It named three more walls in one run, and the last of them was
the call arm's own size.

## Carried

- **The call arm keeps only what its leaf holds.** Around every call it had saved and reloaded 42 registers: x0,
  x8, x30, x10 to x17, x1 to x7, d0 to d7 and d16 to d31. Now it keeps x0, x8 and x30, the slots holding a value
  (parameters, lets and hidden string slots, read once the leaf is admitted), and the temporaries held below the
  two counters at the call. Everything else was the callee's to take anyway.
- **The tables follow the page.** Each derivation carries its reason in the source:
  - `FK_F64_CALL_CAP` = WORD_CAP/8, because a call arm writes more than eight words.
  - `FK_F64_OVF_CAP` = WORD_CAP/2, because an overflow site is a branch word beside at least one other word.
  - `FK_F64_NODE_CAP` = 3 × WORD_CAP, because a node writes at least one word or is one of at most two leaves
    beside one that does.
- **loop-lane-call-band reads 4095.** dd59ab024's fkwu reads 1023 on it. Two bits are new:
  - Bit 1024: a leaf of forty nested calls reads state 1.
  - Bit 2048: Dragon4's digit-loop shape reads state 2 and answers eighteen 3s cold and hot. Its condition inlines
    two helpers that call leaves, and its self call carries three call arguments and a cons.
- **spillall is row 1513.**

Witnessed:
- **The walls, in the order they stood.** The loop is fr2-run: the chain-shaped d4-loop over core's bn kit.
  - Overflow sites (64): list narrows take sites too, and the loop is unrolled twice.
  - The node table (128).
  - The page (4096 words): about 26 calls, at about 120 words each.
- **The loop now:** fr2-run reads 2 and renders 0 mismatches against value_str on about 2,900 values: 2^k and
  3, 5 and 7 · 2^k for k in [-200, 200], 10^k for k in [-40, 40], 41 subnormals, the tie, the halfgap, 1e-300, the
  largest double, and 1,200 LCG doubles.
- **Capacity:** 44 nested calls crystallize in one leaf. Before, 36 did not.
- **Timing:** one process per case, 5000 renderings, host load about 8. value_str's native takes 0.01 to 0.02 s for
  the same work.

  | float-repr of | before | after |
  |---|---|---|
  | 0.1 | 0.08 s | 0.07 s |
  | the tie | 0.65 s | 0.46 s |
  | 123456.789 | 0.43 s | 0.32 s |
  | 1e-300 (200 renderings) | 1.51 s | 1.46 s |

  The same leaves compile before and after, so what changed is the cost of each call. The tie's seventeen digits
  make many calls; 0.1's single digit makes few.
- **Bands:**
  - Every lane band reads as registered.
  - float-repr 255, value-str 1023, str-to-int-reading 127, once-hold 15.
- **Before landing:** TestFkwu; freshness 31, the corpus band 32767, drift 16383 of 16383, porcelain 0.

## Still open, measured

- **float-repr (a) waits on epic-edison's fixes:** the block belongs inside core's `(do … 0)`, the band needs its
  registration line, the receipt its edge as it stands on dd59ab024, and the frontier row goes in as 1512.
- **The chain-shaped orchestration (b) goes into core.fk after (a).** fr2's d4-scale, d4-loop, digit string and
  final digit are witnessed above.
- **The extreme exponents cost about 7 ms each.** The time goes to decimal limbs and to bn-pow2's one doubling per
  bit (1074 of them for 5e-324). Base-1e9 limbs (epic-edison's warm3 took 1e-300 about five times faster) and
  powers of two taken in chunks are the kit's next rung.
- **Common values are still slower than the native:** 0.1 takes about 10 µs where the native takes about 1.
  Dragon4's per-digit bignum work is what is left. Ryu or Schubfach in BML is the rung after the kit.
- The open items of receipts 12 to 51 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was a number that did not hold. My first before-and-after read eight times faster on 0.1. Measured
again, both binaries took 0.07 to 0.08 s. The first "before" run had rebuilt core.fk's cached image, because the
other binary had written that image, and the first run after a switch pays for the rebuild. The gain that holds is
the tie's, 0.65 s to 0.46 s.

The discomfort came twice. First, three probes went to the wrong suspects: the node table, the `gt` rewrite and
the if-expression condition. Each change left the same bare -1. The trail of failing returns then named every wall
in one run: first the overflow sites, then the nodes, then the words. The last wall pointed at the call arm
itself, and that was the rung. Second, I nearly carried a headline number. Measuring a second time kept the
eight-times figure out of the tree. The gold is a lane whose calls cost what the caller holds, with a band bit for
the loop that found it.

Frontier word, row 1513: **spillall**, a call that saves every register a callee might touch rather than the ones
its caller holds.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
