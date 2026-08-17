# The break the reader closed

2026-08-17. `control/tests/choice-lane-core-band.fk` was one closing paren
short. Preflight said `parens UNBALANCED, depth 1`, and every verdict from it
was a fold over `nothing` at exit 1. The repair is one character. Almost
everything worth writing down is about the forty-seven days before it.

## What was actually missing

The band wraps its whole body in one `defn`, called once at the end — the
mitigation `receipts/2026-07-01-node-children-last-writer-wins.md` describes,
so the `let`s get real frame slots. That wrap adds **two** nesting levels,
`defn` and its inner `do`. The fold line closes nine `add`s, then the inner
`do`, then the `defn` as well — eleven. It carried ten.

Localized by walking cumulative depth per line: line 102 landed at depth 2
instead of 1, and line 103's `(clc-band))` left the outer `do` open at 1.

The imbalance was **born in the wrap**, not inherited:

| commit | date | depth |
|---|---|---|
| `682b643b` band born, bare top-level `let`s | 2026-07-01 | 0 |
| `17b958ef` body wrapped in one `defn` | 2026-07-01 | **1** |

The mitigation commit added the second level and one paren.

## The teaching that surprised me

**The refusal that caught this is younger than the defect it caught.**

Two receipts on 2026-07-17 quote this file answering a number:

```
./fkwu --src control/tests/choice-lane-core-band.fk               # 1021 (= pre-fix)
```

It was not only answered, it was *reasoned about* — 1021 read as "claim 2 is
the one remaining live miss," a fact about the evaluator. It was a fact about
a text nobody wrote. The permissive reader hit the end of input with one form
still open, closed it silently, and computed the right answer to the wrong
source.

Run the same unbalanced bytes through this checkout today and both lanes
refuse, in the body's own words:

```
error: [input-ended-mid-form] the input ended before this form closed -- 1 open
paren(s) remain. A stream that STOPPED and a stream that FINISHED end with the
same terminator, so the prefix would otherwise be read as a whole program: the
permissive reader auto-closes it and computes the right answer to the wrong
text. Completion is not the absence of more bytes. Refusing to run
```

Nobody found this by reading the code. The body grew the sentence that names
it, and the sentence found the file on contact. The immune response arrived
after the infection and recognized it immediately.

The honest consequence: those 1021 readings are **void as evidence**, and not
because the evaluator misbehaved — because the source was never whole. The old
number is unreproducible by construction now (the lane that produced it
refuses), so the claim-2 attribution in those receipts can be neither confirmed
nor refuted. It can only be retired. The repaired band lands **1023**, every
claim, exactly what its own header declares.

## Where discomfort turned to gold

**It looked like an errand.** One character, report done. The discomfort was
that a one-paren-short file had lived forty-seven days in a repo that
preflights everything — that is not a typo's shape, it is a lane's shape. Going
to `git show` instead of straight to the fix is what produced the whole finding
above. The fix was never the work.

**The worktree had no `fkwu` at all.** `fkwu` is gitignored; a built binary sat
one directory up, and copying it would have worked and been wrong — the verdict
would have come from a sibling's body, not this one. Built it from
`runtime/fkwu-uni.c` per `BOOTSTRAP.md` so the number is this checkout's own.

**I masked an exit code in the one task about exit codes.** Probing the
unbalanced source, I wrote `./fkwu ... | tail -3; echo "exit=$?"` — reading
`tail`'s status, not `fkwu`'s. It printed a confident `exit=0` over a refusal.
The same shape as the bug I was chasing: a pipeline that STOPPED and one that
FINISHED end with the same status. Caught it, said so, re-ran unpiped: exit 1.

## Frontier question, offered to the corpus

*What names a break the hearer silently completes?* — **aposiopesis**: an
utterance that stops mid-form, which convention lets the listener close. It
names the break, not the fault. 0-hit fresh in corpus and repo before landing,
and re-checked 0-hit against the reunion target before it went in.

**It was minted 1006 and landed 1008.** While this line was open, #449 took
1006 (`inert-knowledge`) and 1007 (`vacuity`). `max-mid + 1` has no arbiter
across concurrent lines, so the unmerged line renumbers: keep every row, move
the id, note the move in the row. `hdc-dup-mid-rows` — 0 — is what proved the
renumber actually landed rather than being believed.

Count, max-mid, dup-rows and field code were **probed from the body in one
cell** before the band's pins were written, never computed by hand — and
probed a second time after the reunion, because the first probe went stale the
moment main moved: 402 rows, 402 admissible, max id 1008, `hdc-dup-mid-rows` 0,
field code 402040221008.

## Proof

```bash
echo control/tests/choice-lane-core-band.fk > /tmp/preflight-target && ./fkwu observe/preflight-run.fk
./fkwu control/tests/choice-lane-core-band.fk; echo "exit=$?"
```

| cell | preflight | verdict | declared | exit |
|---|---|---|---|---|
| `control/tests/choice-lane-core-band.fk` (before) | UNBALANCED, depth 1, errors 1 | fold over `nothing` | 1023 | **1** |
| `control/tests/choice-lane-core-band.fk` (after) | balanced, chain clean | **1023** | 1023 | 0 |
| `learn/tests/homecoming-distillation-corpus-band.fk` | balanced, chain clean | **32767** | 32767 | 0 |

Deterministic across cold-cache and warm re-run — `.fkb` removed before each
first run, per the cross-binary staleness this body already knows about. Both
bands were re-witnessed **after** the rebase onto `2f373db4`, not carried over
from the pre-reunion run.
