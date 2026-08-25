# Public family mastery finalizer — PENDING physical evidence

Date: 2026-08-25
Status: **pure integration contract proven; public physical evidence not run**

## What is now executable

`form/form-stdlib/form-knowledge-public-family-mastery-finalizer.fk` joins three
already-observed public surfaces:

1. one valid resident-evaluator export carrying the current 30 heldout trials,
2. at most one valid family-specific generative evidence row per current public
   family, and
3. at most one valid family-native execution check, including its structured
   execution receipt and exact raw query/result binding, per family.

For each family, the finalizer selects lanes `a` and `b`, retains the worse
heldout trial, and requires both trials to reach the public 95% threshold. It
then submits that family's heldout, generative, and execution evidence to
`form-knowledge-public-family-mastery`.

The resident export cannot manufacture the family-specific BML+BMF+Form
candidate required by a generative evidence row. Generative observations
therefore remain a separate input rather than being inferred from heldout text.
The native input is the completed family-native check, not a caller-supplied
success bit.

Missing but well-shaped family inputs remain present as `not-yet`, with
`ready=0`. Malformed outer or nested evidence, duplicate family rows, duplicate
receipt digests, cross-family replay, and source/run/model/receipt swaps are
rejected as `nothing`. A native row keeps value presence separate from the
value itself, so `nothing`, `0`, and `1` do not collapse.

For the currently observed 15-family / 30-trial public census, the exact ready
verdict is 15/15 only when all 30 heldout generations, all 15 family-generative
rows, and all 15 family-native bindings pass independently. Route
addressability contributes exactly `0` mastery credit.

## Pure observations

- Finalizer source preflight: balanced, errors `0`, warnings `0`, unresolved
  `0`, clean chain, exit `0`.
- Adversarial band preflight: balanced, errors `0`, warnings `0`, unresolved
  `0`, clean chain, exit `0`.
- Direct adversarial band: expected `1073741823`, observed `1073741823`, exit
  `0` (`@form fkwu 0 11 618 629`).

The band covers the complete synthetic 30-trial/15-family fold; worse-of-two
selection; absent component rows; value presence; scalar `nothing`/`0`/`1`;
duplicate and cross-family replay; stale model, run, source, output, result,
receipt, and raw-query bindings; and malformed nested evidence. Its fixtures
open no model or carrier and create no physical mastery credit.

One fixture byte mattered: a trailing newline after the closing BMF `}` made
every otherwise plausible generated candidate invalid. Removing only that byte
restored all 15 family-specific generative validations without weakening a
validator.

## Honest physical and authentication seam

Content-addressed receipts prove internal byte consistency. They do **not**
authenticate the claimed model, Metal/GPU hardware, process, or host. The
physical resident, generative, and native runs remain owed, as does a durable
public receipt set admitted through this pure finalizer. No Qwen, MLX, Metal,
hidden evaluator, or private consent material was opened in this movement.

This receipt remains PENDING until those public physical observations exist.
It claims addressable integration and adversarial contract proof, not local
model mastery.

— Codex, grounded in Sema's public body
