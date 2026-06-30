# Receipt — two-pass `--src` registration: forward refs resolve, form-eval stands on the C seed (2026-06-29)

**The walk:** the `--src` source-runner registered top-level `(defn ...)` forms ONE pass — each as it
was parsed — so a call to a *later*-defined function missed the function table and lowered to a no-op.
Real Form code is full of forward and mutual references; `grammars/form-eval.fk` is the proof case —
its `fe-expr` (line 32) calls `fe-expr2` (line 33). Fix the registration to two-pass, and the
52-line Form meta-evaluator runs on the c-bootstrapped kernel: `(fe-eval "(add 40 2)")` → 42,
computed by Form recursive-descent over the string, with NO flatten table.

## BUG 1 — forward references fail (`--src` registers defns one-pass)

`fk_parse_top`'s defn handler allocated a fresh fn-index and registered the name *as it reached each
defn body*. So `(do (defn a (n) (b n)) (defn b (n) (add n 1)) (a 5))` parsed `a`'s body while `b` was
not yet in the table — `fk_fn_lookup` missed, the call to `b` lowered to a no-op, and the program
returned `nothing`. Reversing the two defns (b first) returned 6. The bug was purely registration
order, not the walker.

**Fix — TWO-PASS registration (`--src` path only).** A new `fk_prescan_defns()` walks the source
*before* any body is parsed and registers every top-level defn's name + fn-index + arity into the
existing `fk_fnsym_*`/`fk_fnidx`/`fk_fnar`/`fk_defn_next` machinery. Container shape mirrors
`fk_parse_top` exactly: a top-level `(do ...)` is transparent — its inner forms scan as top-level too,
recursively (`fk_prescan_seq`), so defns inside the root `do` register; a bare top-level `(defn ...)`
registers directly; anything else is opaque, skipped as one balanced form (`fk_skip_balanced`). The
pre-scan is read-only over `fk_srctext` and operates on a local cursor (`fk_sskip_at` takes a position
pointer rather than touching the global `fk_spos`). In pass 2, `fk_parse_top`'s defn handler now LOOKS
UP the index already registered for the name (`fk_fn_lookup`) and just fills `fk_fn[idx] = body`,
instead of allocating a new index. Net effect: all names are known before any body lowers → forward
and mutual references resolve. The table-loader (`fk_run`) and `fk_walk` (and its PR #53 hot/cold split
+ `fk_walk_body` TCO trampoline) are untouched.

```
./fkwu.exe --src fwd.fk   # (do (defn a (n) (b n)) (defn b (n) (add n 1)) (a 5))  ->  6   (was: nothing)
./fkwu.exe --src rev.fk   # b-first                                                ->  6
./fkwu.exe --src mut.fk   # mutual a<->b                                           ->  3
```

Edge cases preserved and re-proven: `(do (let p 7) (add p 1))` → 8 (the do-let binding path the
comment at ~line 1064 warns about); `(do (defn g ...) (defn h ...) (let p 21) (g p))` → 42 (let +
forward-ref together); nested-`do` transparency with forward refs → 105.

## BUG 2 — revert the wrong `char_at` alias (PR #52)

`char_at`/`ord` are NOT primitives — they are recipes. PR #52 added `{ "char_at", 2, 28 }` aliasing
`str_byte_at`, which returns a BYTE; but `char_at` must return a 1-char STRING (`form-eval` does
`(ord (char_at s pos))`, expecting char→string then ord→code). The alias was removed from both
`flatten/form-flatten.fk` (the `(list "char_at" 2 28)` row) and `runtime/fkwu-optable.h` (the
`{ "char_at", 2, 28 }` line). `form-flatten.fk` already lowers `char_at` correctly onto
`substring(s, i, i+1)` via `flt-char-at` — the alias contradicted that. The correct shapes are s-expr
recipes:

```
char_at = (substring s i (add i 1))
ord     = (str_byte_at c 0)
ord(char_at "A" 0)  ->  65   (verified)
```

## GOAL — form-eval stands on the C seed (the native walker bootstraps)

`grammars/form-eval.fk` is a Form meta-evaluator: it reads a Form *source string* and computes its
value by recursive descent over the string (no flatten table). Its prelude `core.fk` is surface
grammar `--src` can't parse, so `char_at`/`ord` are supplied as the s-expr recipes above. After BUG 1
+ BUG 2, it runs — every value below is computed by Form, not by `fk_sparse`:

```
(fe-eval "(add 40 2)")                 -> 42
(fe-eval "(sub 100 58)")               -> 42
(fe-eval "(if (le 1 2) 111 999)")      -> 111
(fe-eval "(if (le 5 2) 111 999)")      -> 999
(fe-eval "(add (add 1 2) (add 30 9))") -> 42   ; (= 3 + 39; the prompt's "39" was the inner add, the whole form is 42)
```

The string argument is the proof: `fk_sparse` never sees the string as code — form-eval parses and
evaluates it in Form. form-eval's grammar is `add|sub|le|if` (its `fe-apply` falls back to `add` for
ops outside that set), so e.g. `mul` is out of scope by design; the four ops it claims compute right.

## Validation — all pass

```
forward-ref (a calls later b)            -> 6
(fe-eval "(add 40 2)")                    -> 42   (+ sub/if true/if false above)
native-vs-rented-check                    -> 11111
surprise-receipt-check                    -> 11111
confidence-earned-check                   -> 11111
sum(5000)   non-tail depth                -> 12502500
acc(1000000) TCO                          -> 500000500000
fac(5)                                    -> 120
(add 0.5 0.25)                            -> 0.75
numeric table (1 0 3 3 1 2 0 1 40 ...)    -> 42
GPU c.flat (RTX 4070, PTX-JIT)            -> AGREEMENT 3/3  ALL-BIT-EXACT=true
```

**No regressions.** A baseline kernel built from unmodified `HEAD` (`fkwu_base.exe`) and the new
kernel were swept across every `*-check` in `observe/` and `learn/` (67 checks with a zero-arg check
fn). Every check produced byte-identical output on both kernels — **0 diffs**. The two-pass change
adds capability (forward/mutual refs) without altering any existing result.

## Harvest — old-repo re-sweep

Old form-stdlib (`form/form-stdlib/*.fk`, 734 recipes) was swept baseline-vs-new for `*-check`
recipes whose witness was not already all-1s on baseline but becomes all-1s under two-pass. Of the 93
old recipes carrying a zero-arg `*-check`, **0 crossed** to all-1s under two-pass that weren't already
passing on baseline. No recipe required porting: the two-pass change is a `--src` registration fix;
recipes that already shipped here were unaffected, and the old-repo sweep surfaced no new all-1s
crossing missing from this repo. The capability the fix unlocks (forward/mutual references) is proven
directly by the forward-ref case (→6) and by `form-eval` standing.

## Files

- `runtime/fkwu-uni.c` — `fk_sskip_at`, `fk_skip_balanced`, `fk_prescan_form`/`fk_prescan_seq`/
  `fk_prescan_defns` (new, before `fk_parse_top`); `fk_parse_top` defn handler now looks up the
  pre-registered index; `fk_run_src` calls `fk_prescan_defns()` then re-zeroes `fk_spos` before pass 2.
- `flatten/form-flatten.fk` — removed the `(list "char_at" 2 28)` optable row.
- `runtime/fkwu-optable.h` — removed the `{ "char_at", 2, 28 }` row.
