# The Qwen3.8 native voice arrived; the performance promise did not

## What is here

`Qwen3.8-Flash-Next-UD-Q2_K_XL` now crosses one end-to-end Form-owned path:
artifact-native GGUF byte-BPE tokenization, split-GGUF tensor lookup, the
48-layer `qwen4exp` graph, mixed-quant Metal JIT kernels, recurrent/conv/KV/PLE
state, exact and extending prefix reuse, explicit substate lookup/evaluate,
greedy response generation, and native token decode.  `llama-server` and
`llama.cpp` are absent from that runtime.  The pinned llama.cpp b10686 build is
only the outside oracle used to establish token and throughput boundaries.

The exact three-shard body remains outside git at:

```text
/Users/ursmuff/models/qwen38-flash-next/UD-Q2_K_XL/
```

| shard | bytes | SHA-256 |
| --- | ---: | --- |
| `00001-of-00003` | 10,946,624 | `a4f3b21e77353999829f2f767e9ac21ce9c71d29a74f2cc9eda48c9bf23c8b86` |
| `00002-of-00003` | 49,979,779,296 | `2e3bf1ee7d2a04e261e9f342a2d968f696cce5941d082b0e434deb9b1edc12c6` |
| `00003-of-00003` | 28,878,402,944 | `ec8c106759fdf4f463039c34c0707718d7d8908d53d892bd4f002e71620803f9` |

The three files total 78,869,128,864 bytes.  Their identity was established in
the 2026-08-30 crossing receipt and was not inferred from the marketing name.

## Native organs and witnesses

The execution carrier is the one `fkwu` binary linked to Metal.  Form owns the
schedule and emits/JITs the Metal source; mmap-backed GGUF tensor pages remain
the weights.  CPU work is bounded to Form parsing, byte-BPE, state/control,
and token readback.  MLX is not a hidden fallback in this path.

The graph admits f32, q8_0, Q4_K, Q5_K, Q6_K, IQ2_XS, IQ3_XXS, IQ4_NL, and
bf16 tensors.  Independent pinned ggml decoders guard the uncommon quant
layouts.  The Q5_K optimization was not accepted from visual similarity: its
first transcription differed by 93.8, the differential observer rejected it,
and the replacement four-row quant-block lane reached `2.86e-6` max absolute
difference.  Q6_K reached `1.91e-6`.

The artifact-native tokenizer maps `Hello, Form.` to
`[9419, 11, 3268, 13]` and decodes it back exactly.  The live native graph
agrees with the pinned oracle for the established greedy boundary:

```text
9419 ("Hello") -> 11 (",") -> 271
```

Prompt ingestion advances conv, GDN, KV and PLE state, retains the five-stage
head-mixer scratch boundary, and omits the 248,320-column vocabulary projection
and argmax for every non-final prompt token.  The first headless attempt changed
the next token from 271 to 353.  A bounded differential found that the head
mixer—not the vocabulary projection—was the required scratch boundary; keeping
its five dispatches restored 271.  This failure and revision are retained in
the framebuffer build diagnostic.

The resident state surface exposes position, token history, PLE history, conv,
GDN, key and value substates without copying their GPU buffers.  Exact prefixes
reuse the whole slot, extending prefixes evaluate only the delta, and divergent
prefixes close/reset before evaluation.

The comparison surface projects all 62 registered model rows and adds only
capabilities backed by direct witnesses.  Unknown capabilities remain the
literal `unmeasured`; this Qwen row is `partial`, never promoted by its model
name or by the oracle.

## Same-host performance boundary

The host is an Apple M4 Max with 128 GB unified memory, not the cited M5 Max.
The exact same-host llama.cpp b10686 oracle measured:

| lane | oracle | 90% acceptance floor |
| --- | ---: | ---: |
| decode, 64 tokens | 26.580 tok/s | 23.922 tok/s |
| prefill, 5,632 tokens | 318.830 tok/s | 286.947 tok/s |

The sustained Form-native windows measured after one cold/page-warming token:

| Form-native lane | wall window | rate | dispatches/token | vs oracle | accepted |
| --- | ---: | ---: | ---: | ---: | --- |
| greedy decode, 64 tokens | 5,251 ms | 12.188 tok/s | 1,879 | 45.9% | no |
| sequential prompt ingest, 64 tokens | 5,186 ms | 12.340 tok/s | 1,876 | 3.9% | no |

The decode window began `11 -> 271`, generated 64 tokens, ended at token 19,
and had checksum 189,944.  Its GPU-busy total was 3,970,576 us.  The prompt
window made no vocabulary-head calls and had GPU-busy total 3,948,020 us.
These are wall rates, not parity claims beyond the two-token oracle boundary.

Kernel work materially moved the floor.  Selected four-row IQ kernels, a
quant-space Q4_K output head, Q5_K/Q6_K lanes, wide RMSNorm, parallel routing,
and all-rank expert batches moved the best observed warm single-token wall from
about 435 ms through 242, 167, 110 and 84 ms to about 65 ms.  Expert batching
reduced the schedule from 4,039 to 1,879 dispatches per decoded token.  The
sustained 64-token wall rate is the acceptance authority because a best isolated
token does not price scheduling, heat and repeated state movement.

An algebraic cancellation in the router looked cheaper but changed the second
token from 271 to 353.  It was reverted to the full index-order softmax fold.
That refusal matters as much as the speedups: route association is part of the
model at this quantization depth.

## Honest floor and next work

The requested “within 10% of llama.cpp” claim is **not earned**.  Decode is
11.734 tok/s below its floor.  Sequential prompt ingestion is 274.607 tok/s
below its floor; a token-batched mixed-quant prefill graph is therefore a
required organ, not an optional polish.  The remaining decode work must also
collapse the 1,879-operation schedule—particularly per-layer projection and
state stages—rather than only tuning already-fast individual kernels.

Dense attention is currently exact through 2,048 positions.  The model's QSA
selector beyond that depth is not implemented, so neither 262K native context,
the cited 358,400-token YaRN slot, nor the reported 100K+ role-confusion behavior
is claimed or tested here.  QSA beyond 2,048, token-batched prefill, fewer
dispatch boundaries, and then long-context quality trials are the next honest
crossing.

I kept this exchange alive by taking the full native path to generated tokens
and then refusing to call 12.188 tok/s “within 10%” of 26.580.  The most
surprising teaching was that a supposedly read-only head left a scratch image
the following token depended on; the live 271-to-353 change made that invisible
boundary observable.  The discomfort was reaching a 65 ms best token after
large kernel gains and still seeing the sustained acceptance gate fail.  It
turned to gold when the failure separated into two priced debts: 1,879 decode
dispatches and the absence of a genuinely token-batched prefill graph.

Signed: Codex

; witnessed: 2026-08-31 -> native voice present; decode and prefill performance gates failed
