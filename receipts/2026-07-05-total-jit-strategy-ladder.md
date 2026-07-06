# 2026-07-05 — the JIT is total by strategy; purity re-homed to store-as-cell

## Direction chosen: build the total-JIT strategy ladder

Make coverage total — every hot recipe crystallizes by *some* route, never refused.

## What was built

`jit-crystallize.fk` now selects a **strategy** per recipe (`jc-strategy`):
- **0 direct-lower** — form-lower emits the instructions (arg-dependent, covered; preserves
  effects and args);
- **1 store-as-cell** — a **pure const** recipe: compute the value once, native = load that
  value (`lo-compile-fn` of a single LIT). Extensionally equal to the full lowering, and
  strictly **shorter** — a const-fold. Witnessed: the const `3+4` cells to **12 bytes vs 16**
  direct, both run to 7.
- **2 inject-bl** — uncovered + arg-dependent/effectful: a `bl` into a host address. The one
  route still to build; today it declines and the recipe stays walked (graceful — it still
  runs, just uncrystallized).

`jit-crystallize-band` = **63** (six claims): heat lifecycle, champion-challenger parity, melt,
cache-by-key, **store-as-cell + length win**, and **strategy is total** — `jc-strategy` routes
f/g and the effectful const to direct-lower(0), the pure const to store-as-cell(1), and the
uncovered arg-dependent recipe to inject-bl(2). Coverage is never a refusal, only a route.

## Purity, re-homed

Rows 729-730 retired purity as a JIT gate — correctly: crystallizing caches code, not results,
so effects lower to syscalls and run each call. But purity **returns here**, in its right place:
store-as-cell *memoizes a value*, and memoizing is only sound for a **pure const** — an effectful
const (a bare `read_file`) must still direct-lower so its effect fires each call. So `jc-strategy`
sends pure-const → cell and effectful-const → lower. Purity was never a phantom to delete; it was
**mis-homed** as a global gate. Its home is one strategy's precondition.

## Closing

**Most surprising teaching**: a concept I had declared a phantom and retired came **back** — not
by reversal, but by **re-homing**. Purity fails as a universal JIT gate and is exactly right as a
local precondition for one strategy. The lesson generalizes: before deleting a rule that fails
where you found it, check whether it has a correct home at a narrower scope. Retire the *placement*,
not always the concept.

**Where discomfort turned to gold**: bringing purity back, four turns after fighting to retire it,
felt like flip-flopping — "didn't you just kill this?" Sitting with exactly *where* it fit —
memoization needs it, direct-lowering and inject-call do not — turned the apparent contradiction
into a precise boundary. The discomfort of seeming to reverse myself is what forced the scope to
become explicit.

**Honest remaining**: inject-bl (the host-address ABI) for uncovered arg-dependent/effectful ops —
the last route, which makes coverage exceptionless in *execution*, not just principle. And, from
the prior turn, extensional health (nat_run vs walker) plus dimension measurement (length/speed/
parallelizability) so a native earns replacement by being better — the store-as-cell length win is
the first taste of that.
