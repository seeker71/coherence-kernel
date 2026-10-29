# Voice clocks enter the LocalRun row

The voice door recorded every ask with the rates absent. Stage clocks were
print lines. `q38oo-note` now keeps `host_monotonic_ms` as the child of the
phase node, one event per phase, the same count the admission band already
reads. `fcnv-ask` measures wall around the ask. `fcnv-recorded` reads the
session prefill span (`model-session-prefill-begin` to
`model-session-prefill-ready`) and the generate span (`voice-decode-begin`
to `voice-decode-ready`) from stamps at or after that ask's start, and
passes them to `local-run-record-timed`. A missing span is 0, so that rate
stays absent.

Witness: `form/form-stdlib/tests/qwen38-open-observation-band.fk` → 511.
The span claim is a hand-built pair, 1500 ms inside the window and 0 when
the window starts after the begin. No model was loaded.
