# 2026-07-29 — what actually fits on this machine, measured

Urs: *"so there is a sibling deepseek model that does work for mac? is there another current model that
fits on a 128GB Mac M4? what about GLM or Kimi K3 or any other high quality model?"*

Sizes below are from the HuggingFace API today, summed across shards — not from blog summaries and not
from memory.

## The machine, grounded

```
Apple M4 Max · 128 GiB RAM (137 GB decimal) · 16 cores (12P/4E)
Metal maxBufferLength: 80.6 GiB     ← the hard per-buffer cap
disk free: 469 GiB
```

That buffer cap matters: our 91 GB DS4 file already exceeds it, which is why the Form lane mmaps and
wraps in views rather than asking for one buffer.

## Yes — the sibling DeepSeek runs on Mac

`antirez/deepseek-v4-gguf`, all Metal-supported by the ds4 checkout already on disk:

| GB | file | ds4's own guidance |
|---:|---|---|
| **86.7** | `...IQ2XXS-w2Q2K-AProjQ8-SExpQ8-OutQ8-chat-v2-imatrix` | *"recommended model for 96 and 128 GB RAM machines"* |
| **97.6** | `...Layers37-42Q4KExperts-...-imatrix` | *"good for higher quality inference for 128 GB MacBooks"* |
| 164.6 | `...Q4KExperts-F16HC-...` | 256 GB+ |
| 442–465 | V4-Pro variants | 512 GB |

Same model family as the one our Form lane implements. Same tokenizer, same 43-layer MLA + hyper-
connection + MoE architecture our cells were written against.

## The two you asked about are both out of range

| model | smallest quant found | verdict |
|---|---:|---|
| **GLM 5.2** | 216.7 GB (`UD-IQ1_S`) | out — and ds4's own GLM targets are 262 GB and 434 GB |
| **Kimi K3** | **594.0 GB** (`UD-IQ1_S`) | far out — 2.8T params |
| Qwen3-Coder-480B | 149.7 GB (`UD-IQ1_M`) | out |

## What does fit

| GB | model | note |
|---:|---|---|
| 2.0 | llama3.2:3b | **here** — our Form lane, 31.7 tok/s measured today |
| **17.4** | **KAT-Coder V2.5** | **here** — llama.cpp 81.06 tok/s measured 2026-07-28 |
| 26–36 | Qwen3-Coder-30B-A3B | |
| 62.6 | gpt-oss-120b | see below |
| 86.7 | DeepSeek-V4-Flash q2-imatrix | architecture-matched |

## The recommendation, and why it is not the biggest one

For a *coding* model that works today, nothing needs downloading: **KAT-Coder V2.5 is already here at
17.4 GB and 81 tok/s.**

For the thing this project actually lacks — **an oracle** — the answer is the 86.7 GB DeepSeek, and its
value is not its size. It is the only file on the list whose architecture our Form lane already
implements. Every DS4 cell in this body targets it: `dsv4-mla-core`, the hyper-connection split, the
router, the 43-layer stack. gpt-oss-120b would be a fine model and would validate nothing we have
built. The oracle has to speak the same architecture or it is just another program.

## The most surprising teaching

gpt-oss-120b's entire quantization ladder:

```
62.6 GB : Q2_K, Q3_K_M, Q3_K_S, Q4_0
62.8 GB : Q4_K_M, Q4_K_S
63.3 GB : Q6_K
63.4 GB : Q8_0
65.4 GB : F16
```

**Q2_K and F16 differ by 4.5%.** For any ordinary model that span is 6–8×. The model ships MXFP4-native,
so the GGUF "quantization" only reaches the small non-expert tensors and the ladder is almost flat:
choosing a quant there is choosing almost nothing, while the label promises a memory/quality tradeoff
that the artifact cannot make. `flatladder` — 0 hits before this row, as are `mootquant` and
`sameshelf`.

It is the same shape as yesterday's `misaddressed`: a name that describes the *reader's* categories
rather than the artifact's. A quant label answers "what did the packager ask for", never "what is in
the file" — and the only way to tell is to measure, which is one API call.

## Where discomfort turned to gold

Nearly answering this from the search results. The web summaries named half a dozen models with
confident tok/s figures, and repeating them would have read exactly like an answer. Every number above
that matters is instead a summed file listing or a `sysctl`, and two of the search's suggestions —
Kimi K3 and GLM — turned out to be 594 GB and 217 GB, which no amount of confident prose would have
revealed. The discomfort worth keeping is how *close* the plausible answer was to hand, and that the
grounding cost about four minutes.

## Ground stamp

```
sysctl hw.memsize -> 137438953472 (128.0 GiB); maxBufferLength 80.6 GiB; disk free 469 GiB
HF API, shard-summed, 2026-07-29:
  antirez/deepseek-v4-gguf   86.7 / 97.6 / 164.6 / 442-465 GB
  unsloth/GLM-5.2-GGUF       216.7 GB smallest (UD-IQ1_S)
  unsloth/Kimi-K3-GGUF       594.0 GB smallest (UD-IQ1_S)
  unsloth/Qwen3-Coder-480B   149.7 GB smallest (UD-IQ1_M)
  unsloth/gpt-oss-120b       62.6 GB (Q2_K) .. 65.4 GB (F16) — a 4.5% ladder
on disk here: KAT-Coder V2.5 17.39 GB (81.06 tok/s), llama3.2:3b 2.02 GB (31.7 tok/s Form-native)
```
