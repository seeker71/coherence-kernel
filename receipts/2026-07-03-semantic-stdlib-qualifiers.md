# 2026-07-03 -- semantic stdlib qualifiers and residue

## Ground

Before this layer moved, the checkout witness and prior layer bands were clean:

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk
./fkwu --src bootstrap/ground-recursive.fk 10
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk
( cat observe/native-vs-rented.fk; echo '(native-vs-rented-check)' ) > /tmp/nvr.fk
./fkwu --src /tmp/nvr.fk
```

Witness:

```text
42
55
15
11111
```

The prior core layer still held:

```text
defdata band: 2047
homecoming corpus band: 511
constellation lane band: 1023
```

## Pre-Review

The next layer was reviewed before implementation with Grok and Claude.

Grok reviewed the pasted `semantic-stdlib.fk`, its band, and the nearby BMF and
grammar context. It agreed this is the right layer before
`domain-grammar-core`, with a narrow scope: keep `semantic-stdlib` as the
language pivot and translation-evidence layer, not a full domain grammar. Grok
asked for:

- qualified map rows rather than bare `(lang, surface, sem-id)`;
- structured translation results with `ok`, `lossy`, and `missing`;
- explicit residue on lossy paths;
- missing paths that do not guess a surface;
- old `ssl-translate` exact paths kept green where possible.

Claude's pre-review was summary-only after a prior tool-grounding attempt did
not return a useful review. Claude agreed on the layer order and sharpened the
semantic language: residue should be cargo, not a verdict. A lossy translation
that discards residue cannot honestly be reversed. Claude also warned that
`fidelity` must not pretend to be a calibrated metric; it can only mean that all
tracked qualifiers survived, or that tracked residue remains.

Urs then named the wider discipline: each layer needs semantic language that is
appropriate to its domain, clean to observe at a high level, and still able to
carry lower-level details. That became an explicit observation row in this
layer. Urs then corrected the boundary: the observable language for a layer
should not collapse into s-expressions, records, tokenizers, or line grammars.
The parser/emitter surface for this stack belongs in a BMF-cursor grammar layer,
not inside the stdlib pivot itself.

## What Changed

- `form/form-stdlib/semantic-stdlib.fk`
  - `ssl-map-row` now carries qualifier tags.
  - `ssl-translate-result` returns `(status, surface, sem-id, fidelity,
    residue)`.
  - Exact translations return `ok`, `100`, and empty residue.
  - Lossy translations return `lossy`, categorical fidelity `50`, and named
    residue tags.
  - Missing source or target paths return `missing`, `0`, and explicit
    `missing-source` / `missing-target` residue.
  - `ssl-translate` remains a surface-or-missing wrapper for existing exact
    callers.
  - `ssl-observe-translation` returns the internal observation carrier:
    high-level `semantic-translation` status plus lower-level source, target,
    sem-id, fidelity, and residue tags.
- `form/form-stdlib/tests/semantic-stdlib-band.fk`
  - verdict moved from `127` to `8388607`.

## Layer Boundary Contract

`semantic-stdlib` consumes already-identified surface rows:

```text
language + surface form -> semantic id + qualifier tags
```

It does not scan raw source text, tokenize, parse lines, or define a domain
surface grammar. That work belongs below/alongside this layer in the BMF cursor
and BMF grammar stack. This layer owns the semantic operation vocabulary and the
translation evidence returned after a surface has already been recognized.

That is why the implementation is still direct executable Form: this file is
the semantic pivot inside the stdlib, not the high-level authoring syntax for
the pivot. A future non-s-expression semantic surface can lower into these
rows through a BMF cursor grammar without changing the semantic result shape.

## Translation Result Contract

`ssl-translate-result` returns:

```text
(status surface sem-id fidelity residue)
```

Field meanings:

| Field | Meaning |
| --- | --- |
| `status` | `ok`, `lossy`, or `missing`. |
| `surface` | Target-language surface when one is known, otherwise `missing`. |
| `sem-id` | Shared semantic operation id. Missing source gives `0`; missing target keeps the source sem-id. |
| `fidelity` | Categorical label: `100` means all tracked qualifiers survived, `50` means tracked residue remains, `0` means no honest row. It is not a calibrated percentage. |
| `residue` | Cargo describing the source facts not carried by the target row, or why a row is missing. |

Residue taxonomy in this layer:

| Residue | Trigger | Status |
| --- | --- | --- |
| `assign` | Python `=` translated to JavaScript declaration vocabulary loses Python assignment/rebinding shape. | `lossy` |
| `short-decl` | Go `:=` translated to Python loses short declaration shape. | `lossy` |
| `immutable-default` | Rust `let` translated to JavaScript `const` loses Rust's immutable-default/type-inferred declaration detail. | `lossy` |
| `missing-source` | No row exists for the requested source language/surface pair. | `missing` |
| `missing-target` | Source row exists, but no row exists for the requested target language and semantic id. | `missing` |

`ssl-observe-translation` wraps that result in an observation carrier:

```text
("semantic-translation"
 from-lang source-surface to-lang
 status target-surface sem-id fidelity residue)
