# Six pinned public-data task families across thirteen programming lenses

Date: 2026-07-18

## What is real

This increment replaces invented operational fixtures with a reproducible,
committed snapshot of six authoritative public APIs. The network fetch is a
separate refresh operation. Normal gates are deterministic and offline: they
verify each committed raw response by SHA-256, re-derive the JSON task rows and
generated Form cell in memory, and require byte equality.

Snapshot manifest SHA-256:

```text
5b4b8244f67c781eeae8e75a4db2c70a72f58e00d1cf572284f3c4a6fefbdc91
```

All six responses were retrieved at `2026-07-18T06:20:31.999Z`:

| family | authority and fixed query | raw SHA-256 |
|---|---|---|
| streamflow threshold | USGS NWIS, site 01646500, daily discharge 00060, 2024-01-01..10 | `ce0142e8b4ad46beb5b993b9c7406ef6c0c6f50215a63b9f2904f855d415e887` |
| debt reconciliation | U.S. Treasury Fiscal Data, Debt to the Penny, 2024-01-02..05 | `c4d108a835b6243aacf224bb9a1577fda6f2b04539d34133cb1538717cdc5621` |
| orbital interval overlap | NASA Exoplanet Archive TAP, first 100 measured periods with errors | `b27e6b239c1dcabf56a7c9cdd0b61002afab1703a12ab306485485c9e9862c5c` |
| device record completeness | FDA openFDA device events received 2024-01-01..02 | `3e7d40aa0692220632ec1b00309936ba71625bb2598b1c531f40c132bdb16a02` |
| seismic network repetition | USGS Earthquake Hazards, magnitude 2+, 2024-01-01 | `76d35a069dee6c8adf933f59adc7253228521eea515100754e0d9405525370e9` |
| diameter range width | NASA NeoWs feed, 2024-01-01..03 | `f88647971cf9e11125f5af482e9487402fafee48c238ccc92a2237969b675585` |

The full query URLs, authorities, retrieval time, byte counts, filenames, and
hashes live in `presence/fixtures/concept-pl-task-families-source/source-manifest.json`.
The raw responses are committed beside it.

## Transparent derivations and observed values

| concept/family | baseline public-data fields | mutation | baseline -> mutation |
|---|---|---|---:|
| water 377 / streamflow threshold | NWIS Jan 1-3 daily means `[8930,7760,6860]`; Jan 7 threshold `7410` | substitute Jan 9 value `9460` for Jan 3 | `2 -> 3` |
| debt 2594 / debt reconciliation | Jan 2 public/intragovernmental debt, rounded to millions: `[26966334,7023794]`; reported total `33990128` | substitute Jan 3 public-held component `26967954` | `0 -> 1620` |
| schedule 2430 / orbital interval overlap | two Kepler-790 b uncertainty intervals, scaled by 1e8 days: `[1373465781,1373473833]`, `[1373468365,1373476065]` | substitute HATS-24 b interval `[134849717,134849783]` | `5468 -> 0` |
| record 912 / device record completeness | FDA report `9610595-2024-00002` presence flags for event date/type/device name `[1,1,1]` | adjacent report `...00001`, missing event date: `[0,1,1]` | `0 -> 1` missing |
| earthquake 5860 / seismic network repetition | named USGS event network codes `[4,6,2,2]` | replace repeated `hv` event with named `ak` event: `[4,6,2,1]` | `1 -> 0` repeats |
| range 2440 / diameter range width | rounded min/max metres for NASA NEO IDs 2415949, 3160747, 3309828: `[355,794,17,38,200,447]` | next named IDs 3457842, 3553062, 3591616: `[101,226,16,36,43,97]` | `707 -> 199` metres |

These are historical public-data snapshots, not claims of current sensor or
production-system connectivity. Every mutation is another field present in the
same committed authoritative response; none is an invented operational value.

## Reproduction without Python

Offline verification performs no network access:

```sh
node presence/fixtures/concept-pl-task-families-source/build.mjs --verify
# 5b4b8244f67c781eeae8e75a4db2c70a72f58e00d1cf572284f3c4a6fefbdc91 6 verified
```

An intentional refresh uses Node's built-in `fetch`, rewrites raw responses,
manifest, derived JSON, and generated Form, and must be reviewed as a snapshot
change:

```sh
node presence/fixtures/concept-pl-task-families-source/build.mjs --refresh
```

