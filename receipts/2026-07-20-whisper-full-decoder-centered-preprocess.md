# Full Whisper decoder trace and corrected centered preprocessing

Witnessed 2026-07-20 on the committed 22,828-byte CC0 human recording
`model/fixtures/whisper-tiny/lingua-libre-book-16k.wav`.  The speaker says
“book”.  No Python, TypeScript, Bash model, foreign model runtime, generated C,
or C-seed growth participates in these executions.

## Why this lane exists

The earlier native frontend used only fully valid 400-sample windows.  That
made 70 mel columns and 35 encoder tokens, but it did not reproduce Whisper's
input geometry.  Running more model layers over a wrong geometry can produce
precise nonsense.  This lane therefore does two things in order:

1. preserves and exposes the exact cost and output of the completed 35-token
   decoder/vocabulary attempt;
2. implements the missing official centered-STFT, right-padding, 3,000-frame,
   and 1,500-token entry path in Form, with a trace-first shape witness.

The upstream reference is OpenAI Whisper's
[`audio.py`](https://github.com/openai/whisper/blob/main/whisper/audio.py):
16 kHz, 400-sample FFT, 160-sample hop, 480,000 samples, 3,000 mel frames,
periodic Hann window, centered `torch.stft`, and removal of its final STFT
column.  Its
[`transcribe.py`](https://github.com/openai/whisper/blob/main/whisper/transcribe.py)
adds 480,000 zero samples for slicing and pads/trims each encoder segment to
3,000 mel frames.

## Exact encoder carrier

The complete row-major 35x384 encoder was recomputed once, streamed at every
boundary, serialized as little-endian f64, and read back exactly.  It was not
rerun for the decoder.

| stage | ms | dispatches | boxed floats | I/O |
|---|---:|---:|---:|---:|
| frontend | 39,592 | 7,473,016,473 | 590,586,368 | 1,052 |
| weights + position | 24,036 | 513,936,197 | 249,064 | 5,642 |
| encoder layer 0 | 60,622 | 2,173,165,551 | 178,464,648 | 0 |
| encoder layer 1 | 76,325 | 2,161,859,096 | 177,127,875 | 0 |
| encoder layer 2 | 74,917 | 2,161,721,271 | 177,165,521 | 0 |
| encoder layer 3 | 69,349 | 2,171,937,763 | 178,724,314 | 0 |
| final layer norm | 77 | 6,854,489 | 517,930 | 0 |
| f64 export + readback | 412 | 89,253,945 | 7,122,710 | 3 |

Whole framebuffer window: 345,330 ms, 16,751,789,856 dispatches,
1,309,958,432 boxed floats, and 6,697 I/O/sense calls.  The carrier is 107,520
bytes, CRC32 `270126731`, and its aggregate readback distance is exactly `0`.

## Complete decoder and one-pass vocabulary scan

Command, run once against that same carrier:

```text
./fkwu --src presence/run-whisper-full-decoder-vocabulary-observed.fk
```

| stage | ms | dispatches | boxed floats | I/O | outcome |
|---|---:|---:|---:|---:|---|
| encoder carrier | 89 | 29,092,285 | 112,836 | 2 | exact 35x384 loaded |
| weights + 4-token prompt | 59 | 944,030 | 74,406 | 8 | complete |
| decoder layer 0 | 19,269 | 1,157,622,542 | 92,540,429 | 0 | complete |
| decoder layer 1 | 17,218 | 1,157,669,975 | 92,543,078 | 0 | complete |
| decoder layer 2 | 17,344 | 1,155,960,192 | 92,337,240 | 0 | complete |
| decoder layer 3 | 18,934 | 1,149,117,877 | 91,520,331 | 0 | complete |
| final layer norm | 11 | 863,609 | 68,760 | 0 | width 384 |
| raw + policy vocabulary | 32,050 | 5,367,764,868 | 475,287,539 | 1,622 | 51,865 rows, one logit pass |
| human truth comparison | 7 | 938,309 | 0 | 4 | mismatch rejected |

Whole framebuffer window: 104,982 ms, 10,020,023,426 dispatches,
844,484,619 boxed floats, and 1,636 I/O/sense calls.  The per-stage dispatch
sum is 10,019,973,687; the 49,739-dispatch remainder is visible observer and
inter-stage work.  Vocabulary scoring is the largest stage by dispatch and
boxed-float cost; decoder layer 0 is the largest transformer stage by both.

Raw and generation-policy scans both chose token 261, text ` w`, logit
`14.686595948257283`.  Raw runner token 2460 had
`14.404344461010284`, margin `0.28225148724699878`.  Policy runner token 272,
text ` b`, had `14.217089328411717`, margin `0.46950661984556596`.
The true candidates were lower: token 1446 ` book` at
`13.513512241697107`, and token 2939 `book` at `11.132028521804601`.

Human truth match is `0`, `native-recognizer` is `0`, and the only admitted
outcome is `decoder-complete-semantic-mismatch-rejected`.  The immutable exact
trace and result live in `observe/whisper-full-decoder-vocabulary-sample.fk`.

## Corrected Form-native input geometry

`audio/whisper-tiny-centered-preprocess.fk` now implements:

- left reflection for the centered 400-sample window (`-1 -> 1`,
  `-200 -> 200`);
- 480,000 right-side zero samples;
- periodic Hann window and existing native mel filterbank over centered
  columns;
- final-column-drop frame count `floor((samples + 480000) / 160)`;
- first 3,000 mel columns, conv1 length 3,000, stride-two conv2 length 1,500;
- the same released row-major four-block encoder entrypoint used by the
  completed 35-token lane.

For the real 11,392-sample utterance, the live shape is: 491,392 padded
samples, 3,071 available mel frames, 3,000 segment frames, 3,000 conv1 tokens,
and 1,500 conv2/encoder tokens.  The first centered mel column contains real
signal and segment column 2,999 is entirely in the zero-padded tail.

Command:

```text
./fkwu --src observe/whisper-tiny-centered-preprocess-live.fk
```

| framebuffer stage | ms | dispatches | boxed floats | I/O | outcome |
|---|---:|---:|---:|---:|---|
| WAV + official shape | 0 | 7,593 | 0 | 4 | 3000-to-1500-ready |
| centered edge windows | 161 | 10,237,308 | 1,117,014 | 4 | reflect-left+zero-tail-observed |

Whole gate: 162 ms, 10,257,148 dispatches, 1,117,014 boxed floats, 8 I/O calls,
outcome `official-input-geometry-ready`.  The lightweight executable band
returns `65535`.

The complete corrected 3,000-column stem plus 1,500-token four-layer encoder
is executable but still unwitnessed.  It is not called “recognition”: the
quadratic attention and current boxed-float representation make it a distinct,
large resource run that needs its own streamed budget and must end in a real
`book` comparison before promotion.

## What the framebuffer taught

The surprising teaching is that the 51,865-row vocabulary scan costs more
dispatches and boxed floats than any decoder block, while layer 0 is the
largest transformer block.  The discomfort was a numerically complete model
returning a confident wrong token.  It turned to gold when the trace separated
model completeness from semantic truth and exposed the missing 35-to-1,500
input-geometry correction as an executable work order rather than allowing the
false token to become a claim.
