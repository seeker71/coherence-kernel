# Basic science crosses four ways; Rust regains structural equality

Signed 2026-08-28 by Codex.

## What arrived

`form/form-stdlib/basic-science.fk` is a small, executable science surface in
native Form. It does not claim a universal scientific world model. It holds
three inspectable classroom models with their boundary visible:

- labelled quantities, constant-velocity displacement, `F = ma`, and
  `1/2 mv²` in named SI units;
- atom conservation for caller-declared elements in a reaction;
- one-locus Mendelian segregation, explicitly not a model of whole organisms,
  people, or medical reality.

The cell returns `nothing` for a dimensional mismatch. Zero remains a valid
quantity and a valid recessive allele. This is an actual third state in the
model, not a zero-shaped error code.

## The truth seam and its repair

The first cross exposed Rust's minimal proof walker missing `value_eq`, even
though fkwu, Go, TypeScript, and the full Rust kernel already carry structural
equality. Rephrasing the science cell around list width would have hidden that
kernel gap. Instead `walkers/rust/src/main.rs` now carries the same genuine
relation: values compare only within their kind, lists compare recursively,
`nothing` equals itself, and it never equals `0`.

The repair was grounded against the full Rust kernel's existing
`value_equal` implementation. Direct Rust witnesses after its rebuild:

```text
(value_eq (list 1 (list "a" 2)) (list 1 (list "a" 2))) -> 1
(value_eq nothing nothing) + (value_eq nothing 0)        -> 1
```

The second result is `1 + 0`; it carries both truths without collapsing them.

## Witness

Both new cells passed `observe/preflight-run.fk` with balanced parentheses,
zero errors, warnings, and unresolved calls. The same band then returned
`16383` on all four independent arms:

```text
./fkwu form/form-stdlib/tests/basic-science-band.fk                         16383
./walkers/go/walker core.fk basic-science.fk basic-science-band.fk          16383
./walkers/rust/target/release/form-walker-rust core.fk basic-science.fk \
  basic-science-band.fk                                                      16383
node --experimental-strip-types walkers/ts/main.ts core.fk basic-science.fk \
  basic-science-band.fk                                                      16383
```

`cargo build --release` and `cargo test` for the Rust proof walker also exit
cleanly. The four-way band is the end-to-end test of the new primitive and the
new science models; it observes compatible addition, direct `nothing`, zero as
a real quantity, three physical equations, balanced and unbalanced water
formation, and the four / three / one Mendelian outcome structure.

## Next honest extension

Let a real question choose the next model—e.g. reaction coefficients beyond
one declared reaction, conservation of momentum, or a genetics model with
explicit probability and assumptions. Each addition should be a small
observable recipe with its own counterexample, rather than a broad scientific
claim carried only by prose.

The surprising teaching was that one missing equality primitive could tempt a
recipe to inspect representation instead of meaning. The failed Rust arm was
gold: it showed exactly where the truth belonged, and made the shared kernel
more whole rather than making the test smaller.
