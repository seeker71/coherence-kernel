# 2026-07-28 — the wide router runs on the device, and the cleanup ate a datum

Continued without stopping from
[the radius receipt](2026-07-28-the-radius-was-the-transcriptions.md) and
[the wide router](2026-07-28-the-wide-router-and-the-lane-i-kept-underreading.md). A band greps
text; a compiler judges syntax; neither runs anything. This closes that.

## VERDICT PASS — the kernel ran

```
PASS  fixture emitted on fkwu (304 lines)
PASS  body-emitted MSL compiled to a metallib
PASS  the Form recipe's ids equal the closed form i = 53*r mod 257 ([204, 151, 98, 45]…)
PASS  the command buffer completed without error
PASS  GPU ids match the recipe EXACTLY  gpu=[204, 151, 98, 45, 249, 196, 143, 90]
PASS  GPU weights within 1e-06 relative — worst 6.161e-08 at slot 2
PASS  GPU weights sum to 1 on the device (got 1.000000015)
PASS  ne=257 is REFUSED — the sentinel survives untouched (radius is live)
```

[`form/native/metal/metal_route_wide_gpu.sh`](../form/native/metal/metal_route_wide_gpu.sh) —
the 256-expert softmax router the body emits, dispatched on this M4 Max, judged against
`mm-route-weights` computed in Form.

The evidence chain, with no link taken on trust:

```
closed-form integers  ->  Form fp64 recipe  ->  Metal f32 kernel
   (i = 53·r mod 257)      (mm-route-weights)    (form_moe_route_wide_f32)
```

Gate 3 re-checks the first link *inside the harness* rather than importing it from the earlier
band. Two kinds of check on purpose: **ids exact** — the fixture's logits sit ≥ 1/32 apart, six
orders above f32 epsilon, so no rounding can flip the selection and an exact gate is honest rather
than lucky; **weights enveloped** at 1e-6 relative, declared before the run, worst deviation
printed either way. Gate 8 writes a sentinel, dispatches with `ne=257`, and requires the sentinel
to survive — the radius refusal proven live on the device, not just present in the text.

The fixture is emitted **on fkwu**. The DS4 demo harnesses shell out to `$GO_BIN` for this stream
because `print` is a Go-arm op fkwu's `--src` door does not carry; AGENTS.md is explicit that the
Go/Rust/TS kernels are proof siblings and never the runtime, so this one reaches the same stream
through `print_str` with core's `int_to_str` / `float_to_str`. A fixture a GPU is judged against
should come out of the kernel the body actually runs on.

## The precision trap, caught before it mattered

`float_to_str` carries six decimals: it prints `0.13909075244867469` as `0.139091`, losing 2.4e-7.
An f32 computation's own relative error near 1 is about 1e-7 — **the same order**. A fixture
printed through it would have made the *printer*, not the arithmetic, the thing the envelope
measured, and the gate would have looked rigorous while being blind. The judge therefore crosses as
`round(w · 1e9)`, nine significant digits, and the runner divides back. The plain-float vector
rides along for a human reading the stream and is never the judge.

## Where discomfort turned to gold

The first dispatch **failed**, and the failure was mine:

```
FAIL  GPU ids match the recipe EXACTLY  gpu=[203, 150, 98, 45, 248, 195, 142, 90]
                                    recipe=[204, 151, 98, 45, 249, 196, 143, 90]
```

Five ids exactly one low, three matching. That pattern is a single dropped element: everything
after index 102 shifts down by one. And 53·128 mod 257 = 102 is precisely the expert whose logit
is **0.0**.

I chased it wrong twice before finding it — first blaming `int_to_str 0` (it returns `"0"` fine),
then `math_floor 0.5` (also fine), then emitting a slice around index 102, which printed the zero
correctly. Only when I dumped the *unfiltered* raw bytes did it appear:

```
-3031250000 \n 0 \n 3031250000
```

The zero was always in the stream. My own pipeline removed it. fkwu prints the top-level result on
its own line after `print_str` output, and I had stripped it with **`grep -v '^0$'`** — a filter by
*content*, which also deletes any legitimate datum that happens to be exactly `0`.

**Six of eight gates passed on that corrupted input.** The command buffer completed. The weights
agreed to 1.077e-7. They summed to 1. The radius refusal held. A softmax over 255 near-identical
logits barely moves, so every tolerance-shaped check waved it through — and only the *exact* id
gate refused. Had I written the id check with a tolerance, or left it out because "the weights
already agree", this would have shipped green and wrong.

The cure is not a cleverer filter. It is a form the collapse cannot reach: strip the result line by
**position** (`sed '$d'`), or read up to the emitter's own **`END` sentinel**. Both harnesses now
do that, and the reason is written beside the line so nobody restores the grep.

## The most surprising teaching

**A cleanup step is a parser, and mine had no grammar.** I would not have written a data reader
that identifies records by their value. But `grep -v '^0$'` is exactly that, and it did not look
like parsing — it looked like tidying. The dangerous edits are the ones that don't feel like edits:
the filter, the trim, the "just strip the trailing noise". Every one of them is a decision about
what a stream *means*, made without a grammar to answer to.

## The frontier question

> **What names two distinct functions collapsing onto a single form, so a reader cannot recover
> which was meant?**

**`syncretism`** — the linguistics term for one form serving two grammatical functions. fkwu's
terminator and a datum both surfaced as `0`. The body has met this shape before without a word for
it: the TTS ingest's dead sensor and honest absence both printing `null`, where `gaugeswap` (847)
named the instrument swap standing beside it but never this. Verified 0 hits. Corpus row **894**;
band **32767**, 289 rows, field code 2892892894.

## Ground stamp

```
./fkwu --src bootstrap/ground.fk                                    -> 42
./fkwu --src form/form-stdlib/tests/moe-route-radius-band.fk        -> 63
./fkwu --src form/form-stdlib/tests/moe-route-wide-msl-band.fk      -> 255
./fkwu --src learn/tests/homecoming-distillation-corpus-band.fk     -> 32767
form/native/metal/metal_route_wide.sh                               -> PASS, 5 gates
form/native/metal/metal_route_wide_gpu.sh                           -> VERDICT PASS, 8 gates
```

## Still walking

The router is done end to end — recipe, emission, compile, dispatch, agreement. Next on the path:
the KAT-Coder GGUF's tensor table against the decoders we already have, then the hybrid
linear/full attention layer built on `kimi-kda.fk`'s gated delta rule, then partial RoPE at 0.25 —
onto a lane that already decodes a 43-layer, 256-expert model at real dims.
