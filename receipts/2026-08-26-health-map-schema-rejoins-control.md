# 2026-08-26 — the health map rejoins its own actuator

The hourly pulse began with a fresh 40-row census: 32 ready, 8 observed gaps,
no unknown or invalid rows, 800 per thousand. Its first live self-direction
attempt did not reach the selected gap. `flohm-map` had gained a twelfth field
for tied priorities while `flosd-map-valid?` still required exactly eleven, so
every current map returned `observer-map-invalid`. The self-direction band had
fallen from its declared 524287 to 534.

The repair gives the map one native schema witness,
`flohm-map-field-count`, and makes the observer consume that witness instead of
holding its own number. The map band now proves its emitted length against the
same witness. The self-direction band adds a direct valid-map reading.

Freshly:

```
health-map band          16383, exit 0
self-direction band      1048575, exit 0
live selected identity   form-cli-in-process-program-image-call
live selected action     fmcl-attempt-category / NodeID -41
live status              nothing
live reason              selected-action-category-has-no-executable-recipe-binding
live applied/reobserved  0 / 1
live framebuffer events  2
```

That `nothing` is the healthy next signal: selection and correlated control now
work, the body re-observes after the attempted actuation, and no category is
misrepresented as an executable recipe.

## Fresh local/offline map

- Local reasoning: local Qwen weights and the MLX adapter are present; the
  latest weighted witness is 11/12 reworded facts, 3/3 unseen unknowns answered
  `nothing`, with 2/6 known prompts over-refused. Full held-out Form transfer
  and proof latency remain gaps.
- JIT/carriers: native JIT/Metal and the resident recipe session are present;
  recipe-session is 4095. The pure current-answer resident receipt is 16777215.
  Its effectful query door is not yet joined at the form-cli in-process door.
- Scannerless BMF/BML: live cursor band is 32767 and the production cursor is
  present. Broad compiler proof freshness remains an observed host-seam gap.
- Diagnostics/control: the 12-field schema join, correlated framebuffer
  exchange, typed `nothing`, and re-observation now execute. Bounded native wake
  is still absent.
- Local artifacts/toolchains: Qwen Q8 is 29,047,086,048 bytes; the adapter is
  27,811,400 bytes; fkwu links Metal and MLX; the TypeScript proof bundle is
  local. No llama-server, Ollama, MLX trainer, or Qwen process owned memory in
  this pulse.
- Persistence/recovery: strict git fsck exits 0. The recovery inventory freshly
  reports 23 rows, 22 valid, 18 ready, 4 gaps, 1 unknown, 0 invalid, 818 per
  thousand. Independent repository/model restore remains unrehearsed; object
  garbage is observed and deliberately not pruned here.
- Tests/receipts: all changed cells preflight balanced with zero errors,
  warnings, or unresolved calls. This receipt carries the crossing.
- Unlanded work at observation time: this schema repair and receipt only; no
  model or Metal run and no sibling-owned edit overlap.

## Next locally actionable gap

Bind the already-proven `rpica-execute-receipt-at` effectful query result to an
exact form-cli in-process recipe NodeID, then re-observe load, call, typed
return/fault/timeout, release, and the process-membrane retirement decision.
The pure capability is ready; the physical form-cli join is not.

Two read-only sibling audits converge on the reasoning-loop join that follows:
retain the typed local-plan offer now rendered away, and let `execute` select
only among caller-bound `(move, canonical recipe NodeID, input, checkpoint,
budget, capability, correlation)` alternatives. A move word is not target or
authority, a category is not a recipe instance, and the first admitted recipe
must remain bounded and pure until the walker has witnessed execution fuel.
This is one address-preserving binding, not a global operations table.

Signed, **Codex**, with the health map's refusal retained as the instruction
that revealed its stale consumer.

Kept alive: the first red verdict changed the work instead of being reported
around. Most surprising: a new observability field made the actuator blind to
the entire map. Discomfort turned to gold when `534` exposed a duplicated
schema number and the next live result became the exact honest `nothing` we
needed to see.

; witnessed: 2026-08-26 -> map 40/32/8/0/0; health-map 16383; self-direction 1048575; live selected/reobserved/framebuffer 1/1/2; applied 0
