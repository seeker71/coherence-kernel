# 2026-07-03 -- form definition language layer review

## Ground

This layer follows the BMF cursor/grammar waist and the current `defdata`
policy:

- `form/form-stdlib/form-definition-language.fk`
- `grammars/form-definition-language.fk`
- `form/form-stdlib/tests/form-definition-language-band.fk`
- `form/form-stdlib/defdata.fk`

The layer language is a scannerless BMF cursor grammar for a small module,
data, and function declaration surface:

```text
module calc {
  data rows = [8,13,21];
  fn bump(x) = add(x,1);
  fn first() = head(rows);
}
```

It is not an s-expression surface, not a line grammar, not a tokenizer, not a
C runtime primitive, and not yet a main source-compiler entry path.

## Pre-Review

Grok pre-review was attempted with a design-only prompt. It began grounding the
review but ended with:

```text
Max turns reached
Error: max turns reached
```

Claude pre-review was attempted with a design-only prompt. It produced no
output after roughly ninety seconds, was interrupted, and returned:

```text
Execution error
```

Those are recorded as review-tool friction, not approval.

## Implementation

`form-definition-language.fk` adds:

- `form-definition-language-manifest`;
- `form-definition-language-grammar`;
- full-source parse through `g-match-rule` plus cursor EOF check, so trailing
  source sediment fails;
- readers for module, data, function, params, and expression nodes;
- lowering to current Form source floor.

The current lowered source shape is:

```text
module calc { data rows = [8,13,21]; fn bump(x) = add(x,1); fn first() = head(rows); }
  -> (let rows (list 8 13 21))
     (defn bump (x) (add x 1))
     (defn first () (head rows))
```

The `form/form-stdlib` and `grammars` copies are intended to remain
byte-identical.

## Investigation

The first lowering target wrapped module content in one top-level `do`:

```text
(do (let rows (list 8 13 21)) (defn bump (x) (add x 1)) (defn first () (head rows)))
```

That was not an honest target. When a final call was appended, the direct
source runner returned `16` instead of the expected `10`:

```text
(do (let rows (list 8 13 21))
    (defn bump (x) (add x 1))
    (defn first () (head rows))
    (add (bump 1) (first)))
-> 16
```

The smallest probes showed the source-runner fault is specifically a
value-bearing top-level `do` where a `let` comes before a later `defn`:

```text
(do (defn bump (x) (add x 1)) (bump 1))                         -> 2
(do (let rows (list 8 13 21)) (defn bump (x) (add x 1)) (bump 1)) -> 8
(do (let rows (list 8 13 21)) (defn two (x) 2) (two 1))           -> 8
(do (defn bump (x) (add x 1)) (let rows (list 8 13 21)) (bump 1)) -> 2
```

Reading `runtime/fkwu-uni.c` points at the temporary source parser route:
after the first value-bearing `let`, `fk_parse_do` parses the later `defn`
through the value parser instead of the top-level defn path. That contaminates
the parser binding frame. This layer does not grow the C seed to repair that
shape because the module language has a better target: actual top-level module
forms. The wrapped-`do` shape is recorded as a source-runner red signal, not
ignored.

The corrected lowering target is:

```text
(let rows (list 8 13 21))
(defn bump (x) (add x 1))
(defn first () (head rows))
```

With a final call appended, that floor returns the expected `10`.

## Witness

Required checkout witnesses before implementation:

```text
ground.fk                -> 42
ground-recursive.fk 10   -> 55
binary-freshness-band.fk -> 15
native-vs-rented-check   -> 11111
```

Layer witnesses:

```text
form-definition-language-band        -> 65535
form-definition-language-floor-band  -> 10
source-runner-admission-band         -> 1048575
form-definition-language copy cmp    -> 0
```

Band bits:

```text
1      manifest declares scannerless
2      manifest declares no-line-grammar
4      manifest declares not-s-expression-surface
8      parses the sample module
16     reads module name calc
32     reads three declarations
64     reads data name rows
128    reads three data values
256    reuses defdata inline policy
512    reads function name bump
1024   reads one bump param
2048   lowers bump body to (add x 1)
4096   lowering kind is form-top-level-source-floor
8192   lowered source matches the top-level Form floor
16384  zero-arg function lowers to (defn first () (head rows))
32768  comments/whitespace parse, trailing sediment fails, empty data lowers
```

No OOM-killed process occurred during this layer pass.

## Post-Review

Grok post-review was attempted with a read-only prompt containing the files,
local witness outputs, and the wrapped-`do` red signal. It began grounding the
review and then ended with:

```text
Max turns reached
Error: max turns reached
```

Claude post-review was attempted with a read-only prompt containing the same
evidence. It produced no output after roughly ninety seconds, was interrupted,
and returned:

```text
Execution error
```

No external post-review approval was obtained. The local witness commands above
remain the available evidence; review-tool failure is recorded separately from
layer correctness.

## Follow-Up Repair

`receipts/2026-07-03-source-runner-top-do-defn-repair.md` later repaired the
specific source-runner red signal where a top-level `do` value `let` before a
later `defn` left the function body unfilled and corrupted do-local binding
state. The form-definition language still lowers to real top-level module
forms, because functions defined inside a value-bearing `do` do not acquire
closure semantics over do-local data.

## Deferred

- Main source-compiler integration. This layer lowers to source text; it does
  not make `fkwu --src` accept the module surface directly.
- Full Form expression syntax, infix precedence, conditionals, blocks, local
  bindings, strings, floats, bools, nested list/data literals, and typed params.
- Keyword boundary hardening beyond the current BMF literal/run behavior.
- Lowering to recipe/image values instead of source strings.
- Closure semantics for functions defined inside a value-bearing `do` after a
  do-local data binding. The current layer avoids that target and proves the
  top-level-module floor instead.
- Program-image `.fkb` and native `.dylib` artifact selectors.
- Full C-seed shrink.
