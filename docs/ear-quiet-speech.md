# Quiet speech and revisable phrases

The live ear uses local Whisper-tiny through Form and its native Metal carrier.
No transcription service, Python process, NeMo sidecar, or new model download is
part of this change. Microphone PCM stays local and unmodified.

`form/form-stdlib/bml/ear-listening.bml` owns the listening policy:

- The first chunk initializes the room estimate. A soft onset requires two
  consecutive chunks above twice that estimate (minimum RMS 8). A strong onset
  still uses four times the floor, minimum 100. An open phrase uses a lower
  continuation threshold. These are **ASR candidates**, not proof of speech.
- The estimate does not learn candidates as background. A single dropout lowers
  it slowly. The previous one second of audio remains available at a soft onset.
- A single window-wide gain targets peak 1024/32768, bounded to 1–16x. It is
  enabled only when the window peak exceeds four times the estimated room RMS.
  Peaks are measured once per incoming chunk and retained with byte lengths.
  Scaling happens inside the existing spectrogram dispatch, before the log floor;
  there is no PCM rewrite, extra GPU dispatch, or host/device readback for gain.
- Every open phrase is decoded afresh. Prior token agreement is no longer forced
  into the next decode. Agreement still informs the display's settled state.
- A decoder end-marker needs a real 200 ms microphone pause and one confirming
  pass. The existing 500 ms fallback and full-window boundary remain.

Glass's room/gate row displays candidate level, applied gain, and `revisable`.
It retains the existing body-local transcript channel and stable language slots.
The listening policy, native encoder, and shader source are watched reload
dependencies; a policy-only edit must also reach the resident ear.
Changing words in the current phrase is intentional; changing its display slot
is not. Frames carry `gainmilli` and `revision` only when those facts were measured.

## Reproduce locally

Run each band through `observe/preflight-stdin-run.fk` first, passing its path on
stdin. Then use `form-run ./fkwu` with these paths:

| Band | Expected | What it proves |
| --- | ---: | --- |
| `form/form-stdlib/tests/ear-listening-band.fk` | 4194303 | Onset, hysteresis, dropout, gain bounds, pre-roll, revisability, close policy, quiet-level display |
| `form/form-stdlib/tests/ear-quiet-quality-band.fk` | 255 | Real local model, attenuation ladder, negative controls, erroneous-prefix correction |
| `form/form-stdlib/tests/ear-native-band.fk` | 32767 | Existing unity-gain mel, encoder, language, text, and doubt pins unchanged |
| `form/form-stdlib/tests/ear-axes-band.fk` | 131071 | Glass decoding, missing-field honesty, gain and revisability display |

The quality band uses only the existing committed two-second fixture. Its original,
4x-quieter, and 16x-quieter versions produced exact open-phrase matches at 3/3
levels with the policy, versus 1/3 without gain. The 64x-quieter version still
failed; its peak is only 14 integer PCM units. These are development fixtures,
**not held-out WER, multilingual validation, or proof of correct final commits**.
Their open-phrase log probabilities can still fall below the unchanged commit
threshold. Adding trailing silence also exposed the tiny model's sensitivity to
window position. Neither limitation is disguised as a passed quality evaluation.

The deterministic noise control can fool Whisper's no-speech probability. It is
not admitted by the energy policy, not boosted, and fails the existing final word
confidence threshold. Silence is independently refused by the model. Gain cannot
improve signal-to-noise ratio or recover information lost below quantization.
Live room recordings and a sealed, multilingual, position-varied speech/noise set
are still needed before claiming broad accuracy gains. The stronger native ASR
model remains a separate implementation task.

## What we learned from open implementations

- [Whisper-Streaming](https://github.com/ufal/whisper_streaming) separates growing
  hypotheses from confirmed prefixes and reprocesses incoming context. Our current
  phrase remains fully revisable, matching the requested interaction rather than
  turning repeated hypotheses into forced decoder inputs. Its successor is
  [SimulStreaming](https://github.com/ufal/SimulStreaming).
- [NVIDIA Nemotron 3.5 ASR](https://huggingface.co/nvidia/nemotron-3.5-asr-streaming-0.6b)
  demonstrates cached streaming encoder context and selectable 80–1120 ms chunks.
  This is a useful native-port direction, not an interchangeable Whisper weight
  file. Its published language list does not include Persian or Indonesian.
  [Parakeet TDT v3](https://huggingface.co/nvidia/parakeet-tdt-0.6b-v3) likewise does
  not cover those two current ear languages. Replacing the ear with either alone
  would remove language coverage.
- [Whisper's reference frontend](https://github.com/openai/whisper/blob/main/whisper/audio.py)
  establishes PCM scaling and log-mel normalization. Unity gain retains the native
  implementation's existing pinned outputs; added gain is explicit input
  conditioning, not a claim that the reference itself performs automatic gain.

Sources observed 2026-09-10. No private microphone text was used in these reports.
