# Native Open ASR Token Stream Receipt

Date: 2026-06-30

This answers the missing-token-stream question directly. Speech now has a
Form-native mixed token stream that can carry ordinary transcript words and
special structural tokens:

- identity/provenance: `<NODE>`, `<SOURCE>`
- route/contract: `<CHANNEL>`, `<INTERFACE>`
- control: `<CHOICE>`, `<FAIL>`, `<UNDO>`, `<TIMEOUT>`, `<CUT>`
- evidence: `<OBSERVE>`, `<GRADE>`, `<FEEDBACK>`, `<REPAIR>`, `<RECEIPT>`
- memory/scope: `<STATE>`, `<MEMORY>`, `<SCOPE>`
- word payloads: `<WORD>`

Each token row carries voice-side metadata:

- confidence
- warmth
- cadence
- hesitation
- excitement
- attunement

## What changed

- Added `observe/speech-token-stream.fk`.
- Added `observe/tests/speech-token-stream-band.fk`.
- Added `observe/open-asr-ctc.fk`.
- Added `observe/tests/open-asr-ctc-band.fk`.
- Wired `native-open-asr-ctc` into `learn/speech-model-auto-selection.fk` as a native, trainable ASR candidate.

The CTC decoder collapses frame tokens, emits a speech token stream, extracts
free transcript text, carries minimum confidence into the native candidate row,
and lowers into `learn/open-dictation-transcript-learning.fk`.

## Witnesses

Token stream:

```sh
cat observe/speech-token-stream.fk \
    observe/tests/speech-token-stream-band.fk \
  > /tmp/speech-token-stream.fk
./fkwu --src /tmp/speech-token-stream.fk
```

Observed:

```text
32767
```

Native CTC decoder into open dictation:

```sh
cat observe/stt-wer.fk \
    learn/speech-loopback-promotion.fk \
    form/form-stdlib/observed-auto-learning.fk \
    learn/open-dictation-transcript-learning.fk \
    observe/speech-token-stream.fk \
    observe/open-asr-ctc.fk \
    observe/tests/open-asr-ctc-band.fk \
  > /tmp/open-asr-ctc.fk
./fkwu --src /tmp/open-asr-ctc.fk
```

Observed:

```text
32767
```

Speech model selector:

```text
16383
```

## Boundary

This is a native open-ASR decoder candidate and shared token stream, not a live
audio encoder. The missing live climb is audio frames -> native acoustic frame
tokens. Until that exists and wins receipts, `prototype-asr` remains selected.