```

The high-level event remains readable as a semantic translation observation
while preserving enough lower-level detail to inspect the exact semantic id and
residue.

## Witness

```sh
cat form/form-stdlib/semantic-stdlib.fk \
    form/form-stdlib/tests/semantic-stdlib-band.fk > /tmp/semantic-stdlib.fk
./fkwu --src /tmp/semantic-stdlib.fk
```

```text
8388607
```

The band proves:

- prior first-rung translations still work for `+`, function declarations,
  immutable binding, arity, and Form target names;
- `python "+" -> javascript` is `ok` with all tracked qualifiers preserved;
- `python "=" -> javascript` is `lossy` and carries `assign` as residue;
- `go ":=" -> python` is `lossy` and carries `short-decl` as residue;
- `rust "let" -> javascript` is `lossy` and carries `immutable-default` as
  residue;
- unknown source and unknown target paths return `missing` with named residue;
- missing source returns `missing` surface and sem-id `0`, while missing target
  returns `missing` surface and preserves the source sem-id;
- `ssl-observe-translation` exposes a high-level semantic observation while
  preserving the lower sem-id and residue detail.

Bit decoding:

```text
1       python "+" and javascript "+" share semantic add
2       python "+" translates to javascript "+"
4       javascript "function" translates to python "def"
8       javascript "const" translates to Form "let"
16      javascript "const" and javascript "let" are distinct semantic ids
32      python "+" arity is 2
64      rust "fn" maps to Form target name "defn"
128     python "+" -> javascript status is ok with categorical fidelity 100
256     python "+" -> javascript has empty residue
512     python "=" -> javascript status is lossy
1024    python "=" -> javascript carries assign residue
2048    go ":=" -> python status is lossy
4096    go ":=" -> python carries short-decl residue
8192    rust "let" -> javascript status is lossy
16384   rust "let" -> javascript carries immutable-default residue
32768   unknown source returns missing with missing-source residue
65536   unknown target returns missing with missing-target residue
131072  observation kind is semantic-translation with lossy status
262144  observation preserves mutable-bind sem-id and assign residue
524288  unknown source returns surface missing
1048576 unknown source returns sem-id 0
2097152 unknown target returns surface missing
4194304 unknown target preserves source semantic add id
```

## Deferred

- This is not a full programming-language translator.
- The qualifier vocabulary is small and tracked-dimension only; residue is not
  exhaustive over all language semantics.
- Fidelity is categorical (`100` all tracked qualifiers survived, `50` tracked
  residue remains, `0` missing), not a calibrated score.
- `semantic-stdlib` is not yet wired into BMF templates, `grammar-loader`, or
  `emits/semantic-lowerer.fk`.
- The non-s-expression semantic surface grammar is deferred to a BMF-cursor
  grammar layer. This receipt does not claim a line grammar, tokenizer, or
  s-expression surface.
- `domain-grammar-core.fk` remains pending. This layer provides a residue and
  observation shape for that future layer; it does not satisfy the
  field-domain `truth-and-evidence` or `residuals` line items by itself.

## Post-Review

Claude post-review was attempted through the local `claude --bare --print`
CLI. The CLI returned:

```text
Not logged in - Please run /login
```

That is review-tool unavailability, not approval.

Grok post-review was attempted with supplied evidence before this receipt had a
bit legend and explicit contracts. Grok returned `BLOCK`: not for the
implementation shape, but because the supplied evidence did not include the
receipt body and therefore could not prove that the receipt documented the
witness legend, categorical-fidelity language, layer boundary, residue
taxonomy, observation contract, and regression scope.

Those corrections were added, and Grok follow-up passed the corrected 19-bit
packet. Grok also suggested optional missing-path hardening so the documented
`surface` and `sem-id` missing contracts were executable, not prose-only.

That hardening is now implemented. Final local witnesses:

```text
semantic-stdlib-band         -> 8388607
defdata-band                 -> 2047
source-runner-admission-band -> 1048575
defdata-language-band        -> 8191
```

Grok final post-review of the 23-bit packet returned `PASS` with no required
corrections. It accepted that the semantic stdlib layer is a pivot over
identified surfaces, not a parser/tokenizer/line grammar, and that the missing
source/target paths no longer guess surfaces.

No OOM-killed process occurred during this semantic stdlib follow-up. The only
tool failure was Claude CLI authentication unavailability.
