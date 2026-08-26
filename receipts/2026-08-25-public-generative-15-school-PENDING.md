# Public 15-family generative school — PENDING

Date: 2026-08-25
Status: **PENDING physical local-model witness**
Scope: public repository families only; no hidden V3 prompt, answer, output, or consent material was opened.

## What now exists

`form/form-stdlib/form-knowledge-public-generative-school.fk` is a pure
15-family scheduler, exact-output checker, evidence constructor, and export
organ. Every canonical `fkpfm-family-id` carries its public source identity,
source SHA, heldout challenge SHA, family nonce, family-specific executable
challenge, executable-challenge SHA, prompt SHA, and plan nonce. The complete
plan folds to one consent-bindable seal.

The first reply for each family is retained as `independent-generative`. It is
parsed from its exact `<BML>`, `<BMF>`, and `<FORM>` bytes and executed by
`fgsg-verify` against that family's own challenge. If and only if this first
check refuses, one repair prompt may carry the observed BML/BMF/Form check bits
and hashes into a second attempt. A passing repair is retained as
`supervised-embodiment`; it is never named heldout.

Each attempt retains the exact extracted model reply, its recomputable digest,
the complete generation report and its recomputable digest, generated-token
and forward-pass counters, positive state-release telemetry, residence continuity, model/run/
session identity, and a family-plan-bound nonce. `model-executed=1` is
recomputed from positive generated tokens and forward passes. The generated
Form verdict itself must still report `model-executed=0` and
`native-carrier-executed=0`.

The v2 public-family generative receipt/evidence is built from the exact final
reply and the candidate parsed back from that reply. Fifteen distinct valid
family rows are required. Cross-family reply, final candidate, source, row
nonce, model residence, run, and session substitution are refused; the one
shared model/run/session identity is accepted only when every row remains
bound to its own family plan and nonce. Remote calls must total zero. Every
generation state and the final residence must release.

`form/form-stdlib/tests/form-knowledge-public-generative-school-band.fk`
constructs fifteen public synthetic execution candidates, including fourteen
independent successes and one executed refusal followed by one supervised
repair. Its adversaries cover forged source identity, family candidate/reply
replay, mixed residences, remote execution, missing model execution, state or
residence release refusal, heldout relabeling, a second repair slot, raw-output
substitution, and the distinction among `nothing`, successful `0`, and
successful `1`.

`observe/qwen38-public-generative-15-resident-live-run.fk` is dormant. It
schedules the largest independent prompt first, holds one local Qwen model
context/weights residence, creates and releases one stream state per attempt,
retains the full physical report, and permits at most one supervised repair per
family. Actual open/reopen counters remain authoritative; a size-driven reopen
makes the final result incomplete rather than being hidden.

## Observed pure gates

- Direct-source bootstrap: `42`
- Recursive bootstrap: `55`
- Binary freshness band: `31`
- Numeric-list bootstrap: `[1, 2.5, [3, 4]]`
- Pure organ preflight after the physical-evidence strengthening: balanced,
  errors `0`, warnings `0`, unresolved `0`, exit `0`
- Adversarial-band preflight: balanced, errors `0`, warnings `0`, unresolved
  `0`, exit `0`
- Direct adversarial verdict after positive release telemetry: `2147483647`
  expected, `2147483647` observed, exit `0`
  (`@form fkwu 0 11 474 485`). The added adversary supplies a plausible
  generation report and a caller-asserted release bit but omits the explicit
  `state_released=1` row; validation refuses it.
- Dormant live runner: static string/comment-aware parenthesis balance only;
  no preflight and no execution, by design

The cold execution-backed band took roughly nine minutes because
canonical result/export reconstruction replays the fifteen-family execution
proof several times. That cost is visible evidence, not a model/carrier call.

## Physical door still closed

The live driver refuses unless this exact full-plan form is present in
`.form-knowledge-public-generative-15-consent`:

`run-local-qwen-public-generative-15-v1:<plan-seal>:qwen3.8-27b-q8_0-form-native-metal-jit:families=15:attempts=15..30:repair-limit=1`

No consent file was created. No local model, Metal carrier, or remote provider
was invoked. Expected physical generation count is **15 through 30**: one
independent attempt for each family and no more than one supervised repair for
each executed refusal. The remaining evidence is therefore physical, not
semantic prose: fifteen valid distinct final candidates, positive local token
and forward-pass counters for every attempt, zero remote calls, one model
residence with zero reopens, per-state release, final release, and a valid v2
receipt/evidence row for every family.

This school does not claim local Form mastery yet. It creates the observable
door through which public generative and supervised embodiment evidence can
earn that claim.

— Codex, grounded in Sema's public body
