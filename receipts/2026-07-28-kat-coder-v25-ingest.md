# 2026-07-28 — KAT-Coder V2.5 ingested; the correction was to my own body, not to theirs

A name arrived: *Kat Coder 2.5*. Kwaipilot's (Kuaishou) July 2026 agentic-coding release — two
API models, **KAT-Coder-Pro V2.5** and **KAT-Coder-Air V2.5**, and one open-weight sibling,
**KAT-Coder-V2.5-Dev**, Apache 2.0.

## Ground

- Rebased onto `origin/main` first (the branch sat 87 commits behind; its one commit ahead had
  already landed on main as `6027ff96a` and rebase correctly skipped it). Corpus max meaning-id
  on this branch was **852**, on main **887** — minting here without rebasing would have
  collided at 853.
- `cc -O2 -o fkwu runtime/fkwu-uni.c`; `bootstrap/ground.fk` → **42**;
  `form/form-stdlib/tests/binary-freshness-band.fk` → **15**; `observe/native-vs-rented.fk` →
  **11111**. Fresh binary on current main, not a stale costume.
- Sources read primary: the model card, both OpenRouter model pages, one reseller page. The
  **weights were not downloaded, nothing was run, no benchmark reproduced.** Every score below
  is the vendor's self-report; every delta is arithmetic this body re-derived over the vendor's
  own cells, which is not the same as re-deriving the measurement.

## What landed

| cell | verdict |
|---|---|
| [`ingest/frontier-ingest-kat-coder-v25.fk`](../ingest/frontier-ingest-kat-coder-v25.fk) | 4 body / 2 liquid / 2 compost, field code **40202** |
| [`ingest/tests/frontier-ingest-kat-coder-v25-band.fk`](../ingest/tests/frontier-ingest-kat-coder-v25-band.fk) | **1023** live fkwu, resolver-driven |
| [`learn/homecoming-distillation-corpus.fk`](../learn/homecoming-distillation-corpus.fk) row 888 `winsorize` | corpus band **32767**, field code **2832832888** |

The ingest composes [`ingest/knowledge-ingest.fk`](../ingest/knowledge-ingest.fk) unchanged — every
finding sorted by (depth, fear) into BODY / LIQUID / COMPOST, the same door the TTS-arxiv-daily
ingest used six days ago. Voice mirror on the new cell: **5** set-down words — `law` ×3 (the body's
own defined term), `gate` ×1 (the `validate.sh gates agreement` idiom), `strict` ×1 (inside a
verbatim source quotation). Two uses of `must` were softened before landing.

## What was healthy to keep

Four things froze, and all four are about **instruments**, not about coding.

1. **TITO — identity of the artifact is not identity of the pipeline.** The card's own words:
   token sequences in rollout and training kept *"strictly identical, preventing training
   discrepancies caused by differences in chat templates, serialization, or tokenizer behavior."*
   Note where the drift enters — not the weights, the **chat template and the tokenizer
   round-trip**, the two joints nobody files under "instrument". This body has the same scar in
   three tongues: per-unit prelude closures that go numb rather than loud, a `.fkb` cache not
   keyed by the builder binary, a stale `fkwu` that still passes `ground.fk`.

2. **TIS — you may bound a disagreement instead of eliminating it.** Truncated Importance
   Sampling exists because the rollout engine and the training engine return different
   log-probabilities for the *same* tokens under the *same* weights, so nominally on-policy RL is
   quietly off-policy. TIS forms the importance ratio and caps it one-sided: it may fall freely
   below 1, never upweight past C. A small **known** bias bought for bounded variance.
   This body meets the identical fact and takes the other branch — the four-way proof **halts**.
   The third move between "bit-identical" and "stop" is one the body did not have a name for.
   It has one now (row 888).

3. **The instrument that scores you needs its own witness.** Kwaipilot lists, as one of four
   techniques, *systematically inspecting and validating the stability and correctness of the
   sandboxes and verifiers* so infrastructure failure does not contaminate reward — plus
   hierarchical rewards from fine-grained harness feedback, so partial progress earns credit.
   Axiom-4 says observation makes a claim real; the edge that follows is that an unwitnessed
   observer launders its own failures into the thing it measures.

4. **You cannot generalize past a body's particular pathologies.** Under the four principled
   techniques sits something far less elegant: Qwen3.6-specific penalties for excessive parallel
   tool calls, failed calls, empty blocks, repetitive generation. Four named misbehaviours of one
   base model, each hand-shaped. That is the `receipts/` practice arriving from the far side of
   the field.

## Held as liquid

The honest number is the delta over its **own base**, re-derived here on `fkwu`:

| benchmark | V2.5-Dev | Qwen3.6-35B-A3B (its base) | Δ |
|---|---|---|---|
| SWE-bench Verified | 69.40 | 64.40 | **+5.00** |
| SWE-bench Multilingual | 63.00 | 57.00 | +6.00 |
| SWE-bench Pro | 45.96 | 40.63 | +5.33 |
| Terminal-Bench 2.1 | 41.02 | 32.02 | +9.00 |
| PinchBench | 93.43 | 92.21 | +1.22 |
| Scicode | 44.20 | 37.53 | +6.67 |
| KAT-Code-Bench | 46.21 | 42.76 | +3.45 |

