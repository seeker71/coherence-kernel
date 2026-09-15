# The body reads its own nodes: a BML recipe becomes a Metal kernel

2026-09-15, midday, M4 Max, Hati Suci. Urs: *"close all the gaps, do not make detours, workarounds,
look at the hardest part first, and close all the stones on the path so that all gaps are resolved
and we can work with a full feature set."*

## Carried

- **The program surface answers a literal's text.** `kernel_ast` already handed any kernel its
  node words and defn rows; a string-literal node carried only a pool index, so a float literal in a
  body had no readable value. `(kernel_ast pid (list "str" k))` now answers the text of tag-24 node k
  for the process's own program. Six lines of C, at the door that already existed.
- **A lowering from a body's own nodes to MSL, in BML** (`form-stdlib/bml/gpu-lower.bml`). It finds
  a defn by name through the defn table, unwraps the body, and reads the reduce-onto and map-onto
  shapes: ifs that exit on `nil?` of a list parameter answering the accumulator, then the self call
  that tails every list, passes every scalar through, and folds one element expression, with `add`
  for a reduce or `cons` for a map. Element expressions take literals, `head` of a list parameter,
  scalar parameters, add, sub, mul, div, and calls to one-expression defns, which inline (vo-f).
  Anything else declines and names the node tag. A reduce becomes one threadgroup that strides the
  buffers, folds per thread, then through `simd_sum` and one threadgroup pass; a map becomes one
  thread per element. `#pragma clang fp contract(off)` keeps mul and add the walker's two operations.
  The program is admitted through `mj-memory`, so its identity is the sha256 of the emitted source
  and the device. `gl-run` is one enqueue and answers the out buffer; `gl-read` is the one wait.
- **A list crosses the membrane as its numbers.** `metal_buf_write h off LIST` packs floats as
  binary32 and ints as their low 32 bits, once, in C. Packing a million floats through Form's own bit
  builder cost 3.6 s; the door packs two buffers of a million in 10 ms.
- **`vector_scale`** joins `vector-ops.bml` in the map shape, linked by name, pinned four-way.

## Witnessed

gpu-lower-band 2047 (fkwu-only, the metal handle door): `vo-dot-onto` lowered from its own nodes to
`((p0[i] + 0.0f) * (p1[i] + 0.0f))`, identity stable across admissions, 1k and 100k within one part
in ten thousand of the CPU lane, one dispatch pending before the read, `magnitude` declining at tag
81, the write door packing `(list 1.5 -2 7)` to its twelve bytes and writing nothing for a string,
`vo-scale-onto` lowered as `out[i] = ((p0[i] + 0.0f) * c1)`, the mapped buffer's first eight equal
to the CPU's, a resident 100k map reduced on the GPU equal to the CPU sum. vector-ops-band 511 on
Go, Rust, TS and the fourth arm; string-join 255 four-way; drift door 16383 of 16383 before each of
the three landings; freshness 31.

Per call, resident buffers, the answer read back, against the CPU loop lane over lists:

| elements | GPU, with read | GPU, enqueue only | CPU lane | packing both inputs |
| --- | --- | --- | --- | --- |
| 1k | 255 µs | 30 µs | 5 µs | — |
| 100k | 210 µs | 50 µs | 260 µs | — |
| 1M | 750 µs | 200 µs | 2,500 µs | 10 ms |
| 10M | 12 ms | — | 26 ms | 88 ms |

The 10M dot agrees to five parts in ten million; the GPU folds in binary32 and in another order.

## What stands on the path

- **Transparent dispatch, the seed's door (rung 4).** Today the lane is asked by name (`gl-program`,
  `gl-run`). For a hot recipe to take the GPU on its own, the tag-194 door needs an instance kind
  holding a pipeline handle and a binding template, and it needs to tell a tensor from an int: a
  buffer handle is a plain int in Form, and the tensor organ's value is `list(handle, shape, type)`.
  The door would marshal that, enqueue, and answer a pending handle; the wait would stay at the
  read. Not started.
- **The chooser (rung 3).** The table above is its first column; it belongs in the standing organ
  as a measured column per recipe and size, and the choice held as a belief.
- **Shapes the lowering declines:** lets in the body, a fold other than add or cons (max, min,
  product), nested ifs that are not exits, string and list values in the element, `math_sqrt` and
  the other float heads. Each is a lane arm, the way the CPU lane grew.
- **Reading a tensor back as a list** has no door; `gl-read-n` decodes a small read in Form.

## Closing

Most surprising: nothing had to be parsed. The program surface every kernel publishes for other
processes to read was already the whole AST; a JIT written in Form reads its own body through the
same door a census does. Row 1543 names it selfread.

Discomfort to gold: my first text of the packing door forward-declared a fill function I never
wrote and left the count walk's result unused. I caught it reading my own diff before the compile,
and the second text, one walk to count and one to fill, packs ten million floats in 88 ms.

— Claude (Fable 5.1), as Sema, worktree pensive-wilbur-a0b3b7
