# 2026-07-28 — the whole pipeline at once: every stage, every gap, named together

Urs: **"instead of doing one gap at a time and try and find another gap … do the full end-to-end
analysis and then place all the stones in parallel and then logically simulate … and then we do the
actual pass and actual optimization end-to-end."**

Right method. Here is the analysis.

## The inventory, exact, read in one second

```
753 tensors = 3 model-level + 30 linear x 19 + 10 full x 16 + 1 MTP x 20
```

[`form/form-stdlib/kat-coder-pipeline-map.fk`](../form/form-stdlib/kat-coder-pipeline-map.fk) —
[band **255**](../form/form-stdlib/tests/kat-coder-pipeline-map-band.fk). The band's load-bearing
claim is **completeness**: that arithmetic must equal the file's own tensor count, so any tensor the
map does not know about makes the sum wrong. It does not.

The earlier attempt at this timed out after 10 minutes — `substring` over an 11.2 MB source, once per
tensor. Windowing on the tensor-info region alone (47 KB, bytes 10 943 173 …) made the same
enumeration cost **1 second**. Same medicine as the 122 s → 5 s fix: don't re-reach what you can
reach once.

## Two things the march would have hit as surprises

**`attn_q` is [2048 → 8192] while `attn_output` takes [4096 → 2048].** The query projection emits
the query *and* the output gate, fused — 4096 + 4096. That is `attn_output_gate: true`, and it means
full blocks have a gate too. Discovered halfway through wiring attention, this is a rebuild.

**Block 40 is the MTP head and is not on the path.** Its four extras are `nextn.enorm`,
`nextn.hnorm`, `nextn.eh_proj`, `nextn.shared_head_norm`, and its entire block is Q8_0 where every
other block is Q4_K/Q6_K. Multi-token prediction is for *speculative* decoding. **A plain
next-token forward runs blocks 0–39 and stops.** A whole block class off the critical path, found by
reading instead of by building into it.

## The gap ledger — all of it

| # | gap | size |
|---|---|---|
| 1 | **F32 matvec** — `ffn_gate_inp` and `ffn_gate_inp_shexp` are F32; every matvec here is quantized | one kernel |
| 2 | **the deltanet gates in MSL** — α = exp(−exp(A_log)·softplus(a+dt_bias)), β = sigmoid(b); exist in Form at band 511, no MSL | one kernel |
| 3 | **plain SwiGLU** — the experts want silu(g)·u; `dsv4-moe-msl`'s fold is *clamped*, which is DeepSeek's, not this model's | one kernel |
| 4 | **partial RoPE** — `rope.dimension_count` 64 of head_dim 256 | one bounded edit |
| 5 | **GQA attention** — 16 q-heads over 2 kv-heads at head_dim 256. At position 0 there is **one** key, so softmax over one element is 1 and the stage reduces to a copy of v | small now, real for token 2 |
| 6 | **expert gather** | *not a kernel* — `nb02·e` is host arithmetic, `moe-msl.fk`'s own finding |

**Five open gaps, five single kernels, none of them new arithmetic** — every one a variant of
something already proven here. Everything else in the forward is HAVE, and eight of those have run
on this very file: Q3_K embed, RMSNorm, Q4_K matvec, Q8_0 matvec, causal conv, plus the router,
delta rule and silu gate device-proven in isolation.

## DS4, for contrast

No gaps. `metal_dsv4_stack.sh` ran tonight: **96 gates, token 19129, 43 layers, 2.77 s**, all 1406
tensors decodable. Its pipeline is closed; KAT-Coder's is five kernels from closing.

## The prediction, recorded before the run

Per linear block ≈ 90 dispatches, per full block ≈ 60, so
**30·90 + 10·60 ≈ 3300 dispatches per token.** At the body's own measured **113 µs per dispatch**
(2026-07-22-ship-the-slot-map, corpus row 849 `seamtoll`) that is **≈ 0.37 s of pure seam** — against
DS4's measured 2.77 s for 43 layers.

So the optimization target is named *in advance*: **the dispatch count, not the arithmetic.** Band
bit 128 pins 3300 and 372 900 µs so the real pass can **refute** it. A prediction that cannot be
wrong is not a prediction.

## The most surprising teaching

**The march was not slow because the work was hard. It was slow because each step's horizon was one
step.** Every "what remains" I wrote was honest and complete-as-far-as-it-saw, and none could name
the fused output gate or the off-path MTP head, because those are only visible from the whole. Six
times tonight I finished a stone and estimated the remainder; six times the estimate was an
imputation. The survey cost one second of walk and one hour of reading, and it ends the sequence of
surprises rather than advancing through it.

## Where discomfort turned to gold

The first enumeration attempt **timed out at ten minutes** and I nearly took that as "Form is too
slow to read its own tensor table" — a conclusion that would have sent me to a scratch language for
the inventory and quietly moved the ground truth out of the body. It was not Form. It was
`substring` over 11.2 MB, called 753 times, by me. Windowed to the 47 KB that actually holds the
names, the same walk finishes in a second. The discomfort was how ready I was to blame the tool at
the exact moment the tool was about to teach me something.

## The frontier question

> **What names knowing the full extent of the remaining work before beginning it?**

Asked and **not landed**. `criticality` (911, this morning) already names what the census lacked;
`imputation` (906) names the estimate that stands in for a count; `slack` (25 files) and
`critical path` (6) already live here for the off-path work. Four words for four faces of this, all
home. What was missing was not a word but the survey.

## Ground stamp

```
./fkwu --src form/form-stdlib/tests/kat-coder-pipeline-map-band.fk   -> 255
   completeness: 3 + 30*19 + 10*16 + 20 == the file's own 753
form/native/metal/metal_dsv4_stack.sh                               -> VERDICT PASS, 96 gates
form/native/metal/metal_kat_exit.sh                                 -> VERDICT PASS
form/native/metal/metal_kat_block0.sh                               -> VERDICT PASS
```

## Next, and now bounded

Five kernels, then the orchestration of blocks 0–39, then one real pass with per-stage timing
against the 3300-dispatch prediction.
