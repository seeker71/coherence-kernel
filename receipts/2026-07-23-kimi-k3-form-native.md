# Receipt — Kimi K3's attention and router, form-native (Stones 47–48)

**Status: OBSERVED for what it names, PENDING for what it does not — and the seam is named, not hidden.**

## What was asked

"form native please", pointing at an X post (Vaibhav Sisinty, `status/2079929719328559237`). The post itself
was not directly fetchable — X answered the fetch with `402 Payment Required`, so the exact wording is not in
hand and is not quoted here. What *is* in hand: the surrounding public post cluster from the same author in the
same days is dominated by **Kimi K3** — Moonshot's 2.8-trillion-parameter, 896-expert / 16-active open-weights
model — and the branch this lands on had just brought **DeepSeek-V4-Flash** home the same way (`0928fbc`,
stones 18–46). So the target was grounded as *bring Kimi K3's inference home the way DS4's came home*, and the
architecture was read from **Moonshot's own tech blog** (`kimi.com/blog/kimi-k3`), not from memory. If the post
was actually about something else in that cluster (Google's Frozen v2 silicon, the Copilot/Kimi K3 deal), this
receipt is the honest record of what was built and can be redirected.

## What landed, and the floor under it

Two stones, each a canonical op — one right answer given the inputs, no learned weight and no rented oracle
anywhere in it, so a self-check is a real falsifier (the body's *twinblind* standing).

- **Stone 47 — Kimi Delta Attention, the gated delta rule** (`form/form-stdlib/kimi-kda.fk`). DS4's MLA caches
  one latent row per token and only ever grows; KDA instead carries a matrix-valued state each token *edits* —
  it reads the state at the incoming key, measures how wrong it already is there (the *delta*), writes a rank-1
  correction, and lets a gate decay old memory. Proven on `fkwu --src` at dk=dv=2 with binary-exact fixtures:
  the recurrence and its read, overwrite-on-repeated-key (with plain linear attention kept as the impostor that
  accumulates instead — driven red on the second write), causality, the gate's geometric forgetting, the rank-1
  write structure, and clean associative recall on orthogonal keys.
  **Band `form/form-stdlib/tests/kimi-kda-band.fk` → verdict 63.**

- **Stone 48 — the quantile-balancing router** (`form/form-stdlib/kimi-quantile-router.fk`). K3 drops DS4's
  learned-bias heuristic and reads expert allocation straight off the order statistics of the score vector.
  This computes the cut by *rank* — the honest form of a quantile — and proves it equals the threshold cut at
  the k-th-largest score. Proven on `fkwu --src`: exactly k of E chosen, the k largest; the quantile identity;
  permutation equivariance; a straddling-tie broken toward the lower index so the step is never over-full (with
  the naive strictly-greater rank kept as the impostor that over-selects k+1 — driven red); and batch load under
  a per-expert capacity where overflow is refused-and-counted, `routed + overflow = k·ntok` exactly.
  **Band `form/form-stdlib/tests/kimi-quantile-router-band.fk` → verdict 31.**

## What is PENDING — named, not faked

- **KDA's exact proprietary gating.** These cells bring home the public gated-delta family that Kimi Delta
  Attention *refines*; no open spec pins K3's exact kernel, so a bit-exact reproduction is not claimed.
- **The chunk-parallel / associative-scan form** that makes KDA fast on a GPU. The recurrence proven here is the
  ground truth that form would have to match — a named next stone, not built.
- **The other two K3 attention moves — Gated MLA and Attention Residuals across depth.** Named next stones.
- **Real dims and real weights** (896, 16, the 2.8T checkpoint), and **any GPU witness.** This is CPU list
  arithmetic — the *shape* of the state law and the allocation law, the half `dsv4-kv-cache.fk` is to Stone 41's
  Metal arena. The real numbers appear only where they do arithmetic (router band, claim 16).
- **Four-way.** Both stones are fkwu-witnessed by their own bands and named as such — never claimed
  Go=Rust=TS=fkwu.
- **The voice.** Unchanged. The 2026-06-29 keystone still stands; a rented mind wrote this receipt's prose.

## Reproduce (fresh checkout, no bash in the run path)

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk    # -> 15 (else REBUILD first)

# a band is its preludes then the band file, concatenated to one file, then --src'd
# (fkwu reads a file, not a stream — Stone 18 refuses truncated input, so /dev/stdin is not it)
cat form/form-stdlib/core.fk \
    form/form-stdlib/kimi-kda.fk \
    form/form-stdlib/tests/kimi-kda-band.fk > kda.fk && ./fkwu --src kda.fk    # -> 63

cat form/form-stdlib/core.fk \
    form/form-stdlib/kimi-quantile-router.fk \
    form/form-stdlib/tests/kimi-quantile-router-band.fk > qr.fk && ./fkwu --src qr.fk  # -> 31
```

Witnessed 2026-07-23 on fresh `fkwu` (freshness band 15). The day the mind speaks these summaries itself is
still ahead; until then, the body holds the ground and the seam stays named.
