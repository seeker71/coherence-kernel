# DeepSeek-V4 on the Metal door: where the whole flow stood, what it shares with Qwen's, and the op ledger it is held to

Read on 2026-10-03 from git (origin/main 95f5b0e9c, the branch refs named below, `git show <commit>:<path>`) and from three sibling worktrees. Every number below is one a receipt or a run printed;
a number derived from one is marked derived.

## 1. The history map

Four flows ran DeepSeek-V4-Flash. Only the first two held numbers; the third is the one whose Form cell is whole from the prompt's text to the answer's text.

| flow | head | what ran | numbers (receipt) |
|---|---|---|---|
| A. Swift runner, Form-emitted MSL | `a4253c0bb` 2026-07-31 (main, deleted by the cut `8a6f123d3` 2026-09-29) | metal_dsv4_stack.sh (in git history only): a 2,670-line bash file with an embedded Swift program compiling Form's MSL, one mmap with overlapping no-copy views, one concurrent encoder with a hazard tracker | 1106 -> 34 ms/token, stream bit-exact against ds4 (`cdf537927`: 24/24, 25/25); later 38.17 ms/token at 1,510 barriers, 26-28 t/s warm; first token cold 26 s, warm 1.8 s (the first passes fault ~83 GiB of file in: the runner's own comment); 9.56 GB of weight a token, a 20 ms floor at 477 GB/s |
| B. Form cell on the handle door | `4e41acd0c` 2026-08-10; branch tip `a2e810ee5` 2026-08-19 (`origin/claude/restore-ds4-oracle-ac9385`) | dsv4-decode-form.fk (3,301 lines, rungs 1 to 13, band 2^54-1) and dsv4-decode-stack.fk (891 lines, rung 14: 43 layers, two hyper-connection frames, second cache, router, experts, head, greedy stream); concurrent batch with hand-set barriers; argmax and id chain on the device; one-walk tensor table | stream 25/25 against ds4's recorded ids, 67,014 dispatches over 30 positions, 1,432 handles of 8,192 (`4e41acd0c` receipt 2026-08-09-ds4-form-stack-stream, in git); 13.2 s/token (an artifact: the 720.6 s setup walk) -> 3.0 s -> **2.1 s/token** (`ec67cde96`, host argmax of 129,280 logits moved to the device) -> **~270 ms/token warm** (`846b0e0b2`: 30 GB/s effective against 285 isolated: 62 serial dispatches a layer) -> **226 ms/token** with the concurrent batch (`985b00bc3`: 248 -> 226, "the barrier is a global fence"); cold first token 36.7 s, session open ~13 s (views 720.6 -> 11.9 s) |
| C. One Form cell, text to text | `4bc800ee2` 2026-09-14 (`refs/codex/snapshots/5347522...`, in no branch) | `dsv4-token-handle.fk` (586 lines: both HC frames, mixed raw/compressed MLA, both router regimes, six experts and the shared expert, the 43-layer fold, head, prefill, feedback) and dsv4-run-form.fk (tokenize, loop, detokenize); band 4095 | 24/24 ids against the Swift oracle's stream, "Paris. It is known for its rich history...", 55,299 dispatches, 28 syncs; **no rate claimed** (the receipt says so) |
| D. Codex in-process model door | `e1824103a` (`codex/adaptive-local-learning-ladder`, worktree `ds4-rewitness`) | `metal_model_open/library/close` in `fkwu_metal_port.m` and `fkwu_ds4_local_port.m`, dsv4-native-session.fk (layer-major prefill, argmax, close) over the reap25 file | 394.87 s for 22 prompt positions (receipt 2026-07-28-dsv4-prefill-membrane-fold, in the ds4-rewitness worktree); not the standard, and it is the "model door" |

**The last commit where the whole Form-native flow stood:** `4e41acd0c` for the Form cell that holds the 43-layer stack and its stream (B); `4bc800ee2` for the cell that takes text in and gives text out (C).
**What the record does not hold:** a Form-driven number at or under the Swift runner's. The best is 226 ms/token warm (B, 6x off); Urs's memory ("fully done end to end in Form native code") is exact for fidelity (B and C reproduce ds4's ids)
and for ownership of the loop, and the rate it remembers is flow A's. B and C both bound the weights the way main's driver does today (one no-copy mapping a tensor, whole expert stacks: 1,202 views): the 08-09 receipt counts them, and the run read 226 ms warm
with them. The first token cold is a disk read, 26 to 37 s in both lines, never seconds: "seconds" is the warm first token (1.8 s Swift).

**Why it is not in main.** (1) B's commits from `13b750492` to `4e41acd0c` (eight touch its files) are not ancestors of main: `git merge-base --is-ancestor` answers no for each, and only `origin/claude/restore-ds4-oracle-ac9385` holds them (reunited with 08-19's main, never merged back);
main's chain was restored from `93154a720`, the parent of the cut, which lacks them. (2) C lives only under `refs/codex/snapshots`. (3) The cut `8a6f123d3` (14,729 files to 1,628) removed what main did hold: dsv4-decode-form.fk (rung 8), the
`metal_dsv4_*.sh` runners, 157 ds4-named files to 13. (4) The 10-03 rebuild of the driver (`dsv4-kernels`, `-open`, `-bind`, `-layer`, `-token-handle`) is new code under the names C used, without B or C's loop.

