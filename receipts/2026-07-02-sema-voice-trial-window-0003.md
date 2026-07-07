# Sema voice trial window 0003

This receipt adds the third scoped Sema voice challenger window. It does not
promote global live Sema voice authority.

Local observation:

- render: macOS `say -v Samantha "Speech becomes clearer."`
- normalize: `ffmpeg`, 16 kHz mono wav
- oracle: `whisper.cpp` large-v3-turbo on Metal
- oracle transcript: `Speech becomes clearer.`
- WER: `0`
- wav bytes: `43638`
- cksum: `1616552855`

Executable receipt:

- `learn/sema-voice-trial-window-0003.fk`
- `learn/tests/sema-voice-trial-window-0003-band.fk`
- verdict: `32767`

Movement:

- scoped Sema voice trial floor: `2/2 -> 3/3`
- combined Sema voice training floor: `native 2/3 -> 3/4`
- live Sema voice authority remains: `oracle 1/1, native 0/1, WER 100`
- C seed growth: `0`