No `curl` executable or Python interpreter is required for snapshot generation.

## Fresh-checkout carrier prerequisites

First build and freshness-check the repository kernel exactly as `AGENTS.md`
requires. Then prepare the external language carriers:

```sh
presence/carriers/concept-pl-task-families-13-bootstrap.sh
```

The bootstrap fails explicitly if Node/npx, Clang/Clang++, .NET, Go, Rust,
Ruby, PHP, or Swift are missing. It invokes the existing checksum-verified
toolchain bootstrap for Temurin JDK `25.0.3+9` and Kotlin `2.3.21` under
`/tmp/coherence-cp10-toolchains`, and preflights exact npm packages:

```text
typescript 5.9.3
tsx        4.20.6
```

The live Form commands use those pinned npm versions directly. Python remains
generation/recovery-only by explicit policy and is never invoked.

## Generated source and native evidence

Baseline and mutation sources are generated in Python, JavaScript, TypeScript,
Java, C, C++, C#, Go, Rust, Ruby, PHP, Swift, and Kotlin. `FKTF13` recovery
regenerates the complete bounded source from source-backed task rows and
requires byte equality; an appended byte is rejected.

C and C++ derive every array length with `sizeof array / sizeof *array`. This
matters for the two-component Treasury vector: the generation gate requires the
derived `a1` length and explicitly rejects the former `budget_ledger(a1,3,...)`
out-of-bounds call.

Both baseline and mutation are independently validated and type-stated before
their separate executions. Observed live totals:

```text
PL lenses                              13
permitted / available carriers         12 / 12
source pairs written / recovered       13 / 13
native parser validations              24 / 24 passed
generator structural IR                13 / 13 present
native AST probes                       6 passed
native AST unavailable states          18 explicit
static typechecks                       18 / 18 passed
dynamic no-static-typecheck states       6 explicit
baseline+mutation executions            24
executed output lines                  144 / 144 exact
executed semantic changes               72 / 72
all-lens semantic changes               78 / 78
Python policy-held rows                   1
carrier failures                          0
```

The six native AST passes are baseline+mutation for C, C++, and Swift. The
other nine executed languages expose `unavailable-no-stable-cli-ast` for both
variants. Generator structural IR is separate and is never promoted to native
AST evidence.

## Gates observed after public-data and C-length fixes

```sh
./fkwu --src presence/tests/concept-pl-task-families-13-band.fk
# 4095

./fkwu --src presence/tests/concept-pl-task-families-13-generation-band.fk
# 8191

./fkwu --src presence/tests/concept-pl-task-families-13-live-band.fk
# 32767
```

The evaluator/router band is independently observed as `4095` by fkwu, Go,
Rust, and TypeScript. The generation band is observed as `8191` by fkwu and Go.
At this receipt's stamp, the Rust and TypeScript proof walkers exhaust their
host call stacks on the 26-source recovery sweep; that is a proof-carrier limit,
not reported as four-way agreement. The twelve native target carriers pass the
live matrix above.

Exact remaining sibling-proof observations from the same generation-band file
list used by fkwu and Go:

```text
cargo run --quiet --manifest-path walkers/rust/Cargo.toml -- <preludes> <generation-band>
thread 'main' (...) has overflowed its stack
fatal runtime error: stack overflow, aborting

npx --yes --package tsx@4.20.6 tsx walkers/ts/main.ts <preludes> <generation-band>
RangeError: Maximum call stack size exceeded
```

## Honest boundary

- This is a bounded six-family public-data generator, not arbitrary source
  parsing, general AST construction, repair, or natural-language synthesis.
- The source APIs are authoritative public sources; the committed rows are a
  pinned historical snapshot, not live production telemetry.
- Only six concepts have reviewed source-backed task inputs. Other 10k routes
  remain deterministic, explicitly unreviewed semantic-fingerprint routes.
- Python is written and exactly recovered, but never parsed or executed.
- Generated C/C++ are target carriers. `runtime/fkwu-uni.c` is unchanged.

The exchange stayed alive by replacing domain-shaped stories with raw public
responses, hashes, dates, and field-level derivations. The surprising teaching
was that a real two-value Treasury row exposed an old three-value assumption
which toy fixtures had hidden. Discomfort turned to gold when that green-looking
C result was recognized as undefined behavior: every C/C++ length is now
derived, and the old call is an explicit negative assertion in the gate.
