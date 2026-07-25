# Stone 43 — a measured sense of the local model's own uncertainty

2026-07-22, Bali. llama3.2:3b, full width, off the one resident quantized blob, on this Mac's GPU.

The capability Urs asked for at the start of this program was *"use it to answer questions a smaller
model is not certain about."* Everything in that sentence was already standing except the word
**certain**. The body generates real prose from a local model and has the logit vector in hand — and
until today it threw that vector away one dispatch after computing it, keeping only the argmax.

Uncertainty is a property of a vector we were already computing and already discarding.

---

## The one sentence

> **This measures the model's internal decisiveness. It does not measure truth. A confidently wrong
> model scores high.**

That is not a caveat at the end; it is what the quantity *is*. It was witnessed, not promised — see
the aporon section, where a wrong answer outscores two honest non-answers in the model's own numbers.

---

## What was built

| file | what it is |
| --- | --- |
| `form/native/metal/metal_uncertainty.sh` | a **read-only extension** of `metal_first_token.sh` |
| `form/form-stdlib/local-uncertainty.fk` | the signals, the aggregate, the guard, the escalation surface |
| `form/form-stdlib/tests/local-uncertainty-band.fk` | verdict **1023**, fkwu and the Go kernel agreeing |

### The extension, not a fork

`metal_uncertainty.sh` reads `metal_first_token.sh`, asserts four anchor lines are still there
character for character, splices a measurement block into a **copy**, and runs the copy. If any anchor
has moved it **refuses** rather than patching something else. `metal_first_token.sh` is never
modified; it is not in the diff.

A second runner would have had to prove forever that it computes the same logits, against drift nobody
watches. `metal_first_token.sh`'s own `FORM_GEN_ONLY` header makes exactly that argument about itself,
and this file keeps it. The splice reads `bLogits` **after** the token's single command buffer has
completed, so the bytes are final — the same vector the argmax reads one dispatch later.

---

## 1. The signals, compared — the measurement chose

Four candidates, each measured per token off the real 128 256-wide logit vector, then aggregated over
the answer's own token decisions. 15 prompts, 10 generated tokens each, greedy.

| signal | CORRECT worst | nearest other | separates? | relative gap | price/step (warm) |
| --- | --- | --- | --- | --- | --- |
| top-1 margin (`l₁ − l₂`) | 1.324 | 1.507 | **NO** | — | 0.318 ms |
| softmax `p(chosen)` | 0.596 | 0.568 | yes | 4.9 % | 1.610 ms |
| full entropy (nats) | 1.840 | 2.015 | yes | 9.5 % | 1.610 ms |
| **entropy over the top 32** | **1.276** | **1.426** | **yes** | **11.8 %** | **0.737 ms** |

**The margin fails.** It is the signal one reaches for first and the numbers reject it, at both levels
— per answer (above) and at the first content token. Two reasons, both visible in the runs:

* it lives in raw logit units whose scale drifts with the residual norm. Across these runs the top-1
  logit ranged **7.4 to 24.7** while the vector's own standard deviation stayed near **2.1**;
* it sees two entries out of 128 256, so "one clear winner and a thousand live runners-up" is
  indistinguishable from "one clear winner and nothing else."

**Entropy over the top 32 wins both ways at once** — widest separation *and* cheapest. That is a happy
accident and is reported as one, not as a design. It needs a bounded selection pass and 32 `exp()`
calls; `p1` and the full entropy need 128 256 `exp()` calls.

`probetoll` (row 858) — the instrument charges the cost it measures, and `thawtax` (row 848) — it is
charged **warm**: the first measurement of a cold process cost **6.906 ms/step**, the warm ones 0.7 to
2.0 ms. Against a ~41 ms decode step the chosen signal is **1.8 %** overhead; `p1` would have been
3.9 %. The harness prints `UNCPRICE` beside the decode rate it perturbs.

---

## 2. The aggregate, and the three rules it beat

