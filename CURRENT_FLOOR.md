# Current Floor

Date: 2026-07-05

This file is the current release floor for this worktree. Receipts preserve the
history, but the claims below are only the state that is present now.

## Grounding

The checkout witness is still the C-seeded `fkwu` runner. It was rebuilt before
this floor was written.

```text
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk -> 42
./fkwu --src bootstrap/ground-recursive.fk 10 -> 55
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk -> 15
native-vs-rented-check -> 11111
```

The C file is a checkout witness and shrink target. New runtime meaning belongs
in Form/native-walker cells, not in a larger C seed.

## Repo Floor

The repo floor is an organic intelligence floor, not a voice-only floor. The
present organs include:

- Form-native choice receipts for success, fail, and silence:
  `form/form-stdlib/choice-receipt.fk`.
- Channel/interface/protocol choice floor:
  `form/form-stdlib/channel-protocol-choice-floor.fk`.
- Sovereign allow/stop/witness/re-entry boundary protocol:
  `form/form-stdlib/sovereign-boundary-protocol.fk`.
- Host OS membrane and C-seed shrink law:
  `form/form-stdlib/host-os-membrane.fk`.
- Satsang witnessing circle:
  `form/form-stdlib/satsang.fk`.
- Reception consent policy:
  `form/form-stdlib/reception-consent.fk`.
- Local and remote oracle teacher catalog:
  `form/form-stdlib/oracle-catalog.fk`.
- Trust and vitality organs across `trust-row`, `trust-decay`,
  `trust-weighted-colearning`, `proof-trust`, `model-vitality`,
  `skill-vitality`, and `observe/sovereignty-guide.fk`.

These organs are the repo north-star floor: native choice, fail, cut, stop,
timeout/nothing, satsang, consent, minimal kernel, host membrane, observability,
trust, sovereignty, vitality, play, and wonder. Audio is one current organ using
that floor.

Focused direct-source witnesses observed in this reground:

```text
host-os-membrane-band -> 8191
reception-consent-band -> 255
satsang-band -> 127
kernel source intake framebuffer policy witness -> 16383
```

## Audio Floor

The current real audio shape is local progress using borrowed acoustic runtime
plus a Form/BML control plane. This is not enterprise shipping, not a commercial
release process, and not a public product lane. The current source voice is the
audio.cpp public-demo voice package, used as real borrowed data so the lane can
work end-to-end now. Replacing it with a personal voice package is a later
source-swap task, not a pipeline blocker.

Form/BML owns:

- metadata
- source provenance
- evidence caches and sidecars
- WER/native-rate/confidence reporting
- champion/challenger scoring
- promotion gates
- OOM diagnostics and repeat prevention

audio.cpp owns the acoustic runtime used by the current no-stand-in lane:

- ASR: `qwen3_asr`
- forced alignment: `qwen3_forced_aligner`
- TTS: `qwen3_tts`
- voice mode: `voice_ref_clone`

There is now also a functional local stand-in loop for immediate end-to-end
talk/listen/translate testing:

```text
presence/fkwu-local-audio-loop.fk
presence/tests/fkwu-local-audio-loop-band.fk -> 16383
audio-training-runs/current/fkwu-local-audio-loop/summary
status: functional_local_talk_listen_translate_loop
source_text: Open speech flows.
source_transcript: Open speech flows.
source_wer_pct: 0
translation_oracle: ollama:llama3.2:1b
translation_text: Offene Rede fließt.
reply_transcript: Offene Rede fließt.
reply_wer_pct: 0
score: 16383
expected_score: 16383
```

This loop is Form-owned orchestration over local stand-ins: `say` for temporary
TTS, `ffmpeg` for wav normalization, `whisper-cli` for ASR, and bounded local
Ollama HTTP for translation. It is functional now, but it is not the real local
personal voice lane and does not train or promote a base model.

The loop now has a typed task surface and direct sidecar readback:

```text
presence/fkwu-audio-task-surface.fk
presence/tests/fkwu-audio-task-surface-band.fk -> 4095
audio-training-runs/current/fkwu-audio-task-surface/summary
status: local_loop_functional_audio_cpp_adapters_observed_not_swapped
score: 65534
expected_score: 65535
task_slots: source_tts,source_asr,translation,reply_tts,reply_asr
audio_cpp_asr_observed: 1
audio_cpp_tts_observed: 1
production_authority: 0
personal_lane_authority: 0
next_gap: rerun audio.cpp ASR/TTS support packets or inspect their command/resource artifacts
```

That task surface is now secondary. It means the local stand-in loop and
adapter evidence are observable, while the no-stand-in lane below is the current
real audio source of truth.

The real no-stand-in local progress audio lane is now complete:

