# Unchanged Glass admission is demanded, not eagerly refreshed

Glass already painted a small truthful frame before the full graph, but an
unchanged launch still opened fourteen child `fkwu` processes to validate BML
units which the demanded live door would validate again.  This movement keeps
the loader as the authority and removes only that redundant eager work.

`fglu-launch` now observes the dependency manifest before refresh.  Changed
identity still refreshes the bounded units and then recomputes the manifest so
the persisted identity cannot be a pre-refresh symbol row.  Unchanged identity
selects `identity-current-loader-on-demand`, spawns no refresh child, and hands
the full view to `observe/form-glass-live-run.fk`.  That door still enters
through `fkwu`; missing, stale, and foreign images are rejected by the loader.
The staged startup continues to call every disk image a candidate only.

## Physical timing

Before the change, a current launch painted in 42 ms, mapped fourteen units in
8--12 ms each, and completed admission in 180 ms (`real 0.19 s`).  After the
change, the same current identity painted in 44 ms, reported
`refresh-plan=identity-current-loader-on-demand refreshed-units=0`, and
completed admission in 45 ms (`real 0.05 s`).

A separate bounded physical call of the changed-identity branch persisted no
manifest and refreshed exactly 14 units in 141 ms; its individual maps were
8--13 ms and all remained within attention.  Thus both plan branches were
executed, while only the unchanged branch was allowed to skip loader work.

Task-local exact source copies gave target-cache-absent measurements without
moving the standing Glass or its cache files:

- the staged startup BML: 1.78 s on the first source path, including one
  observed rebuild of foreign compiler/ontology cache images; immediate disk
  replay: 0.00 s;
- the launch BML: 0.29 s; immediate disk replay: 0.00 s;
- the 49,375-byte full live BML behind a finite compile-only runner: 3.29 s;
  immediate disk replay: 0.01 s;
- the complete one-frame stage profiler: 1.63 s from an absent target image,
  including 357 ms of measured collection; disk replay: 0.36 s, including
  350 ms of collection.

No current physical stage crossed the five-second attention boundary.  The
older demand-JIT diagnostic still contains its historical 5.43 s and 6.17 s
fixture observations; those numbers were not relabelled as current.  The live
scalar demand lane answered through Form in 0 ms, specialized and published in
4 ms, then used the retained native page in the same process.  A second process
answered through Form in 1 ms, installed the exact disk image in 0 ms, then
used its retained native page.

The cache witness initially and correctly said `stale`: the 4,459-byte current
authority was newer than an image whose symbol identity recorded the former
3,844-byte source.  Direct BML re-lowering took 0.15 s.  Only after that did the
witness say `dispatch-candidate=image root-compatibility=current`; the next
direct image dispatch measured 0.00 s.  Presence was never promoted to
freshness.

## Proof and boundary

All listed verdicts exited zero:

- binary freshness `31`;
- BML cache: `image/current`, readiness gate `0`;
- high-authoring band `4095`;
- staged-startup band `65535`;
- launch band `32767`;
- demand-JIT band `16777215`;
- task-local preflight: launch band balanced, zero errors, warnings, and
  unresolved calls.

The high-grammar authority's direct-run `input-ended-mid-form` error remains a
known lowering seam.  A task-local archive of the unmodified `HEAD` authority
produced the same three-open-parentheses refusal, proving this patch did not
introduce it.  The executable BML and proof band run cleanly.  Whole-program
callable image installation after startup is also still unavailable; this
change does not rename scalar leaf JIT or disk reuse into that missing carrier.
No C source, model route, resident model, or share-meter file changed.

The current bounded Glass panel at `16:05:56.404Z`, frame `#0`, read
`m92 s45 o25 drop=0 cap=39/row`, memory river `R@=53`, and the measured mixed
pipeline carried `M+5ms -> F+f42 -> G+s8`.  That is the panel consulted for
this crossing, not a fabricated performance fixture.

Alive: the first truthful frame stays immediate while later work remains a
demand.  Most surprising: the full copied Glass source graph compiled in
3.29 seconds; the repeated cost in the current path was fourteen already-warm
validations, not a hidden minute-long stage.  Discomfort turned to gold when a
present cache first looked reusable but its symbol bytes proved stale; the
image was re-lowered and only then named current.
