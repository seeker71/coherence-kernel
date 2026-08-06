# Receipt — form-eval grows from the first stone to a meta-circular core: defn + user-calls + let, in Form (2026-06-29)

**What happened:** `grammars/form-eval.fk` — the Form meta-evaluator written in Form, the source tree-walk that
computes a recipe's value AS it reads the source string (no flatten table, the cursor reads and computes directly) —
now evaluates a **fuller grammar**. The first stone evaluated `integer | (op a b)` for `op ∈ {add,sub,le,if}`,
stateless, threading `(value, end-pos)`. It now threads an **environment** as a third element `(value, end-pos, env)`
and evaluates a real meta-circular core: all binary builtins, `let`, `do`, `defn`, **user-defined-function calls**,
and recursion. A recipe with a user-defined function runs THROUGH the Form walker on the c-bootstrapped `fkwu`
kernel — integers all the way down, no host interpreter. The C seed (`runtime/fkwu-uni.c`) is **unchanged**; this is
Form coding.

## The grammar form-eval now evaluates

```
expr   := integer | symbol | '(' form ')'
form   := (op a b)            op in {add,sub,mul,div,mod,le,lt,gt,ge,eq}   ; all binary builtins
        | (if c t e)                                                       ; conditional (true short-circuit)
        | (let name val body)                                             ; lexical binding
        | (defn name (params...) body)                                    ; register a function
        | (do e1 e2 ...)                                                   ; sequence, threading env
        | (fname arg...)        fname a user-defined function             ; THE meta-circular call
symbol := lookup in the environment (a var bound by let, or an arg bound at a call)
```

**The environment** is the new thread: an assoc-list of bindings, cons-chained and terminated by integer `0` (our
nil — `(eq env 0)` is the empty test). Two entry kinds: a variable `(name 0 value)` and a function
`(name 1 params body-pos)`. A `defn` stores the function's **body source-position** (not a pre-parsed form) and
extends the env; a call looks the name up, evaluates the args (threading env), zips them onto the params, and
**re-evaluates the stored body** under that extended env. Because `defn` puts the function in the env before its
body is ever run, a function sees **itself** — recursion is free. `if` is a **true short-circuit**: only the taken
arm is evaluated, the other is merely *skipped* for position (without this, a recursive `(if base 0 (recur))` would
recurse on its dead arm forever — that was the one real design bug, fixed by `fe-skip-expr`, a no-eval expression
skipper).

## Proof values (each `fe-eval` on a SOURCE STRING, on `fkwu` via `--src`)

| source | value | claim |
|---|---|---|
| `(add 40 2)` | **42** | first stone preserved |
| `(if (le 5 2) 111 999)` | **999** | first stone preserved |
| `(mul 6 7)` | **42** | all binary ops |
| `(eq 5 5)` | **1** | comparison → 1/0 |
| `(let x 40 (add x 2))` | **42** | lexical binding |
| `(do (defn f (n) (add n 1)) (f 5))` | **6** | **THE milestone — a recipe with a user-defined function, run THROUGH form-eval** |
| `(do (defn s (n) (if (le n 0) 0 (add n (s (sub n 1))))) (s 5))` | **15** | recursive user call |
| `(do (defn fac (n) (if (le n 1) 1 (mul n (fac (sub n 1))))) (fac 5))` | **120** | factorial (mul + recursion) |
| `(do (defn fib (n) (if (le n 1) n (add (fib (sub n 1)) (fib (sub n 2))))) (fib 10))` | **55** | tree recursion |
| `(do (defn dbl (x) (mul x 2)) (defn inc (x) (add x 1)) (dbl (inc 20)))` | **42** | two co-defined fns + nesting |
| `(do (defn f (n) (let y (mul n 2) (add y 1))) (f 20))` | **41** | `let` inside a function body |

Band: `grammars/tests/form-eval-band.fk` rolls ten of these into one verdict — each claim a distinct power of two —
and computes **1023** (all claims land), the milestone being claim **64**.

## Bodies touched

- `grammars/form-eval.fk` — extended from the first-stone evaluator to the meta-circular core (env-threaded,
  +181/−25 lines). Pure Form, integers-only values.
- `grammars/tests/form-eval-band.fk` — new band, verdict **1023**, the milestone pinned as claim 64.

## Honest floor / what remains

- **Observed native on Windows (`fkwu.exe`, `--src`):** every value in the table above, run on the C-bootstrapped
  kernel built with TDM-gcc. Witnessed on metal.
- **Values are integers** (and 1/0 for comparisons) — the proof scope. **String literals, float literals, and lists
  as evaluated values** are the named next rung: the C seed already carries them as primitives, so the extension is
  more grammar in `fe-apply`/`fe-expr2`, not new kernel ops.
- **Scoping is the threaded env** — a `let`'s binding does not escape (returns the original env); a call evaluates
  its body under the *call-site* env + params. Lexically-closed closures over a definition-site env are a further
  rung, not needed for the proof.
- **Four-way:** the band uses only the integer/list/string surface all four kernels carry; it is four-way-honest by
  construction. Running it four-way through the walkers (vs. on-`fkwu` `--src`) awaits the same on-platform flatten
  gap named for the other Windows bands — the band encodes the witnessed invariants.

## Reproduce

```
gcc -O2 -o fkwu.exe runtime/fkwu-uni.c -lws2_32 -lwinmm -lavicap32 -luser32 -lwlanapi -lbthprops -lwinhttp

# core.fk's surface-grammar prelude isn't parseable by the bare C seed, so inline its two leaf helpers:
PRE='(defn char_at (s i) (substring s i (add i 1)))
(defn ord (c) (str_byte_at c 0))'
# THE milestone — a user-defined function run through form-eval -> 6
( printf '%s\n' "$PRE"; cat grammars/form-eval.fk; printf '\n(fe-eval "(do (defn f (n) (add n 1)) (f 5))")\n' ) > /tmp/fe.fk; ./fkwu.exe --src /tmp/fe.fk
# the full band -> 1023
( printf '%s\n' "$PRE"; cat grammars/form-eval.fk; cat grammars/tests/form-eval-band.fk ) > /tmp/band.fk; ./fkwu.exe --src /tmp/band.fk
```