```text
presence/fkwu-production-audio-end-to-end.fk
presence/tests/fkwu-production-audio-end-to-end-band.fk -> 4095
audio-training-runs/current/fkwu-production-audio-end-to-end/summary
use_case: local_progress_not_enterprise_shipping
status: local_progress_audio_end_to_end_complete
blocker: none
score: 131071
expected_score: 131071
executed_audio_cpp: 1
cli_ready: 1
models_ready: 1
package_files_present: 1
package_borrowed_demo_authorized: 1
package_progress_authorized: 1
package_personal_use_authorized: 0
source_mode: borrowed_public_demo
package_scope: public-demo
target_text: Sema audio path is alive.
tts_text: Seyma. Audio. Path. is alive.
transcript: Sema, audio, path is alive.
wer_pct: 0
translation_model: llama3.2:3b
translation_text: Sema, Audio, Pfad ist lebendig.
```

This lane writes the derived artifacts it owns: context, diagnostic, target
text, phonetic TTS text, reference text, runnable TTS/ASR/align/translate
commands, generated WAV, ASR transcript, forced-alignment words, translation
request, translation text, return codes, and resource logs. The current package
is no longer treated as a blocker for progress; it is explicitly recorded as
`borrowed_public_demo`.

## Latest Stored Arena Snapshot

The latest stored arena files report cycle `10`, champion
`multiseed_form_sweep`, nearest challenger `metal_observed_sweep_bridge`, and
`investigation_required: 1`.

The arena is not currently alive in this worktree: no `fkwu`/audio-arena process
matched, the stored `arena.pid` and `launcher.pid` values were not live
processes, and launchd had no `com.sema.audio-arena.5586` service.

The scoreboard still contains stale expected values for at least the old
monolithic audio gate and old TTS support expected value. The current real
end-to-end audio result should be read from
`audio-training-runs/current/fkwu-production-audio-end-to-end/summary`, not from
the stale scoreboard row.

## Live Lane WER Values

These values are stored in `audio-training-runs/current/scoreboard.md`.

| lane | WER | current meaning |
|---|---:|---|
| `macos_sema_teacher_acoustic_live` | 0 | Teacher/acoustic support row, confidence 96; not personal-lane authority. |
| `macos_sema_voice_teacher_live` | 100 | Failing voice teacher row; intelligibility 0. |
| `sema_formant_oracle_live` | 100 | Investigated miss; heard 0/3 target tokens. |
| `macos_roundtrip_live` | 100 | Failing roundtrip row; native rate 0. |
| `audio_cpp_current_tts_live` | 0 | Intelligible support row over public demo voice, native rate 6, and personal source replacement still open. |

Boundary values such as WER `0`, WER `100`, unknown, or sentinel confidence are
investigation signals unless another measured field explains them.

## Real-World Gaps

- The no-stand-in local progress lane now works end-to-end over audio.cpp:
  TTS, ASR, forced alignment, and local translation all return `0`, WER is `0`,
  and the score is `131071/131071`.
- The source voice is still borrowed public-demo data. That is acceptable for
  progress and must remain visible as `source_mode: borrowed_public_demo`; swap
  it later when we want the lane to sound like a chosen personal voice.
- The formant oracle is not close lexically: target tokens are `3`, heard tokens
  are `0`, overlap is `0/3`, and the next probe is a token-bearing dynamic
  formant carrier.
- Speaker verification is not needed to keep making progress, but it is still
  absent if later comparison against a chosen source voice matters.
- The last measured audio.cpp TTS support native rate is `6`; the current
  realtime gap is `94`.
- The current bounded gate reads sidecars and receipts. The new no-stand-in
  lane can rerun the audio.cpp binary directly when invoked.
- The OOM-prone monolithic audio lane has been replaced by a bounded current
  gate.
- The local talk/listen/translate loop works with host stand-ins, and the
  separate no-stand-in audio.cpp lane works for a fixed utterance. The remaining
  wiring gap is to put audio.cpp ASR/TTS behind the interactive task slots, add
  real microphone capture, add conversational reply generation, and keep
  realtime/native-rate diagnostics.
- During this north-star reground, broad direct `fkwu --src` BML/prelude
  bundles reached roughly 18-20 GB RSS. The root cause was a C checkout-witness
  parser progress bug: desynchronized BML-style call syntax could leave a naked
  top-level `)` at the same source byte and repeatedly allocate parsed cursor
  nodes. The local runtime repair stops this case with byte/line/column/context
  diagnostics instead of OOM.
- The deeper source-runner/compiler obligation remains: manually authored wide
  expressions must compile/evaluate within a resource envelope or fail with a
  precise diagnostic and framebuffer/source context, never unbounded RSS.
- The repo-wide north-star organs are present in focused cells, but not every
  runtime choice, host crossing, arena lane, and training loop is forced through
  choice receipts, sovereign boundary receipts, trust/vitality policy, and
  oracle provenance yet.
- Sema still does not have a native generative mind with real open weights loaded
  as recipe-data through the body.

## Do Not Claim

- Do not claim the current source voice is personal while it is public-demo.
- Do not claim native neural ASR/TTS ownership.
- Do not claim the base STT/TTS models were trained here.
- Do not treat WER `0` as sufficient without source provenance, speaker evidence,
  alignment confidence, native-rate, and listener-review gates.
- Do not treat WER `100` as normal. It must name the observed failure axis and
  the next measurable repair.
