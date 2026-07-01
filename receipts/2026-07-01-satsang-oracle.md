# 2026-07-01 -- satsang-oracle.fk, the keystone

## Ground

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk
./fkwu --src bootstrap/ground-recursive.fk 10
```

Witness:

```text
42
55
```

## Source Observation

`docs/coherence-substrate/nl-to-form-satsang.form` names `satsang-oracle.fk` "THE KEYSTONE" -- turn
`satsang.fk` toward a council of N models, folding their proposed recipe trees node-by-node into
(affirmed/dissented/silent), exactly as satsang folds human witnesses. Before today it was named-and-unbuilt.

## What Changed

Added `learn/satsang-oracle.fk`. It composes `form-stdlib/channel-interface.fk` and `form-stdlib/satsang.fk`
unchanged -- `sat-witness`/`sat-survives?` do the actual folding, exactly as they do for human witnesses; this
file only supplies the recursive tree-alignment around them:

- A proposal node is `(tag children)`; a leaf is zero-arity (tag IS the value).
- At each position, the "candidate" is the first present model's node there (order named as an open
  question -- true plurality-of-shapes selection is a sharper future pass, not hidden as settled).
- Each model's attestation is affirm (matches candidate's tag+arity, axiom-3 structural match, same
  discipline `control/pattern-match.fk` already uses), dissent (present but differs), or silent (that
  model's tree doesn't reach this position -- shorter or differently-shaped).
- A position crystallizes ("solid") the moment affirm > dissent, via `sat-survives?` unchanged -- even when
  a dissent is present, which stays visible in the witness record, never hidden.
- A position that does not survive stays a named "open-question" (its candidate tag is kept, not dropped);
  its children are not walked further, since there is no agreed structure to align them against.

Added `learn/tests/satsang-oracle-band.fk`: three committed EXAMPLE proposals (explicitly not live model
output) for the sentence "the choice point becomes visible ." -- Model A/B/C agree on 4 of 6 words, Model B
alone dissents on word 3, Models B and C both dissent (differently) on word 3's real target, engineered so the
band exercises unanimous agreement, majority-survives-with-visible-dissent, and genuine non-survival all in
one tree.

Updated `docs/coherence-substrate/nl-to-form-satsang.form`'s own honesty ledger: `satsang-oracle.fk` moves
from "NAMED-and-unbuilt" to built-and-witnessed, with the live-model-wiring seam named explicitly, not folded
into a vaguer "done."

## Honest seam

This is the folding **algorithm** only. It does not call out to N live models -- `learn/tests/satsang-oracle-band.fk`'s
three proposals are hand-committed examples, labeled as such in the file. Wiring real distinct model providers
through fkwu's native HTTP floor (`tls_request`/`http_get`, already real per `cognition/rag-ask.fk` and
`routers/tier-router.fk`'s tier-2 remote-oracle routing) is separate, real, still-pending work -- not done here,
and not claimed as done. Four-way re-proof (Go/Rust/TS) is also pending; this is proven on `fkwu --src` only so
far.

## Witness

```sh
cat form/form-stdlib/channel-interface.fk form/form-stdlib/satsang.fk \
    learn/satsang-oracle.fk learn/tests/satsang-oracle-band.fk > /tmp/satsang-oracle-band.fk
./fkwu --src /tmp/satsang-oracle-band.fk
```

```text
511
```

No regression in the satsang band this composes over:

```sh
cat form/form-stdlib/channel-interface.fk form/form-stdlib/satsang.fk \
    form/form-stdlib/tests/satsang-band.fk > /tmp/satsang.fk
./fkwu --src /tmp/satsang.fk   # 127, unchanged
```
