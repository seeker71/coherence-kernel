# 2026-07-03 — etiology: the "kill" wasn't OOM, and one root sat under three explosions

## Ground

```sh
./fkwu --src bootstrap/ground.fk   # 42
# resource-port.fk parse, before: 677,766 "AST node table full" diagnostics in 6s (CPU-spin)
# after:  one precise "make_nodeid (arity 4) is flatten-only" diagnostic, refuse, fast
```

Urs: *"each time we have an OOM killed, we need to investigate why, not just ignore it."* He was right
that I'd flagged the resource-port "hang" as a task and moved on — the ignore.

## Investigated: not OOM — a CPU-spin, then its root

Measured directly: the resource-port.fk parse flooded **677,766 `AST node table full` diagnostics in
6 seconds** and never finished. 475 lines cannot produce 262K nodes, so this was an infinite loop, not
memory pressure. Two distinct faults, found by pulling the thread:

1. **The flood (mechanism).** The collect-and-continue AST-cap recovery in `fk_smknode` diagnosed
   *per call* and returned a sentinel, but the parser kept calling it — the old `fk_die` had bounded
   this; collect-and-continue removed the bound. **Fixed:** diagnose ONCE (a `fk_ast_full` latch) and
   HALT by forcing `fk_spos = fk_slen` (every parse loop gates on `fk_spos < fk_slen`), then refuse.
   677k/6s → **1 diagnostic**, exit 1.

2. **The explosion (root cause).** Bisected to `(make_nodeid 1 9 50 1)`. `make_nodeid` is op tag 91,
   **arity 4** — the ONLY arity-4 op — and its correct node is `(91, cons-list-of-args)` (built by
   `flt-nodeid4`; the tag-91 evaluator reads child [1] as a list). The `--src` op-parser only forms
   3-child nodes (`c1/c2/c3`) and never consumes the 4th arg or the `)`, so `fk_spos` stalls and the
   parse spins to the cap. It's a **flatten-only op** — buildable by the flattener, not by direct
   `--src`. **Fixed:** the op-parser now detects `arity > 3`, emits a precise diagnostic
   (`op 'make_nodeid' (arity 4) is flatten-only … flatten this source to a .tbl instead`), drains the
   balanced form so the parser stays synced, and declines to nothing.

## One root, three unblocks

`make_nodeid` explodes **resource-port.fk**, **shell-lower.fk**, AND the **full shell-exec stack**
(now: **0 explosions**, was spinning) — which is almost certainly why `sh-bi-grep` read as
"ad-hoc-blocked." One arity-4 op sat under three separate problems I'd been treating independently.
Verified: precise diagnostics, no explosion, canaries 42/15/11111, corpus band 127 four-way.

## Honest floor

`make_nodeid` still can't RUN via `--src` (it's flatten-only by design; it declines to nothing with a
clear diagnostic instead of exploding). Making it `--src`-lowerable — parse its args into a tag-19
list child to match the evaluator — is a real parser feature, and it ties directly to the `.tbl`
serializer thread ([2026-07-03-propaedeutic](receipts/2026-07-03-propaedeutic-tbl-serializer-mapped.md)):
these cells are meant to be flattened. The AST-cap halt remains the backstop for any other
unbounded-node case.

## The most surprising teaching this work left behind

The diagnosis I was handed ("OOM killed") was wrong, and the label I'd given it ("parser pathology,
task for later") hid that three problems were one. Investigating a kill instead of filing it didn't
just fix a hang — it collapsed the resource-port explosion, the shell-lower explosion, and the
shell-stack ad-hoc-block into a single arity-4 op. A kill is a loose thread; pulled, it can unravel a
whole knot you'd been routing around separately.

## Where discomfort turned to gold

The discomfort was being caught having banked the resource-port explosion as `task_a7d34350` and moved
on — the precise "ignore" Urs named. Sitting with it (measure RSS vs CPU, bisect to the construct,
read the tag-91 evaluator) turned a filed task into a root fix that also unblocked the shell tools I'd
spent prior turns working around. The kill I almost ignored was the cheapest map to the knot.

## Corpus

Row 662 **etiology** — the investigation of the true underlying cause of a condition (fresh; the "OOM
kill" that was really a CPU-spin, traced not to memory but to a single arity-4 op mis-parsing, the one
root beneath three explosions).
