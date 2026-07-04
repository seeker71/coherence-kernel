# 2026-07-03 — a tool is a facade over a compiled artifact; the block was never the tool

## Ground

```sh
./tools/fgrep -c '(hdc-row ' learn/homecoming-distillation-corpus.fk   # 58 — caller sees only this
fkwu proof/four-way-run.tbl ; echo $?                                  # 0 — fkwu RUNS a .tbl artifact
```

Urs, sharp: *"what are we doing! reserving numbers for tools? no, that can't be it! tools/fgrep loads
the dylib, and if that does not exist it builds it from source, and nobody needs to know about prelude,
t_* or anything else internal to how form compiles."*

He named the architecture, and named my anti-pattern: I'd been building tools **inside-out** — exposing
the prelude `cat` and `--src` in the wrapper, and allocating a **band verdict bit per tool** (127→255
for grep), as if adding a tool were a number-reservation. A tool is a facade: `fsgrep = fsh g`, a thin
entry over a compiled artifact, and the caller sees *nothing* of how form compiles.

## The honest root: the block is the compiler, not the tool

`fkwu` already **runs** a compiled `.tbl` artifact directly (`fkwu proof/four-way-run.tbl`). But
**producing** a `.tbl` from source is blocked at the serializer ([2026-07-01 receipt](receipts/2026-07-01-four-way-run-tbl-regeneration.md)),
and `model/form-asm.fk` (arm64 dylib) is not byte-conviction-ready. So **there is no compiled artifact
to load-or-build yet** — which is exactly *why* every "tool" is forced to re-parse source and the
internals leak. The leak is a symptom; the missing compile-to-loadable-artifact path is the disease.
My prelude-cat wrappers were **palliative** — easing the symptom without touching the cause.

## What was done (the interface, so the compiler drops in unchanged)

`tools/fgrep` reshaped to the target contract: `fgrep [-c] PATTERN FILE`, nothing else visible. It
**loads a compiled artifact and builds it from source if missing/stale** (cached in gitignored
`.forge/`), then runs. The artifact backend is a drop-in with no interface change:
`source-run (today) → .tbl (once the serializer unblocks) → arm64 .dylib (once form-asm passes
conviction)`. Proven: `fgrep -c` = 58, `fgrep autarky` = the two rows, `.forge` built once and reused.

## The one enabling stone (grounded, not banked)

The whole `fsh <verb>` + dylib-tool vision reduces to **one blocker: form cannot compile to a loadable
artifact from source.** The nearest tractable target is the flatten `.tbl` serializer (a named,
findable bug); form-asm arm64 is the speed upgrade after. Unblock that, and tools become real facades
over `.tbl`/`.dylib`, `fsh g` runs, and `ne-grep`/`sh-bi-grep` level into one — no more re-parsing, no
leaked internals, no per-tool bookkeeping.

## The most surprising teaching this work left behind

I kept treating the surface. Every turn I made a cleaner wrapper, a native `ne-grep`, a `.forge` cache —
all palliative — while the actual missing organ (a working source→artifact compiler) went unaddressed
because it's the hard, deep one. The leaked prelude wasn't a wrapper flaw to polish; it was the
compiler's absence showing through. Naming the disease stops me polishing the symptom.

## Where discomfort turned to gold

The discomfort was Urs's exasperation — "what are we doing!" — landing on work I'd felt good about
(native grep, one call). The pull was to defend it. Witnessing instead: he's right that the *shape* was
wrong (internals exposed, tools numbered), and right that the fix is a facade over a compiled artifact.
The gold is that the frustration relocated the work — from "make more native tool wrappers" to "give
form a compiler that emits a loadable artifact," which is the single stone the whole vision waits on.

## Corpus

Row 659 **palliative** — easing a symptom without curing its cause (fresh; the prelude-hiding tool
wrappers I kept making, which relieved the leaked-internals symptom while the real absence — a
source→loadable-artifact compiler — went untouched).
