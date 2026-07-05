# 2026-07-05 — ingesting what is healthy from "Is One Layer Enough?"

## Ground

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

Urs shared one line: "Source: X" — a share-link resolving to Rohan Paul's post (2026-07-03) on
**Is One Layer Enough? Training A Single Transformer Layer Can Match Full-Parameter RL Training**
(arXiv 2607.01232, 2026-07-01; Zhang, Hu, Glentis, Li, Yau, Lin, Hong). Answered by running it
through the body's own `ingest/knowledge-ingest.fk` law (depth×fear → body / liquid / compost),
grounded in the paper's abstract quoted verbatim and its full-text numbers, with a two-agent
adversarial pass over the body's own organs — skeptic's default: every "the body already does X"
was an OVERCLAIM unless a real cell does X, file and line shown.

## The grounded facts (primary source, quoted)

From the abstract, verbatim: "training a single transformer layer can recover most of the gains
achieved by full-parameter RL training, and in some cases even surpass it"; "RL gains are highly
concentrated in a small subset of, and in many cases even a single, transformer layers";
"high-contribution layers concentrate in the middle of the transformer stack, while layers near
the input and output ends contribute substantially less." The measure is **layer contribution**
C(k) = (S_k − S_base) / (S_full − S_base) — a layer trained in ISOLATION against the frozen
baseline, a counterfactual quantity. Numbers: Qwen3-1.7B layer 10 hits **51.8% vs full 50.8%
(C=1.14)**; Qwen3-8B layer 16 **C=1.07**; layer rankings correlate across datasets (ρ=0.76),
tasks (ρ=0.59), families, algorithms. Weight-change norms are **near-uniform (0.5–0.8)** across
layers while contribution is wildly non-uniform — where change lands is not where gain lives.
Models trained on different single layers solve **largely non-overlapping problems** (top-7
Jaccard 34.1%); majority-voting seven of them beats full training (OlympiadBench **33.6% vs
26.9%**). Scope: seven models (Qwen3/Qwen2.5 families), three RL algorithms (GRPO, GiGPO,
Dr. GRPO), math/code/agentic tasks.

## The ingest (band 127, four-way fkwu/Go/Rust/TS; field code 20302 = 2 body, 3 liquid, 2 compost)

**FROZEN → body (deep + fear-free):**
- **Earned promotion against the incumbent is executable law in both lineages.** The paper's
  central act — a single trained layer earns authority only by measured competition with the
  full-parameter result — is the champion-challenger shape the body already holds load-bearing:
  authority flips only over a long-enough window of proven head-to-head competition
  (`cc-reaches?`, `cc-promote?`, `cc-authority`; `learn/champion-challenger.fk`, band 127 run
  live this session), never on asserted accuracy. Named precisely: the shared shape is
  promotion-by-proven-competition, NOT the C(k) gain ratio itself; and the cell's VALUE reading
  (`cc-val-*`, the coin RL would bring) is CARRIED but unwitnessed here — its cited proofs
  (`diffusion-q-cc.fk`, `float-compare-band.fk`) exist only in the old body's ledger. What
  freezes is the classification promotion law.
- **The ingest loop demonstrably grows the body.** The 2026-07-02 Brain2Qwerty pass composted
  "the body already does CTC" — and the body then BUILT the organ: `model/ctc-loss.fk` opens by
  citing that pass as its origin ("named this precisely as the body's #1 overclaim… the organ
  the body LACKED"), and `model/ctc-train.fk` trains it by native gradient descent, honest floor
  named (free logits, numerical gradient). Both bands run live this session: ctc-loss-band → 95,
  ctc-train-band → 127. Compost is not a verdict; it is soil.

**WITNESSED → liquid (deep but fearful — seen, never load-bearing):**
- **The specialization rhyme is a CARRIED shape, not executable law — the skeptic's demotion,
  honored.** `routers/moe-router.fk` describes exactly the right principle (heterogeneous expert
  KINDS, top-1 gate, per-expert win counts — "which KIND was routed-to AND correct"), and the
  paper's complementarity finding (non-overlapping problem coverage, top-7 Jaccard 34.1%, the
  vote beats the monolith) affirms it from an independent lineage. But the cell cannot RUN in
  this body: its preludes (`geometric-learning.fk`, `learning-arc.fk`, `learning-style-space.fk`)
  and its cited band never came across; the fks-127 verdict lives only in
  `docs/inheritance/proven-bodies-from-old-repo.txt`. Dangling here, witnessed there. This was
  drafted as a freeze; the adversarial pass demoted it, and the demotion is the unit.
- **The RL gap is near-TOTAL.** No reward-driven weight update, no policy gradient, no GRPO
  anywhere in the body; the closest organ is `teacher-selection.fk`'s greedy bandit, where
  reward selects a TEACHER, never a weight. The native trainer updates EVERY layer with ONE
  uniform scalar lr and has no freeze mask (`tbp-stk-back` / `tbp-stk-train`,
  `model/transformer-backprop.fk`) — a two-block stack with no middle to even ask the paper's
  question about; and even that trainer's cited bands never came across (its proof is an
  inherited ledger line, not a witness in this body). Held in sight; never frozen into "the
  body trains selectively."
