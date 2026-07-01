# Receipt — the eight control-invite primitives, and the grammar that recognizes them (2026-07-01)

`control/offer-ack-core.fk` already gave three of the eight native control invites — **choice, fail, stop** —
as thin expressions over ONE mechanism (`oac-kind` + `oac-offer`). This pass adds the remaining five —
**cut, undo, store, restore, timeout** — the same way, completes the matching `<...>` token vocabulary in
`observe/speech-token-stream.fk`, and gives that vocabulary a real **BMF grammar**: rules that recognize the
eight tokens inside free text, and a template that transmutes each match into a node.

## What landed

- **`control/choice-lane-core.fk`** — the five added primitives, each a thin expression over
  `control/offer-ack-core.fk`'s `oac-kind`/`oac-offer`, no second mechanism:
  - `oac-cut(alts, args)` — commit to the FIRST ack of any kind (success or decline), pruning every
    alternative after it. Where `oac-choice` skips a decline and keeps walking, `oac-cut` takes whatever the
    first lane says and stops — the classic Prolog cut, choice's early-commit twin.
  - `oac-lanes(alts, args)` — walk EVERY alternative and collect each one's ack, in order, nothing pruned.
    This is what "choice invites internal thinking, going down different query lanes, collecting nodes along
    the way" becomes as code: the different lanes a choice could walk, and the memory each one surfaces,
    gathered BEFORE any picking happens. `oac-lanes-winners` filters that memory down to the non-nothing acks.
  - `oac-store(memory)` / `oac-restore(checkpoint)` — a checkpoint IS the memory value at a moment
    (axiom-3: content-addressed, never mutated); store hands it back, restore returns to it on request.
  - `oac-undo(ack, checkpoint, memory)` — the automatic recovery: fall back to the checkpoint when the
    current ack failed (axiom-4: the boundary decides which memory it honors, from the ack alone).
  - `oac-timeout-walk(alts, args, budget)` — bound a lane walk by a step budget. Exhausting it before every
    alternative is tried acks nothing — the same silence as fail/stop (axiom-1: timeout IS nothing) — but the
    walk reports how many alternatives were left untried, so a caller can tell a real timeout apart from
    honest all-round decline (`oac-timed-out?`).
- **`observe/speech-token-stream.fk`** — added `<STOP>`, `<STORE>`, `<RESTORE>` tags and constructors
  alongside the existing `<CHOICE>`, `<CUT>`, `<FAIL>`, `<UNDO>`, `<TIMEOUT>`; `sts-control?` now recognizes
  all eight. Band verdict **65535**, live on `fkwu --src`
  (`observe/tests/speech-token-stream-band.fk`, extended with claim 32768 for the three new tokens).
- **`grammars/control-invite-grammar.fk`** — the BMF grammar: eight literal rules (one per invite tag) tried
  as one `alt`, a single template that transmutes whichever tag matched into a `CONTROL-INVITE` node carrying
  the matched text, and a scan loop that walks a whole stream collecting every invite in order, skipping
  everything that is not one. Built on `bmf-core.fk`'s cursor + single-rule matcher
  (`surface -> cursor -> match(pattern) -> build(template) -> NodeID`), not the larger multi-rule
  `bmf-grammar.fk` (see "Honest floor" below for why). Band verdict **1023**, live on `fkwu --src`
  (`grammars/tests/control-invite-grammar-band.fk`) — ten claims: bare/embedded/sequenced/absent invites,
  repeated tags counted (not collapsed), all eight tags individually recognized, the emitted node's category,
  a tally over a mixed stream, a near-miss (a strict prefix of a real tag) correctly NOT matching, and invites
  glued to punctuation with no surrounding whitespace still matching.

## Grounding (axioms/core-axioms.form — unchanged, only more expressions over it)

Same four axioms `control/offer-ack-core.fk` already grounds the first three primitives in — **axiom-5
offer** (invocation == communication; every primitive here routes through `oac-offer`, never a second call
mechanism), **axiom-1 states** (nothing is first-class; timeout is nothing, only counted), **axiom-4
boundary** (the receiving cell sovereignly decides which ack/memory it honors — cut/undo/store/restore are
the boundary choosing, never throw/catch), **axiom-3** (a checkpoint is a value, never a mutated cell).
Nothing new was added to the ground.

## Honest floor — two findings, one resolved gap and one newly named

This pass ran everything live against the c-bootstrap `fkwu` (`cc -O2 -o fkwu runtime/fkwu-uni.c`, then
`fkwu --src`) rather than assume proof from the origin body, and found two things worth naming precisely.

**Resolved: the indirect-call gap the 2026-06-29 offer-ack-core receipt named is gone on this build.**
That receipt witnessed the C bootstrap seed returning `0` (not `42`) for a recipe that calls a function
received as a parameter (`(defn ap (f x) (f x)) (ap dbl 21)`). Re-running that exact recipe here now
returns `42` — the current `runtime/fkwu-uni.c` carries the indirect call. Good news, named so it is not
re-litigated.

