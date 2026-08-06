# Real semantic audio, video, and world-model witness

Date: 2026-07-18

Correction witnessed later the same day: the upstream route name `loop` had
been mistaken for the lexical concept loop, and courthouse footage had been
assigned to the broader `court` id. The executable cells now align the visible
office route to **office 493**, keep **university 1927**, and align the civic
building to **courthouse 9066**. The original ids 6196 and 751 are superseded
by the corrected rows in this receipt and by
`2026-07-18-video-content-sensing-alignment.md`.

## What moved

The 10,000-concept address is no longer demonstrated only by exact synthetic
codes. Two independent closed-set semantic organs now learn from real carrier
content:

- speech: `water` (377), `world` (227), and `camera` (959), trained with the
  Samantha, Daniel, and Karen macOS voices and tested only with held-out Moira;
- video: `office` (493), `university` (1927), and `courthouse` (9066), learned
  from committed 24-frame real-life trajectories and tested on disjoint frames.

The carrier boundary is narrow. `say` and ffmpeg create/decode files. Form
locates WAV speech, reads PCM samples, computes temporal/spectral features,
validates BMPs, reads pixels, computes spatial colour/edge features, retains
training examples, measures distances, chooses concepts, and carries detected
concepts into the ordinary kernel `wm-model` persistence path.

These are closed-set examples, not open ASR, open-vocabulary vision, or full
10,000-concept perceptual training. That remaining floor is explicit.

## Live speech evidence

Each word produced three training WAVs and one held-out WAV: 12 real speech
files in total. Form aligned active speech and computed four time windows. Each
window carries mean energy, zero crossings, and eight Goertzel spectral bands,
for 40 features per utterance.

| held-out voice | word | expected id | detected id | winner L1 |
|---|---:|---:|---:|---:|
| Moira | water | 377 | 377 | 78.3368 |
| Moira | world | 227 | 227 | 102.1475 |
| Moira | camera | 959 | 959 | 96.9349 |

Observed result: **3/3 held-out words**. The first unaligned attempt was only
2/3; active-speech alignment repaired the actual acoustic failure rather than
changing an expected answer.

Pure decision band:

```text
fkwu 127
Go   127
Rust 127
TS   127
```

## Live video evidence

The sources are committed MKV trajectories under
`model/fixtures/lingbot-map/real-life/`. Every frame is 518x294 real imagery:
an indoor office loop, a snowy university route, and an outdoor courthouse
walk. Per frame, Form divides the image into a 4x3 grid and computes mean
luminance, horizontal and vertical edge energy, and mean R/G/B: 72 values.

Training indices are 2, 8, 14, and 20. Held-out indices are 5, 11, 17, and 23.
The interleaved split spans the whole camera route without reusing any test
frame as an exemplar.

| scene | concept id | held-out correct |
|---|---:|---:|
| office route | 493 | 4/4 |
| university | 1927 | 4/4 |
| courthouse | 9066 | 4/4 |

Observed result: **12/12 held-out frames**. An opening-half centroid first
scored 9/12 and exposed trajectory shift. Retaining four distributed views
made the scene memory match the real route rather than hiding that variation.

Pure decision band:

```text
fkwu 127
Go   127
Rust 127
TS   127
```

## World-model integration

`model/concept-world-model.fk` translates a detected concept id into a stable
14-bit bipolar recognition embedding. The generic `wm-entity-embed` and
`wm-persist` path then handles it as kind `concept`; no parallel map was added.
The exact remembered signature scores 1400, while one different bit scores
1200 and remains below the identity floor.

Held-out frame 23 from all three videos was classified and persisted:

| detected id | persistent cell | position | persistence |
|---:|---|---:|---:|
| 493 | office | 1 | 2 |
| 1927 | university | 2 | 2 |
| 9066 | courthouse | 3 | 2 |

Observed result: **3/3 concepts**, three ordinary `wm-model` rows, and
`wm-orient-count world "concept"` = 3.

World bridge band:

```text
fkwu 127
Go   127
Rust 127
TS   127
```

## Boundaries

- No Python was used.
- `runtime/fkwu-uni.c` was not changed.
- Speech synthesis and video decode are host carriers, not claimed native
  generative weights or native codecs.
- Full semantic perceptual coverage remains much smaller than 10,000 concepts;
  this receipt establishes real data and an extensible native learning path.
- The 12/12 score is specifically an interleaved split from the same three
  trajectories. A harder early-to-late split and its reverse each score 10/18;
  an independent Oxford walk scores 0/7 under this exemplar bank. Those misses
  are preserved in the alignment receipt rather than inflated into general
  scene recognition.
