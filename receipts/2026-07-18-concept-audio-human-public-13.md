# Human speech from thirteen public recordings enters the concept world

**Verdict:** thirteen independently human-recorded Lingua Libre pronunciations
were acquired from Wikimedia Commons without authentication: thirteen NL
lenses, thirteen named speakers, and thirteen distinct ordinary-life concepts.
Unprompted Whisper transcripts were passed, content-only, to the complete Form
10,000-label detector. Eight expected concepts were present and entered the
existing `cwm-persist` world bridge; five acoustic/orthographic misses remained
misses and entered no world cell.

This closes the previous all-TTS evidence gap. It does not claim conversational
speech, overlap, microphone distance coverage, or Form-native acoustic weights.

## Source and license evidence

The source snapshot is
`presence/fixtures/concept-audio-human-13-source.tsv` (SHA-256
`8661d9d15390e2bdd8077d1cdacb3c592257732cb40711016e3378146814d40f`).
It retains, per recording: raw and description URLs, Commons MediaInfo id,
source transcription, language, speaker name and Lingua Libre id, recording
date, license and license URL, API timestamp, SHA-1, SHA-256, bytes, duration,
and retrieval state. Total raw input is **1,265,092 bytes**.

Commons MediaInfo supplied the labels rather than this repository: audio
transcription `P9533`, language `P407`, speaker `P10894` with author-name
qualifier `P2093`, recording date `P10135`, and license `P275`. The ordinary
image-info API independently supplied the raw URL, size, duration, uploader,
timestamp, and SHA-1. Three files are CC0; ten are CC BY-SA 4.0. Attribution is
retained row by row rather than collapsed into a corpus-level name.

The first sequential acquisition met real HTTP 429 responses for Arabic,
Hindi, and Turkish. Those failures remain in the manifest as
`unavailable-http-429-then-query-retry-verified`. Retrying the same raw URLs
with the inert `?download=1` query returned the original WAV bytes; both their
API SHA-1 and newly pinned SHA-256 matched. The live carrier uses that route and
still records any later download failure as `unavailable`.

## Actual content-only results

The filename Whisper received was always `sample-<index>.wav`; no source word,
concept id, speaker, or locale appeared in the basename. The command supplied
an explicit language but no `--prompt`. Only after ASR and the complete Form
scan did the runtime consult the expected id to classify presence or absence.

| lens | human speaker | source concept | actual Whisper transcript | complete candidate ids | result / world |
|---|---|---|---|---|---|
| en | Simplificationalizer | 571 `book` | `book.` | `571` | success / admitted |
| id | Xbypass | 377 `air` | `air` | `377` | success / admitted |
| es | Millars | 3966 `abeja` | `Abeja` | `3966` | success / admitted |
| fr | Fhala.K | 370 `médecin` | `Médecins.` | `1906` | miss / rejected |
| pt-br | Santamarcanda | 259 `amigo` | `Amigo.` | `259,822,1800,9821` | success / admitted |
| sw | Rigolearning | 137 `jambo` | `Jumbo.` | none | miss / rejected |
| de | Natschoba | 351 `abbiegen` | `abbiegen` | `351` | success / admitted |
| ru | Tatiana Kerbush | 9326 `аббатство` | `Абатство.` | none | miss / rejected |
| zh | 雲角 | 206 `人` | `人` | `378,3414,206,419,4987` | success / admitted |
| ja | Higa4 | 791 `ホテル` | `ホテル` | `791,6865` | success / admitted |
| ar | Fenakhay | 996 `آلة` | `لا` | `27,26,6841,187,1009` | miss / rejected |
| hi | AryamanA | 1876 `अंडा` | `अन्दा` | none | miss / rejected |
| tr | ToprakM | 225 `anne` | `Anne.` | `225,570,261,979,969,1332,2642,3328,7814,8052,8684` | success / admitted |

The five misses are substantive. French ASR pluralized the source word and the
detector returned a different pinned id; Swahili changed `jambo` to `Jumbo`;
Russian dropped one `б`; Arabic decoded a different word; Hindi changed the
vowel/orthography. None was normalized, respoken, repaired, or inserted.

The raw recordings also preserve materially different acquisition conditions.
Measured RMS spans -33.76 to -18.72 dB and peaks span -18.13 to -0.61 dB.
Finite measured noise floors range from -84.29 to -40.88 dB; four clips report
`-inf` at the analyzer's floor. These are measured variations among independent
human recordings, not invented noise-class labels and not a controlled noisy-
speech benchmark.

## Offline replay and live raw-waveform gate

`presence/fixtures/concept-audio-human-13-observed.tsv` retains the exact
Whisper transcript bytes as UTF-8 hex, transcript SHA-256, normalized WAV
SHA-256, measured levels, complete candidate-id list, expected presence, and
world admission. Its SHA-256 is
`6c10777ac8c4f4b0d78ac1ded75e71add9c6715376b7c8cad2068f167f76e2a6`.

The Form companion `model/concept-audio-human-13-observed.fk` replays all exact
transcript byte strings through the full detector and requires all thirteen
candidate lists to match. It also exposes `cah13-human-audio-audit` for the
central completion ledger.

```sh
node presence/carriers/concept-audio-human-13-source-verify.mjs
# source-rows=13 live-pages=13 live-entities=13 mismatches=0

./fkwu --src model/tests/concept-audio-human-13-band.fk
# 127

./fkwu --src model/tests/concept-audio-human-13-observed-band.fk
# 255

./fkwu --src presence/tests/concept-audio-human-13-integrity-band.fk
# 31 (correct pins pass; same-length false hash and wrong size fail)

CAH13_EVIDENCE_FILE=/tmp/cah13-live-evidence.log \
  presence/carriers/concept-audio-human-13-live.sh
# human-recordings=13 locales=13 speakers=13 success=8 miss=5 unavailable=0
# world-admitted=8 offline-exact=13 detector-limit=10000 tts=0 prompt=0
```

The final clean live evidence file was 25,884 bytes with SHA-256
`27b095ee4f4e1d162e58316414de71a791bc4a03ec00ec5a473aeb5f3890733e`.
Each successful row contains the complete candidate records with source and
sense evidence, not only the bounded id list shown above. Form independently
rehashes both the raw source file and normalized WAV before admitting the row.

## Integration and honest floor

`presence/concept-audio-human-13-live.fk` is a non-test runtime. Its
`cah13-world-admit` takes the id from an actually returned detector candidate,
then calls the existing `cwm-persist`; a miss returns an empty admission. The
same file freezes neither expected id nor transcription into the acoustic
path. The offline expected row is consulted only after live detection, to
compare output.

This is a real human-speech floor, not universal acoustic parity. The corpus is
thirteen short pronunciation clips, not 130,000 concept/lens recordings and
not spontaneous sentences with overlap, distance, or deliberately controlled
noise. Whisper remains a hash-pinned host oracle; its weights are not Form-
native. The detector preserves ambiguity but does not solve word sense from a
one-word clip. No Python ran, and `runtime/fkwu-uni.c` was not changed.

The exchange stayed alive by allowing the five human misses to control world
admission instead of letting source metadata repair them. The surprising
teaching was that real voices produced more semantic ambiguity than the TTS
lane even on single common words. Discomfort turned to gold when the three 429
responses and five recognition misses became reproducible states and hashes,
rather than disappearing behind a success total.
