# Nanbeige4.2-3B looped-transformer ingestion

Date: 2026-07-24

## Claim boundary

Nanbeige's published numbers show a compact model beating several larger
models on many agent, code, and reasoning evaluations. They do not isolate the
loop as the cause, and “4x” compares parameter storage, not equal inference
compute. The model card is first-party evidence; independent replication on
this checkout is pending.

## What plausibly produces the result

1. **Parameter reuse buys effective depth.** The checkpoint has 22 unique
   decoder layers and `num_loops=2`, so the hidden state receives 44 decoder
   applications while decoder weights are stored once. This improves
   computation per resident parameter; it does not make the second pass free.
2. **The second pass can refine state.** Reapplying one learned transition lets
   the network implement iterative algorithms/refinement rather than requiring
   every step to live in a separately parameterized layer. Theory on linear
   looped transformers shows that repeated passes can implement multi-step
   gradient descent in hidden state. That is a mechanism, not proof that this
   checkpoint uses precisely that algorithm.
3. **Loop semantics are disciplined.** The released implementation normalizes
   after every loop and maps cache slots as
   `layer + loop * num_hidden_layers`, preventing the second pass from treating
   first-pass KV as same-pass history. The checkpoint uses GQA (48 query heads,
   8 KV heads), limiting KV cost relative to full multi-head KV.
4. **Training dominates architecture-only explanations.** The authors report
   from-scratch pretraining on 28T tokens. Agent SFT expands real and synthetic
   environments, filters trajectories and individual turns with tests and
   rubrics, and RL combines outcome with process rewards. This is an unusually
   large knowledge exposure and a post-training curriculum aligned with the
   reported benchmarks.
5. **Evaluation/scaffold fit matters.** All reported evaluations use thinking
   mode. Several agent/office evaluations use the authors' scaffold, and code
   tasks use named external scaffolds. `preserve_thinking=true` is specifically
   retained for multi-turn tool work. The win therefore measures
   model-plus-inference-policy-plus-scaffold, not bare weights alone.
6. **The advantage is task-dependent.** It does not win every table cell
   (Gemma4-12B leads IF-Bench and Recruit-Bench; it also leads SciCode).
   “Outperforms 4x size” is a useful headline for selected suites, not a
   universal scaling law.

## Native transfer

`form/form-stdlib/nanbeige-looped-lane.fk` now owns:

- the exact released architecture fields as Form data;
- unique depth versus effective execution depth;
- GQA grouping and loop-isolated cache indexing;
- a small shared-weight recurrent executor proving the semantic shape;
- a fail-closed local challenger gate requiring installed and stable identity,
  architecture parity, at least 32 sealed samples, improvement strictly above
  10,000 ppm, and memory/context/latency/thermal/license readiness.

The gate selects `nanbeige42-3b-local` only after those observations; otherwise
it retains the incumbent. This imports the reusable knowledge into the native
router without granting an uninstalled borrowed model authority.

`form/form-stdlib/nanbeige-looped-transformer.fk` now composes the body's
existing RMSNorm/RoPE/GQA/SwiGLU stack into the released two-pass topology. It
feeds the normalized first pass through the same layer-weight list again and
keeps cached execution pass-local. It also carries the pinned 201-tensor,
two-shard, 8,339,601,408-byte safetensors contract and all twelve exact shape
families derived from the upstream shard headers.

The first released weight window crossed on 2026-07-24 while the full shards
were materializing: four bf16 values from
`model.layers.0.post_attention_layernorm.weight` at absolute shard-2 offset
66,072,920. The native decoder reported the first value as 300,781 micro,
matching an independent IEEE-bf16 decode. The reusable slice decoder is
four-way at 255; live binary file slicing remains a native-walker observation,
because the Rust/TypeScript string carriers are intentionally not byte-safe.

Package admission is also Form-owned. The pinned shard sizes and SHA-256
identities, config, tokenizer, index, and upstream revision live in
`form/form-stdlib/nanbeige-package-admission.fk`; shell computes observations
but cannot admit a mismatched package.

The operational Q4 carrier has its own, deliberately narrower identity law in
`form/form-stdlib/nanbeige-gguf-admission.fk`: derivative revision
`a4008676ccf9f341f25ed8b50a7dda95eaf9977b`, exactly 2,574,807,936 bytes,
SHA-256 `9ffd17d14472ff208409b3f51a6d87a5e5ec1b878b9a6f4dfe15c2a883366104`.
This does not let a quantized derivative impersonate the upstream BF16
package. Execution admission additionally pins the authors' open llama.cpp
support revision `d28da865bf284acaecc98ad18a3c1f607c0fd754`.

