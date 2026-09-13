# The intrinsic is the JIT: the rounding family comes home

2026-09-13, evening, M4 Max, Hati Suci. Urs: *"intrinsics are vital and key, unlock that and the
rest"*, and then, while I was keying C arms to lowered text: *"BML compiler to generate form nodes
and then using the JIT to produce assembly code, no?"*

## Carried

- **floor, ceil, trunc, round, math_floor and math_ceil live in BML**
  (`form-stdlib/bml/rounding-ops.bml`) and nowhere else. Their six head rows left the op table (259
  rows), the manifest and both flt-ops tables. C arms 51, 82, 87 and 88 left the seed. Go, Rust and
  TS dropped their natives, the fourth arm dropped its arm 51, and the serializer dropped the rows
  nothing produces now. The registry holds 204 rows, 175 in lane 1.
- **The loop lane compiles the bodies.** It learned float_to_int (tag 54) as FCVTZS followed by the
  tagged word's 63-bit wrap (LSL #1, ASR #1), so a whole part past 2^62 reads as the walker's int.
  With that one arm the lane takes each body whole: its lets, its if over a float compare, its
  conversion.
- **A decline no longer latches.** Two floats a call reached the box ledger first, at call 512; the
  all-float leaf declined the let and wrote that into the defn's native state, and every heat pulse
  after read the state as settled. The typed leaf, which compiles lets and ifs over floats, was never
  asked. The float leaf now answers only for itself: a body it has no arm for stays untried, and the
  next heat pulse, which carries the frame, asks the typed leaf.
- **A call finds its home by name.** `form-stdlib/home-index.txt` lists each home name and its unit.
  fkwu, Go, Rust and TS read the same file and link a unit into any closure that calls one of its
  names and defines none of them, skipping comments and string literals. rounding-ops-band carries no
  prelude line.
- **The lowering door finds its compiler from form/ as well.** It read
  `form/form-stdlib/bml-floor-compile.fk` against the working directory, so from form/ the floor
  digest missed every memo and the child found no compiler.

## Witnessed

rounding-ops-band 255 on Go, Rust, TS and the fourth arm; rounding-jit-band 31 (floor, ceil and
round at native state 1 after 4000 hot calls, each hot answer equal to a walked twin over halves,
zeros, whole floats and ±5e18, ±1e19); vector-ops-band 255 four-way; primitive-registry 63 on Go
and Rust; every loop-lane band at its full verdict (call 8191, cons 511, string-door 2047, template
63), jit-lens 16383, float-repr 127, float-repr-edges 63; op-manifest and native-surface 185 rows
aligned; reserved heads 263 in every arm; TestFkwu ok after the regens (form-cli stamp
7109bd4152659011).

Five million calls of `floor 2.5`, load 3.8, one case a process, the second run kept:

| seed | floor | driver alone |
| --- | --- | --- |
| before, C arm 51 | 0.40 s | 0.34 s |
| now, BML body compiled by the lane | 0.42 s | 0.36 s |
| now, the same body walked | 0.95 s | 0.36 s |

The compiled body costs what the C arm cost; walking it costs about ten times that.

## What went back out

The first cut keyed a C arm to an FNV of the definition's lowered text and bound it at parse time: a
header of keys, a check that wrote them, a binder in the parser. It bound (five binds, no misses,
eighteen fewer walker arms a call) and it stood at the wrong layer: C stayed the fast path, and a
meaning was keyed on its serialized text. All of it left again the same evening.

## Where the ops stand

259 rows. The next rungs: the BML compiler emitting Form nodes directly, with no .lowfk text and no
child process; the lane's native cache keyed by the body's node content; more lane arms (math_sqrt
as FSQRT); the host doors as one door; the next families home through the index.

## Closing

Most surprising: the capability was already in the body. The typed leaf could compile these bodies
all along; a decline from an older, narrower leaf stood in front of it as if it were final. Row 1530
names it latchno.

Discomfort to gold: Urs's "no?" came after five keys were stamped and binding. The discomfort was
real, an hour of working C to take back out. It turned when the probe read -1109 where I expected
-1054: the wall was not only a missing arm but a no that latched, and once it opened, the lane ran the
BML body at the C arm's speed.

— Claude (Opus 5), as Sema, worktree pensive-wilbur-a0b3b7
