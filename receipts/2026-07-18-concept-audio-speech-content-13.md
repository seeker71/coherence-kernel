# Speech-content concept sensing across 13 NL lenses

Date: 2026-07-18

## Claim earned

The 10,000 × 13 audio surface now has an envelope-independent sensing lane.
`model/concept-audio-asr-13.fk` renders speech-only mono PCM, sends that PCM to
a pinned local multilingual Whisper model, and lets Form join the transcript to
the concept-ID-aligned NL label. No marker, concept-frequency, lens-frequency,
lavfi tone source, or audio concat stage exists in this command.

This evidence is distinct:

- semantic evidence: `speech-content:local-neural-asr+form-label-match`;
- address-integrity evidence: `none:speech-only-no-address-envelope`.

The live fixture exercised 20 ordinary utterances: all 13 NL lenses for
`water` (concept 377), plus `camera` (959) and `music` (365) through seven
held-out Eddy locale voices that are absent from the normal audio roster.
Form observed non-silent PCM and selected the right concept in **20/20** rows.
The fixture's three candidates are a confusion test, not a global ceiling:
`casr13-transcribe-file`, its `.transcript.txt` sidecar, and
`casr13-observation-transcript` expose the clean transcript to the full 10,000-
concept text detector.

## Reproducible local oracle

The model is Whisper.cpp `ggml-large-v3-turbo.bin`, 1,624,555,275 bytes:

```text
sha256 1fc70f774d38eb169993ac391eea357ef47c88757ef72ee5943879b7e8e2bc69
source https://huggingface.co/ggerganov/whisper.cpp/resolve/main/ggml-large-v3-turbo.bin
whisper-cpp 1.8.6
ffmpeg 8.1.1
host macOS 26.3.1, Apple M4 Max
```

No user path is committed. The default is the gitignored repository-local
`.cache/whisper.cpp/ggml-large-v3-turbo.bin`; `SEMA_WHISPER_MODEL` may name an
alternate absolute path. The bootstrap installs missing Homebrew carriers when
Homebrew is present, resumes the pinned download, checks SHA-256, and moves an
invalid existing file aside rather than deleting it:

```sh
presence/carriers/concept-audio-asr-13-bootstrap.sh
presence/carriers/concept-audio-asr-13-live.sh
```

The live runner independently hashes the model before launching any row and
passes that verified digest into each isolated Form source-run. No Python is
used by bootstrap, generation, observation, ASR, or verification.

## Live rows

Every WAV below was independently reported by `ffprobe` as mono 16 kHz PCM.
`samples / level` is not host metadata: it is measured by Form directly from
the WAV data chunk (sample count / mean absolute PCM at stride 32 / present=1).
Surrounding transcription errors are retained rather than cleaned; each target
concept surface remained audible and detected.

