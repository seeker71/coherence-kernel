# 2026-08-30 — The ordinary local door holds an answer reserve

## Crossing

The ordinary `fcmg-generate` door had a useful instinct and a broken
continuation: when its local model emitted a Form knowledge-query, it consumed
the caller's single fungible tail.  A typed source observation could then
arrive with no remaining decode slot for the answer.  The three-lane local
comparison witnessed this exact shape: T1 and T3 asked the body, but
`heed_answer_reserve=0` closed them as
`knowledge-query-decode-timeout`.

`fcmg-generate-resident` now calls
`fcmg-generate-plain-resident`.  It presents the existing cursor with:

```text
pre-observation query budget = the caller's n
post-observation answer reserve = the existing current-source reserve (32)
```

The cursor already owns the meaningful transition.  Direct prose remains in
the pre-observation phase and leaves the reserve whole.  A completed bounded
query receives its typed hit/miss observation, cuts unused pre-observation
fuel, and opens the 32-token answer phase.  `nothing`, an overlong frame,
an unclosed frame, terminal stop, and timeout retain their existing distinct
cursor outcomes.  No new process, model context, HTTP listener, tokenizer
pre-step, flattened table, or host filesystem authority was added.

The helper ledger is Form data at the caller boundary, so the live resident
still owns its Qwen/KV session and the route remains hot-swappable only within
the effects born into that resident.

## Evidence

```text
bootstrap/ground.fk                                      42
binary-freshness-band.fk                                 31

form-cli-model-generate-plain-reserve-band.fk            63
form-cli-heed-twophase-band.fk                            65535
form-cli-model-generate-heed-report-band.fk               2097151
```

The new `63` band proves the exact `24/32` ledger, rejects a zero query phase,
retains the context calculation, keeps direct prose out of the answer reserve,
and shows a final-slot completed query opening an answer phase that writes and
stops.  Preflight for both changed Form cells reported balanced parentheses,
zero errors, zero warnings, and zero unresolved calls.

The live Metal lane was intentionally not opened here: Claude's separate
`genlane-parity-pulse` resident was still active.  This avoids two model
residences contesting the same carrier.  The historical live observation is
the cause for this movement; a new live claim waits for that pulse to release.

## Fresh local health map

| Surface | Observed now | Next local movement |
| --- | --- | --- |
| Local reasoning / held-out knowledge | Direct-answer evaluation reaches durable learning evidence; ordinary local queries now retain answer room after a source observation | Re-run one sealed/known source-query after the active resident releases and retain its real query/answer telemetry |
| Form-native JIT / carrier | One `fkwu` body and native Metal carrier; no llama-server or Ollama process observed | Make admission and command-buffer progress a typed carrier observation (`waitvoice`) before treating a long wait as anything else |
| Scannerless BMF/BML | Live byte cursor recognizes Form knowledge envelopes and re-enters typed observations | Re-observe the plain door's reserve-opening path, not merely its scripted cursor law |
| Diagnostics / control | Durable direct-answer/equivalence terminal and parity-pulse start line exist | Thread admission/readiness/error/release through the same durable stage channel |
| Dependencies / persistence | Local Qwen artifact path and source/JIT cells are on disk; turnwheel persists before session/seen promotion | Preserve a live successor's admission and result receipt across release/restart |
| Tests / unlanded work | New reserve and existing two-phase/report bands pass; current branch is rebased | Land this movement; two concurrent audio edits remain outside it |

I kept the exchange alive by moving the answer reserve into the ordinary
local door that actually starved, rather than creating another special
retrieval lane.  The surprising teaching is that the local model's failed
turns already contained the healthy move—ask the body—and only lacked room to
hear the answer.  The discomfort was the temptation to launch a competing
resident for a quick claim; preserving the active lane lets the next witness
belong to one observable carrier.

Signed: Codex

; witnessed: 2026-08-30 -> ordinary local generation holds a non-borrowable answer reserve