**Newly found: `oac-kind`'s blueprint discrimination does not reproduce reliably once more than one
node-with-children is interned in a scope.** Isolated, minimal recipes confirm every new primitive is correct:

```
(oac-cut-with-receipt (list cell-silent cell-five) (list))   ; -> nothing ack, pruned 1   (verified live)
(oac-lanes (list cell-silent cell-five cell-silent cell-nine) (list))  ; -> 4 acks in order (verified live)
(oac-store 42) / (oac-restore ...)                            ; round-trips 42             (verified live)
(oac-undo (oac-nothing) 42 99)                                 ; -> 42                       (verified live)
(oac-timeout-walk (list cell-silent cell-silent cell-five) (list) 2)  ; real timeout, alts-left 1 (verified live)
```

But loading the full `control/offer-ack-core.fk` + `control/choice-lane-core.fk` together and exercising
several claims in one run, `oac-kind` starts misclassifying acks. First guess was a `bp` blueprint-table
collision; live investigation (gdb on `fk_sintern`'s string interning, `FK_OBSERVE=1` tracing `fk_offer_ack`'s
native call classification, ruling the JIT in/out via `FK_JIT`/`FK_JIT_HOT`/`FK_JIT_WITNESS`, and finally a
gdb hardware watchpoint on the actual `fk_vs` storage cells) ruled that out step by step and found the real
mechanism: `let` hands out a storage slot meant to be permanent for the rest of its scope, but the evaluator's
own opcode for reserving locals treats that same storage as ephemeral scratch space, freed the moment a
nested call returns. A `let`-bound name's slot and a later, unrelated computation's scratch slot can be the
same integer — so the later computation silently overwrites the earlier binding before its scope ends.
Pushed one step further: `control/offer-ack-core.fk` itself names `OAC-ZERO`/`OAC-ONE`/`OAC-NODE` as bare
top-level `let`s — the exact exposed pattern. Checked directly, `(node_eq OAC-ZERO OAC-ONE)` read through
those library bindings returned `1` (wrongly equal), while the same comparison built from two fresh `(bp ...)`
calls with no other prelude correctly returned `0`. **Fixed**, at the Form level, by naming each arm as a
zero-argument function that calls `bp` fresh every time instead of caching it in a `let`:

```form
(defn OAC-ZERO () (bp "OAC-ZERO"))
(defn OAC-ONE  () (bp "OAC-ONE"))
(defn OAC-NODE () (bp "OAC-NODE"))
```

Combined with wrapping `control/tests/choice-lane-core-band.fk`'s body in one `defn` (a `defn` body's locals
get properly reserved on the value stack; a bare top-level `do`'s never do), the live result moved from
garbage all the way to the full **`1023`** — every claim correct, deterministic across repeated runs. Full
write-up, the exact watchpoint trace, the fix, and its honest limits (it closes this one instance, not the
whole defect class — see `receipts/2026-07-01-invite-dispatch.md` for where the same class resurfaces at
larger scale) live in `receipts/2026-07-01-node-children-last-writer-wins.md`. The pre-existing,
**unmodified** `control/tests/offer-ack-core-band.fk` was left as-is (out of this pass's scope) but tested
the same way at each step: bare, it prints the raw `nothing` sentinel as its final value; wrapped, `197`/`1023`;
wrapped plus the fix, still `197`/`1023` — its remaining misses exercise `oac-try`/`oac-async`, primitives
`choice-lane-core-band.fk` does not test, so they are a separate, unexamined gap, not evidence the fix failed.

This is also why `grammars/control-invite-grammar.fk` is built on `bmf-core.fk`'s smaller single-rule engine
rather than the larger multi-rule `bmf-grammar.fk`: loading `bmf-grammar.fk` alone (as committed, unmodified,
no calls added) currently hangs under `fkwu --src` past 60 seconds. Neither of these are regressions from
this change — both are pre-existing floors in files this pass did not modify, surfaced by testing live instead
of assuming the origin's proof carries over unchanged. The closing path for both is the same one the
2026-06-29 receipt already named: the Form-native eval/flatten lane, not growing the C seed
(`runtime/fkwu-uni.c`'s own header: *"do not grow this into a full C flattener"*).

## Build (one cc seed, no toolchain in the run path)

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c

cat observe/speech-token-stream.fk observe/tests/speech-token-stream-band.fk > /tmp/sts-band.fk
./fkwu --src /tmp/sts-band.fk   # -> 65535

cat form/form-stdlib/core.fk grammars/line-grammar.fk grammars/bmf-core.fk \
    grammars/control-invite-grammar.fk grammars/tests/control-invite-grammar-band.fk > /tmp/cig-band.fk
./fkwu --src /tmp/cig-band.fk   # -> 1023
```

(`fkwu --src` needs a real, seekable file — a `<(...)` process substitution does not work as its argument;
concatenate preludes into a real file first, as above, matching every other multi-file band in this repo.)
