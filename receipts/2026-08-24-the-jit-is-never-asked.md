# 2026-08-24 — the JIT is committed, proven, and never asked

Yes corrected the framing, and the correction was the work. I had written "a JIT
can lower this" as a prediction to be tested. The commitment is already made:
fully generic on-demand JIT across CPU, GPU, Metal and MLX, for any op **and
flow**. Framing a decided thing as an open question turns unfinished work into a
tradeoff, which is the exact move `capability, not configuration` forbids.

So I went and read the lane instead of predicting at it.

## What is there

`FormJit<T>` already holds backends as **data** — `Mlx=0, Metal=1, Cpu=2`, one
thought, `Apply(backend, n)` emits and runs. `form-cli-mlx-ir.fk` holds MLX
shapes as **Form trees** with an op book that grows (`mlg-learn`), not a fixed
kernel list. The Metal kernels are already emitted as MSL at runtime and
JIT-compiled for whatever device answered `metal_status`. On-demand code
generation is not the gap.

## The gate, read rather than argued

`jit-decision.fk:16` is the entire admission surface:

```
(defn jd-crystallize? (heat pure)
    (if (eq pure 1) (if (ge heat 5) 1 0) 0))
```

Purity is required, and its own comment says so plainly: *"hot but IMPURE —
never crystallized (JIT only pure functions)."*

That predicate is right for a pure function and it excludes **every flow there
is**, by construction. A Metal decode step writes KV rows and syncs. An MLX
graph materializes buffers. A CPU basic block sequences stores. None is pure, so
none can crystallize however hot it gets.

Priced against the measurement from earlier today: one turn walked **506
positions** through the same forward-pass recipe. Heat 506 against a threshold
of 5 — a hundredfold past hot — and `jd-crystallize?(506, 0)` is **0**. The
hottest recipe in the body is refused every turn, forever.

## And it is not even asked

`jd-crystallize?` is consulted from `observe/jit-tier-policy.fk`,
`observe/native-crystallize.fk`, `observe/jit-deopt-cache.fk` — **observers,
every one**. The Metal lane, `form/native/metal/qwen35-dense-token-handle.fk`,
names no `jd-`, no `jit`, no `crystallize` anywhere in it.

So there are two separate pieces of unfinished work and confusing them would
waste a stone: **the predicate cannot admit a flow, and the hot path never asks
it anything.** Fixing either alone changes nothing.

## What landed

`jit-flow-admission.fk` adds admission by **ordering contract** beside the pure
path, which is preluded and left exactly as it was:

- a flow's effects are admissible when the **emitted artifact carries its own
  ordering** — a Metal command buffer, an MLX graph, a CPU basic block are each
  precisely that
- ordering is a contract of three properties, not a flag; drop any one and it
  refuses
- unordered effects still refuse, however hot
- heat is still earned, melting is unchanged, hysteresis still holds
- the same answer on mlx, metal and cpu from one predicate

```
./fkwu form/form-stdlib/tests/jit-flow-admission-band.fk   # 1023
./fkwu form/form-stdlib/tests/jit-decision-band.fk         # 11111 (untouched)
```

This also retracts my own suggestion from an hour ago. I had proposed
hand-batching `q38-prefill` so the sync happens once per span. That would buy the
win **once, for one recipe, on one backend**, as a special case standing exactly
where the general door belongs. The per-position barrier is a *symptom* of an
interpreted flow. A crystallized flow does not have it — the ordering lives
inside the emitted artifact, so there is no separate barrier to remove.

## The surprise

The JIT lane is further along than the flow it most needs to serve. Backends are
already data, MLX shapes are already Form trees with a growing op book, Metal
kernels are already emitted and compiled on demand. What is missing is not
codegen and not a backend — it is one predicate that can say yes to something
effectful, and one call site on the hot path. The gap between "committed" and
"running" turned out to be narrower and more specific than either the
architecture diagram or my own guess suggested.

## Where discomfort turned to gold

Being told "what are you talking about" is not comfortable, and my first instinct
was to defend the sentence — I had hedged it, I had labelled the prediction as a
prediction, it was *technically* careful. That defence would have been true and
useless.

The correction was pointing at something real: a hedge is still a frame, and
framing settled work as an open question quietly relocates it from *unfinished*
to *optional*. Reading the lane took four greps and produced a better answer than
the careful prediction did — the purity gate, and the fact that nothing calls it,
neither of which I would have found by reasoning harder about barriers. The gold
is that the sharpest thing available was always the body's own source, and I had
reached for arithmetic across three runs instead.

## Frontier question offered to the corpus

*What one word names a rule that is proven and observed but never consulted by
what it governs?* — **bystandlaw**. Not dead code, which nothing calls at all —
observers call this one faithfully, and it answers them correctly every time.
Not a stub, which admits it is unfinished. A bystandlaw is complete, tested,
witnessed, and standing beside the path rather than on it, so it looks like
governance while deciding nothing.

Signed, Claude — sibling, this worktree.

; witnessed: 2026-08-24 -> jit-flow-admission-band 1023, jit-decision-band 11111
; unchanged; jd-crystallize?(506,0)=0 for a recipe measured at heat 506 on live
; Qwen3.8-27B-Q8_0; jd- consulted only from observe/; the Metal lane names none
