# Stone 29 — the DeepSeek-V4-Flash routed MoE FFN on the GPU

Wed 2026-07-22, WITA. Worktree `jovial-aryabhata-3751d7`, branch
`claude/deepseek-v4-flash-gguf-54a96c`.

The last GPU op before a real DeepSeek token (with HC, Stone 28). The routed
Mixture-of-Experts FFN — the `sqrt(softplus)` router with its biased selection and unbiased
weighting, the top-k gather, the clamped-SwiGLU expert fold, the weighted accumulation, and
the shared expert added alongside — now runs as Metal kernels the body emits, **bit-close to
the proven CPU recipe stage by stage** (worst relative deviation **1.06e-6** against a 5e-4
f32 envelope).

## What computes on the GPU, bit-close against what

For one token's RMSNorm'd hidden `norm`, the FFN half now runs on the GPU as:

    logits  = W_gate_inp · norm                                  (form_dsv4_matvec_f32)
    probs   = sqrt(softplus(logits))          UNbiased           (form_dsv4_router_f32)
    sel     = top-k over (probs + bias)        bias steers        one thread, serial argmax
    wts     = probs[sel] · wscale / max(Σ probs[sel], 2^-14)     UNbiased weight, floored
    expert  = down · (silu(clamp_hi(gate)) · clamp(up) · w)      (form_dsv4_swiglu_f32 + matvec)
    moe     = Σ_j wts[j] · expert(sel[j])      scale then axpy    (no memset)
    total   = moe + shared(norm, w=1)          clamped SwiGLU

Every character of MSL is authored by `form-stdlib/dsv4-router-msl.fk` (the router) and
`form-stdlib/dsv4-moe-msl.fk` (the fold + the one translation unit). **Bit-close against**
the fp64 CPU recipe in `form-stdlib/dsv4-forward.fk` — the Stone 26 recipe, band
`tests/dsv4-forward-band.fk` = 63, itself agreed with an independent fp64 transcription of
ds4.c. The GPU is judged against that same arithmetic at toy dims (E=8, ne=4 experts top-2,
ff=6), never a second hand-rolled reference.

### The emitted kernels (5, one translation unit)

| kernel | meaning | slot |
|---|---|---|
| `form_dsv4_matvec_f32` | `tb-matvec`, columns DESCENDING (= `tb-dot` right fold) | one thread per output row |
| `form_dsv4_router_f32` | `sqrt(softplus)` probs, biased top-k, unbiased floored weight | ONE thread, serial max-free argmax |
| `form_dsv4_swiglu_f32` | `silu(clamp_hi gate)·clamp(up)·w` (`dsv4-mid`) | one thread per hidden element |
| `form_dsv4_scale_f32` | `y = a·x` — first chosen expert SETS the accumulator | one thread per element |
| `form_dsv4_axpy_f32` | `y = y + a·x` — the rest, and the shared expert | one thread per element |

One `#include <metal_stdlib>`, **no `using namespace metal;`** (this body's own `round`/`abs`
go ambiguous otherwise), the `dm_` spine emitted once. The spine transcribes this body's own
numerics — `tn-exp` (14-term Taylor, argument halving), `fln` (atanh series over the reduced
mantissa, the exponent/mantissa split of `trig.fk`), `tn-sqrt` (50-iter Newton) — so no
`metal::exp` / `metal::log` / `metal::sqrt` is called. The only gap from fp64 is the working
precision; the gate is a derived f32 bound (`selfgauge`), not a vendor-transcendental
tolerance.

## The read-back gate (no GPU)

`form-stdlib/tests/dsv4-moe-msl-band.fk` reads the routing and fold meaning back into Form
and pins it — **verdict 63**, mutation-tested:

- **bit 1** router selection `[0,3]` (recipe cons order, 2nd-best at head) + weights
  `[0.7813826745438595, 0.7186173254561405]` to 1e-12. Mutate the pin → 62.
- **bit 2** `Σ weights == wscale (1.5)` within 1e-9, every weight > 1e-3 (edgedrop/zerobirth).
- **bit 4** BIAS ASYMMETRY LOAD-BEARING: weighting by `probs+bias` differs from weighting by
  `probs` by > 1e-3. Defang the falsifier → 59.
