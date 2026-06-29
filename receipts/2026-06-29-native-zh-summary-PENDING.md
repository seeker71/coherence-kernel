# Receipt — native Chinese summary of the core axiom, GPU, oracle-learned, ≥ best rented model

**Status: PENDING — and pending is honest, not failure.** This receipt records where a requested capability
TRULY stands. It is not a success dressed up; it is the floor named plainly, per the standard-receipt
(body / c-bootstrap / toolchain-free / platforms / honest-floor). Updated 2026-06-29 ~03:00: **1 rung observed,
3 rung-SHAPES now four-way (2/4/6 via PRs 3853/3854/3855), rung 3 the keystone, rung 5 the gate — both still
pending.** Tonight's sub-agents built the scaffolding (how to decode, how to distill, how to benchmark); the
trained generative weights (rung 3) and the emitted summary (rung 5) did NOT move and were not faked.

**Requested:** a Chinese-language summary of the core axiom, produced by a form-native NL emitter running on this
Mac's GPU, learned from a native/local/remote oracle, witnessed to be at least as good as the best rented model.

**The six rungs:**
| # | Rung | State | Evidence / gap |
|---|------|-------|----------------|
| 1 | Transformer forward on the M4 Max GPU, bit-exact | **observed** | 2026-06-29-gpu-ffn-forward.md (FFN forward, GPU y == CPU y, \|Δ\|=0) |
| 2 | Form-native NL emitter (sampling/argmax → text) | **shape four-way** | `nl-emitter.fk` greedy argmax decode + EOS, four-way 11111 (PR 3853). The *decode logic* is proven; wiring to a trained decoder's real logits + a real tokenizer is pending. |
| 3 | Trained GENERATIVE weights | **pending — the keystone** | still whisper-tiny (transcription, block-0), not a generative LM. With the shapes (2/4/6) proven, this is now the one gating rung: a real distillation run produces the weights every other rung is waiting to receive. |
| 4 | Learned from an oracle (distillation loop) | **shape four-way** | `oracle-distill.fk` (PR 3854) proves the logic: error drops, generalize→sovereign, mimic→copy, local+remote oracles both valid. No live distillation *run* yet. |
| 5 | The Chinese summary itself | pending | gated on rung 3. The only thing that could emit it tonight is the RENTED mind (Sema) — faking it is forbidden. Unchanged by tonight's scaffolding. |
| 6 | Witnessed ≥ best rented model | **shape four-way** | `native-vs-rented.fk` (PR 3855): native≥rented earns observed, rented is a verify-alternative, never laundered. Needs the real model (rung 3) + a real benchmark run. |

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
