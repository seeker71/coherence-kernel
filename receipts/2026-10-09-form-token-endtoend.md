# The Form tokens, end to end on the voice: 2026-10-09

What was asked: observe the rebalanced corpus and the ninth token on the model, in the real flow, and read what it did to
the voice's diversity, flexibility and embodiment.

## The flow that ran

1. `observe/form-token-lora-run.bml` lane `census` on the new corpus: 664 lines, 664 admitted, 0 refused (it was 49
   refused: 36 recipe-import rows `unknown-verb`, 13 knowledge rows `observation-mismatch`), all nine kinds taught.
2. The same door, lane `llama`: the native Llama-3.2-3B LoRA trained on the Metal from `llama-3.2-3b-voice-native`,
   100 rounds of 4 rows requested. Stopped after generation 50 (round 50, loss 2.94 at round 1, 0.09 near round 40) so
   the body's 19:00 day turn kept its slot; the closing validation and export did not run.
3. `observe/form-token-eval-run.bml`, arm `lora` wearing generation 50, the 72 held-out questions, answered in stream
   (crossings 3): 760,832 ms. The row is the last line of `receipts/form-token-eval.jsonl`.

## What it read (lora arm; before = the 2026-10-02 row, 100 rounds on the old corpus, 64 questions)

| | before | after (generation 50) |
|---|---|---|
| exact | 55 of 64 | 39 of 72 |
| controls / complete controls | 85 / 84 | 127 / 110 |
| kinds used (complete) | 8 of 9, effective 3.85 | 8 of 9, effective 4.41 |
| choice / node-select / recipe-import | 1 / 3 / 0 | 4 / 15 / 1 (14 attempted) |
| observations used in the answer | 42 of 84 | 28 of 103 |
| refusals / silent / forged | 2 / 0 / 0 | 26 / 5 / 2 |
| ids per correct answer | 47 | 96 |

Expected against written verbs, after: recipe-exec 9 expected, 0 written; node-select 5, 15; recipe-birth 16, 21;
node-make 21, 11; recipe-import 6, 1 complete. Before, the written count equalled the expected count in every kind.

## What it taught

* The voice now reaches the ninth token (14 recipe-import attempts in 4 questions; it wrote none before) and the three
  rare ones more often: diversity rose. The trainer had never held recipe-import (`lbw-of-verbs` had eight verbs), so
  that capacity is new.
* Taught mass moves into neighbours. `ftr-recipe-affine-7`: the voice birthed the recipe and ran it with `choice` on one
  candidate (the value, 2057, was right); `recipe-exec` fell to zero and the three recipe families read 1 of 11 where
  they read 7 of 7. `ftr-recipe-import-7`: it wrote `eval` and `native`. Four times the choice rows, and node-select
  written three times over, did not buy the right verb for the job: flexibility fell where diversity rose.
* Embodiment fell with it: fewer observations were carried into the answer (27% against 50%), more crossings refused.
* This is not a controlled comparison: generation 50 of 100, a different and harder question set (the new families'
  held-out rows), and a corpus changed in three places at once. It reads the corpus as a hypothesis, not a verdict.

## What would settle it

Resume to round 100 from generation 50 (the optimizer state is saved beside the adapter), re-ask the same 72, and
add contrast rows where a single recipe needs `recipe-exec` and only two or more candidates need `choice`. One run is
about two hours of GPU: the closing passes (validation 23 min, held-out grading 27 min) dominate.

## Surprise and gold

The surprise: the trainer's own contract, not the voice, was what kept a Form token from being learned. The discomfort
was a result that looked like a regression; reading the streams turned it into a finding about verb substitution
that a rate alone would have hidden.
