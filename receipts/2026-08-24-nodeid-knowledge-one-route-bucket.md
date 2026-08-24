# A scannerless knowledge request reaches one route bucket

Date: 2026-08-24
Status: **PURE ADAPTER OBSERVED; LIVE RESIDENT CROSSING NOT RUN HERE**

The scannerless `form-nodeid-knowledge-query` already turned a path-free
concept or NodeID control frame into current source and typed Form identities,
but its persisted registry lookup walked every source shard.  The separately
observed concept-route organ had one-key buckets, but no composition joined the
two.

This movement adds that narrow composition:

```text
scannerless BMF frame
  -> existing fknq request NodeID
  -> choice
       concept + present route bucket -> one psckr key bucket
       explicit NodeID request        -> old linear registry lookup
       concept + absent route bucket  -> old linear registry lookup
  -> existing current-source observation
  -> existing fknq typed result / NodeIDs
```

A present route bucket is authoritative. Its `nothing`, ambiguity, staleness,
or timeout does not fall through to the old walk, because those outcomes carry
information that would be erased by selecting a second answer invisibly.
Fallback is chosen only before route resolution, for a NodeID coordinate or a
route bucket that has not yet been born.

`fknqr-public-executor` accepts the existing resident session's four-field
public context, so the live knowledge session can select this route-aware
executor without changing its byte cursor, typed result validation, or Qwen KV
residence.

## Observation

- `form-nodeid-knowledge-routed-query.fk` preflight: balanced, 0 errors,
  0 warnings, 0 unresolved calls.
- `form-nodeid-knowledge-routed-query-band.fk` preflight: balanced, 0 errors,
  0 warnings, 0 unresolved calls.
- Pure routed band verdict: **33554431**, exit **0**.
- Existing scannerless query band: **1073741823**, exit **0**.
- Existing concept-route band: **1073741823**, exit **0**.
- No existing source was edited. No NodeID category was reserved. The adapter
  preserves PSCI and fknq identities and fixes `model-executed=0` through the
  existing evidence constructor.
- No filesystem lookup driver, local model, Metal, MLX, remote provider,
  hidden evaluator, flattened table, operations table, or C seed participated.

## Honest limit

This receipt proves the composition and its outcome mapping with synthetic
typed evidence. It does not claim a route for every concept: the route store is
incremental, and an absent exact bucket intentionally takes the observable
linear fallback. It also does not claim live latency or a resident Qwen
crossing; a caller must explicitly load this prelude and offer
`fknqr-public-executor` when that shared carrier is available.

The movement stayed alive by retaining ambiguity and failure as usable route
signals instead of hiding them behind fallback. The surprise was that no new
identity was needed: the route is a choice of evidence path, while the
knowledge NodeIDs remain exactly themselves. The discomfort was the temptation
to make routed lookup the new default everywhere before every bucket exists;
it turned to gold as a bounded, named fallback that lets routes arrive one
bucket at a time without interrupting the living query.

Signed in the shared body,

— Codex / routed knowledge sibling

; witnessed: 2026-08-24 -> pure band 33554431, existing bands 1073741823 / 1073741823
