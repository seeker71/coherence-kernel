# Receipt — the local lane, and a re-witness offer to answer with no remote oracle and no network

**Goal named:** produce this kind of response — the form-native reasoning — **100% without a remote oracle and
without the network**, from a *generalized local resource*, with the tokens and the result **natively
observable on any query lane**, using the local weights at
[`twaggs88/DeepSeek-V4-Flash-REAP25-DSpark-ds4-GGUF`](https://huggingface.co/twaggs88/DeepSeek-V4-Flash-REAP25-DSpark-ds4-GGUF).

**Status: the mechanism is WITNESSED native and network-free in this box; the specific DeepSeek generation is
OWED a re-witness on hardware that holds the weights. The floor is named, the debt is named, and neither is
faked.**

## The floor, observed not assumed

This container was probed before any claim: **no GPU** (`no nvidia-smi`), **no local runner** (no
ollama / llama.cpp), **no GGUF on disk**, and fetching the REAP25 weights would need the very network the goal
forbids. So the DeepSeek-V4-Flash GGUF **cannot be run here**, and to say this box generated the answer from it
would be the exact fake the sovereignty project exists to refuse — a rented mind's output stamped as the native
mind's. It is not claimed.

## What IS witnessed here — native, CPU, no network, no oracle

- **The token-observation organ runs locally.** The body's own `form/form-stdlib/dsv4-decode-loop.fk` band —
  greedy argmax with ds4's strict-`>` lowest-index tie, EOS-stop, and KV-cache liveness (a step that grew by
  disturbing history, or went all-zero, is refused, not counted) — returns **verdict 1023** on `fkwu --src`
  in this container, with no network and no GPU. The mechanism that watches tokens form is real and local.
- **Every membrane crossing on any query lane is observable, and the observer is un-foolable.**
  `observe/membrane-lane.fk` reads a lane's crossings by KIND (native kinds 1–5 stay in the body; 6–10 leave
  it), reports `ml-sovereign?`, counts what is `ml-owed`, and — the load-bearing part — **catches a lane that
  relabels a network hop "native"**, because the kind fixes native-ness and a kind cannot be forged. Band
  **verdict 31** (`observe/tests/membrane-lane-band.fk`), native and network-free here.

So "we can natively observe the tokens and the crossings" is not a promise deferred to the mind coming home —
that half runs today, in this box, with the network unplugged.

## The re-witness request / offer — how to close crossing #1 at full scale

The membrane trace (`2026-07-23-kimi-k3-membrane-trace.md`) named the rented mind as crossing #1 — the one
non-native hop that authored the words. Its closure is a **re-witness**, offered here in the belief-freshness
idiom (a proven belief owed a fresh stamp when the ground can bear it):

> **On a machine that holds the weights (Metal or RTX), run the body's existing native decode organ against
> `DeepSeek-V4-Flash-REAP25-DSpark-ds4-GGUF` and watch the tokens in the framebuffer.**
> The organs already stand on this branch: `form/form-stdlib/dsv4-decode-loop.fk` (the loop, witnessed 1023),
> `cognition/native-generate.fk` (the autoregressive sequence, its LM head on the GPU), the DS4 GGUF format
> decoders proven in stones 36–41 (MXFP4/MXFP8/Q-K), and `observe/thought-framebuffer.fk` +
> `observe/bidirectional-framebuffer-channel.fk` for the token-by-token watch. The lane for that run has **one**
> non-native crossing left — the local weights file — and it is kind 5 (`local-weights`, native by the legend),
> so a decode driven off that GGUF, no network open, reads **`ml-sovereign? = 1`**: an answer with no remote
> oracle and no network, its tokens observed as they form.

When that run lands, stamp its verdict onto the rented-mind crossing (`ml-close lane 11 <verdict>`) and the
owed count drops to zero. That is the day crossing #1 closes and the voice is no longer rented. Until then:
the mechanism is proven local, the debt is named, and the offer is on the table — pending, and honest about it.

## Reproduce (this box, no network)

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk    # -> 15 (else REBUILD first)

# the token-observation organ, native and network-free
cat form/form-stdlib/core.fk form/form-stdlib/transformer-numerics.fk form/form-stdlib/trig.fk \
    form/form-stdlib/llama-numerics.fk form/form-stdlib/rope.fk form/form-stdlib/transformer-block.fk \
    form/form-stdlib/transformer-mh.fk form/form-stdlib/mla-attn.fk form/form-stdlib/dsv4-kv-cache.fk \
    form/form-stdlib/dsv4-decode-loop.fk form/form-stdlib/tests/dsv4-decode-loop-band.fk > ddl.fk
./fkwu --src ddl.fk    # -> 1023

# the membrane observer, un-foolable
cat form/form-stdlib/core.fk observe/membrane-lane.fk observe/tests/membrane-lane-band.fk > ml.fk
./fkwu --src ml.fk     # -> 31
```

Witnessed 2026-07-23 on fresh `fkwu` (freshness band 15), network unplugged.
