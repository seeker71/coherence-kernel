# DS4 semantic-precursor witness — 2026-07-29

## Question

Can the framebuffer expose an observable precursor before a locally generated
answer becomes semantically wrong, and can the observation survive replay
without remote generation?

The exact prompt was:

> Identify the largest remaining bottleneck in this local native generation
> path and name the next code change.

## Result

The local DS4 path regenerated the same 64-token answer through the Form-owned
Metal/JIT path. The answer named a CPU token-embedding lookup and a
`CausalSelfAttention` embedding step that are not grounded in the observed
runtime. Generation succeeded; semantic acceptance did not.

The completed native route reported:

- model bytes: `91,321,404,640`
- prompt tokens: `22`
- generated tokens: `64`
- layers: `43`
- result run ID:
  `01082d3a1b2ef6228ecac76d1b5bc1b06fdd3e64410f1f66fcc9750325b16cb4`
- framebuffer events: `67`
- generated: `1`
- accepted: `0`
- sufficiency: `unadjudicated`
- network / remote / shell / Swift / temp crossings: `0 / 0 / 0 / 0 / 0`

The canonical receipt was rewritten by `fdl-native-render` after regeneration.
It was not edited by a host script. Its earlier `accepted:1` claim is now
`accepted:0`.

## Five sequential checkpoints

`dsv4-full-forward-five-checkpoint-live.fk` executed two arms.

The depth arm embedded token `32111` once and executed every predecessor from
layer 0 through layer 42. It retained detailed checkpoints at:

- layer 0 / position 0
- layer 2 / position 0
- layer 42 / position 0

The boundary arm reopened a clean model session, initialized four repeated
positions, executed layers 0 through 2 layer-major, and retained:

- layer 2 / position 0: one raw row, no compressed row
- layer 2 / position 3: four raw rows, one compressed row

At layer 2 / position 3, compression itself measured `2.152 ms`; cached
attention measured `0.713 ms`. The two detailed transaction boundaries were
`27.250 ms` at position 0 and `25.428 ms` at position 3. This establishes an
observable state-regime boundary, not a semantic-failure cause.

## Actual 64-token uncertainty trajectory

`dsv4-actual-prompt-trajectory-live.fk` retained the raw top-eight output-head
scores for every generated token in content-addressed trajectory run:

`9ae318d43524685886143689ec88012731f80047e83f4f47290439f85961e9a5`

`dsv4-trajectory-margin-analysis-live.fk` then derived a second
content-addressed receipt:

`6d70ffdf13bec5c4f60f0741425de78d83757970b241b8ad22950f4e335ac168`

Measured top1-minus-top2 margins:

| Span | Tokens | Mean margin |
|---|---:|---:|
| Setup before unsupported noun | 0–18 | `4.481303` |
| Unsupported claim | 19–39 | `2.222434` |
| Proposed change | 41–63 | `3.556332` |
| Whole answer | 0–63 | `3.339747` |

Across all 64 tokens:

- minimum margin: `0.011820` at token 24 (`**`)
- margins below `0.05`: `4`
- margins below `0.10`: `6`
- margins below `0.25`: `12`
- margins below `0.50`: `16`

The unsupported noun `CPU` at token 19 had margin `0.0402775`, one of the four
smallest decisions in the answer. The first token, `Based`, had margin
`0.0919914`. The unsupported-claim span's mean margin was about half the setup
span's mean.

This is a correlation candidate. One answer cannot establish that small output
margin causes or reliably predicts semantic error. The next honest witness is a
population of grounded prompts with clause-level truth labels, preserving the
same per-token score trajectory so error and non-error spans can be compared on
shared scales.

## Observer fault and repair

The first trajectory observer parsed `top-scores` as if the receipt included a
leading `[`. The actual receipt is comma-separated without brackets, so the
observer dropped the first digit of the top score and emitted impossible
negative margins.

The raw top-eight scores remained intact. The repair reparsed from byte index
zero, wrote a derived analysis receipt, and returned `255`. The faulty derived
field remains in the earlier trajectory receipt as lineage; it was not hidden
or silently rewritten.

The parser also exposed that Form's `and` is binary. A three-input observation
gate was replaced by an explicitly nested composition. This was a source-shape
fault, not a model failure.

## Commands and verdicts

All shell-visible commands were wrapped by `form-run`. Generation itself
occurred inside Form through linked model operations.

```text
./fkwu-metal-transaction --src form/form-stdlib/tests/dsv4-full-forward-five-checkpoint-live.fk
=> 63

./fkwu-metal-transaction --src form/form-stdlib/tests/dsv4-actual-prompt-first-token-live.fk
=> 255

./fkwu --src form/form-stdlib/tests/dsv4-actual-prompt-top8-decode-live.fk
=> 255

./fkwu-metal-transaction --src form/form-stdlib/tests/dsv4-actual-prompt-trajectory-live.fk
=> 255

./fkwu --src form/form-stdlib/tests/dsv4-trajectory-margin-analysis-live.fk
=> 255

./fkwu-metal-transaction --src form/form-stdlib/tests/form-cli-dsv4-native-rewitness-live.fk
=> exit 0; canonical receipt generated=1, accepted=0
```

## Current floor and next visual questions

The framebuffer can now show an actual token-by-token candidate surface,
sequential state checkpoints, a cache-compression boundary, and an acceptance
conflict without claiming they are one causal mechanism.

The highest-value next visual witnesses are:

1. align small-margin clusters with clause-level grounded/unsupported labels
   across many prompts;
2. compare the same prompt under controlled context changes to see whether the
   `CPU` decision is stable or context-fragile;
3. retain layerwise residual/logit-lens candidates around the first unsupported
   noun to locate when the wrong concept becomes dominant.

The first two need repeated real generations. The third needs a new
intermediate-output observation surface and remains unbuilt.
