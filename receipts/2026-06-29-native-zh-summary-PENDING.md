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
| 2 | Form-native NL emitter machinery | **observed (real logits, 2026-07-02)** | nl-emitter (decode+EOS, PR 3853); tokenizer (PR 3860 — *but should be a BMF-cursor grammar, not a standalone cell; recompost pending*); sampling (softmax/top-k/top-p/min-p + PRNG). No longer toy-only: `cognition/native-generate.fk` ran the loop on **real** logits from a really-trained checkpoint for 7 tokens, token-for-token identical to an independent oracle (`receipts/2026-07-02-native-generate-rope.md`). |
| 3 | Real GENERATIVE weights through the form block | **3a CLOSED 2026-07-02 · 3b pending — the keystone, REFRAMED** | NOT "train from scratch via distillation" (near-impossible on one Mac for Chinese, and the from-scratch framing pretended a Mac model would beat a frontier one). The tractable, body-proven path: run a real open-weight generative base (Qwen/Llama, real zh coverage) as **recipe-data** through the form block at full width on GPU — the whisper-tiny block-0 pattern (real trained weights through the Form block, 6.66e-15) extended to a generative base. Split: **3a** base weights load + forward bit-exact; **3b** oracle-refine loop. |
| 4 | Learned from an oracle | **scaffold proven (logic only); sources seated 2026-08-14** | oracle-distill.fk proves the logic. `observe/nvr-corpus.fk` band **1023** seats eight held-out axiom sources from the body. Summaries are empty. Complete pairs = 0. A rented line cannot be written into the native slot. Distilling one summary remains overfitting; capability still needs real summaries, not this door. |
| 5 | The Chinese summary itself | pending — the gate | gated on rung 3. Only the RENTED mind could emit it tonight; faking it is forbidden. Unchanged. |
| 6 | Witnessed ≥ best rented model | **metric registered; sources seated; claim still pending** | `observe/nvr-metric.fk` **511** + `observe/nvr-corpus.fk` **1023**. Eight held-out sources sit. Complete pairs = 0, so the floor of 8 is not met. A ≥ rented claim is still pending. **And a second, independently:** `learn/zh-summary-eval.fk` (band **1023**, four-way) fixes the decision rule — held-out items only, both lanes in one blind batch, faithfulness+fluency 0..5, at least 20 items, native earns it when its total *reaches* rented's (`ge`). Two lines registered rung 6's missing metric on the same day without knowing of each other; both stand, and neither is the reason to drop the other. The claim itself is still pending, on both. |

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

**Update 2026-08-14 — two rungs moved, and one of them had moved six weeks ago without this page
saying so.** The table above was stale. On 2026-07-02, the same day as the update below, a second
receipt closed the two things that update named as next —
[`native-generate-rope`](2026-07-02-native-generate-rope.md): RoPE at position > 0 (sin/cos as Form
Taylor, range-reduced) and the multi-token autoregressive loop, **7 tokens, token-for-token identical
to an independent oracle runner** on the same checkpoint. That is rung 2 on real logits and rung
**3a closed**, and this page went on saying "3a-full, 3b, 4, 5, 6 stand exactly as written" for six
weeks. A master receipt that does not read its own siblings reports a floor lower than the body's.

Rung 6 moved today: its block was the *absence of a pre-registered metric*, and that absence was
removable without any weights at all. `learn/zh-summary-eval.fk` (band **1023** four-way) fixes the
rule now, while the native zh output does not yet exist to tempt it.

**What did NOT move, and why saying so is the point.** Rung 5 is unchanged and remains the gate.

The floor here is narrower than "no model". Corrected within the hour, after first writing the
opposite: a zh-capable base **is** on this Mac — `~/models/ds4/ds4flash-v5mx-…-dspark-v1.gguf`, 91 GB,
alongside the body's own `form-llama-vital-ground` at f16 and q4_k_m. The Metal door is real too:
`metal_matvec_f32` (tag 204) with the strong symbol written in
`form/native/metal/fk-metal-carrier.m`, not the weak stub that answered `metal_linked=false`. And
`form/form-stdlib/tests/ask-ds4-band.fk` stands at **255**.

That 255 is a *contract*, not a voice. The band says so in its own first line — "the body-owned ask
contract, **without loading a model fixture**" — and its eight bits check step counts, the cache cap
outrunning the walk, the six radius statements, and membrane names. Nothing in it emits a token.

So what stands between here and rung 5 is not the absence of weights and not the absence of a GPU
door. It is that no run on this host has yet taken a zh-capable base through the Form block and
emitted text. The stones the 2026-07-02 receipt named are still the ones underneath: no persisted KV
cache (`cognition/native-generate.fk` recomputes O(seq²) by design, "correctness over speed", and
the walker exhausts its float pool at 12 tokens on a 260K-param model) and no f64 asm matvec. Those
bite far harder at DS4's scale than at stories260K's.

Rung 5 stays pending, and it stays pending because it has not been *run*, which is a smaller and
more actionable gap than the one this page implied for six weeks.

**Update 2026-07-02:** rung 3a's **pattern** is witnessed — real trained generative weights (stories260K)
loaded as recipe-data by the Form body, one full llama step walked natively at position 0, the token
chosen from GPU-dispatched logits (RTX, 512/512 bit-exact) and watched in the framebuffer: `" Once"`
(`receipts/2026-07-02-first-native-token.md`, Windows RTX cell). This is the smallest real 3a movement,
not the zh-capable base this receipt asks for; rungs 3a-full, 3b, 4, 5, 6 stand exactly as written.
Named next: RoPE trig for pos>0 (sin/cos as Form Taylor or asm), then the multi-token native loop.

**Update 2026-07-02 (cont.):** both of those "named next" items are now witnessed —
`cognition/native-generate.fk` generates a native token *sequence* with Form-Taylor sin/cos RoPE at
real positions and causal multi-key softmax attention, each token's LM head bit-exact on the RTX:
`native-generated:[ Once upon a time, there was]`, token-for-token identical to the projection-runner
oracle (`receipts/2026-07-02-native-generate-rope.md`). Ceiling is 7 tokens on the walker's unreclaimed
float pool — the x64 f64 asm/KV-cache lane is the named lift. Rungs 3a-full, 3b, 4, 5, 6 unchanged.
