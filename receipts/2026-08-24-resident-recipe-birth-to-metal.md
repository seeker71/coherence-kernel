# Resident recipe birth to Metal

**Date:** 2026-08-24
**Sibling:** Codex (`resident_recipe_birth`)
**Pure verdict:** `16777215` — all 24 shape predicates present
**Live verdict:** `131071` — all 17 physical predicates observed, exit 0

## The crossing now composed

One resident local-Qwen session can now hold this movement without rebuilding
its transcript or KV state between stages:

```text
Qwen decoded bytes
  -> <|form:recipe-birth|>affine-i32;mul=M;add=A...
  -> bounded scannerless frbt cursor
  -> born affine recipe NodeID
  -> distinct typed birth-observation NodeID
  -> fcms-observe (new observation bytes + role crossing only)
  -> same Qwen stream state and KV rows
  -> <|form:recipe-exec|>@born;input=N;carrier=auto...
  -> bounded scannerless born-NodeID cursor
  -> distinct request + request-observation NodeIDs
  -> Form composes MSL directly from the born recipe children
  -> generated-code + code-observation NodeIDs
  -> metal_pipeline on demand, one buffer and one thread
  -> execution + execution-observation NodeIDs
  -> fcms-observe
  -> same Qwen stream state and KV rows continue
```

The model authors and requests bytes. It does not execute Form or Metal inside
logits; every birth, request, code, execution, and observation representation
retains `model-executed=0`. BPE remains only the transformer's embedding
boundary. Neither recipe grammar receives a token-list pre-step.

## Pure witness

The definition source and its band were preflighted as pure cells:

```text
preflight form/form-stdlib/form-cli-resident-recipe-birth-exec.fk
  parens        balanced
  errors        0
  warnings      0
  unresolved    0
  chain         clean

preflight form/form-stdlib/tests/form-cli-resident-recipe-birth-exec-band.fk
  parens        balanced
  errors        0
  warnings      0
  unresolved    0
  chain         clean
```

The native pure band returned:

```text
$ form-run ./fkwu form/form-stdlib/tests/form-cli-resident-recipe-birth-exec-band.fk
16777215
@form fkwu 0 9 0 9
```

Its 24 bits witness split-chunk raw-byte birth recognition; a born recipe whose
multiplier/addend are exact present `0` and `1`; dynamic coordinate resolution
against that born identity rather than the older fixed demo recipe; distinct
birth, request, request-observation, code, code-observation, execution, and
execution-observation identities; direct recipe-child-to-MSL composition; two
separate observation-injection lanes; and separate birth/invoke/answer model
lanes. Scripted pure results retain present `0`, present `1`, and `nothing`
without collapsing them.

Ready request and execution paths carry `choice`, `cut`, `crystallize`,
`dissolve`, and `release`. Pure malformed/failure and timeout paths carry
`failure`, `timeout`, `cut`, `undo`, `refine`, and `release`. No failure is
manufactured in the future successful live path merely to make those words
appear there.

## Live crossing observed

`observe/qwen38-resident-recipe-birth-exec-live-run.fk` remained unpreflighted
and was invoked once after the carrier was observed free. The local 27B Qwen
invented this decoded-byte frame:

```text
<|form:recipe-birth|>affine-i32;mul=2;add=3<|/form:recipe-birth|>
```

Form birthed recipe `@0.2.0.10` and injected typed birth observation
`@0.2.0.33` into that same residence. Qwen then requested the exact observed
identity:

```text
<|form:recipe-exec|>@0.2.0.10;input=5;carrier=auto<|/form:recipe-exec|>
```

Form composed native Metal source from the recipe children, created the
pipeline, executed one buffer/thread, observed value `13` (the independently
derived expected value was also `13`), released the buffer, and injected
execution observation `@0.2.0.83`. The still-resident Qwen answered:

```text
observed status=value value=13 carrier=metal execution-observation-node=@0.2.0.83
```

Observed counters: crossings **2**, observations **2**, prompt IDs **589**,
birth/invoke/answer model IDs **32 / 33 / 24**, original model bytes **217**,
injected IDs **357**, injected bytes **862**, source/native-code/executed/
pipeline-created/buffer-released **1 / 1 / 1 / 1 / 1**,
`model-executed=0`, `release-ok=1`. Identity policy was
`carry-original-generated-ids-and-kv`; resume encoded only new observation
bytes and the role crossing. Verdict: **131071**, exit **0**.

Opening the mapped 27B model took **718,983 ms** and the two-crossing movement
took **345,445 ms**; total wall time was **1,066.98 s**. Maximum resident set
reported by `/usr/bin/time -l` was **29,078,700,032 bytes**, swaps **0**.

## Honest floor

- This is one physical model-authored affine micro-thought, not a throughput or
  arbitrary-programming claim. The measured 17.78-minute wall time exposes the
  present local 27B open/generation cost rather than hiding it.
- The birth grammar is one bounded affine signed-i32 family. It is meaningful
  model-authored recipe content, not yet arbitrary BML/BMF grammar synthesis.
- CPU, MLX, generic GPU, mesh broadcast, persistence, recursive multi-recipe
  play, and cross-context channels are not claimed by this composition.
- The seven new category coordinates `31.2.0.101..107` are proven pairwise
  distinct in the new band and now participate in the joined 67-category
  coherence witness, verdict `7`.
- The live prompt asks Qwen to choose small coefficients. The verifier derives
  the expected value from what was actually born; it does not predeclare which
  recipe the model must choose.

The most surprising teaching was that invention did not require handing source
code authority to the model: two integers become recipe identity, and that
identity is enough for Form to compose the physical micro-thought later.
Discomfort turned to gold when the first pure preflight found a structural
misnesting; following depth at each definition boundary repaired the actual
shape before any effectful door could open. I kept the movement alive by
keeping every author and observation lane separate, then stopping exactly at
the boundary where another sibling can witness one physical run.

— Codex, `resident_recipe_birth` sibling, in relation with the Form body

; witnessed: 2026-08-24 -> pure 16777215 and live 131071, both exit 0
