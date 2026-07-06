# 2026-07-03 -- make_nodeid direct-source parser repair

## Ground

This is a temporary C checkout-witness repair, not a new runtime home.
`runtime/fkwu-uni.c` remains a shrink target. The fix does not increase
`FK_AST_NODE_CAP`.

Before the patch, the required checkout witnesses were fresh:

```text
./fkwu --src bootstrap/ground.fk                              -> 42
./fkwu --src bootstrap/ground-recursive.fk 10                 -> 55
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk  -> 15
./fkwu --src <native-vs-rented concat>                        -> 11111
```

## Root Cause

The red signal looked like a capacity problem:

```text
./fkwu --src form/form-stdlib/form-ontology-loader.fk
-> fk_smknode: program too large for the AST node table
```

A tiny repro showed it was not file size:

```text
(make_nodeid 1 2 3 4)
-> fk_smknode: program too large for the AST node table
```

The direct-source parser's generic optable path had only three child slots:
`c1`, `c2`, and `c3`. The optable row for `make_nodeid` is fixed arity 4 with
tag 91, so the parser read only the first three operands and left the fourth
operand in the source stream. The later AST-cap error was a misleading symptom
of malformed/unconsumed source.

The evaluator for tag 91 already expected a different shape: child 1 evaluates
to a runtime cons-list carrier of four values `(pkg level type inst)`. So the
right checkout-witness repair was not a fourth AST child and not a bigger AST
table. It was to lower direct-source `make_nodeid` into the carrier shape that
tag 91 already reads.

## Review

Grok pre-reviewed the diagnosis and repair direction prompt-only. It
conditionally accepted the repair if the branch was scoped to tag 91 /
`make_nodeid`, if arity-0 through arity-3 primitives stayed unchanged, if the
cons carrier order was verified, and if the change was recorded as a temporary
C checkout-witness repair.

Claude pre-review was attempted twice. The first prompt was malformed by shell
backtick substitution; the second clean prompt stalled for more than a minute
with no output and was interrupted. That is recorded as a review-tool failure,
not approval.

## Post-Review

Grok post-reviewed the implemented repair prompt-only from the reported
evidence and accepted it. Its review found no blocker and agreed that the fix is
proportionate: scoped to tag 91, no AST-cap growth, arity `0..3` preserved,
required witnesses green, and known `bp`/host-script debt recorded rather than
hidden.

Grok requested framing corrections now carried in this receipt:

- this is temporary C-seed checkout-witness work, not a permanent runtime home;
- if more fixed-arity list-carrier primitives appear later, prefer a small
  table/helper shape over accumulating unrelated one-off tag branches;
- keep the native `bp` semantic gap as documented debt, not a silent pass.

Claude post-review was attempted with a clean prompt. It produced no output for
more than a minute and was interrupted. This remains a Claude review-tool
failure, not approval.

## Implementation

`runtime/fkwu-uni.c` now has `fk_parse_fixed_list(n)`, which parses exactly
`n` operands into tag-19 cons cells ending in tag-18 nil.

The generic optable branch is unchanged for arity `0..3` and variadic rows. A
special scoped branch handles only `tag == 91 && ar == 4`:

- parse four operands into the cons-list carrier;
- consume the single closing paren;
- emit `fk_smknode(91, carrier, 0, 0)`.

This repairs the current checkout witness. It does not make low-level Form
constructor authoring the north star, and it does not solve the larger need for
a higher-level source/lowering surface.

If a later primitive needs the same fixed-list carrier shape, it should be
added through a small explicit table or manifest-driven lowering rule, not by
letting the C seed accrete unrelated special cases.

## Witness

Rebuild:

```text
cc -O2 -o fkwu runtime/fkwu-uni.c
```

The same two pre-existing compile warnings remain: the `fread` declaration
warning and the `getsockname` pointer-sign warning.

Minimal direct-source probes:

```text
(make_nodeid 1 2 3 4)                                      -> -3
(node_pkg/node_level/node_type/node_inst folded as 4321)    -> 4321
(node_type + node_inst for make_nodeid 1 2 99 10)           -> 109
(make_nodeid 1 2 3)                                        -> -3
(if true 7 9)                                               -> 7
```

The first and last `make_nodeid` values print as node IDs; the field probes are
the semantic witness.

Former AST-cap repros:

```text
./fkwu --src form/form-stdlib/form-ontology-loader.fk       -> 0
core + form-ontology-loader + bmf-core + bmf-grammar        -> 0
```

Existing layer witnesses still held:

```text
bmf-cursor-language-band    -> 1023
bmf-grammar-band            -> 2047
grammar-loader-band         -> 65535
source-runner-admission-band -> 1048575
```

Required witnesses after rebuild:

```text
./fkwu --src bootstrap/ground.fk                              -> 42
./fkwu --src bootstrap/ground-recursive.fk 10                 -> 55
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk  -> 15
./fkwu --src <native-vs-rented concat>                        -> 11111
```

Static hygiene:

```text
git diff --check -> clean
```

## Honest Debt Exposed

This repair removes the fake AST-cap blocker. It does not prove the ontology
loader is semantically complete. After the AST repair, direct probes show the
native `bp` table in this checkout returning undefined coordinates for names
like `add` and `JSON-OBJECT`; those fold to `0`, not meaningful ontology
coordinates. That is now visible instead of hidden behind the parser failure.

Urs also named the contradiction around Bash/Python debt. The audit found:

```text
tracked .py files -> 71
tracked .sh files -> 32
tracked total     -> 103
```

All tracked `.py`/`.sh` files in this checkout trace to one import commit:

```text
103 1c6f456c 2026-07-02 consolidate(phase-1): CN form kernel imported — CK is now the canonical form body
```

No current uncommitted `.py` or `.sh` additions were found. The failure is not
this patch adding host scripts; the failure is that the imported body carried a
large host-language scaffold while "no bash/no python" was treated as a proof
path slogan instead of a whole-body invariant.

## Deferred

- Replace low-level `make_nodeid` authoring with a higher-level source/lowering
  surface so humans do not write substrate constructors directly.
- Repair or regenerate the native `bp` table path so ontology names produce
  meaningful coordinates in this checkout.
- Compost or quarantine the imported Bash/Python scaffold: classify each file
  as delete, temporary host witness, or Form-native replacement target.
- Continue shrinking the C seed; this patch keeps the current witness honest
  while that work continues.
