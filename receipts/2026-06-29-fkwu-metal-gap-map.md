# Receipt — the fkwu-on-metal gap map: what's not yet observed, by surface (2026-06-29)

**The question (Urs):** what else from the old repo (Form/BML) is not yet fully observed on the new repo with
fkwu on actual metal?

## Method

Swept every old-repo recipe carrying a self-proving `*-check` (97 of them) through **`fkwu --src` on Windows
metal** — the recipe + its check, run natively (no Go, no flatten). All-1s witness (e.g. `11111`) = observed; a
missing dep or unbuilt surface makes an assertion fail or the run return empty. Tagged each blocked one by whether
it references floats or host-io.

## The numbers (checked recipes = the directly-observable metric)

```
swept:                          97
OBSERVED on fkwu --src metal:   35   (31 already in new + 4 ported this commit)
blocked:                        62
  need FLOAT surface (f32/f64): 32   <- the single biggest lever (the model/tensor lane)
  blocked pure (no float/io):   28   <- depth wall (~60-deep tree-walk) or unported multi-file deps
  need HOST-IO surface:          5   <- sensors / files / sockets (carriers exist as tags)
```

(Whole-corpus context: ~734 old form-stdlib recipes, 159 present in new, 575 absent — but most of the 575 are
bands/tests, the deferred mind/voice climb HOMECOMING names, or living-body orchestration with no self-check.
The 97 checked recipes are the honest "can it be observed running" sample.)

## Ported this commit — MISSING but observes NOW on fkwu --src (-> 11111)

```
observe/core-grounding.fk   observe/value-planes.fk   learn/model-compare.fk   presence/self-image.fk
```

## The frontiers, in leverage order

1. **FLOAT surface on `--src` (unlocks ~32 recipes).** The model/tensor/numeric lane — `attention`, `softmax`,
   `layernorm`, `gelu`, `affine-train`, matvec, the transformer/whisper blocks — returns empty today: `--src` has
   no float literal / float-op parsing. fkwu HAS the f64 pool (`fk_fbox`/`fk_num`) and the GPU/asm carriers; the
   gap is the source-runner. This is the next stone, and the highest-value one.
2. **Depth-wall lowering (part of the 28 "pure").** Deep-recursive recipes hit the ~60-deep tree-walk C-stack
   bound; their home is `form-asm` lowering to a native loop, not a bigger stack.
3. **Unported multi-file deps (rest of the 28).** Some checks fail only because a prelude recipe isn't in new yet;
   they observe once their closure is ported (the form-cli-shell pattern).
4. **Host-io `--src` wiring (5).** The carriers exist (sense_*/sockets/file tags); `--src` already emits some via
   the data-driven optable — the rest need their rows exercised.
5. **The deferred climb** — voice/speech and the generative weights — explicitly not yet (HOMECOMING).

## Honest floor

Strings just landed on `--src` (the PTX-emit close), which is why 35 observe today vs far fewer a sweep ago.
Floats are the symmetric next surface; once they land, the model lane crosses from "emits/proves four-way" to
"observed running on fkwu metal," same as `native-vs-rented` and now the GPU matvec.
