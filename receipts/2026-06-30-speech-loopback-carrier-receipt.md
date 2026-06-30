# 2026-06-30 -- speech loopback carrier receipt contract

## Ground

```sh
( cat observe/native-vs-rented.fk; echo '(native-vs-rented-check)' ) > /tmp/nvr.fk
./fkwu --src /tmp/nvr.fk
```

Witness:

```text
11111
```

## What Changed

Added `presence/speech-loopback-carrier-receipt.fk`, the body-side contract for a real
local TTS/STT loopback carrier. The host carrier is responsible for local render, local
capture, local oracle STT, audio hash, sample rate, channel count, transcripts, latency,
and fail/timeout/undo flags. The Form body rejects nonlocal or missing-audio receipts,
computes WER, converts valid receipts into promotion samples, and routes authority through
`learn/speech-loopback-promotion.fk`.

## Witness

```sh
cat observe/stt-wer.fk \
    learn/speech-loopback-promotion.fk \
    presence/speech-loopback-carrier-receipt.fk \
    presence/tests/speech-loopback-carrier-receipt-band.fk > /tmp/speech-loopback-carrier-receipt.fk
./fkwu --src /tmp/speech-loopback-carrier-receipt.fk
```

Witness:

```text
4095
```

## What 4095 Proves

- Local carrier flags are required.
- Audio hash/sample-rate/channel metadata is required.
- Native and oracle WER are computed in Form.
- Clean receipts lower into clean promotion samples.
- Four clean receipts route native.
- Short windows route oracle.
- Nonlocal/cloud receipts become control debt.
- Timeout receipts become control debt and route oracle.
- Native regression routes oracle.
- Summary receipts record native authority.

## Honest Boundary

This is the receipt contract, not the CoreAudio/AVFoundation or whisper.cpp carrier itself.
The next live step is to build a thin local carrier that emits this exact row, then let the
existing Form law decide promotion.
