# Local reasoning homecoming — what is home, and the seams that remain

The local reasoning lane: a real open model (Qwen3.8-27B) runs Form-native on this
Mac's metal, the body feeds it typed observations from its own source, and every
crossing is measured at route level. This page names the pieces that stand, the
mechanism they compose, and what is still owed. Bands cited with a number were
re-run 2026-09-04; the others declare their own.

## What is home

| piece | cell | witness |
|---|---|---|
| byte-BPE tokenizer, GGUF vocab/merges, byte-exact decode, chat template | `form/form-stdlib/qwen35-tokenizer.fk` | the sorted fixed-row index (`qwen35-tokfast-v2`) carries encode |
| decode loop transcribed from ds4-engine C | `form/form-stdlib/dsv4-decode-loop.fk` | exists |
| per-token hook: an arbitrary Form recipe answers between argmax and the next embedding | `dsv4-decode-hook-door.fk`, `dsv4-decode-token-hook.fk` | 1023 |
| single forward step: one id + position → next id; incremental prefill at a position, same state (the KV-preserving seam) | `form/native/metal/qwen35-dense-token-handle.fk` `q38-forward`, `q38-prefill` | `qwen35-dense-token-handle-band` 2147483647 |
| span injection into a live stream | `form-cli-model-generate.fk` `fcmg-offer-stream` | exists |
| RAG: embed, index codec, ask, adaptive-k, freshness, nearest-shape | `form/form-stdlib/rag-*.fk`, `nearest-shape.fk` | exists |
| bounded raw-byte heed cursor: decoded output → query → typed prefill | `form-cli-heed-cursor.fk` | 524287 |
| attributed lookup against the current Form source body | `form-cli-heed-current-source.fk` | 16777215 |
| generation-path wiring, two ledgers, sealed-path context hint, bounded counters | `form-cli-model-generate.fk` | `form-cli-model-generate-heed-report-band` 8388607 |
| teach overlay, local-ready marks, one-turn budget | `form-cli-local-ready.bml`, `form-cli-one-turn.bml` | 1023 and 2047 |
| the mints — knowledge, universe, unique, domain, organ, lane, lens, planner, LoRA corpus, embodiment census, speaker home | `form-cli-*-mint.bml`, `form-cli-lora-corpus.bml`, `form-cli-embody-census.bml`, `form-cli-speaker-home.bml` | each band declares its own; row counts are census evidence, never a target |
| BML/BMF live-byte curriculum and control curriculum | `bml-bmf-stream-curriculum.bml`, `bml-bmf-control-curriculum.bml` | 16777215 and 1048575 (each band preludes its `.bml` authority) |
| Qwen teach overlay | `form-cli-qwen-teach-layer.fk` | 33554431 |
| LoRA identity `(W+B·A)·x == W·x + B·(A·x)` | `lora-adapter.fk` | 31 |
| error absorption: rank-1 `B·(A·x)` from one withheld error, sealed-surface transfer in the local Form classifier | `cognition/error-absorption-kernel.fk` | 4095 |
| LoRA tensor writer | — | **0**. `LoraWriter = 0`, `fqt-lora?` 0 |

Mint row counts and held-out scores are corpus/overlay observations, not claims
that Qwen learned those lessons; split-lineage adoption keeps exact leakage at zero
and exposes lesson-disjoint transfer as the current learning signal, not retuned away.

## The heedmark — the observed mechanism

A **heedmark** is a Form-native execution token: the model writes it into its own
output as ordinary text, the carrier heeds it, Form looks the query up, and the
answer re-enters as prefill at the current position.

- `form/form-stdlib/bml/form-cli-heedmark.bml` — executable grammar source
- `form/form-stdlib/form-cli-heedmark.bml` — executable lowering
- `form/form-stdlib/form-cli-heedmark-run.fk` — evidence printer
- `form/form-stdlib/tests/form-cli-heedmark-band.fk` — 1023

Evidence, `./fkwu form/form-stdlib/form-cli-heedmark-run.fk`:

```
logits-executed=0    the standing refusal, a named constant
outcome grounded-row=hit   no-row=miss   window-closed=nothing
        no-index=nothing   budget-gone=spent
span-enters      hit=1 miss=1 nothing=0 spent=0
knowledge-enters hit=1 miss=0
admits-hit no-source=0  with-source=1
bounded 0-marks=0  1-mark=1  5-marks=2      (MaxHeeds=2 — the bound)
prefix-preserved=1
```

The four rows the observation distinguishes:

- **hit** — a grounded row above threshold. Knowledge enters **with attribution**;
  `admits-hit` refuses a hit whose source is empty.
- **miss** — the lookup ran and found nothing. A *named status* enters, not
  content, so the model is not left to invent a row the body does not hold.
- **nothing** — the lookup could not answer inside the window. Axiom-1: nothing
  enters. Silence is whole.
- **spent** — the per-turn budget is gone. The mark is **not heeded** and stays
  plain text; five marks under `MaxHeeds=2` honor exactly 2.

