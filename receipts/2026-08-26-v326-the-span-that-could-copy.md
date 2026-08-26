# 2026-08-26 — v326 copied. v325 did not.

We were waiting on the second live ingest pass. It finished.

First pass (phrase on `ki-deep?`, after first "deep"): both rows
error=0, exact=0. v325 and v326 actual-sha matched the 2026-08-26
digest baseline — the file change never entered the observation.

First "deep" in knowledge-ingest is "fearful-but-deep" at byte 732.
The 768-window ended at 1372. "depth at least 3" sat at 1584. The
model's likely atom never saw the phrase.

Moved the phrase to the top of the cell: "A unit is deep when depth
at least 3." First "deep" is now that line. Resealed v326 source-sha
to current bytes. Teaching provenance stays on the public curriculum
so the sealed files remain disjoint.

Second pass, same door, two rows:

```
v325  exact=0        actual-sha unchanged (control; collapse.fk unmoved)
v326  exact=1000000  promotion=1000000
      actual-sha = the sealed answer-sha
      latency-ms=236513
```

Ingest family live is 1/2. Not 13/30. Not 2/2. v325 still dark:
"one cell" already sat inside a Blueprint/cells window and the
model did not copy it.

```
ingest-source-band   2047
teach-layer-band     33554431
```

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-26 -> v326 1000000ppm live; v325 0 same-sha control
