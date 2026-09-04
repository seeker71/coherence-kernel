# The heed cursor and the current-source lookup — open seams

`form/form-stdlib/form-cli-heed-cursor.fk` (the bounded raw-byte cursor over the
model's decoded output) and `form/form-stdlib/form-cli-heed-current-source.fk` (the
attributed lookup against the current Form source body, ABI in
`form-knowledge-query-token.fk`) fit without a shim: `fhcs-lookup (ctx surface
local-ready)` matches the cursor's `lookupf` arity and order and returns
`(list status span reason)`. Bands re-run 2026-09-04: `form-cli-heed-cursor-band`
524287, `form-cli-heed-current-source-band` 16777215,
`form-cli-model-generate-heed-report-band` 8388607.

What is sound, and stays so:

- **Status vocabulary.** `hit` / `miss` / `nothing` map onto `OutHit` / `OutMiss` /
  `OutNothing`; an unrecognized status falls to `OutNothing`, so a future status
  degrades safely instead of entering knowledge.
- **`nothing` carries no span**, and the cursor gates independently — belt and braces.
- **No lookup before parse.** Invalid, partial, nested, empty, and over-budget all
  return before `fkss-search-at`; the one-envelope-one-search bound holds.
- **The injected span cannot re-trigger the cursor.** `fhc-resume` prefills the span
  and resets the window without feeding those bytes through `fhc-feed`.
- **Attribution for this schema is `source-path`, not `source-ref`.** `source-ref=`
  and `content-ref=` are emitted empty on purpose (a current source artifact is
  not a registered NamedCell); anything wired to test `source-ref` refuses every
  current-source hit.

## Open, re-observed 2026-09-04

1. **cuckoomark** — the retrieved answer can forge the observation boundary.
   `fhcs-render` interpolates the answer slice raw at `\nanswer:`. If that slice
   contains `<|/form:knowledge-observation|>`, the injected span carries a premature
   close and everything after it reads to the model as outside the typed
   observation — source content wearing the carrier's own structure. The corpus is
   the repository and `.fk` is an eligible extension, so the close mark is present
   in eligible files (this cell's own band among them). The repair: neutralize
   occurrences of `fkqt-observation-open` / `fkqt-observation-close` inside the
   answer before rendering and count the substitutions in an
   `answer-marks-neutralized=` field so the act is visible. The cell carries no
   such field today (corpus word: *cuckoomark* — content that speaks its own
   container's closing mark, so the reader raises the content as structure).
2. **Per-lookup scan cost is unbounded relative to the GPU work it interrupts.**
   `form-knowledge-source-search.fk` states it: query cost is proportional to the
   eligible source bytes times the atom count, byte-at-a-time in Form, while the
   resident sits idle mid-turn. `fhcs-lookup-at` takes an explicit root — that is
   the lever; a narrow root for a live run, and one measured number before the
   lookup goes in a resident loop.
3. **`fhcs-grammar-agrees` overvouches.** It compares the ABI against hardcoded
   literals, not against the cursor's `HeedOpen` / `HeedClose`; if the cursor's
   marks drift it still answers 1 and every lookup answers `nothing` forever with
   no status naming it. `fhcs-grammar-agrees-at (open close)` exists; the caller
   should hand it the cursor's own marks, as `form-cli-heed-fkqt.fk`'s
   `fhq-grammar-agrees` does.

Healed and standing:

- `answer-truncated=` is computed from the actual cut, not hardcoded.
- `fhcs-render` clips to `fkqt-max-render-bytes()`: the answer slice is taken with
  the remaining fuel after the prefix and close mark, and `answer-render-truncated=`
  names the cut. For the cursor the rendered length *is* prefill positions, and it
  is bounded.
- An over-budget query is a named observation, not silence. Once an open mark is
  seen the cursor holds from it (never clipped from the left); outgrowing
  `fhm-hold-cap()` answers `nothing` with reason `query-budget-exceeded`, no IO.

## Minimal safe wiring for the resident loop

1. Wire `fhcs-lookup` directly as the cursor's `lookupf`. No shim.
2. Sanitize the answer for the observation marks (1) — the one not to go live without.
3. Use `fhcs-lookup-at` with a narrow root for a live run, and time one lookup (2).
4. Keep the cursor budget at `MaxHeeds = 2` while the cost is unmeasured.
5. Read `source-path`, not `source-ref`, for this schema.
6. Then (3), an honesty repair, not a blocker.

Compatibility is not safety: the pieces fit better than the pair is safe, and the
clean fit is exactly what would carry a boundary forgery into the model's context
on the first realistic query. A reachability check on a retrieval hazard costs one
grep, and the corpus being the repository means every mark we invent becomes
findable content the moment we write it down.
