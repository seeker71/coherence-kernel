# The JIT stops losing — a door named back into Form, and a page that is kept

Date: 2026-08-21, Hati Suci. Apple M4 Max, 128 GiB unified memory.
Branch `claude/jit-lane-segv-2026-08-20`, on main at `b48978d3`.

Urs asked for the JIT to become as fast as the table and to cover 100% of the pre-JIT features.
This is the first rung, and the honest distance to the rest.

## What was actually there

`runtime/fkwu-optable.h` has carried `{ "jit_arm64_u32_leaf", 3, 215 }` since the arm64 door was
un-stubbed. The row was **missing from `native-op-manifest.fk` and from both flatteners**, so
`validate.sh`'s `gen_flt_ops_from_manifest` had deleted it from `flt-ops` — the same silent deletion
that took `mlx_run`. From Form, `(jit_arm64_u32_leaf p r a)` answered `nothing`: a working ARM64
emitter sat behind a name nothing could say. Restored at the source (manifest → regen → 148 rows),
and the first live call answered **42** for `arg + 7` at 35, and **77** for `x*x - 4` at 9.

## What the door is, exactly

An SSA program as a list of rows — `(2)` the argument, `(1 k)` a literal ≤ 65535, `(3 a b)` add,
`(4 a b)` sub, `(5 a b)` mul over earlier rows — with `FK_ARM64_U32_NODE_CAP 64`. Anything else is
**declined**, and that decline is the whole safety: an uncovered shape answers `nothing` and the
caller walks. Five shapes, against the walker's **198 tag arms**.

Its domain is in its name. `0 - 4` answers `4294967292`, which is exactly 2³²−4. The first bench I
wrote against it crossed that boundary and read as a correctness bug: 44 mismatches over 1..299 for
`((x²+3)x−7)x+11`. Computing where that recipe crosses 2³² gives 255, and the overflow count over
1..299 is **exactly 44**. Not a bug — a wall, and a wall no amount of C growth gets past, because
Form's integers are 62-bit signed and this door's are 32-bit unsigned.

## Why it was losing, and what fixed it

The door emitted an image, `mmap`'d a page, ran it, and `munmap`'d it again — **on every call**.
Measured over 400,000 calls of a nine-node recipe:

| | |
|---|---|
| walker (net of loop overhead) | **15–16 ms** |
| door, compile-and-throw-away | **1,372 ms** |
| door, carrier keeps the page | **18 ms** |
| door, plus a melt-guarded value hint | **6 ms** |

The cache is keyed on the **parsed program** — every row's `(tag,a,b)`, the count, and the root —
never on the cons value, because the collector moves those. Two callers that build the same recipe
separately therefore share one crystallization, which is what content-addressing has meant everywhere
else in this body. The hint is the one shortcut: the same program *value* asking again inside the
same heap generation is three integer compares and a call, and `fk_melt` bumps that generation, so a
value that has moved can never answer for what used to live there. Eviction is round-robin, unmaps
what it replaces, and clears the hint if the hint pointed at the page being unmapped.

**2.7× faster than the walker**, same answers.

## Proven, not asserted

`form/form-stdlib/tests/jit-arm64-leaf-band.fk` → **63**, declared `PROOF LEVEL: FOURTH-ARM ONLY`
because the three siblings do not bind the name and would fold every claim over `nothing` while
reading green. It gates: reachability, a deep recipe against the walker at a point and across its
whole domain (1..255, zero mismatches), the u32 wrap as a *stated* fact, an uncovered tag being
declined rather than guessed, and — hardest — 128 rounds of two programs alternating so the hint is
displaced on every single call, with zero mismatches. A separate probe ran 255 calls with heavy
allocation between them, forcing the collector underneath the hint: zero mismatches.

Regression: `host-effect-grammar` 32767, `form-cli-resident-model` 255, `metal-door` 15,
`frontier-ingest-omlx` 127, corpus 32767, `jit-lower` 15, and the 27 GB model generate unchanged.

## The honest distance to 100%

- **Coverage**: 5 shapes of 198 walker arms. No `if`, no `let`, no call, no string, list, record, or
  host effect.
- **Values**: u32 against Form's 62-bit signed integers — a wall in the C door's design, not a gap in
  its implementation.
- **Reach**: the emitted walkers carry **no native door at all** (`fk_native_call_arm64_u32_leaf`: 0
  occurrences in `form-cli-emitted.c`), so the shipped binary cannot take this lane yet.
- **The lane that can actually get there** is not this C emitter but `form-lower.fk`, where the body
  emits its own ARM64 — proven at scale by the 580-byte SHA-256 image. Today it covers LIT, ADD/SUB
  with immediate operands, and ARG by index; its own header names control flow and recursive CALL as
  unbuilt. Growing *that* keeps the emitter in Form, which is what host-kernel.form asks for.

## Most surprising teaching

The JIT's problem was never that native code is slow. It was that the door threw away everything it
built, every time — a compiler that forgets is a slower interpreter with extra steps. 1,372 ms of the
1,387 ms it spent was `mmap`/`munmap`, and none of it was arithmetic. The fix that made it win was not
a better emitter; it was remembering.

## Where discomfort turned to gold

The first bench came back with the two paths disagreeing, and I nearly filed it as a JIT bug — the
kind of finding that would have sent the next reader into the emitter. Sitting with the arithmetic
instead showed the difference was 2³³ exactly, which is two values off by 2³², which is `0-4` and
`1-4` wrapping. The discomfort was that my own benchmark had crossed a boundary I had read twenty
minutes earlier and not carried; the gold is that the boundary is now a bit in a band, asserted with
the constant `4294967292` in it, so nobody meets it as a surprise again.