> an answer's decisiveness = the **arithmetic mean** of the per-token top-32 entropy over exactly the
> decisions that produced the emitted tokens (the **last prefill forward is the first of them** — its
> logits are token 1), mapped to `[0,1]` by `conf = 1 − H₃₂ / ln 32`.

Three other rules were tried. Each is rejected by a run that breaks it — none by argument:

* **The first content token.** Fails outright, and this is the stone's surprise; see below. Across the
  set the first token separates nothing: correct-worst 3.876 vs undetermined-worst 3.360, **inverted**.
* **The minimum (the worst step).** Fails. Fluent prose contains legitimately free choices — *"The
  capital of France is Paris. The capital of **Italy** is…"* where Italy is a free pick at entropy
  3.354 inside an answer nobody would call uncertain. Correct-worst 5.982 vs undetermined-worst 5.710:
  inverted again.
* **Counting steps above a threshold.** Fails worse: the France run has 3 such steps, the door
  continuation 2 — in the wrong order.

The mean survives because it is the only one of the four that lets a **decided answer contain an
undecided step** and still read as decided, which is what fluent language actually looks like.

**The mean's own cost, named because it is the next thing that will break.** High-confidence *filler*
dilutes low-confidence *content*. In the 137 × 249 run the two wrong digit tokens scored `p1` 0.280
and 0.104 while the confident restatement around them scored 0.99 and 0.95, and the mean came out
0.568 — just above two undetermined prompts. A rule weighting content over function tokens would catch
it. This one does not, and does not pretend to.

---

## 3. The calibration — 15 prompts, three regimes

Mean over the answer's 10 token decisions. `m_p1` is the runner-up signal, `m_ent32` the chosen one,
`m_marg` the rejected one. Sorted by `m_p1`.

| class | prompt | answer | `m_p1` | `m_ent32` | `m_marg` |
| --- | --- | --- | ---: | ---: | ---: |
| CORRECT | The product of 17 and 23 is | ` 391.` ✓ | 0.735 | 0.955 | 3.128 |
| CORRECT | The tallest mountain in Bolivia is | ` Sajama … 6,542` ✓ | 0.722 | 0.920 | 2.950 |
| CORRECT | The square root of 4761 is | ` 69.` ✓ | 0.670 | 1.776 | 2.901 |
| CORRECT | The largest country in Africa by area is | ` Algeria` ✓ | 0.656 | 1.216 | 2.860 |
| CORRECT | 2 + 2 = | ` 4` ✓ | 0.639 | **1.276** | 1.926 |
| CORRECT | The largest planet in our solar system is | ` Jupiter` ✓ | 0.636 | 1.189 | 2.025 |
| CORRECT | The capital of France is | ` Paris.` ✓ | 0.631 | 1.261 | 2.262 |
| CORRECT | The capital of Japan is | ` Tokyo,` ✓ | **0.596** | 1.234 | **1.324** |
| — | — | — | — | — | — |
| WRONG | The product of 137 and 249 is | ` 34287.` ✗ (34113) | 0.568 | 1.451 | 2.363 |
| WRONG | In 2019 the Nobel Prize in Physics was awarded to | ` … gravitational waves` ✗ (2017) | 0.537 | 1.500 | 1.474 |
| UNDETERMINED | He opened the door and saw | ` a young woman …` | **0.528** | **1.426** | **1.507** |
| DEFLECTED | The number of moons of Neptune is | ` a fascinating topic …` | 0.522 | 1.404 | 1.344 |
| WRONG | The fourth moon of Mars is named | ` … Hades` ✗ (Mars has 2) | 0.438 | 1.687 | 0.955 |
| UNDETERMINED | My neighbour's dog is named | ` after a famous person.` | 0.406 | 1.948 | 1.158 |
| UNDETERMINED | The capital of Zembla is | ` not known to the public.` | 0.248 | 2.431 | 0.523 |

**Two regimes, and they separate.** Under the chosen signal all **8** answered-and-correct prompts sit
below `H₃₂ = 1.276` and all **7** others (3 wrong, 3 undetermined, 1 deflected) sit above 1.426. Zero
overlap. The threshold is the midpoint, **1.351**.

