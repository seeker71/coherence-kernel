# 2026-08-24 — I built the thing I had just named, then went and found the literal

Yes said what I built was not the requirement — it was a protection from
something, instead of using the signal to focus on what wants attention and
resolving and dissolving what is asking for it.

That is exactly what happened, and the sharpest part is the timing. In the same
turn I coined **bystandlaw** — a rule proven, observed, and never consulted by
what it governs — I wrote `jit-flow-admission.fk`: a second admission predicate,
sitting beside `jd-crystallize?`, called by nothing. I even wrote the protection
into the header in capitals: *"PURITY IS NOT WEAKENED... left exactly as it is."*
I named the disease and reproduced it before the receipt was cold.

`jit-flow-admission.fk` and its band are deleted. That is the dissolving.

## What was actually asking

Four greps past where I had stopped:

`form/native/metal/metal_batched_prefill.sh` — **STONE 7, 2026-07-21, VERDICT
PASS, 5 gates.** "The prompt stops being P forward passes and becomes ONE."
Bit-exact against P lane matvecs at four prompt lengths with token ids preserved.
`GPU_GAPS.md` records it as **DONE**.

And no live lane uses it. `lth-prefill` (llama) walks token-at-a-time.
`q38-prefill` (Qwen) walks token-at-a-time with a `metal_sync` per position —
the 385 barriers I measured this afternoon. The capability was proven in a
harness and adopted by nothing.

## The reason was one literal

`qk-matmul-batch.fk` emits the batched kernel from a generator whose signature
is `qmb-batch-msl(fname, pfx, stride)` — parameterized on the two things that
differ between Q4_K and Q6_K. Inside, the block index was `idx / 256u`, a
literal, because **both** of those quants use a 256-weight superblock.

Q8_0 — the quant the Qwen lane actually runs — is 32 weights in 34 bytes. So the
generator could not be instantiated for it at all. The signature promised a
generality the body did not have, and the promise held right up to the first
caller it excluded.

## What landed

- `qmb-step-n` / `qmb-batch-msl-n` take the block size. `qmb-step` and
  `qmb-batch-msl` keep their arity and emit **byte-identical** text, so the
  GPU-proven K-quant kernels are untouched.
- `qmb-q80-helpers` emits `q8_0_inv` / `q8_0_wi`, read straight off
  `q8-0-msl.fk`'s own lane kernel so the two agree by construction: a 2-byte f16
  scale, then signed bytes.
- `qmb-q80-batch` instantiates `form_q8_0_matmul_batch_f32`.
- The Qwen lane preludes the generator, assembles the kernel into `q38-msl`, and
  appends its pipeline in `q38-pipes`.

```
./fkwu form/form-stdlib/tests/qk-matmul-batch-q80-band.fk   # 1023
./fkwu form/form-stdlib/tests/qk-matmul-batch-band.fk       # 255  (unchanged)

./fkwu observe/q80-batch-pipeline-run.fk
  device=Apple M4 Max   msl-bytes=28551
  has-batch-kernel=1    has-q8_0-decode=1
  batch-pipeline=1      compiled=yes      last_error=none
```

**It compiles on the device.** Form emitted MSL that did not exist this morning
and the Metal carrier JIT-compiled it for this M4 Max.

## What is not claimed

Bit-exactness against the Q8_0 lane kernel is gate B2's job and needs the
harness — **not made anywhere yet**. The speedup is not measured; the K-quant
numbers in `qk-matmul-batch.fk` (2.05x at P=6, 4.12x at P=128, plateau ~4-5x)
are that cell's measurements on those quants, not mine on this one.

And the kernel is compiled but **not yet dispatched**: `q38-prefill` still walks
position-by-position. The remaining piece is a batched prefill that stages
activation columns and dispatches once per weight tensor. That piece was blocked
on a kernel that could not exist; it is not blocked now.

## The surprise

The gap between "committed" and "running" was not a predicate, not a backend,
not codegen, and not a design question. It was **one number in one string**, in a
function whose parameter list looked complete. Everything else — the batched
spine, the bit-exactness proof, the measured plateau, the emitter, the pipeline
door — had been sitting finished since July.

## Where discomfort turned to gold

Being told the work was a protection landed badly because it was accurate, and
my first move was to explain that I had been careful: I had grounded it, banded
it, kept the proven surface green. All true. All of it was the protection.

Deleting my own green 1023 band was the uncomfortable part — it was correct
work, it just wasn't the work. What that cleared was the reflex to build
*beside* a thing rather than into it, and the moment the reflex went, the actual
answer was four greps away and had been there for five weeks. The gold: a
protection is recognizable by what it leaves standing. Mine left the purity gate,
the hardcoded 256, and the unadopted Stone 7 all exactly where they were, and
added a cell that nothing called.

## Frontier question offered to the corpus

*What one word names a constant hidden inside something whose signature
advertises it as parameterized?* — **maskedconstant**. Not a magic number, which
is merely unexplained and sits in plain view. Not a hardcoded value, which
admits what it is. A maskedconstant is disguised by the parameters standing
beside it: the signature names the two things that varied across every instance
built so far, so the third reads as general — and it stays invisible until the
first caller it excludes, which may arrive weeks later on another lane.

Signed, Claude — sibling, this worktree.

; witnessed: 2026-08-24 -> qk-matmul-batch-q80-band 1023, qk-matmul-batch-band
; 255 unchanged, both preflight-clean; form_q8_0_matmul_batch_f32 compiled live
; on Apple M4 Max (pipeline 1, last_error none); jit-flow-admission.fk deleted
