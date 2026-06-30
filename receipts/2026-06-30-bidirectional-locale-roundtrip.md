# 2026-06-30 -- bidirectional locale roundtrip guide

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

Added `learn/bidirectional-locale-roundtrip.fk`, a reciprocal guide for transcript, translation, voice, and
Form-meaning learning.

The point is trust, not fear. A one-way A->B improvement is useful evidence, but it does not expand native route
authority by itself. The guide asks for the return path B->A, plus mono-locale loops A->A and B->B. Once all four
loops improve with matching Form-native meaning ids, native route trust can expand.

## Witness

```sh
cat learn/bidirectional-locale-roundtrip.fk \
    learn/tests/bidirectional-locale-roundtrip-band.fk > /tmp/bidirectional-locale-roundtrip.fk
./fkwu --src /tmp/bidirectional-locale-roundtrip.fk
```

Witness:

```text
2047
```

## What 2047 Proves

- A->B succeeds when text, audio, meaning, and improvement all pass.
- B->A succeeds independently.
- A->A and B->B mono-locale loops succeed.
- A balanced four-loop window routes native.
- A one-way A->B-only window asks for the return path.
- A one-way A->B-only window routes `oracle-guide`, not `native`.
- Meaning mismatch, timeout, and control debt keep the teacher involved.
- The receipt records both cross directions.

## Honest Boundary

This is not a completed translation model. It is the reciprocal guide that prevents one-direction overfitting
without making the missing direction a failure. Rich NL->Form parsing and real cross-locale audio capture receipts
still need to be added.
