# 2026-07-03 -- Grammar loader layer review

## Ground

Layer 4 is the ontology-free named grammar registry:

- `form/form-stdlib/grammar-loader.fk`
- `grammars/grammar-loader.fk`
- `form/form-stdlib/tests/grammar-loader-band.fk`

Layer 2 closed the streaming BMF cursor. Layer 3 closed the recursive BMF
grammar waist. This layer adds the smallest higher abstraction over those two:
a caller can register grammar values under names and parse source through a
selected grammar.

This is not yet a domain grammar shell, source ontology loader, line grammar,
or tokenizer. Those are higher integration layers.

## Pre-Review

Claude reviewed the layer summary before implementation and agreed that
`grammar-loader` is the correct next file only if it is scoped as an
ontology-free registry over BMF grammar values. Claude asked that
`form-ontology-loader` and `line-grammar` be removed from the advertised
prelude, that the witness use helper functions rather than a large local-value
probe, and that missing grammar and parse-failure sentinels both be tested.

Grok reviewed against the repository. It reproduced the current boundary:

```text
./fkwu --src form/form-stdlib/form-ontology-loader.fk
fk_smknode: program too large for the AST node table
```

and confirmed that the slim loader stack loads:

```text
core.fk + bmf-core.fk + bmf-grammar.fk + grammar-loader.fk -> 0
```

Grok also confirmed the two `grammar-loader.fk` copies were byte-identical
before the layer change. Its recommendation was to close this layer as a named
registry, not as an ontology/domain integration layer.

## Investigation

The previous layer receipt warned that `grammar-loader` could not be claimed
green until the `core + form-ontology-loader + bmf-core + bmf-grammar`
composition fit direct source. The follow-up check sharpened that statement:
`form-ontology-loader.fk` fails by itself with the AST table error, before
`grammar-loader` is involved. That makes ontology integration a red upstream
track, not a reason to keep this registry layer blocked.

Later source-runner repairs closed that upstream AST failure. The current
composition checks now fit direct source:

```text
core + form-ontology-loader + bmf-core + bmf-grammar                  -> 0
core + form-ontology-loader + bmf-core + bmf-grammar + grammar-loader -> 0
```

That does not change the layer boundary: `grammar-loader` is still a named
registry layer, not an ontology/domain integration layer. It does remove the old
current-red upstream blocker from the receipt.

The slim stack:

```sh
./fkwu --src <(cat form/form-stdlib/core.fk \
    form/form-stdlib/bmf-core.fk \
    form/form-stdlib/bmf-grammar.fk \
    form/form-stdlib/grammar-loader.fk)
```

returned:

```text
0
```

Several scratch probes that held grammar values, registries, parse outputs, and
result checks in one long local `let` body produced misleading or clobbered
results. The layer witness therefore rebuilds small grammar and registry values
through helper functions. The lesson is practical: when a layer is about value
shape and the runtime still has source/AST pressure, a witness should avoid
retaining more local intermediate values than the behavior under test requires.

## What Changed

Both mirrored loader files now advertise the correct layer:

- `grammar-loader.fk -- ontology-free named registry over BMF grammars`
- prelude: `core.fk`, `bmf-core.fk`, `bmf-grammar.fk`
- explicit no-ontology-loader, no-line-grammar, no-tokenizer boundary

The layer now exposes an observable language manifest:

- `named-registry`
- `immutable-register`
- `shadowing`
- `parse-with`
- `parse-by-name`
- `ontology-free`
- `no-line-grammar`

The existing registry functions remain the layer API:

- `gl-empty`
- `gl-register`
- `gl-find`
- `gl-has?`
- `gl-count`
- `gl-names`
- `gl-parse-with`
- `gl-parse`
- `gl-ok?`

The new band tests those functions against a tiny `alpha` grammar, a shadowing
literal grammar, a missing grammar, and a parse-failure case. Its first four
bits are manifest/invariant checks; the behavioral checks start at bit `16`.
The `immutable-register` feature is inspected directly in `gl-register` using
`cons`, but it does not yet have a separate behavioral bit beyond the registry
count and shadowing checks.

