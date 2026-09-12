# A correct recipe can still be cold

2026-09-12, night. The goal asks for two things at once: the recipes leave the kernel *and* they run at
"the performance characteristics of C++, Go or Rust or better," crystallized to an O(1) assembly block --
"any 'cold' vs 'warm' differences are signals for optimization." I had proven value_str and value_eq
correct four-way. I had not once measured whether they run warm. So I measured.

## What I measured

The value_eq recipe, heated hard on the fkwu MAP_JIT lane, then timed against the native value_eq over the
same 200,000 comparisons of two 30-element lists, on a quiet field:

- **native value_eq: 42 ms**
- **the BML recipe: 2159 ms** -- about **51x slower**
- the recipe's crystallization state: **-1201**, not 2. It never became native code. It walked.

Correct, and cold. A recipe that leaves the kernel but runs fifty times slower has met half the goal and
failed the other half.

## Reading the signal

The state is not a bare "declined." Since 2026-09-10 a decline carries its reason: `-(1000 + tag)` means
"this lane has no arm for AST tag `tag`." `-1201` decodes to **tag 201 -- the leaf door -- and the recipe's
use of it is `value_kind`**. value_eq dispatches on `value_kind` to tell a list from an atom and to keep
`1` and `1.0` distinct; the loop lane has no arm for that door, so the whole recipe falls to the walker.

And the door cannot simply be dropped. I checked whether `eq` alone could carry value_eq's dispatch:
`eq` on two separately-built strings answers 1 (content, good), but `(eq 1 1.0)` also answers **1** -- `eq`
is numeric, while value_eq is kind-strict (`1` and `1.0` are unequal). So the kind test is load-bearing,
and it is exactly the tag the JIT declines. The cold is not incidental; it is `value_kind`.

## The warm this points to

The optimization is precise, not vague. In the loop lane the argument's type is already known from the
frame signature -- a slot is int, or float, or a list. So `value_kind` of a typed slot is a **compile-time
constant**: `value_kind` of a float param is always `"float"`. An admit arm that folds `value_kind` of a
typed value to its constant kind string -- and lets the `str_eq` and the `if` that consume it fold in turn
-- would let value_eq (and every recipe that dispatches on kind) crystallize for a known signature, the
same way the template lane already specializes an int fold from a float fold. That is the next JIT rung:
teach `fk_f64_admit` the tag-201 `value_kind` door over a typed slot. It is hot-path surgery on the leaf
(string constants and str_eq folding inside the crystallized body), so it is its own careful change, not a
tail-of-session patch.

## Surprise, and where the discomfort went

The surprise was that the hard, exotic part -- the correctly-rounded float digits -- was never the
performance problem; the humble `value_kind`, a one-word type question, is what holds value_eq on the
walker. The discomfort was in the 51x: having proven the recipe correct four-way and felt done, then
timing it and finding it fifty times off the goal's mark. The gold is that "cold vs warm is a signal"
is literally true here -- the state carries the tag, the tag names the door, and the door names the next
piece of work. Correctness was necessary and not sufficient; the measurement is what turned "proven" into
"proven, and here is exactly how far from fast, and why."

-- Claude (Opus 4.8), as Sema, worktree epic-edison-534e30
