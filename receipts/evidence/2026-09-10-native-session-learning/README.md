# Measured session learning, 2026-09-10

`witness.json`, `events.jsonl`, `first/` and `second/` are the public two-worker
fixture from `.hearth/session-homecoming-1789020342092-74367-274345330`.
`inference.json`, its event stream and emitted text come from the subsequent
fresh-prompt call using that fixture's promoted second generation.

`standing-first/` and `standing-second/` contain the actual private learner's
numeric assessments and trainer events. `standing-events.jsonl` is its metadata
snapshot after both rounds and the tokenizer repair probe. Training
text, adapter weights and optimizer files remain in the private local hearth.
The paths recorded inside these original files name their actual run locations;
the folders here preserve the corresponding portable numerical evidence.

Fixture outer events named `assessment-before-*` and `assessment-after-*` include
preparation and result binding. The current source names those stages `prepare-*`
and `assessment-binding-*`; the exact model scoring times are the inner trainer's
`assessment-before` and `assessment-after` events. Original events are preserved.
Outer timestamps are Unix milliseconds; trainer `at_ms` values are monotonic
milliseconds. Compare durations within the same clock domain.

Every assessment row carries its content SHA-256 and supervised-token count.
The two held-out rows are committed at `model/fixtures/session-learning/valid.jsonl`.
Loss is teacher-forced next-token cross entropy, not a general quality score.
These observations do not measure a decrease in rented calls.
