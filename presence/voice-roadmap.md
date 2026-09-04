# Voice Roadmap

This roadmap names the current voice floor and the next target only. How the
floor was reached lives in receipts.

## Present Architecture

The direction is local progress, not enterprise shipping and not a public
product release. It is also not "train a base audio model here." The direction is:

```text
Form/BML control plane
  -> metadata, source provenance, evidence, frame-buffer, scoring, gates
audio.cpp acoustic runtime
  -> ASR, forced alignment, TTS voice-reference clone
```

The route is materialized by `presence/fkwu-production-audio-end-to-end.fk`:
Form writes the command plan and reads back real audio.cpp TTS, ASR,
forced-alignment, translation, return-code, and resource artifacts. The
public-demo voice reference is accepted as borrowed real data for progress; the
source-voice replacement stays visible and does not block the lane.

`presence/fkwu-local-audio-loop.fk` is the practical talk/listen/translate loop
over host stand-ins (macOS `say`, `ffmpeg` 16 kHz mono PCM16 normalization, local
`whisper-cli`, bounded local Ollama translation); `presence/fkwu-audio-task-surface.fk`
lowers it into typed task slots (`source_tts, source_asr, translation, reply_tts,
reply_asr`) and reads back the audio.cpp adapter evidence. Each writes its summary
under `audio-training-runs/current/<lane>/summary` when run; those runs are local
artifacts, never tree truth. The cells and their bands declare their own verdicts.

## The duplex frame grid

One shared ~80 ms frame axis replaces the ASR → LLM → TTS cascade: every channel
holds a value at every frame, agent silence is an explicit pad, user silence is
encoded actual silence, speech boundaries are emitted tokens, and interruption has
no mechanism at all — overlap is representable on the shared axis and yielding is
learned. The teaching lives in `presence/duplex-frame-grid.fk`; its band
`presence/tests/duplex-frame-grid-band.fk` answers 511 (re-run 2026-09-04): over
the same twenty frames, a turn-gated ear loses one of five user words; the grid
loses none.

Consequence for the interactive wiring: seat the microphone → ASR → reply → TTS
loop on the frame grid — both channels always valued, silence a value
end-to-end — rather than as request/response turns. A barge-in then needs no new
machinery, only the yield policy.

## Many voices, one neutral feed

`presence/fkwu-many-voices-live.fk` (door `presence/fkwu-many-voices-live-run.fk`)
runs the grid at room scale: several speakers voice different languages at the
same time on one timeline; each speaker is a channel; whisper `-l auto` witnesses
each language itself; the detected language grounds the local translator's prompt;
the merged feed interleaves the voices in spoken order in the neutral tongue
(English today, one defn to change); one neutral voice speaks the feed back out.
The same voices mixed into ONE channel lose words and languages — so the
architecture is settled: one ear per speaker (the listening fleet's shape), never
one ear on the room.

## Current Gaps

- Seat the ears on the live microphone fleet (one device per speaker); chunked
  streaming so the feed grows while the room still speaks.
- Wire audio.cpp ASR/TTS behind the interactive task slots, on the frame grid.
- Add conversational reply generation between listen and speak.
- Keep the public-demo source visible as borrowed data until it is replaced;
  provision independent speaker verification later if source-voice comparison matters.
- Improve native rate toward realtime.
- Treat forced-aligner confidence zero as a calibrated sidecar, not a pass; add
  listener review before voice promotion.
- The formant oracle (`sema_formant_oracle_live`) hears zero tokens: render a
  phoneme-sequenced dynamic formant carrier with consonant onsets, syllable timing,
  and moving formants, then rerun local Whisper. Promotion cannot begin until
  `heard_token_count >= 1`.
- Fine-tune the hati-translator on witnessed false friends (Air/air is the first
  row of that corpus).
- Keep the bounded current gate; the monolithic audio contract does not return to
  the arena scoring loop.

WER `0` and WER `100` are both suspicious until explained by other fields; a
lane's summary carries those fields.

## Runtime Rule

Manual Form expressions are not "bad shape." If a valid expression can OOM the
compiler or source-runner, the runtime is wrong. The voice pipeline must keep
lane, command, source context, resource use, and frame-buffer state available
for every failure.

## Next Target

An interactive local audio loop backed by audio.cpp for ASR/TTS, using the
borrowed public-demo source until a better source voice is chosen — seated on the
duplex frame grid, one ear per speaker.
