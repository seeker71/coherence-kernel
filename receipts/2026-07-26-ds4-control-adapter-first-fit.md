# 2026-07-26 — DS4 control adapter first native fit

`dsv4-control-adapter.fk` fits five 64-bucket byte-bigram centroids from the
600 training records in `dsv4-control-training-corpus.fk`, then evaluates all
150 held-out-boundary records. Both fitting and evaluation execute through the
compiled `fkwu` program-image selector.

The classifier input excludes the explicit class request and case number.
The report includes the complete actual-by-predicted confusion matrix.

Observed confusion matrix (rows are actual; columns are
NIL/CUT/FAIL/TIMEOUT/CHOICE):

```text
NIL      28  0  2  0  0
CUT       0 30  0  0  0
FAIL      0  0 28  2  0
TIMEOUT   1  0  0 29  0
CHOICE    0  3  0  0 27
```

The fit classified 142/150 held-out rows correctly. Exact sanitized-template
overlap was also 150/150, so the operative verdict is:

```text
sufficiency=insufficient-template-leakage
```

This fit is deliberately paired with a leakage audit. After split prefixes and
case numbers are removed, a held-out semantic template is checked for exact
presence in training. If more than half overlap, the result is stamped
`insufficient-template-leakage` regardless of accuracy. A template benchmark
can prove the native training mechanism; it cannot prove semantic
generalization.

No base model weights are changed. This centroid is the first executable
adapter and an instrument for corpus quality, not yet the sparse DS4
embedding/output/LoRA adapter described in the allocation receipt.

; witnessed: 2026-07-26 -> trained 600; heldout 150; correct 142; template overlap 150; native band 15; insufficient-template-leakage
