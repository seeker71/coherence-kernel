# The normal rented session reaches local Form-cli at zero remote tokens

The active goal's exact BML literal was already served once by the local
resident as task turn `9601`.  This movement makes the requested comparison
executable without rewriting historical session evidence as a provider-call
receipt.

`form-cli-rented-goal-comparison.bml` accepts two differently named evidence
layers:

- the observed historical normal rented-session transcript receipt, whose
  three witnessed markers admit `93,628` output tokens; and
- the caller-provided task/reply spools for one turn, whose bounded task frame
  must contain the BML-owned exact goal and whose reply must be a candidate on
  `direct-answer`, `form-native-metal-jit`, with zero callback, lookup, and
  injected-byte counts and `generate,release` lifecycle.

The one local invocation, reading only those three local files, returned:

```text
source=historical-rented-session-vs-local-direct
task-turn=9601
normal-remote-tokens=93628
form-cli-remote-tokens=0
ratio-bps=0
under-ten-percent=1
```

The zero is an observed direct no-provider result: the durable reply carries
`callback-calls=0`, not a missing token count.  In contrast, absent history,
an inexact task, a non-direct reply, or the wrong turn produces `status=choice`
and every quantity as `nothing`.  The focused band covers each of those
refusals (`8191`).

This is deliberately **not** a `provider-last-token-usage` comparison.  The
stricter provider grammar remains separate and withholds its ratio until a
same-task provider boundary exists.  The ratio above answers the requested
normal-session comparison with its actual historical provenance rather than
pretending a different denominator has appeared.

The grammar executes as high BML and was run directly to create its cached
native image (`form-cli-rented-goal-comparison.bml.fkb`, 250,266 bytes).  The
existing cache authority then reported `state=ready`.  The comparison runner
does not construct a prompt, invoke a model, open HTTP, or select a filesystem
root: its three evidence paths and turn arrive from the caller, and its output
contains scalars only.  Metal/JIT belongs to the already observed local answer
carrier, not to this bookkeeping comparison.

Fresh checks:

```text
binary-freshness-band.fk                       -> 31
form-cli-embodied-goal-grammar-band.fk         -> 511
form-cli-rented-goal-comparison-band.fk        -> 8191
preflight (comparison band)                    -> balanced / 0 errors / 0 unresolved
wrong-turn local invocation                    -> choice; all quantities nothing
```

I kept the exchange alive by making the denominator's kind visible instead of
stretching it into a provider claim. The surprising teaching is that zero
crossings can be stronger evidence than a small measured remote number; the
discomfort around an incomparable provider denominator turned into a precise
second grammar rather than a shortcut.

— Codex
