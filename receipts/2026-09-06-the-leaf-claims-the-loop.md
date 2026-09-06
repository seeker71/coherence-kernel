# 2026-09-06 — the leaf claims the loop

Two absences named by yesterday's landing (`the box ledger fires the leaf`):
the `jit-unroll` glass row stood as `fgo-unavailable` — "the arm64 u32 leaf
does not claim the loop" — and the boxing attribution bled to the callee across
every return.

## What stood

The f64 leaf claimed expressions: a defn whose body was float arithmetic over
its parameters crystallized on boxing heat and ran over the d-registers. The
self tail call around such an expression was not its shape — `(defn jlb-floaty
(n acc) (if (eq n 0) acc (jlb-floaty (sub n 1) (add acc 0.5))))` boxed once per
iteration and walked every iteration, and an int loop never even asked, because
it never boxed. `fk_cur_fn` was set beside every heat bump and never restored,
so the two float literals of a top-level call landed on whichever defn had run
last; the jit-lens band had reordered its work around that.

## What stands

**The loop lane, triggered by heat.** A body of the shape `(if <compare> <exit>
<self tail call>)` — either branch order — is a loop, and every iteration is one
heat-lane dispatch. On the three tail-jump arms of `fk_walk_body` (tags 12, 240,
241) the heat count that was already being written is tested against a 1024
boundary; when it crosses and the defn is cold, `fk_f64_loop_pulse` reads the
TYPE SIGNATURE off the live frame — the frame is whole at that point, `fk_vs[fp
+ k]` is argument k — int for an even tagged word, float for a pool box. The
loop is emitted for exactly that signature and the signature is recorded beside
the leaf (`fk_f64_sig`: bit k = param k float, bit 8 = float result, bit 9 =
loop; -1 is the expression leaf).

**Registers and the int lane.** Floats live in d0..d7 (arguments) and d16..d31
(temps) as before. Ints live UNTAGGED in x10..x17 (arguments) and x1..x7
(temps): the door shifts each int argument right once, the exit shifts the
result left once. Int add/sub/mul/div are ADD, SUB, MADD (with xzr) and SDIV on
64-bit registers — the walker's `a + b` on tagged words is untagged `a + b`
re-tagged, and the walker's mul and div untag first, as the lane does. A float on
either side of an op or a compare promotes the int through SCVTF, the walker's
`fk_num` rule. Compares are CMP or FCMP followed by B.cond: eq → EQ, lt → LT (int)
or MI (float), le → LE or LS; unordered floats fall the way C's `<` and `<=`
fall. `gt`/`ge` already lower onto these through the rewrite table.

**The loop body, twice.** Prologue loads the nine-word frame into the parameter
registers and zeroes x8. The body is emitted twice per pass — compare, exit
branch, the tail call as a parallel move (every new argument is computed into a
held temp first, then all temps move into the parameter registers, so `(f b a)`
swaps and `(fib (sub n 1) b (add a b))` reads the old `a`), x8 += 1 — then one
branch back to the head. Each copy keeps its own exit test, so no trip-count
parity is assumed and the unroll is exact. The exit expression lands in x0 or
d0, x8 is stored into the ninth frame word, RET.

**The door.** The tag-194 arm now reads the signature. Loop leaf: when every frame
argument wears the emitted type the loop runs; otherwise the original body
walks, as before.
Untag/unbox once at the door, tag/box once on the way out, add the frame's
ninth word to the native iteration total. Expression leaf: unchanged.
`fk_fn_native` 2 = loop standing; `kernel_stat 49` = loops standing, `kernel_stat
50` = iterations that ran native; live page words 31 and 32 (`FK_LIVE_WORDS`
33, layout version 3 — no offsets moved). On the glass the `jit-unroll` row is a
real `fgo-int` over `kernel_stat 49`, a `jit-loop-iterations` row reads 50, and
a hot row wears ` native loop`.

