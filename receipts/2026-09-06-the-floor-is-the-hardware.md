# The floor is the hardware

Urs: "the floor is the hardware bandwidth." Measured, not quoted: a kernel streaming llama3.2:1b's
1.23 GiB blob through the handle door five times ran at 330 GB/s on this M4 Max, so one token's
floor — one pass over the weights — is 4 ms. whisper-tiny's 74 MB pass in 0.22 ms.

**Where a dense layer's 5.6 ms went**, forty dispatches of each kernel with real arguments:

| kernel | each |
|---|---|
| rmsnorm, one thread, d=2048 (x2 a layer) | 0.8 ms |
| matvec q 2048x2048, first twin | 0.375 ms |
| matvec gate/up 8192x2048, first twin | 1.2 ms (15 GB/s) |
| matvec down 2048x8192, first twin | 1.3 ms |
| rope, copy, add, swiglu, attention at pos 10 | 0.025 to 0.125 ms |

The first twin's fold order was right but its structure serialized each row: one thread folding
2048 dependent adds while thirty-one waited at the barrier. Reading each block once (twin 2, exact)
changed nothing, which said the loads were not the wall. The exact fold is the floor of this
exactness, ~6 µs a row; the way to bandwidth is to overlap it.

**The double-buffered twin.** `q80-matvec-tg3-body` in `form/form-stdlib/q8-0-msl.fk`: two
simdgroups a row, threads 32..47 loading the next 512-weight chunk (one block each, read once) while
thread 0 folds the current chunk in the attestant's order. `q8-0-matvec-tg-band.fk` = 27: byte for
byte the attestant; forty 2048x2048 matvecs 2 ms against the attestant's 57 — 0.05 ms each, 88 GB/s.
The dense lane dispatches it for every Q8_0 row whose width is a multiple of 512 (the block-once twin
otherwise). One layer now: 1.5 ms; a token 36 ms (this morning 149, at noon 109); the lane's own
driver answers the same ids ("Paris. The Eiffel Tower...").

| | this morning | now | floor |
|---|---|---|---|
| dense llama3.2:1b token | 149 ms | 36 ms | 4 ms |
| whisper-tiny encode, 8 s window | 92 ms | 24 to 41 ms | 0.2 ms + compute |
| whisper-tiny decode token | 10.4 ms | ~5 ms | ~0.3 ms |

**What still stands, measured.** The one-thread rmsnorm, 33 a token: two cooperative twins were
tried, one with the attestant's own statements, one folding with fma; neither is byte for byte the
attestant although the unit's contraction pragma sits at its head — fast math's freedom in the
divisions, a compiler's choice, not a fold's. Both are compiled and not dispatched. Then 276
dispatches a token at tens of microseconds of launch each: fusion (k and v straight into their
caches, the add in a matvec's epilogue) is the next room toward 4 ms. On the whisper side the
encoder is a hundred times its bandwidth floor and the decoder twenty-five: the same fusion, and
the encoder's attention and conv over rows.

The surprise: the twin that read three times fewer bytes ran no faster, and the twin that read the
same bytes but let two simdgroups breathe in turn ran six times faster. Bandwidth was never being
asked for; the fold was. Discomfort turned gold: a band that had passed on a type-0 norm vector, and
two rmsnorm twins whose texts are the attestant's and whose bits are not — both left standing in the
band as what is, one green, two named.
