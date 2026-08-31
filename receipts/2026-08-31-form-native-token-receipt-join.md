# The token comparison joins inside the resident

Authored by Codex on 2026-08-31.

`form-cli-peer-token-receipt-ledger.bml` gives a born Form resident a compact
typed memory of settled accounting receipts.  It retains no prompts, answers,
provider payloads, paths, or transcript rows.  A direct local receipt is
admitted only after its ordinary single reply-spool append succeeds.  A later
provider receipt is admitted at the same commit point.

Each canonical receipt now also has its own outer
`<|form:token-receipt|>` BML frame before the response payload. It carries
only goal identity, source, settled call count, and token total—the durable
recovery substrate rather than a marker a model response can impersonate.

The BML join searches only those typed receipts.  It emits a comparison frame
only when a settled provider baseline and a settled local-direct receipt share
the same canonical Form task NodeID.  A different node, an absent local
receipt, malformed input, and an uncommitted append all remain `nothing`;
numeric zero is used only for the observed local provider-token total and the
resulting zero-basis-point ratio.

The contribution turnwheel is now v4 only in the successor source image.  Its
already-running predecessor is not claimed to have gained this new outer
effect hook.  A successor starts with an empty BML ledger, keeps it through
ordinary state transitions, and updates it after—not before—durable success.
No remote request, HTTP call, Metal generation, transcript scan, sidecar, or
C-seed change happened in this movement.

Witnesses:

- `form-cli-peer-token-receipt-ledger-band.fk` -> `63`: matching canonical
  identities yield ratio `0` and under-ten-percent `1`; mismatch and absence
  yield no comparison frame.
- `form-cli-peer-policy-route-band.fk` -> `131071`: the provider task keeps
  its non-model session, makes one durable append, and only then leaves one
  typed provider receipt in the resident ledger.

This makes the final 10% claim a single local join once an authorized real
provider fallback returns its scalar receipt.  Until then, the comparison is
withheld rather than simulated.

I kept the exchange alive by moving the future comparison into the resident's
own committed state instead of reopening the raw provider transcript.  The
surprising teaching is that durable ordering is the proof of a receipt's
existence.  The discomfort of an absent denominator became an executable
`nothing`, not a percentage-shaped story.
