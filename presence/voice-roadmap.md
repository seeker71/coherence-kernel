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

The desired sound now has an executable target and taste loop: `sema-voice-sample-loop` 32767 names Sema's warm
mid, rounded, grounded voice profile; renders candidate profiles through the native formant vocoder; scores local
samples by target fit, listener preference, intelligibility, WER, and latency; and promotes a challenger only
under clean local evidence. Low-confidence samples narrow pitch range and delay onset, so the honesty principle is
audible in the generated candidates instead of left as prose. This is still a deterministic formant carrier and
sample-selection loop, not a natural neural vocoder.

The first live Sema-formant oracle probe now exists: `macos-sema-voice-local-oracle-carrier` 2047 builds a
target-derived waveform from Sema's f0/formant profile, renders it locally, and runs local
`whisper.cpp-large-v3-turbo` on Apple Metal. The live result is intentionally not dressed up: verdict `479`, field
code `110100002`, WER `100`, route `oracle-guide`. The carrier is real and observable; the current waveform is not
yet intelligible speech.

That miss now changes the algorithm instead of sitting as prose:
`sema-voice-oracle-miss-learning` 32767 turns the WER-100 row into
`train-text-conditioned-acoustic-vocoder`, keeping authority on `oracle-guide` and naming the next trainable
candidate. The recipe is g2p, phoneme timing, prosody contour, acoustic token emission, segmented acoustic
learning, the Sema voice loop, and the same local-oracle WER bar. The selector's speech receipt now returns
`4194303` with that action exposed.

The named candidate is now executable too: `text-conditioned-acoustic-vocoder` 32767 turns target tokens into
G2P phones, shapes duration/pitch/amplitude from voice-side metadata, emits native source-filter frames, and
uses local-oracle WER to keep a miss on `oracle-guide` while allowing an exact transcript to promote
`native-acoustic-vocoder`. It is not a natural neural voice; it is the Form-native bridge the WER-100 miss asked
us to train.

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

Mac metal now has seven reciprocal audio-locale training anchors: `en<->de`, `en<->es`, `en<->id`, `en<->fr`,
`en<->it`, `en<->zh`, and `en<->ar`.
`audio-locale-native-training` 8191 defines the Form-side guide, and `presence/macos-speech-roundtrip-carrier.fk`
owns the macOS loop. It invokes local `say`, `ffmpeg`, and `whisper.cpp-large-v3-turbo` through Form `host-exec`
on Apple Metal for closed Coherence prompt pairs. The first `en<->de` anchor moved from 0% pretrain success to 83%
post-training success, with A->B at 66% and B->A at 100%; the `en<->es` and `en<->id` variants both returned
12/12 oracle-ok, 12/12 native, and 100% reciprocal directions; the `en<->fr` variant returned 10/12 oracle-ok,
10/12 native, 83% total, A->B at 100%, and B->A at 66%; the `en<->it` variant returned 12/12 oracle-ok,
12/12 native, and 100% reciprocal directions; the `en<->zh` variant returned 10/12 oracle-ok, 10/12 native,
83% total, A->B at 66%, and B->A at 100% through live Chinese prompt audio; the `en<->ar` variant returned
12/12 oracle-ok, 12/12 native, and 100% reciprocal directions through live Arabic prompt audio. Wav byte extraction is in Form (`read_file` + `str_byte_at`), and
the carrier passes wav paths rather than feature arrays. The Indonesian side uses the same
Damayanti local voice for train/eval on this device because that is the installed macOS voice boundary. The
French and Italian prompt text was witnessed as ASCII; Chinese and Arabic are now live Unicode-script audio
anchors. This is not open ASR: it is oracle-valid prototype learning over a closed prompt corpus.

