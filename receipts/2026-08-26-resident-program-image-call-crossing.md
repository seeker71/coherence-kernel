# 2026-08-26 — a program image called its own function in residence

The 4,096 function-seat wall belongs to the temporary C seed's joined source
closure. It is not the shape of a Form program. This movement crossed the first
real alternative: an already-admitted program image resolved `unit/main` by its
canonical symbol and that function called `unit/helper` through its own PIF
function-root table. No target function entered the seed's global `fk_fn[]`, no
child process launched, and no C or flattened table grew.

The executable movement lives in
`runtime-program-image-fkb-micro-walker.fk`. Tag 12 now:

1. validates the target function index and argument node;
2. evaluates the argument under the same bounded budget;
3. binds the result as the callee input;
4. walks the target root from the admitted PIF function-root table;
5. propagates success, error, and timeout with exact accumulated steps.

An out-of-range target is the loud reason `call-target-out-of-range`. An invalid
argument remains `child-out-of-range`. A recursive program-image call spends
the same budget and terminates as `step-budget-exhausted`; recursion does not
escape into an unbounded host stack claim.

The symbol resolver itself had one hidden rented seam: `value_kind`, which
exists on the three reference walkers but intentionally not on `fkwu`. Its
request guards now use the body's native string recognition, exact integer
decimal round trip, and remaining admitted list schema. Preflight is therefore
clean on the local kernel rather than green only on sibling proof walkers.

Live direct-source observation:

```text
status=trace-produced
reason=walk-completed
canonical-key=unit/main
entry=9
root=20
zero-value=0
zero-output-count=1
one-value=1
one-output-count=1
steps=5
```

The output count matters: successful value `0` is present and is not confused
with an error row whose placeholder field is zero. Successful `1` travels the
same route. Timeout and target/child failure are separate trace status/reason
signals.

Evidence:

```text
program-image-symbol-entry-band                 33554431, exit 0
runtime-program-image-fkb-micro-walker-band    134217727, exit 0
runtime-program-image-fkb-symbol-walk-band     536870911, exit 0
all six changed source/band chains             preflight clean, unresolved 0
source/grammar mirrors                          byte-identical
runtime/fkwu-uni.c                              unchanged
```

This is the in-process multi-function call primitive, not yet the whole
form-cli homecoming. The admitted fixture is an in-memory PIF. The current
source-runner `.fkb` and the Form container builder still carry different
builder identities, and the micro-walker does not yet implement the
strings/lists/files/hashes/NodeID operations needed by `fcnkd-query`.
Consequently the child-process NodeID carrier remains visible. The next honest
movement is to lower one strict content-bound NodeID reader unit into this PIF
vocabulary, observe its exact lookup, then remove the process membrane.

Signed, Codex — sibling, this worktree.

Kept alive: the seat wall produced a loadable function boundary instead of a
larger hall.

The surprising teaching: canonical symbol resolution and resident walking were
already present; one missing CALL row and one reference-walker-only kind guard
kept them from becoming a native multi-function organ.

Discomfort turned to gold when the first aggregate bands returned almost-green
numbers: their missing receipt bits exposed that `read_file` reports an absent
file as an empty string on this lane, not `nothing`. The fallback was corrected
and the exact full verdicts returned.

; witnessed: 2026-08-26 -> resident canonical multi-function call observed; form-cli NodeID lowering still owed
