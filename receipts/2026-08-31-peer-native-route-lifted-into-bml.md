# Peer native route authored in BML

Date: 2026-08-31
Crossing: high-grammar BML → cached native image → retained Form evaluator →
NodeID invocation inside the existing live peer.

The duplicate handwritten Form definitions are gone. Three BML cells now own
the new semantic path:

- `form/form-stdlib/bml/form-cli-resident-native-program.bml` owns BML → BMF
  module lowering, resident-environment retention, NodeID load, and invocation.
- `form/form-stdlib/form-cli-peer-live-grammar.bml` owns the task grammar,
  including the explicit `native ` route before any model-facing observation.
- `form/form-stdlib/form-cli-peer-native-dispatch.bml` owns direct native
  execution, typed `nothing`/`one`/value receipts, and the dispatcher branch.
- `form/form-stdlib/bml/form-cli-peer-agent-live.bml` owns peer ingress,
  reporting, release, model-session admission, and the long-lived task loop.

`observe/form-cli-peer-agent-live.fk` is now only the one-line compatibility
entry door. At peer birth, the BML loop loads the BML program once, then passes
that retained value to the BML dispatcher for each request.

## Fresh witness

```
$ ./fkwu form/form-stdlib/bml/form-cli-resident-native-program.bml
0
$ ./fkwu form/form-stdlib/form-cli-peer-live-grammar.bml
0
$ ./fkwu form/form-stdlib/form-cli-peer-native-dispatch.bml
0
$ ./fkwu form/form-stdlib/bml/form-cli-peer-agent-live.bml
0
$ ./fkwu form/form-stdlib/tests/form-cli-resident-native-program-band.fk
511
$ ./fkwu form/form-stdlib/tests/form-cli-peer-native-program-action-band.fk
255
$ ./fkwu form/form-stdlib/tests/form-cli-peer-live-dispatch-band.fk
255
$ ./fkwu form/form-stdlib/tests/form-cli-peer-live-grammar-band.fk
4293918719
```

The direct native bands establish the BML/BMF load, retained NodeID, original
session preservation, direct branch before the model-facing observer, zero
model/injection/host-crossing fields, and the separation of `nothing`, `0`,
`1`, and a present value. The grammar aggregate adds the BML-held native-prefix
rule. In particular, BML's `fcrnp-zero-is-value?` witnesses that `nothing?(0)`
is false and `eq(0, nothing())` is false.

```
$ printf '0\n1\n41\n' | ./fkwu form/form-stdlib/form-cli-resident-native-program-live.fk
resident-native-program input=0 signal=nothing route=resident-native-program model=0 host-crossings=0
resident-native-program input=1 signal=one route=resident-native-program model=0 host-crossings=0
resident-native-program input=41 signal=value route=resident-native-program model=0 host-crossings=0
```

`observe/preflight-run.fk` over `observe/form-cli-peer-agent-live.fk` reported
balanced parens, zero errors, warnings, and unresolved calls.

## Cache-seed repair

The temporary `fkwu` seed already stated that BML dependency images must not be
imported as standalone units: their own carried prelude chain belongs to the
BML floor. Its second import loop omitted that same predicate and therefore
probed `form-cli-peer-native-dispatch.bml` alone, stamped an unresolved-image
refusal, and fell back to a whole-program compile on warm imports. The repair
applies the existing `.bml` exclusion to that loop as well. It adds no
evaluator meaning, table, allocation, or new C subsystem; it removes an
incorrect cache crossing while the seed continues to shrink toward the native
Form walker.

After rebuilding the one `fkwu` binary, `bootstrap/ground.fk` returned `42`,
`binary-freshness-band.fk` returned `31`, and `form-cli-bml-cache-run.fk`
reported `bml-cache state=ready bounded=1`. A second, warm run returned:

```
$ ./fkwu form/form-stdlib/form-cli-peer-native-dispatch.bml
0
$ ./fkwu form/form-stdlib/tests/form-cli-peer-live-grammar-band.fk
4293918719
```

with no cache fallback or diagnostic output. The C seed remains a checkout
witness, not a home for peer semantics; all new route meaning above is BML.

## Boundary held open

The low-level local model-session carrier remains an existing Form dependency
behind the BML peer, while all new peer route meaning is BML-owned. No provider
request was made by this witness, so remote-token reduction remains
intentionally unmeasured. BML cache files are local derived `.bml.fkb`/`.bml.sym`
images, not source truth.

I kept the exchange alive by moving meaning rather than wrapping it: the peer
loop, resident program, grammar, and dispatcher now originate in BML while the
old path retains only its entry invocation. The surprising teaching is that a
NodeID can hold a BML-born thought across requests; the discomfort of the former
split source became a single executable semantic path.
