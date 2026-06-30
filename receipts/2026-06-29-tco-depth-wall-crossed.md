# Receipt — the depth wall crossed: tail-call elimination lands in fkwu (2026-06-29)

**The walk:** finish the depth-wall lane the prior receipt scoped — make deep tail-recursive Form
recipes run in O(1) C-stack instead of overflowing the `fk_walk` tree-walker at ~60-120 frames deep.

## Before (measured on clean `runtime/fkwu-uni.c` at HEAD, no TCO)

`fk_walk` recurses on the C stack (~17 KB/frame). Both tail and non-tail recursion overflow:

```
cd(120) -> 7      cd(140) -> (overflow, empty)     1-arg tail wall ~120-140
acc(60) -> 1830   acc(70) -> (overflow, empty)     2-arg tail wall ~60-70
fac(60) -> ...    fac(70) -> (overflow)            non-tail wall (unchanged by TCO)
```

## After (TCO via `fk_walk_body` trampoline)

```
acc(1000000 0) -> 500000500000     deep 2-arg tail, O(1) C-stack
cd(1000000)    -> 7                deep 1-arg tail, O(1) C-stack
```

## The change — one trampoline, every call path routed

Added `static long long fk_walk_body(long long i, long long fp)` immediately before `fk_walk`
(with a forward decl of `fk_walk` so the two can mutually recurse). `fk_walk_body` evaluates a
function BODY by LOOPING through tail position instead of recursing:

- **tag 6 (if):** eval cond via `fk_walk`; `i = chosen branch`; `continue`. Exact truth test mirrors
  `fk_walk`: `if (cond==0) i=else(child3); else i=then(child2);`.
- **tag 69 (do):** `fk_walk(first); i = rest; continue`.
- **tag 109 (let):** `fk_vs[fp + (idx>>1)] = fk_walk(child2); i = child3; continue`.
- **tag 111 (reserve):** extend `fk_vsp` to `fp+1+k` (zero-init new slots); `i = body; continue`.
- **tail CALLS — reuse the frame, then `continue`:** evaluate args in the current frame, write the
  new args down into `fk_vs[fp..]`, set `fk_vsp = fp + nargs`, set `i = fk_fn[callee]`. This is what
  makes tail recursion O(1).
- **any other tag:** `return fk_walk(i, fp)` (non-control tail expr — a single non-looping descent).

### Every function-call site routed (the prior receipt's blocker)

The prior attempt routed only the obvious tag-12 handler; the active recursion bypassed it through a
*different* path, so `fk_walk_body` was never invoked and the wall barely moved. This pass routes
ALL six call paths inside `fk_walk` to `fk_walk_body` (caller restores `fk_vsp` to the frame base
after return, since `fk_walk_body` does not), and `fk_walk_body` itself loops every tail call:

- **tag 7** — self-call `fk_fn[0]`, 1 arg
- **tag 12** — direct call, 1 arg
- **tag 240** — direct call, 2 args
- **tag 241** — direct call, n args (list of tag-242 cells)
- **tag 244** — indirect call through a computed fn-value head
- **tag 44** — the `f44` argument **combination/currying** path (`carg44 = comb44`, pops 2 / pushes 1).
  This was the path the prior attempt missed. The trampoline reproduces the combination semantics
  exactly, then reuses the frame with the combined arg.

`fk_offer_ack` is a no-op unless observe-mode (it only `printf`s under `fk_observe_on()`, else returns
its value unchanged), so looping past it on internal tail-calls is safe; it is preserved at the outer
`fk_walk` call boundary where each routed site still `return fk_offer_ack(...)`.

**Invocation proven:** a temporary one-shot `WB!` marker at `fk_walk_body` entry confirmed the
trampoline actually engages on a recursive run (`(cd 5)` printed `WB!` once); the marker was removed
before shipping. The shipped binary prints no marker and returns the correct values.

## Validation — all green, zero regressions

New capability (clean, non-instrumented binary):

```
acc(1000000 0) -> 500000500000   cd(1000000) -> 7
acc(50)        -> 1275           fac(5)      -> 120   (add 0.5 0.25) -> 0.75
```

Regression (the named witnesses + table + GPU):

```
native-vs-rented-check  -> 11111   surprise-receipt-check -> 11111
confidence-earned-check -> 11111   numeric table          -> 42
GPU fptx-matvec         -> AGREEMENT: 3/3  ALL-BIT-EXACT=true
```

Differential sweep — every `.fk` in observe/ learn/ agent/ cognition/ control/ flatten/ form-cli/
gate/ grammars/ model/ proof/ routers/ substrate/ surface/ presence/ ingest/ io/ run on BOTH the
HEAD binary and the TCO binary, output compared byte-for-byte:

```
CHECK-recipe witnesses (the all-1s *-check defns):  PASS=100  FAIL=0
standalone recipe runs:                              PASS=243  FAIL=0  TIMEOUT=10
```

The 10 timeouts are genuinely long-running / network / REPL recipes (RAG ask+heal, the form-cli
REPL, ontology + grammar loaders, resource-port, surface/core) that time out identically on both
binaries — unrelated to TCO. **Zero behavioral diffs anywhere.**

## Note for the next cell

Some recipes regenerate `runtime/fkwu-optable.h` as a side-effect of being run (see
`2026-06-29-optable-regen-is-form-no-bash.md`). During the standalone sweep one such recipe truncated
the optable, which silently breaks `add`/`sub`/`le`/`mul` for any binary rebuilt afterward. The sweep
scripts restore `runtime/fkwu-optable.h` from a known-good copy after every recipe run; the committed
optable is unchanged from HEAD. If a rebuilt fkwu suddenly returns `0`/`nothing` for arithmetic,
check `git diff runtime/fkwu-optable.h` first.

## The lane forward

Non-tail recursion (`fac`) is the separate residual the prior receipt named: it genuinely needs depth
(a heap-stack evaluator or native lowering). Most deep recipes are tail-recursive, so this TCO is the
high-value first move. North star remains `form-asm` lowering — the same recursion->loop transform in
the SHIPPED native path (recipe -> asm bytes, tiny frames) rather than the bootstrap walker.