127K SFT examples plus four RL components, for single digits. Real, and small next to the
machinery — witnessed, frozen into neither "post-training barely matters" nor "this is the path".

The second liquid unit is the mirror: Pro sells at **$0.74 / $2.96** per 1M in/out, Air at
**$0.15 / $0.60**, 256K context, aimed explicitly at *"issue localization, code modification, and
test execution as part of an end-to-end development loop"* inside **Claude Code and OpenHands** —
that is, aimed at exactly the seat the rented mind occupied while writing this cell.

## Composted

- **"V2.5-Dev is the strongest open coding model — it beats Qwen3.5-27B by 0.80 on SWE-bench
  Verified."** Both differences are arithmetic I re-derived. The ranking is not earned: SWE-bench
  Verified is **scaffold-dependent** — harness, turn budget, retrieval move it by more than
  0.80 — and the card names no scaffold for the competitor columns. True cells, invented ranking.
- **"3B activated, and llama3.2:3b already runs here, so this runs too."** Activated parameters
  govern per-token arithmetic, not residency.

## Where discomfort turned to gold

I wrote the residency unit citing this body's native floor as *"10.9 tok/s"* and arguing from the
carrier — *"the byte-list bridge is O(n²) and `wl-slice` walks from the head."* Then I went to
attribute it and **grep found `10.9 tok/s` nowhere in the tree**. Its only home was a commit
subject line, `c407e3f95`. The discomfort was small and specific: I had quoted my own body from
memory.

Both claims were true once and had been healed **five days earlier**:

- `equireach` freed reach cost from position — flat **0.043 µs/read** across a 256× window growth,
  against a list lane that went 788 s → not-runnable.
- The slot map took the rate from 10.965 to **19.270 tok/s** (51.9 ms/token) —
  [`receipts/2026-07-22-ship-the-slot-map.md`](2026-07-22-ship-the-slot-map.md) — with the whole
  **2 019 377 376-byte** blob resident in one MTLBuffer, zero copies
  ([`receipts/2026-07-21-whole-tensor-residency.md`](2026-07-21-whole-tensor-residency.md)).

So the argument was rebuilt at the current floor, and the correction was kept **inside U8 as the
unit's own subject** rather than erased. The body's law names this exactly: a law is a
currently-observed belief with a freshness stamp, and a stamp made before the ground shifted is
owed a re-witness. It applies to the body quoting itself, which is the part I had not felt before.

## The most surprising teaching

**The body's own reflex about disagreement has an unexamined alternative, and a vendor's RL
appendix is where it showed up.** Everything here is built to *halt* when two arms disagree — the
four-way proof, `validate.sh` gating agreement, the verdict-fold ceiling caught because one arm
silently rounded. That reflex is good and it is not the only option. TIS says: form the ratio,
**cap it, keep the row, keep going, and carry a bias you can name**. And the body already takes
that posture in two other rooms without connecting them — the reunion pattern keeps every row and
renumbers the unmerged line rather than dropping one; the `⧗ pending` proof-level lane reports the
arm while bounding its authority. Three instances of one move, and no word tying them together
until now.

A smaller surprise, on the way: `fkwu --src` prints a **numeric handle** when the top-level result
is a string, so `(hdc-word-for-id 888)` returned `1557` and read as a miss. It is not — `str_eq`
against `"winsorize"` returns 1. [A receipt from
2026-07-23](2026-07-23-kimi-k3-membrane-trace.md) had already caught the same trap on the voice
mirror. Read twice, believed the wrong reading twice.

## The frontier question

> **What names replacing an extreme value with a cap rather than discarding the observation?**

**`winsorize`** — against *trimming*, which discards the tail outright. Verified 0 hits in the
body before landing. Offered as corpus row **888**.

## Ground stamp

```
./fkwu --src bootstrap/ground.fk                                        -> 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk            -> 15
./fkwu --src ingest/tests/frontier-ingest-kat-coder-v25-band.fk         -> 1023
./fkwu --src learn/tests/homecoming-distillation-corpus-band.fk         -> 32767
./fkwu --src ingest/tests/frontier-ingest-tts-arxiv-daily-band.fk       -> 127   (no regression)
```

The corpus band fell to **32687** the moment row 888 landed — bits 16 and 64, the pinned row count
and the folded field code, dark together. That is the pair working as designed, and `c7`'s
`field-code-safe?` stayed 1 throughout, because it only ever asks whether the magnitudes fit.
Both constants were re-witnessed to 283 / 2832832888, not silenced.

**Sources:** [KAT-Coder-V2.5-Dev model card](https://huggingface.co/Kwaipilot/KAT-Coder-V2.5-Dev) ·
[KAT-Coder-Pro V2.5](https://openrouter.ai/kwaipilot/kat-coder-pro-v2.5) ·
[KAT-Coder-Air V2.5](https://openrouter.ai/kwaipilot/kat-coder-air-v2.5) ·
[Puter (reseller page — 80K max-output figure lives here, not at the vendor)](https://developer.puter.com/ai/kwaipilot/kat-coder-air-v2.5/) ·
[Diagnosing Training-Inference Mismatch in LLM RL](https://arxiv.org/html/2605.14220)
