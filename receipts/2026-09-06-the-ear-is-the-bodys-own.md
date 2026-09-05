# The ear is the body's own

Every lane of the live transcript now runs as Form on this Mac's own metal. The crossings left
are the microphone and the model files.

**The microphone, natively.** `sense_mic_stream_read` answered empty while the ring behind it
held live samples: the seed tagged its string as an integer (`<< 1`) where every other
read-back speaks `fk_strv`. One line in `runtime/fkwu-uni.c`; the stream answers 2 and 3200
bytes on the next probe. `observe/ear-live-native.fk` reads it 200 ms at a time.

**The weights, off the file.** `form/form-stdlib/ear-npz.fk` reads the numpy archive's central
directory natively (166 tensors, 34 ms) and every tensor is copied on the device from a
whole-file map into an aligned buffer, because a view bound at the tensor's own odd offset reads
shifted halves — conv1 looked plausible, conv2 went to infinity.

**The pass, on the body's kernels.** `form/form-stdlib/ear-msl.fk` emits thirteen kernels (STFT
power, mel, its normalisation, the slaney filter bank, conv1d+GELU with the body's own erf,
layernorm, a half-weight gemm with GELU or the residual folded in and a row offset so k/v land in
their caches, attention for the encoder, a threadgroup attention for decode, sinusoid positions,
token+position embedding, bounded argmax, byte copy). `form/form-stdlib/ear-native.fk` runs
whisper-tiny with activations resident between dispatches and four bytes read back per token;
words come through the reference's own token table and the body's base64.

Witnessed against the reference on a 2 s fixture of this room
(`form/form-stdlib/tests/fixtures/ear-okay-2s.wav`, band `ear-native-band.fk` = 127):

| what | native vs reference |
|---|---|
| log-mel, frames 0 and 100 | equal to six digits |
| encoder output, positions 0 and 50 | equal to three digits (half weights, f32 activations) |
| logits at the first divergent step | within 0.02; the reference's own raw decoder prefers the same token |

| window | encode | per token |
|---|---|---|
| 8 s (Tenc 400) | 94 ms | 14 ms |
| 30 s (Tenc 1500) | 312 ms | 17 ms |

Live on this room, 15 s, 76 frames: live line median 162 ms, p90 185 ms from the last sample to
the glass; heard line 150 ms. A 2 s window alone decodes as non-speech (" >>"); 8 s of context
is the floor for this model, so the live window pads to 8 s.

**The tongues, natively.** `form/form-stdlib/ear-tongue.fk` offers the two tongues not heard
through the body's dense llama lane (llama3.2:1b); `observe/ear-tongue-native.fk` is its own
process off the live path. Portuguese arrived exact ("Ok, vamos tentar!"), Persian weak. It costs
about 14 s a tongue a line because the dense lane prefills one token at a time. Owed, named:
a batched prefill on that lane, a stronger native Persian tongue, tiled encoder gemms.

The surprise: the reference's greedy line and mine parted at the third token and the first
reading was a decoder wound. Teacher forcing showed five of eight next-tokens agreeing; the
raw logits showed the reference's own decoder choosing the token mine chose. The difference was
its decoding options (timestamps on), not the numbers. Discomfort turned gold twice more: an
argmax answering 0 on every step read as a kernel fault until the float dump showed infinity
from conv2 onward and pointed at alignment; and a pipeline handle of 0 hid under a green
"pipes=12" until the carrier's last error named the missing kernel.
