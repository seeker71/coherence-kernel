# Speech content in thirteen ordinary settings

**Verdict:** 13/13 distinct daily-life concepts were recovered from
speech-only PCM across all 13 NL lenses.  Seven observations used Eddy voices
absent from the original locale roster.  Each Whisper transcript was consumed
by Form's complete 10,000-label sentence detector; no expected-id candidate
list was passed to it.  Every row explicitly reports `address-envelope=0`.

## Live observations

The actual transcripts are retained below.  They are not substitutions of the
input text: the Swahili and Chinese rows visibly contain ASR changes.

| lens | id / pinned label | real setting | actual Whisper transcript | full-10k candidates |
|---|---|---|---|---:|
| en | 571 `book` | library return | `I returned the book to the public library after work.` | 9 |
| id | 296 `pintu` | lock the house | `Saya mengunci pintu rumah sebelum pergi bekerja.` | 19 |
| es | 248 `automóvil` | family road trip | `el automóvil necesita combustible antes del viaje familiar.` | 14 |
| fr | 370 `médecin` | clinic visit | `Le médecin examine mon fils à la clinique ce matin.` | 16 |
| pt-br | 532 `comida` | family dinner | `A comida está pronta para o jantar da família.` | 15 |
| sw | 150 `kazi` | build a safe bridge | `Kazi ya leo ni kujenga darija salama pamija.` | 26 |
| de | 259 `Freund` | help a friend move | `Mein Freund hilft mir heute beim Umzug.` | 6 |
| ru | 1071 `дерево` | plant a tree | `Мы посадили дерево возле дома прошлой весной.` | 5 |
| zh | 332 `电话` | charge phone at station | `我的电话没电了所以我在车上充电` | 22 |
| ja | 270 `家庭` | weekly family meal | `私の家庭では毎週日曜日に一緒に夕食を食べます。` | 26 |
| ar | 377 `ماء` | water after exercise | `أشرب ماء نظيف بعد التمرين في الحديقة.` | 5 |
| hi | 468 `बच्चा` | child at the park | `बच्चा स्कूल के बाद पार्क में खेल रहा है।` | 14 |
| tr | 365 `müzik` | music while cooking | `Akşam yemeği hazırlarken mutfakta müzik dinliyoruz.` | 4 |

All thirteen expected ids were present among the listed full-scan matches with
nonzero NL provenance: one `F`, eleven `W`, and one explicit `G` machine-mapped
surface.  Candidate counts from 4 through 26 are useful evidence that this is
not the old closed three-candidate ceiling.

## Waveform evidence

All files are 16 kHz mono signed-16-bit PCM produced by the speech-only command.
The level is the mean absolute sample measured by Form at stride 32.

| lens | samples | level | WAV SHA-256 |
|---|---:|---:|---|
| en | 69,865 | 1,590 | `3f602fa20dfdb17c2d4c2f146962c0b15d738341ed14f4679d6d2298aa8075e1` |
| id | 63,510 | 4,440 | `7d066c1f824ccb8043fb7a639623d4d8e33d89aa943610aa3e39ae96a4ba9a49` |
| es | 70,121 | 1,907 | `33622810909fbe937d411f3b2b91f806e2e07310f4557a2878fd1dc42b0032d8` |
| fr | 59,111 | 1,651 | `3cb3f80177fe05030310bc6b86c895ac41298b83d5bb0bf5de5c55f8f7fb35aa` |
| pt-br | 65,001 | 1,845 | `01867ceeb97241781d8dd87b021be0461e27ac9e3a825748807c62273f435a49` |
| sw | 61,507 | 4,013 | `5f6729a1b6c00a591b5e23b52991bfa5ca073d2b516fd7450075e6ddd2e7c292` |
| de | 58,342 | 1,184 | `0b4da803d77afa9dd5d2c1dc59773c279a52b7fda5d1362a54699c2ccf2afd78` |
| ru | 53,683 | 3,332 | `3aa3147d0915f756bb8c89948b6e14da6658f0c631a81930309b312bbcffbec7` |
| zh | 70,118 | 1,322 | `709a27c1303a71a9187d92cba33261664c63a2e1a340da405133812c34c5854b` |
| ja | 100,582 | 1,715 | `ba75cb067e965f6e9705cd30d7b402e6949ac27af6945e35194203ee9399e2f9` |
| ar | 75,060 | 2,937 | `ebd10a167e3db8f749c8260c01bd76179b1af0e56a0d77947077b39fe525eda8` |
| hi | 50,303 | 2,128 | `134472ba937c6ff92b558ccbe2bc6a1e47cfe6625ab1d2b3d6797398b48ad77c` |
| tr | 72,286 | 3,084 | `fa0364aded4aec5327362f9eaa110ed473a50cdb4b38a09d99813e853de4e9b7` |

The pinned acoustic oracle was `whisper.cpp-large-v3-turbo`, model SHA-256
`1fc70f774d38eb169993ac391eea357ef47c88757ef72ee5943879b7e8e2bc69`.
The model path was supplied only through the command-scoped
`SEMA_WHISPER_MODEL`; no user path is in the cells or carrier.

## The miss that changed the witness

The first Swahili trial was the ordinary school sentence
`Watoto wanatembea kwenda shule kila asubuhi.`  Because macOS has no Swahili
voice on this host, the fallback English voice pronounced `shule` such that
Whisper returned `shiul`.  The full detector correctly reported
`expected-found=0` and verdict 63.  That failed trace was not normalized or
counted.  The retained work sentence still contains real fallback-voice errors
(`daraja` became `darija`, `pamoja` became `pamija`) while preserving the
pinned `kazi`; its expected id is therefore honestly observed.

## Reproduction and proof

```sh
SEMA_WHISPER_MODEL=/path/to/ggml-large-v3-turbo.bin \
  presence/carriers/concept-audio-real-life-13-live.sh
# semantic_content=13/13 concepts=13 locales=13 heldout_voices=7
# full_detector_limit=10000 address_envelope=0

./fkwu --src model/tests/concept-audio-real-life-13-band.fk
# 31
form/form-kernel-go/bin-go form/form-stdlib/core.fk \
  model/concept-audio-real-life-13.fk model/tests/concept-audio-real-life-13-band.fk
# 31
form/form-kernel-rust/target/release/form-kernel-rust form/form-stdlib/core.fk \
  model/concept-audio-real-life-13.fk model/tests/concept-audio-real-life-13-band.fk
# 31
form/form-kernel-ts/node_modules/.bin/tsx form/form-kernel-ts/src/main.ts \
  form/form-stdlib/core.fk model/concept-audio-real-life-13.fk \
  model/tests/concept-audio-real-life-13-band.fk
# 31
```

The pure 31 contract requires thirteen distinct ids, thirteen locales,
speech-only/no-envelope evidence, and a full-10k rather than closed detector.
The live per-row verdict 127 additionally requires a present PCM waveform,
64-byte hex hash, nonempty Whisper transcript, scan limit 10,000, and expected
concept recovery.

## Honest boundary

The acoustic weights are still host-rented, not native Form weights.  These
thirteen retained examples are materially broader evidence, not a claim that
all 130,000 concept/lens speech combinations have been acoustically tested.
Synthesis is also a controlled carrier witness, not uncontrolled microphone
audio.  The next non-toy floor is recorded human speech with noise, overlap,
distance, and speakers never present in the synthesis system.
