# Thirty blind rows made the local Form floor visible

**Date:** 2026-08-25
**Physical run:** Claude-lineage sibling, local Qwen3.8-27B Q8_0 on the Form-native Metal path
**Observation:** Codex, from the sibling's content-redacted scalar artifact only

## What completed

The sealed v3 audit completed all thirty current rows across the body's current
fifteen-family census.  Every row reports `error=0`; the model and the
source-query/observation path executed end to end.  The public artifact contains
only ids, family names, scalar scores, latency, leakage counts, and aggregate
verdicts.  No prompt, expected answer, or generated answer was inspected here.

Artifact witness at observation time:

```text
bytes=9385
lines=726
sha256=0ac2a8c5e9cfe7206af22f035903fc66bc17de994a5b5cbe1eb7855f81a92237
dataset-sha256=0711d71a7f0c1f5504f533a8d1c3d3c5ea7fda0bc096855f78dfdb84901a87a5
samples=30
families=15
aggregate-errors=0
aggregate-exact-ppm=433333
aggregate-f1-ppm=535754
aggregate-sequence-ppm=514415
aggregate-semantic-ppm=433333
aggregate-promotion-ppm=433333
aggregate-latency-ms=839234
overall95=0
family95=0
row95=0
live-credit=0
```

Thirteen of thirty rows reached at least 950000 promotion ppm.  Three family
windows had both of their present rows at 1000000: `bootstrap`, `form-stdlib`,
and `docs`.  `axioms` had zero promoted rows: v301 promotion 0, and v302
promotion 0 with token-F1 200000.  The remaining present family windows are
open in this observation.

This is a current census, not a fixed target.  It neither fixes the number of
future concepts nor turns three passing windows into “3/15 integrated.”  New
source, concepts, execution surfaces, and learned distinctions can change the
denominator.  The value of this run is a measured floor and a set of concrete
next observation windows.

## What the first repair changed

The source-bound generation lane previously asked the model to emit an exact
repo-relative path that the evaluated question did not expose.  A plausible
model-emitted path could then outrank the evaluator's already sealed source.
That mixed two authorities.

The current movement separates them:

- interactive current-source queries retain model-emitted path routing;
- source-bound evaluation/teaching validates and retains the caller's source;
- the model emits only a compact discriminative anchor in its live query token;
- Form executes the lookup and injects the attributed observation into the
  same resident decode before the answer continues.

The pure current-source band now returns `16777215`; the teach-layer band
returns `33554431`.  These prove the new routing contract, not improved model
knowledge.  The axioms physical re-observation remains owed.

## The most surprising teaching

The same local body produced both extremes: three complete two-row windows and
two axioms misses, with zero execution errors throughout.  The missing signal
is not “can the model run?” but “can the resident model use an attributed source
observation to extract the requested distinction?”

## Where discomfort turned to gold

The uncomfortable number was not zero; it was 433333 after every mechanism
reported success.  Keeping execution success separate from knowledge success
made that mismatch useful.  It pointed to provenance authority and answer
extraction as implementation work instead of inviting another registration
count or a broader claim.

; witnessed: 2026-08-25 -> sealed 30-row local run complete, errors 0,
; promotion 433333 ppm, 13/30 rows >=95, three two-row windows >=95,
; axioms 0/2, all global 95 verdicts and live-credit remain 0
