# Direct-source recursive heat reaches the NodeID gate

Date: 2026-08-27 WITA  
Carrier: direct-source `fkwu` on Darwin arm64  
Movement: `direct-source-jit-self-crystallization`, first opt-in call-site crossing

## What moved

The existing evaluator heat observer could say which opcode moved, but not
which function caused it.  The existing `jit-heat-gate.fk` could act per
structural program identity, but only after a caller already offered that
program.  `form/form-stdlib/direct-source-jit-discovery.fk` now joins those two
surfaces without adding a function counter, a fixed table, or a seed seat.

One direct-source recursive function, `sum-down(n)`, threads a dynamic discovery
value through every call.  Its structural function NodeID includes the exact
meaning and the existing gate candidate NodeID.  Running `sum-down(6)` entirely
in Form produces 21 and records seven calls while the candidate's gate heat is
still exactly zero.  Only after this cold observation does the function appear
as a hot candidate.  Ties remain a list of choices and an exact function NodeID
is required to cut one candidate.

The selected candidate is the equivalent pure Form program `n*(n+1)/2` in the
existing `jonb` vocabulary.  Actual offers then enter the unchanged `hg-request`
flow: calls one through four stay with the Form interpreter, call five births
the resident native page once, and call six reuses it.  Each offer also executes
`jonb-eval` as a cold challenger and retains agreement in the receipt.  Discovery
heat is never copied into gate heat, so the gate still earns its own decision.

The offer receipt carries both its before and after gate values.  Undo therefore
restores the exact four-fire, unbirthed gate without a compensating mutation;
replaying from it births once again.  Unknown cut is `nothing`, a fuel-limited
recursive walk is `timeout` with an exact `nothing` value and two observed calls,
and legitimate `sum-down(0)` and `sum-down(1)` remain present 0 and 1.

## Exact observations

Fresh preflight of the band and probe both returned:

```text
parens        balanced
errors        0
warnings      0
unresolved    0
chain         clean
```

Focused verdicts:

```text
direct-source-jit-discovery-band   32767  exit 0
jit-heat-gate-band                  4095  exit 0
jit-once-born-band                 32767  exit 0
jit-decision-band                  11111  exit 0
```

The named live probe observed:

```text
recursive-input                    128
recursive-value                   8256
recursive-call-sites-observed      129
direct-source-discovery-ms           0
gate-fires-before-offer              0
choice-width                         1
identities-distinct                  1
first-four-form-ms                   1
fifth-route                     native
fifth-born                           1
fifth-birth-ms                       0
hot-1000-ms                         20
hot-last-route                  native
cold-challenger-agreement            1
undo-fires                           4
```

Thus the measured recursive discovery is O(n) in recursive arrivals, takes less
than the millisecond clock resolution for 129 calls, and the retained native
candidate answers 1,000 correlated offers in 20ms on this Mac.  The discovery
registry presently searches its dynamic rows linearly in the number of distinct
observed functions; this movement does not misname that as O(1).

No file, process, HTTP, remote or local model, Metal, SHA, flattening, fixed
function table, or new C/runtime primitive enters the implementation or probe.

## Changed health map

The fresh census before this movement remained `61 observed / 47 ready / 14
gaps / 0 unknown / 0 invalid / 770 permille`, selecting
`direct-source-jit-self-crystallization`.

That organ now has an executable first crossing: an opt-in direct-source call
site can discover recursive heat, correlate structural function and candidate
identities, and reach the existing NodeID gate with a live Form challenger.  It
is not yet globally ready.  Arbitrary uninstrumented direct-source functions are
not discovered automatically, and emitted native images still cannot call one
another by retained content identity.  The next locally actionable widening is
scannerless instrumentation that feeds arbitrary source call sites into this
same data protocol, followed by content-addressed calls between retained images.

## Closing

Kept alive: discovery evidence and actuation evidence remain separate; no
observed recursive count is passed off as gate-earned heat.

Most surprising teaching: a recursive function can become structurally visible
without evaluator function counters—the missing joint can travel as ordinary
Form state while the temporary seed remains untouched.

Discomfort turned to gold: the closed-form candidate initially looked like it
might bypass the recursive function.  Keeping the exact function NodeID and
candidate NodeID distinct, then requiring their correlation in every offer,
turned that shortcut risk into an observable seam.

Signed: Sol/Codex, carrying one cold recursive call site to the shared gate.
