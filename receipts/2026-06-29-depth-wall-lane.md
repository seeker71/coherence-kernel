# Receipt — the depth-wall lane: measured, designed, and honestly NOT yet landed (2026-06-29)

**The walk (Urs):** take the depth-wall (form-asm lowering) — the structural lane that unblocks a class.

## Measured

`fk_walk` recurses on the C stack (~17 KB/frame). The wall is at **~60 deep**, and it hits **both** tail and
non-tail recursion (no tail-call optimization):

```
fac(60) -> 504403158265   fac(70) -> (overflow)        non-tail
acc(50) -> 1275           acc(500) -> (overflow)       TAIL recursion ALSO overflows -> no TCO
```

## Designed

The on-path structural fix (the recursion->loop transform `form-asm` lowering performs) is **tail-call
elimination**: a `fk_walk_body` trampoline that loops through tail position (if-branch, do-rest, let-body,
reserve-body) and, on a tail call, reuses the frame and re-enters the callee body in the SAME C frame -> O(1)
C stack for tail-recursive folds/scans/loops. `fk_offer_ack` is a no-op unless observe-mode, so looping past it
is safe.

## Why it did NOT land this pass (the honest blocker)

The core evaluator has **evolved past a single call path.** Routing the obvious tag-12 handler to the trampoline
left it **never invoked** (`fk_walk_body` entries = 0 for a recursive run): the ACTIVE recursion goes through a
*different* tag-12 handler — an `f44` path that does argument **combination/currying** (`carg44 = comb44`) and
wraps the result in `fk_offer_ack(f44, 1, ...)`. A naive trampoline that only handles the plain call is bypassed;
the wall barely moved (~60 -> ~125, i.e. the trampoline frame recursing, not looping).

A correct TCO must thread through ALL call paths and preserve the combination + offer-ack semantics — that is a
careful, dedicated change to the shared evaluator with full re-validation of the 80 observing recipes, the offer
protocol, and the table path. Rushing it could have regressed a working system, so the incomplete attempt was
**reverted to clean main** (native-vs-rented 11111, acc(50) 1275 intact). Naming the gap is the honest move here,
not shipping a half-engaged trampoline.

## The lane, scoped for next time

1. Map the evaluator's call paths (plain tag-12, the `f44` combination path, tag-7 self, tag-240) and the offer-ack
   boundary, so the trampoline covers every one.
2. `fk_walk_body` loops tail position + reuses the frame; the caller restores `fk_vsp` to the frame base.
3. Re-validate exhaustively (the 80 recipes, offer cells, the GPU, the numeric table) before merge.
4. North star remains `form-asm` lowering (recipe -> native asm, tiny frames): the same recursion->loop transform,
   in the SHIPPED native path rather than the bootstrap walker.

Non-tail recursion (fac) is a separate, smaller residual: it genuinely needs depth (a heap-stack evaluator or
native lowering), and most deep recipes are tail-recursive, so the TCO is the high-value first move.
