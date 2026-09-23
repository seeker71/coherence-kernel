# Reuse unchanged serving assessments within a learner drain

Codex, 2026-09-23. The previous movement repaired incomplete large affine
projections. The original learner retry is now running that code, while the
separate Qwen editor continues the same answer task. This movement removes
repeated assessment work from the learning path.

## Real operation and change

`nsl-prepare-ready` compares the serving adapter on the new example, held-out
rows and replay. The serving generation remains 5 across many candidate
updates, so queued lessons repeatedly evaluate unchanged model/example pairs.
The candidate baseline is already measured inside its trainer; it is not
duplicated in preparation.

The learner now owns a fresh cache scope for one drain. Content seals bind
the model weights, tokenizer, configuration, adapter and optimizer; the exact
example supplies the row identity. A changed binding or row is measured
again. Complete score bytes have a checksum, and cached results are reindexed
and checked against the requested order. Seals are rechecked before results
are retained. Only missing rows admit a model. Normal drain completion removes
its cache; another process or drain receives a fresh scope. The changing
candidate's before/after assessment and promotion rules remain in force.

## Re-observation

The [native operation](artifacts/2026-09-23-assessment-reuse/verified/assessment-reuse-run.bml)
uses the two actual held-out examples, serving generation 5 and candidate
generation 152. It performs no training, serving selection or provider call.
The [report](artifacts/2026-09-23-assessment-reuse/verified/report.json) and
[events](artifacts/2026-09-23-assessment-reuse/verified/events.jsonl) retain:

| Operation | Measured rows | Reused rows | Model admissions | Elapsed |
| --- | ---: | ---: | ---: | ---: |
| Fresh serving assessment | 2 | 0 | 1 | 35,310 ms |
| Same examples in reverse order | 0 | 2 | 0 | 1,056 ms |
| Candidate adapter, same examples | 2 | 0 | 1 | 32,146 ms |

The reordered reading preserves every serialized score value and its correct
row index. Serving losses remain 1.995106428861618 and 3.6242683231830597;
the different adapter returns 1.9231765270233154 and 4.008831739425659.
All model buffers are released. These are measurements of this assessment,
not whole-session speed or answer-quality parity. GPU work was concurrent.

## Failures retained and resolved

The first preflight of `observe/native-session-learning-run.fk` exited 1:
`fs-mkdirs -> UNOBSERVED`, one unresolved call. The existing native
`host_dir_mkdir` operation replaced that nonexistent helper; preflight is clean.

The [initial reader](artifacts/2026-09-23-assessment-reuse/initial/assessment-reuse-initial.bml)
then exited 1 with `reused assessment changed a score or row order` after
31,559 ms for calculation and 1,031 ms for reuse. Its object-identity comparison
distinguished a floating-point token count from the integer restored by JSON.
The [isolated observation](artifacts/2026-09-23-assessment-reuse/initial/representation.json)
shows identical serialized bytes and loss, with count types 7 and 1. The
corrected comparison checks the entire serialized score and order; it passes.
The native check now also covers this serialization boundary. Guessed source
paths and an unmatched shell glob produced read failures; no edits or verdicts
were derived from those failed reads.

## Verification and remaining gap

The loss-evidence check retains its **63** verdict while adding changed-binding,
changed-example, altered-score, missing-record, row-order and serialization
checks. Per-row reuse provenance is checked as well. The existing failed-worker
check returns **63**: a real failing child retains its exit, releases its lock
and keeps the example pending. Full source-backed CLI preflight is clean.
Landing gates return **8191**, exit 0; no kernel sources moved. The closing
native guide reports 0 Python implementations, 2 invocation candidates and
0 unread files. Glass first frame **34 ms**, stopped intentionally with Ctrl-C
(exit 130); counsel reports **0 orphans** and 11/12 lanes unobserved without a
standing hearth. The share reading remains declared with its percentage withheld.

The verified teaching was retained as
`8ac5d791489768dc8138a595aaee5ca46f1b82958c5298a805694ccd5f937ee6`,
event `2026-09-23-assessment-reuse`. Its subsequent update is still pending.

The active learner predates this cache change; its existing work is allowed to
finish. Fresh drains take the new path. Its original 5,485-token retry has
completed all 28 forward layers; its final projection guard, gradient and
promotion outcome remain pending. The Qwen editor first claimed completion
without editing, was returned to repair by the unchanged check, and named the
required revision. Its [first completed edit](artifacts/2026-09-23-assessment-reuse/native-edit/completed-edit.json)
reduces the original 547 words to **515** and corrects the cell-lifetime claim.
The [native replay](artifacts/2026-09-23-assessment-reuse/native-edit/first-edit-observation.json)
verifies that guarded edit against the frozen original documents and reruns
the unchanged word check. It still fails the 350–450 range; the live controller
has returned to repair. This is an intermediate improvement, not a completed
answer. The [actual candidate](artifacts/2026-09-23-assessment-reuse/native-edit/candidate-after-first-edit.txt)
still reads largely as a glossary and asserts a resonance outcome that these
source lookups do not establish.

The [preceding completed coordinator turn](artifacts/2026-09-23-assessment-reuse/verified/affine-grid-completed-turn-cost.json)
used **6,554,982 tokens**, including **6,246,912 cached input tokens**. Its
provider and tool boundaries reconcile. The current open turn awaits its own
completed cost observation. These costs remain far from the requested minimum.
Native answer quality, resonance, learning correctness on the long example
and overall session parity remain open.
