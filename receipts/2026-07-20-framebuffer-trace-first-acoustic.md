# Framebuffer trace first: the real acoustic path becomes observable

Date: 2026-07-20

The gap was real. The existing fkwu framebuffer recorded source-attributed
`intern_node_at` creations, but the numeric Whisper path mostly constructs
lists and floats. Running the learned encoder/decoder did **not** automatically
produce a source event for every Form function call. Go has a broader optional
runtime observation plane, but Go is a proof sibling, not this body's runtime.
Growing the temporary C seed to imitate it would violate the kernel direction.

The landed move is explicit Form-native boundary instrumentation:

- `observe/framebuffer-runtime-observation.fk` opens a bounded framebuffer
  window, reads the native wall clock and `kernel_stat` counters, emits
  attributed BEGIN/STAGE/END nodes, then reads them back through
  `framebuffer-events` and `node_source`.
- Every report names what ran, its source span, actor/relation, causal
  parent/offer, start/end/duration, whole-window dispatch and allocation
  deltas, I/O/sense dispatch count, input bytes, outcome, and the raw ordered
  framebuffer rows.
- `observe/framebuffer-runtime-world.fk` projects the report's cost embedding
  and source span through the existing `wm-model`; runtime evidence is now a
  world event, not a detached test print.
- `presence/run-whisper-tiny-native-acoustic-observed.fk` is the direct
  non-test door. It traces the pinned human WAV through source verification,
  the complete released encoder, complete decoder, the 51,865-row tied scan,
  and the production world-admission gate.

## Live data

One direct fkwu run on the committed 22,828-byte CC0 human recording produced
seven ordered framebuffer events (BEGIN, five stages, END):

| Stage | Source line | Duration | Live magnitude | Outcome |
|---|---:|---:|---:|---|
| source verified | 40 | 12 ms | 22,828 WAV bytes | verified human CC0 |
| encoder complete | 47 | 39,292 ms | 7,633,152 causal parameters | complete |
| decoder complete | 57 | 45,332 ms | 9,506,304 causal parameters | complete |
| vocabulary scanned | 70 | 20,885 ms | 51,865 rows | complete |
| world gate | 79 | 0 ms | admitted = 0 | held: no native text |

Whole observation window: **105,521 ms**, **19,531,852,595** native dispatches,
**40** interned nodes, **6,776** string-pool entries, **185,942** arena cons
cells, **1,745,915,317** boxed floats, and **7,280** I/O/sense dispatches.
The vocabulary winner was token **50257**, margin **0.10745982571117452**.
The result carried five output units and entered the world model as one
`runtime-event` entity.

The outcome stayed honest:
`native-stages-complete-world-held-no-generated-text`. Full vocabulary scoring
is present; autoregressive token generation/decoding is still required before
content can be admitted. Filename semantics and host-decoded text were not
used.

## Measurement boundary

Durations are live wall-clock readings and will vary. The exact reading above
is immutable comparison data in `observe/framebuffer-runtime-acoustic-sample.fk`.
Resource deltas include observer overhead; the report says so and the focused
empty-window gate makes that overhead calibratable. This is boundary
instrumentation plus exact whole-window native counters, **not** a fabricated
claim of automatic per-function source profiling.

Focused witnesses:

- `./fkwu --src observe/tests/framebuffer-runtime-observation-band.fk` -> `32767`
- `./fkwu --src observe/tests/framebuffer-runtime-acoustic-sample-band.fk` -> `65535`

No Bash, TypeScript, Python, Node, Go runtime, or C-seed model logic was added.