Important boundary: `gl-parse-with` and `gl-parse` delegate to `g-parse`, the
prefix-oriented Layer 3 parse door. Layer 4 proves named registry and
parse-by-name routing; it does not claim whole-source admission through
`g-parse-full`.

## Alternatives

| Alternative | Disposition | Why |
| --- | --- | --- |
| Keep the old fat advertised prelude | Rejected | It overclaims this layer and points at `form-ontology-loader`, which is red by itself. |
| Treat `form-ontology-loader` as a dependency of this layer | Rejected | The loader registry can be witnessed without ontology; FOL is a separate shrink/loading problem. |
| Inline all grammar values at call sites | Rejected | The named registry is the useful abstraction for selecting among source languages. |
| Fix shell/domain grammars in this layer | Deferred | Domain grammars sit above the loader and should be reviewed as their own language layers. |
| Increase the C seed AST cap | Rejected | That would grow the temporary C checkout witness and hide a real source-shape boundary. |
| Use a long local-retention probe as the band | Rejected | It confuses witness pressure with loader behavior. Helper functions make the tested behavior narrower. |

## Witness

Loader-only stack:

```sh
./fkwu --src <(cat form/form-stdlib/core.fk \
    form/form-stdlib/bmf-core.fk \
    form/form-stdlib/bmf-grammar.fk \
    form/form-stdlib/grammar-loader.fk)
```

```text
0
```

Layer band:

```sh
./fkwu --src <(cat form/form-stdlib/core.fk \
    form/form-stdlib/bmf-core.fk \
    form/form-stdlib/bmf-grammar.fk \
    form/form-stdlib/grammar-loader.fk \
    form/form-stdlib/tests/grammar-loader-band.fk)
```

```text
65535
```

Bit decoding. Bits `1` through `8` are manifest/invariant checks; bits `16`
through `32768` are behavioral registry and parse checks.

```text
1      manifest declares named-registry
2      manifest declares parse-by-name
4      manifest declares ontology-free
8      manifest declares no-line-grammar
16     empty registry count is 0
32     one registered grammar count is 1
64     gl-has? finds the registered name
128    gl-names exposes the registered name
256    gl-parse-with parses "hello"
512    gl-parse by name parses "hello"
1024   missing grammar returns NO-SUCH-GRAMMAR
2048   gl-ok? rejects missing grammar sentinel
4096   bad source returns PARSE-FAIL
8192   gl-ok? rejects parse-failure sentinel
16384  newest registered grammar shadows older grammar
32768  adding a second name gives count 2
```

Copy integrity:

```sh
cmp -s grammars/grammar-loader.fk form/form-stdlib/grammar-loader.fk; printf '%s\n' $?
```

```text
0
```

Historical red, now resolved by later source-runner repairs:

```sh
./fkwu --src form/form-stdlib/form-ontology-loader.fk
```

```text
fk_smknode: program too large for the AST node table
```

Layer 3's ontology-composition warning now applies to integration stacks that
actually include `form-ontology-loader`, not to this slim registry layer. The
registry could close green while the ontology loader was red on its own track;
later source-runner repairs made the ontology-loader composition fit direct
source, without changing the Layer 4 boundary.

## Deferred

- Shrink or redesign `form-ontology-loader.fk` so it fits direct source.
- Decide whether ontology loading comes through smaller source cells,
  generated/loadable data, a non-source image path, or a different native
  walker route.
- Domain grammar files that still carry stale fat preludes.
- Shell/source language integration above the loader registry.
- Expression sugar was deferred from the BMF grammar waist and is now handled as
  later grammar-sugar layer work.
- Stable ontology-backed NodeID equality and source-attributed emission.
- Larger `rep`/`sep` performance work from the BMF grammar layer.
- Runtime local-retention robustness. This band avoids the issue; it does not
  claim to solve it.

