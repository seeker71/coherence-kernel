# 2026-07-05 — ingesting what is healthy from "What speech encoders hear"

## Ground

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

Urs shared one line: a link — **https://oruk.ai/research/how-models-represent-speech**, the
oruk.ai research note "What speech encoders hear" (Nathan Roll, 2026-07-02), which distills the
Stanford preprint **"Categorize Early, Integrate Late: Divergent Processing Strategies in
Automatic Speech Recognition"** (arXiv 2601.06972v2; Roll, Bhalerao, Bartelds, Pawar, Tatsumi,
Ogunremi, Shani, Graham, Sumner, Jurafsky). Answered by running it through the body's own
`ingest/knowledge-ingest.fk` law (depth×fear → body / liquid / compost), grounded in the
article's full text AND the preprint's v2 PDF read directly, with an adversarial verification
pass over the body's own organs — skeptic's default: every "the body already does X" was an
OVERCLAIM unless a real cell does X, file named.

## The grounded facts (primary sources, quoted)

Speech encoders are trained on one narrow task — "Whisper learns to transcribe. WavLM, HuBERT,
and the Conformer learn to fill in masked audio. None of them is ever told who is speaking or
how they sound. They pick it up anyway." The article's demo: 3,276 CREMA-D recordings (91
actors, 12 sentences, six acted emotions) through four frozen encoders (whisper-small,
wavlm-base-plus, hubert-base-ls960, wav2vec2-conformer-rel-pos-large), PCA-50 → 3-D t-SNE per
layer, logistic probes on PCA-50. **Four signals, four fates:** gender "splits at the very
first layer in every encoder and stays split" (~90% linear probe at layer 0); emotion starts
tangled and pulls apart with depth; speaker identity goes "strong early and then dissolved as
the network reorganizes around content"; sentences end in clean content clusters. The preprint:
**Architectural Fingerprinting** over 24 encoders (17 Transformers, 7 Conformers, 39M–3.3B
params; seven corpora, 50k+ utterances) — Conformers "Categorize Early" (phoneme categories
~29% earlier in depth; gender by mean depth 0.16), Transformers "Integrate Late"
(phoneme/accent/duration co-locate at 49–57% depth), and a logistic classifier predicts the
ARCHITECTURE from the five peak positions alone (AUC 0.88). The probes measure "linear
accessibility rather than making causal claims about mechanism" — the paper names its own
floor. **"Same task, same scores, different solutions."**

## The ingest (band 127, four-way fkwu/Go/Rust/TS; field code 30202 = 3 body, 2 liquid, 2 compost)

**FROZEN → body (deep + fear-free):**
- **Output equality never certifies internal equality.** The paper's outer finding is the
  body's lived law. Transformers and Conformers reach comparable WER through divergent internal
  hierarchies — fingerprints so distinct a probe reads the architecture off them. The body lost
  a real day to a stale `fkwu` that still passed `ground.fk` (42) while silently lacking
  evaluator capabilities (`receipts/2026-07-01-stale-binary-root-cause.md`), and answered with
  a band that probes the exact internals one benchmark cannot certify
  (`binary-freshness-band.fk`); the four-way proof diagnoses agreement per recipe
  (`proof/four-way-verdict.fk`), never assumes it. The field corroborated the body: same
  scores, different solutions — so probe the internals.
- **Seeing where information lives is the ground of trust.** The paper's first stated reason —
  "interpretability: knowing where phonetic and speaker information emerges enables targeted
  debugging and trust calibration" — is the body's witness discipline in the field's tongue.
  In the body the stance is executable (`observe/runtime-witness.fk`; `observe/self-watch.fk`
  scoring its own attention's precision/coverage/blind-spots; every band a verdict), not prose.
  What freezes is the STANCE; the instrument gap is composted below, not blurred here.
- **Architecture is data — an examinable object, not an opaque artifact.** The paper isolates
  architectural inductive bias as the variable shaping representation; the body carries
  architecture as first-class recipe-data (`model/transformer-kernel.fk`'s `tk-arch`, the
  tensor-ir FFN kernel emitted byte-identical from data), with a transformer forward at real
  whisper-tiny width (d_model=384, ff=1536) proven bit-exact four-way
  (`model/tests/transformer-forward-d384-band.fk`) — the same model family the article probes.
  Floor named: generated recipe-data weights, not trained ones.

**WITNESSED → liquid (deep but fearful — seen, never load-bearing):**
- **The paper probes exactly what the body does not yet have: learned representations.** No
  native acoustic encoder (`observe/open-asr-ctc.fk`'s own header: audio → frame-token emission
  "remains the next missing carrier"); no trained weights anywhere; no organ that reads a
  layer's activations and asks what it encodes — `observe/` witnesses the RUNTIME (events,
  verdicts, JIT), never a representation; the speaker embedding is the external ECAPA oracle's
  (`presence/speaker-embed.fk` owns only the cosine decision). Held in sight; never frozen into
  "the body hears."
