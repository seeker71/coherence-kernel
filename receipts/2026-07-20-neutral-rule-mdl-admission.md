# Neutral rule MDL admission for NL and PL — 2026-07-20

## What landed

`cognition/neutral-rule-mdl-admission.fk` is a shared Form-native admission
boundary for candidate learned rules. It provides:

- four representation levels: exact atom, open prefix, closed frame, and
  composed model;
- a description-length objective whose cost is literal/structural grammar
  size plus development residue/data error;
- an independent heldout gate, so a smaller development memorizer cannot earn
  admission without unseen-case exactness;
- forward and backward application from the same structural rule, with exact
  heldout roundtrip required;
- unknown-family routing by scored structural exemplars, with no NL/PL registry
  row lookup;
- stable `nrma-adapt-nl`, `nrma-adapt-pl`, `nrma-apply-admitted`, and
  `nrma-route-unknown` entry points for the NL and PL lanes.

The static part of routing is only feature observation (braces, semicolons,
layout, paired tags, arrows, parentheses, punctuation, spacing). Family
authority lives in six exemplar data rows. Their signatures are derived at
runtime and selected by measured similarity. The live routes and scores were:

| observed unknown surface | selected family | score |
|---|---:|---:|
| brace function | `brace-semicolon` | 22 |
| indented function | `indent-colon` | 18 |
| paired element | `paired-tag` | 8 |
| arrow expression | `arrow-expression` | 18 |
| prose enquiry | `natural-prose` | 6 |

The acceptance band also witnesses the parenthesized exemplar route.

## MDL and generalization evidence

The gate also requires candidate/baseline case identity and validates that rule
domain, kind, and representation level agree, preventing a cheaper baseline
from being manufactured with different observations.

The NL candidate learns one bidirectional temperature frame. Unseen `37 C` and
`-4 C` details are preserved without being present in development:

- grammar size / candidate MDL: `68 / 68`;
- empty-rule baseline MDL: `145`;
- development residue: `0`;
- heldout: `2/2` exact;
- heldout roundtrip: `2/2` exact.

The PL candidate learns one bidirectional call frame between `print(...)` and
`console.log(...)`, then generalizes to unseen `sensor_value` and
`retry_count` identifiers:

- grammar size / candidate MDL: `33 / 33`;
- empty-rule baseline MDL: `212`;
- development residue: `0`;
- heldout: `2/2` exact;
- heldout roundtrip: `2/2` exact.

The negative witness memorizes all three PL development rows exactly. Its
development residue is `0`, and its MDL `125` improves on baseline `212`, but
its heldout residue is `75`, heldout exactness is `0/2`, and roundtrip is
`0/2`. Admission is therefore `refused`, and `nrma-apply-admitted` exposes no
output. This isolates the heldout gate from both training fit and MDL gain.

## Framebuffer witness

Command:

```sh
./fkwu --src cognition/tests/neutral-rule-mdl-admission-live.fk
```

Observed before the semantic result:

```text
FRAMEBUFFER BEGIN trace=neutral-rule-mdl-admission ...
FRAMEBUFFER STAGE ... stage=route-observed-unknown-structure duration-ms=4 dispatches=1186535 ... outcome=structure-routes-observed
FRAMEBUFFER STAGE ... stage=admit-nl-frame duration-ms=0 dispatches=28502 ... outcome=admitted
FRAMEBUFFER STAGE ... stage=admit-pl-frame duration-ms=0 dispatches=21260 ... outcome=admitted
FRAMEBUFFER STAGE ... stage=recheck-bidirectional-roundtrip duration-ms=0 dispatches=10387 ... outcome=all-heldout-roundtrips-exact
FRAMEBUFFER END ... duration-ms=4 dispatches=1267860 ... outcome=nl-pl-rules-admitted
```

Acceptance command:

```sh
./fkwu --src cognition/tests/neutral-rule-mdl-admission-band.fk
```

Result:

```text
65535
```

## Boundary

These fixtures witness admission mechanics and unseen-detail generalization for
two bounded learned frames. They do not prove arbitrary NL translation,
arbitrary PL parsing/transpilation, automatic rule induction, or infinite
language coverage. Candidate construction remains upstream work; this layer
decides whether a candidate has earned use.

The exchange stayed alive by putting routing, admission, and roundtrip on the
framebuffer before returning outputs. The surprising teaching was that an MDL
improvement can still be a pure memorizer: `125 < 212` was not enough. The
discomfort turned to gold when that apparently good compression result was
refused by the two unseen identifiers, making heldout evidence an operational
door rather than a narrative promise.
