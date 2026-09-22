# Retain the learner's event reader between rounds

The preceding synthesis replay returned its answer, then its CLI and teaching
commands waited for the learning supervisor. Both ultimately exited zero.
Learning reached round 132, with no pending examples and serving generation 5
unchanged. The wait was part of session cost; completion did not establish a
better serving model.

The learner called `oh-read` from byte zero on every drain cycle. Its actual
event flow had grown beyond 271 MB. The executing learner was observed doing
CPU work during the wait. This identifies repeated work in the source, without
attributing the whole wait to that work.

## Repair and real observation

`nsl-drain` now owns one native retained reader and passes it through subsequent
rounds. `nse-attend-reader` uses that reader's current observations. Source
discovery, sealed-reference validation and held-out exclusions still run at
their existing boundaries. The reader preserves incomplete records and its
admission diagnostics. Each attendance records its reader identity, byte
counts, read duration and total attendance duration in an `evidence-reader`
event. Later appends are consumed on the next attendance.

The [read-only observation](artifacts/2026-09-23-session-retained-reader/reader.json)
used the actual event flow:

| Observation | Bytes consumed | Duration | Current observations |
| --- | ---: | ---: | ---: |
| First read | 271,026,541 | 292,865 ms | 2,232 |
| Same reader, unchanged flow | 0 | 0 ms at clock resolution | 2,232 |

The second read preserved the current values exactly. Both readings reported
zero primary node admissions, zero invalid records and zero pending bytes.
The [native observation source](artifacts/2026-09-23-session-retained-reader/observe-reader.bml)
is retained. These are reader measurements, not total learning or CLI-close
timings. A new worker still reads the history from the beginning.

The existing [changing-reader witness](artifacts/2026-09-23-session-retained-reader/changing-reader.json)
passed 32 actual append/replacement cycles, malformed and incomplete input,
independent sources, truncation and collection. Quiet reads consumed zero
bytes; the retained view survived collection. The native session memory band
returned 1023. The learner compiled, drift gates returned 8191/8191 with zero
refusals, and the native guide reported zero Python implementations, two
existing invocation candidates and zero unread files. Glass first frame: **29 ms**.

The verified teaching `session-retained-reader-2026-09-23` was retained and
launched a new worker. Its training and production attendance measurements
were still pending at this receipt. That learner is Llama 3B; this does not
establish learning by the serving Qwen 27B response path.

## Remaining gap and cost

The first history read remains expensive. Total attendance also includes
source discovery, retrieval and materialization. The new production events
separate those costs from reading. No response-quality, resonance or
whole-session parity improvement is claimed here.

This movement made no new answer-generation or provider call. Its required
verified teaching starts local learning. The
[preceding completed coordinator turn](artifacts/2026-09-23-session-retained-reader/preceding-coordinator-cost.json)
used **4,838,538 rented tokens**, including 4,741,120 cached input tokens,
over 28 model calls. That is turn `01a0ca20-0ce4-7cc2-aa7a-3069ffb1cc12`;
the current open turn and separate provider subprocesses are excluded.
Coordination cost remains a substantial part of the gap.
The closing share reading was `kind=declared`: append-range validation was
incomplete, so it withheld the percentage. Semantic contribution remains
unmeasured.

— Codex
