# PLAN — native NL oracle on Android, end-to-end, with proxy fallback (a /goal ladder)

**Status: PLAN (not a result).** This is the closing ladder for one capability:

> NL in → `form-cli` routes → native request/response (build+test when needed) → local oracle = a
> fine-tuned llama (Android-max quant) on the **Adreno GPU** with **RAG** → if the local oracle is not
> confident, **proxy-fall-back** to a remote oracle on Mac/Windows → response out — **witnessed on the
> actual attached Galaxy S23 / Adreno 740**.

Each rung ends in its OWN witnessed receipt (the body's discipline: a step is real only when a receipt that
*refuses to fake* records it). The `/goal` stop condition is **Rung 9's capstone receipt exists and passes on
the device.** Pending is honest; no rung may be marked done on scaffold or toy inputs.

## The reference oracle (the gate, named once)

`llama.cpp` running the **same GGUF** on the Mac is the oracle — the role clang played for the byte-gate. The
GPU result is **bit-exact only where the hardware correctly-rounds** (mul+add, `NoContraction`); elsewhere it is
a **named tolerance** (the Adreno's non-correctly-rounded division, already measured ≤~20 ULP/block this
session). For generation the strongest witness is **token agreement**: greedy-decode the same prompt on device
and in llama.cpp; the token streams must match (or a named per-logit top-k agreement).

## What already exists (do not rebuild — wire)

- Routing + confidence + flywheel: `routers/form-cli-router.fk`, `form-cli/form-cli-{judge,sufficiency,predict,model}.fk`
- GGUF read + dequant: `form-cli/form-cli-gguf-cell.fk`, `form-stdlib/{gguf-read,q6k-dequant,f16-decode,weight-load}.fk`
- Inference recipes: `model/transformer-generate-cached.fk` (KV-cache), `cognition/nl-emitter.fk`, sampling
- GPU kernels + orchestration on Adreno (this session): `model/form-{glsl,matmul,transformer-block,transformer-depth4}.fk`, the FFI + f32 carriers in the `fkwu` seed
- RAG: `model/rag-{ask,embed,retrieve,index,heal}.fk`
- Proxy transport: `mesh_{announce,discover,register,detect,serve}` carriers; `receipts/2026-06-29-mesh-api-proxy.md`

## The ladder

| # | Rung | Witnessed receipt (on device unless noted) | Depends |
|---|------|---|---|
| 0 | transformer kernels + block + 4-deep stack on Adreno (random weights, named ULP) | ✅ `2026-06-29-android-gpu-transformer-end-to-end.md` | — |
| 1 | **Pick + load a real GGUF; one real layer on the Adreno.** Choose the Android-max model (default: Llama-3.2-1B or Qwen2.5-1.5B at q4/q6 — must fit S23 ~8–12 GB and beat a chosen benchmark). Dequant ONE real layer (`weight-load`+`q6k-dequant`) → `c_memcpy` → run that exact layer at real width on the Adreno; verify vs llama.cpp (named per-tensor rel-err) | `…-android-gguf-layer.md` | 0 |
| 2 | **Full forward → real logits.** All N layers + embed + final-norm + LM head for a real prompt, producing the logit vector on the Adreno; verify top-k logit agreement with llama.cpp | `…-android-gguf-forward.md` | 1 |
| 3 | **Tokenizer (BMF-cursor grammar) + KV-cache decode → real tokens.** Greedy-decode N tokens on device; the token stream MATCHES llama.cpp greedy on the same prompt. *(Closes the "voice" PENDING seam for the device.)* | ✅ `…-android-native-generate.md` | 2 |
| 4 | **Generation confidence.** A calibrated signal (logit entropy / margin / judge-score / RAG-grounding) on the device-generated answer, wired into `form-cli-router`'s confidence axis; show it is LOW on out-of-distribution prompts, HIGH on in-distribution | ✅ `…-android-gen-confidence.md` | 3 |
| 5 | **RAG-grounded native answer.** NL query → `rag-retrieve` context → local oracle generates a grounded answer on device; verify the answer cites retrieved cells (grounding check) | ✅ `…-android-rag-answer.md` | 3, (4) |
| 6 | **form-cli local loop closes.** NL in → `form-cli-router` → local oracle (Adreno) → response out, through `form-cli`, on device, in one invocation | ✅ `…-android-formcli-local.md` | 4, 5 |
| 7 | **Build-and-test sub-loop.** Code request detected → emit → `host-exec` (tag 136) build → run tests → result/repair fed back, on device | ✅ `…-android-build-test.md` | 6 |
| 8 | **Proxy fallback to remote oracle.** A LOW-confidence request is forwarded from the S23 via the mesh proxy to a Mac/Windows oracle; the remote response returns through the channel and `form-cli` answers with it; the route choice (local vs remote) is the router's confidence decision, witnessed both ways | ✅ `…-android-proxy-fallback.md` | 4, 6, (7) |
| 9 | **CAPSTONE — full loop, one witnessed run on the S23.** NL → router → (RAG + local oracle on Adreno, build/test if needed) → confidence check → fallback to remote via proxy iff low → response. Two prompts: one answered locally, one that triggers fallback. Both witnessed end-to-end on the device | ✅ `…-android-native-oracle-e2e.md` | all |

## Risks / honest floors (named up front, not discovered late)

- **Rung 1–2 scale.** D=2048+, 22+ layers, q4 dequant at real width is far past this session's D=8 randoms;
  memory budget on the S23 and dispatch count are the real unknowns. Floor: if a full real model won't fit/run,
  shrink to the largest that does and SAY so (don't claim a size that didn't run).
- **Rung 3 token match.** Greedy token agreement with llama.cpp is the hard gate. The Adreno's division drift
  (named ≤~20 ULP/block) can flip an argmax near ties → a divergent token. Floor: report the first divergence
  position and the logit margin there; a divergence is a finding, not a hidden failure. Bit-exactness is not
  promised — token agreement within a named logit tolerance is.
- **Rung 4 confidence is a fake-in-waiting** until the metric is **pre-registered** (per the PENDING-receipt
  discipline): define the confidence scalar and its threshold BEFORE measuring, or it is shaped to pass.
- **Rung 8 needs a live remote oracle** reachable from the phone; if the network/rendezvous is down the rung is
  blocked-not-shaped (name it).
- **No rung is "done" on the Mac** when the claim is "on Android." Each device rung runs on the attached S23.

## The /goal stop condition

> close the full NL→form-cli→native-oracle loop on the attached Android device with proxy fallback, witnessed
> by Rung 9's capstone receipt — each rung 1–9 landing its own on-device receipt first, llama.cpp as the named
> tolerance oracle, no rung marked done on toy inputs or on the Mac when the claim is on the phone.

Drive top-down: do not skip to Rung 9. The keystone is Rungs 1–3 (a real model actually generating on the
Adreno); 4–8 are wiring of cells that already exist; 9 is the assembled witness.
