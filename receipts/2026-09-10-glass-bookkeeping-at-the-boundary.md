# Glass bookkeeping at the boundary

Codex, 2026-09-10. The running renderer (PID 28339) in the saved checkout
showed 97.3% CPU; a one-second host sample reported a 25.4 GB footprint and
84 top-of-stack samples in `fk_live_note`. This is evidence of an observation
bottleneck, not a claim that all of that memory comes from one cause.

Every `kernel_stat` call entered `fk_live_note`, which scanned the append-only
gift handle table, including every released slot. Keys 41 and 42 scanned it
again. Map/release churn therefore made an unchanged counter increasingly
expensive to observe.

The temporary carrier now updates active mappings and bytes on successful map
and close. All internal page/roster readers and the public release door share
one close operation. Observation reads those counts directly. No handle reuse
was introduced: stale handles remain stale. The patch removes four C lines;
the shrink path is the Form-owned mapping lifecycle and shared observation
ledger, leaving only OS map/unmap effects in a carrier. This does not add runtime
policy to C or claim that the append-only handle table is now bounded.

On this host, the same native measurement cell made 20,000 counter reads before
and after 20,000 physical receive/release cycles:

| Carrier | Before churn | After churn |
| --- | ---: | ---: |
| Before repair | 6 ms | 129 ms |
| Repaired | 5 ms | 5 ms |

The active count was identical before/after churn. The actual mapping lifecycle
band returned 1023, gift-frame band 4095, shared-memory wait band 255 (20/20
ten-millisecond rests within its existing bound; changed-sequence wake 201 ms
after the child's 200 ms publication delay). Drift gates returned 8191/8191.
The earlier standalone physical collector profile measured 244 ms total,
including 168 ms storage; that cold collector is not the resident's cached
shared-memory frame path and is not an end-to-end screen measurement.

Surprise: asking what was still mapped revisited everything already unmapped.
The expensive observation became a precise lifecycle repair. The living
screen still needs re-observation after its old process is replaced; these
numbers alone do not certify screen latency.
