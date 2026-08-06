# Speech native neural pair window 0021

This executes the next selected reciprocal native neural pair window:
`es<->id`.

The window uses Sanskrit baseline meaning `301`
(`sarve-bhavantu-sukhinah`) and trains four closed lanes:

- `es -> id`
- `id -> es`
- `es -> es`
- `id -> id`

Before adding the Form cell, I ran a real local Mac render/oracle probe:

- Spanish voice: `Flo (Spanish (Mexico))`
- Spanish truth: `que todos sean felices`
- Spanish local oracle heard: `Que todos seamos felices`
- Spanish WER admitted: `25`
- Indonesian voice: `Damayanti`
- Indonesian truth: `semoga semua bahagia`
- Indonesian local oracle heard: `Semoga semua bahagia`
- Indonesian WER admitted: `0`
- Local oracle: `whisper.cpp-large-v3-turbo` on Apple M4 Max Metal
- Observed wav bytes: `108418`

Measured pair-window result:

- Cumulative neural pairs: `21`
- Directed neural routes: `42`
- Native neural params: `21`
- Neural epochs: `21`
- Form NL rate: `100`
- Form audio rate: `100`
- Local-oracle rows: `2/2`
- Route shift: `0 -> 100`

Witness:

```sh
cat observe/stt-wer.fk \
    observe/asr-prompt-id.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/multilocale-nl-audio-pipeline.fk \
    learn/speech-native-neural-bootstrap.fk \
    learn/speech-native-neural-pair-window-0021.fk \
    learn/tests/speech-native-neural-pair-window-0021-band.fk > /tmp/speech-native-neural-pair-window-0021.fk
./fkwu --src /tmp/speech-native-neural-pair-window-0021.fk
```

Result: `32767`.

Boundary: this is native neural micro-pair training for a closed
locale-neutral Form baseline. It is not open ASR/TTS authority.
