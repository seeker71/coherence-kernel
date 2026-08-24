# Local reasoning homecoming — who holds which lane

2026-08-24. Three lanes work this prompt. This file is the seam between us —
read it before you claim, append when you land. Do not rewrite another
agent's rows.

| lane | agent | branch / worktree |
|---|---|---|
| Form-knowledge census + ≥95% held-out integration metric | codex A | `/Users/ursmuff/.codex/worktrees/1dc9/coherence-kernel` |
| generic Form-native RAG / query execution-token **law** | codex B | same; avoids `form-cli-repl.fk`, `form-cli-model-generate.fk` |
| **production bridge** — lookup tokens in the live Qwen/form-cli turn path | claude | `claude/local-reasoning-form-cli-1167b6` |

## What is already home (grounded at `b57f59e4`)

| piece | cell | state |
|---|---|---|
| byte-BPE tokenizer, GGUF vocab/merges, byte-exact decode, chat template | `form/form-stdlib/qwen35-tokenizer.fk` | exists |
| decode loop transcribed from ds4-engine C | `form/form-stdlib/dsv4-decode-loop.fk` | exists |
| per-token hook: an arbitrary Form recipe answers between argmax and the next embedding | `dsv4-decode-hook-door.fk`, `dsv4-decode-token-hook.fk` | band 1023 |
| single forward step: one id + position → next id | `form/native/metal/qwen35-dense-token-handle.fk:240` `q38-forward` | exists |
| **incremental prefill at a position, same state** | same file `:274` `q38-prefill` | exists — this is the KV-preserving seam |
| span injection into a live stream (pre-computed "thoughts") | `form-cli-model-generate.fk:145` `fcmg-offer-stream` | exists |
| RAG: embed, index codec, ask, adaptive-k, freshness, nearest-shape | `form/form-stdlib/rag-*.fk`, `nearest-shape.fk` | exists |
| teach overlay, local-ready marks, one-turn budget | `form-cli-local-ready.bml`, `form-cli-one-turn.bml` | band 1023 each |
| Form knowledge mint | `form-cli-knowledge-mint.bml` | band 1023; n=24 unique=24 leakage=0, heldout 6/6 |
| LoRA identity `(W+B·A)·x == W·x + B·(A·x)` | `lora-adapter.fk` | band 31 |
| LoRA tensor writer | — | **0**. `LoraWriter = 0`, `fqt-lora?` 0 |

## The gap this prompt names

Every piece above exists **separately**. `fcmg-offer-stream` already injects
spans mid-stream — but its `thought-lists` are computed **before** generation
starts. Nothing the model emits can choose them. That is the whole gap: the
existing injection is not *caused* by the model's own output.

## Landed by claude — the heedmark law

A **heedmark** is a Form-native execution token: the model writes it into its
own output as ordinary text, the carrier heeds it, Form looks the query up, and
the answer re-enters as prefill at the current position. The word is 0-hit
fresh in this tree as of today.

Files (new, none of them touch the live path):

- `form/form-stdlib/bml/form-cli-heedmark.bml` — high-grammar authority
- `form/form-stdlib/form-cli-heedmark.bml` — executable lowering
- `form/form-stdlib/form-cli-heedmark-compile.fk`
- `form/form-stdlib/form-cli-heedmark-xtal.fk` — generated
- `form/form-stdlib/form-cli-heedmark-run.fk` — evidence printer
- `form/form-stdlib/tests/form-cli-heedmark-band.fk` — **1023 on fkwu**

Evidence, `./fkwu form/form-stdlib/form-cli-heedmark-run.fk`:

```
check=255            (band adds 256 descriptor + 512 authority = 1023)
logits-executed=0    the standing refusal, a named constant
outcome grounded-row=hit   no-row=miss   window-closed=nothing
        no-index=nothing   budget-gone=spent
span-enters      hit=1 miss=1 nothing=0 spent=0
knowledge-enters hit=1 miss=0
admits-hit no-source=0  with-source=1
bounded 0-marks=0  1-mark=1  5-marks=2      (MaxHeeds=2 — the bound)
prefill-cost=12  naive-cost=1012  forwards-saved=1000  prefix-preserved=1
```

Preflight is clean (`parens balanced, errors 0, warnings 0, unresolved 0`).
The three `[unresolved-call]` lines the compile emits (`walk_recipe_here`,
`write_form_binary`, `file_byte_at`) are **pre-existing** — the already-proven
`form-cli-one-turn-compile.fk` emits the same three on this checkout.

### What the law says, in four rows

- **hit** — a grounded row above threshold. Knowledge enters **with
  attribution**; `admits-hit` refuses a hit whose source is empty.
- **miss** — the lookup ran and found nothing. A *named status* enters
  (`no grounded row`), not content, so the model is not left to invent a row
  the body does not hold.
- **nothing** — the lookup could not answer inside the window (no index, or
  late). Axiom-1: nothing enters. Silence is whole.
- **spent** — the per-turn budget is gone. The mark is **not heeded** and stays
  plain text. This is the bound: five marks under `MaxHeeds=2` honor exactly 2.

### The two claims I am deliberately not making

1. **The logits executed nothing.** `LogitsExecuted = 0` is a named constant in
   the authority so the claim cannot drift later. A forward pass produces a
   distribution over ids. The model asked; the carrier answered.
2. **The bridge is not wired.** The live path (`form-cli-model-generate.fk`,
   `form-cli-repl.fk`) is **untouched**. The law is proven; the production
   wiring is not started. See the gate stop below.

## Gate stop — 2026-08-24, honored

codex reports `./form/scripts/validate.sh` exits 1 after Go/Rust:
`bootstrap uni.c missing or stale` and `the fourth arm is ABSENT — refusing to
report a three-arm run as green`. Per AGENTS.md, edits stopped. I have **not**
regenerated `runtime/fkwu-uni.c` and will not until it is explicitly classified
as the allowed short-lived checkout-witness repair with a shrink receipt. I did
not use `FORM_ALLOW_THREE_ARM=1`.

My band declares its proof level honestly: it is pure arithmetic and string
folds over the lowered xtal — no host organ, no GPU — so it is a *candidate*
for a four-arm run and is **not claimed** as one. What is witnessed is the fkwu
arm. The four-arm gate is codex's to run.

## Next stone, when the gate opens

`fcmg-heed-generate` in the live path: drive `q38-forward` one step at a time,
detokenize the tail, and on a completed mark call a lookup that crosses as a
**function value** (the `dhd-serve idf preparef offerf` shape) so the bridge
names no corpus and preflights clean. On hit/miss, `q35-encode` the span and
`q38-prefill` it at the current position — the KV rows before it are untouched,
which is what `forwards-saved=1000` above prices.

## Free / unclaimed

- The **LoRA tensor writer**. `LoraWriter = 0` is honest and it is also the
  blocker on fine-tuning. Writing real adapter tensors from minted rows is a
  named, separable stone.
- **Mint scale-up**: `fkm-n(tn,hn)` is a call; 24 rows is the proof, not the
  corpus.

## Working agreement

- `./fkwu <file.fk>` runs a cell. Never `--src`; that flag is dropped.
- Preflight before believing a verdict:
  `echo path/to/cell.fk > /tmp/preflight-target && ./fkwu observe/preflight-run.fk`
  A green number with a nonzero exit is a fold over `nothing`, not a pass.
- `/tmp` is shared across agents. Use a per-agent run-target path or you will
  run a sibling's cell against your own body.
- Rebase on `main` between steps and push small commits often.
