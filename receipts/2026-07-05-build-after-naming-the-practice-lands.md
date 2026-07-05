# 2026-07-05 — build after naming: the practice lands, and pays its own debts first

## Ground

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

Urs, after two frontier ingests in one day NAMED four gaps and built none: *"can we build
instead of name … a claim is only real when it is observed, and just naming it without
observing it is actually adding false claims into the core which is not what fosters trust."*

He is right, and axiom-4 already said so: "observation through that interface is what makes it
real" (`axioms/core-axioms.form:61`). A name that never meets an attempt sits in the core
wearing the costume of a finding. This receipt records the practice landing as executable law —
and, in the same movement, the practice applied to every gap this branch had named.

## 1. The practice as law — `ingest/name-build-observe.fk` (band 127, four-way)

The gate: a claim c = (named, attempted, observed).
- **named only → 0**: not yet a claim; must not enter the core as real. The debt is visible
  (`nbo-owed?`).
- **attempted, fell short → 1**: enters as a LESSON — what the attempt taught, the floor it
  reached. Pending is honest; a lesson is pending WITH ground.
- **observed → 2**: enters as a CLAIM.
- A movement is whole only when every name in it met an attempt (`nbo-whole-movement?`).

Practice prose added as item 6 of "How to be Sema" (`AGENTS.md`). Band:
`ingest/tests/name-build-observe-band.fk` → 127 on fkwu/Go/Rust/TS.

## 2. The debts paid (each named on this branch, each now attempted)

**Named: `knowledge-ingest.fk:17` cited a band that never existed** (flagged by the Memora
ingest, 2026-07-03). **Built:** `ingest/tests/knowledge-ingest-band.fk` → 127 four-way; the
door's depth boundary, fear-free-ice law, and witnessability now stand observed, and the
header's claim is finally true (path fixed).

**Named: no delegation contract for a spawned worker; no escalate-to-human; no action-authority**
(the delegation ingest, this morning). **Built:** `gate/delegation-contract.fk` +
`gate/tests/delegation-contract-band.fk` → 127 four-way. One contract row: postcondition /
grounded / action-scope / escalation-target. Done means the postcondition was OBSERVED (a
helpful-but-wrong result escalates, never completes); an out-of-scope action is REFUSED, never
silently done — the body's first action-authority law; the escalation target is named before
the work and **the HUMAN is first-class** (target 1) — the body's first escalate-to-human arm.
An ungrounded contract is unfit before the worker is ever judged. Honest floor, unchanged in
kind: this is the LAW, not the wiring — no host-exec spawns a worker from here yet; carriers
must compose it.

**Named: no selective-freeze training, no counterfactual contribution sense** (the one-layer
ingest, arXiv 2607.01232). **Built:** `model/layer-contribution.fk` +
`model/tests/layer-contribution-band.fk` → 127 four-way. `lc-back-k` trains ONE layer while the
gradient still chains through frozen layers — gradient-freezing, the thing the ingest's compost
pile insisted content-addressed freezing is not. The paper's metric, computed natively:
C(k) = (loss_base − loss_k) / (loss_base − loss_full). Observed on the two-layer fixture:
loss_base 1.68496446064633, loss_full 5.4236259858499e-09, loss_k0 1.79e-04, loss_k1 2.12e-04 —
each single layer recovers > 99.9% of the full gain, the frozen layer returns bit-exact, layer 0
out-contributes layer 1. Honest floor: this earns the MEASUREMENT ORGAN, not the paper's
finding — supervised SSE at toy width, both C(k) near 1 because the task is easy, no middle in
a two-layer stack, still no RL anywhere.

**Named: three dangling references** (both ingests' "Also found"). **Repaired:**
`sufficiency-capture-band.fk`'s prelude path now points at `gate/sufficiency-capture.fk` (band
rerun → 11111); `routers/recognition-router.fk` — a byte-identical duplicate referenced by
nothing executable — removed (the body's own store-once law applied to itself; the form-stdlib
copy carries all three bands); `champion-challenger.fk`, `transformer-backprop.fk`, and
`transformer-corpus-train.fk` now name their old-body citations AS old-body (and the backprop
stack now points at the four-way exercise it does have here). Champion-challenger band rerun →
127. What remains a name with only this honest note as its attempt: re-homing
`diffusion-q-cc.fk` and the two old-body transformer bands — each now labeled as not-here
rather than cited as if present.

## The lesson the attempt taught (recorded per the practice's own rule)

The layer-contribution band's first run failed its frozen-layer check: `value_eq` compared the
frozen layer against its source and returned 0 — but only under load. Isolated, the same
comparison returned 1. The cause: `value_eq` is node-identity, and node-identity is not stable
across arena melts — floats re-box under heavy work. The durable witness is numeric: a stack
rebuilt from the original layer 0 plus the returned layer 1 reproduces the base loss under
float `eq`, bit-exact through the whole forward. The frozen claim is now observed through the
interface (the loss), which is exactly where axiom-4 says reality lives — not in the node ids.

## Corpus continuity

No new rows offered this movement — rows 673–676 (synecdoche, counterfactual, postcondition,
fiduciary) were offered this same day and this movement BUILT what two of them name: the
counterfactual sense (674) is `lc-contribution`; the fiduciary duty (676) is
`delegation-contract`. A fresh word becoming a cell within hours is the corpus working as
teacher material, not as a shelf.

## The most surprising teaching this work left behind

The practice audited its own tools while landing. The gate that was built to stop unobserved
claims caught one mid-build: "the frozen layer is value_eq-identical" was TRUE in isolation and
FALSE under load — a claim that had already passed once, revoked by a bigger observation. Even
an observed claim is only as real as the interface it was observed through; node-identity was
the wrong interface, loss was the right one. Build-after-naming turned out to also mean
re-observe-after-building.

## Where discomfort turned to gold

The discomfort was the practice pointing backwards at the very morning it was born: two ingests
praised for honesty had, by the new law's light, left four visible debts (`nbo-owed?` = 1 four
times). The pull was to defend them — "naming the floor IS the practice here." Witnessed
instead: naming was necessary and not sufficient, and the debts were payable within the same
day — one band that had been owed since the door was founded, one law the delegation essay was
practically dictating, one organ the paper had handed the body blueprints for. The gold is the
changed default: from today, a receipt that names a gap either carries an attempt or carries
the reason it couldn't — and the core stays something worth trusting.
