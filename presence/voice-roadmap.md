# Voice Current Roadmap

Date: 2026-07-05

This roadmap names the current voice floor only. Historical exploration remains
in receipts and `RELEASE_HISTORY.md`.

## Present Architecture

The current direction is local progress, not enterprise shipping and not a
public product release. It is also not "train a base audio model here." The
current direction is:

```text
Form/BML control plane
  -> metadata, source provenance, evidence, frame-buffer, scoring, gates
audio.cpp acoustic runtime
  -> ASR, forced alignment, TTS voice-reference clone
```

The route is materialized by `presence/fkwu-production-audio-end-to-end.fk`:
Form writes the command plan and reads back real audio.cpp TTS, ASR,
forced-alignment, translation, return-code, and resource artifacts.

## Functional Local Loop

`presence/fkwu-local-audio-loop.fk` now gives the practical end-to-end loop:

```text
Form task surface
  -> macOS say stand-in TTS
  -> ffmpeg 16 kHz mono PCM16 wav normalization
  -> local whisper-cli ASR
  -> bounded local Ollama translation
  -> macOS say reply TTS
  -> local whisper-cli reply ASR
```

Current observed run:

```text
summary: audio-training-runs/current/fkwu-local-audio-loop/summary
status: functional_local_talk_listen_translate_loop
source_text: Open speech flows.
source_wer_pct: 0
translation_oracle: ollama:llama3.2:1b
translation_text: Offene Rede fließt.
reply_wer_pct: 0
promotion_authority: 0
base_model_training_authority: 0
```

This closes the immediate talk/listen/transcribe/translate loop with local
oracles and host stand-ins. The no-stand-in audio.cpp lane below closes the
fixed-utterance borrowed-source path. The remaining work is to wire audio.cpp
behind the interactive task slots, add real microphone capture, generate replies,
and keep realtime/native-rate diagnostics.

`presence/fkwu-audio-task-surface.fk` now lowers that loop into typed task
slots and reads back the current audio.cpp adapter evidence:

```text
task_slots: source_tts,source_asr,translation,reply_tts,reply_asr
status: local_loop_functional_audio_cpp_adapters_observed_not_swapped
score: 65534/65535
audio_cpp_asr_observed: 1
audio_cpp_tts_observed: 1
next_gap: rerun audio.cpp ASR/TTS support packets or inspect their command/resource artifacts
```

The no-stand-in lane now exists beside that task surface:

```text
presence/fkwu-production-audio-end-to-end.fk
summary: audio-training-runs/current/fkwu-production-audio-end-to-end/summary
use_case: local_progress_not_enterprise_shipping
status: local_progress_audio_end_to_end_complete
score: 131071/131071
executed_audio_cpp: 1
source_mode: borrowed_public_demo
target_text: Sema audio path is alive.
tts_text: Seyma. Audio. Path. is alive.
transcript: Sema, audio, path is alive.
wer_pct: 0
translation_model: llama3.2:3b
translation_text: Sema, Audio, Pfad ist lebendig.
package_borrowed_demo_authorized: 1
package_progress_authorized: 1
package_personal_use_authorized: 0
package_scope: public-demo
```

It writes and runs the audio.cpp TTS, ASR, forced-alignment, and translation
commands. The public-demo source is accepted as borrowed real data for progress;
the later source-voice replacement remains visible but does not block the lane.

## Current Live Voice Rows

| lane | WER | status |
|---|---:|---|
| `macos_sema_teacher_acoustic_live` | 0 | Teacher/acoustic support only, confidence 96. |
| `macos_sema_voice_teacher_live` | 100 | Failing row, intelligibility 0. |
| `sema_formant_oracle_live` | 100 | Investigated miss, heard tokens 0/3. |
| `macos_roundtrip_live` | 100 | Failing roundtrip, native rate 0. |
| `audio_cpp_current_tts_live` | 0 | Intelligible support over public-demo voice. |

WER `0` and WER `100` are both suspicious until explained by other fields. The
current explanations are below.

## Formant Oracle WER 100

This is not close in the way that matters for speech recognition:

```text
target_text: Open speech flows.
heard_text:
target_token_count: 3
heard_token_count: 0
target_overlap_count: 0
diagnostic_kind: empty_oracle_transcript
primary_improvement: token_bearing_acoustic_carrier
promotion_authority: 0
```

The next probe is explicit: render a phoneme-sequenced dynamic formant carrier
for `open,speech,flows`, add consonant onsets, syllable timing, and moving
formants, then rerun local Whisper. Promotion cannot even begin until
`heard_token_count >= 1`, and the real target is overlap `3/3` with WER within
gate.

## audio.cpp TTS WER 0

This row is useful because it is listenable and helped close the borrowed-source
path:

```text
target_text: Sema audio path is alive.
heard_text: Sema, audio, path is alive.
wer: 0
voice_ref_source: public_demo_fallback
native_rate_pct: 6
forced_aligner_words_present_zero_confidence: present
listener_review_ready: 0
promotion_authority: 0
```

The next real-lane step is not more base-model training. It is interactive
wiring: microphone -> audio.cpp ASR -> text/reply -> audio.cpp TTS -> ASR/align
readback. Source-voice replacement can happen later.

## Current Gaps

- Wire audio.cpp ASR/TTS behind the interactive task slots.
- Add real microphone capture.
- Add conversational reply generation between listen and speak.
- Keep the public-demo source visible as borrowed data until it is replaced.
- Provision independent speaker verification later if source-voice comparison
  matters.
- Improve native rate toward realtime; current gap is `94`.
- Treat forced-aligner confidence zero as a calibrated sidecar, not a pass.
- Add listener review before voice promotion.
- Keep the bounded current gate; do not put the monolithic audio contract back
  into the arena scoring loop.

## Runtime Rule

Manual Form expressions are not "bad shape." If a valid expression can OOM the
compiler or source-runner, the runtime is wrong. The voice pipeline must keep
lane, command, source context, resource use, and frame-buffer state available
for every failure.

## Next Release Target

The next release target is an interactive local audio loop backed by audio.cpp
for ASR/TTS, using the borrowed public-demo source until a better source voice
is chosen.