Two distinctions the observation retains: `heed_model_executed=0` belongs to the
lookup — the local model did not execute repository IO; the surrounding forward
passes did. And a direct band witnesses mechanism shape; a scored live row
witnesses real weights. Neither one row nor one band stands in for the
fifteen-family denominator.

The NodeID loop crosses live on the same lane: the model emits a strict
`form:recipe-exec` request for a recipe by NodeID, the raw-byte cursor calls its
carrier once, Form generates the Metal kernel from the recipe children, the value
returns as a typed observation into the same original-ID/KV session, and the
session continues (`form-recipe-exec-token-band` 1048575 and
`form-cli-recipe-exec-cursor-band` 33554431; `form-recipe-exec-token-live-band`
needs the resident model and declares its own). That is one live
affine Metal thought, not yet CPU/MLX parity or a recursive model-authored
recipe-birth run.

## Effective Form reasoning in practice

Write the teaching as executable BML, carry the custom grammar as BMF data, and
feed raw decoded bytes into the live cursor. Do not require a tokenizer pre-step:
an incomplete frame is retained within the bounded cursor, a complete frame lowers
and runs, and malformed or over-budget input becomes `nothing`.

Use the reasoning controls by their observed behavior:

| need | Form move | practical contract |
|---|---|---|
| first usable answer | `oac-choice` | Walk in order, skip `nothing`, return the first non-`nothing` acknowledgement. If evidence is tied before ordering, abstain as `nothing`; do not invent a winner. |
| inspect alternatives | `oac-lanes` | Execute every lane and preserve every acknowledgement in order. Use `oac-lanes-winners` only to project non-`nothing` results; choosing is a later act. |
| commit now | `oac-cut-with-receipt` | Take the first acknowledgement even when it is `nothing`, stop, and retain the count of pruned, untried alternatives. Cut is not choice. |
| speculate safely | `oac-store`, `oac-undo` | Store the immutable memory value before the attempt. On a `nothing` acknowledgement, undo returns that exact checkpoint; a successful acknowledgement keeps the new memory. |
| bound work | `oac-timeout-walk`, `oac-timed-out?` | `nothing` with alternatives left is timeout; `nothing` after all alternatives were tried is honest exhaustion. Preserve `alts-left` so these cannot collapse into one status. |
| abstain exactly | `nothing`, `oac-nothing?` | `nothing` is neither `0` nor `1`. Test it only through the nothing/equality surface; never use it as arithmetic, ordering, or a branch condition. |
| select cognition | `find-plane`, `bbcc-thought-route` | Route `when`/`where`/`which` to computable kernels and learned planes such as `how`/`why` to learned kernels. A missing plane or missing evidence remains `nothing`. |
| birth and run a physical micro-thought | `frbt-parse-stream`, `frex-parse`, `frexl-execute-request`, `frxs-run` | A model can invent an affine recipe as scannerless raw bytes, receive its content-addressed NodeID, request that NodeID with an input and carrier, and continue from the typed observation. The native path is generated from the recipe itself on demand; there is no flatten prerequisite or operations table. Request, generated artifact, execution, observation, refinement, crystallization, dissolution, and release stay separately visible. |

The control curriculum invokes the repository's actual offer/ack, choice-lane,
inquiry-plane, and native-generation cells rather than matching their names
(`control/tests/offer-ack-core-band.fk` 2097151, `choice-lane-core-band` 1023).
Prompt/curriculum evidence is evidence for the teaching layer, not evidence that
model weights were trained or that the resident executed the controls correctly.

## Owed

- A ≥95% multi-token resident answer and the complete fifteen-family resident
  pass. The sealed denominator holds 30 unseen rows, exactly two in each of 15
  families, with zero recorded leakage; the claim waits until every family's live
  observations have run and reached the threshold
  (`form-knowledge-qwen-heldout-v3-eval-band` answers 65511 of 65535 today).
  One exact source-hit answer does not imply the other families; a multi-token
  crossing at a low ppm score is a failure signal to refine, not family credit.
- The **LoRA tensor writer**. `LoraWriter = 0` is honest and it is the blocker on
  fine-tuning; writing real adapter tensors from minted rows is a named, separable stone.
- **Typed move → addressed execution**: local output is a validated Form move
  symbol; binding general moves such as preflight/land to registered recipe
  NodeIDs and executing them inside the same loop is the next seam.
- **Mint scale-up**: `fkm-n(tn,hn)` is a call; the proof rows are the proof, not
  the corpus.

## Working agreement

- `./fkwu <file.fk>` runs a cell. Never `--src`; that flag is dropped.
- Preflight before believing a verdict:
  `echo path/to/cell.fk > /tmp/preflight-target && ./fkwu observe/preflight-run.fk`
  A green number with a nonzero exit is a fold over `nothing`, not a pass.
  Do not aim this door at an effectful top-level live driver: its fresh fkwu
  probe can reach those effects. Preflight the mechanism and pure band, then run
  the effectful driver once intentionally when its carrier is free.
- `/tmp` is shared across agents. Use a per-agent run-target path or you will
  run a sibling's cell against your own body.
- Land on `main` between steps in small commits.
