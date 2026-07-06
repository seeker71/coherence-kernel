# 2026-07-05 — JIT health is extensional; JIT coverage is total by strategy

## "where does byte-identical come from, to what, why?" · "why would jc-lowerable ever be false?"

Two more gates I built, dissolved. Grounded.

## Byte-identity: to clang, and it is a ceiling the body already lifts

`form-lower-band.fk:5-7,19`: the conviction gate checks form-lower's bytes **are clang's,
byte-for-byte**, and only then "clang may be dropped for this program." So it is
correctness-by-oracle-matching — trust our lowering because it reproduces clang's exact bytes.

Over-constrained, and the body knows it. `form-lower-sim-band.fk:3-9` names clang byte-identity
a **"ceiling"** and lifts it: fsim validates a lowering by **EXECUTION** — it runs the sequence
and reproduces the function — "**WITHOUT renting clang's whole-function choices**." Byte-identity
is kept only where it is truly necessary: **per-instruction**, where an ARM instruction has one
true ISA encoding (that is the ISA, via form-asm — not clang). Per-**function** (register
allocation, instruction selection, ordering) is judged by running.

`nat_run` (built this session) makes that stronger than fsim: **hardware execution** is the
ultimate correctness oracle — run the native, match the walker. So health should be
**EXTENSIONAL** (same result, any working asm) not **intensional** (same bytes). clang and other
oracles become **benchmarks** to learn from on length / speed / parallelizability, not a template
to match — freeing the JIT to emit any working asm and *improve* it. (Honest gap: no length/speed/
parallel measurement exists in jit-tier-policy / jit-profile / the valuation ledger yet — to build.)

## jc-lowerable: another phantom; the JIT is total by strategy

It should never be false. The JIT is **total**:
- covered op → direct-lower (form-lower emits instructions);
- uncovered host op → inject a `bl` into a host address;
- const / pure recipe → **store-as-cell** — `cell-log-store.fk` is a content-addressed cell store
  (`record_new` + `make_nodeid`); compute once, native = load the cached value.

Even a recipe computing a const is JIT'able that way. So `jc-lowerable?` is not a JIT-eligibility
gate — it asks only "is *direct-lowering* available today," a shrinking coverage frontier. "Can we
JIT this recipe" is always **yes**; only the strategy varies. Corrected the cell's comments to say
so (logic unchanged; band still 31).

## Closing

**Most surprising teaching**: the JIT's correctness criterion should be **extensional** — judged
by what the native computes, not by the form of its bytes. Every gate I manufactured this session
was *intensional* thinking: purity (the recipe's form must have no effects), coverage (its ops must
match form-lower's set), byte-identity (its bytes must match clang's). Each demanded a specific
FORM where the architecture only needs the right BEHAVIOR, by any means. Extensional health
dissolves all three: run it, does it match?

**Where discomfort turned to gold**: four corrections in a row, and this one caught me
manufacturing a fifth gate (coverage) the same turn I retired the fourth (purity). "Why would
jc-lowerable ever be false?" named the *reflex*, not just the instance — I reach for a "can't"
where there is only a "how." Grounding that even a const stores as a cell is what let me see the
pattern instead of patching one more hole.

**Honest remaining**: the strategy ladder (inject-call, store-as-cell) that makes coverage total;
execution-based health (nat_run vs walker) replacing byte-identity conviction; and the dimension
measurement (length/speed/parallelizability vs oracles) that lets a native EARN replacement by
being better — not merely by matching clang.
