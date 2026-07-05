# 2026-07-05 — the AST-cap "wall" was a parser bug; the cap raise was iatrogenic

## "assume the JIT is fully functional; if you find an issue, deal with THAT issue"

Urs asked me to update the docs that had led me the wrong way, to assume the JIT works,
and — the load-bearing clause — that if I hit an issue, to **deal with that issue instead
of assuming something is fundamentally missing.** Following that literally is what cracked
the whole session's confusion.

## What was actually wrong

The `map` and `multiarg` `form-lower` bands returned 0. I had read that as "the JIT can't
lower list-ops / multi-arg." It wasn't. Grounding with a new `FK_NODES=1` probe (prints the
true parse `node_count` at parse-done):

- The whole compiler — `core + form-asm + form-lower` — is **7,474 nodes**. Healthy.
- The `map` band (58 lines) and `multiarg` band (83 lines) **each blew past 1,048,576 nodes
  on their own.** A 58-line file of tiny list literals cannot legitimately be a million nodes.

Bisecting to a minimal repro pinned the exact trigger: a `(let …)` in a `do` **followed by**
a `(defn …)` of **arity ≥ 2**. Cause, in `runtime/fkwu-uni.c`: `fk_sparse`'s defn arm read
only ONE parameter, then expected the closing `)`. Any arity-≥2 defn in value/mid-do
position left the cursor parked on the 2nd param; with a `let`'s binding already on the
stack, the paren+scope desync drove the collect-and-continue error recovery into a
sentinel-minting **spin** that only halted at `FK_AST_NODE_CAP`.

A second, older bug sat behind it: the same arm never stored `fk_fn[idx]`, so a mid-do defn
(reached whenever a `do` doesn't open with defns — e.g. after a `let`) registered its NAME
via the prescan but **lost its BODY**. Calls then returned the caller's arg. Pre-existing:
reproduced on the git-HEAD binary for the 1-param case too.

## The fix (root, not vessel)

`fk_sparse`'s defn arm now reads ALL params into slots 0..k-1 and stores `fk_fnar[idx]` +
`fk_fn[idx]`, mirroring `fk_parse_top`. Grounded results on the rebuilt binary:

- `map` band **31/31**, `multiarg` band **127/127** (0 errors) — the JIT had list + multi-arg
  lowering all along.
- float **15**, streq **31**, cond **31** unchanged; recipe42 **42**, homecoming **127**,
  rag-retrieve **31**, rag-embed **15**, rag-ask-grounded **7** — byte-identical to git HEAD.
- The sparse-TF-IDF RAG parses to **2,832 nodes** (was cited as needing the 1M cap). Its
  self-proof still returns 111.
- `FK_AST_NODE_CAP` **reverted 1048576 → 262144** (~12× the largest committed program,
  `bml.fk` at ~20.5K nodes). The comment now tells the true story.

The bug lives only in `runtime/fkwu-uni.c`. The bootstrap seed
(`form/form-stdlib/bootstrap/fkwu-uni.c`, validate.sh's fourth arm) has **no parser** — it
walks pre-flattened tables — so no sync is needed and the four-way harness is unaffected.

## Closing

**Most surprising teaching**: two prior same-day "fixes" — raising `FK_AST_NODE_CAP`
65536→262144→1048576, and the "use top-level globals" workaround — were both **dodging the
same parser bug**, and corpus row 723 had enshrined the first as "capacity, not wall." The
raise didn't just fail to cure; it **buried** the bug (moved the ceiling the spin ran into)
and spent 24MB to hide it. The remedy became the fault's new home. The word for that is
**iatrogenic** (corpus row 726). Every "raise the cap when the AST fills" reflex in this
repo's history deserves the `FK_NODES=1` grep before the edit.

**Where discomfort turned to gold**: the instruction "deal with THAT issue instead of
assuming something is fundamentally missing" is a rebuke and a trust at once — it named that
my habit all session was to declare a capability missing rather than look for my own bug. Sitting
with that (the JIT was never the thing lacking; my grounding was) is exactly what turned
"the JIT can't do lists" into a 15-line parser fix that made the bands green. The tell I'd
been missing was never in the JIT — it was a node count I hadn't thought to print.
