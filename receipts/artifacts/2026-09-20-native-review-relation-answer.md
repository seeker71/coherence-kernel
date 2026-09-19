**Finding 1 — `cacheWriteInputTokens` excluded from the cumulative monotonicity check**

- **Triggering event sequence:**
  1. First valid `thread/tokenUsage/updated` notification for the selected (threadId, turnId) carries `total.cacheWriteInputTokens = 5`. `fpn-usage` stores this `total` object as `nth(state,3)` and sets status to `"observed-prefix"`.
  2. Second valid notification for the same identity carries `total.cacheWriteInputTokens = 4` (all other cumulative fields also decrease by one, but the probe isolates this field).
  3. `fpn-usage` calls `fpn-monotone(nth(state,3), total, list("inputTokens","cachedInputTokens","outputTokens","reasoningOutputTokens","totalTokens"))`. Because `"cacheWriteInputTokens"` is absent from that key list, the monotonicity test never compares the 5→4 decrease.
  4. The guard `not(fpn-monotone(...))` evaluates to false, so the `"cumulative-usage-regressed"` branch is not taken. The state is updated to the new (lower) snapshot with status `"observed-prefix"`.

- **Source location:** `fpn-usage`, the `fpn-monotone` call:
  `fpn-monotone(nth(state,3),total,list("inputTokens","cachedInputTokens","outputTokens","reasoningOutputTokens","totalTokens"))`
  The key list omits `"cacheWriteInputTokens"`, even though `fpn-valid-usage` accepts it as a valid cumulative field and `fpn-report` surfaces it via `fpn-quantity(state,"cacheWriteInputTokens")`.

- **Actual check result:** The probe reports `cacheWriteInputTokens` first=5, second=4, status=`"observed-prefix"`, gap=1. The five other fields each show status=`"cumulative-usage-regressed"`, gap=0. The single gap is exactly this field.

**Finding 2 — No other supported defect**

The remaining five fields are all covered by the monotonicity key list and correctly trigger `"cumulative-usage-regressed"`. Correlation filtering (empty-identity fail, non-matching-identity passthrough), the `last`-vs-`total` separation, the `uncached_input_tokens` and `unattributed_tokens` arithmetic, and the `cache_write_input_relation: null` contract scope all behave as specified. The preflight and band checks (exit 0, band output 1) cover only their original cases and do not contradict the above.

**Conclusion:** One concrete accounting defect is supported: `cacheWriteInputTokens` is validated and reported but omitted from the cumulative monotonicity guard, allowing a decreasing cumulative cache-write count to be silently accepted as a valid `"observed-prefix"` snapshot. No further defect is supported by the supplied evidence.