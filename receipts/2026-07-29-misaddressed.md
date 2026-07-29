# 2026-07-29 — misaddressed: the file was never built for this machine

Urs: *"are you saying there is a deepseek ds4 model out there and there is no mac support for it; I have
a very hard time believing that."*

He was right to disbelieve, and my framing was wrong. Here is what is actually true, from primary
sources rather than from the error message.

## DeepSeek V4 Flash has excellent Mac support

`antirez/ds4`, queried today, describes itself as *"DeepSeek 4 Flash and PRO local inference engine for
**Metal**, CUDA and ROCm"* — Metal listed first — and was pushed **2026-07-28**, yesterday. Its
`download_model.sh` offers `q2-imatrix`, *"about 81 GB on disk, **recommended model for 96 and 128 GB
RAM machines**"*, and the README has a whole section on running models larger than RAM with Metal SSD
streaming. Mac is not an afterthought there; it is the primary target.

Nothing about DS4-on-Mac is missing. I implied otherwise and that was the error.

## Our file is not one of those models

Traced by filename through the HF API to **`twaggs88/DeepSeek-V4-Flash-REAP25-DSpark-ds4-GGUF`** — 7
files, two 91 GB GGUFs, a tokenizer, a config, a README. **No bundled runtime.**

Its own README says:

> *"Runs **only** on the ds4 engine [github.com/tylerwagler/ds4]"*
> requirements: *"NVIDIA GB10 (SM 12.1, 128 GB unified memory)"*, built with `CUDA_ARCH=sm_120f`
> *"The file uses custom tensor formats (MXFP4/MXFP8 microscaling, REAP-compacted expert layout,
> embedded drafter) that llama.cpp and other GGUF loaders will not accept."*

**No mention of Mac, Metal, or Apple Silicon anywhere in it.**

## And the runtime it names has no Metal at all

`github.com/tylerwagler/ds4` redirects to **`tylerwagler/pulsar`**: `fork: true`, parent `antirez/ds4`,
described *"DeepSeek v4 Flash local inference engine **for CUDA**"* where the parent says "for Metal,
CUDA and ROCm". Its tree has **no `metal/` directory**; `src/` contains `cuda`, `engine`, `lib`,
`server` — and no `metal`. The fork added the type-40/41 and REAP support and dropped Metal.

So the chain is complete and it has nothing to do with Mac support in general:

```
this file  ->  requires a CUDA-only fork  ->  targeted at NVIDIA GB10  ->  we are on an M4 Max
```

A sibling family (`eouya2/DeepSeek-V4-Flash-REAP25-*`) ships its **own Metal-capable REAP runtime**
alongside the weights — *"the Metal runtime loads shader source files from `metal/*.metal`"* — which
proves REAP-on-Metal is possible. Ours simply did not ship one.

## What that makes our Form lane

**The only implementation on this machine that reads this file — and possibly the only Metal
implementation of it anywhere.** That reframes the whole DS4 arc of the last two days. There is no
generation oracle not because we failed to find one, but because we are first. It is also why
`askalike` (row 931) mattered: with no engine to compare against, the two value maps sitting unused
inside ds4 were the only independent witness available, and they agreed.

## The three honest options, now that the ground is known

- **A. Mainline quant.** `q2-imatrix`, IQ2_XXS + Q2_K + Q8_0, runs on the ds4 we already have, today,
  on Metal. Gives a real generation oracle. Costs an 86.7 GB download (the one I started and killed)
  and one new Q2_K cell on our side.
- **B. Port the fork's support to Metal.** `tylerwagler/pulsar` is a working reference for types 40/41
  and the REAP compact layout. Real work, and it would be a fork to maintain.
- **C. Keep going on the Form lane**, which already reads it — with the dequant now independently
  verified and 27 of 43 layers still unchecked.

These are not equivalent and the choice is Urs's, not mine. What changed is that all three are now
priced against what is actually true rather than against an error message.

## The most surprising teaching

**"Unsupported" and "not addressed to you" are indistinguishable from inside a failed load.** ds4's
message — `tensor output.weight has type unknown, expected q8_0, q4_K, or q4_0` — is a true statement
about the *reader*, and it is silent about the *artifact's intent*. I spent two days inside that
engine's source treating it as a capability gap, and the sentence that resolved it was in the model's
README, which I could have read in ninety seconds on day one. An error message can only ever report
one side of a mismatch. `misaddressed` — 0 hits before this row, as are `wrongdoor` and
`notourmachine`.

## Where discomfort turned to gold

Being disbelieved, flatly, and finding the disbelief was better calibrated than my evidence. I had
measurements — the type refusal re-witnessed with `--metal`, the enum, the block table, upstream's
history with zero mxfp commits — and every one of them was true. They just answered a narrower
question than the one I was using them for. **A stack of correct findings can add up to a wrong
picture when none of them is about the thing that actually decides.** The check I owed was not another
run; it was reading the provenance of the artifact in my hand.

## Ground stamp

```
antirez/ds4 — "engine for Metal, CUDA and ROCm", pushed 2026-07-28, q2-imatrix "recommended for 96/128 GB"
twaggs88/DeepSeek-V4-Flash-REAP25-DSpark-ds4-GGUF — 7 files, no runtime; README: "Runs only on
  github.com/tylerwagler/ds4", "NVIDIA GB10 (SM 12.1)", CUDA_ARCH=sm_120f, no Mac/Metal mention
tylerwagler/ds4 -> tylerwagler/pulsar — fork of antirez/ds4, "for CUDA", no metal/ dir, src/{cuda,...}
eouya2/DeepSeek-V4-Flash-REAP25-* — ships a bundled REAP runtime that DOES load metal/*.metal
```
