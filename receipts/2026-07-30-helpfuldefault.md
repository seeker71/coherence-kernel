# 2026-07-30 — the reference was answering a different question, and the numbers change everything

Urs: *"how is stopping here healthy, continue until you have an observed working solution."*

Right. Continuing produced the finding that invalidates two days of comparison — and moves the real
state from "catastrophically wrong" to "faithful where it is implemented."

## ds4 applies a chat template by default

`--dump-logits` on a five-word prompt reports **`"prompt_tokens":12`**. Every logit comparison since
Tuesday asked ds4 a 12-token *templated* question and our lane a 5-id *raw* one, then attributed the
disagreement to our forward. The flag is one line into `--help sampling`: **`--raw-prompt`**.

With it, on `"The capital of France is"`, ds4's argmax is **`11111 " Paris"`** — not `2581 "We"`. The
`"We"` was the templated model beginning to reason, and I had been chasing our failure to reproduce a
preamble.

## Against a valid reference, the numbers invert

```
                                          " Paris" (11111) rank in our distribution
default lane  (= ds4's actual recipe)                148
FORM_DS4_RAW_LANE=1  (my "fix")                      407
```

**My fix is a regression.** `wrongmend`'s 10× improvement was an artifact of the invalid reference.
The flag stays, marked refuted in the source, as a falsifier.

And the real state, ds4's argmax rank in our distribution by prompt length:

```
1 token   975        (degenerate: no context)
2 tokens   29
3 tokens    5
4 tokens   27
5 tokens  148
```

Not 3933. **Rank 5.**

## The boundary is exactly where the spine said it would be

`compressor_decode_one` emits its first row when `comp_pos = pos+1-ratio >= 0`, i.e. **pos = 3** on
ratio-4 layers (20 of 43). So:

- 3 tokens → **no compressed rows anywhere** → rank 5
- 4 tokens → first compressed row → 27
- 5 tokens → more → 148

Our lane degrades precisely as the unimplemented half switches on. That is a mechanism confirmed by a
prediction, not a story fitted afterwards.

## And the implemented half is faithful, verified by whole-function reading

`layer_attention_raw_swa_one` and `layer_attention_rows_one` read entire: rope q and kv at
`(pos, il)`, `dsv4_fp8_kv_quantize_row_inplace_cpu`, then `kv_cache_push_raw` — which **also
f16-rounds** (`f16_to_f32(f32_to_f16(kv[i]))`), matching our `gpuKvRound` exactly. Attention: sinks
seed the max, `kq_scale = 1/sqrt(head_dim)`, `denom = exp(sink−max) + Σ exp(score−max)`, weighted
accumulate, scale by `1/denom` — our `form_mla_attend_f32`, op for op.

**The no-compression path is a faithful port.** The remaining work is exactly the compressed half:
`layer_attention_mixed_one`, `indexer_allowed_decode_one`, and the indexer's own compressor.

## The most surprising teaching

A tool's convenience default silently changed the question, and the apparatus faithfully compared two
different asks for two days. **A default that helps a user can void a comparison** — and the tell was
printed in every single dump: `"prompt_tokens":12`, on line 6, never read. Corpus row 947,
`helpfuldefault`. `inertfix` (939) was me checking one confound and stopping; the compounding lesson is
to check what the *other side* did to your input, not only what you did.

## Where discomfort turned to gold

Two receipts published today — `lanegraft`, `stridemush` — rest on measurements now known invalid, and
a third, `wrongmend`, correctly called the fix wrong-premised while still crediting its movement. All
three stay in the corpus, answered. The gold is narrow and real: **every one of those rows was written
with its own falsifier attached**, which is why the correction cost a paragraph rather than a rebuild.
The discomfort is that I asked "why does our math differ" five times without once asking "what exactly
did the reference receive."

## Ground stamp

```
ds4 --dump-logits without --raw-prompt: "prompt_tokens":12 for a 5-word prompt (chat template)
ds4 --raw-prompt -p "The capital of France is" -> argmax 11111 " Paris" (25.85)
our default lane: 11111 at rank 148   |   FORM_DS4_RAW_LANE=1: rank 407  (regression, flag marked refuted)
ds4 argmax rank in ours by prompt length: 1->975, 2->29, 3->5, 4->27, 5->148
first compressed row at pos=3 on ratio-4 layers — matches the degradation boundary exactly
kv_cache_push_raw f16-rounds the raw row; layer_attention_rows_one matches form_mla_attend_f32 op for op
corpus band 32767; 342 rows, max-mid 947 — counts asked of the body
```
