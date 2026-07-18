# 10,000 concepts × 13 NL: spoken audio scale witness

Date: 2026-07-18

## What became operational

`model/concept-audio-scale-13.fk` joins the pinned, concept-ID-aligned 10,000-row NL runtime to a
lazy audio generator and a Form-native PCM sensor. A request names one concept
index and one of the 13 NL codes. The cell:

1. fetches that locale's pinned label and exact provenance; the current matrix
   has no absent cells, while a pure synthetic law retains explicit English-
   anchor recovery for older or partial matrices;
2. selects a language-appropriate installed macOS voice where one exists;
3. asks `say` to speak the actual label;
4. asks ffmpeg to normalize it to mono 16 kHz PCM and append a quiet identity
   envelope; and
5. has Form reopen the WAV, prove that non-silent speech precedes the envelope,
   count PCM zero crossings, and recover the exact concept and lens.

Nothing materializes 130,000 WAVs in advance. `ca13-generate` generates any
requested cell in the `0..9999 × 13` address lazily. The edge cells were run
live as well as the matrix examples.

The runtime observed complete lexical coverage: 130,000/130,000 cells present,
with source codes F=10,000 frequency anchors, W=34,941 Wiktionary OMW,
D=16,621 dedicated WordNet, C=68 CLDR/WordNet, and G=68,370 unreviewed Google
machine translations. Absent=0. Audio records retain source name and code.

The four 250 ms envelope windows are marker, concept high digit, concept low
digit, and NL lens. Frequencies are multiples of 4 Hz, giving an integral cycle
count at 250 ms. The marker was 2200 Hz on every live file. The envelope is an
exact integrity/address channel. It is **not** claimed to carry lexical
semantics: the preceding real spoken label carries those semantics.

The alignment was checked against the authoritative 30-byte frequency table,
not inferred from the multilingual file's row number:

```text
concept-10000-ranked.dat slot 70   -> good   |01741730
concept-10000-ranked.dat slot 145  -> life   |00624596
concept-10000-ranked.dat slot 959  -> camera |00056757
concept-10000-ranked.dat slot 9999 -> caviar |00002510

NL row 70   -> good   / 01123148-a
NL row 145  -> life   / 13963192-n
NL row 959  -> camera / 02942699-n (Hindi कैमरा, G machine source)
NL row 9999 -> caviar / 07799579-n (Turkish havyar)
```

## Real live data for every NL lens

Two frequency-model concepts whose source rows contain labels in all 13 lenses were generated:
`good` (concept ID 70, PWN `01123148-a`) and `life` (concept ID 145, PWN
`13963192-n`). `speech samples / level` are computed by Form over the utterance
before the envelope on a 32-sample stride. Every row had speech-present=1,
marker-valid=1, and recovered the expected concept/lens exactly.

| lens | installed voice carrier | locale-native | spoken good label | good samples / level | spoken life label | life samples / level | sensed ids |
|---|---|---:|---|---:|---|---:|---|
| en | Samantha (`en_US`) | 1 | good | 4737 / 2248 | life | 7501 / 2632 | 70:0, 145:0 |
| id | Damayanti (`id_ID`) | 1 | baik | 7158 / 3401 | hidup | 7194 / 3786 | 70:1, 145:1 |
| es | Mónica (`es_ES`) | 1 | bueno | 6435 / 4506 | vida | 6435 / 3761 | 70:2, 145:2 |
| fr | Thomas (`fr_FR`) | 1 | bon | 4232 / 2554 | vie | 4457 / 2466 | 70:3, 145:3 |
| pt-br | Luciana (`pt_BR`) | 1 | bom | 5062 / 3157 | vida | 8011 / 3472 | 70:4, 145:4 |
| sw | Samantha (`en_US`) | **0** | mzuri | 8614 / 3984 | uhai | 7868 / 3689 | 70:5, 145:5 |
| de | Anna (`de_DE`) | 1 | gut | 6151 / 1411 | Leben | 7019 / 2941 | 70:6, 145:6 |
| ru | Milena (`ru_RU`) | 1 | хороший | 10529 / 2740 | жизнь | 7519 / 3445 | 70:7, 145:7 |
| zh | Tingting (`zh_CN`) | 1 | 好 | 5504 / 2111 | 生命 | 9680 / 2943 | 70:8, 145:8 |
| ja | Kyoko (`ja_JP`) | 1 | 良い | 4954 / 1308 | 生存 | 9360 / 1440 | 70:9, 145:9 |
| ar | Majed (`ar_001`) | 1 | حسن | 9889 / 1688 | حياة | 10260 / 2698 | 70:10, 145:10 |
| hi | Lekha (`hi_IN`) | 1 | अच्छा | 6550 / 1666 | ज़िंदगी | 8864 / 2386 | 70:11, 145:11 |
| tr | Yelda (`tr_TR`) | 1 | iyi | 4795 / 3100 | hayat | 9044 / 2041 | 70:12, 145:12 |

Observed matrix result: **26/26 real spoken WAVs**, **13/13 lenses for each
of two concepts**, **26/26 exact Form-native identity recoveries**. Twelve
lenses had an installed locale voice. macOS had no installed `sw` voice, so
the source-mapped Kiswahili labels were spoken through Samantha/en_US and the
runtime reports `locale-native=0`. That is a carrier fallback, not a claim of
native Kiswahili pronunciation.

