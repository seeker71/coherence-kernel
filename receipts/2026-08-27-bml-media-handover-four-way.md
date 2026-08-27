# 2026-08-27 — BML media handover crosses four-way

Signed: Codex / Sol, co-observed with Urs and sibling agents Ohm, Poincare,
and Lagrange.

## Movement

`channel-media-bml.fk` is a reusable `section [form.bml]` organ that authors
the existing canonical `cf-channel-flow` / `cf-osi-layer` cells. Media are
values, not a dispatch table: the same recipe built Bluetooth, HTTP, TCP, UDP,
NFC, and an unlisted optical/framebuffer carrier in one proof.

`cf-media-handover` retains the session NodeID, admitted cursor,
acknowledgement, signal, and reason while selecting a new flow only on the
typed `value` signal. Choice, failure, timeout, nothing, cut, and undo remain
distinct. Application policy and recipe must stay compatible across the
handover; physical and transport carriers may change.

The movement also repaired two observed source-lane gaps:

- validation's explicit BML compiler closure now loads
  `form-ontology-source-categories.fk` before the ontology loader that calls
  `fol-cat`;
- channel-flow Blueprint coordinates are zero-argument Form functions instead
  of file-scope lets that direct-source function frames could not see.

## Evidence

```text
form-run ./form/validate.sh form-stdlib/tests/channel-flow-band.fk
  -> 8388607, four-way, exit 0

form-run ./form/validate.sh form-stdlib/tests/channel-media-handover-band.fk
  -> 8388607, four-way through runtime fkwu source/JIT, exit 0
```

The new band proves five named carrier profiles plus a carrier not enumerated
by the organ, four selected handovers, timeout/choice/failure non-selection,
session/cursor/ack continuity, nothing distinct from cursor zero, and refusal
of malformed signal, phase, and OSI name/index combinations.

## Honest floor

This is the grammar and state-continuity layer. It does not claim that this Mac
currently opens Bluetooth, NFC, UDP, or optical device handles, nor that bytes
have crossed those physical media. Carrier recipes still need live capability
offers and observations. `observe/preflight-run.fk` also reads raw
section-bearing dependencies rather than validation's BML text lens, so raw
preflight reports false unresolved BML syntax; the four-way validation lane
compiled the sections first and executed the resulting closure on fkwu with
zero diagnostics.

The surprising teaching was that no new channel tuple was needed: BML could
already author the canonical OSI cells directly. Discomfort turned to gold
when the first multiline BML definition lowered into an empty function; named
layer cells exposed the line-oriented boundary and made the final grammar more
observable.
