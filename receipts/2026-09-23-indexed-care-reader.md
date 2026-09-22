# Less time reducing the learner's history, with the same observations

The learner's fresh history read was taking several minutes before native work
could continue. Its retained reader searched every current observation and
reconstructed composite keys for each incoming event. The repair uses Form's
existing exact-key map for `(organ, flow, aspect)`. The shared reducer still
owns observation, attention and control semantics; it receives the matching
prior observation. First-seen report order, reset behavior and diagnostics
remain part of the contract. Unchanged reads reuse the published view.

## Same real input, complete output comparison

The [original reading](artifacts/2026-09-23-indexed-care-reader/before.json)
and [repaired reading](artifacts/2026-09-23-indexed-care-reader/after.json)
ran sequentially against the actual local learning event flow:

| Observation | Original reader | Indexed reader |
| --- | ---: | ---: |
| Source bytes | 282,247,824 | 282,247,824 |
| Read duration | 324,463 ms | 203,458 ms |
| Current observations | 2,304 | 2,304 |
| Serialized current bytes | 91,369,563 | 91,369,563 |
| Primary node admissions | 0 | 0 |
| Invalid records / pending bytes | 0 / 0 | 0 / 0 |

The source SHA-256 was identical and checked before and after each reading.
The complete serialized outputs matched byte for byte, including order. The
original reader source matched commit `497946c75` exactly. This is one paired
reading on this machine: **121,005 ms less read time, about 37%**. It is not a
whole-learning, whole-session or response-quality measurement. Source sealing
and evidence export are outside these read-duration intervals.

The [native driver](artifacts/2026-09-23-indexed-care-reader/reader-gap-bounded.bml)
retains one observation at a time and compares the complete files in bounded
byte windows. Full event and current-observation content stays private. The
[original source](artifacts/2026-09-23-indexed-care-reader/reader-gap-before.source.bml)
and both entry cells are retained alongside the summaries. No provider or
answer-generation call ran in this movement.

## The failed export is part of this movement

The first command, `form-run ./fkwu .hearth/reader-gap-observe.bml` with stdin
`before`, ended with exit **137**, `Killed: 9`, before publishing a report.
Its cause of termination was not established. It supplies no before timing.
The [initial driver](artifacts/2026-09-23-indexed-care-reader/reader-gap-before-driver.bml)
remains available.

The first revised-reader execution reported a completed read in **202,007 ms**,
then accumulated **54,877,200 KiB RSS** while constructing the complete JSON
export. Codex explicitly terminated that owned process with SIGTERM; its exit
was **143**. That interrupted execution is not the successful comparison above.
The native diagnostic exchange requested the live kernel evidence; the reader
had hundreds of millions of JSON string-scanner calls. The original process's
page was already absent. No private event content entered the framebuffer.

Writing each current row incrementally completed the later paired observation.
The general JSON-view emitter's large aggregate allocation remains a separate
open boundary; this repair does not claim to have changed it. A receipt-helper
delimiter error was also corrected, and its compile check then passed. Failed
and interrupted work belongs to session cost alongside the completed readings.

## Verification and retained learning

The [extended existing care witness](artifacts/2026-09-23-indexed-care-reader/changing-reader.json)
passed 32 actual writes and read-back actions across distinct observation keys,
with complete ordered equality to the shared reference reducer. A mismatched
control preserved the prior observation and added the correlation need.
The same witness passed append, equal-size publisher renewal, malformed and
incomplete records, truncation, quiet reads and collection. Held views stayed
exact across collection. The keyed-map band returned **4,095** and native
session-memory band **1,023**, with clean preflights and exit 0. The learner
compiled. Drift gates returned **8,191 / 8,191**, zero refusals, exit 0.

The verified teaching `indexed-care-reader-2026-09-23` was retained and launched
the existing learner. Its completion and production attendance remain to be
observed. This learner uses Llama 3B; the repair does not establish changed Qwen
27B weights or a better answer. Glass first frame was **29 ms**. The closing
native guide reported zero Python implementations, two existing invocation
candidates and zero unread files. No C seed or runtime dependency was added.

## Cost and the remaining gap

The [preceding completed coordinator turn](artifacts/2026-09-23-indexed-care-reader/preceding-coordinator-cost.json)
used **9,133,758 rented tokens**, including **8,756,096 cached input tokens**
and 27,411 explicitly unattributed tokens, over 57 model calls. It is turn
`01a0ca59-bb7c-7a60-bec3-97e5d9f68c1b`; this open turn and separate provider
subprocesses are excluded. Coordination remains a major part of the gap.

The share door observed that completed turn with
`basis=observed-boundary-event-counts-v1`: 13 native, 64 local and 57 remote
events, normalized to 10 / 48 / 42. Those are event shares, not token shares or
semantic contributions.

A fresh worker still reads all history, now with indexed reduction. The live
profile identified the JSON string scanner as a large remaining source of
calls. Source discovery, care and training have their separate durations.
Useful response quality, felt resonance and whole-session parity remain open.

— Codex
