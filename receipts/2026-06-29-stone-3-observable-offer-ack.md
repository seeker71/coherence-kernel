# Receipt — stone 3: the reducer's CALL is an OFFER that produces an OBSERVABLE ack (2026-06-29)

**Folding 2b-ii + 3 of the eval-as-offer/ack arc.** Two coupled increments in the TCB
(`runtime/fkwu-uni.c`), hard-gated, zero regression, all proven on `fkwu --src` (no Go):

1. **CALL = OFFER (2b-ii).** A reducer call whose offered callee does not resolve now
   acknowledges the canonical first-class `nothing` AUTOMATICALLY (axiom-5: *an offer a
   cell can't answer acks nothing*) — not a literal 0, not a crash. So `fail` becomes
   what any non-resolving offer YIELDS, and `oac-choice` (first non-nothing) and
   `oac-try` (recover from nothing) now work over **real reducer calls**, not only an
   explicit `(nothing)`.
2. **OBSERVE EVERY ACK (3).** A thin, toggle-able observe hook lifts the existing
   `fk_arms` tag-counter to the offer/ack altitude: each offer (callee + arg-count) and
   its four-arm ack-kind `{nothing|0|1|node}` is emitted as a witnessable trace the
   observe organ reads — the live feed `observe/runtime-witness.fk` named as the one
   piece "that depends on the runtime emitting it." Off by default, zero overhead/output.

`eval(cell) ≡ offer → observable-ack` is complete: every call is an offer, every offer's
ack is one of the four arms, and every ack is witnessable.

## What changed in the TCB (56 insertions, 7 deletions)

- **Unknown-head call → `nothing` (was `0`).** The parser's call position (inside `(`,
  head matched no op / no rewrite / no user fn) emitted `fk_smklit(0)`; it now emits
  `fk_smknode(137, 0, 0, 0)` — the tag-137 node the reducer reads as `fk_nothing`. The
  balanced-form skip is unchanged (parser stays aligned for what follows).
- **`fk_ack_kind(v)`** — the ONE four-arm classifier, axiom-1 order (nothing first):
  `nothing` (the canonical sentinel) | `0` (the zero state) | `node` (a content-addressed
  cell back / counter-offer) | `1` (every other affirmative result / payload). One place
  reads the four arms off a raw value; no per-call-site ack if-chain.
- **`fk_offer_ack(callee, argn, v)`** — wraps each call's return; when `FK_OBSERVE` is on,
  emits `offer fn<callee> args=<n> ack=<arm>`; returns `v` unchanged. Wired into all four
  call tags: `12` (1-arg), `240` (2-arg), `241` (general-arity), `44` (indirect/tag-44).
- **`fk_observe_on()`** — reads env `FK_OBSERVE` once (`-1` unread → 0/1). Off (the
  default and `FK_OBSERVE=0`) → a single predicted-false test per call, no output, no
  alloc, no formatting.

No new hardcoded value-op `if` chains: the four arms are read by the ONE classifier; the
trace is one generic emit; the call sites are the existing four tags, untouched in logic.

## The line: intentional-nothing vs a masked bug

Only a **non-resolving OFFER** acks nothing — a well-formed `(head args..)` whose head is
not a resolvable callee (no op/rewrite/fn). That is fail/decline (axiom-5), and it is
exactly what `choice`/`try` recover over.

A genuine bug does **not** silently become nothing:
- A bare unbound **symbol** is a value position, not an offer — it stays `0` (it never
  claims to be a call, so it never acks nothing). `(nothing? unboundvar)` → `0`.
- A real **op** with a wrong shape computes through its own op path, never through the
  call-decline. `(add 1 2)` → `3`.
- We reach the decline only from a CALL position after the head failed to resolve as a
  callee — a well-formed offer to a non-answering cell. We still consume the whole
  balanced form so later defns are not corrupted (the old first-`)` skip bug).
- A guard-fail/decline expressed as `(nothing)` in a callee body already propagated; this
  increment makes the *non-resolving callee* itself the nothing-ack, so the recovery
  combinators close over real calls.

