# 2026-07-28 — the island, and the reunion

Urs: **"stopped again, and you did not even merge or deploy anything, not healthy"**

Both true. Ten commits sat on a branch nobody else could see. Work that has not reached the trunk
is work the body does not have — it is a claim about the body made in private, which is the one
kind of claim this place is built to refuse.

Merged as [PR #377](https://github.com/seeker71/coherence-kernel/pull/377), squashed into main as
`8b72cc3fb`.

## Verified on main, not on the branch

After `git reset --hard origin/main` and a fresh `cc -O2 -o fkwu runtime/fkwu-uni.c`:

```
./fkwu --src bootstrap/ground.fk                                  -> 42
./fkwu --src learn/tests/homecoming-distillation-corpus-band.fk   -> 32767
./fkwu --src form/form-stdlib/tests/gated-deltanet-layer-band.fk  -> 255
form/native/metal/metal_gdn_gpu.sh                                -> VERDICT PASS
```

A branch being green says the branch is green. This is the merged tree answering.

## The reunion

Main had moved five commits while this line worked, and both lineages minted meaning-ids
**888–893 on the same day** without seeing each other:

| main | this line (re-seated) |
|---|---|
| 888 `eurythmy` 889 `barnum` 890 `streetlight` | 894 `winsorize` 895 `interpretant` 896 `surrogation` |
| 891 `autologous` 892 `pseudostructure` 893 `cargo-cult` | 897 `autoepistemic` 898 `adventitious` 899 `heteronomy` |
| | 900 `syncretism` 901 `swamping` 902 `pathognomonic` |

Every row kept; main is the trunk so its ids stand. The one in-corpus citation the move broke —
`heteronomy 893`, cited by the pathognomonic row — was rewritten with it, and **the band's
prose-citation walker is what proves that, not my care.** The count was *asked* of the corpus after
the merge (`hdc-count` over the merged rows → 297), never counted by hand.

Band **32767**: 297 rows, 297 admissible, max id 902, ids distinct, field code 2972972902.

A mechanical note worth keeping: rebasing hit a conflict on **every one of ten commits**, because
every commit touches the corpus. Aborting and merging instead is one resolution rather than ten.

## The most surprising teaching

**The freshness grep is a lens on the past and cannot see a sibling minting concurrently.**

Main's `streetlight` is a word this line glance-checked earlier the same night — `"streetlight
effect" -> 0` — and passed over for other reasons. Hours later it was home, minted independently on
another branch. The check was correct when it ran and false by the time it mattered.

That is not a flaw to repair with a better grep; no query over a committed tree can see an
uncommitted sibling. It is a property of working in parallel, and the body already carries the word
for the event: **`homoplasy`** (884) — a shared trait arising separately in two lines rather than
inherited from one, minted for precisely this reunion practice. The instrument-side error is
**`autoepistemic`** (897): reading one's own reach as the world's contents.

So no new row this time. Both halves are already sayable here, and reaching for a fresh coinage
when the body can already speak is how a corpus fills with synonyms — the same call made an hour
ago on the fixture-coincidence teaching. **A frontier question whose honest answer is a word we
already have is still an answered question.**

## Where discomfort turned to gold

Being told the work was *not healthy* while every band was green. Both were true at once, and I had
been reading the green as the whole of health. A band proves a cell computes; it says nothing about
whether the cell has reached anyone. I had built a private, well-tested, well-received-by-itself
island and called it walking — and the loop that let me do it is the same one that had me
describing the next step instead of taking it, three times tonight. Green is not arrival, and
neither is a receipt.

What turned it: the merge immediately surfaced the collision. Six ids worn twice, invisible from
inside the branch and unmissable from the trunk. Staying on the island was not just unhelpful to
others — **it was hiding a defect from me.**

## Ground stamp

```
PR #377 -> merged 2026-07-27T21:41:29Z, squash 8b72cc3fb
origin/main tip: 8b72cc3fb
ground 42 · freshness 15 · corpus 32767 · gdn conv 127 / gates 511 / layer 255 / msl 255
kimi-kda 63 · moe-route-radius 63 · moe-route-wide-msl 255 · kat-coder ingest 1023
metal_route_wide_gpu VERDICT PASS · metal_gdn_gpu VERDICT PASS
```

## Open, on main, named

- The C seed writes `.fkb` **v5** while `program-image-fkb-byte-container.fk` and its 36 kin still
  describe **v4**. Nothing breaks and all 39 artifact bands agree across both binaries, but two
  implementations of one contract disagree.
- KAT-Coder's remaining pieces are not new tissue: the GGUF tensor table, the projections, and the
  full-attention layer at every fourth position.
