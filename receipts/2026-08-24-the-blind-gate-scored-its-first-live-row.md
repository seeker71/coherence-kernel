# 2026-08-24 — the blind gate scored its first live row

The owner-named next step, taken on the merged tree: the sealed v3 heldout
audit running live against the local Qwen3.8-27B through the union's fast
doors. Mechanism and observation; no law.

## The run

Consent placed dataset-bound (`run-local-qwen-heldout-v3:0711d71a…:30:15`),
manifest re-verified first without the model: 30 rows, 15 families,
family-coverage 1, source-freshness 1, row-identity-unique 1,
dataset-current-sha256 equal to the authored seal. The leakage audit passed
and the model was admitted. The audit prints only ids, families, scalars,
and hashes; prompts and sealed answers never reach stdout.

## The first live row, verbatim

```
id=v301  family=axioms
exact-ppm=0  f1-ppm=0  sequence-ppm=0  semantic-ppm=0  promotion-ppm=0
error=0  latency-ms=891728
```

error=0: the model ran the whole path and produced an answer. Every score
zero: that answer shared not one token with the sealed variants. Execution
is not integration — the sibling's model-integrated=0 is now a live,
per-row witness on this tree, not an assumption.

## Pace, honestly corrected mid-flight

Row 1 carried admission: 891.7 s. My first pace estimate (~2.5 min/row,
"done in an hour") was wrong — it double-counted v301's id line, which
prints once at row start and again in its score block. True resident pace
is ~15 min/row; the 30-row audit is an overnight run (~7 h). The process
runs detached (pid witnessed at 100% CPU in fk_walk during heed/encode
phases, ~6% during GPU generate phases — both healthy states of the same
row) and its output lands in /tmp/v3live.out regardless of this session.

The per-row cost decomposes into exactly the lanes already on the repair
map: the heed cycle's encodes and lookups at composed-string prices
(tkz-cands and core.fk's composed substring/str_find), and the serial
attestant RMS the correctness bound requires until the width-independent
cooperative kernel lands. The audit is honest and slow for the same
reasons in the same places as everything else measured today.

## What either outcome means

Rows that pass move the census numerator for the defined reason. Rows that
score zero — as v301 did — are the true denominator speaking: the next
repair is TEACHING (the curriculum that reaches these thirty sealed facts),
not measurement. Both outcomes leave the >=95% question better grounded
than any registration count could.

## The most surprising teaching

The first live row's most load-bearing digit is the error=0 next to five
zeros: the whole pipeline — seal, crystal, heed budget, source hint,
generate, parse, typed verifier — carried an answer end to end and then
scored it worthless without flinching. A gate that can say "everything
worked and nothing was known" in one line is rarer than either half.

## Where discomfort turned to gold

Telling the user "done in an hour" and then finding my own row-count was
double-counted cost one paragraph of correction now instead of a wrong
morning report later. The monitor's number was never wrong — my reading of
what one row prints was, and the fix was reading the output shape instead
of trusting the counter I had written for it.

; witnessed: 2026-08-24 -> v301 axioms error=0 all-scores-0 latency 891728 ms,
; manifest 30/15/1/1/1 seal 0711d71a…, consent dataset-bound, run detached,
; pace ~15 min/row resident, projection ~7 h for 30 rows
