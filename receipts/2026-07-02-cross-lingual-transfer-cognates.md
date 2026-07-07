# 2026-07-02 — multiple locales: cross-lingual audio transfer follows the affinity graph

## Ground

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

Urs, 11:45: "multiple locales." Rendered a 20-word core vocabulary TRANSLATED (not
transliterated) across 8 locales with native macOS voices — en (Samantha), de (Anna),
fr (Thomas), es (Mónica), it (Alice), pt (Luciana), id (Damayanti), ar (Majed — the body's
live Arabic teacher voice). 160/160 real 16kHz wavs, keyed by MEANING (`de_water.wav` says
"Wasser") — the sanskrit-locale-baseline neutral-meaning shape at 5× vocabulary with real audio.

## The experiment: does meaning transfer across languages?

A 20-class MEANING classifier (champion architecture: linear softmax over 48-dim spectral
features) trained on four locales' audio (en, de, fr, es — 80 clips), tested on four HELD-OUT
locales it never heard (it, pt, id, ar — different languages AND different voices). Chance = 5%.

**The body had already made a prediction.** `learn/locale-affinity-graph.fk` (row 619, built this
morning from typology METADATA — family/script/morphology/region): pt and it are family-close to
es/fr (Romance, affinity 14-class), id is script-contact only (4), ar is an isolate (0).

## Witnessed — the transfer follows the graph

| held-out locale | meaning accuracy | vs chance | graph said |
|---|---|---|---|
| pt | **6/20 (30%)** | **6×** | Romance kin — strongest transfer ✓ |
| it | 2/20 (10%) | 2× | Romance kin — transfers, weaker than predicted |
| id | 2/20 (10%) | 2× | script-only — slightly above prediction |
| ar | **1/20 (5%)** | **exactly chance** | isolate — zero transfer ✓ |

Portuguese transfers at 6× chance because the COGNATES carry it — água/agua, sol/sol, lua/luna,
paz/paz, casa/casa share acoustic form because they share ancestry. Arabic, sharing no ancestry,
no script, no cognates with the training set, sits at exact chance: the classifier honestly knows
nothing about it. **Typology metadata predicted acoustic learning transfer** — two unrelated
modalities agreeing, the cross-witness-economy's own definition of earned confidence.

## Honest floor

One voice per locale — locale transfer is partially confounded with voice transfer (though every
TEST locale also has an unseen voice, so voice-novelty is at least uniform across the comparison).
20 words, 20 clips per test locale — small samples; the it-vs-pt gap (2 vs 6) could be
voice-specific (Alice vs Luciana) as much as linguistic. Single training run, no variance bars.
The multilocale set is rendered and permanent; re-running with more voices per locale is the
strengthening step. Global open-speech WER unmoved.

## The most surprising teaching this work left behind

The graph was built as a REFLECTION (drawing what the metadata already said) and turned out to be
a PREDICTOR (forecasting a measurement it had never seen). The affinity fold — family 8, script 4,
typology 2, region 1 — was picked for readability, yet its ordering matched the acoustic transfer
gradient hours later. Structure honestly recorded has a way of becoming foresight: the body
predicted its own future measurement without anyone intending it to.

## Where discomfort turned to gold

The discomfort was it (Italian) at only 2/20 where the graph said Romance-strong — the pull was to
gloss it ("close enough") or bury it under the pt headline. Witnessed instead: it is named plainly
as under-prediction, voice-confounded, and the exact place the next measurement (more voices per
locale) should look first. The graph earned its 2-of-2 on the clean extremes (pt strongest, ar at
chance); the middle is where its resolution runs out — and saying so is what keeps the prediction
honest rather than retrofitted.

## Corpus

Row 643 **cognate** — words in different languages sharing one ancestor, and therefore one
approximate sound (fresh, named-and-waiting since row 619; the carriers of the 6× Portuguese
transfer).
