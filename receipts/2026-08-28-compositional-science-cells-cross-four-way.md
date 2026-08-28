# Compositional science cells cross four ways

Signed 2026-08-28 by Codex.

The first science cell began with named quantity labels. This movement folded
those labels into a real SI dimensional algebra: a dimension is the seven
base-exponent cell `(length mass time current temperature amount luminous)`.
Multiplication adds exponent cells, division subtracts them, and an attempted
addition across unequal dimensions returns `nothing`. A mismatched dimension
width also returns `nothing`; it is never padded or silently truncated.

Physics now derives rather than merely labels its output dimensions:

- speed × time becomes length;
- mass × acceleration becomes force;
- `1/2 × mass × speed²` becomes energy.

The chemistry witness changed element identities from strings to atomic
numbers, so water is the explicit composition `[(1,2), (8,1)]` and
conservation is a comparison over caller-declared atoms. The biology surface
now includes DNA complementary pairing as visible relation data: A=1, C=2,
G=3, T=4. The pairing is generic over an input relation, maps ACGT to TGCA,
returns the original strand when applied twice, and returns `nothing` for an
unknown base. The existing one-locus Mendelian model remains visibly narrow.

`form/form-stdlib/tests/basic-science-band.fk` passed clean preflight and
returned `1048575` independently on fkwu, Go, Rust, and TypeScript. Its twenty
claims include direct absence versus zero, three physical equations, water
balance and its counterexample, Mendelian outcomes, dimensional composition,
DNA complement/involution/refusal, and unequal-dimension refusal.

Preflight caught two unclosed forms while the band grew. Tree-heal accepted a
closer for the test fold; the source leak was then located directly and only
kept after every kernel read the repaired source. The band was simplified to a
Form-native recursive sum, leaving no one-use runner in the tree and making the
final fold inspectable.

The next useful question can now enter this algebra as a real model rather
than a label: momentum conservation, reaction coefficients, or a probability
model for inheritance. The surprising teaching was that dimensional meaning is
not a table of special cases—its exponents compose. The parser refusals became
gold by keeping the source shape honest across every kernel.
