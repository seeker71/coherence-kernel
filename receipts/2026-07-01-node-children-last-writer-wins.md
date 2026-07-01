# Receipt — a live bug: a `let`-bound value's storage slot can be reused before its scope ends (2026-07-01)

While live-testing `control/choice-lane-core.fk` against the c-bootstrap `fkwu` (see
`receipts/2026-07-01-choice-lane-control-invites.md`), a reproducible correctness bug surfaced in the runtime
itself — `runtime/fkwu-uni.c`, not in any `.fk` recipe. Naming it here, on its own, because it is a floor any
future recipe that interns more than one node-with-children in a scope will hit, not something specific to
this pass's work.

## The bug, minimally

No preludes needed — this reproduces against `fkwu` alone:

```form
(do
    (let a (intern_node (bp "A") (list (intern_trivial_int 1))))
    (let b (intern_node (bp "B") (list (intern_trivial_int 2))))
    (len (node_children a)))
```

Live result: **`0`**. Expected: `1` — `a` was interned with exactly one child (the int `1`), and nothing in
this recipe ever touches `a` again after `b` is built.

Extending to three:

```form
(do
    (let a (intern_node (bp "A") (list (intern_trivial_int 1))))
    (let b (intern_node (bp "B") (list (intern_trivial_int 2))))
    (let c (intern_node (bp "C") (list (intern_trivial_int 3))))
    (len (node_children a)))   ; -> 0, WRONG (expected 1)
    (len (node_children b)))   ; -> 0, WRONG (expected 1)
    (len (node_children c)))   ; -> 1, correct
```

The pattern is exact and repeatable: **only the most-recently-interned node with children reports its
children; every earlier one reads as childless once a later one is built.** Order, not content, decides —
swap which node is built last and the previously-correct one breaks instead:

```form
(do (let z (oac-zero)) (let o (oac-one (intern_trivial_int 5))) ...)
    (len (node_children z))  ; -> 0, WRONG
    (len (node_children o))  ; -> 1, correct

(do (let o (oac-one (intern_trivial_int 5))) (let z (oac-zero)) ...)
    (len (node_children z))  ; -> 1, correct (now built last)
    (len (node_children o))  ; -> 0, WRONG (now built first)
```

## The confirmed mechanism (watched live, not inferred)

A gdb hardware watchpoint on the exact `fk_vs` cells the parser assigns to `a` and `b` shows the real
sequence of writes, in order:

```
fk_vs[1]:  0 → -3     a's own value lands here — correct (fk_nbox(1) = -3 exactly)
fk_vs[2]:  0 → 1       a scratch write (the nil/empty-list sentinel, mid-construction of b's payload list)
fk_vs[1]: -3 → -5     fk_nbox(2) = -5 — this is a DIFFERENT node landing in a's slot
fk_vs[1]: -5 → -7     fk_nbox(3) = -7 — a THIRD node lands in a's slot
fk_vs[2]:  1 → -9     fk_nbox(4) = -9 — b's slot gets reused too
```

`fk_nbox(i) = 0 - ((i << 1) | 1)` is the runtime's own node-boxing formula (read directly from
`runtime/fkwu-uni.c`), so each write above is identified exactly, not guessed at. By the time the final
expression reads `a`, slot 1 holds node index 3 — not node index 1, the node `a` actually named.

