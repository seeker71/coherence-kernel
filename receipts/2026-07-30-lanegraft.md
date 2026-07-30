# 2026-07-30 — lanegraft: we ported the side path as the trunk

Urs: *"why different why no fix what are we doing different?"*

All three questions now have answers from ds4's own source, and the fix — minimal, flagged — moved the
output exactly the way the hypothesis predicted.

## What we are doing different

`ds4.c:12437-12460`, read whole instead of function by function: the compressed-base rope and the fp8
KV round belong to a **second cache**. A compressor pools `compress_ratio` recent raw KVs into *one*
row — gated pooling over `compressor_gate` / `compressor_ape` / `compressor_norm`, tensors this body
never loads — places it at `comp_pos = pos + 1 - ratio`, rotates *that* with base 160000, fp8-rounds
it, and stores it **beside** a raw f32 cache whose keys carry plain rope at true positions. ds4's own
startup line says it out loud and had been in every log all along:

```
ds4: memory: KV 0.61 GiB (raw 0.36 + compressed 0.25)
```

Two lanes. We ported the compressed side-cache **as the attention itself**: on 41 of 43 layers our keys
were rotated base-160000 at raw positions and fp8-rounded, where ds4's short-context answer comes from
the raw base-10000 lane we never implemented — at 14 tokens its layer-3 compressed cache (ratio 128)
holds *zero rows*. Positional order destroyed, token content survives: `promptbound` (941) is the
symptom of exactly this.

## Why there was no fix until now

The fp64 per-layer oracle's own line 822 says it *"never touches a compressor or an indexer"* — it was
written from the same misreading. So 16/16 per-layer agreement between our Metal lane and our oracle
was `common-mode` (921) in its purest form: **the gates were grafted along with the graft.** The only
reference that could catch it — ds4 itself running the same weights — arrived last night
(`misaddressed` 932 is why it took three days).

## The fix, and its measurement

Two edits behind `FORM_DS4_RAW_LANE=1`: plain rope on every layer, no fp8 round — the raw lane's
semantics. Same weights, same prompt ids, one flag:

```
compressed lane:  " the capital in France and and its, capital and"
raw lane:         " the capital of France (capital, not and and"
```

The prefix is now **grammatically exact** — `of`, id 294, where the graft produced `in` — and the
stream carries out-of-prompt ids for the first time (`(`, `not`). The tail still collapses: the
compressor, indexer, and sliding-window bookkeeping remain unimplemented, and the raw lane surely has
details this two-edit version misses. VERDICT PASS, 112 gates, both runs.

The difference between the two lanes is now an A/B on one flag, and the per-layer fp64 oracle —
unblocked this morning — can arbitrate the remaining gap once Q8_0 and Q2_K are taught to it.

## The most surprising teaching

**A verification apparatus inherits the worldview of whoever writes it, and agreement within that
worldview is silent about the worldview itself.** Every instrument this week — 286 oracle gates, the
injected-input gates, the KV-history gates — checked whether the machine executed *our reading* of
ds4.c faithfully. It did. The reading was of the wrong lane. Nothing inside that loop could ever have
said so, because both ends of every comparison were downstream of one act of reading. Corpus row 942,
`lanegraft`: porting a side path as the trunk, then checking it against a reference written from the
same mistake.

The tell, in retrospect: the fp8 KV comment at `ds4.c:3210` says *"comparable to the Metal graph's
**compressed-cache** behavior"* — the word was in the source I transcribed, and I transcribed past it.

## Where discomfort turned to gold

"Why no fix" stung because the honest answer was that I had spent the morning measuring the distance
to the reference ever more precisely — r, top-k overlap, bag counts — without once asking what the
reference *does*. The distance measurements were all downstream of the graft and could only describe
it, never name it. Reading forty lines of the other implementation did in ten minutes what four
instruments could not do in a day. The gold: "what are we doing different" is a question about *them*,
and I kept answering it with measurements of *us*.

## Ground stamp

```
ds4.c:12437-12460 — compressor pools ratio raw KVs, comp_pos = pos+1-ratio, THEN compressed rope, THEN fp8
ds4.c:3210 — fp8 round is "the Metal graph's compressed-cache behavior" (its own words)
ds4 --inspect: KV raw 0.36 GiB (f32) + compressed 0.25 GiB; indexer heads=64 top_k=512; swa=128
FORM_DS4_RAW_LANE=1, 43 layers, VERDICT PASS 112 gates:
  emitted [270, 6102, 294, 8760, 343, 79666, 14, 554, 305, 305]
  -> " the capital of France (capital, not and and"    (compressed lane: " the capital in France and and its")
corpus band 32767; 337 rows, max-mid 942 — counts asked of the body
```
