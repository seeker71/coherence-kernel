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

The complete one-layer integration remained green at 40 gates. This stone
proves the body-emitted primitive, its compiled ABI, a real model row, exact
append semantics, and the untouched frontier. It does not yet claim multi-row
attention. The next stone replaces the transient one-row attention binding
with one arena per layer and binds `nrows = pos + 1`.

; witnessed: 2026-07-26 -> real DS4 layer-0 KV row appended by Form-emitted Metal, 40 gates PASS
