# Receipt — form-eval evaluates STRING, LIST, and FLOAT values, in Form (2026-06-29)

**What happened:** `grammars/form-eval.fk` — the Form meta-evaluator written in Form, the source tree-walk that
computes a recipe's value AS it reads the source string (no flatten table) — now evaluates **three new value kinds**
on top of its integer meta-circular core: **STRING literals**, **LIST ops**, and **FLOAT literals**. The values are
**opaque** to form-eval — a string value, a list value, a float value passes straight through the `(value, end-pos,
env)` triple; the kernel primitive does the work. The new work was almost entirely **parsing** (`fe-expr2` branching
on `"` and on float-marker chars inside a number) plus routing the new ops through a generic variadic primitive path.
The C seed (`runtime/fkwu-uni.c`) is **unchanged** — provably: the source is byte-identical to main, the C seed
already carries strings/floats/lists as primitives. This is pure Form coding.

## The grammar form-eval now evaluates

```
expr   := integer | float | string | symbol | '(' form ')'
float  := a number literal carrying '.' or 'e'/'E'   -> (str_to_float <literal-text>)
string := '"' ... '"'  (escapes \n \" \\ handled)    -> the literal's inner text
form   := (op a b)            op in {add,sub,mul,div,mod,le,lt,gt,ge,eq}   ; binary builtins (now float-aware)
        | (if c t e) | (let name val body) | (defn name (params...) body) | (do e1 e2 ...)
        | (prim arg...)        prim a STRING/LIST kernel op (see below)     ; variadic-arity, applied by name
        | (fname arg...)       fname a user-defined function                ; the meta-circular call
```

**New primitive ops** routed through `fe-prim` (collect all args into a value-list, apply the kernel op by name,
pull positional args by head/tail): `str_concat`, `str_eq`, `substring`, `str_len`, `str_byte_at`, `str_to_float`;
`list` (variadic), `cons`, `head`, `tail`, `empty`, `nth`, `len`.

**Parsing additions:** `fe-expr2` now branches on `"` (char 34 → `fe-str`, scan to the closing quote with `\`-escape
handling, value = unescaped inner text); a number is scanned to its delimiter and, if it carries `.`/`e`/`E`
(`fe-isfloat`), interned as a float via `str_to_float` over the literal substring, else the existing integer
digit-fold. `fe-skip-expr` (the no-eval skipper used to find a defn body's end / skip an if's dead arm) was taught
to skip a string literal past its closing quote and to end a number at its delimiter — so a defn body or `if` arm
containing a string/float skips correctly (proven by recursion-to-string-base below).

## Proof values (each `fe-eval` on a SOURCE STRING, on `fkwu.exe --src`, TDM-gcc on Windows)

| source | value | kind |
|---|---|---|
| `(str_concat "a" "b")` | **ab** | STRING |
| `(str_eq "x" "x")` / `(str_eq "x" "y")` | **1** / **0** | STRING |
| `(str_len "hello")` | **5** | STRING |
| `(substring "hello" 1 3)` | **el** | STRING |
| `(str_byte_at "abc" 0)` | **97** | STRING |
| `(head (list 10 20 30))` | **10** | LIST |
| `(head (tail (list 10 20 30)))` | **20** | LIST |
| `(len (list 10 20 30))` | **3** | LIST |
| `(nth (list 10 20 30) 1)` | **20** | LIST |
| `(head (cons 5 (empty)))` | **5** | LIST |
| `(add 0.5 0.25)` | **0.75** | FLOAT |
| `(mul 1.5 2.0)` | **3** | FLOAT |
| `(add 1.5e1 0.0)` | **15** | FLOAT (e-notation) |
| `(str_to_float "2.5")` | **2.5** | FLOAT |
| `(do (defn g (s) (str_concat s "!")) (g "hi"))` | **hi!** | **COMBINE — value kind + defn + call** |
| `(do (defn h (n) (str_concat "v=" (substring "xyz" 0 1))) (h 0))` | **v=x** | string inside a defn body |
| `(do (defn lbl (n) (if (le n 0) "done" (lbl (sub n 1)))) (lbl 3))` | **done** | recursion to a string base case |

Band `grammars/tests/form-eval-band.fk` now rolls **16 claims** into one verdict (each a distinct power of two) and
computes **65535**: claims 1..512 are the meta-circular core (unchanged), claims 1024..32768 are the three new value
kinds (string, list, float).

## A note on top-level printing (not a correctness gap)

The kernel's top-level result printer (`fk_pv_root`) decides string-vs-number rendering by **static analysis of the
top-level call's root op**, not by the value's tag. So a bare `(fe-eval "(str_concat ...)")` at top level prints the
string value's internal number; the **value is correct** — `str_len`, `head`, `str_concat`, and round-trips all
confirm it (`(str_concat (head (list "xy" 1)) "z")` → `xyz`). To render a string proof, wrap it so the root op is
string-returning: `(str_concat (fe-eval "...") "")` → `ab`. This is purely a display convention of the seed's static
print path; form-eval threads the value faithfully.

## Bodies touched

- `grammars/form-eval.fk` — string/float literal parsing in `fe-expr2`/`fe-num`/`fe-str`, the `fe-prim` variadic
  primitive path + `fe-prim-apply`/`fe-isprim`, and `fe-skip-expr` taught to skip strings/floats. Pure Form.
- `grammars/tests/form-eval-band.fk` — six new claims (1024..32768) for the three value kinds; verdict **65535**.

## Honest floor / what remains

- **Observed native on Windows (`fkwu.exe`, `--src`):** every value above, on the C-bootstrapped kernel built with
  TDM-gcc. Witnessed on metal. Kernel C source **byte-identical to main** (only `form-eval.fk` + the band changed).
- **All three value kinds landed** — string, list, float. The combine milestone (`g "hi"` → `hi!`) lands.
- **Closures** over a definition-site env remain a further rung (calls evaluate the body under the call-site env +
  params), as before — not needed for these proofs.
- **Four-way:** the band uses only the integer/string/list/float surface all four kernels carry — four-way honest by
  construction. Running it through the Go/Rust/TS walkers (vs. on-`fkwu` `--src`) awaits the same on-platform flatten
  gap named for the other Windows bands.

## Reproduce

```
gcc -O2 -o fkwu.exe runtime/fkwu-uni.c -lws2_32 -lwinmm -lavicap32 -luser32 -lwlanapi -lbthprops -lwinhttp

PRE='(defn char_at (s i) (substring s i (add i 1)))
(defn ord (c) (str_byte_at c 0))'
# the combine milestone — a string value through defn + call -> hi!
( printf '%s\n' "$PRE"; cat grammars/form-eval.fk; printf '\n(str_concat (fe-eval "(do (defn g (s) (str_concat s \\"!\\")) (g \\"hi\\"))") "")\n' ) > /tmp/fe.fk; ./fkwu.exe --src /tmp/fe.fk
# the full band -> 65535
( printf '%s\n' "$PRE"; cat grammars/form-eval.fk; cat grammars/tests/form-eval-band.fk ) > /tmp/band.fk; ./fkwu.exe --src /tmp/band.fk
```
