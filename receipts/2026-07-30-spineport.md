# 2026-07-30 — spineport: the port manifest, from ds4's own function names

Urs: *"I still don't understand why and how our code is different from ds4, what are we trying to do
or prove. we have a working example and we cannot port it into form native?"*

## What we are trying to prove

Run this model with **every arithmetic op executed by kernels the body itself emits** — that is the
form-native claim, and it is why we cannot simply link ds4's C. We must re-express it; ds4 running the
same weights is the bar that tells us whether the re-expression is faithful.

## Why our code differed from a working example we can read

Because the port was assembled from **curated fragments**: anchored line numbers — the rope function
here, the KV quantizer there — reassembled by my understanding of the architecture. ds4.c is 13,000+
lines with two KV caches. My understanding picked one and called it the whole. **I ported my reading,
not their code.** Every later gate then verified the reading, faithfully, against itself.

## Yes, we can port it directly — and here is the manifest

ds4's own names answer the question. The per-token CPU forward is:

```
forward_token_raw_swa_cpu            ds4.c:13606   ← the spine; "raw_swa" is in the NAME
  layer_forward_raw_swa_one          13395           one layer, their order
    layer_attention_raw_swa_one      12936           the raw attention — never ported
    layer_attention_mixed_one        12567           raw + compressed, past the window
    compressor_decode_one            12381           the side lane — the ONLY one we ported
  output_hc_head_one                 13876           already matched (146-gate exit head)
  output_logits_one                  13904
supporting, never surfaced by fragment-reading:
  dsv4_hadamard128_inplace_cpu       3253            a Hadamard transform on activations
  dsv4_fp4_act_quantize_row          3268            FP4 activation quantization
  forward_first_token_cpu / prefill_layer_major_cpu  13848 / 13631   (prefill spine)
```

The port discipline this names: **transcribe one entry point and its whole callee closure, in its own
order — the spine — and only then re-express per-function.** Fragments invite the reader's
architecture to fill the gaps, and the reader's architecture is exactly what a port exists to avoid
trusting.

## The plan, in the order the spine dictates

1. **fp64 first**: transcribe `forward_token_raw_swa_cpu`'s closure into the oracle, function for
   function, ds4's order, no interpretation — with Q8_0/Q2_K reads (already device-proven).
2. **Gate the oracle against ds4 itself**: same prompt, `--dump-logits`; the oracle must reproduce
   ds4's argmax before it judges anything else. That closes the common-mode hole permanently — the
   reference is no longer written from my reading alone.
3. **Then per-layer bisect the Metal lane** against the now-anchored oracle, raw mode, both carriers.
4. Re-express each spine function as body-emitted MSL where it is not already (the attend kernel,
   rope, norms, matvecs all exist; the window bounds, mixed-lane merge, and hadamard/fp4 stages are
   the new work).

## The most surprising teaching

The answer to three days of "why is it different" was sitting in a function name. `raw_swa` — raw
sliding-window attention — is the trunk, and ds4 says so at the top of its own call tree. Corpus row
945, `spineport`: `firststop` (937) was *enumerate before running*; this is *enumerate before
porting*.

## Where discomfort turned to gold

Hearing "we have a working example and we cannot port it?" — the sting is that the framing exposes
three days of instrument-building as a substitute for reading forty consecutive lines of the thing
being ported. The instruments were real and each caught something true. But the port itself was never
re-grounded; every gate inherited the original fragmentary reading. The gold is a discipline simple
enough to hold: **the unit of transcription is the call tree, not the function.**

## Ground stamp

```
ds4.c function inventory 12500-14100 via form-run: the spine and its closure, line numbers above
forward_token_raw_swa_cpu:13606 — the name itself states the trunk lane
never-ported functions found: layer_attention_raw_swa_one, layer_attention_mixed_one,
  dsv4_hadamard128_inplace_cpu, dsv4_fp4_act_quantize_row
corpus band 32767; 340 rows, max-mid 945 — counts asked of the body
```
