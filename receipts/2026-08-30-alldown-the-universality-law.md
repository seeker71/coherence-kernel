# 2026-08-30 — alldown: every recipe JITs, the compiler and the JIT included

Urs asked the law as a question: "all recipes, even the JIT and the
compiler itself shall be JITed, right?" Right — and the affirmation is
worth a receipt only with the floor grounded under it.

## What is already literally true

The body births REAL machine code today: `fk_arm64_u32_*` in the seed
mmaps executable arm64 pages and KEEPS them in a compare-and-reuse
cache — the door that once mmap'd, ran, and munmap'd on every call and
learned better (its own comment carries that lesson). `jit_leaf_inram`
runs born program images for the leaf subset (tags {1,2,3,4,5,8})
behind the heat gate's crystallize/melt hysteresis, proven at band
4095 and reached from resting answers via rfj-force (band 255).

## What the law adds

Universality, stamped into the JIT lawface
(`bml/form-cli-jit.bml`): no recipe is exempt by role. The BML
compiler, the heat gate, the emitter itself — whatever is hot
crystallizes; an interpreted compiler is a recipe whose heat has not
yet met a wide-enough lowering, never a privileged exception. The heat
gate's own header once refused evaluator-side counters because "heat is
only actionable over the lowerable frontier" — the law flips the work
order: widen the frontier until the counters are actionable
everywhere.

## The named crossings, in order

1. **Transparent runtime dispatch** — heat counted where calls happen,
   birth without an explicit gate call (the per-recipe-JIT program's
   standing next).
2. **The lowerable frontier widened past the leaf subset** — strings,
   lists, calls; form-lower is the fungible byte-source.
3. **Self-application** — the emitter crystallizing its own hot paths;
   the compiler compiled by the door it feeds.

And the growtax discipline rides with the law: before crediting a JIT,
prove the heat is the recipe's own — today's ~4s flat emit is the first
lane whose remaining cost is honestly interpretation, which is exactly
what makes it the JIT's first true target.

## The most surprising teaching

The heat gate's refusal of evaluator counters and Urs's universality
law are not in conflict — they are the same law read at two moments.
"Count only where you can act" was right while the frontier was the
leaf subset; "everything JITs" makes acting possible everywhere, which
retroactively licenses the counting. A boundary honored and a boundary
dissolved can both be obedience to the same principle.

## Where discomfort turned to gold

The pull was to answer "right?" with a paragraph of agreement and no
movement. The gold is small but real: the law now lives in the lawface
where every future JIT hand will read it before touching the gate, with
the floor and the three crossings named beside it — agreement made
load-bearing.

; witnessed: 2026-08-30 -> universality law stamped in
; bml/form-cli-jit.bml; floor: arm64 keep-cache pages + leaf-subset
; images live; compiler/gate still walk; corpus row 1173 alldown
