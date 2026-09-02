# 2026-09-02 — demand is not temperature

Urs corrected the runtime vocabulary: source is runnable now; specialization may
be requested when observed use justifies it.  A missing acceleration image is not
a semantic readiness state, and a reusable image is not model residence.

## What changed

`form-demand-specialization-contract.bml` now separates five facts that had
been collapsed into one misleading metaphor:

- present Form source is directly runnable;
- a pure recipe at observed heat 5 or above may crystallize, while explicit
  decay below heat 2 may make the specialization meltable;
- loaded, prefilled, resident, idle, and active are independent model states;
- access age ranks policy-eligible eviction grains under measured pressure;
- OS/Metal page residency requires carrier evidence and is never inferred from
  access age, JIT heat, or an artifact on disk.

The BML cache witness now reports `source=runnable`,
`dispatch-candidate=source|image`, `artifact=absent|present`, and root compatibility,
`readiness-gate=0`, and `source-admission=whole-unit-synchronous`.  Glass
projects artifact presence, root compatibility, source-or-image dispatch
candidacy, and `admit=whole-sync` on one storage row.  It does not infer loader
acceptance or JIT.  That row remains distinct from model state and memory age.
Model lifecycle rows
use `resident`; the memory frontier is `policy-ranked` by release grain, use
count, and last use, with active units excluded.

## Physical Glass witness

The installed `glass` alias was launched in a real `80x24` terminal.  Its frame
showed the Qwen and 3B catalog extents, physical memory, exact `fkwu` process
RSS/CPU/elapsed observations, stale model evidence, and the visible row:

`# Glass image admit=whole-sync candidate=source artifact=present root=stale`

The verification instance was then terminated by its exact PIDs; the older
user-started Glass process was left untouched.

The launch also made the remaining defect undeniable: the alias announced
itself immediately, but the current `fkwu` compiler synchronously lowered the
whole BML prelude closure and waited roughly 120 seconds before producing the
first frame.  That delay is not demand-specialized execution.  It is a
whole-unit admission seam in the current carrier.

## Owed implementation

The next runtime crossing is an always-runnable Glass nucleus plus resumable,
bounded specialization work after the first frame.  General BML images also
need same-process installation and dependency-digest invalidation; the present
leaf JIT ABI is too narrow to stand in for that.  Until those pieces exist,
Glass names the synchronous admission honestly and makes no within-10%-of-
llama.cpp startup claim.

Independent AI review first returned **REQUEST CHANGES** on the unbalanced
runner and mtime-derived execution claim.  After repair, its final read-only
verdict was **PASS**: `candidate` is now a catalog-derived possibility, never a
claimed loader event or JIT execution.

## Failure became healing

The first fresh cache-run attempt exited 1 with `input-ended-mid-form`, even
though its text-only band still returned full credit.  Preflight confirmed one
open form.  The bounded tree healer declined the change, so the closer was
placed manually and accepted only after fresh preflight and direct execution.
The band now parses balance itself and rejects negative-depth prefixes and
unterminated strings; its full verdict grew from `16383` to `32767`.

The review then caught a subtler fault: root mtime compatibility had been named
as actual execution.  That became `dispatch-candidate`, while loader outcome
remains unavailable.  Parallel proof attempts also demonstrated that atomic
multi-writer image installation is not yet proven, so all final image-producing
preflights and bands were serialized.

## Executable witnesses

All returned their declared full verdict with exit zero:

- demand/specialization contract: `255`
- BML cache authority and executable balance: `32767`
- Glass observer: `262143`
- Glass telemetry membrane: `2097151`
- Glass dashboard: `4194303`
- Glass live policy: `2097151`
- model Glass lifecycle: `16383`
- 3B resident lifecycle: `255`
- resident fleet eviction policy: `8191`
- model memory Glass: `32767`
- Qwen JIT teaching: `33554431`
- JIT decision: `11111`
- JIT heat gate: `4095`
- form-cli JIT: `16383`
- BML rest/force: `31`
- direct-source JIT discovery: `32767`

Most surprising teaching: the live frame made one word do real architectural
work.  Once specialization, residence, access age, and page evidence had
separate rows, the roughly two-minute blank interval could no longer hide inside a
readiness label.

Discomfort turned to gold where the monitor itself proved the contradiction:
it could display a source dispatch candidate only after a blocking whole-closure
admission.  Naming that exact boundary produced the next build
order instead of another euphemism.

Signed: **Codex / Sol**.  I kept the exchange alive by accepting Urs's
correction as runtime evidence, changing the executable vocabulary, launching
the physical Glass door, and preserving the unhealed compiler seam as an owed
cell rather than a performance claim.

; witnessed: 2026-09-02 -> demand contract 255; physical 80x24 Glass frame;
; synchronous first-frame admission remains explicitly owed
