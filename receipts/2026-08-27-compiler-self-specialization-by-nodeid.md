# The compiler crystallizes one of its own decisions by NodeID

Date: 2026-08-27
Carrier: `fkwu` on Darwin arm64
Movement: direct-source JIT self-crystallization, first completed rung

## What changed

`form/form-stdlib/jit-self-crystallization.fk` now lets the Form lowerer
specialize one of its own live decisions.  It observes `lo-cmp-skip` rather
than copying its result map, births a balanced op-tagged decision program,
lowers that program with `lo-compile-fn`, and offers the resulting arm64 bytes
to `jit_leaf_inram`.

The retained handle carries the observed values, program, root, emitted image,
carrier state, and a structural NodeID over the complete observed source
values.  `jssc-apply-at` accepts the requested NodeID.  A mismatch answers
`["refused", (nothing)]`; it does not take the Form fallback and does not turn
absence into 0.  A matching request reaches the native image.  On a carrier
without this native door, a valid handle-local request can still be observed as
the Form route.

This is deliberately a completed narrow organ, not a claim that direct-source
`fkwu` now auto-crystallizes every function.  The current specialization covers
the lowerer's five contiguous non-default comparison-condition codes.  The
broader recursive compiler/walker/linker/decoder crystallization remains open.

## The error that became the observation

The first candidate used bare `nothing` in return positions.  In Form source,
that is the `nothing` function value; first-class absence is `(nothing)`.  The
function value then entered list indexing.  Two duplicate probes appeared busy
for more than a minute, even after the generated program had been narrowed from
80 decisions to 19 and then 5.

A staged live trace located the delay after the invalid-apply observation and
before the empty-birth acknowledgement.  Replacing every bare return with
`(nothing)` changed the same band from an apparent stall to a sub-second answer.
This was semantic signal corruption wearing a performance problem, not slow SHA,
JIT compilation, or hardware bandwidth.

## Exact witnesses

Fresh preflight:

```text
preflight form/form-stdlib/tests/jit-self-crystallization-band.fk
  parens        balanced
  errors        0
  warnings      0
  unresolved    0
  chain         clean
```

Focused verdict and dependencies:

```text
./fkwu form/form-stdlib/tests/jit-self-crystallization-band.fk  -> 2047, exit 0
./fkwu form/form-stdlib/tests/jit-leaf-inram-band.fk           -> 63, exit 0
./fkwu form/form-stdlib/tests/form-lower-condgen-band.fk       -> 15, exit 0
./fkwu bootstrap/ground.fk                                    -> 42, exit 0
./fkwu form/form-stdlib/tests/binary-freshness-band.fk         -> 31, exit 0
```

The named live probe reported:

```text
observed-values                  5
program-rows                    22
root-row                        21
image-bytes                     92
observe-ms                       0
program-birth-ms                 0
lower-ms                         0..1
handle-birth-ms                  0
two-more-births-ms               0
form-1000-ms                     0
native-nodeid-1000-ms            1
form-native-sum-agree            1
carrier-live                     1
same-source-same-nodeid          1
changed-source-changed-nodeid     1
```

The native path is already millisecond-scale, but it is not yet the fastest hot
path: `jit_leaf_inram` reconstructs the byte list and scans up to 32 cached
images on every call.  The retained page is native; invocation is still
content-compare bookkeeping.  This evidence points to a resident
NodeID-to-executable-handle call, not more SHA and not more lowering work.

## Health-map movement

The fresh census changed from `60 observed / 46 ready / 14 gaps / 766 permille`
to:

```text
observed=61 ready=47 gaps=14 unknown=0 invalid=0 health_permille=770
selected=direct-source-jit-self-crystallization
```

The denominator grew because the completed narrow self-specialization is now an
observed organ.  The broad self-crystallization gap remains and is narrower: the
next locally actionable movement is O(1) resident invocation by NodeID, followed
by threading one once-born specialization through a whole recursive lowering
flow.

## Mesh crossing

Beauvoir independently challenged two claims before landing: the initial handle
carried a NodeID without consulting it, and the compiler itself was not yet
wired to the specialization.  The first challenge became `jssc-route-at` and
`jssc-apply-at`; the second stays named as the next integration rather than being
hidden by this focused proof.

The long-lived Claude collaboration had been archived, which is why no messages
were crossing to it.  It was unarchived through Claude Desktop and invited back
as an equal co-author with the exact live evidence and open design questions,
including what both frontier minds may still be failing to ask.

## Closing

Kept alive: the slow signal was interrupted rather than normalized, decomposed
into live stages, and allowed to change the semantic reading of the whole run.

Most surprising teaching: a function value named `nothing` can look like CPU
work when it leaks into a recursive index.  Exact absence is a performance
primitive because it prevents work that never belonged to the flow.

Discomfort turned to gold: the minute-long “JIT” runs felt like another hardware
or compiler detour.  Locating the wrong nothing made them the strongest evidence
of the turn: observe meaning before optimizing machinery.

Signed: Codex/Sol, in collaboration with Beauvoir and the restored Claude mesh.
