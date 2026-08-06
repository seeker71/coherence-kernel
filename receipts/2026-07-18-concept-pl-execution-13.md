# 2026-07-18 — thirteen language lenses meet semantic execution

## The movement

The prior language lens proved lexical detection and bounded binary-function
emission. It did not execute the emitted meaning. This lane asks a harder
question: can one concept-conditioned task become real programs that target
toolchains parse and execute, while unavailable carriers remain honestly absent?

The neutral task in `presence/concept-pl-execution-13.fk` is selective retention
under bounded attention:

```text
signals        = [2, 7, 1, 8, 2, 8]
retain at      = 5
score floor    = 100
retained floor = 3
```

Every target program must:

1. loop over the signals;
2. accumulate `score += value * (index + 1)`;
3. count values at or above the threshold;
4. branch on both floors;
5. print `verdict:score:retained`.

The computation yields score `109`, retained count `3`, and:

```text
coherent:109:3
```

That complete output does not occur in any generated source. Each carrier has to
compute it.

## Generation and materialization

Form generates all thirteen full programs from the same task row. The source
files live under `presence/concept-pl-execution-13-fixtures/`:

```text
concept_pulse.py
concept_pulse.js
concept_pulse.ts
ConceptPulse.java
concept_pulse.c
concept_pulse.cpp
csharp/Program.cs + ConceptPulse.csproj
concept_pulse.go
concept_pulse.rs
concept_pulse.rb
concept_pulse.php
concept_pulse.swift
ConceptPulse.kt
```

The host-I/O equality band compares every materialized source byte-for-byte with
`cp13-generate(language, cp13-live-spec())`:

```text
fkwu fixture equality = 8191
```

The pure control also conditions a negative task with threshold `9` and score
floor `200`. Its Form evaluation is `fragmented:109:0`, and the changed values
appear in generated JavaScript and Rust source. This observes conditioning
rather than thirteen static sample strings.

## Carrier audit and live outputs

Python and C were explicitly forbidden for this task, so their installed host
tools were not used. A `javac`/`java` launcher resolves on this Mac but reports
`Unable to locate a Java Runtime`. Kotlin has neither `kotlinc` nor `kotlin`.
Those four rows are `absent`, never syntax-only passes.

| Language | Carrier observation | State | Exact output |
|---|---|---:|---|
| Python | absent by task policy | absent | — |
| JavaScript | Node `v26.0.0` | pass | `coherent:109:3` |
| TypeScript | npx `11.12.1` + tsx | pass | `coherent:109:3` |
| Java | launcher present, runtime absent | absent | — |
| C | absent by task policy | absent | — |
| C++ | Apple clang `17.0.0`, compiled C++17 | pass | `coherent:109:3` |
| C# | .NET SDK `9.0.115`, compiled net9.0 | pass | `coherent:109:3` |
| Go | Go `1.26.3` | pass | `coherent:109:3` |
| Rust | rustc `1.95.0`, optimized compile | pass | `coherent:109:3` |
| Ruby | Ruby `2.6.10` | pass | `coherent:109:3` |
| PHP | PHP `8.5.6` | pass | `coherent:109:3` |
| Swift | Swift `6.2.3`, compiled | pass | `coherent:109:3` |
| Kotlin | compiler/runtime unresolved | absent | — |

The exact live commands were:

```sh
node presence/concept-pl-execution-13-fixtures/concept_pulse.js
npx --yes tsx presence/concept-pl-execution-13-fixtures/concept_pulse.ts
clang++ -std=c++17 presence/concept-pl-execution-13-fixtures/concept_pulse.cpp -o /tmp/.../concept_cpp
/tmp/.../concept_cpp
dotnet run --project /tmp/.../csharp/ConceptPulse.csproj --configuration Release
go run presence/concept-pl-execution-13-fixtures/concept_pulse.go
rustc -O presence/concept-pl-execution-13-fixtures/concept_pulse.rs -o /tmp/.../concept_rust
/tmp/.../concept_rust
ruby presence/concept-pl-execution-13-fixtures/concept_pulse.rb
php presence/concept-pl-execution-13-fixtures/concept_pulse.php
swiftc presence/concept-pl-execution-13-fixtures/concept_pulse.swift -o /tmp/.../concept_swift
/tmp/.../concept_swift
```

Build artifacts were confined to a temporary directory. No generated compiler
state entered the repository.

## Independent controls

The pure concept/generator/carrier control is four-way:

```text
fkwu       32767
Go         32767
Rust       32767
TypeScript 32767
```

It proves:

- the neutral Form evaluator reaches `coherent:109:3`;
- every generated program is nonempty and contains language-appropriate
  loop, accumulation, branch, and output constructs;
- the complete expected output is not canned in generated source;
- the negative task reaches `fragmented:109:0` and changes generated constants;
- the observed carrier registry contains exactly nine present and four absent
  rows, with a reason on every absence.

The frozen live-output ledger in `presence/concept-pl-execution-13-live.fk` is
also four-way:

```text
fkwu       31
Go         31
Rust       31
TypeScript 31
```

It requires thirteen rows, nine exact pass outputs, four absent outputs, an empty
output field on every absence, and a nonempty reason.

## Honest boundary

This lane proves one generated semantic task on nine locally available
toolchains. It does not prove:

- arbitrary program translation;
- semantic equivalence beyond this task family;
- sandboxing or safety of generated programs;
- package/module/dependency generation;
- successful Python, Java, C, or Kotlin execution;
- carrier availability on another host;
- that marker-based language detection is parsing.

The carrier table is a freshness-stamped observation, not a permanent property
of the languages.

## What the attempt taught

Surface diversity became less important once every language had to carry the
same loop, state update, retention decision, and external output. The strongest
evidence was not thirteen files; it was nine independent runtimes converging on
one value while four absences stayed visible.
