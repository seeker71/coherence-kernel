# 2026-06-30 -- direct-source band wrappers

## Ground

```sh
( cat observe/native-vs-rented.fk; echo '(native-vs-rented-check)' ) > /tmp/nvr.fk
./fkwu --src /tmp/nvr.fk
```

Witness:

```text
11111
```

## What Changed

The current `fkwu --src` floor binds sequential `let`s reliably inside a function
frame. Several imported stdlib bands were written with top-level `let`s, which
made them return `0` on this direct-source lane even though the body cells were
valid.

Wrapped the affected bands in explicit `*-band` functions and called those
functions at the end, matching the pattern used by the newer speech witnesses.
For raw-source witnesses, removed unnecessary `core.fk` BML preludes and replaced
`sum` helper use with explicit `add` trees where needed.

## Witness Matrix

```text
channel-interface-band              127
recognition-router-band             127
recognition-router-compute-band      63
recognition-router-vision-band       31
satsang-band                        127
satsang-field-band                  255
satsang-share-band                  255
satsang-flip-witness-band          4095
satsang-guidance-event-band         255
satsang-health-memory-band         1023
satsang-host-boundary-band      2097151
satsang-listen-route-band           255
satsang-room-memory-band            255
```

## Honest Boundary

This did not grow the C seed. It adapted imported witness shape to the then-bounded
direct-source surface: function frames for sequential bindings, explicit add
trees for raw-source sums, and the old high-grammar `core.fk` left to the BML/source
lowering lane. Follow-up receipt `2026-06-30-core-direct-refactor.md` records the later
floor lift: `form/form-stdlib/core.fk` is now direct Form and can be concatenated by
new `--src` witnesses.
