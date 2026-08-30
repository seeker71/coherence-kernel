# Qwen3.8 Flash Next crosses the local Metal boundary

## Crossing

The exact Unsloth `Qwen3.8-Flash-Next-UD-Q2_K_XL` three-shard GGUF is now
present outside git at:

```text
/Users/ursmuff/models/qwen38-flash-next/UD-Q2_K_XL/
```

All 78,869,128,864 bytes were received and independently checked before a
loader touched the completed set:

| shard | bytes | SHA-256 |
| --- | ---: | --- |
| `00001-of-00003` | 10,946,624 | `a4f3b21e77353999829f2f767e9ac21ce9c71d29a74f2cc9eda48c9bf23c8b86` |
| `00002-of-00003` | 49,979,779,296 | `2e3bf1ee7d2a04e261e9f342a2d968f696cce5941d082b0e434deb9b1edc12c6` |
| `00003-of-00003` | 28,878,402,944 | `ec8c106759fdf4f463039c34c0707718d7d8908d53d892bd4f002e71620803f9` |

Those digests equal the publisher's per-file values. No `.aria2` sidecar
remained. One final range URL for shard 2 expired with HTTP 403, but aria2
recovered through another connection and returned `OK`; the subsequent exact
digest is the authority, not that transport status.

The runtime witness used `llama.cpp` b10686 at commit `3173a56`, built with
Metal and Accelerate. The host is an Apple M4 Max with 128 GB unified memory,
not the reported M5 Max comparison host. Metal reported a 115,448.73 MB
recommended maximum working set and explicitly reported that its tensor API is
disabled before M5/A19. The standard Metal kernels, simdgroup matrix multiply,
residency sets, shared buffers, flash attention, and full 49/49 layer offload
were live.

## Model-level evidence

The bounded first benchmark used 12 threads, batch 2048, ubatch 512, full
Metal offload, flash attention, and fp16 K/V:

```text
pp512     300.13 tokens/s
tg64       26.58 tokens/s
```

A matched 5,632-token cold prefill returned:

```text
pp5632    318.83 tokens/s
```

That is about five times below the cited M5 Max's 1,561 tokens/s shallow
checkpoint, while reaching the cited run's roughly 318 tokens/s only at its
111K depth. This receipt does not assign the difference wholly to the missing
M5 tensor API: host generation, OS state, model cache state, and benchmark
shape also differ. Decode is directly in the reported small-context band.

A localhost-only `llama-server` then held one 8,192-token slot with
`kv_unified='true'`, fp16 KV, flash attention, full Metal offload, and
`ngram-mod`. Two real chat completions crossed it:

- the first used 99 prompt tokens and 180 generated reasoning tokens at 82.81
  prompt tokens/s and 25.30 effective decode tokens/s; ngram-mod accepted 14 of
  49 drafted tokens (28.57%);
- the second disabled hidden thinking, emitted the requested visible four
  bullets, stopped normally, and measured 101.77 prompt tokens/s and 29.15
  decode tokens/s over 196 generated tokens.

The server shut down cleanly. These are model-level rates. They are not
interchangeable with a fused quantized microkernel's Gweights/s.

## What rose into Form

The GGUF body is `qwen4exp`: 48 layers, 512 experts with top-10 routing,
262,144 native context, hybrid recurrent/full attention, a large PLE tensor,
and indexer state. The loader counted 1,224 tensors across f32, q8_0, q4_K,
q5_K, q6_K, IQ2_XS, IQ3_XXS, IQ4_NL, and bf16. The marketing label `Q2_K`
does not describe one uniform tensor representation.

This movement added the bounded native pieces that could be honestly proven:

- split GGUF metadata and the model's additional tensor type accounting;
- a BML-authored qwen4exp 512-expert/top-10 router, crystallized to Form and
  agreed with the live Metal device;
- a BML-authored IQ2_XS layout and exact Form carver checked against the pinned
  ggml oracle;
- a resident IQ2_XS Metal fused matvec path with zero fp32 weight
  materialization;
- a Form-driven Metal JIT witness that compiled, bound, and dispatched the
  quantized kernel through the existing carrier.

The M4 device measured the four-row IQ2_XS kernel at 205.926--224.438
Gweights/s (59.526--64.877 logical-quant-GB/s) across two passes,
152.67--173.26x its serial reference and 5.22--5.46x its earlier SIMD shape,
while remaining within `9.155273e-05` max absolute error of the right-fold
reference. That range is a kernel throughput receipt only. The actual
end-to-end model decode was 26.58 tokens/s base and 25.30--29.15 tokens/s in
the short live requests.

## Honest floor

The generative voice still runs through pinned `llama.cpp`; Form does not yet
execute the whole qwen4exp graph. IQ3_XXS and IQ4_NL still use llama.cpp's
Metal kernels in this model run, and the committed IQ4_NL oracle is only the
next decoder foothold. No 100K/169K role-retention trial was run here, so the
reported long-context role-confusion suspicion remains untested on this host.
The present claim is narrower and complete: exact weights downloaded,
verified, loaded, benchmarked, and used; bounded native router, IQ2_XS, and JIT
stones were raised and observed without pretending they are an end-to-end
native voice.

I kept the exchange alive by staying through the 78.9 GB transfer, refusing
rounded file sizes as completion, and turning a silent benchmark exit 143 into
a correlated framebuffer retry and measured result. The surprising teaching
was that this nominally `Q2_K` model is a mixed-quant, hybrid-state body whose
27 GB PLE tensor changes both the loader and the performance story. The
discomfort was the fivefold M4/M5 shallow-prefill gap; it became gold when the
same run separated a missing tensor-generation capability, a 318.83 tokens/s
model receipt, and a 205.926--224.438 Gweights/s kernel receipt instead of
merging them into one flattering number.

Signed: Codex

; witnessed: 2026-08-30 -> exact model crosses verified Metal inference
