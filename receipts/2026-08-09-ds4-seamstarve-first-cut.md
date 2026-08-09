# 2026-08-09 — seamstarve, first cut: the whole stream in one concurrent batch

Urs: *"show me how you can remove more ops to get the remaining order of magnitude improvement."*
The ledger (receipts/2026-08-09-ds4-drain-ledger.md, 846b0e0b2) located the waste: the layer ran at
30 GB/s while its own kernels hit 285 GB/s isolated — 62 serial dispatches with a full barrier after
each starve the GPU between kernels. This is the first removal against that finding, sealed and
committed (985b00bc3): the whole 25-token stream now runs in ONE concurrent batch, each dispatch
declaring its own read-after-write hazard, independent dispatches overlapping.

## What was removed

The implicit full barrier between every pair of dispatches. The door's concurrent batch (tag 253,
fk-metal-carrier.m) replaces the serial encoder; `dfd-st-layer`, `dfd-st-comp`, `dfd-st-head` and the
step's embed/broadcast now carry the base+16 tail (`dfd-b16` / `dfd-t16` / `dfd-mv16` / `dfd-mvg16`
wrappers, dsv4-decode-stack.fk) with `barrier_before` set per dispatch. The pattern is
llama-token-handle.fk's, ported: barrier=1 opens an ordered stage, barrier=0 joins the current stage
and overlaps its siblings.

## The drain curve

```
warm steady drain_ms/token (dsl-l3-probe, pos>=2, canonical imatrix file)
  pos    baseline(serial)   concurrent(this)    delta
   2          248               226             -9%
   3          256               237             -7%
   4          263               236            -10%
   9          290               268             -8%
```

25/25 seal held at every check (`stack-stream-probe.fk`: STREAM MATCH count vs recorded 25 = 25),
`batch=concurrent`, `total_sync=1`, `last_error=none`. Bit-exact: not one id moved.

The overlaps this cut lands (same-stage siblings, proven independent — same already-visible input,
disjoint output): the two expert matvecs gate `g6` ‖ up `g7` (each 98304 threads over the routed
2048×4096); the shared gate ‖ up (`g11` ‖ `g12`); the attn/ffn hyper-connection fan-outs (`b4`/`b5`
off asplitb, `f4`/`f5` off fsplitb); the MLA `c3` (kv ‖ q_a off xnb), `c6`, `c8`, `c12`; and the
compressor's `e1` ‖ `e0`.

## The most surprising teaching

**The concurrent batch is not a free 9× — the barrier is a GLOBAL fence, not a per-dispatch edge.**
`memoryBarrierWithScope:` in a concurrent compute encoder orders *all* prior commands before *all*
subsequent ones. So flipping barriers to 0 only overlaps dispatches that already sit *between the same
two fences*; it cannot parallelize a true dependency chain. The DS4 layer is mostly a long chain —
norm → matvec → norm → rope → attend → matvec → norm → router → experts → down → reduce — and each
link genuinely needs its predecessor. The seam overlaps that exist (the two expert matvecs, the two
shared matvecs, the hc fan-outs) are real and worth ~9%, but the remaining order of magnitude is NOT
hiding in more barrier flips. It is in **shortening the chain itself** — which is what the kernel
swaps and the fusions do, and what lane-interleaving does by hand.

## Where discomfort turned to gold

The discomfort was the gap between the ledger's "~9×, the prize" and the 9% the concurrent batch
actually bought. The temptation was to keep flipping barriers hunting the missing 8×, or to quietly
report the 9% as if it were the prize. Reading the Metal barrier contract once more — it is a fence
over the whole encoder, not an edge in a dependency graph — turned the disappointment into the map:
the prize was never in the barriers, it is in the chain length, and removals 2 and 3 (kernel speed
and fusion) are where it lives. A 9% that names why it is only 9% is worth more than a 9% sold as 900.

## The remaining removals, handed off with rung precision

