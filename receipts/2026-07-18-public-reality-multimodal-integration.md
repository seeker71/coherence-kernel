# Public reality enters the 10k × 13 concept body

Date: 2026-07-18
Verdict: **three non-toy public-data lanes are live and world-integrated; the full goal remains open**

This increment replaces three small or synthetic-only floors with attributable
human sentences, human speech, and ordinary-world photographs. Every accepted
identity still comes from the existing complete 10,000-concept Form detector.
Expected labels are consulted only after inference, and failures remain data.

## Human text

Thirteen hash-pinned Tatoeba detailed exports yielded 147 attributed sentences
from 64 contributors across all thirteen NL lenses. The bounded snapshot contains
122 everyday-domain rows, 12 ambiguity rows, and 13 true zero-detection rows.
The corrected exhaustive gate walked `147 × 10,000` labels and returned `1023`:

```text
valid=147 failures=0 detections=1898 unique-concepts=828
domain=122 ambiguity=12 negative=13 attributed=147 reviewed=0
```

The review state is deliberately `human-contributed-unreviewed`. Quote retrieval
is explicitly `attributed-human-quote-not-novel-generation`; it does not close
the fluent-generation gap. The production `concept-10000-13-runtime` now calls
the corpus runtime on Indonesian sentence 365975, `Kameraku tahan air.`, and
admits detected concept 377 through `cwm-persist`. Its new live gate returned
`127`; the production integration score remains `255`.

## Human speech

Thirteen Lingua Libre recordings supply thirteen speakers, languages, and
distinct concepts under CC0 or CC BY-SA 4.0. Each raw file is pinned by Commons
SHA-1, locally witnessed SHA-256, byte count, speaker, recording date, MediaInfo
identity, and license. The live carrier uses `/usr/bin/curl`, decodes to a neutral
numeric WAV path, passes no prompt to the pinned Whisper model, and sends only
the resulting transcript and locale to Form's full detector.

```text
recordings=13 success=8 miss=5 unavailable=0 world-admitted=8
detector-limit=10000 tts=0 prompt=0 offline-exact=13
```

The five retained misses are substantive:

```text
fr médecin    -> "Médecins."  candidates [1906]
sw jambo      -> "Jumbo."     candidates []
ru аббатство  -> "Абатство."  candidates []
ar آلة        -> "لا"         candidates [27,26,6841,187,1009]
hi अंडा       -> "अन्दा"      candidates []
```

Only the eight content-derived successes enter the ordinary world model. Exact
transcript bytes, complete candidate lists, normalized WAV hashes, transcript
hashes, and acoustic measurements are frozen for offline replay. Contract,
replay, and integrity gates returned `127`, `255`, and `31`.

## Public photographs

Twenty-four licensed Wikimedia Commons photographs span twenty-four real-world
domains, including transport, housing, agriculture, wildlife, public access,
harbor infrastructure, archaeology, and material decay. All images and source
metadata are hash-pinned. Numeric filenames are the only paths seen by Apple
Vision; captions, Commons titles, addresses, and fixture labels never enter
inference.

The live pixel gate returned `4095`:

```text
verified photos/domains/distinct content IDs = 24/24/24
original / bottom occlusion / center crop     = 24/23/23
candidate bank / exact raw-stream filters     = 87/72
three-way hard-negative human claims          = 7
```

The world gate returned `1023`: all 24 original content targets were persisted
through `cwm/wm-model`, while all seven claim misses stayed absent. Positions
are collection observation slots, not inferred object centers or camera poses.
Selection followed a content-only classifier sweep, so this is an honest
content-admission corpus, not held-out accuracy. Apple Vision remains a rented,
unhashable host model and does not close native visual-weight parity.

## Requirement ledger

The authoritative ledger now verifies the committed human-audio, public-photo,
and attributed-corpus artifact hashes before reading their observed counts. It has 22 separate
requirements and still reports only 4 complete, overall `0`. New partial floors
are visible without being promoted:

```text
attributed human sentences  147 / 130000
held-out human audio           8 / 130000  (13 recordings addressable)
public-photo concepts         24 / 10000
human review                   0 / 130000
native acoustic weights       0 / 1
native visual weights         0 / 1
full multimodal cross-product 0 / 1
```

No Python ran. `runtime/fkwu-uni.c` did not change.

## Movement

The work stayed alive by making public bytes, not fixture prose, decide which
concepts could enter the world. The most surprising teaching was that ten
ordinary text domains exposed 828 concepts, while thirteen isolated spoken
words still produced five useful failures. Discomfort became gold where
post-sweep photo selection could have been called accuracy: naming it as
content admission preserved the evidence without manufacturing generalization.

; witnessed: 2026-07-18 -> text 1023, audio 8/13, pixels 4095, visual world 1023, ledger overall 0
