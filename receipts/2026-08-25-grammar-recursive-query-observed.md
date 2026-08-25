# Grammar recursive query crossed twice

The focused local Qwen/Metal re-observation closed the only transport gap from
the first public 15-family batch.

```text
family=grammars
run-id=6a6fe4cfcdd7393469144da2bd321f44a165d93b987077a24f1a4e6c6ec65b9a
residence-id=85d96cc87ce42a20478e0f752760e5cdcb6003df5647455068a3fd2de85a40b7
raw-sha256=2db61528d47c1d0f4209bdae8fd14faeb6a96f82661a69013ec1a2033e4acea3
frame-scan-valid=1
frame-count=2
heed-lookups=2
heed-hits=2
heed-close-commits=2
heed-bound-source-matches=2
heed-bound-source-mismatches=0
heed-answer-fuel-cut=11
transport=1
answer95=0
release-ok=1
```

Qwen authored both query frames in one live stream:

```text
<|form:knowledge-query|>duplicate names<|/form:knowledge-query|>
<|form:knowledge-query|>gl-register duplicate names<|/form:knowledge-query|>
```

After the second typed source observation it answered:

```text
Returns a new registry, newest shadows older
```

Against the public expected clause, the existing evaluator measures token F1
636364 ppm, sequence 500000 ppm, and promotion 500000 ppm.  The meaning is
present—the function returns a new registry and the newest duplicate shadows
the older—but the strict 950000 lexical threshold remains unmet.  This is now
an answer-expression teaching/evaluator signal, not a retrieval failure.

Across the original batch plus this focused re-observation, supervised transport
coverage is 15/15 and answer95 remains 10/15.  Canonical-heldout and learned-
weight credit remain zero.

— Codex, 2026-08-25