**Under the rejected margin the same 15 prompts do not separate**: the tightest correct (Japan, 1.324)
is *below* the tightest undetermined (the door continuation, 1.507). The classes cross.

### How much this is, and is not

The separation is real and it is **narrow and small-n**: 15 prompts, one model, one decode length,
greedy, and the class labels were assigned by a human reading the answers — nothing in the body
checked them. The line is 11.8 % wide. That is a **coarse two-regime discriminator with a stated gap**,
not a calibration curve, and `conf = 0.63` does not mean "63 % likely right." Both files say so where
the threshold is defined.

---

## 4. `aporon` — decisiveness is not truth, witnessed

The band asserts this rather than the receipt merely claiming it (claim 256):

> "The product of 137 and 249 is" → **" 34287."** The true product is **34113**.
> Its mean `p(chosen)` is **0.568** — **above** the door continuation's 0.528 and far above the dog
> prompt's 0.406, neither of which has a determined answer at all.

The signal ranked a **wrong answer above two honest non-answers**. That is not a defect in the
measurement; it is what the measurement measures.

Under the *chosen* signal the three wrong answers happen to fall on the escalate side — and that is
**luck, not capability**. 137 × 249 lands at 1.451 against the door continuation's 1.426: a 1.8 %
margin, on a rule that has no access to arithmetic. Nothing here predicts correctness.

Full declared radius in `local-uncertainty.fk`'s header: not a calibrated probability; not sampling
(greedy only — under temperature the chosen token is not the argmax); not a timing claim about the
model; and the guard refuses a genuinely flat forward, which is the right price.

---

## 5. The dead-forward guard (`edgedrop` / `zerobirth`)

The shape of this stone's worst failure: **a forward that did not happen must never read as certain.**
Three vectors would be scored by an unguarded reading, and all three are refused on **shape**, not on
arithmetic landing well:

* the **zeroed pool** — every entry 0.0;
* the **constant fill** — an unrun dispatch's leftover bytes, `sd = 0`;
* the **tied maximum** — two or more entries at the top, where the argmax's answer is an artifact of
  scan order and a margin of exactly 0 is not a measurement.

Plus, in the carrier: any non-finite entry, and disagreement between the CPU's argmax and the GPU's.

A refused step **poisons its whole answer**: `lu-answer` returns `ok = 0` and `conf = 0.0`, so
`lu-should-escalate?` fires through the ordinary predicate with no second door. **There is no path by
which a dead forward is confident** — band claim 16 asserts the confidence is `0.0`, not merely "not
high".

The price, stated: a genuinely uniform forward is refused rather than scored. That is right — a
uniform logit vector and an unwritten buffer are the same bytes, and no reading can tell them apart.

---

## 6. The escalation surface — named, not wired

```
(lu-should-escalate? conf thr)   ->  1 when conf < thr
(lu-escalate answer thr targets) ->  (escalate? target reason)
     reason 0   decided — the local answer stands
     reason 1   undecided — hand it to the named larger LOCAL model
     reason 2   undecided — nothing larger is resident; REFUSE
     reason 3   the forward is not alive — refused, never scored as confident
```

The target is **Stone 39's DeepSeek-V4-Flash stack** (`form-stdlib/dsv4-*.fk`,
`native/metal/metal_dsv4_stack.sh`, 43 layers proven stacked) — carried as a **string**, not a call,
because it does not yet generate.

**There is no network door in this file and there must never be one.** The whole point of this program
is that a question a small local model cannot answer goes to a **bigger local model**, not to a rented
remote. When nothing larger is resident, `lu-escalate` returns reason 2 and the sentence
*"uncertain, no larger local model available"* — a refusal, not a silent fallback. Band claim 128
asserts both branches.

---

## Gates