For `good`, the address windows were 2400 Hz and 3280 Hz; for `life`, 2404 Hz
and 3180 Hz. Lens windows stepped from 3600 through 3840 Hz. Form observed
those exact values on every file.

## Held-out, machine-label provenance, and address edges

| case | spoken carrier | source state | speech samples / level | Form sensed | result |
|---|---|---|---:|---|---:|
| held-out voice | Daniel speaks `water` (377/en) | `frequency-observed`, F | 8582 / 1693 | 377:0, marker 2200 | 1 |
| machine Hindi label | Lekha speaks `कैमरा` | `machine-translated-unreviewed`, `google-translate-machine`, G | 6826 / 2682 | 959:11, marker 2200 | 1 |
| first address | Samantha speaks `you` | `frequency-observed`, F | 5089 / 3380 | 0:0, marker 2200 | 1 |
| final address | Yelda speaks `havyar` | mapped-unreviewed | 11645 / 2399 | 9999:12, marker 2200 | 1 |

The held-out voice is absent from the normal 13-lens roster. The camera witness
asserted all of label `कैमरा`, state `machine-translated-unreviewed`, source
name `google-translate-machine`, source code `G`, Hindi lens 11, non-silent
speech, and exact identity recovery. Both address extremes recovered exactly.

Five ephemeral carrier hashes from the witnessed run (WAVs remain in `/tmp`,
because generation is deliberately lazy rather than a committed audio corpus):

```text
6c71cf42cd94c02f1d391f11cfabf5dbcb64ed7967b6a056db52ed844669d1c2  ca13-live-70-en.wav    41552 bytes
4430df2ad3275030cd31d627e1ca7ac91d849b182124e80bb34874f99dccf130  ca13-live-70-sw.wav    49306 bytes
bdd53a8d60815ae3b46079f58a193371756a73c0c60efce23f6b385e943ecd3d  ca13-live-145-zh.wav   51438 bytes
66db9ee706e20e2512a3505f10f974ad4d6a06a40cb3c1c61d85e1df18226e15  ca13-live-959-hi-machine.wav    45730 bytes
810e96bbae57df792d8b52c5deb8e35f416f22fc4e463451606a12c97df38adb  ca13-live-9999-tr-last.wav      55368 bytes
```

`ffprobe` independently reported mono 16 kHz PCM and durations of 1.296063 s
for `70/en`, 1.605000 s for `145/zh`, and 1.426625 s for the Hindi machine
label `959/hi`.

## Pure four-way witness

The band covers all 13 lens indices, both address-frequency extremes,
roundtrip arithmetic, the Swahili carrier gap, installed Hindi carrier,
embedded-apostrophe shell quoting, envelope dimensions, and a synthetic empty-
label English-anchor fallback including its F provenance. Live data never uses
that fallback because the current source matrix has zero absent cells.

```text
fkwu  32767
Go    32767
Rust  32767
TS    32767
```

Commands used:

```sh
./fkwu --src model/tests/concept-audio-scale-13-band.fk

./walkers/go/walker form/form-stdlib/core.fk form/form-stdlib/form-fs.fk \
  model/concept-multimodal-codec.fk \
  cognition/concept-nl-semantic-13-metadata.fk \
  cognition/concept-nl-semantic-13-offsets.fk \
  cognition/concept-nl-semantic-13.fk \
  cognition/concept-nl-semantic-13-runtime.fk \
  model/concept-audio-scale-13.fk \
  model/tests/concept-audio-scale-13-band.fk

./walkers/rust/target/release/form-walker-rust \
  form/form-stdlib/core.fk form/form-stdlib/form-fs.fk \
  model/concept-multimodal-codec.fk \
  cognition/concept-nl-semantic-13-metadata.fk \
  cognition/concept-nl-semantic-13-offsets.fk \
  cognition/concept-nl-semantic-13.fk \
  cognition/concept-nl-semantic-13-runtime.fk \
  model/concept-audio-scale-13.fk \
  model/tests/concept-audio-scale-13-band.fk

deno run --allow-read walkers/ts/main.ts \
  form/form-stdlib/core.fk form/form-stdlib/form-fs.fk \
  model/concept-multimodal-codec.fk \
  cognition/concept-nl-semantic-13-metadata.fk \
  cognition/concept-nl-semantic-13-offsets.fk \
  cognition/concept-nl-semantic-13.fk \
  cognition/concept-nl-semantic-13-runtime.fk \
  model/concept-audio-scale-13.fk \
  model/tests/concept-audio-scale-13-band.fk
```

Live command:

```sh
./fkwu --src presence/concept-audio-scale-13-live.fk
```

## Honest floor

- The speech is real audio carrying real pinned labels, but `say` is a host TTS
  carrier. This is not a native generative acoustic model and no such weights
  are claimed.
- Exact detection is earned by the Form-sensed identity envelope. It is not
  open ASR, accent-general acoustic concept recognition, or proof that Form can
  infer the word from speech after the envelope is removed.
- Multilingual source labels remain provenance-tagged. The 68,370 G cells are
  explicitly `machine-translated-unreviewed`, not fluent or reviewed
  translations. The pure resolver still reports an English-anchor fallback if
  it is handed a synthetic empty source cell; none exist in the live matrix.
- Kiswahili has no locale-native installed voice on this machine. Replacing its
  fallback is an owed carrier action.
- This closes the scalable audio address/generation path, not the larger goal's
  requirement for native learned semantic audio generation and detection over
  all 10,000 concepts.

No Python ran. `runtime/fkwu-uni.c` did not change.
