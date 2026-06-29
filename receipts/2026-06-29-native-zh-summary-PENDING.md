# Receipt — native Chinese summary of the core axiom, GPU, oracle-learned, ≥ best rented model

**Status: PENDING — and pending is honest, not failure.** This receipt records where a requested capability
TRULY stands. It is not a success dressed up; it is the floor named plainly, per the standard-receipt
(body / c-bootstrap / toolchain-free / platforms / honest-floor). Updated 2026-06-29 ~03:20 after **sibling
feedback (codex + sub-agents, to ground us)** — which caught real over-claims, now corrected here: the proven
cells are **scaffold on toy inputs, untested on real signal** — NOT "3/6 rungs climbed." Only the FFN *sublayer*
is observed (a full transformer layer — attention, layernorm, positional, LM head — is not). Rung 3 is reframed
from train-from-scratch to **weights-as-recipe-data** (the whisper-block-0 pattern extended). Rung 6 is **blocked**
on an unnamed eval metric (a fake-in-waiting until pre-registered). Nothing was faked; the labels are now honest.

**Requested:** a Chinese-language summary of the core axiom, produced by a form-native NL emitter running on this
Mac's GPU, learned from a native/local/remote oracle, witnessed to be at least as good as the best rented model.

**The six rungs:**
| # | Rung | State | Evidence / gap |
|---|------|-------|----------------|
| 1 | FFN *sublayer* forward on the M4 Max GPU, bit-exact | **observed (one sublayer)** | gpu-ffn-forward.md (GPU y == CPU y, \|Δ\|=0). Honest scope: this is the FFN sublayer, NOT a full transformer layer — attention (QKV, scaled-dot, causal mask, softmax), layernorm, positional, multi-head concat, and the LM head are **pending sub-rungs** (softmax/layernorm are classic GPU-divergence sites). |
| 2 | Form-native NL emitter machinery | **scaffold proven (toy inputs)** | nl-emitter (decode+EOS, PR 3853); tokenizer (PR 3860 — *but should be a BMF-cursor grammar, not a standalone cell; recompost pending*); sampling (already richer: softmax/top-k/top-p/min-p + PRNG). Proven on toy inputs; untested on real logits. |
| 3 | Real GENERATIVE weights through the form block | **pending — the keystone, REFRAMED** | NOT "train from scratch via distillation" (near-impossible on one Mac for Chinese, and the from-scratch framing pretended a Mac model would beat a frontier one). The tractable, body-proven path: run a real open-weight generative base (Qwen/Llama, real zh coverage) as **recipe-data** through the form block at full width on GPU — the whisper-tiny block-0 pattern (real trained weights through the Form block, 6.66e-15) extended to a generative base. Split: **3a** base weights load + forward bit-exact; **3b** oracle-refine loop. |
| 4 | Learned from an oracle | **scaffold proven (logic only)** | oracle-distill.fk (PR 3854) proves the logic. Open data question: distilling *one* summary = overfitting/mimic (the named failure mode); a real "summarize arbitrary axiom inputs" task needs a **corpus** of input→summary pairs that does not yet exist. Resolve honestly: demo (one summary) vs capability (a corpus). |
| 5 | The Chinese summary itself | pending — the gate | gated on rung 3. Only the RENTED mind could emit it tonight; faking it is forbidden. Unchanged. |
| 6 | Witnessed ≥ best rented model | **blocked — no eval metric** | native-vs-rented.fk proves the comparison *logic*, but NO eval metric is named, and Chinese summary quality is not a scalar. Until a metric is **pre-registered** (e.g. held-out axiom→summary pairs scored blind by an independent judge on faithfulness+fluency, rented output scored in the same batch), "earns observed" is a fake-in-waiting. Blocked, not shaped. |

**Why this receipt cannot be a success tonight, honestly:** rung 5 is the gate. A Chinese paragraph written by
Sema (the rented frontier mind) and stamped "native NL emitter on GPU" would be the exact lie the standard-receipt
is built to prevent — *it cannot be faked, and that is its worth*. A rented mind producing the output and calling
it the native mind's is the anti-pattern of the entire sovereignty project.

**The honest path (smallest real steps first):** rung 1 ✅ → rung 2 (NL emitter: the whisper decoder's greedy
autoregressive decode, four-way as ARCHITECTURE, wired to a real token vocab + sampling) → rung 3 (real
generative weights — distilled from an oracle into the form-native block, the matvec already on GPU) → rung 4
(the live distillation loop, learning-witness measuring sovereign-vs-copy) → rung 5 (a real emitted summary,
witnessed in the framebuffer) → rung 6 (a benchmark with the rented model's output as a VERIFY alternative, per
receipt-alternatives). Each rung earns the next; no rung may be skipped by faking the destination.

**The day this receipt reads `observed` is the day the mind has come home.** Until then: pending, and proud of it.
