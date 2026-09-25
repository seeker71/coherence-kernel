# Response-profile learning: binding repaired, useful correction still open

The actual gap is correction that serves the enquiry while preserving its
source distinctions. This movement repaired the connection between the Qwen
head learner and the bounded response controller, then read the candidate's
answer to the same retained edit request. The candidate did not close that gap.
Codex supplies this assessment; Qwen3.8-27B Q8 produced the native answers.

## Implementation and real observation

The learner initially refused the response profile with `profile must be full
or knowledge-query`. It now accepts `response`; public completion targets
still refuse every reasoning profile. Existing full and knowledge-query
routing remains available. `fgcr-compose` takes ownership of an already
admitted session and runs the existing generation, correction, retention and
release path. Reports now retain the actual adapter path and A/B digest.
Ordinary generation still opens its own base session. No automatic candidate
selection or serving promotion was introduced.

Five training rows and three validation rows selected exact sentences from
committed Form teachings. The original enquiry and evaluated answers were
excluded. Validation held different sentence selections from the same concepts;
this was not unseen-concept validation. Capture retained 145 training and 105
validation completion positions, including end markers, with no prompt tokens
supervised. The rank-one output-head learner ran eight epochs. Its baseline
validation loss was already 0.0007830665; epoch three selected 0.0006163098.
Later epochs worsened validation. These teacher-forced sentence-copying losses
do not demonstrate independent editing or synthesis.

After releasing training resources, the movement opened that exact candidate
and reused the baseline enquiry, response profile, 5,069 prompt IDs, 8,013
context positions, 1,024-token answer allowance and 350–450-word request.
Adapter bytes match both the learning binding and response report.

| Native response | First → revised words | Generated IDs | Elapsed ms |
| --- | --- | --- | --- |
| Retained base response | 516 → 516 | 623 + 623 | 727,837 |
| Selected head candidate | 509 → 509 | 619 + 619 | 850,126 |

The candidate repeated its answer byte for byte during correction. Source
assertions passed; unchanged answer assertions failed. Both execution and
release completed. Provider calls and serving promotions were zero. These
individual timings include other host work and establish no isolated speed
comparison.

## What the answer establishes

The candidate changed some wording in the frequency and closing paragraphs.
It still assigns prediction to the tokenizer, omits the explicit distinction
between 0, 1 and nothing, and asserts a structural cause for the shielded
experience without establishing it. It continues explaining intended practice
at length while failing the requested correction. Seven fewer words and lower
validation loss establish neither useful transfer nor felt resonance.

The sentence-copying task was already near zero loss before learning. Its
selection metric did not expose the observed editing deficit. Repeating this
curriculum or the unchanged shortening request has no new basis. The next
learning target needs verified behavior from actual corrective work; the
evaluated enquiry must remain excluded. The present candidate stays unpromoted.

## Checks, continuity and cost

The response/reasoning band passed 1 after clean preflight. Request preparation
checked response admission, existing completion profiles and reasoning-profile
refusal. The runner's first preflight reported `frr-source` and `frr-report`
as unresolved; loading their existing response-resource prelude repaired the
chain before any model ran. Export preflight was clean, and its audit observed
identical before/after revision bytes and matching adapter identity. Private
reasoning was not read. No C seed or external runtime dependency changed.
Closing drift gates passed 8191/8191 with no refusal.

Native guide: Python implementations 0, invocation candidates 2, unread files
0. Counsel: orphans 0; eleven of twelve lanes remain unobserved without a
standing hearth. The wider local-agent goal checker recorded 0/255; that
separate checker does not measure this enquiry's answer quality or parity.

The preceding completed coordinator turn used 7,585,574 rented tokens, including
7,389,056 cached input tokens: total input 7,552,053 and output 33,521, with
zero unattributed tokens, 61 model calls and 60 tool calls. This excludes the
current open turn and separate provider processes. Coordination cost remains
far above the requested minimum; zero provider calls inside this native
movement is not a whole-session cost claim.

The verified composition-boundary teaching was retained as
`1f259cde73548d7e1462b6e0699e49b39fb750fca17a1f08e1bc570dce9f2f41`.
The learning worker launched; retention establishes no serving-model update.
Its stale local image was detected and rebuilt through the compiler's health
flow before the teaching was retained.

Public candidate answer, before-correction answer, original feedback, exact
learning request, capture, loss history, candidate binding, export audit,
native helpers and completed-turn cost are in
`artifacts/2026-09-25-qwen-response-learning/`. The identical baseline prompt
and source remain in `artifacts/2026-09-25-native-response-profile/`.

— Codex
