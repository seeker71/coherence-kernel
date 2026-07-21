# Arbitrary-address and situated speech over 10,000 × 13 NL

**Verdict:** the speech organ is now callable at every `(concept id, NL lens)`
address in the `0..9999 × 13` domain.  It retrieves the pinned surface, builds
a lens-specific utterance, synthesizes speech-only PCM, transcribes that PCM
with the pinned local Whisper model, and gives only the transcript to Form's
complete 10,000-label detector.  The returned row retains every candidate,
each candidate's sense analysis and source provenance, the requested anchor's
complete sense set, PCM evidence, a waveform hash, and an explicit `success`
or `miss` reason.

This receipt separates two kinds of evidence.  The arbitrary metalinguistic
frame proves universal addressability without pretending that every concept
has a hand-authored scenario.  A second 13-lens matrix proves concrete public-
life speech in emergency, transport, medical, workplace, weather, school, and
home-safety settings.

## Arbitrary-address API

```form
(cao10l-address-run 503 "en" "-caller-owned-suffix")
(cao10l-situation-run 629 "en"
  "Because black ice covered the street, the road crew closed one lane before the morning commute."
  "Eddy (English (UK))" "-road-ice")
```

Both functions live in `presence/concept-audio-open-10000-13-live.fk`.
`cao10l-address-run` accepts any valid pair, not a fixture index.  Detection is
exactly `ctd13-runtime-detect-sentence runtime code transcript`; neither the
requested id nor its surface is an argument to the detector.  The detector's
limit is `cnl13-anchor-count = 10000` and its candidate records contain id,
PWN anchor, locale surface, source name/code, rank/frequency, primary semantic
record, all explicit senses, context ranking, score, and evidence.

There is no tone source, concatenated identity prelude, identity envelope,
closed acoustic confusion list, or expected-id insertion.  `say`, `ffmpeg`,
and the pinned `whisper.cpp-large-v3-turbo` weights remain named host carriers;
all joins, PCM measurement, 10k scanning, ambiguity retention, and result
classification are Form.

## Deterministic address-space sample

The reproducible sequence is `id = (503 + 7919*i) mod 10000`, with lens
`i mod 13`.  Twenty observations crossed the whole rank range, covered all
thirteen lenses, and returned **14 successes and 6 retained misses**.  None was
selected or replaced after hearing its output.