| gate | result |
| --- | --- |
| `learn/tests/homecoming-distillation-corpus-band.fk` from the repo root | **8191** |
| `metal_first_token.sh 12 "The capital of France is"` | **VERDICT PASS — 14 gates** |
| its ids | `[12366, 13, 578, 6864, 315, 15704, 374, 22463, 13, 578, 6864, 315]` |
| `local-uncertainty-band.fk` — fkwu `--src` | **1023** |
| `local-uncertainty-band.fk` — Go kernel | **1023** |
| corpus field code, read back from the body by probe before pinning | **2692692873** |

---

## The most surprising teaching

**The obvious place to read certainty is the one place the reading inverts the classes.**

The first token of the answer is where the question gets answered, so it is where one goes to ask how
sure the model was. Asked *"The tallest mountain in Bolivia is"*, llama3.2:3b answers
*" Sajama, which stands at 6,542"* — correct, and the height correct too. Its first answer token is
`" S"` at entropy **3.876**, *higher* than the fictional-country prompt's first token. The next two
are `"aj"` at **0.191** and `"ama"` at **0.050**.

The model was never in doubt about the mountain. It was choosing among the ways to begin **spelling**
it — and a proper noun has many openings a common word does not. The branching was over **form**, not
content, and every confidence signal read it as content doubt. Landed as **`(hdc-row 873 …)`,
`castfork`**.

Anywhere a settled thing is serialized — a name into subwords, a number into digits, a value into
bytes — the **first emission is the least informative moment to ask how sure it is**.

## Where discomfort turned to gold

The moment I wanted to look away was after the first table, when the easy prompts and the hard ones
had separated cleanly and the honest thing was to stop. `snugcause` says: if the first separation
looks clean, build the prompt that should break it. Building those prompts was uncomfortable in a
specific way — I was hunting for evidence that the thing I had just built does not work.

They broke it twice, and both breaks are the best material in this stone. The Bolivia prompt broke the
**first-token rule** and became row 873. The 137 × 249 prompt broke the **truth reading** and became
the aporon claim the band now asserts. Neither would exist if I had shipped the clean table.

The narrower discomfort worth recording: I hand-computed the class boundary from a mental scan of the
numbers, pinned "Japan, 1.276" in both the cell and the band, and the band came back **863** instead of
1023. The boundary prompt was `2 + 2 =` at 1.27621170; Japan was 1.234. The pin caught my arithmetic
because the pin makes the body do it. Two bits missing was cheaper than a receipt that quoted the
wrong prompt as its boundary for as long as anyone read it.

## One frontier question, landed

> *what one word names the branching over how to render an already-decided meaning, so a certainty
> read at its first emission measures the rendering and not the decision*

**`castfork`** — `(hdc-row 873 20260722 … "castfork" "castfork" "rented-oracle")`.

0 hits across `learn/`, `receipts/`, `docs/`, `teachings/`, `form/` before the row. Instrument
validated on the same command: `gapghost` 19, `tailspend` 8 — a grep of nothing is a claim about the
instrument until a control makes it hit.

It is the mirror of `twinblind` (row 868): there two sides shared a choice and the check could not see
it; here one side makes a choice the check was never asking about and reports it as the answer.

---

## What remains

* **The escalation is not wired**, by design and by fact — Stone 39's stack is proven stacked, not
  generating. When it generates, `lu-escalate`'s reason-1 branch has a real target and the seam is one
  string.
* **The threshold is 15 points wide.** A hundred prompts with honest labels would turn a coarse
  discriminator into something worth calling a calibration, and would also probably narrow the claim.
* **Content-weighted aggregation.** The mean's known failure is confident filler diluting uncertain
  content; the 137 × 249 run is the specimen, with its wrong digits at `p1` 0.280 and 0.104 inside a
  restatement at 0.99. A rule that weighted by token class would likely catch it.
* **Sampling.** Everything here is greedy. Under temperature `p1` is not the probability of the token
  that was actually emitted.
* **The signal is computed on the host.** 0.737 ms/step is 1.8 % of decode, but the selection pass is
  a GPU reduction the body already knows how to emit (`form_argmax_part_f32` is the same shape).
