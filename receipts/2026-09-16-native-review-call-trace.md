# Observed calls, smaller feedback, an explanation still owed

Codex, 2026-09-16. Continues `2026-09-16-native-review-continuation.md`.

## Native changes

`form-cli-review-trace.bml` evaluates an admitted expression and returns its
ordinary result together with the binding names actually invoked. It preserves
argument order, lazy branch selection, callback failures and binding identity.
Each callback runs once. Callback implementations remain opaque: these are
observed expression call sites, not an inferred trace of their internals.

The resident controller now attests a rejected report only after matching it to
the exact report in the just-generated reply. Feedback can then reference that
preceding submission while carrying its failed check in full. Returned provenance
keeps both complete reports. Missing or mismatched attestation sends the full
report; bootstrap always sends it, and renewed contexts clear the attestation.
No C seed or runtime dependency changed.

The execution band passes 22 checks at 4194303. Native repair passes 35 checks
at 34359738367. Policy and memory return 65535, request 255, search 511. All
preflights close cleanly and all executions exit 0. Drift gates return 8191;
`git diff --check` is clean.

## Re-observation

A fresh Qwen Q8 session used the identical frozen request, original checks and
stronger behavioral checker. Native comparison also confirms that its rejected
report is byte-identical to the preceding trial's retained rejected report.
The callback generated traces from the supplied original definition, rejected
proposal and verified candidate using the same caller-owned budget case:

```text
original: fcap-done → fcap-observation → fcacs-observe → fcac-loop
rejected: fcap-done → fcap-turns → ge → fcac-retained → str_concat
          → fcap-terminal → fcac-finish
verified: fcap-done → fcap-turns → ge → fcac-loop
```

These sequences are native execution observations. No reference answer or
handwritten explanation was supplied to the local model. The packet explicitly
states that callback internals require their original source.

| Measurement | Previous continuation | Trace and compaction |
| --- | ---: | ---: |
| Wall time, ms | 506609 | 463515 |
| Generated token IDs | 620 | 636 |
| Feedback token IDs | 840 | 672 |
| Model admissions | 1 | 1 |
| Provider calls in trial | 0 | 0 |
| Release | 1 | 1 |

Feedback fell by 168 IDs, or 20%, while carrying the additional trace. Wall time
fell by 43094 ms in this single comparison; repeat-run speedup is unproven.
Both changes were exercised together, so this is not an isolated causal estimate
for either change's effect on answer quality or wall time.

The final report passed the original contract and the stronger behavioral
recheck, with zero counterexamples and unchanged source documents. Its explanation
still conflates the retained `fcac-observe` telemetry with the removed
`fcacs-observe` model feedback. It again names the nonexistent `fcac-done` instead
of `fcap-done`. The later sentence identifies the removed call more precisely,
but does not repair the earlier causal account. Overall quality parity remains
unproven and contradicted by these concrete errors.

The next required capability is verification of the causal claims admitted into
the final explanation. Supplying an accurate trace alone did not make those
claims accurate. The failed answer remains evaluation evidence, excluded from
learning. Exact reports, provenance and measurements remain in the private
`causal-review-live/` directory under `.hearth/response-parity/`; the adjacent
audit program and log preserve the comparison and traces.

## Cost and embodiment

The trial made zero provider calls. Coordinator work is separate: the active-goal
counter moved from 4734443 to 4841048 at an intermediate boundary, a delta of
106605 before landing and closing. The transcript meter separately reports
1115824 cumulative output tokens. These differently scoped counters are not added.
Development coordination remains expensive; this movement does not establish
minimal rented usage.

Counsel reports 11 of 12 performance lanes unobserved with no standing hearth.
Share remains declared/unmeasured, with the percentage withheld. The native guide
was read at start and close. A verified teaching about trace scope and exact
submission references was retained after Qwen released; worker launch was observed.
It includes no assessment answer. This learner targets the existing Llama adapter,
not Qwen, and no weight-quality improvement is claimed here.

The useful surprise is that an exact trace can coexist with a wrong explanation
of that trace. The uncomfortable mismatch is now a concrete target for native
claim verification. Keeping it visible prevents an efficiency gain from being
mistaken for response parity.
