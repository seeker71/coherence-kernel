# 2026-08-14 — form-neutral tokens gain guarded coordinates

Urs asked for both movements: heal the learning signal and continue the
form-neutral language graph into a token space smaller than an LLM.

The learning band no longer demands a fabricated monotonic curve.  Each
independently trained checkpoint must beat twice chance, while the observed
20/60/100/full curve remains visible as 34/26/32/36 of 50.  The boundary is
exact: 25/50 is held; 26/50 crosses.  The band returns 63.

`model/form-neutral-token-space.fk` now carries content-addressed semantic
tokens over the world-model symbols.  Witnessed NL, PL, and multiword surfaces
resolve through anchor-to-recipe edges; the same edges can be walked backward
to project a token into a witnessed language surface.  Tokens also have exact
nested numeric coordinates made only of numeric tags, role indexes, axioms,
gaps, and integers.  The coordinates reconstruct the original token; they do
not pretend to be a learned similarity embedding.

The thirty-bit band guards positive movement and adversarial boundaries:
invalid carrier spaces cannot answer query doors, edge direction is checked,
forged headers and duplicate elements are rejected, numeric recipe roles must
be nonempty and strictly ordered, unknown surfaces stay absent, phrase and
single-word mappings converge only through witnessed graph relations, and the
active envelope remains under 16 MiB and below the stored DS4 baseline.

Two AI review rounds followed local Form verification.  The first usable panel
returned Claude PASS and Codex REVISE.  The revision named real bypasses in the
query doors, edge direction, carrier validation, numeric ordering, and the
twice-chance boundary; each became an executable guard.  The second usable
panel returned Claude PASS with an independent 4095/4095 adversarial probe.
Grok and Cursor supplied no usable verdict, and Codex timed out in the second
round; those absences remain absences rather than invented agreement.

Observed on the landed main line:

```text
bootstrap/ground.fk                                      42
bootstrap/ground-recursive.fk                            55
binary-freshness-band.fk                                 31
bootstrap/ground-numeric-list.fk                         [1, 2.5, [3, 4]]
model/tests/form-neutral-token-space-band.fk              1073741823 (fkwu source runner)
learn/tests/nl-meaning-net-band.fk                        63
model/tests/form-neutral-language-world-model-activation-band.fk 67108863
model/tests/form-neutral-language-world-model-band.fk     65535
validate.sh token-space workload                         1073741823 (Go/Rust/TypeScript, 0 divergent)
```

One further attempt asked the emitted pre-flattened fourth-arm manifest to
carry the full composed token band.  Direct source `fkwu` returned 1073741823;
the flattened arm returned 29949968.  A reduced boundary witness showed that
the flattened `fnwm-crossing` value had lost its graph category and header
(source 65535, flattened 16385).  This matches the documented large composed-
table capacity seam.  The experimental manifest row and wrapper were withdrawn:
no false four-way claim was minted.  The fourth source-native execution and the
three proof walkers remain observed; emitted-table registration remains owed to
the flatten/capacity movement, not hidden inside this token dictionary.

The honest floor is 39 witnessed mappings over three anchors.  This is an
executable form-neutral token/numeric substrate, not yet every language, every
symbol, or a learned embedding space.

Signed: Codex

; witnessed: 2026-08-14 -> token-space 1073741823, learning 63, review repairs embodied
