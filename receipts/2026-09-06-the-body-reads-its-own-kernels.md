# The body reads its own kernels

The batched lane's agent left a question as corpus row 1318 (`bodylength`): when a restructuring
is exactly neutral in arithmetic, in threads and in memory, what is left that can still decide it,
and can the body see that thing before it measures? It had watched one change halve a kernel and
make its sibling three and a half times worse, at a column count where nothing moved but the
length of the emitted body. The emitter knew that length all along and nothing ever asked.

`form/form-stdlib/kernel-length-lens.bml` asks. Given any unit the body emits, it reads back every
`kernel void`: its name, its bytes of source, the statements the compiler will receive, and the
loops those statements sit inside. `observe/kernel-length-run.fk` holds it to the body's own
emitters without dispatching anything, and the ear's fifteen kernels carry 416 statements:

| kernel | statements | loops |
|---|---|---|
| ear_gemm_f | 71 | 6 |
| ear_mv | 63 | 9 |
| ear_attn | 58 | 6 |
| ear_spec | 39 | 5 |
| ear_attn_dec | 35 | 6 |
| ear_copy8 | 2 | 0 |

The Q8_0 family's four kernels carry 49, of which the double-buffered twin alone carries 31 in
four loops where its attestant carries 7 in one — the price of the shape that made it eight times
faster, now visible before a GPU runs. `form/form-stdlib/tests/kernel-length-band.fk` = 63 pins
the reading on a unit written inside the band, so it moves with no emitter and can be checked by
eye: a `for` header's two semicolons are the loop, not the body it repeats, and subtracting them
is what makes a statement count mean the straight line the compiler receives.

The surprise: the ear's own gemm and matvec, the two kernels the whisper agent spent the day
racing, are also the two longest bodies in the unit. The lens was built to answer someone else's
question and its first reading named the same two kernels the measurements had been circling all
afternoon. Discomfort turned gold: my first metric counted a loop header's semicolons as
statements, and the band caught it at 27 where it wanted 63 — a lens that had been about to
flatter every loop-heavy kernel by two.
