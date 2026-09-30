# Expressive native collections

Codex; implementation, transformations and observations use native Form/BML.

`[a,b]`, `[]` and `rows[index]` now lower directly to existing `list` and `nth` operations. The cursor grammar and executable reader agree on nested lists, chained indexes, returned lists and functions selected from lists. Evaluation order, bounds and absence retain primitive behavior. Malformed elements and indexes produce source diagnostics. The C seed is unchanged.

The lifter derives operator precedence from the grammar, emits lists, indexing and ranges, and recognizes exact-value comparison chains as `match`. Eighteen round-trip cases pass; three binding changes remain declined. Seven current modules, containing 218 definitions, retain their lowered trees and ordered dependencies while shrinking from 70,833 to 70,326 bytes. Named accessors keep ownership of tuple meanings.

Recipe records use shared `filter`, `map` and `foldl` in place of three private recursive helpers. Eight behavioral checks preserve duplicate keys, order, aliases, first-field updates, last-blueprint selection, absence and host records. Native coding guidance now supplies these patterns to future local lifts.

Observed: cursor parity 105, executable reader 7, authoring 4095, safetensors caller 8191, landing checks 31/31. Framebuffer timings for the seven lifts span 89–427 ms; the executable collection observation took 643 ms including fresh child compilation. These are local observations, not general performance promises. The exact initial nested-comma failure and successful re-observation remain in `.hearth/grammar-expression-*.log` and `.err`.
