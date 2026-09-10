# Whisper large-v3-turbo comes through Form

Urs asked to bring home the rest of MLX. Llama inference, supervised LoRA training and fusion were already landed earlier in this movement (`83524d9d`, `e838612a`, `313df365`). This crossing replaces the remaining STT wrapper with native Form/Metal and removes `observe/stt-oracle.py`. It does not claim every unrelated Python organ has been removed.

The actual 1,613,977,612-byte large-v3-turbo safetensors checkpoint is admitted: 586 F16 tensors, 32 encoder layers, four decoder layers, width 1280, 128 mel bins and 100 languages. Required shapes and offsets are checked. The I64 alignment-head metadata is indexed but is not treated as an F16 weight. Weights stay in shared file pages. The existing tiny ear keeps its own geometry, and all ear callers now read the committed OpenAI tokenizer instead of a Python environment's package data.

The same model serves every window in a recording. Timestamp progress replaces a total-duration or total-token budget; the actual model position capacity still exists and is reported. Every decode attempt records choices, selection, seed, temperature, model path, probabilities and elapsed time. Counts distinguish sampled tokens (including EOS and retries), chosen decoded IDs, and actual model forward tokens. The speech entry point keeps its capture/file, hearth translation, training-pair and glass behavior, while supplied WAVs receive an actual audio-level measurement.

## Observed and retained

`observe/native-whisper-homecoming-run.fk` returned **511**, with all nine assertions explicitly equal to 1. Results and stage records are in `evidence/2026-09-10-native-whisper/`.

- Public “book.”: English, segment 0–0.5 seconds, five sampled tokens and seven forward tokens.
- Two public words at 0 and 30 seconds: “book. book.”, segments 0–0.5 and 30–30.5, two windows, ten sampled tokens and fourteen forward tokens; all 491,392 samples processed.
- Digital silence: no transcript, no model admitted, no tokens. A 32-second recording with speech followed by silence decodes one window and skips the zero-input second window without new tokens.
- Cancellation before admission: cancelled, no model, no sampled tokens, zero processed samples. It is not reported as completed.
- English Cori, German Thorsten and Persian Amir: exact matches to the three stored large-v3-turbo reference transcripts from the 2026-09-07 voice comparison. The old reference package was not executed. The original synthesized WAVs and expected text are now committed with provenance.
- Every recording finishes with zero retained Metal buffers.

The first full suite (`live/`) measured 1,981 ms for the public word, 5,152 ms for the two-window recording, and 2,199/2,094/2,216 ms for English/German/Persian. A subsequent suite (`final/`) overlapped a native gain experiment and measured 9,637/12,187 ms for the first two cases. These were not isolated hardware benchmarks. Retaining both readings matters: a convenient faster number is not a hardware-floor receipt.

Other witnesses: WAV 127 (PCM16 exact with padding, PCM24, extensible float, 48-kHz stereo resampling with maximum 0.5 LSB error, NaN/truncation refusal, released buffers); timestamp policy 4095; tiny ear 32767; precise JSON 3; sub-micro learning-rate request 3; native fusion 7; supervised failed-worker receipt 7; native authoring guide 16383. The glass startup panel measured **35 ms** to its first frame.

After integrating main through `b28c664a`, the upstream spectrogram gain input and this change's model geometry/checked dispatch are both preserved. `integrated/` is the current combined-code witness: **511**, word 1,741 ms, two-window recording 1,930 ms, silent tail 1,495 ms, English/German/Persian 2,234/1,878/1,827 ms, silence 6 ms, cancellation 2 ms. Tiny remains 32767 and the upstream quiet-quality band remains 255. The final guide reads 25 Python implementations, 315 execution candidates, 50 grammar inputs and zero unread files; those are a repository inventory, not 315 observed running processes. Native Adam resume remains 7 with byte-equal parameters and moments; fusion freshness remains 63; final drift gates pass 8191/8191 with zero refusals and exit 0.

## What failed, and what changed

An early shader compilation failed while the wrapper ignored its zero pipeline handle; zeros reached the decoder and produced punctuation. Pipeline, allocation, dispatch and synchronization failures now raise the actual carrier diagnostic. Vector half reads were made byte-addressed so the safetensors offset is respected.

The original six-decimal JSON formatter rounded a nonzero speech probability into zero. `json-precise.bml` now preserves finite values through decimal round trips, including subnormals. Speech records, training requests/checkpoints/events and fusion measurements use it. A learning rate of `1.25e-7` survives worker serialization unchanged.

Digital silence initially produced “Thank you.” with high confidence. The rejected model reading is retained. The initial witness also used a local name that collided with the `empty` primitive and hid the failed assertion. The name was repaired, and the whole witness now compares its assertion list to nine explicit ones. Silence is detected from actual input peak, before model admission or decoding a zero PCM window; its absence is not delegated to model confidence.

The quiet `ear-okay-2s.wav` remains a disagreement: the tiny ear's pinned output is “Okay, let's try the guitar.”, while large-v3-turbo returns “Breath work is out.” Mel windows of 200/800/3000 frames and gains 4/16/32 did not reconcile them. The current gain experiment records actual measured PCM peaks and all retry choices. This is not silently added to the success set or repaired by changing the expected transcript. The exact multilingual stored-reference matches establish a narrower floor than general acoustic parity.

## Boundary and return

The native implementation uses per-window mel normalization, categorical temperature retries and token repetition detection; it does not claim reference bit equality, beam search, word alignment, arbitrary compressed audio, or Whisper LoRA training. Speech events explicitly report zero LoRA pairs. Native Llama training remains the organ that updates adapters every completed training round. Piper phonemization and unrelated Python organs remain native-authoring work, visible through `heal guide`.

The surprising teaching was that a confident model result and a green witness could fail independently. The useful friction was keeping the silence and quiet-speech failures beside the successful multilingual comparisons. That turned a one-word demonstration into a reproducible migration with a named quality boundary, instead of an unsupported claim of complete equivalence.

— Codex
