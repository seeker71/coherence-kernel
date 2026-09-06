# The root crosses the barrier

The gap this morning: one llama3.2:1b token on the dense Metal lane 36 ms, the hardware floor 4 ms
(one pass over the 1.23 GiB blob at the measured 330 GB/s). Now 13.0 ms a token wall, 10.5 ms of GPU,
the same sixteen ids ("Paris. The Eiffel Tower is located in Paris. The Louvre Museum"). Every
kernel the lane dispatches is proven byte for byte its attestant by
`form/form-stdlib/tests/q8-0-matvec-tg-band.fk` = 1023, on real weights and real state.

**Why the rmsnorm twins were not exact — bisected, not guessed.** A twin whose text is character
for character the one-thread attestant's, run as a threadgroup with thread 0 working: exact. The
thread index taken from the threadgroup attribute: exact. The x values staged through threadgroup
memory for the fold: exact. The normalize pass spread over the group with `inv` handed across the
barrier: 738 bytes off, in 369 of 2048 elements, one bit each. The attestant's own text compiled
under `#pragma clang fp reassociate(off)` parts from the fast-math attestant by the SAME 738 bytes,
and is byte for byte the twin. So the fold was never the difference: the attestant's loop writes
`nm = x * inv` with `inv = 1.0f / gg` in view, and the compiler evaluates it as one division —
a stored `inv` on the other side of a barrier takes that view away. The twin that hands `gg`, the
root, across the barrier and lets every thread write `inv = 1.0f / gg; nm = x * inv` exactly as the
attestant's text does: 0 bytes off. Its fold reads device memory eight loads ahead of eight
dependent adds (the chain stays one chain): 265 µs to 30 µs. `#pragma clang fp reciprocal(off)`
does not exist in Metal's clang ("expected contract, reassociate or exceptions"), so the pragma
that would have named the mechanism is not a door; the bisect is.

**Where 36 ms went, and what took it back.**

| | before | after | how |
|---|---|---|---|
| argmax over 128 256 logits | 8.9 ms | 0.13 | the two-stage form_argmax_part/comb already in llama-decode-msl, dispatched (equality: comparison only) |
| rmsnorm, 33 a token | 0.27 ms each | 0.03 | the twin above |
| Q8_0 matvec q 2048x2048 | 120 µs | 20 | sixteen rows a group: sixteen lanes fold sixteen rows while 64 loaders stage the next 128 columns and hold the step after in registers (row stride 129 floats, sixteen banks) |
| gate/up 8192x2048, down 2048x8192 | 160-170 µs | 70-80 | the same |
| head 128256x2048 | 2.4 ms | 1.0 | the same (263 MB: 0.8 ms at the floor) |
| attention at pos 10 | 175 µs | ~10 | one group a head; lane p folds position p's dot, thread 0 the max and the ascending sum, lane i folds y_i; `sumes` crosses the barrier, never `invs` |
| rope, 2 a layer | 50 µs each | one dispatch of pairs | form_rope_pair_f32 over q and k in one dispatch |
| dispatches a token | 276 | 133 | q, k, v in one dispatch with k and v landing in their caches at the position's row; the residual add in the o and down epilogues (`y[row] = r[row] + acc`, the add kernel's own two operands); gate, up and the SwiGLU in one group (lane i folds gate row i, lane 16+i up row i, the up fold handed across) |
| token | 36-37.5 ms | 13.0 (GPU 10.5) | |

The multi-row matvec twin runs near 210 GB/s on every shape against the 330 GB/s floor; the serial
fold per row (cols dependent adds, the floor of this exactness) is no longer the wall — sixteen
lanes fold sixteen rows and the loaders run a step ahead. Threadgroup memory bounds it: 32 rows a
group with four blocks a step (33 KB) refuses to compile; sixteen rows with four blocks (16.6 KB) is
the shape that fits.

**What the band asserts** (1023): tg, tg2, tg3, tg4 on the first projection; tg4 on ffn_down; the
rmsnorm twin; the two-stage argmax on the logits of a real BOS forward; the cooperative attention
and the pair rope on the last layer's cache and q after three generated tokens; and the fused block
against the serial block on one more token — the same 513 KB of logits, the same id. The
dense-family band's dispatch pin moved with the fusion (5796 to 2793 over 21 forwards; 1023).

**Still open.** The wall is 13.0 ms and the GPU 10.5: the 2.5 ms between is the Form dispatch loop
building 133 binding strings a token (md-bind16 through md-le32 each time); bindings that do not
carry the position could be built once at open. On the GPU, the kernels sum to about 6 ms and 133
dispatches carry the rest as launch and drain; the next fusions are the rmsnorm into the qkv and glu
prologues (each group would refold 2048 squares — only worth it if the fold moves off the serial
lane) and the attention into the o projection. The matvec at 210 of 330 GB/s: a third loader
simdgroup, or a deeper register prefetch, is the untried room.

The surprise: the two rmsnorm twins that had stood "compiled, not exact" for a day were exact in
their folds all along; what differed was which scalar crossed the barrier, and handing the root
across instead of its reciprocal was the whole fix. Discomfort turned gold: measuring the
"exact" one-thread attestant against its own text evaluated as written and finding the attestant
was the one the compiler had rewritten — the band now names what is (the twin equals the attestant
as compiled), and the receipt names why.
