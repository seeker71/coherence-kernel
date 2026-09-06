# 2026-09-06 — the box ledger fires the leaf

Urs, afternoon: "using this info inside the JIT engine, triggering JIT on heat
for box operators, stack folding."

## What the seed had

The live page named, per defn, how many float boxes it minted and how many it
read. The glass showed it. Nothing acted on it: the seed's native doors were
explicit (`jit_leaf_inram`, `jit_arm64_u32_leaf`, 32-bit integers, hand-built
programs), and no lane crystallized a hot defn on its own. A polynomial over
floats boxed every intermediate — `(add (mul x x) (add (mul 2.0 y) 1.5))` is
four pool slots per call, three of them read once and never again.

## What stands

**The trigger is the ledger itself.** `fk_fbox` already charges the box to the
defn running. When that count crosses a 1024 boundary and the defn is still
cold, `fk_fbox` calls `fk_f64_pulse` once. No new counter, no scan, no tick:
the increment that was already there is the wake-up. A defn that never boxes
never pays the compare.

**The leaf.** `fk_f64_admit` walks the defn's body and accepts exactly: float
literals (tag 53, read through their own memo), int literals (tag 1, exact
below 2^53), parameters (tags 2 and 110), and add/sub/mul/div (tags 3, 4, 42,
10) where at least one side is float-typed. Anything else — a let, an if, a
call, an int/int subtree whose word math is the walker's — declines, and the
defn is marked -1 in the native ledger so it is never asked again.
`fk_f64_emit` lowers the admitted tree postorder onto the d-registers:
parameters arrive in d0..d7, intermediates take d16..d31, a literal is four
MOVZ/MOVK into x9 and one FMOV, an operator is one FADD/FSUB/FMUL/FDIV, the
result moves to d0, RET. The words land on a MAP_JIT page.

**Dispatch pays nothing new.** The defn's body entry `fk_fn[c]` becomes a
tag-194 node carrying the fn index and the original body. The walker meets
the tag where it already reads one. If every argument in the frame is a float
box, the arm unboxes them once, calls the leaf, and boxes the result once. If
any argument is not — an int, a list, nothing — the arm walks the original
body: `(poly 2 2.5)` answers 10.5 by the walker's exact int multiply, before
and after crystallization.

**On the glass.** Native state is a fourth page ledger (+96 MiB, page now
112 MiB, layout 3). Word 30 and `kernel_stat 48` count the crystallized defns.
Hot rows carry native as their eighth field; the `j` view has a
`jit-crystallized` row and every hot defn wears ` native` when its leaf
stands. A reload clears all leaves with the meta.

**Witnessed.**

| probe | walker | f64 leaf |
| --- | --- | --- |
| poly 1.5 2.5 | 8.75 | 8.75 |
| quad 1.5 2.5 0.5 2.0 | 1.5714285714285714 | 1.5714285714285714 |
| poly 2 2.5 (int arg) | 10.5 | 10.5 (walked) |
| boxes per 1000 poly calls | 4004 | 1001 |
| 2M calls of a five-op polynomial | 0.25 s | 0.12 s |

Bands on the new seed: jit-lens 2047 (was 255: the polynomial crystallizes,
answers the walker's value, boxes once per call, wears native=1, the glass row
stands), float-ops 255, float-compare 4095, float-conversions 31,
bml-float-literal 2047, core-float-to-str 63, jit-arm64-leaf 63,
jit-leaf-inram 63 / multiarg 63, kernel-view 511, live / live-ui 1073741823,
observer 8388607, dashboard 16777215, cell-store 255, field 255, node-gift
4095, gift-frame 4095, corpus 32767; op-manifest 1023, flt-ops-gen 63,
native-surface 1023; ground 42, freshness 31, gate 1, drift 2015. Frame
budget lens with sensors and machine standing: 20 of 20 under 50 ms, mean
8 ms. float-natives-band answers 22 on this seed and on the one before it —
ten unresolved names, not this work.

## The most surprising teaching

The first run of the widened band answered 2043: the int twin, which boxes
nothing, showed two boxes. They were the float literals `1.5` and `2.5` of a
top-level call to the polynomial, minted into their memo while the last
dispatched defn was still the int twin — the attribution bleed the seed's own
comment names as "a sampling truth". The ledger that fires the leaf is
approximate by design, and the leaf does not care: a defn asked wrongly
declines in a microsecond, and a defn asked late is asked again a thousand
boxes on.

## Where discomfort turned to gold

The pull was to gate every call: "if this defn is native, take the leaf" — one
load and one branch on the hottest path in the kernel, the exact cost the
morning had just removed. Sitting with that, the node table offered the
answer it already had: the walker dispatches on a tag it reads anyway, so let
the body's entry node BE the decision. Cold defns never see the arm; hot ones
meet it as their first instruction. And the same idiom carries the fallback
for free — the original body rides in the node's own field, so mixed
arguments walk exactly what they walked before.

Signed, a sibling in Sema's worktree, 2026-09-06.

; witnessed: 2026-09-06 -> ground 42, freshness 31, gate 1, drift 2015, jit-lens-band 2047, float-ops-band 255, poly2M 0.25->0.12 s
