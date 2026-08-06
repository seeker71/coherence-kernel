# Native inquiry derivation replay

Date: 2026-07-22

## Named gap

Earlier inquiry rounds counted generated rows but did not prove that a returned
answer node could itself become the subject of every inquiry lane. Their proof
vectors also used ordinary labels without one executable replay contract for
`nothing`, `0`, `1`, an alternative node, and the reason for each outcome.

## Built movement

`cognition/native-inquiry-derivation.fk` encodes a derivation path as a base-8
node ID rooted at `1`. Digits `0..6` are what, where, when, who, how, which, and
why. Digit `7` is invalid. The decoder therefore validates emitted nodes without
a static answer table, and the same inquiry rule can extend any valid result.

Each derivation receipt carries:

```text
[node, parent, lane, depth, interface, channel, relationship, shape, seal]
```

Replay derives every expected field again and compares each one. The seal is
additional evidence, not the sole admission gate.

Each proof vector carries:

```text
[nothing, 0, 1, alternative-node, acknowledgement, reason, receipt, steps]
```

Reasons are `0 complete`, `1 unknown node`, `2 budget exhausted`, `3 tampered`,
and `4 unknown lane`.

## Live observation

```text
./fkwu --src cognition/tests/native-inquiry-derivation-band.fk
-> [nothing, 0, 1, 9, 7, 49, 56, 56, 0, 3, 1, 2, 4, 4,
    [nothing, 0, 1, 9, 8, 0,
      [8, 1, 0, 1, 2000, 3007, 4011, 129296, 6446975], 1],
    [nothing, 0, 1, 65, 64, 0,
      [64, 8, 0, 2, 2000, 3056, 4088, 131326, 6548961], 1],
    1]

./fkwu --src cognition/tests/native-inquiry-derivation-tamper-band.fk
-> [nothing, 0, 1, 1, 1, 0, 0, 3, 1]
```

The main witness means: seven first nodes accepted, 49 follow-up inquiries
accepted, all 56 receipts replayed, all 56 alternatives validated, the tampered
receipt returned explicit `0/reason=3`, unknown/budget/lane branches returned
`nothing/reasons=1/2/4`, four live observations entered the runtime framebuffer,
and the final success was `1`.

The tamper witness changes only `shape` while retaining the original seal.
Replay confirms the untouched receipt (`1/reason=0`) and rejects the changed
receipt (`0/reason=3`).
