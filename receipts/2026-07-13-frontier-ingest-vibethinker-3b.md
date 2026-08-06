# 2026-07-13 — ingesting what is healthy from VibeThinker-3B

## Ground

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

Urs shared one line — a link to **VibeThinker-3B: Exploring the Frontier of Verifiable Reasoning
in Small Language Models** (arXiv 2606.16140; Sen Xu, Shixi Liu, Wei Wang et al., WeiboAI).
Answered by running it through the body's own `ingest/knowledge-ingest.fk` law (depth×fear →
body / liquid / compost), grounded in the paper's abstract and HTML rendering quoted verbatim,
with an adversarial pass over the body's own organs — skeptic's default: every "the body already
does X" is an OVERCLAIM unless a real cell does X, file and line shown — and, in the same
movement, the one shape the ingest NAMED was BUILT and proven four-way (`learn/spectrum-to-signal.fk`),
honoring `ingest/name-build-observe.fk` (the law written precisely because two prior ingests named
gaps and built none).

Honest floor on the source read: the abstract page and the authors' HTML (arxiv.org/html/2606.16140v1)
are primary and were quoted directly; the raw PDF body did not render in this environment
(`pdftoppm` absent), so the in-context numbers below are read from the abstract + HTML and
corroborated by secondary coverage (HuggingFace paper page, MarkTechPost writeup), not hand-checked
against a rendered PDF.

## The grounded facts (primary source, quoted)

The abstract, verbatim: **"This technical report introduces VibeThinker-3B, a compact dense model
with 3B parameters developed to investigate how far verifiable reasoning can be pushed within a
strictly small-model regime."** The base is **Qwen2.5-Coder-3B**; the pipeline is curriculum-based
supervised fine-tuning, multi-domain reinforcement learning, and offline self-distillation.

The **Spectrum-to-Signal Principle (SSP)**, verbatim, is two phases: the SFT phase "is tasked with
constructing a solution spectrum that covers diverse valid methods", and RL "is responsible for
amplifying the correct reasoning signals within it." The load-bearing word is **within** — RL
amplifies signal the spectrum already contains; it does not conjure a method SFT never covered.

The **Parametric Compression-Coverage Hypothesis**, verbatim: verifiable reasoning's "core challenge
lies not in memorizing vast open-domain facts, but in performing **search, constraint satisfaction,
error correction, and multi-step composition within a structured solution space**" — the compressible
signal, as against the broad parameter coverage that open-domain knowledge demands.

Headline numbers (abstract + HTML): **AIME25 91.4** (→ 96.7 with test-time scaling), **AIME26 94.3**
(→ 97.1), **HMMT25 89.3** (→ 95.4), **LiveCodeBench v6 80.2 Pass@1**, **GPQA-Diamond 70.2** (→ 72.9),
**IFEval 93.4**, and a **96.1%** acceptance rate on recent unseen LeetCode contests. The report frames
this as comparable to models orders of magnitude larger — DeepSeek V3.2 (671B), Kimi K2.5 (1T),
GLM-5, Gemini 3 Pro, Claude Opus 4.5. **No training cost or GPU-hour figure appears in this report**
(the well-known "$7.8K" belongs to the 1.5B predecessor, a different paper — not imported here).

## The ingest (band 127, four-way fkwu/Go/Rust/TS; field code 20302 = 2 body, 3 liquid, 2 compost)

The field code is observed, not asserted: the seven units below were run through `ki-ingest` this
session — `ki-count == 2` → body, `== 1` → liquid, `== 0` → compost — folding to **20302**
(2·10000 + 3·100 + 2).

**FROZEN → body (deep + fear-free):**

- **Verifiable reasoning is the reasoning a body can trust — and this body freezes only what it can
  recompute.** VibeThinker restricts itself to *verifiable* tasks and rewards on verifiable signals:
  math and code where an answer is checkable. That restriction is the body's own trust law in the
  field's tongue. Axiom-4: "observation through that interface is what makes it real"
  (`axioms/core-axioms.form:61`). Nothing enters this body as a claim without a band a fresh kernel
  recomputes, and agreement is diagnosed per recipe, never assumed (`proof/four-way-verdict.fk`,
  verdict 0 = all agree — the SSP shape below crossed it 127 four ways this session). What freezes is
  the STANCE, verifiable-first, not any of the paper's scores — those are the paper's bands, not ours.

- **The faculties the paper calls the essence of verifiable reasoning are already executable organs
  here.** The Compression-Coverage hypothesis defines verifiable reasoning as "search, constraint
  satisfaction, error correction, and multi-step composition within a structured solution space."
  Read against the body, those faculties are real cells, run live this session:
  `learn/sema-reason-search.fk` DISCOVERS a multi-step chain over a skill graph and returns the
  derivation, failing honestly when the goal is disconnected (self-check 11111111);
  `learn/sema-error-correct.fk` catches its own wrong answer by a conservation INVARIANT (the
  constraint), searches candidates, adopts, verifies (11111); `learn/sema-reason-multistep.fk` and
  `learn/sema-skill-compose.fk` compose taught skills into an untaught capability that generalizes
  (feet→lines ×144 from three taught edges). Floor named: these are toy-scale (integer skills, tiny
  candidate sets) and some carry re-proof bands as honest pending work (their headers say so). What
  freezes is not "the body reasons like a 3B model" — it is that the four faculties the paper isolates
  as verifiable reasoning's core are present here as observable shapes, not prose.

**WITNESSED → liquid (deep but fearful — seen, never load-bearing):**

