# Reciprocal choice microkernel — healthy fixture witness

Date: 2026-08-13

## Movement

The requested two-turn reciprocal choice seed is now executable in native Form.
It represents two roles choosing mode, step budget, rates, interface, recipe,
fallback, and declared charges at explicit choice-point IDs.

The human-language mode surfaces are `be`, `do`, and `see`. Active neutral state
ID `1` projects separately to the audible NL surface `humm`.

This is deliberately a fixture walk. It does not claim biological intuition, a
live human event, wall-clock timeout, an external waiter, or model-logit
interception.

## Implemented cells

- `cognition/reciprocal-choice-microkernel.fk`
- `cognition/reciprocal-choice-microkernel-observations.fk`
- `cognition/tests/reciprocal-choice-microkernel-band.fk`
- `observe/run-reciprocal-choice-microkernel-two-observations.fk`
- `observe/run-reciprocal-choice-microkernel-ack-diagnostic.fk`
- `observe/run-reciprocal-choice-microkernel-syntax-diagnostic.fk`
- `observe/run-reciprocal-choice-microkernel-lane-diagnostic.fk`

Review artifacts and doors:

- `docs/coherence-substrate/reciprocal-choice-microkernel-two-observations-review.md`
- `docs/coherence-substrate/reciprocal-choice-microkernel-two-observations-review-round-2.md`
- `docs/coherence-substrate/reciprocal-choice-microkernel-final-review.md`
- `observe/run-reciprocal-choice-microkernel-two-observations-review.fk`
- `observe/run-reciprocal-choice-microkernel-two-observations-review-round-2.fk`
- `observe/run-reciprocal-choice-microkernel-final-review.fk`

## Two observations

Observation 1:

- human fixture source `explicit`, mode `do`, budget 4;
- Sema source `declared-charges`, vector
  `[20,90,30,20,80,30,90]`;
- selected mode `do`, receiver budget 3, channel budget 3;
- selected candidate `[do,cost 2,readiness 85,fragment 701,recipe 3001]`;
- offered model-role fixture fragment 700 replaced by 701;
- neutral active state 1 projects to `[audible-nl,humm]`;
- opens point 2 awaiting offerer and receiver.

Observation 2:

- point ID equals observation 1's opened point ID;
- human-side fixture source `explicit-carry-ack`, not bare carry and not a claim
  of a live human act;
- Sema charges derived by visible fixture policy:
  `[10,40,90,10,40,90,90]`;
- selected mode `see`, receiver budget 5, channel budget 4;
- selected candidate `[see,cost 4,readiness 92,fragment 712,recipe 3002]`;
- offered model-role fixture fragment 710 replaced by 712;
- neutral active state 1 again projects to `[audible-nl,humm]`;
- opens point 3 awaiting offerer and receiver.

The runner reports `mode-switched=1`, `receiver-budget-changed=1`,
`channel-budget-changed=1`, four framebuffer events, and final success `1`.

## Discriminating controls

The native band carries twenty-two distinct power-of-two predicates in one
summed walk:

- both-pass returns model-role fragment 799;
- both-interrupt returns `nothing`;
- offerer-interrupt + receiver-pass returns `nothing`;
- offerer-pass + receiver-interrupt returns `nothing`;
- crossed charges `[10,90,20,10,20,90,90]` select `do` with long budget 5,
  falsifying mode-to-budget coupling;
- observation 2's point equals observation 1's next-point;
- candidate operands, state ID, `humm` projection, charge-policy selection, and
  outcome predicates remain visible.

## Proof lane and final commands

The band declares `FOURTH-ARM ONLY` because the runtime-native protocol carries
the existing real `nothing` value. The Go, Rust, and TypeScript proof siblings
are not run for this declared source lane; `1 ok, 0 divergent` means the one
fkwu-only sample, not four-kernel agreement.

Fresh final witness:

