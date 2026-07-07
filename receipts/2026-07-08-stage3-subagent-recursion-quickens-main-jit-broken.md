# 2026-07-08 — Stage 3 sub-agent: recursion quickens to native; main's JIT is silently broken on arm64

A fresh-context sub-agent was spawned for Stage 3 (bridge main's transparent dispatch to
form-lower). It completed, and the honest accounting matters more than the headline.

## What the sub-agent actually did (verified, not trusted)

**It was spawned on the wrong base.** Its worktree branched from the **main lineage**
(`fffb9d61`, merge-base `f5d28cc2` / corpus row 720), NOT `deploy-merge`. So it could not see my
Stage 0–2 work or the two integration receipts, and it **re-did** the arm64 door + `nat_run` +
`native_call_test` from scratch on main's base (its commit `d45b6e5b`, branch
`worktree-agent-a8b43e707c7ade2a4`). That code is redundant with `deploy-merge`'s Stages 0–2 and
its corpus "row 730" collides with the committed "exceptionless". **Do not merge that branch.**

**It did NOT complete the real Stage 3.** The transparent auto-dispatch bridge (main's `--src`
heat path calling form-lower in-C, plus an AST→prog builder for the tag-7 CALL recursion shape) is
still pending — its own report says so.

## What it delivered that is genuinely new (and I verified)

1. **Recursion quickens to native.** form-lower lowers a *recursive* fib (CALL = tag 7, via
   `lo-rec-fn` → a 68-byte self-recursive arm64 image with a real stack frame and `bl` calls); run
   through `nat_run`, it is **extensionally equal to the walker** at fib(1,5,8,10)=1,8,34,89. I
   confirmed this band passes **15** on `deploy-merge`'s own runtime and **salvaged it** as
   `form/form-stdlib/tests/jit-native-run-band.fk` — a strict advance over my prior non-recursive
   proof (arithmetic only). Deep recursion runs entirely in native asm on Apple Silicon.

2. **main's C self-JIT is silently broken on Apple Silicon — grounded, not asserted.** With its
   JIT toggle on, main lowers a linear recursion and prints
   `[jit] fn1 crystallized in-process: 413 bytes … (native dispatch)` — then **returns `nothing`
   instead of the answer.** The x86 bytes install into nothing (`fk_nat_install`'s `#else return
   0`) and the call falls to `fk_native_call_args`'s `#else return fk_nothing`. main emits x86 for
   an arm64 machine. Its flagship transparent JIT announces success while producing no value. This
   is the strongest possible justification for the integration: main's JIT *needs* the Form/arm64
   lineage to actually run on the machine it ships on.

## Closing

**Most surprising teaching** (the sub-agent's, and it is the session's whole spine in one line): a
mechanism that *reports* success is not a witness — the number it returns is. main's JIT printed
"native dispatch" in green and returned nothing; only measuring the returned value exposed it. The
same discipline caught the sub-agent itself: its report claimed "Stage 3 done," and only building
its runtime and reading its own "remaining" showed the real Stage 3 was still pending. Trust the
value, never the announcement — of a JIT, of a sub-agent, of oneself.

**Where discomfort turned to gold**: a sub-agent on the wrong base, duplicating a day's work,
first read as wasted effort. But its *witness* — recursion running native, and main's JIT caught
lying — was the real payload, and both survived verification onto `deploy-merge` while its
redundant code stayed behind. The value of a parallel run wasn't its code; it was what it saw.

**Frontier word (the sub-agent's distillation), row 736: "quicken"** — the moment a walked
description becomes the machine's own motion. crystallize is the cache; reify is the opposite
motion; quicken is the coming-to-life. The real quickening is the value the native returns, not the
line the JIT prints.

## Remaining
The actual Stage 3 (transparent in-`--src` auto-dispatch wiring main's heat path to form-lower) is
open — but the two halves it joins are both proven: form-lower emits correct recursive arm64
(this band), and the arm64 door runs it. Then Stages 4–7 and the committable git merge.
