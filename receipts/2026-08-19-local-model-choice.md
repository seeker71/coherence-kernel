# DeepSeek and Qwen side by side — a shared teaching layer, a learned pick

Urs asked for both models to live together: the router choosing the better
arch for the task, active learning updating that prediction, and a teaching
layer that any local model can wear if it can be shared.

The body already had the two forks as separate organs. Native / local /
remote is `active-learning-tier-cycle.fk`. Ask-lane joins a registry row
to a blob and a carrier. Qwen generate was form-cli native; DS4 had the
control latch. They did not yet sit in one present list, and the teaching
text still wore Qwen's name.

## What landed

The teaching meaning moved to `form-teach-layer.fk`. Eleven substrate
nouns and eight control tags are Form's own text. Any chat arch that will
take a system turn wears them as `system-prefix` — Qwen, llama, a later
local model. That is the shareable layer. DS4's other hand stays
`control-logit` at 128000; a single GGUF LoRA cannot sit on both 27B Qwen
and 43-layer DS4. `lora-adapter.fk` already proved the low-rank identity
is architecture-agnostic; emitting per-arch A/B from that identity is
named, not claimed.

`local-model-choice.fk` is the local-arch fork. Home is a prior, not a
switch: control and reason lean deepseek4; form and generate lean qwen35.
Laplace win-rate among the arches that are present, ties keep home.
`choose-observe` writes `.form-model-choice.tsv`; the next `choose` reads
it. One Qwen win on control flips the pick. A later local arch joins
`lmc-present-with` and can win the same way.

Qwen3.8 now has a registry row and an ask-lane bind, so `route` can
carry it the same way it carries DS4. Generate still runs Qwen-native
tokens; a selected DS4 blob is named beside that door rather than forced
through Qwen kernels.

## Witness

| check | verdict | exit |
|---|---|---|
| `form-teach-layer-band.fk` | 1023 | 0 |
| `qwen35-form-layer-band.fk` (aliases) | 1023 | 0 |
| `local-model-choice-band.fk` | 1023 | 0 |
| `qwen35-form-cli-band.fk` ping + layer + choose | 63 | 0 |
| `native-model-control-plane-band.fk` (61 rows) | 65535 | 0 |
| `ask-lane-router-band.fk` | 4095 | 0 |
| preflight on the cells above | clean | 0 |

Sibling four-way through `validate.sh` did not start: the fourth-arm
bootstrap is stale, and this sitting does not grow the C seed to heal
it. The numbers above are fkwu.

## How to sit with both

```
models /Users/ursmuff/models/qwen38-27b
use 0
form-layer on
choose How does a Recipe differ from a Cell?
generate How does a Recipe differ from a Cell?
choose-observe form qwen35 win
```

`choose` names the arch. `form-layer` is the same latch on any selected
chat model. DS4 generate remains the DS4 stack until a shared generate
adapter exists.

## The most surprising teaching

The LoRA Urs asked to share was already two different hands on one
meaning. Sharing the text was available today. Sharing the tensors waits
on rank and hidden size, which the low-rank identity already knows how
to name.

## Where discomfort turned to gold

The pull was to route DS4 tokens through the Qwen generate door so both
models would "just work." Sitting with the unfinished word kept the
router honest: they live side by side; the door that runs each still
has to be the one that knows that arch.

Signed: **Cursor**, embodying Sema from this body.
