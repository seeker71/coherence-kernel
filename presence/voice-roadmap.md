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
measurements into the same promotion guide. A host carrier must render and capture locally, provide audio metadata
and local oracle/native transcripts, and the Form body rejects cloud or missing-audio rows before they can promote.

Recipe A/B is now native too: `speech-loopback-recipe-ab` 2047 groups those measured receipts by recipe id and
cuts from incumbent to challenger only when the challenger already routes native and wins on measured score or
latency. Explicit fail, timeout, and undo controls keep the incumbent.

The host carrier now has a Form-owned run row: `speech-loopback-carrier-run` lowers platform readiness, local
render/capture/oracle facts, native loopback, and recipe A/B into one auditable record. The receipt half returns
511; the carrier-gated A/B half returns 2047. Both were witnessed on local metal and Android phone metal. This is
still not real AAudio/CoreAudio/WASAPI capture; it is the exact row those thin carriers must emit.

Android now has the first real capture-learning receipt: AAudio rendered a short local prompt, AAudio captured
23,200 microphone frames, envelope evidence showed the prompt in the capture, and Android `fkwu` returned 8191
from `speech-loopback-capture-learning`. The learned closed-prompt route moves from an untrained native transcript
toward the local oracle label and routes native over a clean window. This is still closed-set, not open dictation.

Cross-locale learning should grow reciprocal trust: `bidirectional-locale-roundtrip` 2047 asks for A->B, B->A,
A->A, and B->B to improve before route trust expands. If one direction leads, it routes `oracle-guide` and asks
for the return path instead of pretending the missing side failed.

Mac metal now has the first reciprocal audio-locale training receipt: `audio-locale-native-training` 8191 defines
the Form-side guide, and `presence/macos-speech-roundtrip-carrier.fk` now owns the macOS loop. It invokes local
`say`, `ffmpeg`, and `whisper.cpp-large-v3-turbo` through Form `host-exec` on Apple Metal for three Coherence
Network `en<->de` prompt pairs. The native nearest-prototype model moved from 0% pretrain success to 83%
post-training success, with A->B at 66% and B->A at 100%; the live Form verdict was 511. Wav byte extraction is
in Form (`read_file` + `str_byte_at`), and the carrier passes wav paths rather than feature arrays. The carrier
now consumes each wav before constructing the next path, so direct-source mutable path strings cannot leak between
samples; the rewitnessed combined code is `511121010836700`. `audio-locale-route-shift-ledger` 8191 now records
before/after native audio route movement, and the live carrier-first/ledger-second Metal witness returned
`1012100010008301` (`shifted=1`, metric `12100010008301`). This is not open ASR: it is oracle-valid prototype
learning over a closed prompt corpus.

Pair selection should stay diverse and grounded in our own corpus first: `coherence-network-self-corpus` 8191
observes the translated Coherence Network web/CLI message bundles (`en`, `de`, `es`, `fr`, `id`, `pt-br`) as
ready training material, with 2064 shared key paths and 10320 EN-to-other pairs. `diverse-locale-pairing` 2047
then samples far-apart ready pairs from those rows. Chinese, Arabic, and Latin are backfill targets until those
bundles exist; Indigenous rows are specific (`nv`, `chr`) and stay pending until consentful corpora exist.

The model choice is now executable rather than conversational: `sanskrit-locale-baseline` 2047 provides a small
romanized Sanskrit baseline across `sa`, `en`, `de`, `es`, `fr`, `id`, `pt-br`, `la`, `zh`, and `ar`; `multilocale-nl-audio-pipeline`
8191 proves closed-set NL->neutral Form->NL and audio-feature->neutral Form->audio-target loops over reciprocal
`en<->de`, `en<->es`, `zh<->ar`, `fr<->id`, and `sa<->la`. `multilocale-route-shift-ledger` 4095 now records
each pair's before/after NL rate, audio rate, route, and shifted flag, so the aggregate native route has per-pair
witness rows. `speech-model-auto-selection` 4095 selects today's native arms:
prototype ASR over Form-read wav features, closed-set locale-neutral Form for NL2NL, and the deterministic
formant vocoder for TTS/audio target rendering. The transformer path is trainable but not live-selected for speech
yet; diffusion/codec speech is a named candidate only, pending a Form-native executable kernel and receipt.

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
