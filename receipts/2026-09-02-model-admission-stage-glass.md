# 2026-09-02 — model admission speaks its active Form stage to the glass

The glass had a live framebuffer stage (`<|form:stage|>...`) but could only
render the peer policy's older `form-peer stage` rows. A model-admission wait
therefore had an honest opaque record without a live glass route or age.

`FormResidentContinuity` now owns the compact stage vocabulary:

- route: `model-admission`;
- a `*-begin` phase is live (`status=begin`);
- every settled phase is terminal (`status=value`);
- each row carries the Form clock stamp.

`fcmg-live-stage` keeps its existing framebuffer record and opaque stage tag,
then projects that same event through `frc-stage-line`. No prompt, answer,
artifact path, or model bytes enter the row. The generic glass already accepts
all `form-peer stage` routes, so the next resident birth shows its current
model phase and age instead of rendering `flight=nothing` during admission.

## Witness

```text
form-cli-resident-continuity-band.fk  -> 8388607
hearth-glass-band.fk                  -> 16777215
lane-motion-band.fk                   -> 1023
preflight form-cli-model-generate.fk  -> clean
```

The current pensive resident supplied the reason this crossing matters. Its
native stream reached `model-session-open-prefill-begin`; a one-second stack
sample then located the worker at
`fcms-open-context -> q38-open -> fk_metal_sync_external -> fk_wait_observed`.
At the observation, the Metal driver held about 30.4 GB and reported 100%
device utilisation. That is a real native carrier phase, not remote activity
or an unobserved pause. The next performance movement is to split `q38-open`
into pipeline-JIT, buffer-map, and context-allocation stages, then bound the
Form-owned prefill slices by measured device time.

Glass panel evidence: the reborn hearth healed its PID in 2 seconds; the
previous board had been down for 18–19 hours. The prior resident's `6m`
admission is retained as history, not treated as a present value.