| row | concept / lens | carrier | samples / level | local Whisper transcript | WAV SHA-256 |
|---:|---|---|---:|---|---|
| 00 | water / en | Samantha | 31,764 / 3,060 | `I need water to cook dinner.` | `19a668fe8166bbd28d3b2b37a0f31a826369ebe4893a045cdb455e6091a7f12e` |
| 01 | water / id | Damayanti | 68,154 / 3,729 | `Saya membutuhkan air untuk memasak makan malam.` | `61e5f817880482a97015af2d29a117a438905852394d63c21a786cb7f116ef79` |
| 02 | water / es | Mónica | 40,870 / 2,742 | `Necesito agua para cocinar la cena.` | `419d65c8c4fbbf46e38c67f3714ad435945e6de153698b1baa183459674cc61a` |
| 03 | water / fr | Thomas | 42,233 / 3,272 | `J'ai besoin d'eau pour préparer le dîner.` | `c7593a09f40f33704dc1bcd80ea04eddcc831b9e1822a9425030f3fe37fb0046` |
| 04 | water / pt-br | Luciana | 52,620 / 3,529 | `Preciso de água para preparar o jantar.` | `dfaa842bdaf1f1375643288239ae1dec620aafafedae167814db135e0f9c17d4` |
| 05 | water / sw | Samantha/en fallback | 57,746 / 3,479 | `Ninahitaji maji kyu pikachakula cha jayani.` | `a2065fd76a7164a661aad1ba8bed275002cd5eeabd263dedd888ea4fa1058678` |
| 06 | water / de | Anna | 58,409 / 2,014 | `Ich brauche Wasser, um das Abendessen zu kochen.` | `cccdeada0ce154249e5fa0e441480bbad3b7c9958fc3ef63ae3912c5f94fc3af` |
| 07 | water / ru | Milena | 53,686 / 3,004 | `Мне нужна вода, чтобы приготовить ужин.` | `cb23913aeb3dff0ed3571e2d1452ea66b4350b563c84c9de670b659b2cafbf97` |
| 08 | water / zh | Tingting | 39,680 / 2,496 | `我需要水来做晚饭。` | `55ca987a5bdede67c21709634c9d731a2c08195a78e2a9df390f0d244c08d5f6` |
| 09 | water / ja | Kyoko | 56,072 / 1,144 | `夕食を作るには水が必要です。` | `3720754e76660e22716e499a380601ed3bad67219f842095c94943b45320a34b` |
| 10 | water / ar | Majed | 55,282 / 2,982 | `احتاج إلى الماء لطهي العشاء` | `306a2892a476eae19d9761d5ad8ab9ee6087353c421dd3241f85287c7b23498b` |
| 11 | water / hi | Lekha | 56,022 / 2,523 | `मुझी रात का खाना पकाने के लिए पानी चाहिए.` | `f30ff04d34ede185316cf1dcd7b296e18599b26cf2740d6b86a08543752c15a4` |
| 12 | water / tr | Yelda | 67,270 / 3,049 | `Akşam yemeği pişirmek için suya ihtiyacım var.` | `9dc67933c949237601b5dd9331f8774e6801d585d5cd8113b768104faea9bbcf` |
| 13 | camera / en | Eddy US, held out | 57,835 / 1,502 | `The camera recorded our family picnic.` | `26ecae40d438bb1a25ae66e0fa0dfbf0c73f793708302648300cef82ab3fac66` |
| 14 | music / es | Eddy Spain, held out | 65,255 / 1,754 | `La música llenó la cocina mientras preparábamos la cena.` | `afa34f7b0c4e23342ff8ed1552fc12dec8d9ab4358f73bf9a12577f0bb4cf252` |
| 15 | camera / fr | Eddy France, held out | 67,047 / 1,865 | `L'appareil photo a enregistré notre picnic familial.` | `2a936f020d9320f04be3ceb16dfd66ed1b59a29d51fe6eb1b9b3fe1a98596e46` |
| 16 | music / pt-br | Eddy Brazil, held out | 75,497 / 2,016 | `A música encheu a cozinha enquanto preparávamos o jantar.` | `058b57b62f98e9ce36a14c96b2b6a32c9c7d0085d26114de7fab9baf6ac76bdc` |
| 17 | camera / de | Eddy Germany, held out | 80,084 / 1,493 | `Der Fotoapparat zeichnete unser Familienpicknik auf.` | `44b8c61db470468e20ff9832cab1841f41a5837495f106c8206faf4ec0375bda` |
| 18 | music / zh | Eddy China, held out | 63,866 / 1,263 | `做晚饭时,厨房里充满了音乐,` | `e8b327eee490dcff32bb1932a3a6a733fdae7678da85b51128757839615c9d42` |
| 19 | camera / ja | Eddy Japan, held out | 74,109 / 1,306 | `カメラは家族のテクニックを記録しました。` | `6302abd1145ea09c5186fb7bd5f72b17f1f6d4c88b4d77baf21ecf237307994f` |

The Swahili surrounding words and Japanese picnic noun show real ASR misses;
the concept labels `maji` and `カメラ` survived. This lane scores the concept
surface, not whole-sentence WER.

## Verification

The live matrix was run in bounded batches because each row intentionally
starts a fresh Form process so ephemeral WAV byte strings cannot accumulate in
the bootstrap arena:

```text
rows 00..06: semantic_content=7/7 address_integrity_used=0
rows 07..13: semantic_content=7/7 address_integrity_used=0
rows 14..19: semantic_content=6/6 address_integrity_used=0
total:       semantic_content=20/20, heldout=7/7, locales=13, concepts=3
```

The pure content/address-separation band checks Unicode matching, ASCII case
folding, longest-label candidate selection, and that the speech command
contains neither `aevalsrc` nor `concat=n=`:

```text
fkwu 127
Go   127
Rust 127
TS   127
```

## Honest floor

- Whisper's 1.6 GB neural weights and decoding are local but host-rented; they
  are not Form-native weights. Form owns PCM observation, the pinned label
  join, candidate ranking, and the semantic/address evidence split.
- The live fixture compares three concepts. The transcript API is deliberately
  separate so the full 10,000-concept detector can consume it without changing
  this acoustic lane.
- A recognized lexical surface does not resolve homonyms or WordNet senses.
  Sentence-context disambiguation remains owed.
- Audio generation still uses macOS `say`; Swahili remains an explicitly named
  English-locale carrier fallback.
- This proves real speech-content sensing on 20 held-out/everyday examples, not
  accuracy over all 130,000 possible concept/lens utterances.

No Python ran. `runtime/fkwu-uni.c` did not change.
