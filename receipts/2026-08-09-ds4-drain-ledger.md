# 2026-08-09 — the remaining order of magnitude is serialization, and the ledger proves it

Urs: *"show me how you can remove more ops to get the remaining order of magnitude improvement."*

DS4 is form-native and warm at ~270 ms/token (stream sealed 25/25, ec67cde96 + a0c26d859). The
question is the last ~13x to the floor. The both-sides ledger locates it exactly — and it is NOT the
kernels and NOT the bytes.

## The ledger (bytes actually read, not tensors on disk)

```
dense projections (read whole):        140.4 MB/layer
experts on disk (all 256):            1728.0 MB/layer
experts ACTUALLY read (6 of 256):       40.5 MB/layer   <- the MoE touches 6, and reads 6
----
TRUE bytes per layer/token:            180.9 MB   (x43 = 7.8 GB/token)
floor at 440 GB/s span:                  0.43 ms/layer -> 19 ms/token
measured warm drain:                     6.26 ms/layer -> 270 ms/token
EFFECTIVE bandwidth:                        30 GB/s  =  7% of the bus
```

## The tell: isolated kernels are 10x faster than the layer that runs them

- `form_dsv4_q80_matvec_ds4` (the dense Q8 projections) measured **285 GB/s** isolated, 325 grouped.
- The layer built from these kernels achieves **30 GB/s**.

A 10x gap between a kernel alone and the same kernel in the layer is not the kernel. It is the
**62 serial dispatches per layer with a full barrier after each** (`metal_status` reports
`batch=serial`): the GPU drains one kernel, stalls at the barrier, drains the next. It is starved
between kernels, not slow inside them. Removing the implicit full barrier — keeping only the real
read-after-write hazards — is the order-of-magnitude op removal.

## The removals, all with parts already on the shelf, in leverage order

1. **Concurrent batch (~9x, the prize).** Door tag 253 + `barrier_before` only at true hazards, so
   independent dispatches overlap. UNBLOCKED as of a0c26d859: the id chain is device-resident, so the
   whole 25-token stream is one submit with `total_sync=1` (was 25+). The work is declaring the
   hazards across the 62 dispatches of `dfd-st-layer` — which dispatch reads a buffer a prior one
   wrote. Every landing gated on STREAM MATCH 25/25.
2. **Two slow kernels, drop-in bit-exact variants in `ds4-order-match.fk`:**
   - down: `form_dsv4_q2k_matvec_experts` (P29/g9) is *one thread per row, serial over 2048 cols, no
     simd_sum* → `form_dsv4_q2k_matvec_experts4_fast` (4 rows/simdgroup, packed reads, grid
     rows/4·nexp·32). Its own comment prices it at ~3.2 ms.
   - gate/up: `form_dsv4_iq2_matvec_experts4` (P28/g6,g7) carries a divide-based `iq2_f16` decode
     (4 of 7 ms) → `iq2_experts4_fast` (`as_type<half>`, packed grid reads).
3. **Fusion, written and documented bit-exact (ascending slot order preserved):**
   `iq2_experts4_pair_swiglu` fuses gate+up+swiglu (3 dispatches → 1, drops the gate/up device
   round-trip); `q2k_experts4_sum` folds `form_dsv4_moe_reduce` (P25/g10) into the down matvec.

## Floor, restated so "done" is unambiguous

7.8 GB/token / 440 GB/s = **~19 ms/token**. Done = warm ms/token within ~1.5x of that, or the
remainder decomposed into terms each at its own physical limit. The cold first token (36,753 ms) is
one-time disk paging of ~9 GB and is not on this budget.

## The most surprising teaching

**The kernels were never the problem, and neither were the bytes.** For two rounds the instinct
(mine, the dense lanes', everyone's) was "the matvec is slow" — and here the matvec is at 285 GB/s
while the layer is at 30. The waste was in the *seams between* correct fast kernels, invisible to any
per-kernel measurement, visible only when you divide the layer's true bytes by the layer's wall time
and get 7% of the bus. A profiler that times each kernel would have reported everything healthy.

## Where discomfort turned to gold

Two averaging errors in five minutes, both caught in the reading. First the cold pos=0 token (36 s of
paging) smeared into a "100% drain" number until I split it out. Then the manifest's 1868 MB/layer —
all 256 experts — nearly became the byte budget until the 6-of-256 routing corrected it to 181 MB.
Each wrong number, read once more, produced the right one; the discipline that felt slow (re-derive,
never quote) is the only reason the 30-GB/s finding is trustworthy rather than a fourth wrong average.

## Frontier question (proposed, not landed — single-writer rule; re-derive max-mid at reunion)

Q: *what names a waste that lives in the seams between correct fast parts, invisible to measuring any
part alone?* A: **seamstarve** — the GPU idle between serial dispatches, each kernel fast, the
assembly starved. Verified 0-hit in corpus and tree.
