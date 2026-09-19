**Finding 1 — `cacheWriteInputTokens` is excluded from the cumulative monotonicity guard**

- **Triggering event sequence:**
  1. First correlated `thread/tokenUsage/updated` notification arrives with `total.cacheWriteInputTokens = 5`. `fpn-usage` stores this snapshot in `state[3]` and sets `state[2] = 1`, `state[4] = "observed-prefix"`.
  2. Second correlated notification arrives with `total.cacheWriteInputTokens = 4` (all other cumulative fields unchanged or increased).
  3. `fpn-usage` calls `fpn-monotone(nth(state,3), total, list("inputTokens","cachedInputTokens","outputTokens","reasoningOutputTokens","totalTokens"))`. Because `cacheWriteInputTokens` is **not** in that key list, the monotonicity check passes.
  4. The state is updated to the new (lower) `cacheWriteInputTokens` value and reported as `"observed-prefix"` rather than `"cumulative-usage-regressed"`.

- **Source location:** `fpn-usage`, the `fpn-monotone` call:
  ```
  fpn-monotone(nth(state,3),total,list("inputTokens","cachedInputTokens","outputTokens","reasoningOutputTokens","totalTokens"))
  ```
  The key list omits `"cacheWriteInputTokens"`.

- **Actual check result:** The native probe reports `cacheWriteInputTokens` first=5, second=4, status=`"observed-prefix"`, gap=1. The other five fields each show status=`"cumulative-usage-regressed"`, gap=0. The single gap corresponds exactly to this omission.

- **Why it is a defect:** The intended contract states that a present cumulative quantity retains its cumulative meaning across notifications. `cacheWriteInputTokens` is validated as a non-negative integer in `fpn-valid-usage` and is reported in `fpn-report` via `fpn-quantity`, yet a decrease in that field is silently accepted and the snapshot is still labelled `"observed-prefix"`. A reader consuming the report cannot distinguish a legitimate first observation from a regressed cumulative value for this field.

---

**Finding 2 — `fpn-valid-usage` does not enforce `cacheWriteInputTokens ≤ inputTokens`**

- **Triggering event sequence:**
  1. A notification carries `total.inputTokens = 100`, `total.cachedInputTokens = 0`, `total.cacheWriteInputTokens = 500`.
  2. `fpn-valid-usage` checks `cachedInputTokens ≤ inputTokens` (0 ≤ 100 ✓) and, because `cacheWriteInputTokens` is present, only checks that it is a non-negative integer (500 ✓). No upper-bound relation to `inputTokens` is verified.
  3. The snapshot is accepted and stored.

- **Source location:** `fpn-valid-usage`:
  ```
  if json-object-has?(node,"cacheWriteInputTokens") then fpn-int(node,"cacheWriteInputTokens") else 1
  ```
  Only `fpn-int` (non-negative integer) is applied; no `le` comparison against `inputTokens` exists.

- **Actual check result:** The supplied band (exit 0, output 1) and preflight (exit 0) do not exercise a case where `cacheWriteInputTokens > inputTokens`. The native probe's bounded single-field-decrement design does not test this either. The defect is therefore **unobserved** by the supplied checks but is structurally present in the source.

- **Why it is a defect:** The intended contract defines `cachedInputTokens` as a subset of `inputTokens` and `cacheWriteInputTokens` as a separate optional quantity. While the contract does not explicitly state `cacheWriteInputTokens ≤ inputTokens`, the reader's own `input_partition` role defines uncached input as `inputTokens − cachedInputTokens`. If `cacheWriteInputTokens` can exceed `inputTokens` without validation, the reported `uncached_input_tokens` (computed as `inputTokens − cachedInputTokens`) and the separately reported `cache_write_input_tokens` can jointly imply a token accounting that is internally inconsistent for any downstream consumer that treats cache writes as a partition of input. This is a correlation defect: the reader accepts a snapshot whose fields cannot jointly describe a coherent token partition.

---

**Finding 3 — No defect supported for the remaining five fields**

The native probe confirms that `cachedInputTokens`, `inputTokens`, `outputTokens`, `reasoningOutputTokens`, and `totalTokens` are all included in the `fpn-monotone` key list and correctly trigger `"cumulative-usage-regressed"` when decreased. The `fpn-valid-usage` cross-field inequalities (`cachedInputTokens ≤ inputTokens`, `reasoningOutputTokens ≤ outputTokens`, `totalTokens ≥ inputTokens + outputTokens`) are present and correctly ordered. The correlation filter in `fpn-event` correctly ignores non-matching thread/turn pairs and fails on empty caller identity. No additional accounting or correlation defect is supported by the supplied source and observations.