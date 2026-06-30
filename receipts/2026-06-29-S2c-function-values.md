# Receipt — stone 2c: function-VALUES + indirect calls — a fn is a first-class value the reducer can return and dispatch on (2026-06-29)

**The gap stone 2b-i named, now closed.** General arity (2b-i) made any-N calls work, but the
call HEAD still had to be a literal function name resolved at parse time (`fk_fn_lookup`). There
was **no first-class function-value**: functions lived only as indices in `fk_fn[]`, with no boxed
value `fk_walk` could return and then dispatch on. So `(f x x)` where `f` is a *parameter*, a fn
*stored in a var*, or a fn *returned from a fn* could not be called — it declined to `nothing`.
Stone 2c mints the function-value and the indirect-call tag. **No Go, no flatten — proven on
`fkwu --src`.**

## What landed (`runtime/fkwu-uni.c`, TCB — value mint + parser + evaluator)

One reserved-band sentinel, two generic tag handlers, two parser hooks — no new value-op `if`-chain.

- **FUNCTION VALUE (the mint).** `fk_fnval(f) = fk_fnbase - (f<<1) - 1` with
  `fk_fnbase = -8000000000000000000LL` — an **odd-negative reserved-band sentinel**, minted exactly
  like stone-2a's `nothing`, collision-proof by arithmetic. It sits ABOVE `nothing`
  (`-8999999999999999999`) and the float base (`fk_fbase = -9e18`, floats live at-or-below it), and
  BELOW every node/record/cons/int (tiny-magnitude or positive). Detection `fk_is_fnval(v)`: in the
  narrow band `(fk_fnbase-(8192<<1), fk_fnbase]`, odd offset from `fk_fnbase`, recovered index `< 4096`
  — so it is NOT an int (ints `v<<1`, even), not `0`/`1`, not a float (`fk_isf` needs `v<=fk_fbase-2`;
  these are ~`-8e18`, above it), not a node (`fk_nidx` maps ~`8e18` far past `fk_np`), not a record
  ((`0-v`) is odd here, records even), not `nothing` (distinct constant). `fk_fnval_idx` recovers the
  fn-index.
- **Bare fn-name in VALUE position → tag 243 (`fk_sparse`).** A bare symbol that is not a bound name
  but IS a registered fn-name lowers to a **tag-243** node carrying the fn-index; `fk_walk` returns
  `fk_fnval(idx)`. A bound name still wins (lexical scope shadows). This is what lets a fn be passed as
  an arg, stored in a `let`, or returned from a fn.
- **INDIRECT CALL → tag 244 (`fk_sparse` + `fk_walk`).** A call `(h args..)` whose head `h` is a BOUND
  NAME (a parameter, or a let-var holding a returned fn) lowers to **tag-244**: `[1]`=head-expr (a
  tag-110 slot read), `[2]`=arg-chain (the SAME head-first tag-242 cells the direct tag-241 path uses).
  At eval, tag-244 evaluates the head to a fn-VALUE, extracts the fn-index, and offers the fn with the
  args — **axiom-5: offer a COMPUTED cell.** If the head does not evaluate to a fn-value, the offer
  acks `nothing` (a cell that can't answer declines). Under `FK_OBSERVE=1` the indirect offer traces a
  distinct `offer-indirect fn<i> args=<n> (computed head)` line before the normal post-call
  `offer fn<i> ... ack=<arm>`.

Direct calls (tag 241/240/12) are untouched: a literal fn-name still resolves at `fk_fn_lookup` into
the direct path. Tag 243/244 join the generic `if (t == N)` dispatch like every other tag — no
symbol-string comparison in the value path, no per-name C case.

## HARD GATE — all via `fkwu --src` (rebuilt `cc -O2 -o /tmp/fkwu-s2c runtime/fkwu-uni.c` from the worktree)

### Indirect call — the witness

```
(do (defn plus (a b)(add a b))(defn apply2 (f x)(f x x))(apply2 plus 5))            -> 10   ✓
(do (defn dbl (a)(add a a))(let g dbl (g 7)))                                       -> 14   ✓  (fn in a var, then called)
(do (defn dbl (a)(add a a))(defn pick (n) dbl)(let g (pick 0)(g 9)))                -> 18   ✓  (fn RETURNED from a fn, then called)
(do (defn inc (a)(add a 1))(defn dec (a)(sub a 1))
    (defn choose (f)(if (eq f 1) inc dec))(let op (choose 1)(op 41)))               -> 42   ✓  (fn returned conditionally)
```

### foldr with a fn-VALUE over a list

```
(do (defn foldr (f z xs)(if (eq (len xs) 0) z (f (head xs)(foldr f z (tail xs)))))
    (defn add2 (a b)(add a b)) (foldr add2 0 (list 1 2 3 4 5)))                      -> 15   ✓
    ... (defn mul2 (a b)(mul a b)) (foldr mul2 1 (list 1 2 3 4)))                    -> 24   ✓
```

### Bare fn-name in value position IS a fn-value (tag 243)

```
(do (defn plus (a b)(add a b)) plus)   -> -8000000000000000003   (= fk_fnval(1), the odd-negative sentinel)
```

### Indirect call to a NON-fn-value head declines cleanly (axiom-5)

```
(do (defn f (x)(let g 7 (g x)))(f 5))  -> nothing   (the offered head is 7, not a fn — a cell that can't answer acks nothing)
```

### ZERO REGRESSION (all `--src`)

```
(mul 6 7)                      -> 42
(head (list 11 22 33))         -> 11
(nth (list 4 5 6 7) 2)         -> 6
(str_eq "ab" "ab") / "ac"      -> 1 / 0
(nothing? (nothing))           -> 1     (eq (nothing) 0) -> 0   (stone 2a intact)
observe/native-vs-rented.fk    -> 11111 (the real server cell, stone 5)
sum8 / fma / 0-arg / 1-arg / 2-arg -> 36 / 4 / 99 / 42 / 42   (stone 2b-i arity path intact)
src-witness/*.fk (presence-feature / same-room / same-room-grouping / sum8) -> 15 / 3 / 2 / 36
```

### DATA discipline held (grep)

```
grep 'fk_sym_eq([^)]*"fn"\|"fnval"\|"apply"' runtime/fkwu-uni.c   -> (none)
fn-values ride the EXISTING generic symbol resolution (fk_fn_lookup / fk_bd_lookup) and the generic
tag dispatch (243/244). No new hardcoded value-op if, no per-name C case.
```

## CLOSURE — the NAMED next gap (not half-done)

The fn-value carries **only the fn-index** — there is no captured environment cell on it yet. So a fn
that closes over a free variable from its defining scope cannot yet be returned with that binding
intact; only top-level `defn`s (whose entire scope is their args) round-trip as values. A closure needs
the fn-value to box `(fn-index, captured-env-frame)` and the indirect call to restore that frame — a
strictly larger value type, orthogonal to this mint. Per the stone's own guidance, function-values +
indirect-call (the bigger unblock) ship alone and closure is named as the next rung, not faked. This is
an UNSUPPORTED capability (a missing env-cell on the value), not a divergence.

## Honest floor

- Function-values + indirect calls are proven on `fkwu --src` (no Go, no flatten) — the `(f x)` /
  `(f x x)` family that stone 2b-i's receipt named as the next rung now runs to correct values.
- Standard receipt: this is the honest floor (native `--src` run on this Mac), **mac observed**;
  windows/android rows pending; four-way-in-CI is a separate rung. The lift here is that fkwu's OWN
  source path now treats a function as a first-class value it can return and dispatch on.
