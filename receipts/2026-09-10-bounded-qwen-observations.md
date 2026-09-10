# Qwen observations retain their state across bounded submissions

An earlier native review observation queued 10,510,885 Metal dispatches in one
command buffer. A live debugger reading found it Scheduled, with no reported
error and no deadline. That is evidence about that wait, not proof that the
whole GPU was unavailable. The oversized attempt was explicitly retired after
the replacement traversal passed its checks; its process exited 143.

The resident session now submits observations at the existing admission span
(64 positions), retaining every original ID, its order, and the same stream
state. Per-position barriers remain. The experimental barrier-free positive
chunk path is not used. Intermediate head predictions are discarded, never
inserted into context. A ready stage requires a valid position and prediction
plus no pending, in-flight or shelved Metal work. A failed partial observation
cannot resume; its context remains owned for release, not claimed rolled back.

The 17 pure checks cover exact order and positions through 10,000 IDs, boundary
sizes, partial failure, bad positions, empty input, invalid spans and unsettled
GPU counters. Fresh preflight is clean; the band exits 0 with 131071. The
existing session band remains 4095, exit 0.

The real Qwen3.8-27B Q8 comparison admits weights once and allocates two stream
states. Each receives the same nine-position prefix, then 65 public synthetic
IDs at position 9. One takes the former whole-span route; the other crosses the
new 64-position boundary. Actual results:

| Observation | Whole span | Bounded span |
|---|---:|---:|
| Final prediction / position | 198 / 74 | 198 / 74 |
| Dispatches | 82229 | 82233 |
| Synchronizations | 2 | 3 |
| Wall time (ms) | 100192 | 116638 |
| Finite logits | yes | yes |

Logits and all retained state buffers across all 64 layers are byte-identical.
Release counts match the owned handles and leave zero buffers. The physical
comparison exits 0, verdict 255. It writes numeric stage evidence under
`.hearth/qwen-observation-parity/`, so an interrupted observer does not lose
completed stage readings. No model output or evaluation example enters training.

```
./fkwu form/form-stdlib/tests/form-cli-model-observation-band.fk
./fkwu form/form-stdlib/tests/form-cli-model-session-band.fk
./fkwu observe/qwen38-observation-parity.bml  # stdin: qwen38-q8
```

This short comparison was slower through the new boundary. It establishes
state preservation and bounded submission, not throughput improvement, broad
language quality, repaired end-to-end review, or 95% local-session equivalence.
Long observations receive progress between submissions, but their total latency
still needs work. The implementation adds no C or foreign-language runtime.

All 13 drift gates passed, 8191/8191. Glass's bounded reading took 36 ms and
showed 24/24 held findings, with 392 unread rows. That remains a partial reading.

The surprising lesson was that smaller submissions can preserve every byte
without improving a short run's time. The long wait became a measured native
boundary; its remaining cost stays visible rather than becoming a speed claim.

Signed, Codex.
