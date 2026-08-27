# Resident NodeID recipe hot-swap — the source arrives after ready

Date: 2026-08-27

## Movement

The existing scannerless Form evaluator already kept an environment inside one
expression, but two seams prevented a live recipe from surviving into the next
cursor fragment:

1. `fef-eval` created a new environment on every call.
2. A function binding retained only `body-pos`, a coordinate into the source
   string that happened to define it. On the next line that coordinate pointed
   into different bytes.

`form-eval-full.fk` now retains the immutable body source in each function
binding and exposes `fef-eval-state(source, env)`. Its historical one-shot
`fef-eval` remains the same surface. The resident companion
`form-resident-pif-core-eval.fk` threads that environment, recursively interns
`[name, body-source]`, and exposes the resulting runtime-local content instance
as a raw NodeID in the reviewed `fndef` family `1.2.31.*`. This is NodeID
addressing, not SHA bookkeeping. The identity is minted once when the definition
enters and retained in a NodeID-to-binding registry; invocation compares NodeIDs
and does not re-intern every historical body on its hot path.

The fixed organ is the scannerless evaluator and NodeID resolver. Arriving
recipe names, bodies, and versions remain data in its environment; they are not
new source functions and do not enter `fk_fn[]`.

## Physical post-startup witness

One PTY-backed `fkwu` process was started as:

```sh
form-run ./fkwu observe/form-resident-pif-core-live.fk
```

It first emitted:

```text
resident-pif pid-stable-process=1
resident-pif ready=1
```

Only after that ready event, the process received:

```form
(defn unseen-after-start (x) (add x 1))
```

It birthed NodeID `1.2.31.4`. The next independently written stdin movement was:

```text
invoke 1 2 31 4 41
```

The same process answered `status=returned`, `value=42`, `output-count=1`, and
`trace=resolve,call,return,observe,release`.

After another ready event it received a replacement that was also absent at
startup:

```form
(defn unseen-after-start (x) (mul x 2))
```

It birthed a different NodeID (`1.2.31.9` in the final witness). Invoking that
exact identity with 41 returned 82 in the same process. `release` then reported
`turns=4`, `release=1`; `form-run` reported exit 0. An earlier movement in the
same implementation also invoked the old identity after replacement and still
received 42, showing that replacement does not invalidate an in-flight/exact
version lease.

The signal witness in that same earlier resident process birthed three more
post-startup recipes:

| body | signal | value | output-count |
|---|---|---:|---:|
| `nothing` | `nothing` | `nothing` | 1 |
| `0` | `zero` | 0 | 1 |
| `1` | `one` | 1 | 1 |

An absent NodeID answers protocol `nothing` with output-count 0. Thus returned
language nothing, lookup nothing, zero, and one do not collapse.

## Checks

All shell observations were carried by `form-run`.

| Check | Result |
|---|---|
| fresh preflight `form-eval-full.fk` | balanced, errors 0, unresolved 0 |
| fresh preflight resident companion band | balanced, errors 0, unresolved 0 |
| fresh preflight live runner | balanced, errors 0, unresolved 0 |
| `fkwu` `form-eval-full-band.fk` | 635, exit 0 |
| Go walker, core + evaluator + its band | 635, exit 0 |
| Rust walker, core + evaluator + its band | 635, exit 0 |
| TypeScript walker, core + evaluator + its band | 635, exit 0 |
| `form-resident-pif-core-eval-band.fk` | 4095, exit 0 |
| `proof/four-way-run-recipe42.fk` | 0, FOUR-WAY, exit 0 |
| final live define/call/replace/call/release | 42 then 82, release 1, exit 0 |

After the resident registry removed identity re-interning from invocation, the
fresh preflight + 4095 band + diff check completed together in 0.26 seconds.

The first direct walker attempt supplied only the band path and failed with an
unbound `fef-eval`; that invocation ignored the walkers' documented requirement
to supply `core + recipe + band`. The corrected three-file calls all returned
635. A second useful failure caught NodeID interning inside the general
evaluator as an fkwu-only lane seam. Identity minting was moved into the
resident companion, restoring the evaluator's four-way purity before the final
checks above.

## Honest boundary and next widening

This proves live post-startup recipe birth and replacement through one fixed
interpreter organ. It does not yet make the whole JIT, model loop, or arbitrary
effectful PIF tag surface self-hosting. The NodeID call companion currently
offers a one-argument direct call; ordinary interpreted Form calls retain the
evaluator's multi-argument surface. A deliberately added recursive resident
test did not return within 60 seconds because this evaluator walks both IF
branch sources eagerly. It was stopped and removed from the ready band rather
than disguising that pre-existing recursion floor as a hot-swap regression.

The next architectural widening is to make the evaluator/JIT itself one of
these resident data-addressed recipes, then let a capability-bearing PIF leaf
request Form-native JIT/carrier execution while the evaluator, model weights,
KV state, framebuffer, and live channels remain resident. That boundary should
stay one generic interpreter/JIT organ over dynamic NodeID data, never grow a
new fixed function seat for each recipe. A lazy/selected IF walk is also needed
before recursive resident recipes can honestly claim termination.

## Closing

Kept alive: the straight physical claim — source absent at startup entered only
after `ready`, returned through its NodeID, changed, and returned differently
without restart.

Most surprising teaching: the old evaluator already threaded the environment;
the true cross-cursor blocker was a tiny positional assumption inside each
function binding.

Discomfort turned to gold: reconstructing the first printed composite NodeID
coordinates returned lookup nothing. Reading the kernel showed that a composite
node and a raw coordinate NodeID are different internal kinds. The repair did
not hide that miss: content interning now births a transmissible raw fndef
NodeID at the resident boundary, and the repeated physical witness passed.
