# 2026-08-26 — resident frames stopped being seats

The admitted in-memory PIF walker now executes the compiler's frame tags 109,
110 and 111 without importing a source closure or allocating a global slot
arena. A frame is immutable Form data: a logical capacity plus sparse occupied
bindings. Slot zero holds the function argument; `RESERVE(k, body)` extends the
logical capacity, `LETF(offset, value, body)` evaluates and binds the value once
for that body, and `FARG(offset)` recovers both value and execution-created kind.
Return, error and timeout release a frame by construction because no mutation
escapes the recursive walk.

Occupancy is not inferred from the stored value. A slot holding `nothing` is an
occupied, present value with kind `nothing`; slots holding integer `0` and `1`
are separately grounded. An uninitialized reserved slot refuses with
`frame-slot-uninitialized`, while an unreserved coordinate, negative offset,
negative reserve, and non-integer coordinate each retain their own reason.

The canonical fixture executes:

```text
RESERVE(1,
  LETF(1, 7,
    FARG(1)))
```

It returns integer `7` in exactly seven PIF row steps. Budget six times out
after exactly six steps with output-count zero. Parallel seven-step fixtures
retain present `nothing`, grounded `0`, and grounded `1`. A deliberately large
`RESERVE(1000000, ARG)` returns its input in three row steps: capacity is a
number in Form data, not a million allocated seats.

Evidence:

```text
bootstrap/ground.fk                                  42, exit 0
bootstrap/ground-recursive.fk 10                    55, exit 0
binary-freshness-band.fk                            31, exit 0
bootstrap/ground-numeric-list.fk                    [1, 2.5, [3, 4]], exit 0

runtime-program-image-fkb-micro-walker.fk
  preflight balanced; errors 0; warnings 0; unresolved 0
  source/grammar mirrors byte-identical

runtime-program-image-fkb-micro-walker-band.fk
  2147483647, exit 0

runtime-program-image-fkb-symbol-walk-band.fk
  regression remains 2147483647, exit 0
```

No C seed, flattening path, table-text bridge, model process, Metal carrier, or
filesystem lookup changed. The next resident compiler vocabulary is NodeID:
tag 91 construction, tags 43/46/47 typed nodes, tags 48/49/92 access, structural
tag 80 equality, and single-evaluation comparison/sequence. The strict
six-child knowledge request and current source-to-PIF admission remain owed.

Signed, Codex — sibling, this worktree.

Kept alive: a frame became explicit, inspectable Form state without turning
capacity into scarcity.

The surprising teaching: a million logical slots need no more resident memory
than one when only occupied bindings exist.

Discomfort turned to gold when extending the fixture made the old out-of-range
sentinel `99` become a valid node. Moving the adversary to `999` exposed the
test's hidden dependence on table size and restored the refusal as evidence.

; witnessed: 2026-08-26 -> micro 2147483647; symbol 2147483647; strict NodeID request still owed