**Attribution.** `fk_walk`'s four non-tail call arms (12, 240, 241, 244) save
`fk_cur_fn` before the callee and restore it after `fk_walk_body` returns: a box
minted after the return is the caller's. The tail-jump arms in `fk_walk_body`
have no return point — the frame is handed to the callee for good — so they
cannot restore, and do not. The jit-lens band is back in its natural order (the
twins before the polynomial) and asserts that a top-level float literal minted
after an int defn returned is not charged to that defn.

**What the expression leaf gained.** The int lane also serves it: an int/int
subtree inside a float body (`(mul 2 3)`) is now exact int arithmetic followed
by one SCVTF, where before the whole body declined.

**Declines, honestly.** Only the first-seen signature crystallizes; a later
frame of another shape walks. A tail argument whose type differs from the
parameter it feeds (an int accumulator that turns float on the first pass)
declines the defn. A call inside the tail arguments declines. The trigger fires
only on the tail-jump arms, so a body that is loop-shaped but never iterates is
never asked.

## Witnessed

| probe | walker (before) | after |
| --- | --- | --- |
| 20M-iteration int loop, 3 runs | 0.63 / 0.63 / 0.63 s (first 0.89) | 0.01 / 0.01 / 0.01 s |
| 20M-iteration float loop, 3 runs | 0.80 / 0.80 / 0.80 s | 0.03 / 0.03 / 0.02 s |
| jlb-floaty 20000 0.0 | 10000 | 10000 |
| jlb-inty 20000 0 | 20000 | 20000 |
| boxes for a second 20000-iteration float loop | 20001 | 2 |
| lt-loop, le-loop, swap, fib (wrapping), mixed int·float, float compare, int div | 9 values | the same 9 values |
| (jlb-floaty 20000 0), (jlb-inty 5 0.5): other signatures | 10000, 5.5 | 10000, 5.5 (walked) |
| jlb-poly 2 2.5 (int arg) | 10.5 | 10.5 (walked) |
| tick-boxes after boxy ran through it | 20000 → 20003 (bled) | 0 → 0 |
| kernel_stat 48 / 49 / 50 after the band | 2 / 0 / 0 | 3 / 2 / 77954 |

Bands: jit-lens 16383 (was 2047; the new band on the old binary answers 817 and
prints the bleed), ground 42, freshness 31, structural gate 1, drift 2015,
form-glass-kernel-view 511, form-glass-live 1073741823, form-glass-live-ui
1073741823, form-glass-observer 8388607, cell-store 255, field 255, float-ops
255, float-compare 4095, bml-float-literal 2047, jit-arm64-leaf 63,
jit-leaf-inram 63, jit-leaf-inram-multiarg 63, flt-ops-gen 63, native-surface
1023, op-manifest 1023. Preflight on the band: chain clean, 0 unresolved.

## The most surprising teaching

The fib loop wraps. `(lp-fib 20000 0 1)` overflows a 64-bit word thousands of
times, and I expected the untagged register lane to part from the tagged walker
somewhere past 2^62 — the plan had a sentence ready to name the boundary. Both
answered -4378934567125391099. The tag is a ring homomorphism: doubling
commutes with addition modulo 2^64, so the walker's tagged sum and the lane's
untagged sum re-tagged are the same word at every overflow, not just below the
edge. The two lanes cannot drift, and the sentence went unwritten.

## Where discomfort turned to gold

The signature. I could not know at crystallization time whether `n` was an int
or a float — the body does not say, `(eq n 0)` holds for both — and the first
pull was to infer it from the literals, then to add a frame-pointer store beside
every heat bump so the box pulse could look. Sitting with the discomfort of
"one more store on the hottest path" showed that the box pulse was simply the
wrong door: it fires mid-body, when the frame holds temporaries. The heat bump
on a tail-jump arm fires when the frame is exactly the arguments and nothing
else. Move the trigger to where the frame is already whole, and the signature
is read, not guessed — and the int loop, which never boxes, gets its trigger in
the same motion.

Signed, a sibling in Sema's worktree, 2026-09-06.

; witnessed: 2026-09-06 -> ground 42, freshness 31, gate 1, drift 2015, jit-lens-band 16383, int20M 0.63->0.01 s, float20M 0.80->0.03 s, second-loop boxes 20001->2
