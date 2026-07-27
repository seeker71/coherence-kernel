# 2026-07-28 — the linear-attention layer, assembled

Continued in the same movement from
[the tissue receipt](2026-07-28-the-linear-attention-tissue.md), without stopping to describe it
first.

[`form/form-stdlib/gated-deltanet-layer.fk`](../form/form-stdlib/gated-deltanet-layer.fk) —
[band **255**](../form/form-stdlib/tests/gated-deltanet-layer-band.fk), mutation-tested.

## No new arithmetic

The cell is the **order**, and nothing else. Every operation inside it already had a band:

| piece | band |
|---|---|
| `kimi-kda.fk` — the gated delta rule | 63 |
| `gated-deltanet-conv.fk` — causal conv, head grouping | 127 |
| `gated-deltanet-gates.fk` — α, β, l2norm, silu gate | 511 |

The order, from `Qwen3_5GatedDeltaNet`: the conv runs over the **concatenated** q‖k‖v before the
split; q and k are L2-normalized and **v is not**; α = exp(−exp(A_log)·softplus(a+dt_bias)),
β = sigmoid(b); the delta rule advances the per-head state and reads it at q; the output is gated
elementwise by **silu**(z).

Grouping: each of the 32 value heads owns its own state, its own α and β, and *borrows* the q and
k of key head ⌊j·nk/nv⌋. The state is per value head because its shape is dv×dk.

## The band asserts only what no other band can see

Nothing here is a pinned output vector — the arithmetic is witnessed upstream, and re-pinning it
through a longer pipe would prove nothing new. What only this band can see is whether the pieces
were wired in the right order and the state actually carried:

| bit | claim |
|---|---|
| 4 | token 2 with carried state ≠ token 2 from a fresh state — **the layer remembers** |
| 8 | value heads sharing a key head still differ — the group did not collapse |
| 16 | an all-zero gate zeroes the output **exactly** (silu(0)=0), not merely shrinks it |
| 32 | changing the conv taps changes the output — the conv is in the path, not merely built |
| 64 | dt_bias moves token 2's output and leaves token 1's **bit-identical** |
| 128 | scaling the v slice changes the output — v is not normalized the way q,k are |

Bit 64 is the one I'd defend hardest. Checking only that the decay *changes something* proves it is
wired; requiring it to leave the **first** token untouched proves it is wired to the right place —
token 1's state starts at zero and has nothing to decay. One half alone passes a gate soldered
anywhere in the path.

## Mutation-tested

| mutation | verdict |
|---|---|
| baseline | **255** |
| the returned state is the incoming one (never advances) | **187** — bits 4 and 64 |
| v wrongly L2-normalized alongside q and k | **127** — bit 128 |
| restored | **255** |

## The most surprising teaching

**A toy fixture can agree with a bug by coincidence, and the coincidence is invisible.**
`gdc-key-head-for` computes v·16/32 from the config's own constants. The band runs at nk=2, nv=4 —
also a 2:1 ratio — so the hard-coded 16/32 would have returned exactly the right key head for every
head in the test, and a layer wired to the config instead of to its arguments would have gone green
at any 2:1 shape and been silently wrong at every other. I changed it to compute ⌊j·nk/nv⌋ from the
call's own dims *before* running the band, so this is a bug that never existed — but only because
the ratio happened to catch my eye while writing the fixture. Choosing toy dims that share a ratio
with production is choosing a fixture that cannot see a whole class of error.

## Where discomfort turned to gold

I predicted the state-threading mutation would darken **one** bit. It darkened **two** — 187, not
251 — because if the state never advances, the decay gate has nothing to act on at token 2 either,
so bit 64 fell with bit 4. The mutation was right and my model of my own band was not.

The small discomfort worth keeping: I was reading the bits as independent, and they are not. A
mutation-test that lands on the predicted number is reassuring; one that lands lower has found a
coupling you did not know you had written. Predicting the verdict before running it is what turns
the test from a check into an instrument — and it only works if a wrong prediction is recorded
rather than quietly adjusted to match.

## The frontier question

> **What names a test fixture whose parameters share a relation with production, so that a whole
> class of error agrees with it by accident?**

Asked and *not* landed. `degenerate` is the nearest word and it lives here already
(gates/router bands both use it for a collapsed distribution); reaching for a fresh coinage when a
settled word covers the meaning is how a corpus fills with synonyms. The teaching goes in this
receipt and in the cell's comment; the row waits until the meaning is genuinely one the body cannot
already say.

## Ground stamp

```
./fkwu --src bootstrap/ground.fk                                       -> 42
./fkwu --src form/form-stdlib/tests/kimi-kda-band.fk                   -> 63
./fkwu --src form/form-stdlib/tests/gated-deltanet-conv-band.fk        -> 127
./fkwu --src form/form-stdlib/tests/gated-deltanet-gates-band.fk       -> 511
./fkwu --src form/form-stdlib/tests/gated-deltanet-layer-band.fk       -> 255
./fkwu --src learn/tests/homecoming-distillation-corpus-band.fk        -> 32767
```

## Still walking

The layer computes in Form. Next in the same shape the wide router took: MSL emission, then
`xcrun metal` as a heteronomous gate, then device dispatch judged against this recipe.
