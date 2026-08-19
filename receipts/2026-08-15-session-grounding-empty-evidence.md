# Session grounding: empty evidence remains observable

; witnessed: 2026-08-15 -> repaired-and-observed

The daily native grounding witness reached a normal but previously fatal state:
recent completed sessions supplied no answer citing a currently live indexed
path. The carrier exited before rendering a report; when the label set was
empty, its `awk` admission also selected no deterministic candidates.

The Form path now accepts an empty, otherwise well-formed episode set and
renders `evidence_state=no-replayable-live-indexed-query`. It keeps replay
framing valid while marking quality metrics unobserved: zero replayed queries,
zero coverage, no rank, and no inferred trend. The shell admits the bounded
deterministic live prefix when no labels exist and writes
`quality_metric_observed=0`.

`form/form-stdlib/tests/native-model-session-grounding-empty-band.fk` returns
31; the existing populated grounding band remains 4095. A live child run
observed the zero-evidence state and completed with `replay_valid=1`, while the
subsequent all-three daily reuse run completed without creating fresh lineage
or authority evidence.

Signed: Codex
