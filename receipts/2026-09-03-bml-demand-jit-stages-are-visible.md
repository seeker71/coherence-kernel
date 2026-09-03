# BML demand JIT stages are visible without inventing cache zeros

Date: 2026-09-03
Scope: lowerable scalar BML graph; retained ARM64 leaf page; Glass projection

## What changed

The existing first-answer-safe demand JIT still answers the first request in
Form and specializes only in the later bounded step.  Its later-step receipt
now retains separate physical observations for cache inspection, image
materialization, install plus native challenge, atomic disk publication, total
time, accounted time, and unattributed time.  Cache and source age/mtime are
presence-bearing fields: an absent path has an absence reason and no numeric
age.  Cache classification is a named disposition (`hit-current`,
`miss-absent`, `miss-stale`, `miss-invalid`, or `source-absent`) rather than a
hit bit whose zero has to be guessed.

The Glass publisher projects two complete, uniquely scoped paths in one
snapshot: the first registry and a second process-shaped registry.  Each path
contains request, pending work, inspect, artifact age, materialize,
install/challenge, disk action, total, unattributed attention, and the exact
remaining frontier.  The bounded `form-glass-bml-jit-current-run.fk` view uses
the shared `term-graphics.bml` canvas and renders sub-millisecond observations
as `<1ms`, not ambiguous `0`.

## Physical observation

A source-authority freshness change produced this first/reuse pair:

```text
first  cache=miss-invalid age=179398ms inspect=0ms compile=0ms
       install/challenge=0ms publish=4ms total=4ms
       status=compiled-published-installed
reuse  cache=hit-current age=403ms inspect=0ms disk-image=0ms
       install/challenge=0ms publish=0ms total=0ms
       status=disk-installed
same-process route=native; new-registry retained route=native
```

The numeric zeros above are raw millisecond-resolution evidence.  Glass shows
each as `<1ms`.  Its 20-node panel therefore reads as stages rather than as an
undifferentiated JIT counter.  The exact unbuilt boundary travels in the same
snapshot as:

```text
runtime.full-program-image.call  carrier-door-absent
```

That is not converted into a synthetic capability.  The current proven lane
installs and reuses scalar ARM64 leaves; arbitrary strings, lists, cross-calls,
and whole-program images remain outside it.

Bounded global Glass panel `#0` at `02:48:48.895Z` reported `ev=93`,
`nodes=218`, and `cons=16K`.  Reading the JIT publisher through its focused
view showed `publisher=jit.bml-demand nodes=20`, including the paired
miss/hit, age, per-stage duration, attention, and frontier rows.  Snapshot age
was visible independently of artifact age.

## Content identity and refusal attention

The former cache fingerprint joined file size to whole-second mtime. That is
not content identity: the band now overwrites one path from `AAAA` to `BBBB`
inside the same second, observes equal size and equal mtime, and proves that
the two known SHA-256 identities reject the old wire as `cache-runtime`.

Hashing the whole executable in pure Form on every demand was attempted and
refused: the fresh proof had still produced no verdict after 30 seconds. The
landed path instead reads only the first 512 bytes of the current
`bml-demand-jit.sym` and consumes its `source-hash`. That unit identity is
already produced and freshness-checked by the fkwu image loader from every
dependency's content digest, including the emitter and this authority. The
leaf ABI is explicit beside it, and a page is installed only after its native
answer agrees with the Form answer at the pending challenge point. A missing
or malformed source-hash withholds reuse as
`runtime-identity-unavailable`; size and mtime are never relabelled.

A refusal now latches `jit-fault-attention` at priority 1000. Requests and
ordinary steps remain preempted, with exact cause, original stage, observed
epoch and fault key preserved. A cause-matched control can record a bounded
`held` receipt or arm a heal probe, but it cannot clear the fault. Only a later
successful compile/install/native challenge clears it and publishes
`jit.bml-demand.healed`, bound to the original publisher, fault key, cause,
priority, and fault epoch. A failed probe returns to held. The physical fault
publisher returned `published`; its exact diagnostic sample is lifecycle
`failed`, kind `diagnostic-control`, attention `immediate`, and its stage
accounting satisfies `accounted + unattributed = total` using the same finish
timestamp. Normal successful execution emits no synthetic heal.

Glass now reads cache inspection at the snapshot epoch and cache write age
from the artifact mtime (the previous projection had those directions
reversed). Fault-bearing stage rows are failed/red rather than green
`within-5s`; the exact diagnostic carries priority 1000 so a consumer can
choose it ahead of same-epoch derived stage rows.

## Startup and cache topology

The six ignored cache artifacts owned by this path were moved—not deleted—to
`/tmp/form-jit-cold.NF6GFM`.  No shared Glass renderer cache was touched.  The
next source admission and execution took `0.91s` real time.  It reused the
freshness-valid 231-byte leaf disk image, installed and challenged the page,
then published 20 Glass nodes.  The immediately repeated process took `0.01s`.
Binary freshness remained `31`, comfortably below the sixty-second startup
bound.

The broad inventory assumption that every BML prelude owns an adjacent image
was false.  A request-evidence control and re-observation established the
actual path topology as `[wrapper-fkb=1, compatibility-door-fkb=1,
authority-adjacent-fkb=0, glass-adjacent-fkb=0]`.  The wrapper and compatibility
door are the reusable program images on this invocation path; the high-BML
authorities are carried into them.  The correlated diagnostic window ended at
8 framebuffer events.  Its request-evidence control physically gated five
bounded file-stat reads; its continue control physically gated `bdj-step` and
one retained request.  On the first run the diagnostic's own leaf image was
absent, so its measured step compiled and published in `11ms`, then reobserved
route `native`.  The next run measured a 231-byte current image aged `9109ms`,
selected `disk-image`, completed below the millisecond clock resolution, and
again reobserved route `native`.  No historical timing is injected into that
cell, and `bfc-apply` is described only as control validation/selection, not as
the actuator.  Absence is retained as topology, not reported as a zero-byte
cache.

The resident dual-model owner was not opened, released, or restarted.  After
the observations it remained PID `18249`, nice `10`, state `SN+`, running
`native-model-dual-resident-live-run.fk`.

## Proof

- `bml-demand-jit-band.fk` preflight: balanced, 0 errors, 0 warnings,
  0 unresolved; verdict `34359738367`.
- `bml-demand-jit-glass-ui-band.fk` preflight: balanced, 0 errors, 0 warnings,
  0 unresolved; verdict `511`.
- `form-glass-bml-jit-current-run.fk` preflight: balanced, 0 errors,
  0 warnings, 0 unresolved.
- The effectful live and diagnostic runners carry
  `preflight-exec: forbidden`; preflight preserves their source and visibly
  refuses execution until a compile-only carrier exists.
- `binary-freshness-band.fk`: `31`.
- scoped `git diff --check`: clean.

## Closing

I kept this movement alive by making the ordinary path fast again while
leaving refusal impossible to mistake for a routine miss. The surprising
teaching was that the compiler's already-validated source-unit identity is a
better demand boundary than rehashing the executable for every leaf. Discomfort
became gold when the whole-file SHA path crossed 30 seconds: stopping it led to
the bounded 512-byte identity read, 4ms physical specialization, and a recovery
protocol that no external `healed` string can counterfeit.

— Codex

; witnessed: 2026-09-03 -> bml-demand-jit-band 34359738367;
; bml-demand-jit-glass-ui-band 511;
; Glass panel #0 ev93 nodes218 cons16K; focused JIT nodes20
