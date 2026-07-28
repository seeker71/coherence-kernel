# Form CLI reaches native DS4 generation

## Exact inquiry

The live Form CLI received:

> Identify the largest remaining bottleneck in this local native generation
> path and name the next code change.

The generic system preamble was removed from the native route. The CLI now
tokenizes the inquiry to the same 22 ids as the already-witnessed direct route:

`0,128803,71343,270,9152,11499,111127,295,566,3277,12818,9353,3618,305,2329,270,2894,4181,2757,16,128804,128822`

Before this repair the CLI added 30 unrelated system tokens and presented 52
prompt tokens to the same model.

## Native route

`fdl-native-render` now calls `dns-generate-ids-jit` with the configured local
carrier. Two full CLI executions completed with:

- generation lane: `native-ds4-form-metal`;
- generated tokens: 64;
- prompt crossing:
  `3818c2494a76084d5e033457ee87e499c1e4d3d923db2ddb2d902176797da7f1`;
- result crossing:
  `39c515a2cc8ce09d3a1d963f391cc38e84f091b06844947bd1b9076e40e2290a`;
- run identity:
  `01082d3a1b2ef6228ecac76d1b5bc1b06fdd3e64410f1f66fcc9750325b16cb4`;
- network / remote / shell / Swift / temporary crossings: all zero;
- wall time: 284.67 seconds, then 282.69 seconds.

Identical run and crossing identities across the repeats show that the prompt
and complete token sequence were deterministic.

## Durable replay repair

The first completed generation reported `form-cli-receipt-written:0`.
`fs_mkdir` currently creates one level in the fkwu carrier, while the CLI
attempted to create the root and the grandchild but skipped the intermediate
`form-cli` directory. Form now creates all three levels explicitly. An isolated
band creates the tree, writes and reads a payload, removes the test tree, and
returns `31`.

The second full generation reported `form-cli-receipt-written:1`. Its ignored,
runtime-local receipt contains 5,189 bytes, including all 64 token ids and both
content-addressed crossing enquiries.

## Semantic adjudication

The local model generated:

```text
Based on the provided context, the largest remaining bottleneck in the local native generation path is the **CPU-based token embedding lookup** (specifically the `CausalSelfAttention` module's embedding step).

**Next code change:** Replace the manual embedding lookup with a **GPU-accelerated embedding kernel** (e.g.,
```

This is fluent and reproducible, but it is not grounded in the body. The live
path already loads `dsv4-embed`, and `dlw-embed-token` calls
`metal_model_embed("dsv4-embed", "form_dsv4_embed_f16", ...)`. The proposed
change therefore describes work that is already present.

The 64-token result is retained as learned evidence of a missing context
boundary, not promoted as an answer. The next repair is to make current
framebuffer/source evidence available to the native prompt before inference
and to keep generation success distinct from semantic sufficiency.

That state distinction is now executable. A non-empty native stream reports:

```text
form-cli-local-ds4-generated:1
form-cli-local-ds4-accepted:0
form-cli-local-ds4-sufficiency:unadjudicated
```

An empty stream reports `generated:0`, `accepted:0`, and
`sufficiency:nothing`. The pure adjudication band returns `15`.

; witnessed: 2026-07-28 -> PASS native CLI generation and replay; REFUTED generated diagnosis