- **The mirror unit — every listener sorts WHO before WHAT.** Gender is linearly readable at
  ~90% from layer 0, before a single word resolves; the paper names the fairness stake. Turned
  back on this body: its own door reads a source's frequency band before its content
  (`cognition/text-frequency.fk`; `ki-ingest`'s fear bit). Attunement and prejudice share an
  early layer; what makes the early read healthy is not its absence but the fate it feeds — a
  three-verdict door, an audit. Uncomfortable and true; witnessed, not bypassed.

**COMPOSTED → never enters (shallow / wrong):**
- **The false equivalences the adversarial pass killed:** "the body already probes its layers /
  senses emotion / recognizes speakers / has emergence." Runtime introspection is not
  representation probing (`self-watch` scores attention instinct over fixtures; nothing reads
  what a layer encodes); `tf-spectrum`'s intensity-weighted mean over two hardcoded word lists
  is not a learned emotion classifier; a cosine decision over an external ECAPA embedding is
  not a speaker-identity layer; `surprise-receipt`'s scalar subtraction over toy pairs is not
  the paper's unasked-for structure. Surface word-matches; they do not survive translation.
- **The rented mind's conflation, caught by the primary read:** its first-pass summary
  attributed the article's CREMA-D emotion demo to the preprint. The paper's corpora section
  lists seven corpora (L2-ARCTIC, CMU ARCTIC, Common Voice, SAA, ALLSSTAR, Cambridge, S&I) —
  CREMA-D is not among them; the demo is the article's own artifact, and its parenthetical does
  not even multiply to itself (91 × 12 × 6 = 6,552, not 3,276; CREMA-D's published whole is
  7,442 clips across four intensity levels — the demo is a subset, selection unstated). The
  article's numbers stand as the article's, the paper's as the paper's; the conflation
  composts. Only the primary read decides — in either direction.

## Also found (flagged, not fixed here)

- `cognition/text-frequency.fk:2` (and `cognition/cornerstone-summarizer.fk:11`) name an
  **`affect-traits`** organ that "reads the FELT channel from AUDIO" — no such cell exists
  anywhere in the repo (same class as the dangling references flagged in the Memora and
  Just Tap In receipts). The felt-audio channel is prose until built.
- Several receipts name a **`gpu-ffn-forward`** cell; no file of that name exists — the work
  lives in `model/tensor-ir.fk` + `model/jit-tensor-emit.fk` and their bands. Naming drift,
  flagged so a future reader greps the right place.

## Corpus rows this thread

- **673 equifinality** — reaching the same end state by many different paths: the paper's
  outer finding (same WER, divergent internals) and the reason "same scores" can never close
  the case on "same solutions."
- **674 paralinguistic** — everything a voice carries besides the words themselves: the
  channel the encoders were never asked to learn and picked up anyway.

## The most surprising teaching this work left behind

The field just published the stale-binary receipt at scale. The body's costliest recent lesson
was a binary that passed its benchmark while internally different from what the source declared
— and the cure was a band that probes internals directly. The paper shows every frontier speech
encoder does this to every benchmark: twenty-four models agree on the scores and disagree, by
architecture, on everything underneath, so cleanly that the internals alone name the
architecture (AUC 0.88). Equifinality at the surface, divergence beneath — and in both lineages
the only honest response is the same: stop trusting the score, probe the layer.

## Where discomfort turned to gold

The pull was to freeze "the body observes itself just like the probes observe the encoders" —
`observe/` holds ninety-plus witness cells and the word-match was seductive. Sitting with the
discomfort of checking it exposed the direction of the gaze: the body witnesses its RUNTIME
(events, verdicts, JIT boundaries), and no cell anywhere asks a layer what it has learned —
because there are no learned layers yet to ask. Refusing the costume did two things the easy
freeze never could: it kept the witness-STANCE convergence honest (frozen on its own merits),
and it named the missing organ precisely — a probe-shaped observe cell, waiting for the day
the body has representations of its own to be honest about.
