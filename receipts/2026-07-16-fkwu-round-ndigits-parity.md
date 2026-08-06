# fkwu exact-decimal rounding parity — 2026-07-16

## Claim

The c-bootstrapped `fkwu` runtime now owns `round_ndigits` with the same exact-decimal,
half-to-even semantics already carried by the Go, Rust, and TypeScript proof siblings.
The siblings remain comparison witnesses; none is invoked as an execution fallback.

## Defect observed

The former C tag-52 implementation multiplied the input by `10^n` in binary64 before
rounding. That scaling changes the stored side of decimal half boundaries. The direct
runtime returned `1.3808` for `round_ndigits(1.38075, 4)` where CPython and all three
proof siblings return `1.3807`.

## Repair

`runtime/fkwu-uni.c` now renders the complete terminating decimal expansion of a
binary64 value (at most 1074 fractional digits), rounds that digit string half-to-even,
and parses the rounded decimal to the nearest binary64. No sibling process, host-language
adapter, flatten pass, or precomputed answer participates.

This is an explicit C-seed growth of the existing output/native membrane. It is shrink
debt: `form/form-stdlib/tests/round-ndigits-band.fk` owns the semantics, and these C lines
retire when the fkwu native walker carries tag 52 directly.

## Witness

```text
$ cc -O2 -o fkwu runtime/fkwu-uni.c
$ ./fkwu --src form/form-stdlib/tests/round-ndigits-band.fk
24

$ form/validate.sh form-stdlib/tests/round-ndigits-band.fk
✓  round-ndigits-band.fk           → 24
1 ok, 0 divergent — kernels agree on every sample.
```

The first command is the production authority. The validation command cross-checks its
primitive assumption against the proof siblings.
