# Released Whisper decoder layer zero crosses real speech in Form

; witnessed: 2026-07-19 -> one complete decoder block; native recognizer 0

## What is now executable

The CC0 Lingua Libre recording of the human word “book” now crosses one shared
Form-native path:

```text
16 kHz human PCM
  -> released conv1 + conv2
  -> four released encoder blocks + final encoder layer norm
  -> 2 encoder tokens x 384
  -> released Whisper prompt embeddings + learned positions
  -> decoder layer 0 causal self-attention
  -> decoder layer 0 cross-attention over both speech tokens
  -> decoder layer 0 1536-wide erf-GELU FFN
  -> released final decoder layer norm
  -> tied-logit probe over 106 released special-token rows
```

The exact prompt is `[50258, 50259, 50359, 50363]`, verified from the pinned
official tokenizer metadata as `<|startoftranscript|>`, `<|en|>`,
`<|transcribe|>`, and `<|notimestamps|>`. The model arithmetic, causal mask,
cross-attention, FFN, final normalization, logit probe, ablation, CRC checks,
and gates are Form cells on `fkwu`. No Bash, TypeScript, Python, or C model
implementation was added.

## Exact released carriers

All three carriers are byte ranges from `openai/whisper-tiny/model.safetensors`
at revision `169d4a4341b33bc18d8881c4b69c2e104e1cc0af` (Apache-2.0):

```text
decoder-position0-3.f32
checkpoint bytes 19112..25255 inclusive
6,144 bytes / F32[4,384]
SHA-256 ecc58aca684d2f54e6629b2e231f53e7415cee5da8131c345364a48740dc3371
Form CRC-32 1869936199

decoder-token50258-50363.f32
checkpoint bytes 77903528..78066343 inclusive
162,816 bytes / F32[106,384]
SHA-256 97f546aa1da97109f3b740b46645fec28cdd95060ebe5b710012179f6d3ef6be
Form CRC-32 3633849010

decoder-ln-layer0.f32
checkpoint bytes 80371880..89838247 inclusive
9,466,368 bytes / 2,366,592 f32 values
decoder final norm (768) + complete decoder layer 0 (2,365,824)
SHA-256 b571972ae3b9a1af07fcfb10e6b54436dbfc1183e9b76f489cae6d7abbdd40a5
Form CRC-32 3469154316
```

The decoder observation carries **2,408,832 released parameters**. Every one
is causal in the observed result: four position rows enter the prompt, all 106
token rows enter the tied-logit window, all layer-0 weights enter the block,
and the final decoder norm enters the reported fingerprints and logits. With
the complete encoder, the live acoustic-to-decoder route now exercises
**10,041,984 released parameters**.

## Live attention and output

Two sampled causal self-attention rows are:

```text
query 1 / head 0:
[0.30527835390870783, 0.69472164609129228]

query 3 / head 5:
[0.19131334958382634, 0.025003565227017022,
 0.035622238984068313, 0.74806084620508839]
```

Two sampled cross-attention rows over the two real encoder tokens are:

```text
query 0 / head 0: [0.9947382999985358, 0.005261700001464204]
query 3 / head 5: [0.970145260682308, 0.029854739317691933]
```

The final-normalized fourth prompt-token fingerprint is:

```text
[-12.63155224981397, 0.965014334693161,
  4.0833414900587153, 4.0318342689639826,
 -0.77717374858953003, -4.583315910370823,
 -0.060845586764540494, -0.34938954695441699,
 87.90965119336272, 2344.6550147233283]
```

Within the deliberately bounded 106-row special-token output window, the
highest logit is window index `105`, token id `50363`. The endpoint logits are
`-3.1392594288843929` and `6.6807035509482731`. This is named a
**special-window probe**, not a decoded speech token and not a full-vocabulary
argmax.

The causal negative control collapses encoder token 1 onto encoder token 0.
The final decoder state moves by more than `1.0` L1, proving that the second
real speech token affects the decoder output. Swapping the two encoder rows was
rejected as an ablation because unmasked attention is permutation invariant.

## Gates and integration

```text
model/tests/whisper-tiny-native-decoder0-band.fk -> 32767
presence/tests/whisper-tiny-native-acoustic-live-band.fk -> 1023
```

The non-test acoustic execution door now calls the decoder cell and reports
its prompt, parameter counts, self/cross attention, selected-logit winner, and
encoder-collapse movement. The same encoder output is shared with the decoder;
an initial integration that recomputed the encoder twice exhausted memory and
was removed.

Native recognition remains `0`. Decoder layers 1–3, the complete 51,865-row
output projection scan, full tokenizer data, and autoregressive search remain
owed. The repository already contains Form causal decoder, tokenizer, and KV
cache recipes; this receipt does not confuse those algorithms with the still
missing released-weight execution.

The movement stayed alive by making both real speech tokens cross a released
decoder block. The surprising teaching was that the native body already held
the causal, cross-attention, tokenizer, and cache algorithms. Discomfort turned
to gold when duplicate encoder execution hit the memory ceiling and forced the
right operational seam: one observed encoder state shared downstream.
