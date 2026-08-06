# fkwu production source boundary

The c-bootstrapped `fkwu --src` runner is the execution authority. Go, Rust,
and TypeScript are differential witnesses for primitive and native behavior;
they are not runtime candidates.

Observed on 2026-07-16:

- the old endpoint path flattened source before invoking a separately emitted
  fourth walker;
- generated endpoint recipes nested loop helper definitions and therefore lost
  outer bindings on that flattened path;
- direct `fkwu --src` rejected those nested definitions and exposed numeric-list
  results as internal cons handles;
- direct-source Form recipes with top-level recursive helpers return the expected
  scalar and list values without flattening;
- `bootstrap/ground-numeric-list.fk` returns `[1, 2.5, [3, 4]]` at stdout;
- the 22 shared endpoint recipes used by Coherence Network compile and execute
  directly on fkwu after the affected loop recipes were lifted.

The C seed grew only at the process-output membrane:  the evaluator already
constructed the numeric lists, while stdout leaked their heap handles. The new
printer recognizes positive cons values and serializes numeric/nested-numeric
lists. No Form operation, call rule, branch, arithmetic rule, or record rule was
added in C.

Shrink debt: move the process-output membrane into a Form-owned value renderer
once the direct source runner can invoke that renderer without first converting
the result back through C. Until then, removing this boundary would make a
correct Form result unusable by its HTTP carrier.

The sibling comparison also showed that fkwu's Form-owned `math_exp` native is a
few ULPs from the CPython/libm result. That remains an explicit primitive
conformance seam. Production does not fall back to a sibling kernel.
