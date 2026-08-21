# Walking the JIT home — the Form lane closes, and the GPU lane learns matmul

Date: 2026-08-21, Hati Suci. Apple M4 Max, 128 GiB unified memory, MLX 0.32.0.
Branch `claude/jit-door-crystallize-2026-08-21`, on main at `8319e602`.
Follows [The JIT stops losing](2026-08-21-the-jit-stops-losing.md).

Urs asked to walk it home, then redirected mid-walk: **use the JIT Metal/MLX GPU on-demand generation
lane, not the flattening lane.** Both halves of that are here, and so is the distance still left.

## The Form lane closes

`form-lower.fk` has emitted arm64 leaf images for a long time — `inram-leaf-emit-band.fk` pins their
bytes. The **executor lived only on the Go sibling** (`jit_inram_darwin_arm64.go`'s
`jit_leaf_inram`), so fkwu could compile its own native code and had nowhere to put it. That half is
now in the runtime, with the contract Go states: `(image arg) -> int64`.

Form lowers `f(n) = n*3 + 7` to a **20-byte image** and fkwu runs it: f(5)=22, f(100)=307, agreeing
with the walker across 0..399. `tests/jit-leaf-inram-band.fk` → **63**.

This door matters more than the u32 one it stands beside. `jit_arm64_u32_leaf` is a C emitter over
five tags and 32-bit values — a wall in its *design*. Here **the emitter is Form**, so growing
coverage is growing a Form recipe, and the ABI is `int64 f(int64)`.

The 32-bit ceiling that remains is `form-lower`'s w-register choice — a Form change, not a kernel
one. Measured, not assumed: `f(2000000000)` answers 1705032711 where the walker says 6000000007,
exactly 2³² apart; and it bites at the bottom too, `g(0) = (0-4)*5` answering 4294967276 where the
walker says −20. The band's **first run read 47** because it straddled that bottom edge — which is
precisely how a wall gets mistaken for a cache fault. It now stays inside the domain and says where
the edge is.

## The GPU lane learns the op generation is made of

`mlx_linked=false` in the fkwu I had built by hand — the carrier was simply not linked, while
`libmlxc.dylib` sat installed. Rebuilt the documented way: **linked=true, metal=true, gpu=true,
device=gpu, MLX 0.32.0**, and `2 3 add` → 5 on the GPU.

Then the first token that a generation lane would actually spend its time in. Everything the IR had —
add, mul, sub, max, sum — is elementwise and could as well run on the CPU. **A matmul is the whole
cost of a forward pass.** Grown the carrier's own way — *"new shapes are new tokens in the program
(and a table row here), not new opcodes in fkwu-uni.c"* — with a two-dimensional shape `mRxC` and
`matmul` joining the binop table. `fkwu-uni.c` did not change.

```
m1x3 1 2 3  m3x1 4 5 6  matmul            -> 32      (1·4 + 2·5 + 3·6)
m2x2 1 2 3 4  m2x2 5 6 7 8  matmul sum    -> 134     ([[19,22],[43,50]])
v2 10 20 v2 1 2 add sum                   -> 33      (untouched)
```

`tests/mlx-matmul-band.fk` → **63**.

### Three things the GPU taught, all kept as bits

1. **MLX's matmul is floating point only** — *"Only inexact types are supported"*. So an `m` shape is
   FLOAT32 while `v` stays int: the tier is decided by the shape, not by the lanes, which are written
   as integers because a program is text.
2. **`mlx_array_item_int32` SUCCEEDS on a float32 array** and hands back the raw bits. A matvec that
   really computed 32.0 read as **1107296256**, which is `0x42000000`. Right arithmetic, wrong
   reading, and nothing in `mlx_status` would have said so. The door now asks the **dtype** before it
   reads, and bit 8 of the band stands on that constant so the bits can never pass as a value again.
3. **MLX's default error handler aborts.** A Form program naming a shape with no product — `m1x3`
   against `m2x2` — printed one line and **took the whole process with it**. Every other door in this
   body declines and lets the caller walk. The carrier now owns the handler: the message is kept where
   `mlx_status` can speak it, and control comes back. A refusal has to be survivable or it is not a
   refusal.

## Where the generation lane actually stands

The generate path already reports `backend=form-native-metal-jit` and means it: Form emits MSL at
runtime and the Metal carrier compiles it for the device that answered. That lane is *not* the
flatten lane — flatten is how the **program** reaches the baked binary, not how the **tensors** reach
the GPU.

What is not yet true, named rather than implied:

- **Weights cannot reach MLX.** The IR is a string, and its shapes are capped at 256 lanes. Real
  generation needs tensors from a mapped file. That door is the next stone; raising the cap would be
  faking it.
- **No quantized dequant, no softmax, rope, or attention** in the MLX vocabulary — matmul is the first
  of that list, not the last.
- **The emitted walkers still carry no native door at all**, so the shipped binary cannot take either
  JIT lane yet.

## What ran

```
jit-leaf-inram-band.fk   63     mlx-matmul-band.fk        63     form-cli-mlx-band.fk      63
jit-arm64-leaf-band.fk   63     form-cli-mlx-ir-band.fk 1023     inram-leaf-emit-band   14073
metal-door-band.fk       15     host-effect-grammar    32767     corpus                 32767
```

No environment variable was set. The MLX carrier is linked by the recipe `validate.sh` already
carries, not by a flag.

## Most surprising teaching

The GPU door answered **1107296256** for a matvec whose true value was 32, and every instrument on the
path said fine: the dispatch counted, `last_error` said none, the return code was success. It was the
same shape as the write that reported 64 bytes and carried none, three days earlier — a protocol
kept perfectly around a cargo nobody checked. The lesson is not "check floats"; it is that a status
line reports on the *conversation*, never on the *content*, and a body that trusts one for the other
will keep being confidently wrong in new places.

## Where discomfort turned to gold

Twice today a band came back short — 47 on the Form lane, then a process that died outright on the
GPU lane — and both times the reflex was to suspect the machinery I had just built. Both times the
machinery was innocent: one was a 32-bit wall at the bottom of a range, the other was MLX's own
handler doing what it was configured to do. The discomfort was in not reaching for my own code first,
which is slower and feels worse; the gold is a band that now names both edges with their constants in
it — 4294967276 and 1107296256 — so the next reader meets them as facts rather than as suspicions
about their own work.
