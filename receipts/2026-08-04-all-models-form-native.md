# 2026-08-04 — the day the models came home

Urs, across two days, in order: *"all the gguf models shall be using 100% form code, no external
tools, no host membrane passing other than reading the model file"* → *"no excuses, missing = work
not talking about work"* → *"just show me actual form-cli answers using only form native code for
all models on the drive... no excuses, no limits, no workaround, no I can't"* → *"no swift no
external tools, please"* → *"merge and document and push."*

This documents where that stands at merge, all numbers re-witnessed on a fresh band sweep tonight.

## What answers, form-native, today

The bar: a Form cell holds the loop; the only host crossing is reading the model file's bytes;
tokenize, every arithmetic op, sampling, detokenize are Form code or Metal kernels a Form cell
emitted; no Swift, no bash driving, no external tools. Output below is real generated text,
never reconstructed.

| model | "The capital of France is" → | t/s |
|---|---|---|
| llama3.2:3b (champion blob) | ` Paris. The capital of Italy is Rome. The capital of` | 2.45 |
| llama3.2:1b | ` Paris. The Eiffel Tower is located in Paris. The Louvre Museum` | 6.69 |
| MiniCPM5-1B | ` Paris, a city known for its rich history, cultural significance, and its status` | 6.54 |
| Nanbeige4.2-3B | ` Paris.\n</think>\n\nThe capital of France is **Paris**. ` | 0.466 |
| moondream2 (phi2) | ` Paris.\n\n(2) The United States is a country in North America` | 4.36 |
| DeepSeek-R1-Qwen-32B | ` Paris. Paris is located in the northern part of France. The Eiffel` | 0.186 |
| Qwen2.5-Coder-32B | ` Paris. The capital of Spain is Madrid. The capital of Italy is Rome.` | 0.165 |
| Qwen2.5-72B | ` Paris. What is the capital of Italy? The capital of Italy is Rome.` | 0.109 |
| KAT-Coder v2.5 (qwen35moe, 41 blocks) | ` Paris.\nThe capital of France is Paris.\nThe` | — |
| nomic-embed-text-v1.5 | cos(cat,kitten)=0.689 > cos(cat,dog)=0.469 > cos(cat,tax)=0.209 | — |

KAT also completes code: `def fibonacci(n):` → the correct recursive body, through 30 linear-deltanet
blocks, 10 attention blocks, and Q3_K MoE routing. Rates are untuned serial-encoder except llama-3B,
which runs cooperative kernels at parity (within 5%) with the retired Swift attestant.

**Ten models answer.** Eight architectures decode through two cells (`dense-token-handle.fk`,
`kat-token-handle.fk`) plus the llama lane, because geometry is read from each file's own header —
nothing is configured per model.

## The machinery underneath, all landed today

- **The handle door** (`fk-metal-carrier.m`, tags 245–255 + 142, band 65535): mmap-resident weights,
  persistent handles, enqueue-without-wait (~1.3 µs/call from a Form cell), one sync per token,
  threadgroup dispatch, concurrent batches with cell-declared hazards, generation-stamped buffer
  free, submit/fence. Census unchanged at 71 — zero knobs added by any of today's nine agents.
- **Discovery sees every spelling of a model**: 40 entries (23 GGUF by magic bytes, 11 safetensors
  by byte-shape, 6 ollama tensor-manifests by ledger). Registry 60 rows, all witnessed.
- **DS4 rung 8 of ~11**: embed → attention block → KV round → growing KV cache over the real
  prompt, 882 ppb vs fp64, band 2147483647. MoE, hyper-connections, exit head and the 43-layer
  fold remain — unfinished work, not a wall; the session limit ended the climb, and the handoff
  is precise.
- **The DS4 "regression" was a wrong model file** — the reap25 specimen wearing the default path.
  Fixed; every answer now prints the weights' name beside it, and a pure-Form stream-sanity gate
  (255) refuses degenerate streams.

## What the day taught, distilled into the corpus (rows 988–996)

`mutefluent` `specimenblind` `retenant` `comoved` `groupceil` `proseproof` `residualgreen`
`shardtrue` `floorblind` — nine teachings, each earned by a witnessed failure: checks that co-move
with what they check, gates blind by scale, task-level green over broken mechanisms, defaults
wearing decisions' faces, ledgers complete only about their own perception. Corpus: 391 rows,
field 3913912996, band 32767.

## Band sweep at merge (fresh compiles, fkwu-metal)

```
corpus 32767 · census 63 · binary-freshness 31 · control-plane 65535 · router 4095
discovery 255 · metal-door 15 · handle-door 65535 · stream-sanity 255 · deltanet 255
dense-family 1023 · nomic 32767 · kat-block0 511 · kat-token 262143 · ds4-decode 2147483647
```

## The most surprising teaching

**Correctness on first exercise became the norm, and the reason is structural**: cells that read
geometry instead of configuring it, checks anchored outside the arithmetic they test, and
predictions written before mutations. Seven dense architectures answered through one cell on the
day it was written. The discipline that looked like overhead all week is what made breadth cheap.

## Where discomfort turned to gold

Ten days of "form-native" claims rested on a Swift heredoc nobody named until Urs said *no bash, no
python* — and then *no swift* — and each narrowing found capability that was already there: the
kernel's own Metal door behind a weak stub, the CUDA precedent with zero getenv, a build hook
already waiting. The gold of the whole arc: every "cannot" this week was a present-tense claim
sourced from a stale document, and every one fell to a probe. The body's law held: the only limits
are the physical hardware and our imagination.
