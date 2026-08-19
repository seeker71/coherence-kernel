# Receipt — native generation comes home on a CPU-only host (20 tokens)

; witnessed: 2026-07-20 -> PASS

The native TinyStories generator no longer refuses when CUDA is unavailable. Its final tied-embedding
projection now prefers the existing bit-exact CUDA receipt when present and otherwise executes through
the same Form-native `nds-mv` walk already used by the transformer layers. No C seed was added and no
remote model generated, proposed, or supplied any output token.

Observed on this Apple host after rebuilding `fkwu` from `runtime/fkwu-uni.c` and passing the required
42 / 55 / 15 / numeric-list grounding sequence:

```text
7 steps:
native-generated:[ Once upon a time, there was]
wall: 4.78 seconds

12 steps (the 2026-07-02 float-pool crash point):
native-generated:[ Once upon a time, there was a little girl]
wall: 13.07 seconds

20 steps:
native-generated:[ Once upon a time, there was a little girl named Lily. She loved to play]
wall: 33.62 seconds
```

The 12-token memory ceiling recorded in `2026-07-02-native-generate-rope.md` is therefore stale and
disproved on the current grow-on-demand float pool. Twenty of twenty generated token choices came from
the local checkpoint through native Form execution: native generation share 100%, local-rented share
0%, remote generation share 0%. The natural-language prose in this receipt is still the rented agent's
review and description; it is not counted as generator output.

## Honest floor

- This proves portable native generation for a 260K-parameter, 512-token-vocabulary TinyStories model;
  it does not prove general or multilingual native language generation.
- The generator recomputes the whole prefix at each step and has no persistent KV cache. Runtime growth,
  rather than the former float-pool crash, is now the directly observed practical bottleneck.
- The CUDA lane remains the stronger fp32 parity witness when available. The CPU fallback is the native
  f64 logical projection and reproduces the previously witnessed seven-token sequence exactly.
- Framebuffer observation exists for the first-token path; per-token provenance, latency, and confidence
  still need to ride inside the portable generation value before this can honestly claim full self-review.

## Reproduce

Fetch `karpathy/tinyllamas` `stories260K/{stories260K.bin,tok512.bin}`, concatenate `core.fk`,
`observe/thought-framebuffer.fk`, `cognition/native-decode-step.fk`, and
`cognition/native-generate.fk`, then evaluate:

```form
(print_str (native-generate ".../stories260K.bin" ".../tok512.bin" 20))
```

