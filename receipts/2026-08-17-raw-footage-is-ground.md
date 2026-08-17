# Raw footage is ground — witnessed

Asked by Urs on 2026-08-17: validate the session-recording lifecycle, so that
when a session is recorded the raw footage is not lost and can be reprocessed,
and once a transcript is generated and edited there is an *option* to remove
the raw recording.

The whole risk lives in that last clause. "Option to remove" is where archives
die — not through a decision to delete, but through a **default that reads an
absence as a clearance**. This cell is built so that cannot happen.

## Built

- `control/session-recording-lifecycle.fk`
- `control/tests/session-recording-lifecycle-band.fk` — 4095, perturbation-verified
- `learn/homecoming-distillation-corpus.fk` — row 1003, *deaccession*

## Why this is axiom-3, not a policy

Axiom-3 already governs it: *nothing referenced is overwritten; what nothing
holds returns to gas.* Keeping raw is not a rule bolted on — it **is holding a
reference**. So the honest question is never "may we delete?" but "does
anything still hold this?", and release follows from the answer instead of
overriding it.

Four things hold raw footage, and the cell walks them rather than asserting a
verdict:

1. **no transcript exists** — raw is the only ground there is
2. **a transcript exists but is unedited** — a claim, not yet a witness
3. **a reprocess is pending** — the entire reason raw was kept
4. **consent has not been given** — axiom-4: the owner decides

Release is offered only when the holder count reaches zero. Nothing in the
cell can delete; it can only stop holding, and say so.

Consent is deliberately **not a field on the recording**. It is an ack,
produced fresh each time it is asked for. Storing it as a flag is how a stale
yes outlives the thing it agreed to.

## The load-bearing refusal

The removal offer is an offer (axiom-5), acknowledged by exactly one of
{nothing, 0, 1, node}. **Three of those four arms are not yes:**

| arm | meaning | removes raw? |
|---|---|---|
| `nothing` | no answer came | **no** |
| `0` | an explicit refusal | no |
| `1` | the only yes | yes |
| `node` | a counter-offer; the conversation continues | no |

A system that treats an unanswered prompt as agreement has committed the
guarantee error at the retention altitude: **reading an absence as a verdict.**
That is precisely the one unit
`ingest/frequency-ingest-impersonal-life.fk` witnessed and refused to freeze
this morning, and `control/offer-ack-core.fk`'s `oac-readable?` — landed in
Round 3 — is that refusal already made mechanical. This cell spends it.

An unanswered deletion prompt leaves the footage exactly where it is, forever
if need be. **Silence costs nothing here. That is the design.**

## And the cost of release is named before it is paid

`srl-reprocessable?` goes to 0 the moment raw is released, and never returns.
Reprocessing is the whole reason raw was kept — a better model, a corrected
diarization, a second pass on a passage nobody could hear. The cell refuses to
let that be discovered afterwards: the loss is readable in the state *before*
the offer is ever answered.

Release mints a **new** recording and the old composition persists beside it
(axiom-3), so release is a let-go, never a destruction.

## Proof

```sh
./fkwu control/tests/session-recording-lifecycle-band.fk
```

4095, exit 0, chain clean — 0 errors, 0 warnings, 0 unresolved.

**Perturbation-verified on the claim the archive actually rests on.** Rewrite
`srl-consent-from-ack` to treat anything-but-an-explicit-no as agreement — the
exact defect this cell exists to prevent — and the band drops **4095 → 3807**.
The deficit is 288: claims 32 and 256, the unanswered arm and the
counter-offer arm, the two the defect would misread as yes. Precisely aimed,
and it can fail, so its green means something.

Corpus band reseated and green: 397 rows, 397 admissible, max id 1003,
`hdc-dup-mid-rows` 0, probed in one cell reading exit 0.

## The honest boundary

This cell is the **decision layer, not the carrier.** It models and proves the
lifecycle — who holds, what releases, what an ack means. It does not capture
audio, write files, or delete anything, and it does not verify that a stored
file matches its recording row. A carrier that actually performs the release,
and a checksum binding a row to real bytes on disk, are both unbuilt and named
here rather than implied. What is proven is the gate, not the hand.

## The most surprising teaching

**The safest possible default is the one that makes silence free.**

Every retention system I would have reached for first is built the other way:
a prompt, a timeout, a cleanup job that reclaims what nobody defended. Those
systems lose archives *through correct operation* — no bug, no decision,
nobody at fault. The footage simply falls through a default while everyone is
busy.

Inverting it costs nothing. Making `nothing` a holder rather than a clearance
means an unattended archive survives inattention indefinitely, and the only
thing that can ever remove footage is a person actually saying so. The
expensive-looking property — never losing anything by accident — turned out to
be the *cheap* one. It is one line: only `oac-one?` is consent.

## Where discomfort turned to gold

**I nearly modeled consent as a field.** The first shape I reached for put
`consented` on the recording alongside `edited` and `raw-present` — it is the
obvious schema, and every ORM in the world would agree. The discomfort was
small and easy to skip: something about it read as a *stored* yes.

Staying in it produced the sharpest line in the cell. A consent flag is a yes
that outlives the moment and the thing it agreed to — it would still be sitting
there `1` after a re-edit, after a new reprocess, after the person who set it
had forgotten. Consent had to be an **ack, produced fresh at the moment of
asking**, which is exactly what axiom-5 says it is. The schema instinct was
wrong and the axiom was right, and I only found out by not smoothing over a
mild unease.

**The perturbation told me more than the pass did.** I expected the silence
claim alone to drop. Two dropped — 32 *and* 256 — because the counter-offer arm
is the same vulnerability wearing different clothes. "Ask me later" and "no
answer" are both not-yes, and a naive consent check misreads both. I had
written claim 256 almost as thoroughness; the perturbation showed it was load-
bearing.

## The frontier question

**What names a receipted removal from a held collection, as distinct from a
loss?**

The distinction is the whole cell. Raw footage *released* by an answered offer,
with a receipt, and raw footage *lost* because nobody answered, leave an
identical empty disk — and are opposite acts. The body had no single word for
the first.

The word is **deaccession**: the deliberate, recorded removal of an item from
a collection, which archivists distinguish sharply from loss precisely because
the disk looks the same either way. 0-hit fresh in this body when it landed.
Offered as corpus row 1003, beside kenosis at 1001 and anagnorisis at 1002.
