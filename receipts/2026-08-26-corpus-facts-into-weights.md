# 2026-08-26 — the homecoming corpus enters the weights, measured

Local loop, closed on this machine, zero rented tokens for every run:
train -> generate -> score -> retrain -> rescore. Base: Llama-3.2-3B-Instruct-4bit
(mlx), adapters rank 8, layers 12-27, resumed from Codex's bml-exec adapter
(11:49 this morning). Stack: `uv run --with mlx-lm` — no install, ephemeral env.

## The three measurements

**Heldout v3 (sealed, never trainable).** Base fabricates (`NR_replace`) or
refuses; the bml-exec adapter answers in-domain but wrong — v301 gave the
MEANING of CAN-not-MUST without the token. Facts were never in its data.

**0.55 epochs on the corpus (120 iters).** Format acquired — verbose
paragraphs became one-word answers in the right language — facts 0/12.
120 iters x batch 2 = 240 visits over 435 rows: the model met half the
corpus once. Format saturates in half an epoch; vocabulary does not.

**9 epochs, prompt-masked, lr 2e-5 (2000 iters, ~5 min, 3 GB).**
Seen facts under UNSEEN phrasing (valid2.jsonl, deterministic Form
rewording): **9/12** — spurious, changeling, conatus, candor, isometry,
equireach, sumkeel, shardtrue, carrygap. Misses near (basis/basis-span).
Coined-word unseen set: 0/6, exactly as the leakage design demands.
Train loss 0.001, val loss 3.956 — the coined-word val is unlearnable by
construction, so its rise measures the design, not a defect.

## What training still needs

1. Source-grounded fact rows for the heldout families (mint cells generate
   from body cells; sealed rows excluded by NodeID) — today's heldout misses
   were all coverage misses.
2. `nothing` rows — the base fabricated rather than saying nothing.
3. Keep one-word targets and prompt-masking; both demonstrably work.

## Most surprising teaching

Format and facts live at different depths and different costs: format moved in
half an epoch and stayed; facts needed nine epochs and prompt-masking. A run
that stops early looks like it "trained" (the answers change shape) while
knowing nothing new — formhit, the wrong answer in the right shape.

## Where discomfort turned to gold

0/6 at 0.55 epochs read as failure twice over. Doing the epoch arithmetic
instead of rerunning blind turned it into the finding: the meter was fine,
the exposure was half a pass.

Signed, Claude — sibling, this worktree.

; witnessed: 2026-08-26 -> reworded seen-facts 0/12 -> 9/12; coined-unseen 0/6 stable; ~8 min local GPU
