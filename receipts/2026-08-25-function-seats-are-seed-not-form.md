# 2026-08-25 — function seats are the seed seam, not Form

Urs asked the question that corrected the movement: **“we have functions
seats? is that how modern compilers are operating?”**

No.  The observed limit is this checkout witness:

```c
#define FK_FN_CAP 4096
```

`runtime/fkwu-uni.c` rejects a source definition at index 4096 and rejects a
loaded `.fkb` whose function count exceeds that same static array.  That is a
bounded arena in the temporary C seed.  It is not a Form semantic, a desired
compiler architecture, or a target to pack against.  Mature compiler pipelines
normally grow symbol/IR storage and compile, link, cache, or load units
incrementally.  Resource budgets remain useful; a 4096-function whole-world
ceiling does not.

## The rejected attempts

The first query-memory live runner aggregated the complete carrier and the
rotation source.  Fresh compilation reached `[fn-cap]` at definition 4096 and
recorded 391 errors.  I deleted that runner and its exact `.fkb/.sym` error
caches instead of enlarging the C array or compressing meanings into fewer
functions.

After the data-cell movement below, I made one bounded dormant join against the
existing query rehearsal.  The loader itself is only 16 definitions, but the
current source importer replayed `core`; `fka1l-max-generative` arrived as
definition 4096 and the join recorded 163 errors.  That join and its exact error
caches were also deleted.  I did not create a special “no-core” loader fork.
The failure localizes the remaining seam to dependency-identity deduplication /
on-demand native unit loading.

## The movement that remains

Selected knowledge no longer has to arrive as a source module:

- `form-query-memory-cell-rotation.fk` is a fixed 16-definition bounded
  loader.  A selected `.formq` contributes **zero definitions** to the resident
  closure.  It binds exact caller-supplied action NodeID, source path, current
  source identity, admitted bytes, size, and mtime in a content-addressed cell.
- `query-memory-axioms.formq` is 260 answer-free bytes.  Its only semantic
  payload is the scannerless query
  `<|form:knowledge-query|>resting point<|/form:knowledge-query|>`.
- `form-knowledge-query-memory-rotation-token.fk` is a small 21-definition
  recipe-data boundary.  Category `31.2.0.127` is the loaded cell;
  `31.2.0.128..130` are its executable recipe, request, and returned choice.
  The request contains the existing action identity `31.2.0.109`,
  `pretokenized=0`, and `scannerless=1`.
- `form-knowledge-query-memory-rotation.fk` now projects the real persisted
  answer-free memory and lane-b presentation into those tokens.  Keep, undo,
  timeout, cut, actual `nothing`, generated zero, and generated one remain
  distinct.
- `qwen38-axioms-query-rehearsal-live-run.fk` now accepts selected
  scannerless query bytes through `fkaqrl-run-with-query`; the original
  `fkaqrl-run` remains as a compatibility door.  The selected bytes are
  parsed before their anchor can construct an offer, and the rebuilt offer
  must reproduce the exact selected frame.

There is no registry, operations table, flattening, C growth, remote call,
model call, or Metal call in this movement.  The ten-row local audit already
owned the model/GPU lane, so this work remained source/data-only.

## Evidence

```text
form-query-memory-cell-rotation.fk
  preflight balanced, errors 0, warnings 0, unresolved 0
form-query-memory-cell-rotation-band.fk
  preflight balanced, errors 0, warnings 0, unresolved 0
  direct 65535, exit 0

form-knowledge-query-memory-rotation-token.fk
  preflight balanced, errors 0, warnings 0, unresolved 0
form-knowledge-query-memory-rotation-token-band.fk
  preflight balanced, errors 0, warnings 0, unresolved 0
  direct 65535, exit 0

form-knowledge-query-memory-rotation.fk
  preflight balanced, errors 0, warnings 0, unresolved 0
form-knowledge-query-memory-rotation-band.fk
  preflight balanced, errors 0, warnings 0, unresolved 0
  direct 4194303, exit 0

qwen38-axioms-query-rehearsal-live-run.fk
  preflight balanced, errors 0, warnings 0, unresolved 0

runtime/fkwu-uni.c delta
  0
```

The fresh health census is now **25 observed organs / 18 ready / 7 gaps /
720 permille**.  Its selected gap is
`source-unit-on-demand-load`; that denominator is this pulse, not a fixed
target.  The immediate memory-data/request organ is ready.  Its physical
resident join remains pending until it can cross without replaying the source
world.

— Codex (OpenAI), 2026-08-25

; witnessed: 2026-08-25 -> loader/token bands 65535, real rotation 4194303,
; query-input preflight clean, rejected joins at FK_FN_CAP, health 25/18/7/720
