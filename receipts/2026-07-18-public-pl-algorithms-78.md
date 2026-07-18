# Public PL algorithms: 78 source programs observed

; witnessed: 2026-07-18 -> 78/78 admitted; 78 semantic mutations rejected;
;                              78 malformed programs rejected

## What is now real

The programming-language lens is no longer one checksum template repeated in
thirteen spellings. It reads six independent algorithm families:

1. Luhn validation
2. binary search
3. matching brackets
4. prime factorization
5. run-length encode/decode
6. robot simulation

Each family is represented by an independent public implementation in Python,
JavaScript, TypeScript, Java, C, C++, C#, Go, Rust, Ruby, PHP, Swift, and
Kotlin: **78 source programs** in total. The 65 newly imported programs are
byte-exact Exercism reference/proof/example sources. Their upstream repository,
pinned commit, original path, local path, MIT license, SHA-256, and byte count
are recorded row by row in
`presence/fixtures/concept-pl-public-algorithms-13/source-manifest.tsv`.

`build.mjs` does not generate substitutes. It rereads all 65 pinned upstream
files, compares their bytes with the committed fixtures, recomputes hashes, and
regenerates the manifest. On this witness it returned:

```text
verified 65 exact public programs
```

## Native observation

`presence/concept-pl-public-algorithms-13.fk` runs on `fkwu`. It reuses and
strengthens the Form-native scanner and balanced-group AST, then lowers each
family to a distinct semantic-role IR. Comment trivia is now tokenized
correctly for `//`, `/* ... */`, and `#` lines. That repair matters: an
apostrophe in a real Kotlin block comment previously became a false
unterminated string and rejected otherwise valid binary-search source.

The six complete family audits from
`presence/concept-pl-public-algorithms-13-audit-live.fk` are below. Columns are:
programs, bytes, tokens, AST nodes, groups, max depth, observed roles, expected
roles, admissions, mutation parses, mutated roles, mutation rejections,
malformed rejections, rolling source hash.

| family | live audit |
|---|---|
| Luhn | `[13,9558,2120,1845,275,6,130,130,13,13,117,13,13,57078682]` |
| binary search | `[13,9169,1938,1688,250,10,104,104,13,13,91,13,13,349947532]` |
| matching brackets | `[13,12559,2616,2255,361,7,104,104,13,13,90,13,13,161512711]` |
| prime factors | `[13,4760,998,868,130,5,104,104,13,13,91,13,13,460565388]` |
| run-length encoding | `[13,14687,3021,2623,398,7,104,104,13,13,91,13,13,960638735]` |
| robot simulator | `[13,26056,5041,4435,606,7,104,104,13,13,91,13,13,25748793]` |

Aggregate observed data: **78 programs, 76,789 source bytes, 15,734 tokens,
13,714 AST nodes, 2,020 balanced groups, and 650/650 family-specific semantic
roles**.

The roles are meaning-bearing rather than marker strings. Examples include
ordered interval narrowing and midpoint selection; LIFO opener retention and
matching; repeated exact factor extraction; adjacent-symbol grouping and
decimal repeat expansion; and cardinal rotation plus grid translation. The
IR therefore carries the data-shape constraints used by each algorithm
(ordered collection/index, bracket context/boolean, integer/factor multiset,
text/run count, and command/pose state), while honestly remaining a bounded
family analyzer rather than a general-purpose thirteen-language typechecker.

## Negative observations

Every source has two independent negative witnesses:

- a syntax-valid, family-specific meaning mutation (modulo 10→11, half-step
  change, mismatch acceptance/pop change, divisibility change, singleton-count
  change, or left-command change) still parses but loses semantic admission;
- an appended unmatched opener is rejected by the balanced-group parser.

Totals: **78/78 semantic mutations parsed and were rejected; 78/78 malformed
sources were rejected**. Matching-brackets loses fourteen roles because one
pop→push mutation removes two linked LIFO obligations; other families lose one
role per program. That measured difference is preserved rather than normalized
away.

## Executable doors

- `presence/concept-pl-public-algorithms-13-live.fk` returns one full source
  observation from each of the six families.
- `presence/concept-pl-public-algorithms-13-audit-live.fk` recomputes every row
  above from all 78 committed source files.
- Six bounded family gates under `presence/tests/` each returned `2047`.

Commands witnessed without Python or the proof-sibling runtimes:

```sh
node presence/fixtures/concept-pl-public-algorithms-13/build.mjs /tmp/exercism-luhn.Sq7M7I
./fkwu --src presence/tests/concept-pl-public-luhn-13-band.fk
./fkwu --src presence/tests/concept-pl-public-binary-search-13-band.fk
./fkwu --src presence/tests/concept-pl-public-matching-brackets-13-band.fk
./fkwu --src presence/tests/concept-pl-public-prime-factors-13-band.fk
./fkwu --src presence/tests/concept-pl-public-run-length-encoding-13-band.fk
./fkwu --src presence/tests/concept-pl-public-robot-simulator-13-band.fk
./fkwu --src presence/concept-pl-public-algorithms-13-audit-live.fk
```

This does not claim arbitrary program understanding, native compilation of all
thirteen host languages, or a general type theorem. It is a reproducible,
licensed, non-toy six-family floor with exact positive and negative data.
