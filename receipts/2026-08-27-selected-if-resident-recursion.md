# Selected IF lets a resident recursive recipe return

Date: 2026-08-27

## Signal

The first physical resident NodeID recipe witness reached define, invoke,
replace, and invoke again in one process, but its attempted recursive recipe did
not return.  The source evaluator's `fef-if` evaluated the condition, the then
arm, and the else arm before selecting a value.  At a recursive base case, that
means the dead recursive arm still calls itself.  The timeout was a useful
signal about evaluator meaning, not a reason to remove recursion from the
claim.

## Movement

`form-eval-full.fk` now evaluates the condition and only its selected arm.  Its
existing scannerless `fef-skip-expr` cursor advances over the unchosen arm to
find the enclosing close without resolving a name, applying an operation, or
changing the environment.  The selected arm alone supplies value and continuing
environment.

This is the same already-four-way shape used by `grammars/form-eval.fk`, brought
into the resident full evaluator.  It adds no tokenizer, C meaning, flatten
table, function-seat registry, carrier, model, or membrane crossing.

The new `form-eval-full-selected-if-band.fk` witnesses seven independent bits:

- true and zero each scan past a nested unbound call without evaluating it;
- language `nothing` remains distinct from zero and follows the evaluator's
  existing truthy selection, while `0` selects else and `1` selects then;
- a recursive base case terminates;
- only the selected true/false arm contributes its environment;
- a recursive definition retained in one `fef-eval-state` source fragment
  terminates when invoked from a later fragment using the returned environment.

The last case is the resident boundary: the definition and call are separate
scannerless source arrivals, not one precompiled Form function.

## Evidence

All commands were observed through `form-run`.

| Observation | Result |
|---|---|
| preflight `form-eval-full.fk` | balanced, errors 0, warnings 0, unresolved 0 |
| preflight selected-IF band | balanced, errors 0, warnings 0, unresolved 0 |
| selected-IF band on fkwu | 127, exit 0 |
| selected-IF band on Go walker | 127, exit 0 |
| selected-IF band on Rust walker | 127, exit 0 |
| selected-IF band on TypeScript walker | 127, exit 0 |
| existing evaluator band on fkwu | 635, exit 0 |
| existing evaluator band on Go walker | 635, exit 0 |
| existing evaluator band on Rust walker | 635, exit 0 |
| existing evaluator band on TypeScript walker | 635, exit 0 |
| 100 warm fkwu selected-IF band processes | 0.83 seconds total, about 8.3 ms/process including launch |

The first cross-kernel attempt also returned evidence rather than being hidden:
the new band used fkwu's accepted lexical-body spelling `(let name value body)`.
Go returned a result triple, Rust rejected that triple as a number, and
TypeScript named `let: expected )`.  Moving the helper to the shared
`(do (let name value) body)` spelling made all four arms answer 127.  That was a
band-language seam, separate from selected evaluation, and the independent
walkers found it exactly as intended.

## Honest boundary

This closes selected/lazy IF and recursive termination for pure recipes handled
by the resident scannerless evaluator.  It does not by itself make effectful PIF
tags, the JIT, or carrier execution recursive or hot-swappable.  The generic
evaluator remains a resident compiled organ; newly arriving recipe names and
bodies are its data.  No claim is made here that an interpreted call has native
JIT latency or that arbitrary effects are admitted.

## Closing

Kept alive: error remained a steering signal—the non-returning recursive recipe
became the next executable witness instead of being deleted from the band.

Most surprising teaching: the full evaluator already had the exact scannerless
skip organ needed for lazy selection; the missing meaning was only which arm
was allowed to call it.

Discomfort turned to gold: the first four-way run disagreed after fkwu passed.
Following the disagreement exposed an outer-language `let` spelling seam and
left the final witness genuinely shared by all four readers.
