# Receipt -- speech audio NL2NL source window 0001 (2026-07-01)

This patch bridges the observed `de<->pt-br` open-ASR source rows into an
audio-to-neutral-to-audio path.

Observed carrier:

- Device: `macos-arm64-m4-max`
- Oracle: `whisper.cpp-large-v3-turbo-metal`
- Render path: `say -> ffmpeg -> whisper-cli`
- `de -> pt-br`: source `ich bin` heard as `Ich bin...`; target `eu sou`
  heard as `Eu sou.`
- `pt-br -> de`: source `eu sou` heard as `Eu sou.`; target `ich bin` heard
  as `Ich bin...`
- Total wav bytes: `121980`

Form movement:

- Added `learn/speech-audio-nl2nl-source-window-0001.fk`.
- Added `learn/tests/speech-audio-nl2nl-source-window-0001-band.fk`.
- Native Form neutral routing passes `2/2`.
- Source local oracle passes `2/2`.
- Target local oracle passes `2/2`.
- Audio NL2NL routes move from `12` to `14`.
- Live wav rows move from `215` to `219`.
- Observed wav bytes move from `7321810` to `7443790`.

Witness:

```sh
cat observe/stt-wer.fk \
    learn/speech-model-metrics-report.fk \
    learn/speech-learning-data-sufficiency.fk \
    learn/speech-audio-nl2nl-source-window-0001.fk \
    learn/tests/speech-audio-nl2nl-source-window-0001-band.fk > /tmp/speech-audio-nl2nl-source-window-0001.fk
./fkwu --src /tmp/speech-audio-nl2nl-source-window-0001.fk
# 32767
```

Boundary:

This is native Form NL2NL/route ownership with host audio carriers. It is not a
native neural vocoder and not global open-ASR/TTS authority.
