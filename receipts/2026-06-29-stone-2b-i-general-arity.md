# Receipt — stone 2b-i: fkwu `--src` call path lifts to GENERAL ARITY — the two sense bodies run native (2026-06-29)

**The floor lifted:** the `--src` source path's call machinery handled arity ≤2 (tag 12 = 1-arg, tag 240 =
2-arg, packed into a 4-column node). Helpers with 3+ args — `presence-feature`'s `pf-sad-row` (8 args),
`same-room`'s `sr-same?` (3), `sr-place-loop`/`sr-any-join?` (4) — could not be CALLED via `--src`, so those
recipe bodies returned 0/floor. Stone #26 (multi-function) gave cross-calls and self-recursion at arity 1; this
stone closes the ARITY gap. **No Go, no flatten — proven on `fkwu --src`.**

## What landed (`runtime/fkwu-uni.c`, TCB — the call machinery + the `--src` parser)

One generic mechanism, no per-arity case:

- **Parser (`fk_sparse` call-building).** A registered function call now parses the callee's `ar` declared arg
  expressions (`fk_fnar[fidx]`) into a temp array and threads them head-first into an arg-chain of **tag-242**
  cells (`[1]=arg-expr-node, [2]=next-cell or -1`). The call is ONE **tag-241** node (`[1]=fidx,
  [2]=chain-head or -1`). `ar`=0/1/2/8 are the SAME code — the old `if (ar>=2) tag-240 else tag-12` split is
  gone. The 4-column node limit is escaped by the cons-style chain, not by widening the node.
- **Evaluator (`fk_walk` tag 241).** Walk the 242-chain; for each cell `fk_vp(fk_walk(arg, fp))` (evaluate the
  arg in the CALLER's frame, push it) and count `n`; then `fk_walk(fk_fn[fidx], base)` with the new frame base
  pointing at the first pushed arg; pop `n`. This is **exactly the pack the table path (`fk_walk` tag 240) uses**
  — `fk_vp` onto `fk_vs`, frame-pointer at the args, slot `k` reads `fk_vs[fp+k]` (tag 110) — now for any N.
- **Bonus correctness fix (TCB, the unknown-op fallthrough).** An unknown head (`append`, a core.fk recipe
  `--src` can't read from BML) used to skip to the FIRST `)`, leaving a nested form like `(append (w s) (b s))`
  half-consumed and **silently corrupting every later defn** in the file. The fallthrough now balances parens
  (respecting `"..."` literals) and returns an honest 0 for just that node — so a missing op degrades to a
  clean 0 instead of wrecking the parse. This is what let the full multi-defn sense files parse at all.

Tags 12/240 handlers remain in `fk_walk` (the table path still emits them); the `--src` parser no longer emits
them. Data discipline held: the generic chain in the parser and the generic walk in the evaluator carry any N
with zero hardcoded per-arity `if` chains.

## HARD GATE — all via `fkwu --src` (rebuilt `cc -O2 -o /tmp/fkwu runtime/fkwu-uni.c`)

### THE WITNESS — the two sense bodies now `--src` to CORRECT non-zero values (were 0/floor)

`observe/presence-feature.fk` — the band's four-way-proven verdict is **15**; each claim verified:

```
pf-present? present 4 20   -> 1   (a present frame reads occupied)
pf-present? vacant  4 20   -> 0   (an empty room reads unoccupied)
pf-changed? vacant present -> 1   (empty -> present is an ENTERED event)
pf-changed? present present-> 0   (a still person is NO event)
verdict (1+2+4+8)          -> 15  ✓  matches observe/tests/presence-feature-band.fk (four-way)
```

This exercises 8-arg `pf-sad-row`, 7-arg `pf-sum-row`, 8-arg `pf-sad`/`pf-sum` — all blocked before.
Witness: `observe/tests/src-witness/presence-feature-arity-witness.fk`.

`observe/same-room.fk` — 3-arg `sr-same?` and the 4-arg grouping spine, compared to the hand-computable verdict:

```
sr-same? (same wifi+bt set, floor 30)      -> 1   (same room)
sr-same? (disjoint set,     floor 30)      -> 0   (different room)
verdict same*1 + (1-diff)*2                -> 3   ✓
sr-room-count [A,B same net; C diff] 30    -> 2   ✓  (4-arg sr-place-loop / sr-any-join?, 3-arg sr-add)
sr-why-overlap (same net) 30               -> 100 ✓  (3-arg sr-why -> 4-elem evidence list)
```

`append` (a core.fk list recipe — `--src` cannot parse core.fk's BML) is prepended as the equivalent Form
recipe `(defn append (a b) (if (eq (len a) 0) b (cons (head a) (append (tail a) b))))`; `cs-overlap` then
returns 100 for identical signatures. Witnesses: `observe/tests/src-witness/same-room-arity-witness.fk`,
`same-room-grouping-witness.fk`.

### A 3+-arg call standalone

```
(do (defn sum8 (a b c d e f g h) (add a (add b ... h)))  (sum8 1 2 3 4 5 6 7 8))   -> 36
(do (defn fma (a b c) (sub a (mul b c)))                 (fma 10 2 3))             -> 4
(do (defn order8 (a b c d e f g h) ...)  (order8 7 6 5 4 0 0 0 0))                 -> 4567  (slot order: first arg = slot 0)
```

Witness: `observe/tests/src-witness/sum8-8arg-witness.fk`.

### ZERO REGRESSION (all `--src`)

```
(mul 6 7)                      -> 42
(head (list 11 22 33))         -> 11
(nth (list 4 5 6 7) 2)         -> 6
(str_eq "ab" "ab") / "ac"      -> 1 / 0
native-vs-rented-check         -> 11111   (the real server cell, stone 5)
(g 5)/(h 9)/(c 0) (#26)        -> 12 / 20 / 4   (cross-calls, chained, nested — arity 1)
0-arg / 1-arg / 2-arg call     -> 99 / 42 / 42  (all route through the ONE generic tag-241 path)
```

### Stone 2a intact

```
(nothing? (nothing))  -> 1
(eq (nothing) 0)      -> 0
```

## Indirect calls — honestly NAMED, not half-done

The call head still must be a literal function name (`fk_fn_lookup` at parse time). An INDIRECT call — head is an
expression that evaluates to a function-cell — needs a deeper change than arity: there is **no first-class
function-value** in the kernel today (functions live only as indices in `fk_fn[]`; no boxed fn-value that
`fk_walk` can return and then dispatch on). Adding one means a new tagged value type for an fn-index plus a call
tag that evaluates its head to obtain it — orthogonal to the arity pack. Per the stone's own guidance, general
arity (the bigger unblock) ships alone and the indirect-call gap is named as the next rung, not faked. This is
an UNSUPPORTED capability (a missing value type), not a divergence.

## Honest floor

- General arity is proven on `fkwu --src` (no Go, no flatten). The two real sense bodies that returned 0/floor
  now run to their correct verdicts.
- `append` is a core.fk Form recipe; the `--src` path cannot yet read core.fk (BML), so the same-room witness
  prepends it. Loading the stdlib preludes natively through `--src` is a separate rung (core.fk is BML, not the
  flat `(defn ...)` Lisp the source-runner parses).
- Standard receipt: this is the honest floor (native `--src` run on this Mac), **mac observed**; windows/android
  rows pending; four-way-in-CI is a separate rung (the recipes' four-way proof predates this; the lift here is
  that fkwu's OWN source path now runs them at full arity).
