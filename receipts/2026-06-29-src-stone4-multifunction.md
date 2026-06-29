# Receipt — stone 4: fkwu runs MULTI-FUNCTION Form (cross-calls, self-recursion) — no Go walker (2026-06-29)

**The correction (Urs):** we still have a go-walker even after proving we don't need one. Right — the Go walker is
a proof sibling, never the runtime; leaning on it to "run the server" dodged the real requirement: **`fkwu` itself
runs the Form, via its own native source path** (`--src` -> `fk_walk`), no Go, no flatten, no T_flat.

`--src` could run only ONE function (`fn[0]`, single arg, self-call via tag 7). The server cells are
multi-function with cross-calls. This stone closes that: a function table — each top-level `(defn ...)` gets its
own index; every call (including self) lowers to **tag 12** (call-by-index); a non-defn top form is the root.

## Witnessed native on Windows 11 — `fkwu --src` (NO Go)

```
(do (defn f (n) (add n 1)) (defn g (n) (mul (f n) 2)))  (g 5)         -> 12   (g calls f)
(do (defn inc..)(defn dbl..)(defn h (n) (dbl (inc n))))  (h 9)        -> 20   (chained cross-calls)
(do (defn a..)(defn b (n)(a (a n)))(defn c (n)(b (b n)))) (c 0)       -> 4    (nested cross-calls)
fac(5) self-recursive                                                 -> 120  (self now via tag 12, not fn[0])
```

Regression clean: stones 1-3 (`(add 40 2)`=42, `do`/`let`=21) and the numeric-table path all run.

## What landed

`runtime/fkwu-uni.c` (`--src` only — the c-bootstrap source-runner): a function table
(`fk_fnsym/fk_fnidx/fk_fn_lookup`); `fk_parse_top` (a top-level loop — `(do ...)` transparent, `(defn ...)`
registers a function at its own index, else the root); `fk_run_src` registers all defns then runs the root
(falls back to the single/last defn for the staged-arg stones 1-2). The old `fn[0]`-only self-call (tag 7) is
dropped — every call routes through the table, so self-recursion and cross-calls share one path (one engine).

## Honest floor — what remains to run the REAL server cell (native-vs-rented.fk -> 11111)

Stone 4 is the multi-function spine. The committed cell also needs, and this does NOT yet have:
- **Lists** — `(list 9 7)` / `head` / `tail` (tags 18/19/20/21) in the parser.
- **Multi-arg calls** — `nvrk (cond bit)` is 2-arg; tag 12 pushes ONE arg, so N-arg calls need the frame-pack
  (push all args, `fp = vsp - n`).
- **`gt`** — lower to `if (le ...) 0 1`.

Those are the next stone (5); then `native-vs-rented-check` runs on fkwu and must match the Go walker's `11111`.
The point stands: the path is fkwu's own source-runner maturing, NOT the Go walker.

## Reproduce

```
gcc -O2 -o fkwu.exe runtime/fkwu-uni.c -lws2_32 -lwinmm -lavicap32 -luser32 -lwlanapi -lbthprops -lwinhttp
printf '(do (defn f (n) (add n 1)) (defn g (n) (mul (f n) 2)))\n(g 5)\n' > mf.fk
./fkwu.exe --src mf.fk    # -> 12
```