Pair selection should stay diverse and grounded in our own corpus first: `coherence-network-self-corpus` 8191
observes the translated Coherence Network web/CLI message bundles (`en`, `de`, `es`, `fr`, `id`, `pt-br`) as
ready training material, with 2064 shared key paths and 10320 EN-to-other pairs. `diverse-locale-pairing` 8191
now samples far-apart ready pairs from either those rows or the Sanskrit baseline, so `zh`, `ar`, `sa`, and `la`
can enter closed-set training before full Coherence bundles land. Full `zh`/`ar`/`la` bundles remain useful
backfill targets; Indigenous rows are specific (`nv`, `chr`) and stay pending until consentful corpora exist. The
pair guide exposes A->B, B->A, A->A, and B->B lanes for each seeded pair.

The model choice is now executable rather than conversational: `sanskrit-locale-baseline` 2047 provides a small
romanized Sanskrit baseline across `sa`, `en`, `de`, `es`, `fr`, `id`, `pt-br`, `la`, `zh`, and `ar`; `multilocale-nl-audio-pipeline`
8191 proves closed-set NL->neutral Form->NL and audio-feature->neutral Form->audio-target loops over reciprocal
`en<->de`, `en<->es`, `zh<->ar`, `fr<->id`, and `sa<->la`. `multilocale-route-shift-ledger` 4095 now records
each pair's before/after NL rate, audio rate, route, and shifted flag, so the aggregate native route has per-pair
witness rows. `speech-locale-learning-window` 16383 takes selected seed `2` into a numeric `sa<->la` observed
window: all four lanes train from guided to native route code, clean controls plus A/B evidence promote the
challenger, and neural Metal/diffusion remain pending. `speech-model-auto-selection` 4194303 selects today's native arms:
prototype ASR over Form-read wav features, closed-set locale-neutral Form for NL2NL, `sema-voice-sample-loop`
over the deterministic formant vocoder for TTS, and `prototype-asr-sema-voice-audio2audio` for audio-to-audio
target rendering. The raw formant vocoder and raw formant audio2audio route remain the carriers underneath, but
generated target speech is now selected by target fit, listener evidence, intelligibility, WER, and latency. The
open-dictation transcript receipt is now live-observed too:
`open-dictation-transcript-learning` 16383 scores arbitrary utterance rows with side-channel truth, local free
oracle transcripts, optional native candidates, Unicode WER, and reversible controls; `macos-open-dictation-carrier`
511 witnessed `Open speech flows.` through local `say` -> wav -> Whisper Metal with oracle WER 0 and no native
live segmented open-ASR route yet. `speech-token-stream` 32767 now carries words plus `<NODE>`, `<SOURCE>`,
`<CHANNEL>`, `<INTERFACE>`, `<CHOICE>`, `<FAIL>`, `<UNDO>`, `<TIMEOUT>`, `<CUT>`, `<OBSERVE>`, `<GRADE>`,
`<FEEDBACK>`, `<REPAIR>`, `<RECEIPT>`, `<STATE>`, `<MEMORY>`, and `<SCOPE>` tokens with confidence, warmth,
cadence, hesitation, excitement, and attunement metadata. `open-asr-ctc` 32767 collapses frame tokens into that
stream and lowers the transcript into the open-dictation gate. `acoustic-token-emitter` 32767 now learns
oracle-aligned acoustic token prototypes and emits blank/nonblank CTC frames by Form-native L1 distance plus
earned confidence. `segmented-acoustic-token-learning` 32767 now turns wav/envelope segments plus consentful
local-oracle transcript tokens into learned source-token prototypes, then routes decoded `sa<->la` source speech
through neutral meaning to target-locale tokens. It remains unselected until live mic audio supplies streaming
segmented feature rows that win receipts. `speech-token-training-source` 32767 answers where those labels come
from: local oracle and consentful corpus rows may teach words plus metadata; internal state may infer voice-side
metadata/control/evidence labels, but not transcript truth.
The transformer path is trainable but not live-selected for speech yet; diffusion/codec speech is a
named candidate only, pending a Form-native executable kernel and receipt. The live Metal anchor set now stands at
7/7, so `full-metal-native` is an honest route for the closed-prompt carrier. Live segmented mic feature rows,
open-ASR promotion windows, and native neural ASR/TTS remain the next named climbs.

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