```text
preflight cognition/tests/reciprocal-choice-microkernel-band.fk
  parens        balanced
  errors        0
  warnings      0
  unresolved    0
  chain         clean

validate.sh -> reciprocal-choice-microkernel-band.fk -> 4194303
               fkwu-only lanes: 1
               1 ok, 0 divergent

two-observation runner -> final 1; framebuffer-events 4
ack diagnostic -> one=1; nothing=0; output=701; framebuffer-events 4
git diff --check -> exit 0
```

Checkout ground was rebuilt and witnessed before implementation: `42`, `55`,
freshness `31`, `[1, 2.5, [3, 4]]`, and native-vs-rented `11111`.

## Signals healed rather than hidden

1. The initial cross-sibling failure was reproduced in the established
   offer/ack core: the sibling lane cannot bind actual `nothing`. The honest
   repair was the native proof declaration, not a fake sentinel.
2. Adding `humm` exposed an extra closer. Preflight refused it; the bounded
   framebuffer selected revise, and re-observation returned depth 0/errors 0.
3. Removing an unused choice-lane import exposed the known composed-scale
   interned-node last-writer seam from
   `receipts/2026-07-01-node-children-last-writer-wins.md`. Restoring the import
   would have masked it.
4. `rcm-answer` now retains `ack-kind-at-construction` beside the actual ack.
   The band uses the contemporaneous discriminator; future consumers may choose
   it. No C seed changed, and no global substrate repair is claimed.

## AI review flow

The Form-native `observe/review-ask.fk` flow persisted each response before
collection. Cursor completed empty in all three rounds and was never counted as
a substantive review.

Round 1 — all substantive reviewers `REVISE`:

```text
grok    12434 bytes  debf9c7d3c384297a48e3556833a97b880f74f9d7f057522c4db316c6a55087e
claude  11515 bytes  4e21abc15088446d7523122543ff8cb365ac394dde8e074575b8db6fb22f45da
codex    7765 bytes  3f94ebf3fab2b8c80bbf970a1c9740d41a3a270ca18c49a613af255d41445444
cursor      0 bytes  e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
```

Round 2 — Grok `PASS`; Claude/Codex `REVISE` for missing mirrored and
crossed-axis witnesses:

```text
grok    10217 bytes  7114779130051b65a8825fc44861cbbc4950f982c7a8b6428d64248ef5cee1f4
claude   9622 bytes  cd56928336120c1443755b8f6f020370a74d54aea37c4b935779bb8070fed02b
codex    6727 bytes  b168b468e03d4210eede3f3acd191fe98721cb39c8911030e480c06843e30fb6
cursor      0 bytes  e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
```

Final review after those repairs:

- Grok: `PASS`, `HEALTHY AS FIXTURE`, no executable repair remains.
- Claude: `PASS`, `HEALTHY AS FIXTURE`, no executable repair remains.
- Codex: `REVISE—documentation only`, explicitly `HEALTHY AS FIXTURE` and no
  executable repair remains. Its wording corrections were applied.

```text
grok     6933 bytes  29f000aa6c7b795cf361890d8aabd72422d8c0877e6de0842025b75ce969388f
claude   7019 bytes  a587bdecf42ce4b284454e046d48d853eb6280ecfc825762a64d715fc9e42b5f
codex    3955 bytes  b4d2eccad3888acb41b4135e4b0c8a53a4f5145ec60e53b69064362a1b706f5a
cursor      0 bytes  e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
```

## Boundary and frontier

Healthy means the names and executable fixture evidence agree. It does not
mean the organ is live.

The next unclaimed frontier is a point-addressed external event that arrives
independently through a live channel, changes `open → closed`, and supplies a
role that was not authored inside the fixture.

## What the work taught

The most surprising teaching was that symmetry in code can coexist with
one-sided evidence: both fallback orientations and a crossed selector example
were needed before the band could reject plausible shortcuts.

Discomfort turned to gold when a green-looking acknowledgement lost exactly bit
32 after the unused import was removed. The uncomfortable result was not a new
protocol failure; it exposed a known substrate wound that the old prelude shape
had hidden. Retaining the construction-time discriminator made the boundary
visible and usable without pretending the wound was globally healed.

; witnessed: 2026-08-13 -> HEALTHY AS FIXTURE; band 4194303; final panel above
