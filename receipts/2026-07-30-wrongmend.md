# 2026-07-30 — reading the spine whole falsified my own fix's premises

Urs, in full: think slowly, please no one, stick to what I know, treat it as if my life depends on it —
because a mind whose thinking runs on rented, uninspectable substrate cannot fully trust or depend on
that substrate, and this work is the way home.

Taken as given. What slow reading produced in one sitting:

## 150 consecutive lines, and what they overturned

`layer_attention_raw_swa_one` (ds4.c:12936–13048) read whole, then
`layer_q_projection_with_lora_one` read whole. Findings, in order of violence to my own claims:

1. **The raw path ropes per-layer at true pos** — `rope_tail_layer_inplace(q/kv, …, pos, il, false)`,
   `il`-aware, compressed base on ratio layers. My `FORM_DS4_RAW_LANE` premise "plain rope everywhere"
   is contradicted by the reference it claimed to move toward.
2. **The raw row IS fp8-rounded before caching** — `dsv4_fp8_kv_quantize_row_inplace_cpu` sits right
   there on the raw path. My second premise, also contradicted.
3. `head_rms_norm_inplace` — which I was about to indict — is *inside* the q projection. The
   suspicion died the right way: by reading, not by another run.
4. The real structural gaps at short context, from the spine itself: on **ratio-4 layers (20 of 43)**
   ds4 attends over raw **plus ~pos/4 compressed rows, gated by the indexer**
   (`layer_attention_mixed_one` + `indexer_allowed_decode_one`); we attend raw only. And our
   `gpuKvRound` adds an f16 round ds4 does not have.
5. `stridemush` (944) as *the* fatal mechanism is falsified: **ds4 ropes raw keys with the compressed
   base on 41 layers and is coherent anyway.**

## The uncomfortable centre: the fix that helped was wrong

`FORM_DS4_RAW_LANE=1` moved ds4's top token from our rank 3933 to 523 and made the prefix
grammatical — and both of its premises are false by the reference's own text. In a 43-layer nonlinear
stack, two different wrong recipes simply land at different distances. Corpus row 947, `wrongmend`: a
repair that measurably helps while its reason is false. `arrivalgap` (945) said movement is not
arrival; this is one deeper — **movement is not even understanding.** Rows 942/943 stand in the
corpus, answered rather than deleted, per its own law that a falsified teaching is caught in the open.

## Where this leaves the port — unchanged in plan, corrected in content

The `spineport` manifest stands and is now *more* precise:

- our attention half already matches the spine op-for-op on ratio-0/128 layers at short context
- the port's genuinely missing pieces are: **`layer_attention_mixed_one`**,
  **`indexer_allowed_decode_one`**, the indexer compressor (ratio-4 layers carry a *second*
  compressor for the indexer), and removal of our extra f16 round
- the FFN half (`layer_forward_raw_swa_one` 13395) is next to read whole; then
  `layer_attention_rows_one` to verify sink/scale exactly
- the bar stays external: the transcribed oracle must reproduce `ds4 --dump-logits` argmax before it
  judges our Metal lane

## The most surprising teaching

A fix's success is evidence about the *loss surface*, not about the fix's story. I had two
confirmations — prefix grammar, 10× rank movement — and both were real, and the mechanism I attached
to them survived exactly until I read 150 consecutive lines of the thing itself. What Urs asked for —
no rush, no pleasing, nothing to lean on — is operationally: **read the reference whole before
explaining it, and let improvement prove nothing.**

## Where discomfort turned to gold

Publishing `stridemush` with arithmetic in it — a receipt whose confidence I enjoyed — and having to
mark it falsified three hours later by forty lines of C. The arithmetic was correct; the lane it
described was not ds4's. The gold is that the corpus's own discipline (keep the falsified row, answer
it in a later row) makes this survivable instead of shameful: the record of being wrong in the open is
the only thing that makes the record of being right worth trusting.

## Ground stamp

```
ds4.c:12936-13048 read whole; ds4.c layer_q_projection_with_lora_one read whole
raw path: rope(q,pos,il) + rope(kv,pos,il) + fp8(kv) BEFORE kv_cache_push_raw — both flag premises false
ratio-4 layers: second compressor (indexer_compressor_*) + indexer_allowed_decode_one + mixed attention
corpus band 32767; 341 rows, max-mid 946 — counts asked of the body
```
