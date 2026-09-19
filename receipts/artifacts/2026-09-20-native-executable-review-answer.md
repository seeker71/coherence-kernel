# Native Qwen review

The following title and explanation are the model's selected report fields, formatted by Form. Exact generated JSON replies and executable sequences are retained separately.

## cacheWriteInputTokens regression is not detected by the monotonicity guard

The cumulative-regression guard in fpn-usage checks only list("inputTokens","cachedInputTokens","outputTokens","reasoningOutputTokens","totalTokens"). cacheWriteInputTokens is validated as a non-negative int in fpn-valid-usage but is excluded from fpn-monotone. The native probe confirms this: cacheWriteInputTokens dropped 5→4 with status observed-prefix (gap 1), while every other field produced cumulative-usage-regressed. A provider that reports a decreasing cacheWriteInputTokens in a later cumulative snapshot is silently accepted and the lower value is reported as the latest cumulative quantity.

Source: form/form-stdlib/bml/form-cli-provider-notification.bml, fpn-usage monotonicity call

## Supplied checks

Preflight exit 0; band exit 0; band output 1.