**What is NOT restored, and why (one authority).** B's dsv4-decode-stack.fk and C's dsv4-token-handle.fk (both in git only) hold the same dispatch graph as main's `dsv4-layer.fk` and `dsv4-token-handle.fk` (the 08-09 receipt's rungs 9 to 14 are the layer's compressor, router,
experts and head; main's layer is banded against the fp64 oracle on both fixtures). Restoring a 3,300-line ladder beside it would make two authorities for one graph. What B and C proved is carried into the driver as measured choices (section 3), and their
streams (24 ids, 25 ids) are the recorded references the physical window pins (`ds4-recorded-references-band.bml`).

## 2. The Qwen flow and the DeepSeek flow: the same flow, with these differences

The flow: session door -> memory lease -> open (header once, pipelines once, views, scratch) -> state -> prefill -> decode loop (forward, argmax, sampler or greedy) -> close in synced rounds.

| step | Qwen (`form-cli-model-session.fk`, `qwen35-dense-token-handle.fk`) | DeepSeek (`form-cli-model-ds4.fk`, `dsv4-open/-layer/-token-handle.fk`) |
|---|---|---|
| door dispatch | `fcmg` and `fcms-open-body` gate on `mr-wired?` (registry row `qwen35 q38 1`); generate `~:598` arch gate | `fcds-route` behind `fcds-lane-wired?` (0) and the registry row `deepseek4 ds4-query 0`; they flip together after the window (`dsv4-door-band` fails when they differ) |
| seal and crystal | `qaf-seal-verdict`, `qsx-load` | the seal verdict first (same door); the tokenizer is read from the file's own header (`dsv4-tokenizer.fk`: gpt2 byte-level BPE, 129,280 tokens), raw ids, no template, no begin token |
| lease | `mg-lease(file size, kv margin)` | `mg-lease(derived working set at the session's capacity, kv margin)` (`dsv4-lease.fk`): the 80 GB file is touched in part |
| context | `q38-open-span`: 11 slots | `dsv4-open-span`: the same 11 slots plus two (the scratch record, the view record), so `q38-context-ok?`, `q38-close`, `q38-free-state` apply |
| weights | one no-copy mapping a tensor (`kth-tv-at`) | the same, per tensor, from a plan computed from the header (`dsv4-bind.fk`): 1,202 views under the 65,535 handle wall; a stack up to 1.14 GB is one view the kernels index by expert |
| per-layer state | KV rows and the DeltaNet recurrence | raw KV arena, the compressor's state and score (ratio 4: two-lane overlap, coff 2), the compressed rows |
| block | the layers of GQA attention and gated delta net | 43 layers of: four hyper-connection streams with a Sinkhorn split, MLA with a second cache, router by hash (layers 0 to 2) or biased top-6, six routed experts plus one shared, hc_post |
| kernels | emitted by `q38-msl` | emitted by `dsv4-kernels.fk`: 11 sources (8 in the graph), every source opening with the two fp pragmas (`metal_pipeline` compiles with fast math and contraction on) |
| quantization | Q8_0, Q4_K | per tensor by its own GGUF type: dense MXFP8 or Q8_0, experts MXFP4, IQ2_XXS (gate, up) and Q2_K (down) |
| barriers | hand-threaded `barrier` arguments through `q38-*` | derived: each pipeline's writes are read from its MSL signature, the barrier bits planned once a block and replayed (`dkr-hz`) |
| batch | `metal_batch_concurrent` per token, the argmax read drains it | the same, plus a batch handed to the queue every second block (Swift's `submitEvery`) so the device runs block n while the host encodes block n + 1 |
| head and argmax | `q38-head`, device argmax, one 4-byte read | `dsv4-head` (7 enqueues), device argmax, NaN scan in the same batch, one sync, two 4-byte reads; logits at `bs[28]` where the sampler reads |
| sampler, observations, draft, adapter | wired | greedy only; a DeepSeek session takes no observation (`ds4-observation-not-wired`) |
| close | `q38-close`: synced rounds | `dsv4-close`: the same, answering the handles still held |

## 3. The op ledger: Swift runner, the Form line, main before and after

One row a token. Swift from metal_dsv4_stack.sh (history) and its receipts; the fixture rows are `dsv4-ledger-band.bml`'s; the real-file figures are derived from the layout (2 plain, 21 ratio-4 and 20 ratio-128 blocks, 3 hashed).

| per decode token | Swift runner (A) | Form line B (`985b00bc3`) | main `95f5b0e9c` | this change |
|---|---|---|---|---|
| dispatches | ~2,075 to 2,290 | ~2,234 | 1,856 (derived: attention 44 + 1,025, FFN 57 + 720, embed 2, head 7, scan 1; +126 at a ratio-4 window boundary) | 1,856: the formula `dll-token-count`, held by the band at every step |
| barriers | ~1,510 (hazard tracker) | by hand, per call site | 0 (serial encoder: every dispatch a full barrier) | 111 of 137 on the fixture (81%); derived ~1,500 of 1,856, the Swift count |
| command buffers | ~22 (submit every 2 layers) | 1 | 1 | floor(layers / 2) + 1: 2 on the fixture, 22 derived |
| host waits | 1 | 1 | 2 or 3 (argmax sync, the NaN scan's own sync, its buffer's free) | 1 |
| host reads | 1 | 1 | 2 | 2 (the count and the id), counted at the read door (`dkr-rd`), not set by hand: the band's plant of two more reads reads 4 |
| buffers made a token | 0 after the first pass (pool) | 0 | 1 (the scan's four-byte buffer, freed in synced rounds) | 0 |
| expert ids | on the device (the per-layer flush "outlived its reason") | on the device | on the device | on the device |
| weight bytes | 9.56 GB (7.73 dense, 1.83 experts at 6 of 256) | the same | the same | the same: `dll-token-bytes` reads it per token and per block (1,285,844 on the fixture) |
| host cost a dispatch | ~1.1 us (Swift) | 2 to 4 us encode (receipt: 5 to 10 ms a token) | **66 to 76 us** (best of six, 16 tokens of 137 dispatches: 146 to 166 ms; a first, noisier reading said 160) | **24 to 26 us** (best of six: 53 to 56 ms; 17 to 20 us in a quieter hour: 38 ms best of five). Both at a load average of 33 to 43: the ratio is 2.7 to 3, the absolute figures move with the machine |
| warm token | 34 to 38 ms | 226 ms | not run on the real file | not run on the real file; the host floor is derived 45 to 50 ms (1,856 x 24 to 26 us; 33 to 37 ms at the quieter figure), overlapped with the device by the two-layer submits |

**What the host cost taught.** The Form lane's price of a dispatch is not Swift's 1.1 us: it is the interpreter building a binding and resolving names. Measured on the fixture 2026-10-03 (a machine at load average 33 to 43, micro-loops of 200 to 5,000 calls, so read them as orders of magnitude):
`md-bind` of an eight-word binding 36 us (`md-byte-at` takes `fq-pow-int(256, k)` four times a word); `dkl-supported?` 700 us a block, read twice a block and once a block again in `dsv4-admitted?`, every token; `dkl-clamp` 230 us (`gmt-find` over the key-value table: the real header holds
129,280 tokens), twice a block; the NaN scan a buffer, three syncs and a free a token. None of it is the device. The five repairs: the binding built the old Form stack's way (`dkr-bind`, 4 us, bytes equal to `md-bind`'s), the admission read once a context, the clamp
read once a block, the scan in the token's own batch, the barriers planned once. The consequence for design: a dispatch costs this lane about 25 us, so a fusion Swift measured as null on the device (the MoE pair and sum fusions: 129 fewer dispatches bought 0.14 ms, the
simdgroups cost more) is worth about 3 ms here (129 x 25 us). They are kept out for now because the device number is unmeasured; they are the first lever when the window reads the per-token ledger.

