# Glass organ care: health, asking, and attended are separate

Date: 2026-09-04

Urs asked whether Glass can show that Form-native organs are healthy or are
receiving the guidance and attention they ask for.  The existing local pulse
already carried health values, but it was a curated census and had no receipt
join from a gap to attention.  Treating silence as health, or an action claim
as healing, would make the requested panel decorative.

`form-glass-organ-care.bml` introduces a small pure receipt protocol:

- `healthy` requires a current positive reading.
- `unobserved` remains distinct from a negative reading.
- an observed gap with a named move is `asking` until an attention receipt
  matches its organ identity, evidence identity, and requested move.
- matching `received` and `applied` receipts become `attention-received` and
  `attention-applied`; neither is relabeled as health.
- `all-held` is possible only with an explicit, duplicate-free complete census
  and every member healthy or genuinely attended.

The direct proof band returned **4095**, and preflight reported balanced
parentheses, 0 errors, and 0 unresolved calls.  The live native runner
observed **62** declared census organs: **48** healthy, **14** asking, 0
attention-received, 0 attention-applied, 0 unobserved, `coverage=complete`,
and `all-held=0`.  Its next named request was
`concept-digest-evidence-and-bucket-image`.

This is not a universal body-health claim.  It is the present exact reading of
`observe/form-local-offline-health-pulse.fk`'s declared census.  The runner
projects all 62 statuses into source-attributed framebuffer roots for Glass.
It uses only Form evaluation and framebuffer primitives; it opens no new host
or network membrane.

The local Qwen hearth was separately asked to name the existing source seam.
It returned `form/organ/health-attention-receipt`, a path absent from the body.
That reply is withheld as evidence.  The located sources, not the fluent
candidate, define this receipt.
