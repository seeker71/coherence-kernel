# The float path needs no dtoa native

2026-09-13 (WITA). value_str's float rendering — the shortest round-trip digits and their layout — was
held on a C primitive (float_sci), framed as a Table-Maker's-Dilemma dtoa that "must stay in C." Today
the whole path is pure BML: Dragon4 over the exact m·2^e, on a decimal-bignum toolkit, with float-repr's
own layout kept. Witnessed byte-identical to Go's `strconv` 'g' on 10180 floats — every power of two
2^k for k in [-260,260] plus 3/5/7·2^k, all powers of ten, six thousand random doubles across both the
fixed and scientific regimes, subnormals down to 2^-1074, and the 147142857142857.125 tie. Zero
mismatches, and none where float_sci itself was tested as the oracle either — because float_sci was not
the oracle it was taken to be.

## The most surprising teaching

The C native I was building the recipe ON TOP OF was the wrong one. float_sci takes the correctly-rounded
digits at each length and asks strtod whether they read back; at a power of two the gap below is half the
gap above, and there the rounded digits can miss while a neighbour one unit away reads back — so
float_sci wrote 2^-24 with seventeen digits where Go and JS write sixteen. The BML Dragon4, which I had
assumed would need a primitive to be trusted, gets it right with no native at all: the Burger-Dybvig
interval and round-half-to-even ARE the principled form of the C writer's later "try the last-digit
neighbours" patch (df91d6f46, already on main for the mode-25 writer). The thing framed as irreducibly-C
was the flawed half; the thing framed as needing-C needed nothing.

## Where discomfort turned to gold

The sweep went red on three cases — my d4 disagreeing with float_sci on 2^-24, 2^-44, 2^-77. The easy
read was "d4 is wrong, the C native is the oracle." The discomfort was refusing that read while not yet
knowing which was right. Followed to the source — str_to_float round-trip first, then Go's strconv run
directly from its own stdlib — the red turned out to be float_sci's, not d4's. The gold: the BML path
does not merely replace the C native, it is more correct, and it dissolves the held item rather than
landing it.

A second discomfort, quieter: I nearly carried "float_sci is buggy" to the lander as a discovery. Grounding
on the current main first showed the halfgap class was already named (row 1510) and already healed in the
C writer. Ground before asserting kept stale-branch news from being dressed as new.

## The performance, measured (a second surprise, and a trap)

Correctness was witnessed; then the cold/warm signal the goal names got read. The C native (snprintf
"%.*e" + strtod search, not bignum) is fast: 1.07 us for 0.1, 3.39 for the 17-digit tie, 4.46 for
1.23e200. The BML recipe WALKED is 100-350x slower (113 us / 1171 us) — cold because the bn-* are written
NON-TAIL, `(cons d (bn-recurse))`, which the lane declines.

The trap: timing BML and the C native in the SAME process reported a flat 54 ms/call for BOTH — a false
parity. BML's bignum consing fills the pair pool, and melt is O(field), so every call in that process —
the C native included — paid the melt tax. Isolated in its own empty-field run, the C native is
microseconds and the BML penalty is real. A benchmark that shares a field with a heavy conser measures
the field, not the code. (Memory already carried the neighbour of this: "measure references quiet.")

The gold under it: reshaping bn-* to the tail-loop-onto-accumulator shape — cons onto an acc, reverse at
the end, the same reshape map/filter/reductions took — CRYSTALLIZES. bn-mul-t and rev-go go loop-native
(state 2), and minimal dual-list loops with it, at zero parity cost. The warm O(1)-block path the goal
asks for is reachable, not hypothetical. One holdout remains, localized: the tail form of bignum ADD
declines where MUL takes, and the obvious cause (two list cursors) is disproven — it is bn-add's own
arm/carry shape, an emitter question handed to the lane's author with the diagnosis, not the guess.

## Warm, achieved for the leaves

The full path was then reshaped with tail-crystallizable bignum leaves and timed. Every leaf loop
crystallized (zipsum, carry-t, zipdiff, borrow-t, cmp-t, bn-mul-t, rev-go — all loop-native), correctness
held (0 of 10180), and it ran 7-8x faster than the cold recipe: float-repr(0.1) 113 us -> 14 us,
float-repr(tie) 1171 us -> 171 us. The teaching in it: the orchestration loops (d4-loop, d4-scale, d4-dig)
STILL walk — their self-args are bn-* calls, rung-10's limitation — yet the 7-8x arrived anyway, because
a walking orchestration dispatches each iteration into the native leaf pages. Most of the warm win does
not wait on the hard rung; it comes from crystallizing the leaves that do the O(n) work.

## Then the radix — a bounded win, attempted not just named

The witnessed residual said two costs: limb count (extreme exponents) and per-digit work. The bignums
carried one decimal digit per limb; base-10^9 limbs (safe in 63-bit, since Dragon4 only multiplies by
<=10) cut the limb count ~9x. Delivered (scratchpad/warm3.fk, still 0 of 10180, leaves still loop-native):
0.1 15->11 us, tie 169->114 us, 1e-300 6570->1281 us (5.1x), 123456.789 104->69 us. Cumulative from cold ~10x.

## The wall behind the wall, and a claim I had to withdraw

I had written that the common-case residual was the walking orchestration, an emitter rung. lucid-lehmann
read the kernel and found the real wall — two page-bounded caps (calls per leaf 8, words per leaf 1000)
that bn-sub blew once the inliner took its callees — and landed a rung raising them (dd59ab024: words
4096, calls 32, overflow sites 64; row 1511 wallbehind). On that kernel the orchestration DOES crystallize:
every bn wrapper reads state 1, every bn loop reads 2, d4-dig reads 2. So my "call-arg emitter rung" was
wrong twice over — it was a capacity wall, and lucid-lehmann took it.

Then the harder correction, witnessed: crystallizing the orchestration bought NOTHING. float-repr(0.1) is
17 us and the tie 179 us with d4-dig and d4-scale warm — the same as the do-form's 14/171. The common-case
cost was never the orchestration walk; it is the leaf bignum work plus melt, which was already warm, and
the orchestration loops run too few passes to matter. The optimization signal I trusted pointed at the
wrong rung; only running it with the wall removed showed that. That is the discomfort of this receipt:
a clean-sounding attribution, followed to root, was simply not where the time went.

## The honest edge, as it stands at landing (dd59ab024)

Correctness is complete and witnessed: == Go 'g' on 10180 doubles offline, and float-repr-band reads 63
four-way (fkwu, Go, Rust, TS agree on every sample) over powers of two and 3/5/7*2^k, powers of ten, the
tie, negative zero, named cases and a random set; the extreme subnormals and the 2^-1017 halfgap the band
leaves to fkwu and the offline sweep, because the recipe walks on the sibling interpreters and those cases
are bignum-heavy.
The landed recipe keeps the house do-form, base-10; its bn leaves and wrappers and d4-dig warm on
dd59ab024, and it runs ~10x faster warm than cold. What still walks is d4-loop (non-tail, about 15 slots
against the lane's 6) and d4-scale — and warming them, we now know, would not move the number. The gap to
the C native is the algorithm and the allocator, not warmth: the levers that actually move it are base-10^9
limbs (5.1x on extreme exponents, scratchpad/warm3.fk) and then a 64-bit fast path or Ryu/Schubfach for the
last stretch. value_str's native stays while that gap holds; each is its own later landing, the lander's
and Urs's call. Recipes: scratchpad/recipe.fk (cold), warm.fk (warm leaves), warm3.fk (base-10^9).
