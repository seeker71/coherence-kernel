# 2026-06-30 -- core.fk direct Form refactor

## Ground

The required checkout witness was rebuilt and re-run before the edit:

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk
./fkwu --src bootstrap/ground-recursive.fk 10
( cat observe/native-vs-rented.fk; echo '(native-vs-rented-check)' ) > /tmp/nvr.fk
./fkwu --src /tmp/nvr.fk
```

Witness:

```text
42
55
11111
```

The compile still emits the pre-existing `fread` declaration warning and the pre-existing
`getsockname` pointer-sign warning. No C seed change was made.

## Problem

`form/form-stdlib/core.fk` was authored in the BML maintenance dialect:

```text
section [form.bml] { class Num<T> { ... } ... }
```

The current direct source runner does not lower that high grammar into callable direct
Form definitions. Running the old file alone returned `0`, and appending a call to a core
recipe also returned the floor:

```sh
( cat form/form-stdlib/core.fk; echo '(len (append (list 1) (list 2)))' ) > /tmp/core-append-current.fk
./fkwu --src /tmp/core-append-current.fk
```

Old witness:

```text
0
```

That meant files either treated core as an implicit convention or hand-copied equivalent
helpers such as `append`. The high-level Num/List/Cell/Task shape was present as BML text,
but unavailable to the active `--src` lane as executable or queryable Form.

## What Changed

`form/form-stdlib/core.fk` is now direct Form:

- public vocabulary remains `identity`, numeric predicates/helpers, list transforms,
  higher-order list combinators, `cell`, `task`, and `assert`;
- primitive direct-access names such as `head`, `tail`, `len`, `nth`, `cons`, `list`, and
  `empty` are intentionally not wrapped or shadowed;
- the old high semantic classes are carried as data through `core-class`, `core-method`,
  `core-num-class`, `core-list-class`, `core-cell-class`, `core-task-class`,
  `core-classes`, and `core-section`.

`surface/core.fk` remains the high-grammar BML reference. The executable stdlib core no
longer depends on that parser lane to be useful to native witnesses.

## Witness

```sh
./fkwu --src form/form-stdlib/core.fk
( cat form/form-stdlib/core.fk form/form-stdlib/tests/core-band.fk ) > /tmp/core-band.fk
./fkwu --src /tmp/core-band.fk
( cat form/form-stdlib/core.fk; echo '(len (append (list 1) (list 2)))' ) > /tmp/core-append-current.fk
./fkwu --src /tmp/core-append-current.fk
```

Witness:

```text
0
255
2
```

`255` means the new band observed numeric helpers, list shape operations, list aggregation,
higher-order calls, cell/task accessors, semantic class descriptors, and the true branch of
`assert`.

## Honest Boundary

This is not a BML compiler. It does not claim that `section`, `class`, or generic syntax
now lowers natively. It moves the load-bearing core stdlib into direct Form so the current
native source lane can use it, while preserving the higher semantic class shape as Form data
that analyzers and JIT policy code can inspect.
