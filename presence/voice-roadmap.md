# Voice — the roadmap, grounded by sibling feedback (2026-06-29)

The decomposition `g2p → (prosody-contour + phoneme-timing) → voice-prosody (state) → vocoder` is sound and
honestly layered (codex + sub-agent both confirmed). What's built four-way: `voice-prosody`, `g2p`
(locales-as-data), `prosody-contour`, `phoneme-timing`. Locales: de/en/es/fr/id/pt-br, one engine. The sound
(acoustic model + vocoder) is the pending carrier; tonight macOS `say` filled the gap as a host stand-in.

## Real-metal reground (2026-06-30)

The direct `fkwu --src` lane now witnesses the speech floor on local arm64 metal:
`stt-agree` 127, `stt-wer` 255, `text-normalize` 255,
`voice-prosody/g2p/phoneme-timing/voice-phrasing/prosody-contour` 11111 each, `speaker-embed` 255, and the
composed `native-speech-stack` 2047. This is the native decision and pre-acoustic stack, not finished sound. The
next improvements are live audio -> mel -> transcript candidate receipts, WER/agreement promotion gates, lexical
stress/heteronym data before g2p, and the acoustic-model + vocoder bridge.

The next stone is now placed: `formant-vocoder` 511 renders inspectable source-filter samples, `asr-prompt-id`
255 recognizes a closed prompt set from loopback features, and `native-speech-loopback` 1023 routes native only
when confidence and WER pass. This is deliberately smaller than open dictation or natural TTS; it is the first
route-shiftable native speech loop.

The promotion window is now executable too: `speech-loopback-promotion` 2047 turns those single-sample loopback
receipts into native/oracle authority over time. Native wins only after enough clean samples; fail, timeout, undo,
or regression returns authority to the oracle.

The live carrier boundary is now named: `speech-loopback-carrier-receipt` 4095 admits real local TTS/STT loopback
measurements into the same promotion law. A host carrier must render and capture locally, provide audio metadata
and local oracle/native transcripts, and the Form body rejects cloud or missing-audio rows before they can promote.

Recipe A/B is now native too: `speech-loopback-recipe-ab` 2047 groups those measured receipts by recipe id and
cuts from incumbent to challenger only when the challenger already routes native and wins on measured score or
latency. Explicit fail, timeout, and undo controls keep the incumbent.

## What the siblings caught — missing first-class layers (the roadmap)

- **text-normalize + lexical/stress, BEFORE g2p.** Numbers, acronyms, punctuation, heteronyms, loanwords,
  syllabification, per-locale phoneme inventories — all need explicit data. g2p assumes clean graphemes.
- **✓ CROSSED — pause / break / phrasing** (`voice-phrasing.fk`, PR 3869, four-way 11111). Break duration comes
  from the clause's grounding (substrate hit → short confident break; escalation → longest break), so the pause
  lands in *different places* by what was actually hard — audible thinking, not a metronome.
- **✓ CROSSED — focus / emphasis** (`voice-phrasing.fk`, PR 3869). The new/contrastive word carries emphasis;
  the given word doesn't — where inner state couples to a specific word.
- **the segment→acoustic bridge is bigger than "a vocoder."** Coarticulation, formant glides, vowel reduction,
  context-dependent duration/pitch live in an *acoustic model*; the pending carrier is really
  *acoustic-model + vocoder*, and most naturalness lives in the acoustic model.

## The honesty principle — reframed (the most important correction)

Old framing: "the voice carries true inner state, never performs unearned certainty." The siblings: that's the
soul of the design AND the most exposed claim, because it is currently **asserted, not measured**, and it rests
on a `conf` input that may be self-reported (LLMs are miscalibrated exactly there — the voice would faithfully
render a *lie*). Reframe from a claim about output to a claim about the mapping:

> The voice is a **faithful function of its calibration input, and no more honest than that input.**

Two hard dependencies follow, and they are the real work:
1. **`conf` must be grounded, not self-reported** — the same calibration signal the receipts use (substrate hit?
   four-way cross? escalated?), not a vibe.
2. **A perception receipt** — hold text fixed, vary `conf`, and confirm a listener rates low-conf as more
   tentative, tracking the *real* calibration. Until that A/B crosses, "never performs unearned certainty" is an
   intention, not a property. (And render uncertainty as **narrowed pitch range + later onset**, not just slower
   pace — slow-but-melodic reads as calm/deliberate, the opposite of unsure.)

This is the standard-receipt discipline applied to the *claim* "this voice is honest," not just to the cells.