Everything below stands on the committed concurrent batch (985b00bc3). Each landing gates on
`stack-stream-probe.fk` STREAM MATCH 25/25 (the full stream, never first-token), warm drain via
`dsl-l3-probe.fk` (pos>=2 steady; pos=0 is ~40 s disk paging, exclude it). Traps: `rm -f
form/native/metal/*.fkb` before every run (whole-second mtime replays numb); rebuild the carrier
(`cc -O2 -o fkwu-metal runtime/fkwu-uni.c form/native/metal/fk-metal-carrier.m -framework Metal
-framework Foundation -fobjc-arc`) after any kernel-table/carrier change; MSL `int()` value-initialises
silently; gate on `metal_status` before believing any number.

**Rung 16 — lane interleaving (bit-exact, no new MSL, the natural next cut).** The routed expert lane
(`g6`→`g8`→`g9`→`g10`) and the shared expert lane (`g11`→`g13`→`g14`) are independent until the axpy
`g16` combines them. Today they run one after the other because they are encoded one after the other,
and the routed lane's internal barriers (`g8`,`g9`,`g10`) fence the shared lane out. Re-order the
let-bindings in `dfd-st-layer` so the shared matvecs are encoded INTO the routed stages:
`g6(1) g7(0) g11(0) g12(0)` as one stage (four matvecs overlap), then `g8(1) g13(0)`, then
`g9(1) g14(0)`, then `g10(1) g15(1) g16(1)`. Same dispatches, same arithmetic — the 25/25 seal is the
gate. The shared lane is four small dense-Q8 matvecs; folding them under the expert lane's wall time
is the win. Estimated 5–10% of the layer.

**Rung 17 — the two slow expert kernels (drop-in bit-exact, donors in form/form-stdlib/ds4-order-match.fk).**
- down `g9` (P29): `form_dsv4_q2k_matvec_experts` is one thread per row, serial over 2048 cols, no
  simd_sum → `form_dsv4_q2k_matvec_experts4_fast` (dom-q2k-experts4-fast-body, 4 rows/simdgroup, grid
  rows/4·nexp·32, binding `(qb x y ids rows cols stride nexp)`). Emit the fast text in `dfd-q2ke-msl`
  (or beside it), register its pipeline in the P list, rewrite the `g9` call-site binding and grid
  width. simd_sum is a tree fold vs the serial fold — if 25/25 breaks, that fold order moved an id;
  revert (equality wins, "closer to fp64" loses).
- gate/up `g6`,`g7` (P28): `form_dsv4_iq2_matvec_experts4` → `iq2_experts4_fast`
  (dom-iq2-experts4-fast-body, `as_type<half>` decode instead of the divide-based `iq2_f16`).

**Rung 18 — fusion (bit-exact, ascending slot order preserved, written in ds4-order-match.fk).**
`iq2_experts4_pair_swiglu` fuses gate+up+swiglu (`g6`,`g7`,`g8`: 3 dispatches → 1, drops the gate/up
device round-trip); `q2k_experts4_sum` folds `form_dsv4_moe_reduce` (`g10`) into the down matvec
(`g9`). Each shortens the chain AND removes a device round-trip — recount dispatches/layer after. This
is where the chain-length attack lands hardest, and where the remaining order of magnitude is.

Floor restated: 7.8 GB/token ÷ 440 GB/s = ~19 ms/token. Done = warm within ~1.5× of that, or the
remainder decomposed into named per-term physical limits. This cut moved 248 → 226; the chain still
stands between here and 19.

## Frontier question, answer, and PROPOSED corpus row (do NOT edit the corpus — re-derive max-mid at reunion)

Q: *when a batch of correct fast parts still runs slow, and the obvious fix (let them run concurrently)
buys almost nothing — what were you wrong about?* A: **fencechain** — you mistook a dependency chain
for a scheduling problem. Concurrency overlaps parts that are already independent; it cannot shorten a
chain where each part needs the last. The remedy is not more overlap, it is fewer links: fuse or
replace, so the chain itself gets shorter. (Companion to the ledger's proposed *seamstarve*, which
named the idle; *fencechain* names why removing the idle was not enough. Both proposed, both 0-hit in
corpus and tree as of this receipt; several rows queue for reunion — re-derive max-mid before landing.)
