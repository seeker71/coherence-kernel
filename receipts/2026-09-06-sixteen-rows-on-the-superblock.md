# Sixteen rows on the superblock

The gap this morning: the body's 3B lane (llama32-3b, Q6_K and Q4_K rows) speaks proper Persian,
Chinese, Portuguese and Indonesian where the 1B cannot, at 30 to 46 s a tongue
(receipts/2026-09-06-overlap-and-tongues-validated.md), because only the Q8_0 matvec had
cooperative twins and every Q6_K and Q4_K row still ran one thread, a serial right-fold over
3072 or 8192 weights. Now one 3B token is 34.5 ms wall (GPU 27.6 ms a forward), from 1117 ms
(GPU 1112) — the same sixteen ids, the same text, "Paris. The capital of Italy is Rome. The
capital of Spain is Madrid." Every Q6_K and Q4_K matvec the lane dispatches is byte for byte
its attestant on the 3B's real weights: `form/form-stdlib/tests/q6k-q4k-matvec-tg-band.fk` = 255.

**The shape carried whole.** q8-0-msl.fk's tg4 — pv[2][16][129] in threadgroup memory, the
sixteen lanes of simdgroup 0 each folding their own row, the 64 threads of simdgroups 1..2
loading and dequantizing the NEXT 128-column step while holding the step after in registers — is
the same text on the 256-weight superblock. A step of 128 columns is one half (h) of a superblock;
the 64 loaders are sixteen rows times the four sub-groups (g) of that half, and loader (row, g)
holds its 32 ql bytes (at h*64 + (g mod 2)*32), its 32 qh bytes (at 128 + h*32), its two scales
(192 + h*8 + 2g) and the super-scale as 34 ushorts (Q6_K) — or its 32 qs bytes, the twelve
scale bytes, d and dmin as 24 (Q4_K). The stage computes each product exactly as the attestant's
`q6k_w` / `q4k_w` text does — the same nib, hi, q, sc integers, d by the attestant's own
arithmetic f16 decode (not `as_type<half>`), `((d * sc) * q) * x` and
`((d * sc) * nib - dmin * mn) * x` — and the fold is untouched: j counting down, `p + acc`. The
exactness never lived in the block format; it lived in the product text and the fold order, and
those moved as one. Exact on the first run, all eight shapes, no bisect.

**The numbers** (forty dispatches, attestant one thread a row → the sixteen-rows twin; eight for the head).

| shape | bytes | 40 attestant | 40 tg4 | a dispatch | GB/s |
|---|---|---|---|---|---|
| attn_q Q4_K 3072x3072 | 5.3 MB | 231 ms | 3 | 75 µs | ~71 |
| attn_k Q4_K 1024x3072 | 1.8 MB | 201 | 2 | 50 | ~35 |
| attn_v Q6_K 1024x3072 | 2.6 MB | 104 | 2 | 50 | ~52 |
| ffn_gate Q4_K 8192x3072 | 14.2 MB | 198 | 5 | 125 | ~113 |
| ffn_down Q4_K 3072x8192 | 14.2 MB | 524 | 6 | 150 | ~94 |
| ffn_down Q6_K 3072x8192 | 20.6 MB | 307 | 11 | 275 | ~75 |
| head Q6_K 128256x3072 | 323 MB | 106 (8) | 31 (8) | 3.9 ms | ~83 |
| one 3B token (decode, 15) | 2.02 GB blob | 1117 ms | 34.5 ms | | |

Prefill of six tokens 6930 ms → 417 ms. The 1B lane is untouched: `q8-0-matvec-tg-band.fk` = 1023
after the dispatch change, the fused block 12 ms.

**Dispatch.** `dth-mv` in `form/native/metal/dense-token-handle.fk` chooses by type: 14 (Q6_K)
and 12 (Q4_K) with cols a multiple of 256 go through `form_q6k_matvec_tg4_f32` /
`form_q4k_matvec_tg4_f32` (pipes 37, 38), mode 1, 96 threads a group, ceil(rows/16) groups —
`dth-mv-tg16`, the one binding the Q8_0 tg4 already used. The emitters are separate defns after
each cell's unit (`q6m-matvec-tg4-body`, `q4m-matvec-tg4-body`), so the text-pinning bands
`q6k-msl-band.fk` and `q4k-msl-band.fk` stay 255. The 3B driver is
`form/native/metal/dense-run-llama3b.fk`, the 1B driver's shape on the registry's 3B blob.

**Still open.** The K-quant twins sit at 35-113 GB/s where the Q8_0 twin sits near 210 against
the 330 GB/s floor, on the same fold: the loaders do more integer work a weight (Q6_K: two field
bytes, a power-of-four division, a mod; Q4_K: the scale/min unpack) and each holds 68 or 48 bytes
for 32 products where a Q8_0 loader holds 34 — whether the wall is now the loaders' arithmetic,
their register footprint or the small shapes' launch cost is unmeasured; the attn_k shape at
50 µs is launch-bound (1.8 MB at the floor is 5 µs). The fused block (qkv in one dispatch, the
residual add in the o and down epilogues, gate+up+SwiGLU in one group) is Q8_0-only; the 3B
runs the serial block, 28 layers of seven matvec dispatches plus norms, rope and attention. The
tongues receipt's 30-46 s a tongue was the one-thread lane's; the tongue lane on the 3B now owes
a re-measurement.

The surprise: three bracket counters — mine, string-aware; preflight's; a plain one — all read
the band as balanced while the reader refused it with a stray close at its last line. A
`print_str` one paren short had left its `defn` open, the twenty-four lines after it had become
that defn's body, and the close I had appended at the end "balanced" the total at the wrong depth.
A net count is blind to depth; the reader walks it and is the only arbiter — bisecting with the
reader itself (growing prefixes, each closed once) found the line in two runs. Discomfort turned
gold: the first run printed 127 beside "1 error(s)" and rc 1, a verdict I could have carried; the
error was read to its root before the number was believed, and the header that named the first
Q6_K view "ffn_down" was corrected to what the file holds (attn_v of layer 0, 1024x3072) with the
Q6_K ffn_down width given its own bit.
