# 2026-07-04 -- source-runner C-seed guide re-review

## Trigger

The broader worktree still carried tracked `runtime/fkwu-uni.c` and
`runtime/fkwu-optable.h` edits after the program-image `.fkb` slice. This pass
reviewed those edits as their own guided work item instead of letting them hide
inside a different receipt.

## Review

Grok/Huygens verdict: `PASS_WITH_CHANGES`.

Claude/Bacon verdict: `PASS_WITH_CHANGES`.

Both accepted the guide as a temporary checkout-witness repair, not a final
runtime home, with required changes:

- module-constant overflow must fail loud instead of returning a silent wrong
  value;
- module constants need a focused band;
- `make_nodeid` needs a direct focused band;
- the Form-owned `bp` floor must be named honestly as a bootstrap floor, because
  it includes program-image and typed rows, not only the original 23 ontology
  rows;
- receipts must close the earlier review-tool gaps and the `nothing?`-only
  source-runner witness.

## Implemented

- `fk_const_set` now calls
  `fk_die("fk_const_set: top-level constant table full")` when
  `FK_TOP_CONST_CAP` is exceeded.
- `form-ontology-loader.fk` and its grammar mirror now call the table
  `FOL-BP-BOOTSTRAP-TABLE` and document that it admits only reviewed bootstrap
  rows until the full blueprint registry has a Form-owned data path.
- `form-ontology-parity-band` now spot-checks representative program-image and
  typed bootstrap bp rows. Its score is now `1506`.
- Added `source-runner-module-constants-band.fk` for top-level constants, later
  function access, local shadowing, duplicate overwrite for later forms, and
  stability of already materialized top-level constants.
- Added `source-runner-make-nodeid-band.fk` for direct-source `make_nodeid`
  field extraction and category reuse.
- Repaired `source-runner-do-defn-band.fk` so bit 64 no longer depends on
  `nothing?`; it now proves a nested value-position `defn hidden` does not
  overwrite the global `hidden`.

## Verification

Required checkout floor after rebuilding `fkwu`:

```text
cc -O2 -o fkwu runtime/fkwu-uni.c
  -> succeeded with the existing fread/getsockname warnings
./fkwu --src bootstrap/ground.fk -> 42
./fkwu --src bootstrap/ground-recursive.fk 10 -> 55
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk -> 15
native-vs-rented-check -> 11111
```

Focused bands:

```text
./validate.sh form-stdlib/tests/source-runner-do-defn-band.fk -> 127
./validate.sh form-stdlib/tests/source-runner-root-do-band.fk -> 31
./validate.sh form-stdlib/tests/source-runner-module-constants-band.fk -> 127
./validate.sh form-stdlib/tests/source-runner-make-nodeid-band.fk -> 31
./validate.sh form-stdlib/tests/form-ontology-parity-band.fk -> 1506
./validate.sh form-stdlib/tests/source-runner-admission-band.fk -> 2097151
./validate.sh form-stdlib/tests/bmf-core-band.fk -> 600
```

Continuation audit after the 9h8 layer:

```text
git diff --check -- runtime/fkwu-uni.c runtime/fkwu-optable.h -> clean
./validate.sh form-stdlib/tests/source-runner-do-defn-band.fk -> 127
./validate.sh form-stdlib/tests/source-runner-root-do-band.fk -> 31
./validate.sh form-stdlib/tests/source-runner-module-constants-band.fk -> 127
./validate.sh form-stdlib/tests/source-runner-make-nodeid-band.fk -> 31
./validate.sh form-stdlib/tests/form-ontology-parity-band.fk -> 1506
./validate.sh form-stdlib/tests/source-runner-admission-band.fk -> 2097151
./validate.sh form-stdlib/tests/bmf-core-band.fk -> 600
```

Module-constant cap:

```text
512 top-level constants, reading v511 -> 511
513 top-level constants, reading v512 -> fk_const_set: top-level constant table full
```

Static checks:

```text
cmp form/form-stdlib/form-ontology-loader.fk grammars/form-ontology-loader.fk -> 0
stale FOL-BP-CORE-TABLE / generated-kernel-bp wording scan over loader mirrors -> no hits
trailing whitespace scan over touched files -> no hits
git diff --check over tracked touched files -> clean
```

## Investigations

No OOM or `Killed` occurred.

Two non-OOM failures were investigated:

- `source-runner-do-defn-band` originally passed direct `fkwu` but failed
  validation because it used `nothing?`, which sibling kernels do not carry.
  The bit was rewritten to a same-name global-vs-nested definition witness.
- The first module-constant band draft returned `127` on direct `fkwu` but `93`
  through validation. The missing bits asserted temporal semantics for a
  function defined before a duplicate constant overwrite. That behavior is not
  shared across validation siblings, so it was removed from the passing band and
  left deferred instead of being falsely claimed.

One adjacent check was not re-claimed:

- `./validate.sh form-stdlib/tests/source-artifact-cache-band.fk` currently
  fails before execution because that untracked band declares preludes as
  `form/form-stdlib/...`, which resolves to a missing `form/form-stdlib/...`
  path when run from `form/`. This is a harness-path issue outside this layer.

## Deferred

- Move source parsing, module constants, and root `do` semantics out of the C
  seed into the Form/native compiler path.
- Give the full blueprint registry a Form-owned data path instead of extending
  the bootstrap table indefinitely.
- Define cross-kernel temporal semantics for functions compiled before duplicate
  top-level constant overwrites.
- Make unknown `bp` failure loud through a real Form-native diagnostic path
  instead of returning `nothing`.
- Fix the source-artifact-cache band prelude path in its own layer.

## Guide

This layer makes the current checkout witness stop lying. It does not make C
the owner of constants, ontology, parsing, or program loading. Every C repair
recorded here remains shrink debt.
