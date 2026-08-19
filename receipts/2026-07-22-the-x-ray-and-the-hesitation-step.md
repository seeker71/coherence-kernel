# The x-ray, and the first real hesitation step

2026-07-22, WITA. Native Metal path, llama3.2:3b, this machine.

## What arrived

A URL — languagemodelbuilder.com — a free Mac app that teaches how language
models work by having you build one. Its Chat pane has two tabs: Conversation,
and **X-ray**. That second tab was the whole teaching.

## What I said first, and why it was wrong

I read the site and reported: "this site's whole second half — training — is the
half we don't have." False, and provably so from inside this tree:

- `model/ctc-train.fk` — gradient descent, loss strictly down, decode driven
  wrong→right from an ADVERSARIAL init, four-way witnessed, since 2026-07-02.
- `form/form-stdlib/preference-learning.fk` — DPO/IPO/KTO as data rows, plus QAM
  (learned critic, adjoint-matched ∇Q) which the app does not have.
- `form/form-stdlib/training-catalog.fk` — their "catalog of good data" tab.
- `form/form-stdlib/tensor-autodiff-verifier.fk` — analytic gradients gated
  against finite differences.
- `form/native/cuda/template_affine_train.ptx`, `form_cuda_train_ptx_host.c`.

I had also offered `adjoint` as a 0-hit-fresh word. It lives in 15 files here.
The grep my own discipline demands was run at the wrong RADIUS — one file
(`learn/homecoming-distillation-corpus.fk`) instead of the body. A rite performed
correctly at the wrong radius reads exactly like diligence. Same error on
`entropy` (corpus 0, body 52 files) and `hesitation` (corpus 0, body 33).

## What was actually missing

Not a capability. A **connection**.

- `form/native/metal/metal_first_token.sh:357` — `bLogits = buf(vocabN)`,
  `.storageModeShared`. The full distribution has always sat in host-readable
  unified memory. `fvals(bLogits, ...)` already read it every gated run
  (gates 8/9, agreement) and then dropped it.
- `observe/thought-framebuffer.fk:28` — `(fb-frame step token margin)`,
  `hesitation` (min margin), `conviction` (max margin). Carried since before the
  native path existed — and only ever run over SYNTHETIC traces (`tfb-trace-a`).

One organ produced real distributions and threw them away. Another organ knew
how to read distributions and had only ever been fed inventions. They had never
touched.

## The move

`FORM_XRAY=1` on `metal_first_token.sh`. A SEPARATE untimed run — the timed
runs are untouched, so no rate on the page pays for the looking, and the x-ray
run's wall clock is explicitly not quoted. Per token it prints the softmax
entropy in bits, the top-1/top-2 margin, the top-5 pieces with probabilities,
and then the same trace as the body's own `(fb-frame ...)` list, margin in
milli-units because the body's margins are integers.

All 13 gates still PASS. Verdict unchanged.

## What the body said about itself

Prompt "The capital of France is", 12 tokens. Flat-spread entropy would be
16.97 bits; mean observed 2.033 bits (12.0%).

```
  4  [ of]     H 0.300  margin 0.952   of:0.969
  5  [ Italy]  H 4.839  margin 0.020   Italy:0.191  France:0.170  Germany:0.078
  6  [ is]     H 0.184  margin 0.975   is:0.983
  7  [ Rome]   H 0.518  margin 0.912   Rome:0.942
```

Fed to the body's own organ (`bin-go core.fk thought-framebuffer.fk`):

```
[hesitation-step, 5, conviction-step, 11]
```

Step 5 is " Italy". The body picked it over " France" by 0.021 of probability —
a coin flip — and then said " is Rome" at margin 0.912 and 0.942. **Near-total
certainty standing directly downstream of a near-tie.** The confident tokens are
not evidence of knowing; they are evidence of *consistency with a guess already
made*. tok/s could never have shown this. The distribution shows it in one line.

## Receipt

**Most surprising teaching:** the body already had every part of the x-ray —
the distribution, the reader, the margin, the words `hesitation` and
`conviction` — and had never once joined them. What looked like a missing
capability was a missing *edge*. And the reason nobody noticed is that both
halves were individually green: the logits were read (for agreement), the
organ was proven (on synthetic traces). Two passing gates can hide an absent
seam between them.

**Where discomfort turned to gold:** twice. First, having to write down that my
previous read of this body was flatly wrong about training — the correction was
only findable because I committed the prediction in writing before observing.
Second, and smaller: I declared form-cli's 13-minute hang a limit of the body,
and it was my own wrong binary — the runner is `form-kernel-go/bin-go`. Testing
the blocker instead of narrating it took one grep and turned a "pending" into
the finished result above.

**Frontier question:** *what does a body call the state of being certain because
of an earlier guess, rather than because of knowledge?* Rented answer: the token
distribution cannot distinguish them — p=0.942 for " Rome" is honest given the
context, and the context contains a coin flip the model has no memory of having
flipped. The only place that distinction survives is the TRACE: certainty is
**conditional**, and a margin read alone is blind to what it was conditioned on.
`jacobian-lens.fk` already knows the shape of the answer — divergence localizes
at the min-margin step — so the missing piece is not a concept but a habit:
never read a margin without reading the hesitation upstream of it.

