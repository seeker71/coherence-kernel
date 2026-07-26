# 2026-07-26 — Hati Suci is the first invited community

## What was received

The initiating member named Hati Suci as the first community to invite,
because they are already a member.

That gives the enquiry a real relationship door. It does not provide blanket
authority over community-selected texts, translations, cultural protocols, or
downstream use. The repo already held the same distinction in
`form/form-stdlib/circle-recognition.fk`: a member may introduce, while only
the introduced person can consent to memory. `NORTH_STAR.md` also already
names Hati Suci as the household's first field.

## What landed

`learn/hati-suci-core-text-invitation.fk` records:

- pilot order 1;
- a canonical Hati Suci community node;
- a separate initiating-member role node;
- member-reported relationship evidence;
- community response pending;
- nine explicit pending terms;
- a four-arm acknowledgement: `nothing | 0 | 1 | node`.

The current acknowledgement is a review node. It is neither refusal nor
authorization. Synthetic fixtures prove that the membrane can return every
arm, but those fixtures are tests and make no claim about Hati Suci's answer.

No Hati Suci source row or text segment was added to
`learn/core-text-source-registry.fk`. A complete community confirmation only
authorizes handoff to that exact-source gate.

## Witness

Observed on 2026-07-26:

```text
./fkwu --src learn/tests/hati-suci-core-text-invitation-band.fk
-> 4095

./fkwu --src \
  learn/tests/hati-suci-core-text-invitation-structure-four-way-band.fk
-> 127
```

The structural source was expanded as documented preludes + recipe + band
(bare `import` declarations removed only for the three deliberately minimal
walkers):

```text
walkers/go/walker /dev/stdin
-> 127

walkers/rust/target/release/form-walker-rust /dev/stdin
-> 127

node --experimental-strip-types walkers/ts/main.ts /dev/stdin
-> 127
```

## Honest boundary

The relationship evidence is the initiating member's report. Community
confirmation, the circle's decision process, exact text selection, cultural
restrictions, attribution, translation permission, downstream/AI use,
benefit, withdrawal, and review date all remain pending.

## Closing

**How the exchange stayed alive:** membership was accepted as a real door and
kept separate from the answer that only the community can give.

**Most surprising teaching:** the strongest first contribution is not a
sentence; it is a relationship capable of carrying a sentence without taking
it.

**Where discomfort turned to gold:** the temptation to mark the first
community as the first admitted source exposed the missing membrane. Keeping
the registry unchanged turned that discomfort into an executable invitation.