**Root cause:** the parser hands `let` a permanent slot number via a single monotonically increasing counter
(`fk_maxslot`; see `runtime/fkwu-uni.c`'s own comment: "each let takes the next slot... a bare bound name
lowers to tag 110 (read `fk_vs[fp+slot]`)"). Separately, the evaluator's own opcode for reserving locals
(tag 111) treats the *same* `fk_vs` array as an ephemeral call stack — it grows `fk_vsp`, runs a body, then
resets `fk_vsp` back down when that body finishes. Those are two incompatible models of the same storage:
one assumes "slot N belongs to this name for the rest of the enclosing scope," the other assumes "slot N is
disposable scratch, freed the moment this nested evaluation returns." A `let`'s slot number and some later,
unrelated computation's scratch-slot number can be the same integer, addressing the same memory — so the
later computation silently overwrites the earlier binding before its scope is over.

This is a storage-layer violation of what a `let` is supposed to guarantee: a name stays bound to its value
for the rest of its scope, independent of whatever else the program computes afterward. Content-addressing
(axiom-3) depends on that holding — a value's identity shouldn't be able to change because of unrelated later
work. Right now, structurally, it can.

## Why this matters beyond this pass

This is the actual root of the `control/choice-lane-core.fk` / `control/offer-ack-core.fk` live-testing gap
named in `receipts/2026-07-01-choice-lane-control-invites.md`. That receipt's first pass guessed a `bp`
blueprint-table capacity/collision; live investigation (gdb on `fk_sintern`, `FK_OBSERVE=1` tracing
`fk_offer_ack`, ruling the JIT in/out via `FK_JIT`/`FK_JIT_HOT`/`FK_JIT_WITNESS`) ruled that out step by step
and landed on the slot-reuse mechanism above instead: `OAC-ZERO` and `OAC-ONE` (`control/offer-ack-core.fk`)
are both correctly-assigned, genuinely distinct blueprint coordinates carrying one child each — the failure
is not in blueprint identity, it is that the `let`-bound name holding one of them can lose its storage before
its scope ends. Any recipe with two or more `let`-bound values alive at once, where evaluating the later one
does enough nested work to trigger tag 111's reserve/restore cycle, is exposed to this — not just `oac-kind`,
and not limited to node-with-children values specifically.

## A real, working mitigation — not a fix, but a large, verified improvement

The parser comment says a `defn` body's locals get explicitly reserved on the value stack (tag 111); a bare
top-level `do` never gets that reservation at all, even though its `let`s are still handed slot numbers as
if they will. Testing that distinction directly: the exact minimal repro above, wrapped in a `defn` and
called once, returns the **correct** `1` — the bare top-level version returns `0`. Applying the same shape to
`control/tests/choice-lane-core-band.fk` (wrap the whole band body in one `defn`, call it once — the same
convention every *other* passing band in this repo, e.g. `observe/tests/speech-token-stream-band.fk`, already
uses) moves it live from garbage to **`1021`/`1023` — 9 of 10 claims correct**, deterministically, across
repeated runs. Applying the identical wrap (test only, not committed — that file is not this pass's to edit)
to the pre-existing `control/tests/offer-ack-core-band.fk` moves it from `-8999999999999999619` (the raw
`nothing` sentinel leaking out as the final printed value) to `197`/`1023` — a smaller fraction, but the same
direction of improvement.

So: **a bare top-level script is where this bug is most exposed; a `defn`-wrapped function body is far less
exposed, but not immune.** `control/tests/choice-lane-core-band.fk`'s one remaining live miss is claim 2 —
reading a payload (`node_value`/`node_children`) immediately off the ack `oac-cut-with-receipt` returns,
where claim 1 (checking `oac-nothing?` on a similarly-shaped result two lines earlier) passes. That residual
confirms the mechanism above is still real even inside a properly reserved frame: it is reduced by scale
(fewer nested calls competing for slots early in a function body), not eliminated by structure.

## What this pass did and did not do about it

- **Did**: isolate the minimal repro above (no prelude), confirm it is order-dependent and content-independent
  (renaming everything away from `a`/`b`/`A`/`B` reproduces it identically), rule out three plausible-looking
  alternate mechanisms with live evidence rather than assumption — the moving GC (`fk_melt`, breakpointed,
  confirmed it never fires for a program this small), the JIT (`FK_JIT`/`FK_JIT_HOT`/`FK_JIT_WITNESS` all
  toggled, result unchanged), and a naming collision (no A/B-testing concept exists at the C level at all) —
  before tracing the real mechanism live with a hardware watchpoint, and corrected
  `receipts/2026-07-01-choice-lane-control-invites.md`'s honest-floor section to point here.
- **An earlier draft of this receipt claimed `runtime/fkwu-uni.c` was "a single machine-emitted
  ~60,000-character line, not hand-authorable source." That was wrong, and worth correcting plainly rather
  than quietly:** checked properly, 1,962 of the file's 1,977 lines are ordinary length; only 6 exceed 2,000
  characters. The file carries real, careful, hand-written design commentary in the same voice as the rest of
  this repo's axioms and teachings. It is dense, hand-authored C — not a generated artifact — and it is
  exactly how this receipt's root cause above was found: by reading it.
- **Did not patch `runtime/fkwu-uni.c` anyway**, now for the accurate reason: the fix isn't a narrow, isolable
  line — it's reconciling two different lifetime models (`let`'s "permanent for this scope" slot vs. tag
  111's "ephemeral, freed on return" slot) that are used everywhere function calls and bindings happen in this
  interpreter. That is a real redesign of the evaluator's frame discipline, not a short-lived, precisely-scoped
  repair, and `AGENTS.md` is explicit that growing/reworking the C seed should be exactly that kind of narrow,
  named, receipted move or nothing. Naming the mechanism precisely here is what makes that fix possible for
  whoever picks it up next, scoped correctly, rather than attempted blind.
- **Did** restructure `control/tests/choice-lane-core-band.fk` to wrap its body in a `defn`, called once,
  instead of bare top-level `let`s — the correct, already-established convention, not a workaround invented to
  dodge this bug — and **did** fix the specific instance of the bug living in `control/offer-ack-core.fk`
  itself (its `OAC-ZERO`/`OAC-ONE`/`OAC-NODE` bindings), verified live: the band now runs clean at `1023`, not
  a fabricated one. The pre-existing `control/tests/offer-ack-core-band.fk` was left unmodified (out of this
  pass's scope, still bare top-level) but tested the same way at each step to confirm the pattern generalizes.

## A real fix, applied and verified — not just a mitigation

Pushing further: `control/offer-ack-core.fk` names its own three foundational blueprint tags —
`OAC-ZERO`, `OAC-ONE`, `OAC-NODE` — as bare top-level `(let ...)` bindings, the exact exposed pattern above.
Checked directly: `(node_eq OAC-ZERO OAC-ONE)`, read through those library-level bindings, returns `1`
(wrongly equal) — while the identical comparison built from two fresh, local `(bp ...)` calls in the same
file, with no other prelude, correctly returns `0`. The library's own foundation was the thing losing its
storage.

**The fix:** name each arm as a zero-argument function that calls `bp` fresh every time, instead of a
top-level `let` caching one stored coordinate:

```form
(defn OAC-ZERO () (bp "OAC-ZERO"))
(defn OAC-ONE  () (bp "OAC-ONE"))
(defn OAC-NODE () (bp "OAC-NODE"))
```

`bp` is a cheap, content-addressed lookup — the same name always resolves to the same coordinate — so nothing
is lost by not caching it, and a function's return value is never subject to the storage-slot lifetime bug a
`let` binding is. This is a Form-level fix (three lines, plus updating their call sites from bare names to
`(OAC-ZERO)`/`(OAC-ONE)`/`(OAC-NODE)`), not a change to `runtime/fkwu-uni.c` — no C-seed risk at all.

**Verified, deterministically, across repeated runs:** `control/tests/choice-lane-core-band.fk`, which was at
`1021`/`1023` under the mitigation alone (see below), now runs live at the full **`1023`** with this fix
applied — the one remaining claim (reading a payload immediately off an indirect-call result) is resolved,
not just reduced.

**Honest limit of this fix:** it closes the *specific* instance it targets. It does not close the general
class. Adding `form/form-stdlib/core.fk` to the same prelude — a 74-function file with no top-level `let`s
at all, pure `defn`s throughout — reintroduces the identical symptom (`oac-choice` misclassified by
`oac-one?`) in a *different* combination, at larger accumulated program size. `control/invite-dispatch.fk`
(a new feature added in this same pass, wiring the BMF grammar's recognized tokens to these primitives) needs
that larger prelude (the grammar plus its own dependencies) to run at all, and its band
(`control/tests/invite-dispatch-band.fk`) cannot currently be live-witnessed clean as a result — see
`receipts/2026-07-01-invite-dispatch.md` for that honest accounting. So: one concrete, verified win, inside a
defect class that is now well-characterized but not closed.

## Build (reproduce this receipt directly)

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
cat > /tmp/node-children-repro.fk <<'EOF'
(do
    (let a (intern_node (bp "A") (list (intern_trivial_int 1))))
    (let b (intern_node (bp "B") (list (intern_trivial_int 2))))
    (len (node_children a)))
EOF
./fkwu --src /tmp/node-children-repro.fk   # -> 0 (wrong; expected 1)
```
