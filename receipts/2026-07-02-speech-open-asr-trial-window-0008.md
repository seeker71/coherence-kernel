# Speech open ASR trial window 0008

This receipt adds the eighth scoped open-ASR source window. It does not promote
global open dictation authority.

Local observation:

- German render: macOS `say -v Anna "Ich bin."`
- German oracle: `whisper.cpp` large-v3-turbo on Metal, `-l de`
- German transcript: `Ich bin`
- German wav bytes: `16068`
- German cksum: `3773526825`
- Italian render: macOS `say -v Alice "Io sono."`
- Italian oracle: `whisper.cpp` large-v3-turbo on Metal, `-l it`
- Italian transcript: `Io sono`
- Italian wav bytes: `16706`
- Italian cksum: `3343634846`

Executable movement:

- scoped open-ASR rows: `14 -> 16`
- scoped open-ASR oracle: `16/16`
- scoped open-ASR native: `16/16`
- combined scoped trial rows: `17 -> 19`
- live open dictation authority remains: `oracle 4/4, native 0/4`
- C seed growth: `0`
