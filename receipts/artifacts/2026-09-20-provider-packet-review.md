Two supported defects:

1. **Cumulative cache-write regressions are accepted.**  
   Location: `form/form-stdlib/bml/form-cli-provider-notification.bml`, `fpn-usage` monotonicity check.

   Trigger: Deliver two notifications matching the selected thread and turn. Both have valid `total` and `last` breakdowns. Keep the five required cumulative counters unchanged, but change explicitly reported `total.cacheWriteInputTokens` from `30` to `10`.

   The monotonicity key list omits `cacheWriteInputTokens`, so the second snapshot replaces the first, reports `10`, and retains `observed-prefix`. A known cumulative counter has decreased without triggering regression detection. Compare this optional counter when both snapshots explicitly provide it; absence must remain unknown.

2. **Large integer usage is rejected on substrates using type 5 integer cells.**  
   Locations: `form/form-stdlib/bml/form-cli-remote-token-evidence.bml`, `frte-json-int-node?`; called through the notification reader’s `fpn-int`. Supporting representation: `form/form-stdlib/json.fk`, `json-emit-leaf`.

   Trigger: On a proof sibling using the documented type 5 representation for integers beyond 32 bits, deliver a correlated notification whose cumulative input and total are `4294967296`, other counters are zero, and `last` is a valid small breakdown.

   These quantities fit the supplied schema’s int64 fields. However, the admission helper accepts only types 1 and 6. It rejects the snapshot as `invalid-usage-snapshot`, leaving quantities unknown or freezing an earlier snapshot. The JSON emitter explicitly supports type 5 integers. This finding concerns those substrates; the supplied source says fkwu retains type 1 for integers.

Supplied actual checks:

- Preflight for `form/form-stdlib/tests/form-cli-provider-notification-band.bml`: **exit 0**, balanced delimiters, **0 errors, 0 warnings, 0 unresolved calls**, clean chain; output `0`.
- Band: **exit 0**; output `1`.

The fixtures cover neither explicitly reported cache-write regressions nor type 5 integer admission. No tools, workspace inspection, startup, or additional checks were performed.