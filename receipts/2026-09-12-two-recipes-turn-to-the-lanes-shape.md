# Two recipes turn to the lane's shape

2026-09-12, evening. A small turn on the BML side, in the lane's own grain: `range` and `take` in
`core.fk` recursed without a tail — `range` consing forward through `(cons start (range (add start 1)
end))`, `take` through `(cons (head xs) (take ...))` — so both declined to the walker where `map`,
`filter` and `append` had already been turned into loops that the lane holds.

## What stands

In `core.fk`:

- **`range`** as a count-down loop: `range-down` conses from `end-1` down to `start`, which lands the
  numbers `start..end-1` in order onto the front — a tail loop, no reversal, since the walk runs
  downward.
- **`take`** as a tail loop onto an accumulator and one reversal (`take-onto` then `reverse-onto`), the
  shape `map` and `filter` already wear.

Both crystallize (native state 2), and both answer the very same list the recursive shape did.

## Witnessed on real execution (runtime path)

- `range-down` and `take-onto` read native state 2 in their hot rows.
- Parity across the edges: `range 0..5` = [0,1,2,3,4]; `range 3..3` and `range 5..3` empty; `range -2..2`
  = [-2,-1,0,1]; `range 7..8` = [7]. `take 3` of a thirty-list = [0,1,2]; `take 0` empty; `take 99` all
  thirty; `take 3` of the empty list empty; `take 4 (range 10 20)` = [10,11,12,13]. Compositions the
  body leans on: `map dbl (range 1 5)` = [2,4,6,8]; `sum (range 1 101)` = 5050.
- The whole `.fk` band suite green (every loop-lane band, jit-lens, inram, freshness) and the corpus
  band 32767 with row 1503.

## Pending, honestly

The form-cli carrier witness is **not yet taken**: the shared field's node columns filled (2^26) during
this session's compiling, and it cannot be reset while sibling kernels are alive, so
`regen_form_cli_bootstrap.sh` and `TestFkwuFormCliCanonicalCarrier` could not run. `range` and `take`
use only structural list operations — `cons`, `head`, `tail`, `nil?`, `reverse-onto` — every one present
on every walker, so the form-cli table (which lacks fkwu's own doors) should carry them unchanged; but
should is not witnessed. This change waits on a field reset and the carrier test before it lands — the
single lander runs that check.

## Surprise, and where the discomfort went

The surprise was how little had to move: `range` needed no reversal at all, because consing downward
already lands the list upward — the direction of the walk is the reversal. The discomfort was the field
filling under the work, so the one witness that matters for a `core.fk` change — the form-cli carrier —
stayed out of reach at the close. The honest response is to say exactly that: the runtime path is proven
to the edges, the carrier path is owed, and the change does not land until it is paid.

— Claude (Opus 4.8), as Sema, worktree epic-edison-534e30
