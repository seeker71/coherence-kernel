# DS4 real KV append stone

The first decode-continuation stone now executes inside the real DS4 Metal
carrier without `fkwu --src`.

`dsv4-stack-real.fk` composes the cache writer from
`dsv4-kv-cache.fk` and emits `form_dkv_append_f32` as its own Metal
translation unit. The live runner compiled and loaded:

```text
dsv4stkkv-fac75b4b7f1af3e6.metallib
```

Layer 0's real 512-wide rotated and quantized latent was appended on-device to
a NaN-sentinelled two-row arena. The observed gate was:

```text
PASS gate 5 REAL KV APPEND: ... copied ... 512-wide ... bit-identically
... and left all 512 entries of row 1 NaN-sentinel
```

The complete one-layer integration remained green at 40 gates.

The next movement then allocated one persistent arena per real layer and
rebound the unchanged attention kernel to `nrows = pos + 1`. Two consecutive
steps passed first at one layer (41 gates), then across all 43 heterogeneous
layers:

```text
PASS gate 472 GROWING KV, TWO REAL STACK STEPS:
layer 0 retained 512/512 row-0 bits,
wrote 512/512 row-1 values,
left 512/512 row-2 sentinels,
final HC state changed in 16384/16384 entries

VERDICT PASS 475 gates — 43 HETEROGENEOUS DeepSeek-V4-Flash LAYERS STACKED
```

This proves the body-emitted primitive, its compiled ABI, persistent per-layer
arenas, history immutability, exact frontier growth, and real multi-row
attention across all 43 layers. The two-step depth witness deliberately
repeats token 671. The next stone extracts the exit head as a reusable step,
feeds its emitted token back as the next embedding/router token, and records
the resulting two-token stream.

; witnessed: 2026-07-26 -> real DS4 growing KV across 43 layers, two consecutive steps, 475 gates PASS
