# BML sheds the walker: the native run step runs on fkwu itself

2026-09-15 — Sema (Claude Opus 5), one worktree, landed on origin main.

## What moved where

`form/form-stdlib/bml/bml-form-lower.bml` lowers a parsed, linked BML unit
into Form defns. fkwu compiles those defns in a child process and runs them on
its own evaluator, where the JIT can reach them. Each method is one defn over
its own parameters. Each mutable local is one Form name per assignment, found
through the slot key that `grammars/bml.fk`'s scope reading gives its binding.
Each loop is one tail-recursive defn over the names in scope. Scope, overload
choice, class layout, control modes, promotions and edge checks all stay
bml.fk's own readings: the new unit reads them and writes Form.

| rung | commit | door reading |
|---|---|---|
| 1 arithmetic, names, calls | 7b6086993 | 56 ok of 70 (the rest control, choose, class) |
| 2 control, loop as one tail-recursive defn | 6d192315a | 59 ok of 75 |
| 3 choose, fail, save/restore/discard, journal | 9f0bc1ea3 | 64 ok of 75 |
| 4 classes over the layout | 5021491e8 | 75 ok of 75 |
| switch: `bml-run-unit-value` moves to the unit | 9d760e988 | 75 ok of 75 on `observe/bml-native-run.bml` |
| field initializers read where the class lives | 5b3114fe9 | 87 ok of 87 |

Each commit landed after `gate/drift-gates-run.bml` read
`pass=16383 full=16383 refused=0`. The door keeps its shape and loads the new
unit. The temporary door `observe/bml-form-run.bml` is gone. bml.fk lost only
its run step.

A lowering's needs still reach `bch-admit` and `bch-ran`. On the switched door
the compiler organ voiced 19 admission, 7 lowering-need and 23 run readings. The four
sources an unheld fail ends still reach `bml-run-heard`, now on lane `form`,
standing on 12, 6, 13 and 7.

What still walks: `bml-hati-run-unit-value` and the other `bml-hati-run-*`
entries (`-linked`, `-class-method`, `-new`, `-value`) serve the registered
four-way proof bands (general-calls, import-symbol, import-consts, path-words,
unresolved-names, lane-parity, class-dispatch). `fks-table-file` and
`hati-os-kernel-emit` read the same tables for the fourth-arm walker and the
Hati OS emit. Those are the proof lanes where Go, Rust, TS and the fourth arm
walk one table. The door no longer walks.

## Measured

Measured on this Mac under a busy host: sibling Go, Rust and TS kernels ran at
100% each, and the load average read 5.6 to 12.6. Before and after ran
back-to-back under that load.

| what | walker | fkwu itself |
|---|---|---|
| the door, 75 sources | 94.84 s wall, 312 MB peak | 2.29 s wall, 246 MB peak (warm) |
| `bml-control-loop.bml` (a million passes) | 95.28 s wall, 67 MB peak | 0.39 s wall, 42 MB peak (the door process) |

## A loop that never ends, voiced and cared for while it runs

The walker cannot voice a runaway loop from inside itself: `while (1)` ran
until an outside watchdog stopped it at 40 s. On fkwu, the run step watches
its child without blocking. `host_alive` answers 0 once the child has ended,
unreaped or not, and `kernel_page_hot` reads the running child's hot defns.
A `while (1)` run on fkwu read `alive 1` after 1.5 s, and `kernel_page_hot`
named the loop's own defn: `w6`, 6,772,921 entries, at its line and column in
the lowered file, beside `m0` with 1 entry.

A run that ends within its time says nothing. One that goes past it raises an
organ-health reading naming the unit, how long it has run and its hot defns,
with two offers: `extend` and `stop`. The budget is that care offer, not a
wall. A source that declares `Budget: N` in its head is given more time while
N holds, and the organ stops a run only when no more time is on offer
(default 2000 ms).

- `bml-control-runaway.bml` loops forever. It was observed running 2031 ms
  against 2000; the response was `stop`, applied as
  `{"run":"stopped","budget_ms":2000}`, and the source answers
  `BML-HATI-UNSUPPORTED run/budget`, which `bch-ran` voices where it ran.
- A 40,000,000-pass source declaring `Budget: 20000` was observed at 2031 ms;
  the response was `extend`, applied as `{"run":"extended","budget_ms":20000}`,
  and it answered 40000000.
- `bml-control-loop.bml` answers 1000000 in about 0.3 s and says nothing.

## The most surprising teaching

Once locals are values, backtracking over them is free. A choose branch that
failed has no locals to roll back: the next branch starts from the names the
choose was entered with. Only a field needs the journal, because a field lives
in its record, outside every frame. Each of the four rungs read full on its
first run. The lowering asks bml.fk for every choice instead of making its own.

## Where discomfort turned to gold

The first drift run held back one name (`kernel-conformance`), and the first
fear was that the new unit had broken a sibling. Reading that door, the
TypeScript kernel was
simply absent from this worktree. With the lock file identical, I copied
`node_modules` in and the gate read full. After the switch, the door read 86 of
87 on a freshly rebased tree: `bml-class-field-init.bml` answered
name/unbound where the table answered 56. The pull was to suspect the new
fixture. The upstream diff (654732cf6) said otherwise: initializers now lower
where their class lives, and my lowering still read them in an empty env. The
difference was mine, and 5b3114fe9 heals it.

## The next stone, named with its measure

The loop runs in constant stack on fkwu's evaluator, but the JIT's loop lane
does not take it. Each pass reads the run's record cells for the mode, so 40M
passes cost about 9.5 s (about 4.2M passes/s). The same shape with the mode as
a value name ran a million passes in about 5 ms. The stone: when no call in a
loop can carry a transfer back, keep the mode local to the loop defn.

## Frontier word

**walkshed** (0 hits in the tree before this receipt).
*Question:* when does a program shed the walker that carried it?
*Answer:* when the kernel that used to run the walker names the program's own
loop. Before, `kernel_page_hot` showed the walker's eval defns. Now it shows
`w6`, the BML loop itself, 9,632,579 entries in two seconds. The shedding can
be observed, not just claimed.
