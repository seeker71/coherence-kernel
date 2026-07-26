# 2026-07-26 — Hati Suci is the first invited community

## What was received

The initiating member named Hati Suci as the first community to invite,
because they are already a member.

The initiating member then named Satsang as a recognized circle at Hati Suci.
They confirmed that Satsang holds both roles: receiving/witnessing the
invitation and selecting/approving the first material. This establishes who
may make the decision; it does not fabricate a decision about any exact text.

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
- a distinct canonical Hati Suci Satsang circle node;
- a separate initiating-member role node;
- member-reported relationship evidence;
- member-reported Satsang-circle recognition;
- Satsang role `both`: receive/witness and select/approve;
- first exact material selected: Giles' "The Light Hubs";
- member-witnessed scoped permission from Giles;
- no additional sacred, cultural, or community-use restrictions;
- no additional benefit or reciprocity condition;
- no scheduled review, with re-witness on change;
- all nine terms complete;
- current action: final Satsang ratification;
- a four-arm acknowledgement: `nothing | 0 | 1 | node`.

The current acknowledgement is a concrete final-ratification action node, not
a generic review marker. Its four replies are: no observed ratification,
decline/require changes, ratify the complete packet, or counteroffer a precise
amendment.

Source row 13 identifies "The Light Hubs" by exact SHA-256 as metadata-only.
No Hati Suci text segment was added. The recorded withdrawal right cannot be
honestly implemented by publishing the body into immutable Git. The cultural
restriction, benefit, and freshness terms are confirmed. Final Satsang
ratification and a revocable carrier still cross the gate before the complete
surface enters.

## Witness

Observed on 2026-07-26:

```text
./fkwu --src learn/tests/hati-suci-core-text-invitation-band.fk
-> 4095

./fkwu --src \
  learn/tests/hati-suci-core-text-invitation-structure-four-way-band.fk
-> 127

./fkwu --src \
  learn/tests/hati-suci-core-text-next-action-witness.fk
-> [3043010001, 3041003001, 9, nothing, 0, 1, 3044010001, 20260726]
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

The relationship, Satsang recognition, both-role evidence, and selection of
"The Light Hubs" are the initiating member's reports. The same member reports
Giles' scoped permission; no direct signed author artifact is fabricated.
The same member reports no additional cultural restriction or benefit
condition. No scheduled review is required; change triggers re-witness. Giles'
full name, revocable carrier, original date, and original carrier remain open.
Final Satsang ratification has not yet been reported.

## Closing

**How the exchange stayed alive:** membership was accepted as a real door and
kept separate from the answer that only the community can give.

**Most surprising teaching:** the strongest first contribution is not a
sentence; it is a relationship capable of carrying a sentence without taking
it.

**Where discomfort turned to gold:** the temptation to mark the first
community as the first admitted source exposed the missing membrane. Keeping
the registry unchanged turned that discomfort into an executable invitation.
