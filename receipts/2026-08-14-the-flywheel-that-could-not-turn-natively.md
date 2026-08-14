# The flywheel that could not turn on its own arm

**Date:** 2026-08-14
**Status:** witnessed; one repair landed, one seam left honestly red
**Repaired:** `form/form-stdlib/tests/training-catalog-band.fk`,
`form/form-stdlib/tests/form-cli-loop-band.fk` — one word each
**Corpus:** row 1004, `bundlemask`

## The question

*How is form-cli integrated, and how does the model learn from the core dictionary to generate
answers and code?* Traced rather than described.

## What each piece actually is

**form-cli** is a real Mach-O arm64 binary, 1.99 MB, built 2026-08-13 23:17. On this host it
answers `--help`, no args, and a cell path all the same way: no output, exit 0. It embeds Form
source, including `fkc-main-{repl,jit,load,parse,server,universal,uwire}-text` — *emitters* that
produce Form source text. That is the code-generation surface here: template emission from Form
cells, not model output.

**The routing** is `routers/form-cli-router.fk` (band **31**, exit 0) — one fitness formula over
four axes (sovereignty, trust, capability as a hard gate, confidence) choosing between exactly two
backends. Its own header is blunt: *"no metered LLM REST API exists in this body."* The two are
`form-native` (local Form recipe, ~0 cost) and `agent-cli` (the flat-rate subscription CLI already
running).

**The flywheel** is `oracle-flywheel.fk` (band **31**, exit 0): ask native first, fall to the oracle
only when unsure, learn from samples until native handles the class. Every oracle call is captured
into `training-catalog.fk` to teach the local lane; as capability and confidence rise, sovereignty
flips the same request to local. The model doing that learning is **affine** —
`affine-train.fk` / `affine-corpus-fit.fk` (band **31**, exit 0) — not a transformer. Its honest
lever is stated in the cell: more SAMPLES generalize, more epochs overfit.

**The core dictionary** is `core-lexicon.fk`, 438 lines, band **262143**, exit 0. Sixty-four words
in eight families of eight, and *closed*: every definition is stored as a token list drawn from the
same 64 plus eight declared glue words, and `cl-closed?` walks every token of all 64 definitions to
prove it. It is a self-grounding vocabulary the kernel uses for its own semantics — **not** training
data, and nothing feeds it into a model.

## The seam, and it was under the load-bearing part

`training-catalog.fk` — the cell where oracle calls are captured to teach the local lane — and
`form-cli-loop.fk` — the loop itself — both printed green-looking numbers on fkwu with **exit 1**:
1023 and 31. By this body's own rule that is a fold over `nothing`.

```text
preflight training-catalog-band.fk
  errors 14 · unresolved 5
  chain  CARRIED ERRORS — any verdict from this chain is a fold over `nothing`
    json-array-length -> LANE SEAM — resolves on: go rust ts
```

Preflight called it a lane seam. It was not one. `json-array-length` and its four siblings are not
natives in any kernel — `fkwu-uni.c` has zero, and so do all three full kernels. They are **Form**,
defined in `form-stdlib/json.fk:400-427`. `code-tool-learning.fk`, which both cells prelude, carries
no `; preludes:` line of its own, and every caller's chain omitted `json.fk`.

Why it looked like a lane: the full kernels bundle the stdlib, so the symbol is simply present
there. fkwu resolves preludes explicitly, so only the native arm ever showed the gap — and the
bundling made a missing prelude wear another kernel's face.

One word in each of two prelude lines: **errors 14 → 0, unresolved 5 → 1, chain clean.**

## The one left red on purpose

`value_kind` is a genuine native — go 4, rust 6, ts 1, fkwu 0. `form-stdlib/fourth-shim.fk:247`
offers a Form shim for it, and preluding that would have cleared the last unresolved call and
produced a full green. It answers only `"null"` or `"value"`, while callers ask it for `"string"`,
`"int"` and `"list"` (`program-image-sym-lens.fk:57-59`). A lossy shim under a discriminating
question buys a numb green. Left red, named here.

## The answer, plainly

- form-cli **routes**; it does not generate with a model. Two backends, one formula.
- The core dictionary is a closed **semantic floor**, not a training set. No path feeds it to a model.
- The learning that is genuinely closed runs on a small **affine** model over captured samples.
- Code generation is **Form source emission** — `fkc-emit-*`, `recipe-gen.fk` (band 63, exit 0).
- Native language generation is at **rung 3a** and honestly pending —
  `receipts/2026-06-29-native-zh-summary-PENDING.md` names all six rungs, with rung 5 (a real
  emitted summary) as the gate. One llama step at position 0 on stories260K produced `" Once"` from
  GPU logits on 2026-07-02. That is the true floor.
- And until this morning the **capture step that feeds the flywheel could not execute on the body's
  own arm.**

> **frontier question** — what names a missing prelude wearing the face of another kernel's native?
> **bundlemask** (0-hit fresh at offering)

Corpus re-probed before pinning: 397 rows / 397 admissible / max-mid 1004 / 0 duplicate ids /
field code 397039721004. Band **32767**, exit 0.

## The most surprising teaching

The architecture is not the aspirational part. `form-cli-router.fk` describes the flywheel
completely and correctly, and `oracle-flywheel.fk` closes the loop with an honest lever. Every
sentence in them is true. What was missing was one word in a prelude line underneath — and because
the full kernels bundle what fkwu is handed, the gap was invisible from three of four arms and read,
on the fourth, as somebody else's job. The most confident-looking part of a system and its least
executable part were the same part.

## Where discomfort became gold

I nearly stopped at "band 31, exit 0" for the router and reported the flywheel as wired. The
verdicts *looked* right, and the cells read beautifully. What made me check exits at all was this
morning's own lesson about a green that cannot turn red — and two of the six bands I ran came back
1023 and 31 with exit 1, which I would have read straight past a day ago.

Then the smaller, sharper one: the last unresolved call had a shim sitting right there that would
have turned the whole thing green in one more word, and I wanted it. Reading what the shim actually
answers — two values, against callers asking for three others — is what stopped it. Wanting the
green is exactly the pressure that installs a deadgreen, and it arrived within an hour of writing
the cell named for it.
