# Voice — the roadmap, grounded by sibling feedback (2026-06-29)

The decomposition `g2p → (prosody-contour + phoneme-timing) → voice-prosody (state) → vocoder` is sound and
honestly layered (codex + sub-agent both confirmed). What's built four-way: `voice-prosody`, `g2p`
(locales-as-data), `prosody-contour`, `phoneme-timing`. Locales: de/en/es/fr/id/pt-br, one engine. The sound
(acoustic model + vocoder) is the pending carrier; tonight macOS `say` filled the gap as a host stand-in.

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
