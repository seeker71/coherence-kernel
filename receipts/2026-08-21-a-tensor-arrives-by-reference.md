# A tensor arrives by reference — the GPU lane reads the real model

Date: 2026-08-21, Hati Suci. Apple M4 Max, 128 GiB unified memory, MLX 0.32.0.
Branch `claude/jit-door-crystallize-2026-08-21`, on main at `10fd457f`.
Continues [Walking the JIT home](2026-08-21-walking-the-jit-home.md), which named this as the next
stone and refused to fake it by raising a cap.

## The stone

Until now the only way data reached this GPU was as literal lanes in the program text, capped at 256.
Fine for proving a matmul; useless for a forward pass. **A program is a string, and a string is not
how gigabytes of weights should ever travel.**

`f32 <path> <off> <r> <c>` names a file, a byte offset and a shape. The carrier reads r×c float32
from where they already lie. Nothing about `fkwu-uni.c` changed — the carrier's own rule again: new
shapes are new tokens.

## Read out of the 29 GB model, and cross-checked

The body's own `equireach-gguf` walked the tensor table of `Qwen3.8-27B-Q8_0.gguf`
(29,047,086,048 bytes) and named two real F32 tensors:

```
output_norm.weight        d0=5120  type=0 (F32)  absolute offset 1,361,857,504
blk.0.attn_norm.weight    d0=5120  type=0 (F32)  absolute offset 2,746,162,144
```

Then, on the GPU, against an independent Python reader of the same bytes:

| | GPU | independent reader |
|---|---|---|
| `sum(output_norm)` | **9953** | 9953.986115 |
| `sum(blk.0.attn_norm)` | **4949** | 4949.1697 |
| `matmul` (1×5120)·(5120×1) | **9626** | 9626.9860 |

That last row is a real dot product of two real weight vectors from the real model, computed by MLX
on the GPU, from bytes never copied into a program. The lane can now be fed.

## What the door refuses

A shape longer than the file holds is **refused**, never padded — padding would hand the GPU whatever
`malloc` last carried and every number downstream would be confidently wrong with nothing saying so.
A missing path is refused. Neither refusal increments `mlx_dispatch`, so the counter keeps meaning
what it says.

`tests/mlx-tensor-band.fk` → **63**, on a 32-byte fixture the band writes itself with the body's own
`md-f32-bits`, so it needs no 29 GB file to be honest: the matrix at byte offset 8 sums to 21, the
same shape from byte 0 sums to 207 (the offset is real), the matvec is 975, and both refusals hold.

## What is still not true

- **Quantized tiers.** F32 tensors in this file are the norms. The model's actual weight is Q8_0 and
  the K-quants, and MLX cannot read those through this door — dequant is the next stone and is named
  rather than faked.
- No softmax, rope, or attention in the IR; matmul is the first of that list, not the last.
- The emitted walkers still carry no native door at all.

## What ran

```
mlx-tensor-band       63     mlx-matmul-band        63     form-cli-mlx-band       63
form-cli-mlx-ir     1023     jit-leaf-inram-band    63     jit-arm64-leaf-band     63
metal-door            15     host-effect-grammar 32767     corpus               32767
```

No environment variable was set.

## Most surprising teaching

The hard part of feeding a GPU was not the GPU. It was admitting that a text program cannot carry a
tensor, and that the fix is for the text to stop trying — to point outside itself instead. Once the
program said *where* rather than *what*, a 5120-element weight crossed in one token, and the same
sentence would carry a 5120×5120 one. The cap that looked like the problem was never the problem; the
representation was.

## Where discomfort turned to gold

Writing the band, I wanted to assert on the real model — the numbers were sitting right there, 9953
and 9626, freshly witnessed and far more impressive than a six-float fixture. A band that reads a
29 GB file nobody else has is a band that reads green on my machine and skips silently everywhere
else. The discomfort was demoting my best evidence to prose; the gold is a band that stands on 32
bytes it writes itself and a receipt that carries the model numbers where they belong — as a witness,
dated, with an independent reader beside them, and not as a gate that only one laptop can pass.
