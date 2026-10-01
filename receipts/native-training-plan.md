# Native training-cost recipe library

Local Qwen3.8-27B-Q8_0 constructed twelve pure Form recipes and one float32
Metal recipe during decoding. Nineteen controls included one refused input
and its local repair. Form injected 1,729 observation IDs into that same KV
stream. The [accepted executable program](native-training-plan-library.txt)
contains the actual constructors, dependency coordinates and calls; the
[independent verification](native-training-plan-results.json) reads execution
observations rather than generated prose.

The problem is quadratic ridge calibration of native LoRA training cost:
`features(t) = [1, t/100, (t/100)^2]`, with regularization `0.000001`.
Cholesky and two triangular solves fit latency and additional memory demand
from five retained training rows. Row lengths are `[64,135,155,158,289]`,
seconds `[7.614,14.423,16.207,16.727,31.341]`, and estimated additional bytes
`[1185176576,2575486080,2981799040,3043302400,5870504576]`.
Timings cover forward plus backward work. Memory is the worker's whole-tape
estimate. Offered budgets are scenarios; the planner reserves 10% headroom
and selects the largest admissible observed row.

| Offered additional GiB | Row tokens | Predicted seconds | Predicted additional GiB |
| ---: | ---: | ---: | ---: |
| 2 | 64 | 7.626039 | 1.103785 |
| 4 | 158 | 16.660780 | 2.834294 |
| 8 | 289 | 31.337461 | 5.467335 |

The same planner serves all three inputs. Its graph is
`planner → admit → predict → {dot, features}` and
`fit → {gram, rhs, solve}`, `gram/rhs → dot`,
`solve → {cholesky, forward, backward}`.

| Form recipe | Observed dispatches |
| --- | ---: |
| dot | 45 |
| features | 31 |
| gram, rhs, fit | 2 each |
| cholesky, forward, backward, solve, admit, planner | 3 each |
| predict | 21 |

All 121 dispatches returned. Independent partial-pivot Gauss-Jordan agrees
with the fitted coefficients within `1.3e-13`; the largest normal-equation
residual is `1.14e-13`. A second positive-definite system returns `[1,2,3]`
with residual `1.78e-15`. The RAM Metal polynomial map evaluates all five
row lengths in one dispatch; its largest difference from float64 is
`1.57e-7` GiB. Decode-run Metal admission and execution took 3 ms and 1 ms.
Release succeeded. Form recipe arithmetic uses the pure evaluator; the
polynomial uses RAM Metal JIT.

The retained program also replays on this field with the refreshed C seed,
without a model. Warm process execution took 779 ms, including startup and
library admission; its values passed the same independent checks.
Coordinates are this field's identities. The recipes belong to the admitting
response and are reconstructed for replay.

The arriving agent supplied the algorithms, prompt and repair guidance.
Qwen emitted the constructors and requests; Form produced the numbers.
Both local generation attempts made zero provider calls. The mathematical
attempt generated 3,073 initial IDs and 1,024 final IDs. Its prose stage
remained unfinished after adding commentary to a complete result JSON.
That outcome stays retained separately from the verified mathematical result.
Native observation data supplies the structured result.
