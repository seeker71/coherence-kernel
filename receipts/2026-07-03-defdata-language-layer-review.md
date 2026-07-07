# 2026-07-03 -- defdata language layer review

## Ground

This layer follows the defdata policy and current source-admission repairs:

- `form/form-stdlib/defdata.fk`
- `form/form-stdlib/defdata-language.fk`
- `grammars/defdata-language.fk`
- `form/form-stdlib/tests/defdata-language-band.fk`
- `receipts/2026-07-03-defdata-admission-current-route-followup.md`

The layer language is a scannerless BMF cursor grammar for a small data-literal
surface:

```text
data rows = [8,13,21]
```

It is not an s-expression surface, not a line grammar, not a tokenizer, and not
a C runtime primitive.

This is still a narrow layer. The current executable checkout surface remains
mostly low-level Form definitions (`let`, `defn`, `list`, direct calls). This
work only proves that constants/data can start from a layer-specific authoring
grammar and lower into that executable floor. It does not yet make the main
language high-level, and it does not yet remove the need to see lowered Form.

## Pre-Review

Grok accepted the scoped design prompt-only: add a BMF cursor grammar that
parses a non-s-expression data declaration and lowers it to the module-constant
target already proven by `defdata.fk`. Grok returned no blockers.

Claude pre-review was attempted through `claude --print`. It produced no output
after more than a minute and was interrupted; the CLI returned
`Execution error`. That is recorded as review-tool friction, not approval.

## Implementation

`defdata-language.fk` adds:

- `defdata-language-manifest`;
- `defdata-language-grammar`;
- full-source parse through `g-match-rule` plus cursor EOF check, so trailing
  source sediment fails instead of being silently ignored;
- declaration readers: `ddl-name`, `ddl-values`, `ddl-value-count`;
- lowering readers: `ddl-lowering-kind`, `ddl-lowering-policy`,
  `ddl-lowering-source`.

The current lowered source shape is:

```text
data rows = [8,13,21] -> (let rows (list 8 13 21))
```

The `form/form-stdlib` and `grammars` copies are byte-identical.

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
defdata-language-band         -> 8191
source-runner-admission-band  -> 1048575
defdata-band                  -> 2047
bmf-grammar-band              -> 2047
grammar-loader-band           -> 65535
source-artifact-cache-band    -> 1048575
defdata-language copy cmp     -> 0
bmf-core copy cmp             -> 0
bmf-grammar copy cmp          -> 0
grammar-loader copy cmp       -> 0
```

Band bits:

```text
1     manifest declares scannerless
2     manifest declares no-line-grammar
4     parses data rows = [8,13,21]
8     reads name rows
16    reads three values
32    reads first value 8
64    reads third value 21
128   policy is inline
256   lowering kind is module-constant
512   lowered source is (let rows (list 8 13 21))
1024  whitespace variant parses
2048  trailing sediment fails
4096  empty list parses and lowers to (let empty (list))
```

No OOM-killed process occurred during this layer pass.

## Post-Review

Claude post-review was attempted through `claude --print`. It produced no
output after more than a minute and was interrupted; the CLI returned
`Execution error`. That remains review-tool friction, not approval.

Grok post-review was attempted several times. Two short supplied-evidence
prompts reached `max turns reached` while trying to verify. A larger
read-only prompt reported, before timing out, that "Band scores don't match
expected values -- investigating causes." Because that could have been either
a real mismatch or a bad invocation, a follow-up asked Grok to list exact
commands, actual outputs, expected outputs, and whether this was a blocker.
That follow-up also ended with only:

```text
Max turns reached
Error: max turns reached
```

No exact mismatch command or output was returned. Therefore this layer has no
external post-review approval. The local witness commands above remain the
available evidence; the unresolved Grok claim is recorded instead of ignored.

## Deferred

- Integrated `defdata` keyword in the real source compiler.
- Non-integer literals, strings, nested data, and recipe declarations.
- Micro-recipe authoring syntax. The policy exists; this grammar only proves
  inline integer lists.
- A broader high-level Form authoring language. `let`/`defn` are still the
  lowered executable floor, not the intended final authoring experience for
  every layer.
- Program-image `.fkb` and native `.dylib` artifact paths.
- Full C-seed shrink.
