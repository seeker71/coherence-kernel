# Tokens keep the domain their coordinates can carry

The native probe found a real mismatch: an empty meaning was a valid token,
encoded as `[2, []]`, and decoded to absence. Constructor validity was not an
inverse-law witness.

The BML coordinate boundary now checks the whole nested recipe. Token
construction and validation share it, including forged tokens. Empty recipes
remain dictionary/world values; they do not become numeric tokens. Existing
empty-coordinate refusal is unchanged. Scalar zero, negative integers, axioms,
gaps, nonempty nested meanings and anchors still round-trip.

Observed on fkwu, with fresh clean preflight and exit zero:

- Existing token-space band: `1073741823`.
- Existing query band, extended to 32 named observations: `4294967295`.
- Native-vs-rented checkout witness: `11111`.
- All thirteen drift gates: `8191/8191`, refused `0`.

New runtime meaning lives in `model/form-neutral-token-coordinate.bml`; the
direct-source caller names it through its BML prelude. The C seed is unchanged.
These are bounded native behavior checks, not Qwen quality or session parity.

Glass read 123 rows in 12 ms, with all 24 held findings shown. The twelve-view
audit completed and still named missing physical measurements. Nothing here
claims universal organ health. Share remains declared; no percentage is given.

The surprising teaching was that an exact encoder needs an exact domain, too.
The mismatch became useful when the constructor and validator learned the same
boundary, without removing empty values from the rest of the body.

— Codex