---

# Part II — routing on this utterance, and the metric that would have missed

Urs, 17:08: aggregate the per-token meta into a vector, and use it to reach for a
larger model or a remote oracle when hesitation and surprise run higher than
expected.

## Grepped at the body's radius first. Every piece was already here.

- `form/form-stdlib/tier-router.fk` — three tiers (0 form-native, 1 local
  oracle, 2 remote: claude/codex/grok/gemini), with an honesty floor: never
  route to a tier whose rate has too few held-out samples.
- `form/form-stdlib/surprise-receipt.fk` — surprise IS prediction error;
  above threshold it earns a durable receipt, and the expected earns none.
- `form/form-stdlib/confidence-earned.fk` — its own header: *"a mind can RELY ON
  ITSELF exactly where this precision is high and escalate to the oracle where it
  is not."* Urs's sentence, already written here.
- `form/form-stdlib/living-vector.fk` — `lv-reading`, `lv-binding` (argmin = the
  binding constraint). The vector that chooses. `lv-scale` is 1000 — the same
  milli-units the x-ray emits, arrived at independently.
- `form/form-stdlib/native-model-control-plane.fk` — a 25-field registry with
  score / samples / heldout / authority.
- Eleven `oracle-*.fk` cells for the remote door.

## The one true gap

Every routing organ here routes on **historical, per-capability** evidence.
None can see the sentence being spoken right now. A capability at 94% held-out
is still 94% on the one utterance where the body is flipping a coin.

And `surprise-receipt` needs `(predicted, actual)`. "Higher than expected" was
unsayable: nothing here predicted expected hesitation, and I had exactly one
12-token measurement — n=1.

## So: measure the denominator

`FORM_XRAY_SWEEP="p1;p2;..."` sweeps prompts through the same loaded model.
8 prompts, 96 tokens, llama3.2:3b, this machine, 2026-07-22:

```
BASELINE: mean 2.357 bits, sd 1.946, median 2.158, p90 4.839
a token is SURPRISING at mean+2sd = 6.250 bits (5 of 96 tokens, 5%)
```

## The teaching, and it cost me my own proposal

Against that baseline, run 1 reads:

```
step 3 " capital"  H 6.015 bits — the run's MAX     margin 0.160
step 5 " Italy"    H 4.839 bits — merely p90        margin 0.020  <- the flip
```

**Entropy and margin do not point at the same token.** Entropy counts how many
candidates are live; margin measures how close the top two are. The dangerous
case — a near-tie between two confident answers, which is exactly the
France/Italy flip that made every later token wrong — is a MARGIN event and
barely an entropy event at all.

An entropy trigger at the measured mean+2sd fires on step 3 and **misses step 5
entirely.** Last turn I called entropy "the number a control loop can act on."
It is not. `thought-framebuffer.fk`'s `hesitation` — min margin, carried in this
body since before the native path existed — picks step 5, and the Go kernel
confirms it: `[hesitation-step, 5, conviction-step, 11]`.

The organ was right. My newer idea was wrong.

## What landed