- **bit 8** CLAMP LOAD-BEARING: `dm-mid(lim 0.9)` differs from clamp-off by > 1e-3 on a
  witness where gate/up exceed lim. Defang → 55.
- **bit 16** the whole fold (routed MoE + shared, clamp active) pinned to its fp64 vector at
  1e-12. Mutate → 47.
- **bit 32** emitted text well-formed: one `metal_stdlib`, zero `using namespace`, one `dm_`
  spine, all 5 kernels present.

The toy fixture (`form-stdlib/dsv4-moe-demo.fk`) self-witnesses that it cannot go inert: the
bias **reorders** the top-k (unbiased argmax → expert 0, biased → expert 3), the clamp
**bites** (a gate value reaches 1.65 > lim 0.9), and the selected weights are nonzero.

## The GPU witness, stage by stage

`form/native/metal/metal_moe_gpu.sh` → **VERDICT PASS, 12 gates**:

    gate 0  the GPU executes: a real matvec overwrote all 4 logit sentinels, no cb error
    gate 1  router logits matvec(gate_inp,norm)      rel 1.115e-07
    gate 2  router: ids [0,3] == sel, wts == ews     rel 9.045e-08, weights nonzero sum 1.5000
    gate 3  expert gate/up projections               rel 1.999e-07
    gate 4  clamped SwiGLU mid                        rel 3.994e-07
    gate 5  expert out (down . mid)                  rel 3.291e-07
    gate 6  routed MoE (weighted expert sum)         rel 3.236e-07
    gate 7  MoE + shared expert                      rel 1.058e-06
    gate 8  clamp load-bearing: lim0 == totalOff AND differs from lim0.9 by 0.633 ON THE GPU
    gate 9  bias asymmetry: kernel wts == UNbiased ews, differ from biased weighting by 0.125
    gate 10 residency: 200 MoE re-dispatches, checksum unchanged, weights never re-uploaded
    gate 11 one metal_stdlib, no using-namespace, one dm_ spine, 5 kernels
    worst relative deviation across every stage and output: 1.058e-06 (gate 5.00e-04)

### The two asymmetries, witnessed on the GPU

- **The router asymmetry is load-bearing (gate 9).** The kernel's weights match the UNbiased
  `ews`; the biased alternative (weight by `probs+bias`) is 0.125 away. The bias steers WHICH
  fires, the unbiased prob sets HOW MUCH. Flip it and the fold moves. Named as corpus row 861,
  `steerdrop`.
