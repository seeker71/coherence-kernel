# Receipt -- speech audio NL2NL source window 0002 (2026-07-01)

This patch bridges a second observed source-window pair, `hi<->it`, into the
audio-to-neutral-to-audio path.

Observed carrier:

- Device: `macos-arm64-m4-max`
- Oracle: `whisper.cpp-large-v3-turbo-metal`
- Render path: `say -> ffmpeg -> whisper-cli`
- `hi -> it`: source `namaste` heard as `Namaste.`; target `ciao` heard as
  `Ciao!`
- `it -> hi`: source `ciao` heard as `Ciao!`; target `namaste` heard as
  `Namaste.`
- Total wav bytes: `90664`

Form movement:

- Added `learn/speech-audio-nl2nl-source-window-0002.fk`.
- Added `learn/tests/speech-audio-nl2nl-source-window-0002-band.fk`.
- Native Form neutral routing passes `2/2`.
- Source local oracle passes `2/2`.
- Target local oracle passes `2/2`.
- Audio NL2NL routes move from `14` to `16`.
- Live wav rows move from `219` to `223`.
- Observed wav bytes move from `7443790` to `7534454`.

Witness:

```sh
cat observe/stt-wer.fk \
    learn/speech-model-metrics-report.fk \
    learn/speech-learning-data-sufficiency.fk \
    learn/speech-audio-nl2nl-source-window-0002.fk \
    learn/tests/speech-audio-nl2nl-source-window-0002-band.fk > /tmp/speech-audio-nl2nl-source-window-0002.fk
./fkwu --src /tmp/speech-audio-nl2nl-source-window-0002.fk
# 32767
```

Boundary:

This is native Form NL2NL/route ownership with host audio carriers. It is not a
native neural vocoder and not global open-ASR/TTS authority.
