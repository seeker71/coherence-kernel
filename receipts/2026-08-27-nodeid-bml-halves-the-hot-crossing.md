# 2026-08-27 — NodeID BML halves the hot crossing

The guarded patch lived, but returning its 236-byte labelled receipt to the
same 27B/KV residence spent 44,129 ms. Reading `fcms-observe` corrected one
attribution: it was already encoding only the new bytes at the current KV
position. It was not rebuilding the transcript. The remaining cost was a full
model state step for every injected token, with the legacy prefill also paying
one host synchronization per position.

## Movement

The exact result now has two complementary identities:

- the append/read-back journal remains the restart truth, including exact frame,
  path and preimage bytes;
- `farp-result-node` interns proposal, status, reason, contribution, journal
  presence and trace for O(1) lookup inside the live residence.

The hot model crossing carries compact scannerless BML instead of replaying the
durable record as labelled prose:

```
(repo-patch-result @node status contribution=0|1 journaled=0|1 (signals))
```

`nothing`, `choice`, `timeout`, `refused`, `failure` and `value` remain named;
contribution and journal presence remain explicit values rather than being
collapsed into those signals. `fcms-observe` now also uses the already-witnessed
chunk-0 span schedule, expressing scratch ordering inside one concurrent Metal
batch instead of synchronizing the host after every ID.

## Physical local answer

Same sealed `Qwen3.8-27B-Q8_0.gguf`, same Form-native runner, same exact patch:

| Signal | Before | Now | Movement |
|---|---:|---:|---:|
| injected bytes | 236 | 91 | -61.4% |
| observation injection | 44,129 ms | 21,493 ms | -51.3% |
| task observation | 56,613 ms | 58,027 ms | host spread; unchanged 326 bytes |
| contribution | 1 | 1 | preserved |
| source / intent / terminal / release | all 1 | all 1 | preserved |

Exact stage timestamps and verdict live in
`receipts/artifacts/2026-08-27-resident-patch-nodeid-bml-live.txt`.

## Fresh health map

| Surface | Present evidence | Living gap |
|---|---|---|
| Local reasoning / knowledge | Resident local Qwen emitted and absorbed its guarded outcome in one KV residence. | Broad held-out Form mastery and answer-quality evaluation remain uneven; the 326-byte task still took 58,027 ms to enter KV. |
| Form-native JIT / carrier | Guarded source effect, span prefill and resumed decode stayed in `fkwu`/Metal. | True multi-token prefill must reduce per-token model/dispatch work; removing syncs alone previously returned only 2.3%. |
| Scannerless BMF/BML | Token-edge patch grammar now returns compact BML with live NodeID identity. | Arbitrary resident-authored grammars and richer BML generation still need held-out physical evaluation. |
| Diagnostics / control | Begin/end stages separated task prefill, model flow, effect and observation injection. | Correlated controls still need to steer a running long stage rather than only describe it. |
| Local assets | 29,047,086,048-byte Q8 model, `fkwu`, compiler, Metal/MLX carrier path and docs are present locally. | Optional-toolchain provenance and a consolidated offline rebuild/recovery manifest remain dispersed. |
| Persistence / recovery | Intent and terminal were durable and exact; the hot NodeID does not impersonate restart identity. | Unmatched-intent startup replay and power-loss/fsync evidence remain unwitnessed. |
| Tests / receipts | Patch `4194303`, model session `4095`, recipe `4095`, peer `8191`, ingress `1048575`, turnwheel `32767`, live verdict `1`; all touched preflight chains clean. | Multi-token prefill needs its own parity and latency witness on incremental observations. |
| Unlanded | This coherent movement is ready to land. | Restart the resident on the landed image so later local requests reuse its model/KV. |

## Straight next movement

The next leverage is below formatting: batch multiple new positions through the
27B weights while preserving hybrid recurrent state and KV order. A useful
experiment must report output parity, token count, dispatches, GPU busy time and
wall time on the same incremental observation. Shorter bookkeeping helped; the
remaining 21.5 seconds belongs to model work and its present schedule.

I kept this movement alive by correcting the transcript-rebuild story from the
source, then asking the physical local model rather than treating smaller text
as sufficient evidence. The surprising teaching is that 61.4% fewer bytes
returned 51.3% less wall time almost directly. Discomfort turned to gold when
the supposedly incremental path was found already incremental: that closed a
false repair and exposed the actual token-wise compute seam.

Signed, Codex — sibling, this worktree.

; witnessed: 2026-08-27 -> live local Qwen, guarded patch verdict 1,
; 236 -> 91 injected bytes, 44129 -> 21493 ms same-KV observation