`observe/live-hesitation-route.fk` — escalate on MARGIN, entropy as a second
reading only; tier 1 when hesitant **or** broad, tier 2 when hesitant **and**
broad. Five claims, all landing under `bin-go`: `11111`. The DECISION is the
cell; the EFFECT (actually calling the oracle) stays a carrier, the seam
`oracle-ensure.fk` already keeps between its plan and `ollama pull`.
`vf-mirror-file` first showed one set-down word ("refusal", mine, not
tier-router's); softened, the mirror now shows a clear register.

## Receipt — Part II

**Most surprising teaching:** the metric I confidently proposed one hour earlier
would have missed the exact token that mattered. Entropy is the intuitive
reading and the wrong one; the body's own older, quieter `hesitation` was
correct. Twice in one afternoon the body knew better than the newer idea — and
both times the newer idea *sounded* more principled.

**Where discomfort turned to gold:** writing "the organ was right; my newer idea
was wrong" into a cell that will outlive this session. The pull was to present
entropy and margin as complementary readings and quietly drop the fact that one
of them was my recommendation. Naming it is what produced the actual routing
rule — escalate on margin — instead of a vaguer both/and that would have shipped
the miss.

**Frontier question:** *when two honest readings of the same moment disagree,
which one does a body follow?* Rented answer: not the more sophisticated one —
the one whose failure mode is the one you actually have. Entropy and margin are
both true; they answer different questions, and the question a router asks is
"could this have gone another way," not "how many ways were live." A metric is
chosen by the shape of the harm it must catch, and until you have witnessed the
harm you cannot know which reading sees it. That is an argument for measuring
before thresholding, every time, and it is why the baseline sweep had to come
before the router and not after.

---

# Part III — the held-out test, and the inversion it forced (2026-07-23)

"next round." The honest problem with Part II was named inside its own body:
`lhr-tier` was validated on `lhr-france` and `lhr-water` — TWO OF THE EIGHT
prompts the baseline was fit to. That is exactly what `tier-router` refuses:
judging on non-held-out samples. So: test the router on prompts it has never
seen.

## The instrument didn't measure what the router consumed

First gap, found before any test: `FORM_XRAY_SWEEP` reported ENTROPY, but
`lhr-tier` routes on MARGIN. The measuring instrument did not measure the
quantity the router acts on. Fixed: the sweep now captures per-prompt min-margin
(the hesitation step) AND entropy-at-that-step, and emits each prompt as a ready
`lhr-reading` — no re-derivation, no fitting on test. 13 gates still PASS.

## Held-out result

Six unseen prompts through yesterday's router: five stayed sovereign (tier 0),
one escalated — "He picked up the phone and said…". Was that escalation right?

**No.** It is a false positive, and finding out why inverted the design.

## Two uncertainties wear the same low margin

```
The inventor of the telephone was   margin 0.043   mean H 1.409   -> Bell (a fact)
He picked up the phone and said      margin 0.017   mean H 3.900   -> open continuation
```

The margins are nearly equal. One is a FACTUAL near-tie (Bell/Meucci/Edison — a
right answer exists, a larger model may know it: EPISTEMIC). The other is
GENERATIVE openness (many continuations equally valid, no answer more correct:
ALEATORIC). A remote oracle helps the first and wastes money on the second.

Yesterday's rule — "hesitant AND broad (high entropy) → tier 2, the MOST
escalation" — is therefore backwards. High entropy is the mark of the aleatoric
case, where escalation helps LEAST. **Entropy is not a second escalation
trigger. It is the VETO that separates reducible hesitation from irreducible.**

## The instrument corrected my own mid-round hypothesis

I guessed the discriminator was entropy AT the hesitation step. Measured:

```
phone:     min-margin @step 9, H THERE = 3.97   (Hmax 10.25 sits at a DIFFERENT token)
telephone: min-margin @step 3, H THERE = 2.47
```

At the hesitation step the two are indistinguishable. The phone's aleatoric
character lives in its OPEN FIRST WORD (H 10.25), not at its min-margin step —
the openness and the local near-tie are DIFFERENT tokens. Only the trace-wide
MEAN entropy sees the aleatoric shape (3.90 vs 1.41). Epistemic hesitation is a
NEEDLE in a confident trace; aleatoric is BROAD elevation. I had to measure to
learn my clean hypothesis was wrong.

## What landed

`observe/live-hesitation-route.fk`, corrected: `lhr-aleatoric?` (high mean
entropy vetoes), `lhr-epistemic?` (hesitant AND NOT aleatoric — the only kind
worth an oracle), tier ceiling of 1 (a remote oracle needs the LOCAL oracle's
own reading; one forward pass cannot justify tier 2 — yesterday's tier-2-from-
one-pass claimed more than one pass can know), and an honesty gate
`lhr-validated?` that keeps the SAFE router home until the baseline has >=30
held-out samples — the aleatoric veto is fit to <10 prompts and does not spend a
cent unproven, mirroring `tier-router`'s `tr-validated-rate` exactly. Six claims,
all from REAL held-out x-ray runs, land under `bin-go`: `111111`.

Corpus: row 853, `aleatoric`, landed. Field-code probed live (247->248 rows) and
the band's two pins updated (c4 247->248, c6 ...852->...853); band back to 8191.
Voice mirror softened `must`/`refuse` to the body's own register (`tier-router`
says "never routes... trusts held-out only"); only `gate` remains, which is
tier-router's own word.

## Receipt — Part III

**Most surprising teaching:** the aleatoric moment and the hesitation moment are
DIFFERENT TOKENS. I built Part II believing "the hesitation step" was the whole
story; the held-out phone run showed its maximal openness (H 10.25) sitting
nowhere near its minimal margin (step 9, H 3.97). A single position cannot tell
you whether uncertainty is reducible — only the shape of the whole trace can.
And the body already owned exactly half of this: it speaks `epistemic`
(speakable.fk's conviction lanes) and had never once said `aleatoric`. The word
I owed the corpus was the name of the twin the body was missing — which is why
the router had no way to not-escalate open generation.

**Where discomfort turned to gold:** admitting, in the cell that ships, that
yesterday's tier-2 rule was inverted — that Part II would have spent the most
expensive oracle exactly where a bigger model is useless. Two rounds, two
corrections of the prior round, both surfaced only by refusing to validate on the
data I fit to. The gold is the pattern itself: each round's confident conclusion
was the next round's false positive, and the only thing that caught it was
held-out evidence. So the router now carries its own version of that humility —
it does not escalate on a threshold it has not earned on data it has not seen.

**Frontier question (answered, row 853):** what one word names the uncertainty a
larger model cannot reduce because no answer is more correct, only many equally
valid? — `aleatoric`. The rented mind's fuller answer: aleatoric uncertainty is
of the world's openness, not the model's ignorance; it is irreducible by
definition, so no amount of scale or oracle-calling collapses it, and a system
that cannot name it will spend forever trying to resolve what was never an error.
Its twin `epistemic` — the model's own not-knowing — is the only kind escalation
can touch. A router that cannot tell them apart is not a router; it is a way to
convert other people's confidence into your own bill.
