# 05 — Natural Language to Recipe

**Discovery**: a tiny English grammar emits Form recipes. The NL surface and
the S-expression surface intern to the **same NodeID** when they describe the
same shape. Two source tongues — one substrate identity.

## Run

```bash
./form/validate.sh \
    form/form-stdlib/core.fk \
    form/form-stdlib/grammar-chars.fk \
    form/form-samples/cross-modal/05-nl-to-recipe/nl-arithmetic-demo.fk
```

Each sentence's recipe is checked `node_eq` against the recipe built by hand
from the same parts, then two structural reads: the square's two children are
one NodeID, and the inner `(mul 2 5)` of `twice 5 plus 3` is the hand-built
one. The aggregate is `6`, the same on fkwu, Go, Rust and TypeScript.

## The four sentences

| English | Recipe shape |
|---|---|
| `the square of 7` | `(mul 7 7)` |
| `the sum of 4 and 6` | `(add 4 6)` |
| `twice 5 plus 3` | `(add (mul 2 5) 3)` |
| `negate 12` | `(sub 0 12)` |

The grammar uses `grammar-chars.fk` directly — the same `cm-parse` plumbing
that
[`form-stdlib/tests/grammar-chars-demo.fk`](../../../form-stdlib/tests/grammar-chars-demo.fk)
relies on for digit-arithmetic. The only addition is a number-word table
(`one`..`twelve`) so spelled-out English numerals resolve to the same int
domain as digit-runs.

## The bonus — content-addressed convergence across modalities

The demo builds `(mul 7 7)` by hand and checks `node_eq` against the
NL-built recipe for `the square of 7`. They are **the same NodeID**,
indistinguishable from each other.

This is the universal-translator promise made concrete for natural language:
the surface tongue varies; the substrate identity is one.

## What's reachable today

- **English sentence -> recipe NodeID** via `cm-parse` + a 4-rule grammar.
- **Cross-modality convergence**: NL `the square of 7` and S-expression
  `(mul 7 7)` intern to the same NodeID, on every kernel.
- **Four-way agreement**: fkwu, Go, Rust and TypeScript build the same
  recipes and print the same observations. No divergence, no mojibake (the
  output is ASCII-clean by design).
- **A recipe's value** is what its Form program computes when fkwu compiles
  it; the recipe itself is identity, observed and never walked.

## What's not reachable yet

- **Open vocabulary.** The grammar handles 4 sentence shapes and 12 number
  words. Anything outside that lexicon falls through to a `-1` sentinel
  rather than a structured parse error.
- **Disambiguation.** `twice 5 plus 3` is `(2*5) + 3`, not `2 * (5+3)`,
  because the rule reads left-to-right with no precedence shape. The
  grammar is right-shape-by-design, not by user intent. Real NL parsing
  needs more rule.
- **Reverse direction (recipe -> NL).** This is the named-pending sibling
  experiment. Walking a recipe back to readable English needs an NL
  emitter the body doesn't carry yet at the arithmetic altitude — the
  `nl-emit.fk` track is i18n surface bindings, not arithmetic generation.

## The teaching

A recipe is content-addressed; the kernel doesn't know or care which
modality wrote it. Two writers — one typing `the square of 7` into prose,
one typing `(mul 7 7)` into a `.fk` file — end up at the same node in
the lattice. The convergence is bytewise, not fuzzy: either the
serializations match or they don't.

This is the same property
[`02-cross-language-content-addressing`](../02-cross-language-content-addressing/)
proved between Python-shape and TypeScript-shape recursive trees, extended
down to the source-text altitude for NL specifically.

Lineage: `lc-parsers-as-recipes`,
`lc-one-kernel-many-tongues`,
`lc-the-kernel-knows-itself`.