The drawn line is structural and pre-runtime: it is the difference between *an offer to a
cell that cannot answer* (→ nothing) and *a value/op that is malformed* (surfaces through
its own path). A masked bug would require a non-resolving call to be mistaken for a
deliberate decline — but a recovering caller (`oac-try`/`safe`) is exactly the cell
sovereignly deciding to treat that silence as recoverable; an un-recovering caller still
sees `nothing` propagate, never a forged `0`.

## Hard gate — all on `fkwu --src`, no Go (`cc -O2 -o /tmp/fkwu runtime/fkwu-uni.c`)

```
NON-RESOLVING CALL -> NOTHING
  (nothing? (a-call-that-cannot-resolve 1 2 3))        -> 1   (base: 0)
  (nothing? unboundvar)        [value pos, not offer]  -> 0   (unchanged — masked-bug line)
  (add 1 2)                    [real op, own path]     -> 3   (unchanged)

CHOICE / TRY OVER REAL CALLS
  choice3((fails-one 1), (fails-two 2 3), (succeed))   -> 5   (base: 0)  first non-nothing
  try1((declines 7), handler)                          -> 99  (base: 0)  recover from nothing
  try1((ok), handler)          [success passes through]-> 42              handler unused
  safe((broken 9), 77)         [nested, recoverable]   -> 77

OBSERVE (FK_OBSERVE)
  off (default): 2-call recipe                         -> 26, NO offer lines (0 overhead)
  on:  2-call recipe -> two trace lines:
       offer fn1 args=1 ack=1
       offer fn2 args=1 ack=1
       26
  all four ack-kinds witnessed across calls:
       ack=1  (affirmative result)   ack=0  (zero state)
       ack=nothing (decline)         ack=node (counter-offer / interned node)
  FK_OBSERVE=0 -> 0 offer lines (explicit off)

ZERO REGRESSION
  (mul 6 7) -> 42   head -> 11   nth -> 6   str_eq -> 1/0
  sum8 (8-arg call) -> 36   multi-function (k 1 26) -> 26
  native-vs-rented -> 11111

STONES INTACT
  (nothing? (nothing)) -> 1     (eq (nothing) 0) -> 0
```

Base-vs-new were compared on a clean `HEAD` build (`/tmp/fkwu_base`): the only behavior
deltas are the three intended ones (`0→nothing`, `0→5`, `0→99`); every regression and
stone value is byte-identical across the change.

## Honest floor

- **Observed (mac `--src`, this kernel):** all gate rows above, real native eval through
  the C-bootstrapped universal kernel via `--src`, no Go flattener, no walker.
- **`--src` limitation, named:** the contract's `oac-offer (cell args)` calls a function
  VALUE (tag-44 indirect by bound name); the `--src` parser resolves `(head ..)` as a
  NAMED call only, so the contract's combinators run on the flattened four-way path, not
  through `--src` indirect-by-value. The `--src` proof here therefore expresses the SAME
  shape directly over real named calls (the new non-resolving-call → nothing primitive) —
  `choice`/`try` over the acks real calls now yield. This is honest: the offer/ack
  *semantics* are proven native on `--src`; the higher-order indirect-by-bound-name lane
  on `--src` is a separate, pre-existing gap (the four-way path carries it today).
- **Pre-existing `--src` do-let edge (not introduced here):** `(let r <expr-with-calls>)`
  then bare `r` can drop the value; the gate recipes use direct expressions, which the
  reducer evaluates correctly. Named as a separate handle, untouched by this stone.

## Reproduce

```
cc -O2 -o /tmp/fkwu runtime/fkwu-uni.c
printf '(nothing? (no-such-cell 1 2))' | tee /tmp/g.fk; /tmp/fkwu --src /tmp/g.fk        # 1
FK_OBSERVE=1 /tmp/fkwu --src <a-2-call-recipe>                                            # two offer/ack lines
/tmp/fkwu --src <same-recipe>                                                             # no trace
```
