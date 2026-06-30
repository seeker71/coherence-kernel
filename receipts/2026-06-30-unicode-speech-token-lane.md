# 2026-06-30 -- unicode speech token lane

## What Changed

Moved transcript token production into `observe/stt-wer.fk` as shared
`sw-tokens`, and changed `presence/macos-speech-roundtrip-carrier.fk` to use
that Form-native lane.

The tokenizer stays compact on purpose: the broad observed-sweep preludes still
fit under the current direct-source function table, so no C seed growth was
needed to carry Unicode transcript evidence.

The old carrier tokenizer only accepted ASCII alphanumerics. That made
Chinese, Arabic, and accent-rich transcript evidence unsafe: it could be
dropped or undercounted before WER had a chance to measure it.

## Witness

```sh
cat observe/stt-wer.fk \
    observe/tests/stt-tokenize-unicode-band.fk > /tmp/stt-tokenize-unicode.fk
./fkwu --src /tmp/stt-tokenize-unicode.fk
```

Output:

```text
4095
```

The band verifies:

```text
ASCII punctuation still splits/lowercases
accented Latin remains inside a word token
CJK ideographs become one token per character
CJK punctuation is a delimiter
CJK WER sees one substitution in four characters
Arabic words stay grouped by spaces
Arabic punctuation is a delimiter
exact Unicode transcripts pass zero-WER acceptance
changed CJK transcripts fail a tighter WER ceiling
```

Existing receipts still hold:

```text
stt-wer-band -> 255
macos-speech-roundtrip-carrier-band -> 511
metal-observed-sweep-bridge-band -> 32767
```

## Honest Boundary

This lands Unicode transcript measurement, not live Unicode speech anchors.
The five current Metal anchors remain the witnessed ASCII/Latin closed-prompt
pairs. The next live climb is to run Chinese and Arabic carrier variants now
that the transcript side can keep their evidence alive.
