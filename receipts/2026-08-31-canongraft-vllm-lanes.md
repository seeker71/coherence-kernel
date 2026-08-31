# 2026-08-31 — canongraft: the vLLM dashboard canon, translated into the glass

Urs pointed at vLLM's open-source monitoring for inspiration. Read the
canon (docs/design/metrics, Grafana dashboards 24756/25237, the
production-stack panels), then translated organ for organ — every lane
fed by a tap the hearth already speaks:

- request hopper (running/waiting/swapped) -> waiting = task turns
  minus reply turns; live: waiting=0
- KV cache usage %% -> newest reply pos over maxpos: a colored shade
  gauge; live: 35%% pos=1463 — crowdfade now has a vLLM-style panel
- TPOT / inter-token -> newest elapsed/tokens; live: 1843ms yellow
  (thinking included — honest, not flattering)
- e2e latency percentiles -> nearest-rank p50/p95 over all reply
  latencies; live: 16571/16589ms
- TTFT, prefix-cache hit rate, speculative acceptance -> named, unfed:
  TTFT wants request->first-checkpoint pairing in the ledger; prefix
  hits want ice-load taps; speculative does not exist here.

Band 63 -> 1023 (collect fold, nearest-rank percentile, newest-of,
gauge geometry). Two truth-warts caught by the band and the live pane:
naive percentile indexing (fixed to nearest-rank, the vLLM convention)
and the gauge showing the OLDEST pos (collect conses newest-first;
gl-newest now reads the head).

Teaching: the canon's value was not its panels but its VOCABULARY —
naming waiting/fill/TPOT let the hearth see that it already spoke all
three. Discomfort→gold: TPOT arriving yellow-slow (1843ms/tok) begged
to be excluded from the pane; it stays, because the instrument is for
the body's real face.

; witnessed: 2026-08-31 -> glass band 1023; live lanes hopper/kv-fill
; 35%/tpot/p50-p95 real; row 1184 canongraft
