# 2026-07-03 -- stdlib core layer language review

## Ground

The bottom-up stdlib pass starts at `form/form-stdlib/core.fk`.

Current witnesses:

```sh
cat form/form-stdlib/core.fk form/form-stdlib/tests/core-band.fk > /tmp/core-band.fk
./fkwu --src /tmp/core-band.fk
cat form/form-stdlib/core.fk form/form-stdlib/tests/core-str-narrow-waist-band.fk > /tmp/core-nw.fk
./fkwu --src /tmp/core-nw.fk
cat form/form-stdlib/core.fk form/form-stdlib/tests/core-str-find-to-int-band.fk > /tmp/core-find.fk
./fkwu --src /tmp/core-find.fk
cat form/form-stdlib/core.fk form/form-stdlib/tests/core-float-to-str-band.fk > /tmp/core-float.fk
./fkwu --src /tmp/core-float.fk
```

Witness:

```text
255
255
255
63
```

## Pre-Review

Urs corrected the direction for every layer: we are not just adding records.
Each layer needs an appropriate semantic language, domain-specific and clean to
observe at a high level, without losing the details that lower layers carry.
For language surfaces, line grammars and tokenizers are the wrong direction; the
body has the BMF cursor and should use streaming cursor grammars.

Grok and Claude reviewed the core layer from that viewpoint.

Claude's summary-only pre-review found that core already has the right
abstraction: it is the vocabulary floor, not a surface grammar. The correct
core "language" is the direct Form prelude, unshadowed structural primitives,
the narrow native string waist, and queryable Num/List/Cell/Task rows. Claude's
small implementation suggestion was to make the native waist queryable as data.

Grok reviewed the actual file and receipts, re-ran the core witnesses, and
agreed. It named two tensions to document rather than "fix" by adding the wrong
layer: `intern_node_at` is graph/provenance glue living in core for prelude
ergonomics, and Num aliases (`plus`, `minus`, etc.) intentionally bridge core to
the BML/public vocabulary rather than exposing only kernel op names.

## Decision

Layer 1 core stays **direct Form**, not a non-s-expression domain grammar.

That is not a retreat from better grammars. It is the layer boundary: core has no
external text surface to parse. It provides the executable waist and vocabulary
that BMF cursor grammars will consume above it.

## What Changed

- `form/form-stdlib/core.fk`
  - added `core-native-string-waist`, a queryable manifest of the four native
    string operations the kernel must trust: `str_len`, `str_byte_at`,
    `byte_to_str`, and `str_concat`;
  - added `core-composed-string-ops`, naming the string operations that should
    remain Form composition over the waist;
  - added lookup helpers so the trust boundary can be witnessed without reading
    comments.
- `form/form-stdlib/tests/core-waist-language-band.fk`
  - verdict `255`;
  - proves the waist manifest declares exactly four native string operations
    for Form-composed string behavior;
  - proves `substring` is not in the native waist and is instead a composed
    string operation;
  - proves `core-class-count` remains `4`, so this did not smuggle a grammar
    layer into core.
  - does not claim to scan every native the host exposes; for example, `str_eq`
    remains an orthogonal compare primitive and fkwu may still shadow some
    composed names with native implementations while the C seed shrinks.

## Alternatives

| Alternative | Disposition | Why |
| --- | --- | --- |
| Wrap `head`, `tail`, `len`, `nth`, `cons`, `list`, `empty` | Rejected | They are the structural floor. Wrapping them hides the direct kernel path and creates a shadow vocabulary. |
| Add a core-level BMF cursor grammar | Deferred | BMF cursor grammar begins at the grammar layer, where there is a real surface to parse. Core is the executable vocabulary floor. |
| Add a line grammar or tokenizer | Rejected | This violates the streaming cursor direction and creates the wrong abstraction for both core and future language surfaces. |
| Keep native `substring`, `str_find`, `int_to_str`, etc. as a fat string API | Rejected | It grows the native trust surface. The four-op byte waist is enough, and the rest is auditable Form. |
| Split string and graph helpers out of core immediately | Deferred | Cleaner layering may come later, but current bare `--src` preludes depend on one core file. Splitting now would be organizational churn, not a semantic improvement. |
| Make executable core BML-only | Rejected | `core.fk` must run directly under `fkwu --src`; BML/high grammar belongs above the executable floor. |
| Full printf/Unicode string semantics | Deferred | Current core explicitly proves byte-waist and scoped float formatting. Wider semantics need their own layer and witnesses. |

## Witness

```sh
cat form/form-stdlib/core.fk \
    form/form-stdlib/tests/core-waist-language-band.fk > /tmp/core-waist.fk
./fkwu --src /tmp/core-waist.fk
```

```text
255
```

Bit decoding:

```text
1     four waist ops are declared
2     str_len is in the waist
4     str_byte_at is in the waist
8     byte_to_str is in the waist
16    str_concat is in the waist
32    substring is not in the waist
64    substring is in the composed string inventory
128   core still has four semantic classes, not a smuggled grammar class
```

## Deferred

- BMF cursor grammar for semantic/domain surfaces.
- Richer failure language than `assert`'s empty-head failure path.
- Moving graph provenance helpers such as `intern_node_at` into a higher layer
  after preludes can express that split without breaking bare source runs.
- Whole-stdlib ordering beyond core. The next layer should be reviewed before
  implementation rather than inferred from filename order.

## Post-Review

Grok reviewed the implemented layer and receipt read-only, re-ran the witnesses
in this checkout, and found no blocker. Its requested corrections were wording
precision: distinguish the four-op string composition waist from a full native
string-op audit, mention `str_eq` as orthogonal, and close this post-review
section.

Claude reviewed the evidence summary read-only and found no blocker. Its main
request was to decode the `255` bitmask and avoid implying that the composed-op
inventory by itself proves every composed body reduces only to waist ops. This
receipt now records the bit map and treats the composed list as inventory; the
existing string bands remain the body-level witnesses for behavior.