No OOM-killed process occurred in this layer pass. The red failure encountered
here was the deterministic AST table error shown above, and it was investigated
rather than ignored. That red failure was later repaired in the source runner.

## Post-Review

Claude post-reviewed the implemented files, witnesses, and receipt read-only.
Claude's verdict was green: close layer 4 as an ontology-free named registry.
It found no blockers and agreed that testing `form-ontology-loader.fk` in
isolation converted the old composition concern into a separate upstream track.
That upstream track is now green after later source-runner repairs. Claude
required no code change, but asked that the receipt distinguish self-declared
manifest bits from behavioral bits and avoid implying that `immutable-register`
has a dedicated behavioral witness.

Grok post-reviewed the same layer read-only and also returned green. Grok found
no blocker for closure at this boundary and agreed that FOL shrink, domain
grammars, and fat-prelude cleanup belong to later integration layers. Grok asked
for three documentation corrections: replace the pending post-review note, add
the manifest-versus-behavior distinction, and explicitly cross-link layer 3's
ontology warning as an integration-stack concern rather than a blocker for this
slim registry.

Those corrections are now recorded here and the bit legend is also carried in
`form/form-stdlib/tests/grammar-loader-band.fk`. Layer 4 is closed as the
ontology-free named grammar registry, not as the FOL/domain grammar stack.

## 2026-07-04 Validator Correction

After the Layer 2/3 proof-boundary corrections, the direct Layer 4 bundle still
returned `65535`, but the repository validator was red:

```sh
cd form && ./validate.sh form-stdlib/tests/grammar-loader-band.fk
```

The failure was not an OOM/killed process and was not ignored. Go/Rust/TypeScript
reported `grammar-loader-language-manifest` as unbound because the band used a
prose `; Prelude:` block instead of the validator-readable `; preludes:` header.

The corrective patch stayed inside the band:

- replaced the prose prelude block with one `; preludes:` line;
- replaced the nested `add` score ladder with `sum (list ...)`, matching the
  sibling-safe band pattern used in the Layer 3 correction;
- moved the close boundary so `grammar-loader-band-probe` ends before the
  top-level `(grammar-loader-band-probe)` call. The first close attempt made
  Go/Rust return closures and TypeScript report `defn: expected )`; that was
  investigated as a source-shape bug, not ignored.

Corrected gate:

```sh
cd form && ./validate.sh form-stdlib/tests/grammar-loader-band.fk
# -> 65535
```

Direct `fkwu` bundle:

```sh
./fkwu --src <(cat form/form-stdlib/core.fk \
    form/form-stdlib/bmf-core.fk \
    form/form-stdlib/bmf-grammar.fk \
    form/form-stdlib/grammar-loader.fk \
    form/form-stdlib/tests/grammar-loader-band.fk)
# -> 65535
```

Neighbor and mirror checks:

```text
cd form && ./validate.sh form-stdlib/tests/bmf-grammar-band.fk -> 2047
cmp form/form-stdlib/grammar-loader.fk grammars/grammar-loader.fk -> 0
```

Pre-review:

- Grok/Jason returned `PASS`, with no extra required changes. Grok confirmed the
  loader boundary remains honest and found no Layer 2/3 byte-window or
  high-byte/multibyte claim leaking into Layer 4.
- Claude/Popper returned `PASS_WITH_CHANGES`. Claude approved the narrow band
  patch and required this receipt to name the `g-parse` versus `g-parse-full`
  boundary, mark the old FOL red block as historical/resolved, and point the
  expression-sugar deferral at later grammar-sugar work.

Post-review:

- Grok/Jason returned `PASS`. Grok confirmed the band now closes the probe
  function before the top-level call, that paren balance is clean, and that the
  receipt records both deterministic failures: prelude header drift and the
  closure/TypeScript parse boundary.
- Claude/Popper returned `PASS`. Claude confirmed the validator drift is
  closed without changing loader semantics, and that the receipt keeps the
  Layer 4 boundary as ontology-free named registry rather than whole-source
  admission.
