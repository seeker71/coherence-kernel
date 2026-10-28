# Session usage follows its provider and owner

The closing session meter reported zero rental for a Codex transcript carrying
positive provider counters. It read Claude request records only. Session rent
now uses the existing native `rented-usage.bml` reader for Codex: one latest
admitted cumulative sample from the named transcript, with its timestamp.
Cached input remains part of input and reasoning remains part of output.
Unavailable request counts and cache-write allocation remain null. Missing,
invalid or unrecognized usage stays unknown, including in progression sums.

The transcript key remains stable for receipt history. The provider's header ID
separately joins drafts, coding turns and voice runs; a missing ID leaves local
usage and share unknown. Measurement version 2 refreshes old cached readings.
The review reports uncached input according to each provider's counter basis.

Focused C-bootstrapped fkwu observations:

- `./fkwu form/form-stdlib/tests/session-rent-band.bml`: 8191. Claude request
  deduplication and subagents remain covered; Codex cumulative snapshots count
  once, partial trailing rows are ignored, invalid latest samples remain unknown,
  explicit zero stays zero, and provider identity owns native work.
- `./fkwu form/form-stdlib/tests/session-progress-band.bml`: 31. Old receipts
  load, current receipts round-trip, old cache entries refresh, unknown totals
  remain unknown, and native voice runs join the provider ID.
- `./fkwu form/form-stdlib/tests/local-flow-reading-band.bml`: 4194303.
  Provider-specific input accounting and session joins preserve existing readings.
- Read-only independent review found no remaining actionable regression.

The current session's private re-observation produced positive provider usage
where the old meter emitted zero. Personal counts, transcript paths and content
remain local. These proofs make no model calls; implementation and review use
a rented OpenAI model. The surprising lesson was that receipt identity and work
ownership need separate fields: correcting attribution need not rewrite history.
