# 2026-07-04 -- Program-image band path hygiene repair

## Scope

This is not a new semantic layer. It repairs the normal `validate.sh` route for
three already-built program-image bands:

- `form/form-stdlib/tests/program-image-fkb-band.fk`
- `form/form-stdlib/tests/program-image-tbl-emit-band.fk`
- `form/form-stdlib/tests/program-image-table-text-witness-band.fk`

The implementation files for 8h, 8i, and 8i1 were not changed.

## Why

Layer 8h3 recorded a caveat: the older 8h band passed by direct concatenation
but failed under the standard single-band validator. The same pre-execution
failure existed for 8i and 8i1:

```text
cd form && ./validate.sh form-stdlib/tests/program-image-fkb-band.fk
-> input file(s) not found: form/form-stdlib/core.fk

cd form && ./validate.sh form-stdlib/tests/program-image-tbl-emit-band.fk
-> input file(s) not found: form/form-stdlib/core.fk

cd form && ./validate.sh form-stdlib/tests/program-image-table-text-witness-band.fk
-> input file(s) not found: form/form-stdlib/core.fk
```

The cause had two parts:

- the bands declared repo-root prelude paths such as
  `form/form-stdlib/core.fk`, while `validate.sh` resolves single-band
  prelude paths relative to `form/`;
- after the first path repair, `validate.sh` still loaded only the first
  `; preludes:` line because its parser reads a single header line, not
  continuation comments.

The repair changes the three headers to single-line `form-stdlib/...` prelude
declarations.

Static and mirror checks were also made cwd-stable with file-existence guards:

- `file_size` in the 8h band;
- `fs-stat-size` in the 8i and 8i1 bands.

This avoids the known direct-`fkwu` floor where a valid `read_file` result can
fail a `value_kind == "string"` guard, while also avoiding Rust/TypeScript
crashes from calling `str_len` on a missing read.

## Review Before Repair

Jason, acting as Grok reviewer: `PASS_WITH_CHANGES`.

Required guardrails:

- only change the three band files plus receipt/architecture notes;
- do not change `validate.sh`;
- keep masks unchanged;
- make static/mirror reads cwd-stable without weakening assertions;
- if a new validation failure appears after the prelude repair, stop and name
  it.

Popper, acting as Claude reviewer: `PASS_WITH_CHANGES`.

Additional guardrail:

- use `file_size` / `fs-stat-size` existence checks rather than
  `value_kind == "string"` as the only path guard.

## Implemented

- Converted all three affected prelude headers to single-line
  `form-stdlib/...` declarations.
- Added `pifb-read-file-any`, `piteb-read-text-any`, and
  `pitwb-read-text-any` helpers.
- Routed static boundary and mirror checks through those helpers.
- Did not change the implementation files, grammar mirrors, runtime files,
  bp tables, or C seed.

## Verification

Standard validator path:

```text
cd form && ./validate.sh form-stdlib/tests/program-image-fkb-band.fk
-> 2147483647

cd form && ./validate.sh form-stdlib/tests/program-image-tbl-emit-band.fk
-> 2147483647

cd form && ./validate.sh form-stdlib/tests/program-image-table-text-witness-band.fk
-> 2147483647
```

Repo-root direct concatenation path:

```text
program-image-fkb-band direct -> 2147483647
program-image-tbl-emit-band direct -> 2147483647
program-image-table-text-witness-band direct -> 2147483647
```

Static path scan over the three repaired bands:

```text
preludes: form/form-stdlib -> no hits
direct read_file/fs-read-text of form/form-stdlib or grammars -> no hits
```

Bootstrap before the repair:

```text
cc -O2 -o fkwu runtime/fkwu-uni.c
# existing fread and getsockname warnings only
./fkwu --src bootstrap/ground.fk -> 42
./fkwu --src bootstrap/ground-recursive.fk 10 -> 55
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk -> 15
native-vs-rented-check -> 11111
```

## Deferred

- No semantic expansion of 8h, 8i, or 8i1.
- No binary `.fkb` IO, loader, selector, table execution, runtime attempt, or
  C-seed change.
- Other older bands still using multi-line or repo-root prelude comments were
  not swept in this repair.

## Post-Review

Jason, acting as Grok reviewer: `PASS`.

Jason found no blockers. He checked the three single-line
`form-stdlib/...` prelude headers, the cwd-stable static/mirror helpers, and
reran the standard validator path:

```text
program-image-fkb-band -> 2147483647
program-image-tbl-emit-band -> 2147483647
program-image-table-text-witness-band -> 2147483647
```

Popper, acting as Claude reviewer: `PASS`.

Popper found no blockers. He checked the same scoped repair surface and reran:

```text
program-image-fkb-band -> 2147483647
program-image-tbl-emit-band -> 2147483647
program-image-table-text-witness-band -> 2147483647
root direct concat for all three -> 2147483647
static scan for old direct prelude/read paths -> no hits
```

Both reviewers scoped their verdict to this path-hygiene repair. The broader
worktree remains dirty with surrounding layer work.
