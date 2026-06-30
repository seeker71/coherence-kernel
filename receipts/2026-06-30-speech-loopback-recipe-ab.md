# 2026-06-30 -- speech loopback recipe A/B

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

Added `learn/speech-loopback-recipe-ab.fk`, a Form-native A/B decision cell over
real local speech-loopback receipt windows. Each arm is a recipe id plus carrier
receipts; the body computes the existing promotion summary, latency score, and
then decides whether to keep the incumbent or cut to the challenger.

Fail, timeout, and undo are explicit route controls. They keep the incumbent even
when the challenger window otherwise wins. Nonlocal/cloud receipts remain control
debt through `presence/speech-loopback-carrier-receipt.fk`, so a cloud-backed row
cannot sneak into a native cutover.

## Witness

```sh
cat observe/stt-wer.fk \
    learn/speech-loopback-promotion.fk \
    presence/speech-loopback-carrier-receipt.fk \
    learn/speech-loopback-recipe-ab.fk \
    learn/tests/speech-loopback-recipe-ab-band.fk > /tmp/speech-loopback-recipe-ab.fk
./fkwu --src /tmp/speech-loopback-recipe-ab.fk
```

Witness:

```text
2047
```

## What 2047 Proves

- A/B arms preserve recipe id, receipt count, native score, debt, route, and latency.
- Incumbents can route native with margin slack.
- A challenger with better measured native score cuts over.
- Equal native score can cut to a lower-latency challenger.
- Short challenger windows keep the incumbent.
- Cloud/control debt blocks challenger cutover.
- Fail, timeout, and undo controls keep the incumbent.
- The decision receipt records the chosen recipe and measured route evidence.

## Honest Boundary

This is the recipe choice law, not the live host mutation. The next carrier step is
to feed multiple TTS/ASR recipe ids into this cell from local loopback runs, then
apply only the returned cut/keep decision to the live registry.