| i | address | pinned surface | result | candidates | source | PCM samples | WAV SHA-256 | actual Whisper transcript |
|---:|---|---|---|---:|---|---:|---|---|
| 0 | 503:en | `cool` | success | 13 | F | 111743 | `20d2cd48817fd262695be2ad7821b1e2541113de41aab730e7398ca8b738fea0` | `Today's concept is cool. It appears in ordinary conversations about daily life.` |
| 1 | 8422:id | `jejak kaki` | success | 31 | G | 116068 | `b902c851d084f75a63e584c4c40bf7d675f90924dddc01253df6fe26456635dc` | `Konsep hari ini adalah jejak kaki. Istilah ini muncul dalam percakapan sehari-hari.` |
| 2 | 6341:es | `seco` | success | 24 | D | 96745 | `456a8c3afa104d0ffff05bee40e0a7b2676990cd851a90910dc940175f9723e5` | `El concepto de hoy es seco. Aparece en conversaciones cotidianas sobre la vida diaria.` |
| 3 | 4260:fr | `fruit sec` | success | 22 | W | 113127 | `98b5af3228e3068f462b5b9eccf6378a6e56dda8654303afb86140edc581ea6c` | `Le concept de jour est fruit sec. Il apparaît dans les conversations ordinaires de la vie quotidienne.` |
| 4 | 2179:pt-br | `inimigos` | success | 22 | G | 112103 | `e960c1f91bf7ad06891384371a39b361b95cd3de9176b01903d56f39b34fe489` | `O conceito de hoje é inimigos. Ele aparece em conversas comuns da vida cotidiana.` |
| 5 | 98:sw | `mtu` | success | 16 | G | 115315 | `1e17507b43518f3aab811b4b9a253757fd9606c943695757fb63ae69b2085b5e` | `Dona ya Leone MTU, Nino Hilihu Tokia Katika Mazungamzo Ya Kaeda Ya Kila Siku.` |
| 6 | 8017:de | `mild` | success | 5 | G | 111060 | `54b979a5677f65c467a37c4555aa7bb44e75653ab697922ee41f1fba27b75837` | `Der heutige Begriff ist mild. Er erscheint in gewöhnlichen Gesprächen über den Alltag.` |
| 7 | 5936:ru | `передача` | miss | 2 | W | 109975 | `f06d1b29ddbb8fd9dd88088fe31d056e250c228e66c3f5616c13ff779317fd69` | `Сегодняшнее понятие «передача», оно встречается в обычных разговорах о повседневной жизни.` |
| 8 | 3855:zh | `使保持某状态` | miss | 41 | D | 126842 | `f4aa53ef18b3d56f53bfd81f559c8bde6ec31ba58be51a459947b0cab80faa45` | `今天的概念是时保存某状态,它会出现在有关日常生活的普通对话中。` |
| 9 | 1774:ja | `さん` | miss | 27 | G | 145789 | `2e9d89943d726734537974ae52c5dd42c66b3141dc4133b0b0297dcb52220a9c` | `今日の概念は、3です。この言葉は、日常生活の普通の会話に登場します。` |
| 10 | 9693:ar | `فيليس` | success | 11 | G | 162174 | `88cbf1e309409766ef42a7ee747f6aa95dc485a253700270edf110a72d2a08d4` | `مفهوم اليوم هو فيليس يظهر هذا التعبير في المحادثات العادية عن الحياة اليومية` |
| 11 | 7612:hi | `मिमी-मिमी` | miss | 12 | G | 108944 | `275bfe8e8e33f4b6637641d30743c3a101358c4bb17be281dd3f8c9d77399893` | `आज की अवधारना मिमी मिमी है। यह शब्द रोजमरा के जीवन की सामाने बातचीत में आता है।` |
| 12 | 5531:tr | `kimya` | success | 22 | G | 108626 | `266b6d26c96d306591645df4db09e0a24187c9bfd22d7e4417fbde58311110d8` | `Bugünün kahramı kimya. Bu ifade günlük yaşamla ilgili sıradan konuşmalar da geçer.` |
| 13 | 3450:en | `instructions` | success | 13 | F | 121214 | `d386c41204a717fd8179f2bb4e911ce5e894e9b0764289a205177a84d74c15fd` | `Today's concept is instructions. It appears in ordinary conversations about daily life.` |
| 14 | 1369:id | `memasang` | success | 32 | D | 111796 | `e85c522e55173b6ff6891304b21428c32e8fce0925828d9d735469c5ea8d194d` | `Konsep hari ini adalah memasang. Istilah ini muncul dalam percakapan sehari-hari.` |
| 15 | 9288:es | `corteza` | success | 24 | D | 100329 | `25ee533171e5fbc6535e65550abd157ac922ac19a401c3c00bf5533a28c1ea16` | `el concepto de hoy es corteza. Aparece en conversaciones cotidianas sobre la vida diaria.` |
| 16 | 7207:fr | `Simone` | success | 19 | G | 110823 | `26cd31421090781427c04cb60ebebdcca886590e9b1283d8e9c0f6ce6f57c0dc` | `le concept de jour et Simone. Il apparaît dans les conversations ordinaires de la vie quotidienne.` |
| 17 | 5126:pt-br | `exterior` | success | 22 | G | 113127 | `c0c7ceb6fa19c16304645eaa30b8757f1c3f01eeae62ea5b0d825686d871ccf3` | `O conceito de hoje é exterior. Ele aparece em conversas comuns da vida cotidiana.` |
| 18 | 3045:sw | `kazi ya nyumbani` | miss | 27 | G | 127018 | `c66be273d1611ba70fcc201936279524067f0cdbd633698e58c894a8a87814b9` | `Dona ya lio ni kazi ya ni ambani. Ni no hili hutoki ya katika mazangamzo ya kaeda ya kila siku.` |
| 19 | 964:de | `Ben` | miss | 4 | G | 111572 | `0cdf1c999db37c51bf7eb16b1a11024a3150eb7e29ff8725e1ec482138838353` | `Der heutige Begriff ist den. Er erscheint in gewöhnlichen Gesprächen über den Alltag.` |

Rows 7 and 11 demonstrate why success is not inferred from visual similarity:
the printed transcript looks close to the pinned surface but the exact Form
detector did not find the requested concept under its locale boundary rules.
Rows 8, 9, 18, and 19 preserve visible acoustic substitutions.

## Concrete real-life matrix

The situated path returned **12 successes and one genuine miss**.  All rows are
speech-only and all detector candidate counts came from the complete 10k scan.

