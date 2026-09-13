# Nine digits a limb

2026-09-13, midday, M4 Max, Hati Suci. float-repr landed on main as epic-edison wrote it: Dragon4 over a bignum
kit that held one decimal digit per limb. Two kernel rungs had already made its orchestration crystallize and its
calls cheaper, and still 5e-324 took about 7 ms. This rung changes the width of the data. The recipe's code
paths stay as they were.

## Carried

- **The kit carries base-10^9 limbs, least significant first.** bn-of-go, bn-mul-go, bn-carry and bn-borrow take
  10^9 where they took 10. The header now says what a bn value is: 1024 is (1024), and 10^9 + 5 is (5 1). Every
  limb product stays under 2^53, so each kernel carries it in plain integers. Dragon4's decimal digits are still
  its quotients, so nothing downstream changed.
- **Powers of two go twenty-three bits a step.** bn-pow2-mul multiplies by 2^23 until fewer than 23 bits remain,
  then by bn-two-to of the rest. 2^1075 takes 47 multiplications where it took 1075.
- **float-repr-edges-band reads 31 (FOURTH-ARM ONLY), registered.** It covers the edges the four-way band leaves
  out:
  - every 2^k for k in [-1074, 1023], and 3, 5 and 7 · 2^k;
  - the subnormals, each alone and one unit up;
  - 10^k at both ends;
  - the halfgap, the largest double and the smallest subnormal, by name.
- **The form-cli bootstrap follows** (stamp 88866976a143b6da, 5391 functions).
- **widelimb is row 1514.**

Witnessed:
- **Sweep on fkwu:** 0 mismatches against value_str on about 10,300 values in 4.1 s: every power of two, the odd
  multiples, 122 subnormals, 632 powers of ten and 1,200 LCG doubles.
- **Four-way:** float-repr-band reads 63 (go=0, rust=0, typescript=0; 1 ok, 0 divergent).
- **Timing:** one process per case, second run of each, host load about 7. value_str's native takes 0.01 to
  0.02 s for the same work.

  | float-repr of | decimal limbs | 10^9 limbs |
  |---|---|---|
  | 0.1 (5000) | 0.06 s | 0.04 s |
  | the tie (5000) | 0.45 s | 0.20 s |
  | 123456.789 (5000) | 0.31 s | 0.13 s |
  | 1e-300 (200) | 1.45 s | 0.16 s |
  | 5e-324 (200) | 1.55 s | 0.19 s |
  | the largest double (200) | 1.48 s | 0.15 s |
- **Bands:**
  - Every lane band reads as registered, except loop-lane-template (see below).
  - value-str 1023, str-to-int-reading 127, once-hold 15.
- **Before landing:** TestFkwu; freshness 31, the corpus band 32767, drift 16383 of 16383, porcelain 0.

## Still open, measured

- **loop-lane-template-band's bit 32 is intermittent.** It asks for three signatures of one defn, all native at
  once. It read 63 on some runs and 31 on others: roughly half of six runs on each of three kernels (d5099abf1,
  dd59ab024 and this one) read 31. So it predates today's rungs, and it is the next thing to take to its root.
- **The common case is still Dragon4's per-digit bignum work.** The tie costs about 36 µs; the native costs about
  3. A 64-bit fast path (Grisu or Ryu in shape) for the digits a double can hold is the rung for the common case.
- **d4-scale still steps once per decade,** about 300 steps for 1e-300. Estimating k from the binary exponent
  would take a single multiplication by 10^k.
- The chain-shaped orchestration (fr2) is worth about 13% on the tie. It goes in when a rung needs it.
- epic-edison's guard rungs (str_eq, depth wall, kind-38 callee) are under way.
- The open items of receipts 12 to 52 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was how little the change took. The base constant in four defns, plus one loop, took the extreme exponents from about
7 ms to under 1. For 5e-324 the lists were 324 limbs long; now they are 36, and the doublings went from 1075 to
47. Warming every orchestration loop had bought 13%.

The discomfort was that epic-edison and I both first named the walking orchestration as the cost. Two kernel
rungs made it crystallize, and the common case moved by little. The measurements pointed at the width of the
data, and the gold is that the kit took the change in one constant, written in four places.

Frontier word, row 1514: **widelimb**, a bignum limb as wide as exact integer arithmetic allows (here nine decimal
digits, since every limb product stays under 2^53).

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