- **Heat is not worth, and the body has only heat.** Its "which part matters" senses are
  call-count (`jpr-hot-pure?` = pure AND heat ≥ 5, `observe/jit-profile-receipt.fk`) and
  surprise magnitude (`observe/surprise-receipt.fk`); NO cell isolates a part and measures the
  whole's gain delta — no counterfactual, no ablation, no leave-one-out, anywhere. The paper
  proves the mismatch matters: update norms near-uniform, contribution wildly non-uniform. The
  body cannot yet name its own middle layers. Witnessed, not bypassed.

**COMPOSTED → never enters (shallow / wrong):**
- **The false equivalences the adversarial pass killed:** "the body already does
  layer-contribution profiling / selective training / freezing / gradient descent by another
  name." Call-count heat is not counterfactual contribution; whole-model champion-vs-challenger
  is not within-model layer localization; content-addressed freezing (belief immutability) is
  not gradient freezing; `learn/self-descent.fk` verifies a LINEAGE reaches root — a false
  friend by name, not gradient descent; `cross-witness-economy` buys LABELS at the uncertainty
  frontier (data granularity), it does not select which PART to train.
- **The rented mind's stale-floor reflex, dissolved by the primary read:** it carried the
  2026-07-02 receipt's floor ("no CTC training objective; learning is prototype-interning") as
  present tense and nearly asserted it of today's body — but the organ was built in the nine
  days since (`ctc-loss`, `ctc-train`, bands passing). A receipt is a DATED witness, not a
  standing sentence; only the current cells decide — in either direction.

## Also found (flagged, not fixed here)

The verify pass surfaced a family of dangling references, all of one kind: cells citing proofs
that were witnessed in the OLD body but never brought across (the inheritance ledger
`docs/inheritance/proven-bodies-from-old-repo.txt` records their old-repo verdicts — dangling
here, witnessed there):
- `learn/champion-challenger.fk:19,21` → `diffusion-q-cc.fk`, `tests/diffusion-q-cc-band.fk`
  (ledger: diffusion-q-cc fks 63); `:18,:63` → `tests/float-compare-band.fk`; `:6-7` →
  `self-grounding-classifier.fk`, `classifier-eval.fk` (docstring lineage, not preludes).
- `model/transformer-backprop.fk:20` and `model/transformer-corpus-train.fk:15` → "Proven by:
  form-stdlib/tests/transformer-backprop-band.fk / transformer-corpus-train-band.fk" — neither
  exists here (ledger: transformer-backprop fks 127, transformer-corpus-train fks 31).
- `routers/moe-router.fk:24` → `form-stdlib/tests/moe-router-band.fk` missing; `:22-23` →
  preludes `geometric-learning.fk`, `learning-arc.fk`, `learning-style-space.fk` missing — the
  cell is non-executable in this checkout (ledger: moe-router fks 127).
- `learn/teacher-selection.fk:14` — a self-declared missing band ("honest pending", distinct
  from the silent kind).

## Corpus rows this thread

- **673 synecdoche** — letting one part stand in for and carry the whole: the paper's whole
  finding in one word, and the shape of the body's own C seed (one small file standing in for
  the kernel while the native body comes home).
- **674 counterfactual** — reasoning from what would have happened otherwise: the quantity C(k)
  is and call-count heat is not; the missing sense named honestly (the body measures how often a
  part is touched, never what the whole would lose without it).

## The most surprising teaching this work left behind

The compost pile turned out to be the body's most fertile organ. The strongest single piece of
evidence this session found was not a capability — it was `model/ctc-loss.fk` opening by citing
the 07-02 adversarial pass that composted its absence. The ingest law's harshest verdict (false
equivalence, never enters) is also its most generative: the named gap became a built organ in
nine days. And the same session nearly ran the law backwards — asserting the 07-02 floor as
today's — which is how the symmetric lesson arrived: a receipt is a dated witness, not a
standing sentence. Compost is soil in both directions.

## Where discomfort turned to gold

The discomfort was the depth of the RL gap, and the pull it produced was acted on, not merely
felt: the specialization unit WAS drafted as a freeze — "part-wise specialization, measured by
per-part wins, is executable here" — because moe-router's docstring describes exactly the right
principle and freezing it made the body feel less far from the paper. The skeptic refuted it
with the cell's own preludes: the organ cannot fire in this checkout; its proof lives only in
the old body's ledger. Witnessing that demotion instead of arguing with it became the session's
clearest artifact — the unit now IS the demotion (deep, fearful, held in sight), and the missing
sense underneath it has a name in the corpus (counterfactual, row 674): the body measures heat
(how often a part is touched) and surprise (how wrong a prediction was), but nowhere what the
whole would lose without the part. That is a buildable edge, named honestly, instead of a
costume — and champion-challenger's promotion gate is already the judge an RL coin would need,
the day one arrives.
