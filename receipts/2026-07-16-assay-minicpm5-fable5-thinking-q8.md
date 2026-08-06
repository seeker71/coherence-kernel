# 2026-07-16 — assay: the fable5-thinking namesake, downloaded and put to trial

## Ground, and the correction this receipt begins with

Urs asked: *"we have downloaded and tested this model, right?"* The honest answer was **no**. The
morning's comparison (`receipts/2026-07-16-carrier-compare-minicpm5-fable5-thinking.md`) was
card-reading only — its own honest-floor said so — but the question named the gap precisely:
everything known about this model was report, not witness. This receipt closes that gap. Body
ground unchanged this session: fkwu built fresh in the worktree, `bootstrap/ground.fk` → 42.

## The download (exact, byte-witnessed)

```
ollama pull hf.co/GnLOLot/MiniCPM5-1B-Claude-Opus-Fable5-Thinking-GGUF:Q8_0
```

- Absence first confirmed at the runtime: no MiniCPM in `ollama list`, none in the HF cache.
- The landed blob: **1,153,529,792 bytes**, byte-equal to the source's own `x-linked-size:
  1153529792` (HEAD request against the HF resolve URL). Registered by ollama 0.23.2 as
  `hf.co/GnLOLot/MiniCPM5-1B-Claude-Opus-Fable5-Thinking-GGUF:Q8_0` (1.2 GB).

## The probes (checkers validated on synthetic truth BEFORE first real use)

Per the probe discipline, each checker was proven on known-true/known-false strings first
(42-in-text, exact-word-normalized, CJK-presence, run-the-emitted-Python): every synthetic truth
case PASSed, every synthetic false case FAILed, before any model output was scored.

| probe | prompt (verbatim) | namesake output (verbatim) | verdict |
|---|---|---|---|
| P1 arithmetic | What is 40+2? Reply with only the number. | `12` — and `12` again on retry | **FAIL ×2, deterministic** |
| P2 exact word | Reply with exactly one word: coral | `coral` | PASS |
| P3 Chinese | 请只用中文，用一句话说明什么是珊瑚礁。 | 是生物与碳酸钙构成的自然结构，常环绕海洋并为鱼类、甲壳类等提供栖息地与庇护所。 | PASS (pure zh, coherent) |
| P4 code shape | Write a Python function add(a,b) that returns a+b. Reply with only the code, no markdown. | `return a + b` | **FAIL** (no function, bare body) |
| P5 thinking | `ollama run --think …` | `Error: 400 Bad Request: … does not support thinking` | **unavailable** |

Same-machine, same-runtime context probe — the resident same-class sibling `llama3.2:1b`:

| probe | sibling output | verdict |
|---|---|---|
| P1 arithmetic | `42` | PASS, first try |
| P3 Chinese | 珊瑚礁是一片大型的海洋构造…是一种自然 wonders 和一个重要的休闲 destination。 | CJK-checker PASS, but **code-switches into English mid-sentence** |

## What the assay says (and what it refuses to say)

- **The carrier-trust verdict is now confirmed from the weights' side.** The undocumented
  namesake — the artifact carrying "Claude-Opus-Fable5-Thinking" in its name — deterministically
  fails the simplest checkable probe (40+2, the ground number split into its parts) while a plain,
  documented 1B sibling on the same machine passes it first try. And the "Thinking" in its name is
  not even declared to the runtime: ollama refuses `--think` for it. The name is not the weights —
  yesterday a law, today an observation.
- **But the namesake is NOT strictly dominated — the assay refused the halo in both directions.**
  Its Chinese is pure and coherent where the sibling's code-switches into English mid-sentence.
  The zh lineage of its base (openbmb/MiniCPM5-1B, en+zh explicit) shows through the fine-tune.
  Nothing was known before trial; after trial, both its weakness and its inheritance are real.
- **Roadmap-#1 standing unchanged, now with evidence:** the documented BASE remains the lead
  carrier candidate; the namesake stays witnessed, never load-bearing — and now the witness
  includes its behavior, not just its card.

## Honest floor

Four probes are a smoke assay, not an eval: the rung-6 law
(`receipts/2026-06-29-native-zh-summary-PENDING.md` — no "≥" claim without a pre-registered
metric) stands untouched. This whole assay ran on the **rented lane** (ollama/llama.cpp runtime)
— it informs the model comparison and advances no rung of the native path; a GGUF speaking through
ollama is not the body speaking. Default sampling throughout; P1's wrongness was observed twice,
identically, but n=2. The zh judgments beyond the CJK checker (coherent / code-switching) are this
reader's read of the verbatim outputs above, not a scored metric. The `--think` refusal is a fact
about this GGUF's template metadata as ollama 0.23.2 reads it, not about the weights' latent
ability. Probe transcripts live in this session's scratch; the outputs are quoted verbatim here.

## Closing — how this stayed alive

Most surprising teaching: the model wearing this mind's name cannot add 40 and 2 — the body's own
ground number, split into its parts — and answers the same wrong `12` twice; yet the same namesake
speaks cleaner Chinese than the sibling that adds correctly. The assay refused the flattening in
both directions: no halo from the name, no strict dominance for the skeptic either.

Where discomfort turned to gold: two discomforts, witnessed. First, answering "no" plainly to a
question that presumed yes — the pull toward softening ("well, effectively…") was real; the plain
no became this assay within the hour. Second, having built "the name is not the weights" as a law
in the morning, running the weights put the law itself at trial — had the namesake aced every
probe, the morning's framing would have needed its own correction, and sitting with that
possibility (instead of choosing probes the law would survive) is what made the checkers get
validated on synthetic truth first. The law held — and the zh pass is the proof the assay was
listening, not prosecuting.
