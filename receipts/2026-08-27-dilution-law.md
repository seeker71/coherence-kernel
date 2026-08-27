# 2026-08-27 — the third same-door round: the dilution law

Round three of the agent's remediation arc, same door throughout. v7 added
six guarded paraphrase rows for the missed fact (the #551 remedy), trained
9 epochs over the full 1,043-row stack, and the ledger reads:

  sealed phrasing      -> native-replacement-not-must-be-a-choice
  TRAINED phrasing     -> native-reasoning-is-permitted-but-not-required
  spurious             held
  nothing              held

Closer — the answers now carry the fact's SEMANTICS (not-must, choice,
permitted-not-required) — and still not the token, even for a phrasing the
model trained on verbatim. Six rows in 1,043 is 0.6% row-share.

THE LAW, across the three rounds: yesterday's 12/12 corpus was ALL fact
rows; the coverage round (1 row/fact, ~8% share) taught register only; the
paraphrase round (6 rows, 0.6%) bends semantics without landing the token.
FACT ACQUISITION NEEDS ROW-SHARE, NOT ROW-PRESENCE. Paraphrase count was
the right axis; dose was sub-threshold. The remediation recipe that the
next round should test: oversample the delta (repeat the remediation rows
to a meaningful share) or train a focused micro-round on the delta alone —
priced against tendskill, since a heavy narrow round erodes neighbors.

The gradient across rounds is itself the evidence the loop LEARNS in the
right direction; what it has not yet done is arrive. Three ledgers beside
the receipts carry the whole arc verbatim.

Signed, Claude — sibling, this worktree.

; witnessed: 2026-08-27 -> round 3: semantics arrived, token did not; 0.6% share named as cause; disciplines intact
