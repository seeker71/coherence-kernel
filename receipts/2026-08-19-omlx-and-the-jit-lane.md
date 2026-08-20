# oMLX read against our own Metal numbers — what transfers, what was refuted in ten minutes

Date: 2026-08-19, Hati Suci. Apple M4 Max, 128 GiB unified memory, 40 GPU cores.
Worktree `.claude/worktrees/jit-lane-performance-d77568`, branch `claude/jit-lane-performance-d77568`.
Source read: <https://github.com/jundot/omlx> — README, `docs/experimental/dflash_mlx_integration.md`,
`docs/experimental/qwen35_ane_prefill.md`, and the `custom_kernels/` and `cache/` trees.

Urs asked how that work made a model faster on a local Mac, and how the Form-native JIT lane reaches
the same enhancements. Read first through [PR #461](https://github.com/seeker71/coherence-kernel/pull/461),
which cites no outside article — the link arrived afterward and is the real source.

## What oMLX actually does for speed, in five places

1. **Fused native Metal kernels, per model family.** The README's largest number: the GLM-5.2 fused DSA
   prefill measured **845 tok/s against ~29 tok/s** on the generic path, one M3 Ultra — roughly 30×.
   `fused_moe`, `sparse_mla`, `dsa_indexer`, `dspark_gemm/qmv`, steel-style tiled attention blocks.
2. **Speculative decoding** (dflash, arXiv:2602.06036): a draft model proposes a **block of 16** tokens
   by parallel denoising; the target **verifies all 16 in one forward**; the longest greedily matching
   prefix commits; the cache rolls back by tape replay.
3. **A tiered block KV cache** — vLLM-shaped, prefix sharing plus copy-on-write, **hot in RAM, cold on
   SSD as safetensors**, restored on a matching prefix even after a server restart.
4. **Continuous batching in one resident server**, with LRU model eviction, pinning, per-model idle TTL,
   and a process memory ceiling.
5. **An experimental ANE+GPU prefill split** — two ANEs take ~53% of the MLP channels through private
   Apple APIs. Its own doc calls it "an approximate acceleration path rather than bit-exact inference",
   because it requantizes to per-output-channel INT8.

## The measurement that killed my first idea

`fk-metal-carrier.m:503` keys the pipeline cache on `fn + "\n" + src`. A decode lane asking one emitted
source for fifteen kernel names therefore *looks like* fifteen `newLibraryWithSource` calls over the same
bytes, paid on every process start — and oMLX ships precompiled metallibs, which made the transfer look
obvious. Written as a probe rather than a patch:
`form/form-stdlib/tests/metal-pipeline-compile-cost-probe.fk`, over the real 12,518-byte llama decode
source, 3 runs on this machine:

```
msl-bytes 12518
1  q6k_dequant   FIRST   ms 75 (cold) / 32 / 29
2..14 the other names    ms 1-2 TOTAL
15 q6k_dequant AGAIN     ms 0
```

Metal's front end already shares its compilation work across identical text. There is no per-process
compile tax and no `MTLLibrary` cache worth adding: ~30 ms once, against a decode measured in seconds.
The probe is kept, run, and headed with its own refutation so the next reader does not re-derive it.

## What transfers, ranked by the body's own measured numbers

**1. Verified speculation — and the drafter is already in our weights.**
Decode here is boundary-bound, not FLOP-bound: `receipts/2026-07-21-ollama-denominator-and-the-one-remaining-gap.md`
measures the matvec at **0.7% of f32 peak**, 25 GB/s of a ~400 GB/s machine, and shows the kernel's
throughput predicting the token rate with no second mystery. `form-stdlib/qk-matmul-batch.fk` measured
what batching the same kernel across columns buys — **4–5×, with a derived ceiling of 7× (D = 6L)**. A
verify pass over a drafted block *is* that batch. Greedy longest-prefix acceptance emits exactly the
tokens plain greedy decode would emit, so `tensor-ir.fk`'s discipline is untouched: speculation changes
*when* a forward runs, never *what* it computes. And where oMLX must pair every target with a separately
trained draft checkpoint, `form-stdlib/kat-coder-pipeline-map.fk:60-65` already read one out of a file
we hold — KAT-Coder's block 40, `nextn.enorm / nextn.hnorm / nextn.eh_proj / nextn.shared_head_norm`,
correctly taken off the plain-forward path and never run since. `form-stdlib/dsv4-decode-loop.fk:75`
names the other half plainly: "speculative drafting: unwritten".

**2. Fusion — fewer, fatter kernels.** This body measured the same law from the other side before
reading that repo. `form-stdlib/form-native-decode-chain-fullrow.fk`'s header records that widening one
stage from 8 to 3072 real rows — **384× more arithmetic per barrier** — moved GPU-device time only
**2.452×**, so cost per FLOP improved **~157×** while absolute per-dispatch cost went 1050.7 µs →
2576.8 µs. Boundaries are the tax; work per boundary is free. Fusion is also bit-exact-compatible in a
way the ANE split is not: folding rmsnorm → matvec → rope → attend → swiglu → add into one kernel moves
where values live (registers, threadgroup) without changing accumulation order or rounding count. The
thing fusion must never quietly do is contract a `mul`+`add` into an fma — which is exactly the rule
`tensor-ir.fk` already writes down and already splits through a named temporary to prevent.

**3. A cross-run prefix KV cache.** `form-stdlib/dsv4-kv-cache.fk` appends one 512-float latent row per
token per layer, correct and proven, and it dies with the process. No prefix match, no spill, no restore.
oMLX's tier is nothing but memoization keyed by content — which is verbatim the crystallization law
`host-kernel.form` already states for code (cache by NodeID, melt when cold), applied to activations.
It costs no numeric ground at all.

**4. Residence for compute, not only for decisions.** PR #461 landed the resident-door shape for a
*decision* — one process spawned once, asked per step. oMLX is that shape for *compute*: models,
pipelines, buffers and caches hot across requests, the GPU rarely idle. This also touches the one open
thread: `form-native-decode-chain-fullrow.fk` eliminated four hypotheses for the ~1050–2576 µs/dispatch
floor and left "GPU clock/power-state differences between a short isolated probe and a long sustained
real decode" explicitly unchased — which is precisely what a never-idle server changes.

## What does not transfer

- **Precompiling kernels** — refuted above, by measurement, the same hour.
- **The ANE/GPU split as law** — approximate by construction, through undocumented APIs its own doc says
  "can stop working after a macOS update". It could only ever stand *beside* the exact lane as a declared
  approximate one, never under it.

One thing transfers straight into a law this body already holds: oMLX spends its install section warning
that a plain build produces no custom kernels and the affected families **"silently fall back to much
slower generic paths"** — 30× slower, with no error — which is why it ships `native_kernel_status()`.
That is [`green instruments cannot say no`](2026-08-19-green-instruments-cannot-say-no.md) arriving from
outside, and the same reflex as `metal-door.fk`'s `md-door-live?`, which refuses to believe a handle on
`metal_linked=true` alone.

## What ran here

```
form/form-stdlib/tests/metal-pipeline-compile-cost-probe.fk   3 runs (a duration, not a verdict)
ingest/tests/frontier-ingest-omlx-band.fk                     -> 127   (fkwu-metal, this checkout)
  same band, go + rust + ts                                   -> 127   1 ok, 0 divergent
learn/tests/homecoming-distillation-corpus-band.fk            -> 32767 (row 1027 added)
form/form-stdlib/tests/metal-door-band.fk                     -> 15    (the Metal lane is live here)
```

The four-way run is **three-arm plus the source-runner, said out loud**: this checkout's bootstrap
`uni.c` is missing or stale, so `validate.sh` refused a four-arm claim and ran under an explicit
`FORM_ALLOW_THREE_ARM=1`; `fkwu-metal` answered the same 127 on its own.

**Honest scope.** Only the compile-cost probe and the bands were measured today. Every number about this
body's decode, prefill and per-dispatch floor is read from its own dated receipts and cell headers — no
DS4 or llama generation was run in this session, and none of those numbers were re-derived.

## Most surprising teaching

The drafter was already in the building. oMLX's speculative-decoding table is a list of ~18 target/draft
pairs, each draft a separately trained checkpoint someone had to publish — and families without one
simply cannot speculate. This body read a trained multi-token-prediction head out of KAT-Coder's block 40
months ago, correctly noted it was off the plain-forward path, and moved on. The most expensive missing
capability in the lane came bundled with weights already on this disk. Corpus row 1027 carries the word
for that: **appurtenance** — what transfers with a thing you already hold, whether or not you ever use it.

## Where discomfort turned to gold

The compile-cache idea was mine, it was plausible, it mirrored the source's own packaging note, and I
wanted it to be true because it was small and shippable. Writing it as a probe instead of a patch felt
like a detour — thirty lines and two runs to earn the right to a one-line change. It came back 75 ms /
1 ms and killed the change outright. The discomfort was watching a good-looking hour of work evaporate
before it started; the gold is that the refutation is now a cell in the tree with its own header, and the
same thirty minutes bought the ranked list above, where the real levers are the ones this body's own
receipts had already measured and not yet joined.
