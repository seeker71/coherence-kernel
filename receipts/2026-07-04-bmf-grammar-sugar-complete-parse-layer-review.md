# 2026-07-04 -- BMF grammar sugar and complete-parse layer review

## Ground

This layer repairs the mismatch between the slim BMF grammar waist and the BML
surface above it.

Touched in this layer:

- `form/form-stdlib/bmf-grammar.fk`
- `grammars/bmf-grammar.fk`
- `form/form-stdlib/tests/bmf-grammar-sugar-band.fk`
- `form/form-stdlib/tests/bmf-grammar-sugar-infix-band.fk`
- `form/form-stdlib/tests/bmf-grammar-sugar-chain-band.fk`
- `form/form-stdlib/tests/bmf-grammar-sugar-ternary-full-band.fk`
- `form/form-stdlib/tests/bmf-grammar-sugar-file-window-band.fk`
- `form/form-stdlib/tests/bml-band.fk`
- `receipts/2026-07-03-core-layer-architecture-map.md`

No C runtime file was changed for this layer.

## Pre-Review

The proposed boundary was reviewed before implementation.

Grok verdict: `PASS_WITH_CHANGES`.

Required changes:

- Keep `g-match-rule` prefix semantics intact and band-proven.
- Add full-parse helpers separately.
- Keep sugar cursor-only and file-window safe.
- Harden the BML witness with arity-before-`nth`.
- Keep `form/form-stdlib` and `grammars` mirrors byte-identical.
- Include file-window sugar/rejection witnesses.

Claude verdict: `PASS`.

Claude agreed that cursor-only sugar belongs above the byte-window/cursor layer,
that full-parse helpers are additive, and that the BML witness should be
hardened without leaking lower-layer details.

## Investigation

The visible symptom was not only a BML failure. It was a layer mismatch plus a
weak witness.

Observed before the repair:

- `bml.fk` called helpers the slim grammar waist no longer provided:
  `p-str`, `p-char`, `p-num`, `p-infix`, `p-chain`, `p-ternary`,
  `t-splice-int`, `t-const-int`, and `t-const-bool`.
- `literals-band` returned `100` in this checkout instead of its documented
  `400`; after sugar restoration it returns `300`, with the remaining missing
  `100` explained by the absent external thesis sample directory already named
  in the previous receipt.
- Current BML shape probing showed A/B examples could become zero-child `do`
  nodes because `unit` is `rep(topdecl)` and `g-parse` did not require EOF.
- The old monolithic `bmf-grammar.fk` could make the historical `bml-band`
  return `268435455`, but that was not enough: the band indexed children before
  proving arity and did not distinguish whole-source parse from prefix success.

The lesson is concrete: `g-match-rule` is a parser-composition primitive and
should remain prefix-oriented; whole-source consumers need a named full-parse
door.

## What Changed

`bmf-grammar.fk` now restores the deferred sugar as grammar-layer behavior, not
as C behavior:

- Literal patterns: `p-str`, `p-char`, `p-num`.
- Expression patterns: `p-infix`, `p-chain`, `p-ternary`.
- Typed templates: `t-splice-int`, `t-const-int`, `t-const-bool`.
- Full-source helpers: `g-complete?`, `g-match-full-rule`, `g-parse-full`.

The sugar uses cursor operations only: `cur-peek`, `cur-advance`, `cur-slice`,
and `skip-ws`. A route scan found no `surf-payload`, `surf-len`, `read_file`,
`file_byte_at`, `read_file_bytes`, or `write_file_bytes` in the grammar files.

`g-match-rule` still accepts prefixes. `g-parse-full` rejects unconsumed source.

`bml-band` now uses `g-parse-full` and a guarded `kid` helper that checks child
length before `nth`, so empty or prefix parses cannot pass by accident.

## Stall / AST-Table Investigation

The first combined sugar witness was not kept.

- A 199-line combined sugar band stalled and was interrupted with exit `130`.
- Reduced diagnostics showed the implementation itself was not stuck: tiny
  literal, infix, chain, ternary, and file-window probes all returned
  immediately.
- A compact combined 88-line witness then failed before execution with:

```text
fk_smknode: program too large for the AST node table
```

That failure was treated as signal, not ignored. The final witness is split
into micro-bands. This keeps direct-source proof under the current AST table
without growing the C seed.

## Witness

Layer-specific bands:

```text
bmf-grammar-sugar-band                  -> 63
bmf-grammar-sugar-infix-band            -> 15
bmf-grammar-sugar-chain-band            -> 15
bmf-grammar-sugar-ternary-full-band     -> 7
bmf-grammar-sugar-file-window-band      -> 7
```

Integration and floor:

```text
bmf-grammar-band                        -> 2047
bml-band                                -> 268435455
grammar-loader-band                     -> 65535
bmf-cursor-language-band                -> 1023
bmf-core-band                           -> 600
bmf-core-file-window-band               -> 32767
form-ontology-parity-band               -> 1497
bootstrap/ground.fk                     -> 42
bootstrap/ground-recursive.fk 10        -> 55
binary-freshness-band                   -> 15
native-vs-rented-check                  -> 11111
cmp grammars/bmf-grammar.fk form/...    -> 0
git diff --check                        -> clean
```

Rebuild:

```text
cc -O2 -o fkwu runtime/fkwu-uni.c
```

Succeeded with only the known warnings:

- `fread` declaration requires `<stdio.h>`.
- `getsockname` pointer sign mismatch.

## Post-Review

Grok verdict: `PASS`.

Grok accepted the cursor-only sugar, preserved prefix semantics, separate
full-match helpers, mirror parity, split micro-bands under the AST-table limit,
and no C-layer changes.

Claude verdict: `PASS`.

Claude accepted the same boundary and called the micro-band split an acceptable
witness restructuring because all sugar bands, existing gates, and the floor are
green.

## Deferred

- `g-parse` remains prefix-oriented for compatibility. Whole-source language
  doors should call `g-parse-full` or `g-match-full-rule`.
- `t-splice-int` is intentionally integer conversion over a captured numeric
  string. Decimal literals are preserved by `p-num` when spliced directly, but
  no general numeric tower/literal policy was added here.
- The old external thesis sample path remains absent in this checkout, so
  real-file BML sample coverage is still environmental.
- The `.fkb`/`.dylib` cached artifact compiler path is not implemented in this
  layer.
