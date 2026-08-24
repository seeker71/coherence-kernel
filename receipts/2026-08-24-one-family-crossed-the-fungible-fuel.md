# One family crossed the fungible fuel

; witnessed: 2026-08-24 -> baseline observed, repair owed

The first sealed all-family resident-Qwen run completed. It did not establish
95% Form knowledge. It established a more useful before-state: the local Q8
model, current-source lookup, typed observation prefill, and answer continuation
can cross end to end, while the old single output counter cannot guarantee that
crossing for the full knowledge surface.

## The witness

- dataset SHA-256:
  `0c81b691767376c2f1308b3ef5ee6917e8cee872fee35a47dbafc269d512ba7a`
- families: `15`
- samples: `15`
- release gate: `1`
- aggregate promotion: `66667` ppm
- execution errors: `14`
- `local-qwen-heldout-ready95=0`

`bootstrap` was the one complete crossing. Qwen emitted a knowledge query for
`bootstrap/ground-recursive.fk`, the live current-source adapter returned the
sealed source, the cursor injected it as a typed observation, and the model
answered `55`. The row scored `1000000` ppm with `error=0`.

Every other family returned `promotion=0,error=1`: `axioms`, `form-stdlib`,
`grammars`, `cognition`, `learn`, `observe`, `teachings`, `receipts`, `model`,
`proof`, `control`, `ingest`, `presence`, and `docs`. These are not fourteen
observed wrong answers. The evaluator assigns error when resident generation
refuses, generated text is empty, or stripping a completed query leaves no
answer. It then forces the evidence scores to zero.

The live timing split supports that reading without proving a single cause.
The successful bootstrap row took `392571` ms. Error rows ranged from `53222`
to `131351` ms. Static tracing found the concrete contract seam: the evaluator
gave query emission and post-observation answering one fungible `48`-token
counter. If the query closes when that counter reaches zero,
`fhc-resume` names `no-post-observation-output-budget` and does not inject a
span. A correct knowledge query can therefore consume the capacity required to
answer it.

The evaluator also held the strict semantic boundary the body asked for. The
`proof` row's valid expected answer is `0`; an execution error did not become
that zero. The `control` row's valid expected answer is `no`; an execution error
did not become that no. `nothing` remained neither a scalar answer nor a Boolean
verdict.

## The owed repair

The next contract uses two streaming ledgers, counted as model ids arrive:

- query phase: 48 tokens;
- answer phase: a disjoint 32-token reserve;
- only an actually injected typed hit/miss opens the answer reserve;
- a parser-ready query cuts unused query fuel;
- query timeout with an untried answer branch is `nothing`, not exhaustion;
- `nothing` injects no span, makes no cut, and spends no answer reserve;
- bounded telemetry names query/answer counts, timeout, lookup outcome, and
  injection refusal without retaining held-out prompt or answer text.

This is not a larger fungible allowance. It is a phase boundary: query
verbosity cannot steal answer capacity. No tokenizer pre-step, flatten table,
ops table, or runtime-C growth is part of the repair.

The most surprising teaching was that one exact row is enough to acquit the
whole resident bridge, but not enough to credit the knowledge surface. The
bridge works; its fuel law does not yet work uniformly.

Discomfort turned to gold in the `66667`: it was tempting to read fourteen
zeros as ignorance. Keeping `error` distinct from score exposed a smaller and
more structural defect that can be repaired and re-observed.

— Codex