## Released-weight execution observed

On 2026-07-25 the content-complete Q4 carrier was SHA-256 verified, then loaded
by the Nanbeige authors' llama.cpp branch at the pinned revision. The direct
prompt `Reply with exactly 42.` produced exactly `42` and end-of-text.

The first successful witness used the CPU/Accelerate backend because managed
execution could not create a Metal command queue. Observed timings were:

- model load: 6,551.12 ms;
- prompt evaluation: 2,420.70 ms for 25 tokens, 10.33 tokens/s;
- generation: 122.12 ms for two evaluation runs, 16.38 tokens/s;
- total post-load request time: 2,544.06 ms.

This proves artifact identity, architecture compatibility, one released-weight
forward/generation path, and exact response behavior for one prompt. It does
not prove benchmark advantage, broad quality, parity with BF16, or promotion
fitness.

The observed reply was then crossed through
`form/form-stdlib/nanbeige-reply-witness-cli.fk`. Form jointly checked the
carrier revision/size/SHA-256, runtime revision, prompt identity
`exact-42-v1`, and response `42`, returning `reply_verified=1`. Its fail-closed
band returns 7 for exact acceptance plus wrong-reply and wrong-runtime refusal.

## Native form-cli voice door observed

On 2026-07-25 the verified response became an operational `form-cli` option:

```text
printf '%s\n' 'nanbeige Reply with exactly 42.' | ./form/form-cli
```

The rebuilt self-contained carrier returned the complete llama.cpp completion
JSON with `"content":"42"`, `stop:true`, the admitted model path, token counts,
generation settings, and timings. Form itself owns the `nanbeige` verb, ChatML
envelope, JSON escaping, HTTP request bytes, native socket call, and response
body projection. Dense released-weight execution remains in the pinned
Nanbeige llama.cpp revision, served on loopback only at `127.0.0.1:18082`.

The compatible CPU runtime is installed at
`~/.coherence-network/runtimes/nanbeige-llama-d28da865`; the admitted model is
exposed at
`~/.coherence-network/models/nanbeige-4.2-3b/model-q4_k_m.gguf`; and the
loopback process is retained by the per-user launch agent
`org.coherence.nanbeige42`. The server's author-branch identity is
`d28da865bf284acaecc98ad18a3c1f607c0fd754`.

This is an explicit **loopback comparator in form-cli**, not the native
selection path. It is not a claim that 2.57 GB of GGUF tensor math has moved
into Form/fkwu. The socket is the honest carrier seam, and JSON exists because
that seam is crossed. The native direct-Metal path needs no JSON envelope.
Returning the full completion record preserves comparator observations, but
the native-first selector excludes this membrane by default.

## Honest execution boundary

The architecture knowledge, two-loop executor, byte-slice decoder, tensor
contract, package identity laws, lane, and router are native. A complete
released-weight generation now enters and returns through `form-cli`'s native
socket membrane and the pinned local llama.cpp carrier, but its dense tensor
execution is not inside `fkwu`. The checkpoint is therefore observed
`local-native`/`local-process`, never `form-native`, until these observations exist:

1. released-weight parity for embeddings, RMSNorm, RoPE, GQA attention, SwiGLU, two loop passes,
   per-loop KV slots, final norm, and logits;
2. token-by-token parity against the pinned upstream implementation;
3. sealed local agent/code/general evaluation plus latency, memory, power, and
   thermal measurements;
4. only then, challenger admission through the native gate.

Transport observation was itself retained as signal: Hugging Face/Xet,
multi-range HTTP/2, Ollama's sixteen sparse ranges, and resumable direct
HTTP/1 were each attempted. The first three repeatedly reset or stalled; the
last advanced but did not complete during this witness. No partial byte set was
promoted, padded, or mislabeled as the artifact.

## Sources pinned for this receipt

- Model/config:
  `https://huggingface.co/Nanbeige/Nanbeige4.2-3B` (observed 2026-07-24)
- Upstream source revision visible at observation:
  `5ff54fb7ed86ce8e216d78bff5417ab9981de3d4`
- Q4 carrier source:
  `https://huggingface.co/owao/Nanbeige4.2-3B-GGUF`
  (revision `a4008676ccf9f341f25ed8b50a7dda95eaf9977b`)
- Nanbeige llama.cpp support:
  `https://github.com/ggml-org/llama.cpp/pull/25994`
  (revision `d28da865bf284acaecc98ad18a3c1f607c0fd754`)
- Looped-transformer mechanism:
  Chen et al., *Bypassing the Exponential Dependency: Looped Transformers
  Efficiently Learn In-context by Multi-step Gradient Descent*,
  arXiv:2410.11268.
