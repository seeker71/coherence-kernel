# Native Whisper

`observe/stt-oracle-run.fk` uses Form/Metal large-v3-turbo for both handed WAVs and captured speech. The model, 586 F16 tensors, 32 encoder layers, four decoder layers, 128 mel bins, and 100 language tokens come from the actual registered checkpoint. Tensor shapes, dtype and alignment are checked before use. Safetensors spans remain shared file mappings; model weights are not copied into a converted checkpoint. Activations and attention caches stay resident between dispatches. The small native ear retains its own geometry and behavior witnesses.

## Run

`form-run ./fkwu observe/native-whisper-run.fk` reads five stdin lines: WAV path, optional language code, optional stream path, optional cancellation-file path, optional JSONL event path. The parent directories must exist. Blank language means detection. A nonempty cancellation file stops at the next window or decoder-token boundary. JSON on stdout carries the transcript, segments, actual status and counts. An incomplete or cancelled direct run exits nonzero after printing its result.

`form-run ./fkwu observe/stt-oracle-run.fk` keeps its three-line contract: seconds, optional language, optional handed WAV. A handed WAV avoids microphone capture. It writes a precise JSON sidecar, stage events and streams under `.form-lora-voice-native/stt/`, measures the actual handed audio, publishes the actual status to the glass, and offers completed English/Persian text to a standing hearth for translation. A missing hearth produces no fabricated translation or training pair.

## Recording and token flow

RIFF WAVE PCM16/24/32 and IEEE float32 are admitted, including the supported extensible subtypes. Channels are mixed and rates from 8–192 kHz are resampled in native Metal. Unknown RIFF chunks and odd padding are traversed; invalid geometry, truncated chunks and nonfinite samples refuse. PCM16 at 16 kHz is exact; other inputs reach the ear as PCM16. The resampler uses windowed sinc interpolation; it does not claim bit equality with an external audio converter.

One model serves the whole recording. There is no recording-duration or total-token cap. The model has 448 text positions per window; EOS, timestamp progress, repetition, confidence and cancellation choose the next action. Unclosed text does not authorize advancing past its audio. Stalled progress returns an incomplete status. Tokens in `.stream.attempt` are provisional and reset on retry; `.stream` contains accepted timestamp segments. Both are byte streams and a partial UTF-8 token may not yet be a complete character.

Each attempt reports temperature, seed, finish reason, elapsed time, actual probabilities, choices and selection. `sampled_tokens` includes EOS and discarded retries. `model_forward_tokens` includes the three prompt tokens and every generated token actually fed back; EOS is not fed back. `selected_decoded_tokens` counts the selected attempt's text and timestamp IDs, including a selected attempt later skipped as silence. These are different measurements. Whisper inference uses no LoRA pair; events say `lora_pairs=0`. Llama training rounds update their adapters through the separate native training worker.

Digital silence can produce a confident neural hallucination. The input's actual peak now decides whether there is any signal to decode: a recording with exactly zero mixed samples admits no model. A zero PCM window after speech runs no encoder or decoder. Quiet nonzero audio is not silently classified as zero by a dBFS threshold. Level measurement uses a scaled RMS calculation to avoid squaring tiny amplitudes into zero.

## Witnesses and limits

`form-run ./fkwu observe/native-whisper-homecoming-run.fk` creates a fresh evidence directory under `.hearth` (or takes a new directory on stdin). The 511 witness covers the public word, two words across the 30-second boundary, digital silence, silence after speech, cancellation before admission, exact English/German/Persian stored-reference transcripts, and released buffers. Every recording has its own result JSON and stage events. Three multilingual WAVs and their stored reference texts are committed under `model/fixtures/whisper-native/`; their provenance is recorded there.

Policy band: `form/form-stdlib/tests/native-whisper-policy-band.fk` (4095). Audio band: `form/form-stdlib/tests/native-wav-band.fk` (127). The tiny ear remains 32767. Numeric serialization is witnessed by `json-precise-band.fk` (3), including subnormals; small probabilities and learning rates are not rounded through the six-decimal display formatter.

The former wrapper used a general reference decoder. This native route currently uses greedy decoding followed by categorical temperature retries, token repetition detection instead of zlib compression ratio, per-window mel normalization, and one encoding for language detection and transcription. It does not offer beam search, word-alignment timestamps, previous-window prompting, compressed audio inputs, or Whisper adapter training. These are not claimed by the result schema. Model and audio arithmetic are not claimed bit-identical to MLX.

The existing quiet `ear-okay-2s.wav` fixture remains a disagreement: tiny's pinned text is “Okay, let's try the guitar.”; large-v3-turbo returned “Breath work is out.” Changing mel padding and testing gains 4/16/32 did not reconcile them. The stored multilingual large-model references pass, but this quiet fixture is not a transcription-parity success. Preserve the failed observation when developing a broader acoustic quality witness.

STT latency is dominated by the encoder in the measured runs. The events expose GPU time and dispatches alongside wall time. Those readings do not establish a hardware floor; concurrent work, admission, the full 1,500-position encoder, preprocessing and kernel choices still affect latency.

`heal guide` names the native STT, training and fusion witnesses and detects MLX import candidates. Cached checkpoint names and historical provenance retain their original publisher names. Piper phonemization and unrelated Python organs remain separate native-authoring work; eliminating the MLX runtime does not claim that the entire repository is Python-free.
