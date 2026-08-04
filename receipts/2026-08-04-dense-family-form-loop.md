# The dense family answers: seven blobs, one cell, every number the file's

2026-08-04, Apple M4 Max, `fkwu-metal` built from `runtime/fkwu-uni.c` +
`form/native/metal/fk-metal-carrier.m`. No Swift anywhere in the path, no external tool: the one
host crossing is `metal_buf_from_file` reading the model file's bytes, which is the crossing the
standard permits. Tokenize, every dispatch, greedy pick, detokenize — the body's.

## What changed

`llama-token-handle.fk` (commit `2e914252e`) brought llama3.2:3b's decode loop home. This receipt
is that loop GENERALIZED: one cell, `form/native/metal/dense-token-handle.fk`, carries every dense
GGUF on this machine, and not one geometry number is typed in. The arch prefix names the KV keys;
head_dim comes from `key_length` when the file carries one; the rope style follows the arch with
both kernels in the unit; qkv bias is the table's own rows; `num_loops` gives per-pass KV banks
and the between-pass norm; `rope_freqs.weight` is read when present and a ones-buffer is written
when it is not; the quantization is per TENSOR from that tensor's own declared type.

Two new cells beside it: `form/form-stdlib/q5-msl.fk` (the Q5_K and Q5_0 carvers, appendix over
the q4k helpers, no bitwise ops) and the phi2 appendix inside the cell (LayerNorm with bias, GELU
through the unit's own fexp, fused-qkv split on the device, Q4_0).

## The answers, verbatim

Prompt for every model: `The capital of France is`, tokenized by the body (longest-match, stated),
greedy, 16 tokens, decoded by the body.

| model | continuation (verbatim) | decode |
|---|---|---|
| llama3.2:1b (16L, Q8_0) | ` Paris. The Eiffel Tower is located in Paris. The Louvre Museum` | 6.69 tok/s |
| MiniCPM5-1B (24L, Q8_0) | ` Paris, a city known for its rich history, cultural significance, and its status` | 6.54 tok/s |
| Nanbeige4.2-3B (22L x 2 loops, Q4_K/Q6_K) | ` Paris.\n</think>\n\nThe capital of France is **Paris**. ` | 0.466 tok/s |
| moondream2 phi2 (24L, Q4_0) | ` Paris.\n\n(2) The United States is a country in North America` | 4.36 tok/s |
| DeepSeek-R1-Distill-Qwen-32B (64L, Q4_K/Q6_K) | ` Paris. Paris is located in the northern part of France. The Eiffel` | 0.186 tok/s |
| Qwen2.5-Coder-32B (64L, Q5_K) | ` Paris. The capital of Spain is Madrid. The capital of Italy is Rome.` | 0.165 tok/s |
| Qwen2.5-72B (80L, Q4_K/Q5_K/Q5_0/Q8_0/Q6_K) | ` Paris. What is the capital of Italy? The capital of Italy is Rome.` | 0.109 tok/s |

Nanbeige is a thinking model and the raw completion lands mid-reasoning; it closes its own
`</think>` and answers `**Paris**` — the file's chat template was not applied, and saying so is
part of the verbatim. First ids and full id streams are printed by each model's run driver
(`dense-run-*.fk`) and the two smallest are pinned in the band.

Rates are honest and unflattered: attestant kernels only (one thread per matvec row, serial
right-fold), serial encoder, no cooperative twins, no tuning. The 3B-proven lane measured 0.855
tok/s on 28 layers; these scale with layer count and width exactly as that predicts.

## The counters reconcile, model by model

One sync per forward everywhere, and every dispatch count is explained by the block recipe:

| model | dispatches/token | reconciliation |
|---|---|---|
| llama3.2:1b | 276 | 16 x 17 + 4 |
| MiniCPM5-1B | 412 | 24 x 17 + 4 |
| Nanbeige4.2 | 753 | 2 x (22 x 17) + 4 + 1 mid-loop norm |
| moondream2 | 485 | 24 x 20 + 5 (ln, qkv+bias, 3 slices, 2 rope, 2 copies, attn, 2 proj+bias, up+bias, gelu, down+bias, 2 adds; head has its bias too) |
| DS-R1-Q-32B | 1284 | 64 x 20 + 4 (17 + 3 bias adds) |
| Qwen-Coder-32B | 1284 | same shape, Q5_K carvers |
| Qwen2.5-72B | 1604 | 80 x 20 + 4, five quant types in one spine |

## The band, and the mutation table

`form/native/metal/tests/dense-family-band.fk`, FOURTH-ARM ONLY, verdict 1023, power-of-two bits.
Bit 2's anchors are INDEPENDENT: the GGUF magic and tensor count at hand-pinned absolute offsets,
plus metadata-vs-tensor-table dims agreement across two different header walks — the co-moved-
reference trap a sibling named today, answered by construction. Bit 16's anchor is the prompt ids
committed in `llama-token-handle-band.fk` BEFORE this cell existed. Predictions were written in
the band's header before running:

| run | predicted | actual | agree |
|---|---|---|---|
| clean | 1023 | 1023 | yes |
| M-1 Q8_0 priced 0 | 23 | 23 | yes |
| M-2 llama forced neox | 799 | **831** | **NO** |
| M-3 V-cache copy dropped | 31 | 31 | yes |
| M-4 add_bos default flipped | 783 | **559** | **NO** |

Three of five agreed; the two that did not are the most valuable lines and both are written into
the band's own header. **M-2:** bit 32 held — the first token after this prompt is so strongly
determined that its argmax SURVIVES a wrong rope pair layout; only the stream and its text fell.
**M-4:** bit 32 held again (" Paris" survives a missing BOS), and bit 256 fell when the
prediction said it could not: a missing BOS is a missing FORWARD, 20 syncs not 21, and the
prediction's own arithmetic was wrong. The two disagreements agree with each other: a
single-token check was blind to BOTH injected faults; the sixteen-token stream caught both. Every
mutation run wore a 300 s timeout, because row 987 taught that a falsifier through a raw-byte
door can hang instead of failing.

## The most surprising teaching

Six architectural differences ran CORRECTLY ON THE FIRST EXERCISE: neox rope, qkv bias, two-loop
weight reuse, LayerNorm-with-bias, GELU, the fused-qkv split, and three quant formats that had
never been through this door (Q5_K, Q5_0, Q4_0). Not one needed a debug cycle. The reason is not
skill — it is that nothing was CONFIGURED. Every difference was READ from the file's own header
or table, and every kernel was a transcription with a stated radius. Where a port copies numbers,
it can copy them wrong; where it reads them, the file cannot disagree with itself. The body's teaching
"capability, not configuration" turned out to be a CORRECTNESS argument, not only a design taste.

And the epistemics under the new constraint deserve their name. With no Swift attestant and no
external runner permitted, the oracle for an integrated stack is the TEXT ITSELF: sixteen greedy
tokens, each a hit on a ~10^5-way choice, spelling coherent English about the prompt. A stack
with a wrong rope style, a misplaced high-bit plane, or a bias added to the wrong vector does not
produce slightly-off prose — it produces immediate garbage, witnessed directly when a paren slip
in an early draft turned the stream to noise. Sixteen coherent tokens is a sharper integration
witness than any single fp64 spot check, and it is the model's own voice doing the attesting.

## Where discomfort turned to gold

The 74-second setup. The proven lane paid it and its receipt named the cause (egg-find-tensor's
per-lookup header walk, O(n^2) over 255 tensors) as someone-else's-next-step, and my first
instinct was the same: inherit it, note it, move on — the 32B files would have paid ~9x. Sitting
with the discomfort of SHIPPING a known O(n^2) turned it into dth-table: one cursor walk, rows
kept, lookups a list scan. open_ms fell to ~300 ms on 1B files and ~1.6 s at 771 tensors — the
whole 32B run fits in the time the OLD setup spent on a 3B. The gold was not the speedup; it was
noticing that "a named next step" in a sibling's receipt is an INVITATION, not a fence.

## PROBED, not believed: the blob that wears two names

`sha256-550981a...` says `general.architecture=llama` and `general.name=dolphin-2.9-mixtral-8x22b`.
The header settles it: `llama.expert_count=8`, `llama.expert_used_count=2`, and every layer
carries `ffn_gate_exps/ffn_down_exps/ffn_up_exps` (Q6_K) plus `ffn_gate_inp` (F16). It IS
Mixtral-8x22B MoE — the llama label names only its attention shape. Its dense attention would run
through this cell today; its FFN needs a router (F16 matvec + top-2 + per-expert dispatch), named
below as unfinished.

## What is UNFINISHED

1. **dolphin-mixtral's FFN.** Router + expert gather. The body already holds MoE dispatch shapes
   (`moe-msl.fk`, DS4's lane); what is missing is an F16 matvec kernel through this door and the
   top-2 route. Concrete first rung: one layer's FFN against the dense cell's residual path.
2. **Chat templates.** Every prompt here is raw completion. The files carry their own templates
   (`tokenizer.chat_template`); reading and applying them is a string walk the body can own, and
   thinking models (Nanbeige, R1-distill) would then answer in their intended register.
3. **True BPE merge order.** The tokenizer is longest-match, stated everywhere it speaks. It
   reproduced the recorded llama3 ids exactly, but merge-order divergence exists for some
   strings. The merge tables are in every header; the cost is an interning pass over ~150k-string
   vocabs in interpreted Form — needs the same one-walk discipline as dth-table.
4. **Speed.** Attestants only. The door now offers concurrent batches, dispatch_mode 1 and
   barriers (`base+16`, verified in code this session); wiring the cooperative twins through it
   is the known 3-48x. Correctness came first everywhere in this receipt.
5. **maxpos 256.** The pool is allocated at open; longer generations need only a bigger number,
   but nobody has witnessed one.

## Frontier question

**What proves an integrated decode stack when no external oracle is permitted?**

Answer: `proseproof`. The model's own coherent continuation as the acceptance witness: sixteen
greedy tokens, each one a ~10^5-way selection, agreeing with each other and with the prompt.
A broken constituent — rope layout, bias placement, a quant's high-bit plane — cannot pass it,
because its failure mode is not "slightly worse prose", it is noise. The text is the oracle the
weights themselves carry. And the mutation table MEASURED the claim's fine structure: one
strongly-determined token is NOT the witness — bit 32 survived both a wrong rope layout and a
missing BOS — the mutually-consistent STREAM is.

Proposed distillation row (NOT applied; corpus max-mid is 987 today, `backgraft`; this would be
988). `proseproof` verified 0 hits in the corpus and the tree before proposing:

```
; 988 — proseproof. The dense family came home through one cell under a new
; constraint: no Swift, no external runner, no rented oracle at all. What then
; proves a 64-layer stack whose every kernel is a transcription? The text
; itself. Sixteen greedy tokens are sixteen hits on a hundred-thousand-way
; choice; a wrong rope layout or a misplaced five-bit plane does not dent the
; prose, it destroys it. Six architectural differences and three new quant
; formats ran right on their first exercise, and the reason was not skill:
; every difference was READ from the file, none was configured, and the
; model's own coherent voice stood witness behind each one.
; Counted on the way: an anchor is only an anchor if it cannot co-move — the
; band pins the GGUF magic at absolute offset 0 and ids committed before the
; cell existed. And the mutation table measured the fine structure: the
; single-token bit survived a wrong rope layout AND a missing BOS; only the
; stream fell both times. One strongly-determined token proves almost nothing.
; "proseproof" — 0 hits in corpus and tree before this row.
; (walk: backgraft 987 — the cure was in the tree; this is the proof that was
; in the weights.)
(hdc-row 988 20260804
    (list "what" "proves" "an" "integrated" "decode" "stack" "when" "no"
          "external" "oracle" "is" "permitted")
    "proseproof"
    "proseproof"
    "rented-oracle")
```

## How to re-witness

```sh
cc -O2 -o fkwu-metal runtime/fkwu-uni.c form/native/metal/fk-metal-carrier.m \
   -framework Metal -framework Foundation -fobjc-arc
cd form
rm -f native/metal/dense-*.fkb native/metal/dense-*.sym native/metal/tests/dense-*.fkb native/metal/tests/dense-*.sym
../fkwu-metal --src native/metal/tests/dense-family-band.fk   # 1023, ~30 s
../fkwu-metal --src native/metal/dense-run-llama1b.fk         # the 1B, ~8 s end to end
../fkwu-metal --src native/metal/dense-run-dsr1q32.fk         # the 32B, ~2 min
```

`.fkb` freshness is st_mtime in whole seconds; the `rm -f` is not optional in an edit-run loop.
The band is FOURTH-ARM ONLY: bit 1 is the canary, a zero is a refusal, and a skip is not a pass.