| # | address | situation | surface | result | candidates | source | PCM samples | WAV SHA-256 | actual Whisper transcript |
|---:|---|---|---|---|---:|---|---:|---|---|
| 0 | 629:en | black-ice-road-closure | `street` | success | 13 | F | 113108 | `fdaa615a59fe80ff0be555234af80cf1f6cca4c9ce69581678098f9aada36f0d` | `Because black ice covered the street, the road crew closed one lane before the morning commute.` |
| 1 | 296:id | emergency-exit-inspection | `pintu` | success | 14 | W | 85324 | `a620bd35b817483943356be6290b8cd3ae430914e52473b93e438589875488bb` | `Petugas memeriksa pintu darurat sebelum gedung dibuka untuk umum.` |
| 2 | 1102:es | delayed-public-transit | `autobús` | success | 16 | W | 79593 | `f3b3d432bb9d7d27ef1535f582bf33d2d30173e7688f963163f8f7a828ddabb6` | `El autobús llegó 20 minutos tarde por una avería durante la hora punta.` |
| 3 | 370:fr | pharmacy-dosage-review | `médecin` | success | 15 | W | 87271 | `3571cac201800cab7a31f5d0a1dc5d187438ebc363fb8bd30eb3121d2df6c6cd` | `Le médecin a vérifié la dose avec le pharmacien avant de renouveler le traitement.` |
| 4 | 6600:pt-br | kitchen-fire-prevention | `fogão` | success | 16 | G | 97758 | `28b2b171df334e75dcbc105ece4cc002f91e8adda563b67bef75f9fa0bda1d36` | `Desliguei o fogão e conferi o detector de fumaça antes de sair de casa.` |
| 5 | 150:sw | workplace-safety-meeting | `kazi` | success | 24 | W | 94014 | `bed7e10286e3ed732a6d872b1b888c5feafdc9e196d749868387ca73390ea09a` | `Katakam kutano wa kazi. Timu ila kagwa mpango wa yu salama wa gala.` |
| 6 | 1071:de | storm-road-obstruction | `Baum` | success | 8 | W | 99814 | `a0d40479f3069ab5f51d6272e119129ee76550b71a14f75b6ae71e6479b9f8df` | `Nach dem Sturm blockierte ein Baum die Landstraße, bis die Feuerwehr eintraf.` |
| 7 | 332:ru | severe-weather-warning | `телефон` | success | 11 | W | 81559 | `19a6d1ea0b0177ca1021ab2ebe23893fddae304a9b73364292f15d5f0fb5b283` | `Я получил предупреждение о сильной грозе на телефон и отменил поездку.` |
| 8 | 571:zh | school-closure-reading | `书` | success | 28 | W | 86397 | `2bf7f0fc17e76609786ad4f1fc6cba998334576c3b9162ee7c0a570e27cf9e82` | `到语情课后,老师通过网络把书和作业发给了学生。` |
| 9 | 916:ja | river-flood-evacuation | `岸` | success | 18 | W | 111591 | `61c3438e94b5349673decf18531b505ffe31b4939f8d023ba4ceee4c66ba3997` | `大雨で、川の岸が浸水したため、住民は高台へ避難しました。` |
| 10 | 377:ar | emergency-water-distribution | `ماء` | success | 7 | W | 116486 | `e0e4fa8a1144518a3be8061aaffb1122ecfc475d7e8c67b20b991388365ffae2` | `وزعت فرق الطوارئ ماء نظيفة على الأسر بعد انقطاع الأنابيب.` |
| 11 | 468:hi | child-medicine-instructions | `बच्चा` | **miss** | 9 | G | 74794 | `5be0d4171a048ce35f68a84ab4e1f20c8535ca2edc00949a3c666cb4d3e8299b` | `नर्स ने बताया कि बचा दवा की अगली खुरात भोजन के बाद लेगा.` |
| 12 | 628:tr | ambulance-hospital-arrival | `hastane` | success | 7 | W | 99219 | `6b7899f3cd2faf1b9e25c7f061a4c737c67e65fb1c77023032ddfaf103873131` | `Ambulans yaralı bisikletçiyi öğleden önce hastane acil servisine getirdi.` |

The Hindi row is the important negative: synthesis spoke pinned `बच्चा`, but
Whisper emitted `बचा`.  Form returned nine other transcript-derived concepts
and correctly left requested ID 468 absent.  The row remains in the fixture
and receipt; it was not respoken, normalized, or converted into a success.

## Reproduction

```sh
presence/carriers/concept-audio-asr-13-bootstrap.sh

presence/carriers/concept-audio-open-10000-13-address.sh 7422 en
# explicit miss: pinned darrin -> actual Darren

CAO10_SAMPLE_COUNT=20 \
  presence/carriers/concept-audio-open-10000-13-sample.sh
# sampled=20 success=14 miss=6 detector-limit=10000 address-domain=130000

presence/carriers/concept-audio-open-real-life-13-live.sh
# situations=13 locales=13 success=12 miss=1 speech-only=13 detector-limit=10000

./fkwu --src model/tests/concept-audio-open-10000-13-band.fk
# 127
```

The pinned Whisper model SHA-256 is
`1fc70f774d38eb169993ac391eea357ef47c88757ef72ee5943879b7e8e2bc69`.
The bootstrap uses `/usr/bin/curl` and hash-verifies the downloaded model.  No
Python runs, no user path is embedded, and `runtime/fkwu-uni.c` is unchanged.

## Honest floor

Universal **addressability** is now operational; universal **acoustic success**
is not claimed.  Twenty spread probes are evidence about the open runtime, not
an exhaustive 130,000-waveform test.  Synthesis is controlled speech rather
than a microphone recording with noise, overlap, distance, and unseen human
speakers.  Whisper's weights remain host-rented rather than Form-native.  The
detector retains word senses and ranks context but does not resolve ambiguity.
The six arbitrary misses and one situated miss name where the next acoustic
and normalization work must begin.
