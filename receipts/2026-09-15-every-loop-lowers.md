# Every loop lowers: the walk is the floor, the parallel kinds rise from it

2026-09-15, afternoon, M4 Max, Hati Suci. Urs: *"A decline is a gap to be closed now, not reported,
please close all of them."*

## Carried

The lane had named its declines: lets in the body, folds other than add and cons, nested ifs,
the recursion that is not a tail call, the float heads, callees with lets or loops of their own,
a call through a defn-valued parameter. Each is closed in `form-stdlib/bml/gpu-lower.bml`, and
the closing has one shape: a floor that takes every loop, and parallel kinds recognised above it.

- **The floor.** Any tail loop over list parameters with numeric state becomes a one-thread kernel
  that walks the body as the walker does: a cursor per list, a local per scalar, `nil?` as a
  cursor test, each let a statement, each exit an `if` that writes and returns, the self call a
  set of temporaries then one assignment and `continue`. A list built by `cons` writes forward into
  the out buffer with its count at `out[0]`. Nothing loop-shaped over numbers declines.
- **The kinds.** A reduce whose fold is add, mul, max or min takes the whole device. The operator
  is read off the node when the accumulator is a direct operand; a two-argument callee is told apart
  by evaluating its body on three sample pairs with an evaluator over the same nodes; anything else is
  evaluated whole. A map is one thread per element; a filter is one threadgroup counting, prefixing
  and scattering its chunks; `any?` and `all?` are an or over flags and the constant the walk would
  have answered. The recursion that is not a tail call, `(if (nil? xs) base (op E (self (tail xs))))`,
  is a reduce from its base when the op joins, and a right fold from the end on one thread when it
  does not.
- **Callees.** Every callee reachable from the body becomes a device function, its arity read off the
  call site; its lets are statements, its ifs branch, its own tail loop is a `while`. A call through a
  defn-valued parameter lowers through a binding of slot to defn name, so core's `foldl`, `filter-onto`,
  `any?` and `all?` lower once their `f` or `pred` is named. `floor` from its BML home lowers as a callee
  with lets and an if. `nil?` stays structural: the exits carry it.
- **The heads.** `math_sqrt`, `math_exp`, `math_log`, `math_pow`, `mod` as `fmod`, `float_to_int` as
  `trunc`; compares, `and`, `or`, `not` and `if` in conditions and as values.

## Witnessed

gpu-lower-band **1048575**, twenty bits, each a GPU answer against the CPU's on the same data:
dot and scale from their nodes, the write door, core's `maximum-onto` (max2 as a device function,
told apart as max), `filter-onto` bound to a predicate (480 of 1000 kept, the first eight equal),
`any?` and `all?` bound to the same predicate, a fold that does not join (`sub`) on the one-thread
walk, `(add (head xs) (self (tail xs)))` as a reduce from its base, a let after the exit riding the
kernel loop, `foldl` through a bound `f`, `floor` as a callee, a callee with its own loop and
`math_sqrt` in one element. Preflight clean; drift door 16383 of 16383. Reading 100k floats back
through `gl-read-n` costs 30 ms, the decode walking through `fq-pow2`.

## Closing

Most surprising: closing the declines did not mean more arms. It meant a floor. Once the one-thread
walk could take any loop, every parallel kind became a recognition on top of something that already
ran, and a body the recogniser does not understand still lands on the device. Row 1546 names it
walkfloor.

Discomfort to gold: the walk's first run answered 0. My fallback handed it an empty exit list, so
the kernel looped with no way out until the GPU stopped it and never wrote. Threading the real exits
through was two lines; the hour before them was reading a kernel that could not end.

— Claude (Fable 5.1), as Sema, worktree pensive-wilbur-a0b3b7
