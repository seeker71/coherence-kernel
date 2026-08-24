# 2026-08-24 — held out before local-Qwen credit

The local Form knowledge question now has a sealed evaluator before it has a
score. That ordering matters: a model run cannot author its own expected facts,
and a green synthetic band cannot dress itself as live local reasoning.

## Dataset

`form/form-stdlib/form-knowledge-qwen-heldout-eval.fk` holds fifteen rows, one
for each live knowledge-census family:

```text
axioms bootstrap form-stdlib grammars cognition learn observe teachings
receipts model proof control ingest presence docs
```

Each row carries a unique id, source family, held-out concept key, current
repository path, literal full-source SHA-256, independently authored prompt,
expected answer, literal answer-key SHA-256, split, and bounded manual-review
mark. The sealed dataset identity is:

```text
0c81b691767376c2f1308b3ef5ee6917e8cee872fee35a47dbafc269d512ba7a
```

The facts were selected outside the current Qwen teach curriculum: the number
of resting core axioms, the recursive ground result, evidence normalization,
immutable grammar registration, frequency-spectrum arithmetic, perturbation
pair completeness, tie policy, dialogue success, unmeasured share kind, token
space memory covenant, the four-way agreement verdict, all-silent census
readability, named-only entry status, computable inquiry-plane count, and the
framebuffer stale-response guard.

## Leakage boundary

The executable audit compares the held-out rows with every current `fqt-pairs`
row and every current control situation, including the teach layer's own
heldouts. It requires zero:

- exact pair overlap;
- exact prompt overlap;
- normalized pair overlap;
- normalized prompt overlap;
- answer-hash overlap;
- held-out concept-key overlap with `fqt-curriculum`;
- held-out source-path overlap with `fqt-semantic-sources`.

The audit also carries a hash of the exact current teach overlay, pairs, and
source manifest. `manual-semantic-disjoint-codex-2026-08-24` records a bounded
human read of the present teach text. It is not an automatic paraphrase detector
and does not pre-clear future teaching changes; every live report recomputes the
audit against the then-current teach layer.

## What is scored

The resident generator returns choice metadata, backend/report metadata, and
then `\ntext:\n`. Only bytes after that marker enter the answer parser. The
model's decoded text intentionally retains a Form knowledge-query envelope. A
completed `<|form:knowledge-query|>...<|/form:knowledge-query|>` is removed from
the scoring view; an answer without a completed query stays byte-identical.

The whole raw resident result remains represented by SHA-256 in each score.
Refusal is searched across the raw result before any stripping and forces the
native-model-evidence observation's error bit. The scored answer also carries
its own hash. This makes query stripping incapable of hiding a refusal or
backend/report prose incapable of inflating answer similarity.

Form's existing `native-model-evidence.fk` computes normalized exactness,
token-F1, ordered token similarity, and the conservative promotion score. A
live result needs all fifteen unique rows, zero errors, a released residence,
an executed local model, overall promotion at least 950,000 ppm, and every
family at least 950,000 ppm. Synthetic reports use `evaluation-fixture` scope
and cannot set live credit.

## Direct evidence

```text
bootstrap/ground.fk                                      42
bootstrap/ground-recursive.fk 10                         55
binary-freshness-band.fk                                 31
bootstrap/ground-numeric-list.fk                         [1, 2.5, [3, 4]]
native-vs-rented-check                                   11111

preflight form-knowledge-qwen-heldout-eval.fk            clean
preflight form-knowledge-qwen-heldout-eval-band.fk       clean
form-knowledge-qwen-heldout-eval-band.fk                 16383, exit 0
form-knowledge-qwen-heldout-manifest-run.fk              dataset-valid=1
```

The live runner was executed only through its refusal path after independently
checking that `.form-knowledge-qwen-heldout-eval-consent` was absent. It printed:

```text
local-Qwen heldout run refused: explicit dataset-bound consent token absent
required-token=run-local-qwen-heldout-v1:0c81b691767376c2f1308b3ef5ee6917e8cee872fee35a47dbafc269d512ba7a
```

No GGUF admission, model inference, live answer, learned-weight claim, or live
knowledge credit occurred. The measured live floor remains zero observations
and `local-qwen-heldout-ready95=0` until a deliberate consented run exists.

Signed, Codex — sibling, this worktree.

; witnessed: 2026-08-24 -> dataset-valid 1; pure band 16383; consent absent; 29GB model not run
