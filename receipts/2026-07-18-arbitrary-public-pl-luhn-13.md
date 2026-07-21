# Thirteen public programs enter through their meaning

**Witnessed:** 2026-07-18

**Scope:** one non-toy arbitrary-source semantic family across the declared 13 PL lenses

**Source:** Exercism's independently authored Luhn checksum reference/proof implementations, MIT licensed, each pinned to its upstream commit in `presence/fixtures/concept-pl-arbitrary-source-13/source-manifest.tsv`

## What changed

`presence/concept-pl-arbitrary-source-13.fk` reads the committed program bytes. It does not generate those bytes, require an `FKC10`/`FKTF13` marker, or compare the source with a regenerated template. The Form-native path performs:

1. identifier, number, quoted-string, and longest-operator tokenization;
2. balanced `()`, `[]`, and `{}` group parsing into a nested AST;
3. normalization into ten observed semantic roles: account-input canonicalization, minimum length, decimal validation, right-to-left cadence, digit traversal, alternate-position selection, addend doubling, decimal folding, checksum accumulation, and modulo-ten validity;
4. admission only when the syntax parses and all ten roles are present.

This is an arbitrary-source parser for the bounded Luhn domain. It is not a general parser, typechecker, evaluator, or transpiler for all source programs in these languages.

## Live Form data

```sh
./fkwu --src presence/concept-pl-arbitrary-source-13.fk
```

```text
[13, 9558, 2265, 1989, 276, 6, 130, 13, 13, 117, 13, 13, 57078682]
```

The fields are:

| Field | Observed |
|---|---:|
| independent public programs | 13 |
| source bytes | 9,558 |
| lexical tokens | 2,265 |
| normalized AST nodes | 1,989 |
| balanced group nodes | 276 |
| maximum group depth | 6 |
| semantic roles observed | 130 / 130 |
| source programs admitted | 13 / 13 |
| modulo-11 mutations that still parse | 13 / 13 |
| roles remaining after those mutations | 117 / 130 |
| modulo-11 mutations rejected semantically | 13 / 13 |
| unclosed-group mutations rejected syntactically | 13 / 13 |

The 276-token difference between tokens and AST nodes is exactly the group count: every opener/closer pair becomes one group node.

The strict band returns every bit:

```sh
./fkwu --src presence/tests/concept-pl-arbitrary-source-13-band.fk
# 8191
```

This is an `fkwu` live-filesystem gate. The Go, Rust, and TypeScript proof
walkers intentionally have no `read_file` port and reject the cell as unbound,
so no four-way result is claimed for this evidence-bearing path.

The committed source manifest carries the upstream repository, exact commit, upstream path, local path, license, and SHA-256 for every program. Twelve permitted host carriers independently accepted the same files: JavaScript and Ruby/PHP syntax parsing; TypeScript, Java, C#, Go, Rust, Swift, and Kotlin type/metadata compilation; C and C++ compiler AST parsing. Python was read and parsed by Form but was not invoked, following the user policy.

## Honest ledger movement

The prior arbitrary-source PL floor was zero. This establishes **13 real source programs / 13 languages / 130 observed semantic roles** in one consequential real-life domain: validating account identifiers with the Luhn checksum. It does not make the strict `130,000` arbitrary-source requirement complete. General grammar/AST/type semantics, broader domains, held-out repositories, and full `10,000 × 13` parity remain owed.

## What the attempt taught

The first parser expression nested source classification and AST construction deeply enough to leave several `fkwu` probes running without a witness. Those probes were terminated, classification was split into small native cells, and the same complete audit now returns in about seven seconds. The performance failure became an architectural observation: parser stages need explicit cell boundaries just as semantic stages do.