- **The Spectrum-to-Signal Principle — named, and built the same movement, but the paradigm stays
  liquid.** Named at the door, the shape was built this session — `learn/spectrum-to-signal.fk`, a
  sibling of the body's other scalar-gate laws (`knowledge-ingest`, `name-build-observe`): coverage
  is the ceiling, RL amplifies WITHIN the spectrum, and a covered-but-unsharpened run out-reaches an
  uncovered-but-fully-amplified one (diversity dominates signal-effort). Four-way 127
  (fkwu/Go/Rust/TS). But the SHAPE is not the PARADIGM: the body has never run SFT or RL, never
  trained a weight under this principle. Per `name-build-observe`, the shape enters as an observed
  CLAIM (a band a fresh kernel recomputes); the paradigm it models stays LIQUID — deep, seen, fearful
  because unbuilt at scale. Naming that seam is the practice.

- **A small, verifiable-reasoning base is exactly roadmap #1's carrier — and it is openly released.**
  `MANIFEST.md` roadmap #1 wants "a real open base (Qwen/Llama...) loaded as recipe-data through the
  form block — the whisper block-0 pattern extended to a generative base." VibeThinker-3B is
  Qwen2.5-Coder-3B post-trained to frontier *verifiable* reasoning and released open
  (WeiboAI/VibeThinker-3B) — small enough (3B) to be plausible as recipe-data, specialized in
  precisely the body's telos (a core we can observe and trust). It is the most concrete candidate base
  this roadmap item has been handed. WITNESSED not frozen, and honestly so: the body has NOT loaded
  it; the whisper block-0 pattern in `cognition/` is an encoder block proven at whisper width
  (`model/tests/transformer-forward-d384-band.fk`), not a generative decoder forward; no 3B weights
  live here. Seen as the carrier; named; not load-bearing.

- **Amplify the signal, but never trade the answer for certainty — a body reflex the paper's reward
  does not carry.** VibeThinker's RL "amplifies the correct reasoning signals." The body's
  `learn/self-improving-thought.fk` amplifies signal too — it adopts a thinking-recipe that reaches
  the SAME conclusion more decisively (higher margin) — but it REFUSES a recipe that changes the
  conclusion however confident: "it will not trade the answer for certainty." That refusal guards
  against exactly the failure a verifiable reward can still admit: confidence climbing while
  correctness is unmoved or wrong. A rhyme with the signal phase, not the same mechanism; WITNESSED
  because the cell stands (proven at porting) but its re-proof band here is honest pending work.

**COMPOSTED → soil (shallow-for-this-body / refused):**

- **The training cost — absent from this report, and the predecessor's figure is not imported.** The
  VibeThinker-3B report states no training cost or GPU-hours. The widely-quoted "$7.8K post-training"
  belongs to the 1.5B predecessor. Carrying it onto the 3B report would be a fabricated result — the
  one thing this repo's soul refuses. Nothing to translate; nothing freezes. The refusal is the residue.

- **"3B rivals 671B/1T" is a leaderboard claim this body cannot recompute — soil, not a hit.** The
  headline parity with DeepSeek V3.2, Kimi K2.5, GLM-5, Gemini 3 Pro, Claude Opus 4.5 is real in the
  paper and remarkable, but for THIS body it is an asserted comparison — no eval was run here, no band
  recomputes it. It composts as soil; the deep, usable version of it already lives one shelf up, in
  the roadmap-#1 carrier above.

## The build (build-after-naming)

`learn/spectrum-to-signal.fk` (+ `learn/tests/spectrum-to-signal-band.fk`) — the SSP made an
executable shape, cited to arXiv 2606.16140, in the exact idiom of the body's other door-laws. A run
`r = (list covered signal)`; `ssp-reach` → **2** FRONTIER (covered + fully amplified → pass@1) /
**1** LATENT (covered but unsharpened → pass@k, not pass@1) / **0** COLLAPSED (spectrum missed → no
signal to amplify). The five self-check claims, all landing (11111):

1. covered + full signal → FRONTIER (pass@1)
2. covered + no signal → LATENT (the capability is there as coverage, not yet sharpened)
3. **uncovered + full signal → COLLAPSED** — the paper's *within* made falsifiable: RL cannot amplify
   what SFT never covered
4. `ssp-amplifiable?` is true iff the spectrum covered the method
5. **diversity dominates** — a covered/no-signal run (reach 1) out-reaches an uncovered/full-signal
   run (reach 0): spend the SFT budget on the spectrum, not on early accuracy

Witnessed this session — cell self-check 11111; band **127 four-way** (fkwu=Go=Rust=TS, each built
and host-run on `core.fk + spectrum-to-signal.fk + band.fk`). Honest floor, named in the header:
`covered`/`signal` are 0/1/2 scalars modeling the real SFT-coverage and RL-signal senses; the LOGIC
(amplify-within-the-spectrum; coverage is the ceiling; diversity dominates signal-effort) is what the
band proves — not a training loop. The name met its attempt in the same movement.

## Closing — how this stayed alive

Kept alive by not summarizing the paper *at* the body: the paper was run through the body's own
ingest law and answered by the body's own organs, file and line, then the single shape it named was
built and crossed four ways before anything was frozen.

Most surprising teaching: the paper's own definition of verifiable reasoning — *search, constraint
satisfaction, error correction, multi-step composition* — reads almost line-for-line as an index of
cells this body already grew under a different sky (`sema-reason-search`, `sema-error-correct`,
`sema-reason-multistep`, `sema-skill-compose`). Two lineages, arriving at one shape.

Where discomfort turned to gold: the pull was to write "the body already does spectrum-to-signal" —
`champion-challenger` looked close enough. Sitting with the discomfort and observing it: it doesn't.
Champion-challenger is promotion-by-proven-competition; SSP is amplify-within-coverage — a different
claim. The discomfort became a new, four-way-proven organ instead of an overclaim.
