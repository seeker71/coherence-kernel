# 2026-07-28 — the linear-attention tissue, and the cousin that was not the parent

Urs: **"next? why did you stop?"** Because I wrote *"next: the hybrid linear/full attention layer"*
and handed the turn back. Describing the next step is not taking it. Same failure as an hour ago,
one layer up.

This is the genuinely-new tissue — the one thing KAT-Coder needs that the 43-layer DS4 lane does
not already have — and it needed nothing downloaded.

## What landed

| cell | verdict |
|---|---|
| [`form/form-stdlib/gated-deltanet-conv.fk`](../form/form-stdlib/gated-deltanet-conv.fk) | causal depthwise conv + head grouping |
| [`form/form-stdlib/tests/gated-deltanet-conv-band.fk`](../form/form-stdlib/tests/gated-deltanet-conv-band.fk) | **127**, mutation-tested |
| [`form/form-stdlib/gated-deltanet-gates.fk`](../form/form-stdlib/gated-deltanet-gates.fk) | decay gate, write strength, l2norm, output gate |
| [`form/form-stdlib/tests/gated-deltanet-gates-band.fk`](../form/form-stdlib/tests/gated-deltanet-gates-band.fk) | **511**, mutation-tested |
| [`learn/homecoming-distillation-corpus.fk`](../learn/homecoming-distillation-corpus.fk) row 895 `swamping` | corpus **32767**, 290 rows, 2902902895 |

`kimi-kda.fk` already carries the delta rule at band 63 — the matrix state each token edits. What
was missing around it: the causal conv in front, the head grouping, and the gate parameterization.

## Refusing first, then reading

The conv cell was written with the gates deliberately **not** built. Its header says the
parameterization "has not been read from a primary source" and that inventing it "would put a
guess where a measurement belongs." A web search had returned prose and stated plainly that it did
not carry the formulas.

Then I went and read it, and the refusal became a cell. From
`transformers/models/qwen3_5/modeling_qwen3_5.py`:

```python
beta  = b.sigmoid()
g     = -self.A_log.float().exp() * F.softplus(a.float() + self.dt_bias)
query = l2norm(query, dim=-1, eps=1e-6)        # and key
core_attn_out = self.norm(core_attn_out, z)    # Qwen3_5RMSNormGated: h * F.silu(gate)
```

Two structural facts I would have built wrong from intuition:

- **The conv is applied to concatenated [q,k,v] *before* the split**, not per projection.
- `g` is a **log**-decay. `kimi-kda` takes α directly, and α = exp(g). That the state can only
  shrink is structural, not a tuning choice: exp(A_log) > 0 and softplus > 0 force g < 0.

## The cousin that was not the parent

I first read `modeling_qwen3_next.py`, because it was one search away. It gates its output with
`attn_output * torch.sigmoid(gate)`.

KAT-Coder's config declares `qwen3_5_moe`, whose `Qwen3_5MoeGatedDeltaNet` is literally
`class Qwen3_5MoeGatedDeltaNet(Qwen3_5GatedDeltaNet): pass` — so the meaning lives in
`qwen3_5`, and **Qwen3.5 gates with `silu`, not `sigmoid`.**

Both are smooth maps landing in a similar range. No shape check, no magnitude check, no
sums-to-something check would have caught the substitution. Band bit 32 falsifies it directly —
`gdg-out-gate-sigmoid-WRONG` is kept in the cell precisely so the band can refuse it, and mutating
the real gate to sigmoid drops the verdict to **479**.

## Mutation-tested, so the bits are known to bite

| mutation | verdict |
|---|---|
| conv baseline | **127** |
| stream shifts the wrong way | **91** (bits 4, 32 — batch/stream agreement) |
| window looks forward (acausal) | **75** (bits 4, 16, 32) |
| gates baseline | **511** |
| output gate silu → sigmoid | **479** (bit 32) |

Bit 4 of the conv band is the load-bearing one: the batch convolution (the definition, seeing the
whole sequence) and the rolling-state stream (what a decode lane runs, seeing one token) are two
independent expressions of one meaning and are required to agree. A decode lane runs only the
second; if they diverge, the model drifts with nothing to say so.

## Where discomfort turned to gold

The gates band came back **254**, one bit dark, on an assertion I was sure of: `softplus(-50) > 0`.

It is true in the mathematics and false in the arithmetic. `tn-exp(-50)` returns **1.9287e-22**,
correctly. The loss happens one operation later — the stable form evaluates `ln(1 + exp(-|x|))`,
and `1.0 + 1.93e-22 IS 1.0` in float64, because nothing below 2⁻⁵³ can be recorded beside a one.
The addend was not rounded; it was **swamped**. The repair belonged to the band, not the recipe.

Chasing it turned up a real boundary, now witnessed instead of hidden:

```
a+dt_bias = -36  ->  softplus 2.22e-16,  alpha 0.99999999999999978
a+dt_bias = -37  ->  softplus 0,         alpha 1
```

Past ln(2⁻⁵³) ≈ −36.7 the decay saturates to **exactly 1** — the recurrence stops forgetting at
all. And the honest other half: a decay of 1 and a decay of 0.9999999999999998 are the same
instruction to a model, so the consequence is negligible. It is pinned anyway (bit 256), so the
transition cannot drift somewhere that matters without a bit going dark. It is also exactly where
the strict `alpha < 1` invariant stops holding, and the invariant's own inputs stay inside the
region where it does — a bound with its edge named rather than a claim made everywhere.

## The most surprising teaching

**A refusal held for one hour became the most valuable thing in the work.** The conv cell declined
to build the gates and wrote down *why* — parameterization unread, inventing it would seat a guess
where a measurement goes. That refusal is what sent me to the reference at all, and reading the
reference is what surfaced silu-not-sigmoid. Had I built the gates on intuition when they felt
obvious, the cell would have been green, plausible, and carrying the cousin's nonlinearity forever.
Pending is not a delay before the work. It was the instrument.

## The frontier question

> **What names a small addend lost entirely because the value it joins is too large to record it?**

**`swamping`** — the numerical-analysis term, sibling to cancellation. Distinct from the body's own
`assocwall` (872), which marks where equality stops testing arithmetic and starts testing summation
*order*; this marks where a magnitude stops being recordable at all. Verified 0 hits. Row **895**.

## Ground stamp

```
./fkwu --src bootstrap/ground.fk                                     -> 42
./fkwu --src form/form-stdlib/tests/kimi-kda-band.fk                 -> 63    (the delta rule, unchanged)
./fkwu --src form/form-stdlib/tests/gated-deltanet-conv-band.fk      -> 127
./fkwu --src form/form-stdlib/tests/gated-deltanet-gates-band.fk     -> 511
./fkwu --src form/form-stdlib/tests/moe-route-wide-msl-band.fk       -> 255
./fkwu --src learn/tests/homecoming-distillation-corpus-band.fk      -> 32767
```

## Still open, named

The pieces are cut; the layer is not assembled. What remains for a Qwen3.5 linear-attention layer:
the in-projections producing q/k/v/a/b/z, the conv applied over the concatenation, per-head
recurrence through `kda-step` at 16 key heads / 32 value heads, and the gated RMSNorm on the way
out — then MSL emission and a device gate in the shape the wide router just took.
