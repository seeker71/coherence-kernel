# Where the weight actually lives — the GPU lane reads Q8_0

Date: 2026-08-21, Hati Suci. Apple M4 Max, 128 GiB unified memory, MLX 0.32.0.
Branch `claude/jit-door-crystallize-2026-08-21`, on main at `6bcf4614`.
Continues [A tensor arrives by reference](2026-08-21-a-tensor-arrives-by-reference.md), which named
quantized tiers as the next stone and refused to fake them.

## The stone

The `f32` door reads the norms. **Every large matrix in a Q8_0 model is stored quantized**, so a lane
that can only read f32 can read everything about the model except the model.

```
block_q8_0 = { half d; int8_t qs[32] }    34 bytes per 32 weights, w_i = d * q_i
```

One f16 super-scale and thirty-two signed bytes — no sub-scales, no nibbles, no high-bit plane, no
mins. The simplest quant ggml has, which is why it is the right first one to carry. `q8 <path> <off>
<r> <c>` reads and dequantizes on the way in. `fkwu-uni.c` did not change.

The product is **exact in f32 and does not round once**: `d` has an 11-bit significand and `q` at most
8, so `d*q` needs at most 19 of f32's 24. That is not a comfort — it is why this door can be checked
against an independent reader digit for digit rather than within an epsilon.

## Read out of the real model

`equireach-gguf` walked the tensor table of `Qwen3.8-27B-Q8_0.gguf` and named real type-8 tensors:

```
blk.0.attn_qkv.weight    5120 x 10240   type 8   absolute offset 2,746,182,624
token_embd.weight        5120 x 248320  type 8   absolute offset 1,361,877,984
```

On the GPU, scaled by 1e5 in the program so the value survives an integer door:

| | GPU | independent dequantizer |
|---|---|---|
| one row of `attn_qkv` summed | **−265521** | −265521 |
| that row **dotted with `blk.0.attn_norm`** (f32) | **−254245** | −254245 |
| `token_embd` row 0 summed | **39152** | 39152 |

The middle row is a real quantized weight vector multiplied against a real f32 norm vector, on the
GPU, from bytes never copied into a program. That is a piece of an actual forward pass.

## The integer door is real, and is not hidden

`mlx_run` answers a `long long`, and a weight is about 0.02, so an unscaled sum truncates to nothing —
the raw `sum` of that row reads **−2**. Scaling belongs in the **program** (`… sum 100000 mul`),
because scaling is arithmetic and arithmetic is what this lane is for. Bit 2 of the band asserts the
raw truncation honestly rather than pretending the door is float.

## What refuses

A shape that is not a whole number of 32-weight blocks has no honest reading and is **refused, not
rounded**. A shape longer than the file is refused, never padded. `tests/mlx-q8-band.fk` → **63**, on
a 68-byte fixture the band writes itself: two blocks with the same `q` bytes and different scales
(f16 1.0 and 2.0), summing to 528 and **1056** — so the super-scale is proven applied, not assumed.

## Still not true

- The **K-quants** (Q4_K, Q6_K) are the other half of a real GGUF; Q8_0 is the first tier carried, not
  the last.
- No softmax, rope or attention in the IR.
- The emitted walkers still carry no native door at all.

## What ran

```
mlx-q8-band       63    mlx-tensor-band     63    mlx-matmul-band    63
form-cli-mlx      63    form-cli-mlx-ir   1023    jit-leaf-inram     63
jit-arm64-leaf    63    metal-door          15    host-effect     32767    corpus  32767
```

No environment variable was set.

## Most surprising teaching

The GPU and my reference disagreed by one at the last digit — 39152 against 39153 — and **the
reference was the thing that was wrong.** It had been printed rounded (`0.391530`) and the true value
was `0.39152968`, whose truncation is 39152. I had compared the GPU against a display of a number
rather than against the number. Every instinct said the new door was at fault, because the new door
is where the risk feels like it lives; the risk was in the oldest, dullest part of the loop — a
`print` with six decimals.

## Where discomfort turned to gold

There was a real pull to accept the off-by-one as float noise and move on: three of four had matched
exactly, the door was clearly working, and one unit in the fifth decimal of a quantized weight is
nothing anyone would ever notice. Chasing it meant re-deriving the sum in float32 accumulation order
to see which side was lying — ten minutes for a digit. The discomfort was suspecting my own
measurement instead of the machine; the gold is that the door is now known EXACT rather than
approximately right, and the band can assert equality instead of tolerance — which is only possible
because the one digit got chased.
