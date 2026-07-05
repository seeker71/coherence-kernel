# 2026-07-05 — the purity gate was tautological; the signal now derives from the recipe

## "impure? why? what? how is this signal going back so that we close any issue?"

Urs caught it in three words. My crystallize-on-heat mechanism had a purity GATE
(`jd-crystallize?` only fires when `pure=1`), but `pure` was a **parameter I hand-passed** —
`1` for f, `0` to "test" the gate. Nothing derived it from the recipe. The gate was
**tautological**: it "checked" a value I supplied, so it decided nothing. A safety check fed
its own answer is theater.

## Why purity is the precondition

`jit-decision.fk:24`: "hot but IMPURE — never crystallized (JIT only pure functions)." A
crystallized native is **cached and reused**, so it is only valid if the result depends solely
on the args. An effectful recipe (host-io, mutation, randomness) would return a stale cached
value or skip its side effect. Purity is the correctness condition for caching — which is
exactly why gating on a *fabricated* purity signal was dangerous, not just empty.

## Closing the loop — derive, don't stipulate

Grounded on form-lower's own tag dispatch (`form-lower.fk` `lo-node`, lines 60-69): the pure
ops are `1 LIT · 2 ARG · 3 ADD · 4 SUB · 5 MUL · 6 COND · 8 DIV · 9 STREQ`; the host-io ops
`10 READ_FILE` and `11 WRITE_FILE` lower to `svc` syscalls (lines 208-227) — effectful. So
purity is *readable off the recipe*:

- `jc-pure?(prog)` walks the recipe rows and checks every op-tag against a **conservative**
  whitelist (any tag not known-pure → impure; an op added tomorrow is gated out until judged);
- `jc-call` now computes `(jc-pure? prog)` itself — the `pure` parameter is **gone**. The
  signal flows *from the recipe into the decision*.

`jit-crystallize-band` = **31**, with the fifth claim rewritten: `jc-pure?` derives `1` for f
and `0` for a bare `(read_file)` recipe (tag 10), and that effectful recipe **never crystallizes
even when hot — with no purity flag passed anywhere.** Direct witness: `pure(f)=1
pure(readfile)=0`.

## Closing

**Most surprising teaching**: a gate that takes its verdict as input is **tautological** — it
launders an assertion as a check. My purity gate *looked* like the JIT's safety interlock and
was, until this, decorative: I fed it the answer and it agreed. The only thing that makes a
gate real is that the signal **originates in the thing being judged** — here, the recipe's own
op-tags, grounded on form-lower's effectful/pure split.

**Where discomfort turned to gold**: "impure? why? what?" — three words that made me look at
where the signal actually came from, and it came from my own hand. The discomfort of finding a
safety check I'd fed its own answer is what forced grounding purity on the body's real effectful
tags, so the loop closes without me in it.

**Honest remaining**: purity is recomputed per call (it is a property of the recipe — cache it
beside the native). And the deeper signal is still open: the champion-challenger **health**
check — verify native == walker at runtime and melt a native that diverges — so trust in a
crystallized composite is also earned, not stipulated (`native-replace CAN-not-MUST`).