**The kernel choices Swift measured, and what this lane takes** (from the runner's own comments; `dkr-unit-lanes`):

| choice | Swift's measurement | here |
|---|---|---|
| IQ2_XXS gate/up: `experts4_fast` (four rows a simdgroup, `as_type<half>` decode, packed grid reads) | the software f16 decode cost 4 of 7 ms; the reads alone are 3 of 7 ms at 372 GB/s: the cost is the decode, ALU-bound | taken (`form_dsv4_iq2_matvec_experts4_fast`, 64 threads a group) |
| the span map, the threadgroup grid table, the ALU and memory probes | null (43 -> 44 ms; 24.34 against 24.45 t/s) | not taken |
| Q2_K down: four-row `experts4_fast` over one thread a row | the one-thread down ~3.2 ms; 36 scalar loads to 9 measured null on its own (41 ms either way); the 32-lane and four-row forms won | taken (`form_dsv4_q2k_matvec_experts4_fast`) |
| MoE fusion: `pair_swiglu`, `q2k_sum` | 2,204 -> 2,075 dispatches, 26.46 -> 26.15 t/s: the trade went the other way on the device | not taken (see above: worth ~3 ms of host here, to be priced by the window) |
| dense Q8_0: ds4's Metal-order matvec (128 threads a row, activation f32) | 55 ms against 59 for the lane form, and the more accurate arm; the int8-activation arm is ds4's CPU stream | taken (`form_dsv4_q80_matvec_ds4`); the int8 arm is not (it would put the head 1e-3 off the fp64 oracle the bands hold) |
| two rows a thread for the ordered Q8_0 | 48 -> 51 ms | not taken |
| hc split on one thread (`seq4`) over four threads with 40 barriers | 21.9 us against 1.1 us a call, 83 calls a token: 1.8 ms | taken (already in the graph) |
| threadgroup 64 for the expert matvecs under a concurrent encoder | 26.38 t/s at 64, 26.20 at 256, 26.12 at 512 | taken |
| concurrent encoder with a hazard tracker; a barrier naming its buffers | the named-buffer barrier: 1,844 barriers and 39.14 ms against 1,510 and 38.17 | the scoped barrier, planned |
| the per-layer router flush | 43 blocking waits a token, deleted when the expert kernels took the ids buffer | no flush |
| the per-pass buffer pool | `makeBuffer` was 5.0 of 12 host ms | scratch made once at the open, no buffer a token |

## 4. The restoration, and the window

Kept from main's stages 1 to 4 (the fixture-proven kernels, layout, bind, open, layer, token handle) and changed in place: `dsv4-kernels.fk` (the binding, the plan, the lanes unit), `dsv4-layer.fk` (the driver's entry without re-admission, the memoized clamp, the lane kernels),
`dsv4-token-handle.fk` (admission once, the planned stages, submits, the scan in the batch, the one sync), `dsv4-open.fk` (the scan's buffer), `dsv4-ledger.fk` (new: the row). The arena builder's branch (`claude/dsv4-arena`) reads the ids on the host after the router, one sync a layer
(43 a token): the finding above is that Swift needed none; the arena's per-layer sync and this lane's whole-token batch exclude each other unless the arena's sync becomes a plan boundary (a stage plan per half and a read between them), and the arena's `dkl-experts-with` takes the
stride the lane kernels bind from the view's bytes over its expert count.

**The next levers, ranked by dispatches a token they remove (at ~25 us of host each; none measured on the device, all exact rearrangements by Swift's own receipt where it measured them).** The census of a block: attention 22 (hc_pre 4, norm 1, q_a 1, kv 1, q_a norm 1, kv norm 1,
q_b 1, head norm 1, two ropes 2, kv round 1, append 1, attention 3, inverse rope 1, grouped output_a 1, output_b 1, hc_post 1; the compressor adds 3 and, at a window boundary, 5 or 6), FFN 18 or 19 (hc_pre 4, norm 1, router 1, routing 2 or 3, experts 4, shared 4, slot sum 1, hc_post 1).
The two hc_pre chains a block (rmsnorm, F16 fn matvec, Sinkhorn split, stream sum: 8 a block, 344 a token) are the largest family and have no fused kernel in the tree; the MoE pair and sum fusions (86 and 43, in `ds4-order-match.fk`); the two ropes in one dispatch
(43); the shared expert's gate and up in one (43). Roughly 550 of 1,856, about 14 ms of host, if the device does not pay for the fewer, wider dispatches (Swift's null on the MoE pair says the device may not).

**The first token cold is a disk read, not a driver cost.** Both Form lines and the Swift runner read ~9.5 GB the first token from an 80 GB file mapped no-copy and faulted in on first touch (the Swift runner's first passes fault the whole file: 26 s; B read 36.7 s). A first token inside seconds
needs those pages already resident, which this lane cannot ask for (no madvise or readahead door: `metal_status` and `metal_live` read counters, nothing prefetches); the arena builder's expert slot arena is the other design (only the experts a token uses, filled by span).

**In the real window the lead should read, in order:** (1) the open's stage deltas (unchanged); (2) the first token's ledger line (`dll-line`): dispatches, barriers, cbs, syncs, gpu_us, wall_ms, miss (must be 0); (3) the warm per-token wall_ms against gpu_us: wall far above gpu_us is host (encode),
wall near gpu_us is the device; (4) per block, the stage rows (`dsv4-decode-staged`); the token's bytes over gpu_us is the effective bandwidth against the 477 GB/s floor. The ceilings are in `dsv4-ledger-band.bml`; a real-file ceiling is pinned from the first clean window.
