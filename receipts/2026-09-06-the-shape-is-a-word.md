# The shape is a word

The gap named this morning: whisper-tiny's 8 s window encoded in 24 to 41 ms and a token cost about
5 ms, against a floor of 0.22 ms for one pass over 74 MB at 330 GB/s plus the arithmetic. Measured
before anything was touched, with two sibling `fkwu` processes sharing this GPU: an encode 21 ms
wall of which 19 on the GPU, a token 8.0 ms of which 6.6 on the GPU, 56 dispatches. The Form side
was a tenth of it; the kernels were the cost.

**Where the GPU time sat.** Thirteen layernorms a token, each one thread walking 384 floats three
times. One thread walking 64,000 mel cells twice for the max and the clamp. One thread walking 51,865
logits for the argmax. A sine and a cosine per DFT term, 64 million of each per window. Attention
with a 17 KB threadgroup partial per (head, query) — one 64-thread group per core at a time, no
latency to hide behind. Fifty-six launches a token.

**What stands now**, `form/form-stdlib/ear-msl.fk` and `ear-native.fk`, `ear-native-band.fk` = 255:

- `ear_gemm_f`: layernorm fused into the gemm that follows it, mean and rsqrt once per row in
  threadgroup memory, applied as the x tile is staged; three weights side by side so q/k/v (and the
  cross k/v with ln_post) are one dispatch; the bias, the GELU or the residual folded in; the row
  offset applies to the second and third outputs only. 32x32 tiles folded on `simdgroup_float8x8`
  multiply-accumulate over K steps of 32, 8 KB of threadgroup memory so several tiles share a core.
  The two convolutions are the same kernel with `x` staged as im2col on the fly (`cstride`).
- `ear_mv`: the same fold for one row — 256 threads stage the normalised row, each simdgroup owns one
  output with 32 lanes striding K by `half4` and `simd_sum`; the logits (51,865 rows) run through it.
- `ear_spec`: the spectrogram as one threadgroup per frame, the twiddle table built once per group in
  threadgroup memory, 201 threads own the bins, 80 the mels, thread 0 the frame's max;
  `ear_melnorm` folds the frame maxima per group and normalises. Two dispatches where there were
  three, mel[0][0..5] unchanged to the last digit.
- `ear_attn`: one threadgroup per (head, 16 queries), a thread owns one query and one of 16 key
  slices in registers, the slices merged with `simd_shuffle_xor` — no partials in threadgroup memory.
  1.4 ms a layer to 0.18. `ear_attn_dec`: max and sum by `simd_max`/`simd_sum`, four key slices for the
  values. 61 µs to 17.
- `ear_argmax`: one threadgroup of 1024 over both windows, ties to the lowest index.
- **The shape word.** The shapes that change between calls — the encoder's rows, the decoder's
  position and its row count, the frame count — live in a five-word device buffer written by the
  kernel that first knows them (the spectrogram writes Te and Tf, the embedding writes pos and T).
  Every layer binding is then the same bytes each call: built once at open into tables
  (`enw-enc-binds`, `enw-cross-binds`, `enw-dec-binds`, `enw-pre-binds`, `enw-misc-binds`), a dispatch
  is one `metal_enqueue` of a stored string with a computed group count. An encode is 2+3+4x5+4
  dispatches, a token 1+4x8+2 = 35, one sync each; one binding is built per call (the spectrogram's
  L and Tf, the embedding's id and pos).

| | before | after | GPU part |
|---|---|---|---|
| 8 s encode | 21 ms | 3.3 ms (min of runs; 3.3 to 5.0 under the siblings) | 2.9 ms |
| token | 8.0 ms, 56 dispatches | 0.6 ms, 35 dispatches | 0.35 ms |
| prefill of 6 rows | 7 ms | 3 ms | |
| detected line (tongue + 9 tokens) | 84 ms | 12 ms | |

Per encode now (minimum over runs, GPU): spectrogram 0.36, conv1 0.04, conv2 0.09, per layer
ln+qkv 0.10, attention 0.18, out 0.03, ln+mlp1 0.13, mlp2 0.10, cross k/v 0.07 a layer.

**Proof.** mel[0][0..5] on the fixture padded to 8 s: -0.138096 0.33575 0.339695 0.272336 0.224791
0.152085 (the reference's 0.224792 in the fifth). enc[0][0..5] on the 30 s window, the reference's own
padding: 0.170423 0.018257 -0.020125 0.121259 0.113597 -0.053409 against 0.170423 0.018257 -0.020125
0.121258 0.113597 -0.05341. Every stage printed side by side against the previous kernels from HEAD
in a second process (conv, q/k/v, attention, out, mlp, the four layers, ln_post, the cross caches,
the decoder's logits and dx at steps 0 to 4): equal to five digits throughout. The 8 s line, the line
from tick 300 behind 6 s of silence, and the 30 s line are the previous pass's lines word for word.
Preflight of the band: parens balanced, 0 unresolved.

**Measurement under company.** `native-model-dual-resident-live-run.fk` and
`form-cli-peer-contribution-live.fk` were resident on this GPU throughout. A command buffer's
`GPUEndTime - GPUStartTime` spans whatever the device ran in between, so every number here moved by
up to 2x between runs; the spectrogram read 0.36 ms in the first run and 0.9 in the last with no
change to its text. The minimum is reported as the kernel's own.

**Still open.** The gemms fold at ~3 TFLOPS: the K loop is bound by staging latency (two barriers a
step), and a register prefetch of the next step, or half tiles, is the next room. The spectrogram
could be three gemms (frames x twiddles, power x filter bank) on the same tiled kernel, ~0.3 ms. The
host side of a token is ~0.25 ms for 35 enqueues — the carrier's setBuffer walk, not Form. The
attention over rows reads K and V from cache per query; staging blocks in threadgroup memory would
halve its traffic. None of these change what the band pins.

The surprise: fusing q/k/v into one dispatch broke the decoder silently — the row offset that puts
k and v into their caches at `pos` was also putting q at `pos`, while attention read q at row 0; step
0 was right because pos was 0, and the language came back "en" from a decoder whose every later step
was reading a stale query. The band's empty line was the only voice. Discomfort turned gold: the
enc[0] the task handed as reference did not match the 8 s fixture, and the old kernels disagreed with
it too; rather than tune toward it, the padding was asked — it is the 30 s window, and there the new
pass matches to five digits.
