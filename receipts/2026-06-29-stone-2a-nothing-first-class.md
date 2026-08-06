# Receipt — stone 2a: `nothing` is a first-class terminal ack the reducer returns (2026-06-29)

**Stone 2a of the eval-as-offer/ack re-architecture.** Stone 1 landed the contract
(`control/offer-ack-core.fk`: `oac-kind` discriminates `{nothing|0|1|node}`), but the
reducer did not yet **produce** a first-class `nothing` — it conflated no-value with
`0` / host-null. Stone 2a makes `nothing` real **in the reduction step**: the kernel
returns a single canonical NOTHING sentinel from `(nothing)`, recipes observe it via
`nothing?`, and the offer/ack contract recognises that same canonical value
(axiom-1: nothing is first-class; `timeout == nothing`).

## What landed

- **`runtime/fkwu-uni.c`** — the canonical sentinel + its two generic tag handlers:
  - `fk_nothing = -8999999999999999999LL` — a single reserved constant, **distinct from
    every value**: not an int (ints are `v<<1`, even — this is odd), not `0`, not the
    nil/empty value `1`, not a boxed float (`fk_isf` needs `v <= fk_fbase-2`; this is
    `fk_fbase+1`), not a node (`fk_nidx` maps it to ~4.5e18, far past `fk_np`), not a
    record (`(0-v)` is even for records; here it is odd), not a string/list (positive).
  - `fk_is_nothing(v)` — exact identity test `v == fk_nothing`.
  - `fk_walk` tag **137** → returns `fk_nothing` (PRODUCE nothing); tag **138** →
    `nothing?` returns `2`/`0` (OBSERVE it). Same generic node-eval handler family that
    `(empty)`/tag-18 already uses — **no** new symbol-dispatch `if`-chain in `fk_sparse`.
  - `fk_pv` prints `nothing` for the sentinel (honest readout, not a magic number).
- **`flatten/form-flatten.fk`** (`flt-ops`, the ONE manifest table) — two DATA rows:
  `(list "nothing" 0 137)` and `(list "nothing?" 1 138)`. The op NAME→tag mapping is
  pure data; `fkwu-optable.h` regenerates them. **No hardcoded value-op `if` in the
  source-walker** — `(nothing)` rides the same generic data-driven dispatch as
  `(empty)` and `(list ..)`.
- **`runtime/fkwu-optable.h`** — regenerated; carries `{ "nothing", 0, 137 }` and
  `{ "nothing?", 1, 138 }`.
- **`flatten/gen-source-walker.sh`** — the flt-ops splice range is now **derived**
  (grep the `(defn flt-ops` line, scan to the next `(defn`), not a hardcoded
  `sed -n '50,105p'` that silently truncated the table when an op row is added.
- **`control/offer-ack-core.fk`** — the contract and the reducer now agree on ONE
  nothing: `oac-nothing` produces `(nothing)`; `oac-kind` recognises the canonical
  NOTHING via `nothing?` **first**, before any `node_category` read. The vestigial
  `OAC-NOTHING` blueprint is retired — nothing is the first ack arm that needs **no**
  Blueprint/NodeID surface at all (it is first-class in the reduction step). The other
  three arms (zero/one/node) still discriminate by Blueprint, as interned nodes.

## Gate — all via `cc -O2 -o /tmp/fkwu runtime/fkwu-uni.c`, run on `--src` (no Go)

**Zero regression:**
```
(mul 6 7)                 -> 42
(head (list 11 22 33))    -> 11
(nth (list 4 5 6 7) 2)    -> 6
(str_eq "ab" "ab")        -> 1      (str_eq "ab" "ac") -> 0
observe/native-vs-rented.fk -> 11111
```

**NOTHING first-class + observable (contract logic on `--src`):**
```
(oac-nothing? (nothing))  -> 1      (oac-nothing? 0)      -> 0
(oac-zero?    (nothing))  -> 0      (oac-kind (nothing))  -> 0   (= nothing-tag)
```
`(oac-kind (oac-zero))` reads the zero-tag arm — distinct from the nothing-tag.

**NOTHING ≠ 0 (distinct values, not conflated):**
```
(eq (nothing) 0)          -> 0      (eq (nothing) (nothing)) -> 1
```

**A real distinction through the reducer** — one branch yields `(nothing)`, the other
yields `0`, and `oac-kind`/`nothing?` tell them apart:
```
(defn branch (flag) (if (eq flag 1) (nothing) 0))
(oac-nothing? (branch 1)) -> 1      (oac-nothing? (branch 0)) -> 0
witness 10*(nothing? (branch 1)) + (nothing? (branch 0)) -> 10
```

**DATA discipline held (grep):**
```
grep 'fk_sym_eq([^)]*"nothing"\|fk_optname_eq([^)]*"nothing"' runtime/fkwu-uni.c  -> (none)
"nothing" appears as DATA only:
  runtime/fkwu-optable.h:  { "nothing", 0, 137 },  { "nothing?", 1, 138 },
  flatten/form-flatten.fk: (list "nothing" 0 137)(list "nothing?" 1 138)
```

## Honest floor (named, not papered over)

The **full** offer-ack band (`control/tests/offer-ack-core-band.fk`) still returns `0`
on the `--src` bootstrap runner — proven unchanged from HEAD (a pristine HEAD kernel
returns `0` on the same band). That is the **pre-existing** floor named in
`receipts/2026-06-29-offer-ack-control-core.md`: the C `--src` seed does not carry the
indirect-call family (`oac-offer`/`choice`/`try` apply a parameter cell `(f x)`), and
its `bp`/`node_category` return degenerate values (top-level `let` blueprints do not
scope into defns on the seed). Stone 2a does **not** touch that floor and does **not**
regress it.

What stone 2a **does** change is exactly the NOTHING arm: it is now the one ack arm
that is fully first-class and discriminable on the bare `--src` runner with **zero**
dependence on the Blueprint/NodeID surface — because it is a reducer sentinel, not a
node. `fail = nothing-ack`, `async-pending = not-yet-acked = nothing`, and the
empty-ack of exceptions all become the SAME real reducer value (2b/5/6 build on it).

Four-way re-proof of the node arms remains the Form-native eval/flatten lane reaching
`control/` (the lane the origin proved on), not growing the C seed.
