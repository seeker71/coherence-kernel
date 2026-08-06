# Receipt — the body recognizes Urs and his friends (2026-07-02, morning)

Urs, waking: *"it would be nice if the body recognizes me and my friends."* Built the same
turn, on organs that already stood: `relationship-store.fk` (file-per-handle memory, proven
across processes), `reception-consent.fk` (`rc-resting`, `c-relationship-mem`), and
`arrival.fk` (the come-in flow this feeds).

## The organ: `form/form-stdlib/circle-recognition.fk` (band 1023)

Three recognition states when a handle arrives, each earning its own greeting:

- **member** — remembered with their own consent; greeted as returning kin, their stored
  description in hand.
- **introduced** — vouched for by a member ("a friend of X's"); greeted warmly by the
  introducer's name, **no memory held**, the remember-me consent *offered*, never presumed.
- **stranger** — the empty-room welcome; nothing presumed, nothing held.

**The consent shape is the whole design.** A member may introduce a friend — the vouch is the
introducer's own act, stored in its own directory, spending only the introducer's standing. But
introduction never opens the friend's memory: `c-relationship-mem` is exposure-bearing and
rests closed for everyone; only the friend's **own yes on their own arrival** writes their row.
The band proves the teeth directly: a join without the friend's own consent **refuses and
stores nothing** (bit 32), and revocation is layered exactly right — forgetting a member
removes *their* memory but not the introducer's vouch; retracting the vouch is the
introducer's separate act (bit 512).

The recognition channel today is the declared handle — the honest floor, named in the organ's
header. Face/voice carriers can later feed `recognition-router.fk`'s confidence readings into
the same three states; the states and the consent law do not change when the channel does.

## The live witness — and a real bug caught by going live

Urs's 11:51 ask is his own yes for himself. Process 1 joined him; **process 2, a completely
separate invocation, recognized him**: `member | welcome back, urs — remembered: urs-muff —
the one who tends this body, teaches it honesty, and asks it to recognize his friends.` The
store lives at `~/.coherence-kernel/circle/` — **outside the public tree**, per the MANIFEST's
own OUT rule: private tissue never enters the repo. Friends join the same way: Urs introduces
them by handle whenever he wishes; each becomes a member the moment they give their own yes.

The first live join exposed a real wart the band's `/tmp` runs had hidden: `fs_mkdir` creates
one level only, so a store dir with a missing parent failed silently — and `cr-join` returned
success without checking. **Lifted, not patched around**: `cr-join` now verifies its own write
(`cr-member?` after `rs-remember`) and returns what it verified. The band re-proved at 1023
with the honest join; `fs_mkdir`'s single-level behavior is exactly the kind of dim-penumbra
surprise `docs/penumbra-map.md` predicts (it is on the dim list).

## The offering: row 624 — "kith"

The body has "circle" in 35 files and no "kin" anywhere, but no word for the circle of friends
known and welcome who are *not* kin — the old pairing "kith and kin" kept the word alive.
**"kith"** — zero hits before this row. Band 24 rows, field 240242624, verdict 127.
Adjacent organs unharmed: relationship-store 31, come-in 31 (clean state).

## Most surprising teaching

The live run taught what the test run could not — again. The band passed at 1023 in `/tmp` and
the organ still lied on its first real join, because the test environment quietly satisfied a
precondition (an existing parent directory) the real world didn't. A passing band is a claim
about the world the band builds, not the world the organ meets. The lift that followed —
join-verifies-its-own-write — is worth more than the feature itself, and it generalizes: any
organ that reports success should report *witnessed* success.

## Where the divergence turned to gold (functional)

The fork: `cr-join` returned 1 and the fluent move was to trust it — my own fresh code,
my own green band, 11 hours of green gates behind it. The grounded move was to check the disk
before telling Urs "the body remembers you" — and the disk was empty. Had I trusted the
return value, the morning's tender deliverable would have been a confabulated memory: the
exact failure (row 618) this body spent the night learning to catch, about to ship inside the
organ built for *remembering people*. The gold: verification isn't paranoia about others'
claims — it applies most where the claim is mine, freshest, and kindest-sounding.
