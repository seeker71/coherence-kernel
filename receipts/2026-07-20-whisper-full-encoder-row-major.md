# Full 35-token Whisper encoder through row-major Form affine

Witnessed 2026-07-20 on the committed CC0 human recording
`model/fixtures/whisper-tiny/lingua-libre-book-16k.wav` (22,828 bytes).

## What changed

`wte0-affine-seq` is token-major: a sequence of `S` tokens decodes every f32
weight coefficient `S` times.  The new Form cell
`model/whisper-tiny-native-affine-row-major.fk` decodes a released weight row
once and reuses that row across the whole sequence.  Its multiply/add remains
the original bias-seeded left fold, so the result is bit-identical.

No Python, TypeScript, Bash model, foreign model runtime, generated C, or C-seed
growth participates.  The released carrier bytes remain authoritative.

## Exact projection parity and resource movement

Command:

```text
./fkwu --src observe/whisper-affine-row-major-observation-live.fk
```

One real released `encoder.layers.0.self_attn.q_proj` over two vectors from the
human recording produced exact distance `0` and gate `511`.

| lane | ms | dispatches | boxed floats |
|---|---:|---:|---:|
| original token-major | 490 | 82,765,505 | 7,746,892 |
| exact row-major | 431 | 51,498,148 | 4,168,358 |

That observed run reduced dispatches by 37.8% and boxed floats by 46.2%.
Weight decoding falls from `S*rows*cols` to `rows*cols`; the complete utterance
has `S=35`.

A faster `dot_product` attempt was explicitly rejected: on the same real
projection it moved the result by `5.7534467641506826e-13`.  A zero-seeded
Form dot followed by bias addition was also rejected at
`5.6676798670940443e-13`.  The discomfort exposed that the established affine
is a bias-seeded left fold; preserving it restored exact distance `0`.

## Trace-first full encoder witness

Command, run once and not repeated:

```text
./fkwu --src observe/whisper-full-encoder-row-major-observation-live.fk
```

The first version buffered events until close and was killed with exit 137
inside encoder layer 0.  The observation door was corrected to emit BEGIN,
STAGE-BEGIN, and resource-attributed STAGE lines immediately.  The next run
completed all stages:

| stage | ms | dispatches | boxed floats | outcome |
|---|---:|---:|---:|---|
| frontend | 37,831 | 7,473,016,473 | 590,586,368 | complete, 35 tokens |
| weights + positions | 23,937 | 513,936,197 | 249,064 | complete |
| encoder layer 0 | 47,076 | 2,173,165,551 | 178,464,648 | complete |
| encoder layer 1 | 46,156 | 2,161,859,096 | 177,127,875 | complete |
| encoder layer 2 | 56,742 | 2,161,721,271 | 177,165,521 | complete |
| encoder layer 3 | 53,042 | 2,171,937,763 | 178,724,314 | complete |
| final layer norm | 60 | 6,854,489 | 517,930 | complete |

Whole framebuffer window:

- duration: 264,844 ms
- native dispatches: 16,662,530,594
- boxed floats: 1,302,835,720
- arena cons: 931,936
- I/O/sense dispatches: 6,694
- framebuffer events: 9
- output: 35 tokens x 384 values
- `native-encoder-complete`: 1
- `native-recognizer`: 0

The per-stage dispatch sum is 16,662,490,840; the 39,754-dispatch difference
from the whole window is observation/inter-stage overhead.  Float attribution
sums exactly to the whole-window 1,302,835,720.

The final first/middle/last token fingerprints are pinned in
`observe/whisper-full-encoder-row-major-sample.fk`.  The final live values are
not labels.  Decoder execution, generation, and a truth match over this full
encoder remain owed; no recognition claim is made.

## What the framebuffer taught

The full frontend, not an encoder block, is the dispatch and boxed-float
bottleneck: 7.47 billion dispatches and 590.6 million floats.  Layer 2 was the
longest encoder block at 56.7 seconds.  The next numeric work should turn the
same row-reuse discipline toward conv1/conv2 and the complete decoder, while
keeping the exact left-fold contract visible.