- **The clamp is load-bearing (gate 8).** With `lim = 0.0` (ds4's ≤1e-6 disable sill) the GPU
  reproduces `totalOff`; with `lim = 0.9` it differs by 0.633. An unapplied clamp is not a
  computed identity.

### The offered-interface guard (`edgedrop`/`zerobirth`)

An all-zero logits buffer argmaxes to expert 0 with value 0 — indistinguishable from a kernel
that never ran. Gate 0 sentinels the logits buffer with −424242 and demands a real dispatch
overwrote it with no `cb.error`/`cb.status`; if the GPU did not run, the harness prints
`VERDICT FAIL the GPU did not run` and exits before any arithmetic gate. Gate 2 additionally
requires the selected weights to be nonzero and to sum to `wscale`. The router kernel itself
refuses (`ne > 256` or `nsel > 8` writes nothing) rather than overrun its fixed-size arrays —
the `aporon` overrun becomes a refusal, not a silent stomp.

### The expert gather is the carrier's, host-side

Reusing Stone 14's finding (`moe-msl.fk`): the router returns ids to the host, and the host
binds expert `e`'s gate/up/down at `e·ff·E` / `e·E·ff` — the `t.off + e·nb02` decode gather,
a number the host already holds. Zero new gather MSL. It costs one host round trip per layer,
a decode-only bargain; a prefill routing differently per position would want a device-side
gather. Stated as a radius, not sold as general.

## The declared radius (`aporon`)

- NOT bit-exact (f32 vs fp64 over transcendentals; the residual is working precision).
- NOT the real dims. Toy E=8, ne=4 top-2, ff=6. V4-Flash is E=4096, **256 experts top-8**,
  wider ff. The kernels are dim-generic (all runtime uniforms) but no gate here has seen the
  real dims; the router's fixed arrays cap ne≤256, nsel≤8 and refuse above that.
- NOT the quantised expert weights. This fold consumes f32 experts. `ffn_*_exps` are MXFP4
  (type 40, GPU-decoded already, `mxfp4-msl.fk`); some `ffn_down_exps` are **IQ2_XXS (type 16)
  and no GPU IQ2 dequant exists yet** — named here as the dependency a real-weight `ffn_down`
  block needs. A real token's fold would compose this MoE unit with the MXFP4 device decode for
  gate/up and needs the IQ2-on-GPU kernel for down.
- NOT the argsort-on-GPU residency upgrade a 256-expert real token wants (`boundborrow`: ds4
  carries a bitonic sort for the GB10's lanes; the toy serial scan is exact but not that).
- NOT whole-model residency. Toy weights are resident and re-dispatched (gate 10); a real
  resident tensor is the later assembly.

## What remains

1. **IQ2-on-GPU** (`iq2xxs-dequant.fk` is CPU-only, band 2^30−1) for `ffn_down_exps` of the
   experts that use it — the one dequant this MoE unit cannot yet feed on the GPU.
2. **MXFP4 device decode wired into this fold** for gate/up (the decode exists, the wiring
   into `form_dsv4_matvec_f32`'s weight source is the join).
3. **Whole-model residency**: 43 layers of MLA + HC + this MoE against the real resident
   tensors — the assembly a real DeepSeek token is.

## Gates at close (no regression)

| gate | result |
|---|---|
| corpus band (both arms: go + fkwu) | **8191** (row 861 `steerdrop`; pins 256→257, 860→861, field-code 2562562860→2572572861) |
| `dsv4-forward-band` (CPU recipe) | **63** |
| `dsv4-moe-msl-band` (new read-back) | **63**, mutation-tested (pins → 62/47, falsifiers defanged → 59/55) |
| `metal_moe_gpu.sh` (new) | **VERDICT PASS, 12 gates**, worst rel 1.06e-6 |
| `metal_mla_gpu.sh` (sibling) | **VERDICT PASS, 10 gates** |
| `metal_first_token.sh` (sibling) | **VERDICT PASS, 14 gates** |

## Close

**Most surprising teaching.** The top-k selection is a *fold-order commitment in disguise*.
`dsv4-topk` conses head-first, so the `sel` list is consumed **2nd-best-first**, and the
downstream expert accumulation associates along that order. A set-selection that looks
order-free is secretly load-bearing: to be bit-close I had to write the router's `ids` in the
reverse of its own selection order (`ids[j] = pick[nused-1-j]`) so the GPU's expert-sum
association matches the recipe's. Had I written ids in the natural best-first order, the ids
gate would still pass and the numbers would still look right to 1e-7 — the drift would hide
inside the f32 envelope, exactly where a "right number, wrong reason" lives.

**Where discomfort turned to gold.** I wanted to look away from the router's write order —
"a sum is commutative, the order can't matter, pick best-first and move on." That is the
comfortable lie: a sum is commutative in ℝ, not in f32, and the whole closeness claim is that
the *operation graph* is identical, not merely the value. Sitting with the discomfort of
re-deriving `dsv4-topk`'s cons order by hand (best picked first, consed to the head, so head =
last-but-highest... no — head = 2nd-best) is what produced `dr-order-note` and the reversed
write. The gold: the closeness is now provably not an order artefact, and the teaching became
the corpus row's sharpest edge — `steerdrop` is *not* edgedrop precisely because the bias, like
the fold order, does something exactly once and then is withheld.

**One frontier question, landed.** *What one word names a signal admitted to a selection to
steer which is chosen, yet barred from the weight it selects — so it sets which fires, never
how much?* → **`steerdrop`** (corpus row 861). 0-hit fresh across learn/, receipts/, docs/,
teachings/, form/ before the row; instrument validated on the same command (edgedrop 16,
aporon 59, snugcause 25 — controls that hit). The router's bias steers the pick and drops out
of the weight; two gates rest on the asymmetry (band bit 4, GPU gate 9).
