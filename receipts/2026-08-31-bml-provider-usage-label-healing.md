# BML call-local usage survives a changed transport label

Authored by Codex on 2026-08-31.

The private rollout did contain provider call-local `last_token_usage` rows.
The live collector withheld them because it admitted only the historical
payload label `token_count`. The lower-level label changed; the semantic
call-local usage object did not.

`form-cli-remote-token-evidence.bml` now owns the marker and the admission
predicate. The collector preserves `token_count` as an event-count detail,
but admits any row carrying the BML `last_token_usage` shape. Its active
pressure reader uses the same predicate, so the dashboard cannot disagree
with the collector about what a provider usage row is.

## Receipt

```text
./fkwu form/form-stdlib/bml/form-cli-remote-token-evidence.bml
-> 0

./fkwu form/form-stdlib/tests/form-cli-remote-token-evidence-band.fk
-> 262143

./fkwu form/form-stdlib/tests/form-cli-turn-evidence-live-band.fk
-> 17406
```

The live parser band preflights with balanced parentheses, zero errors, zero
warnings, and zero unresolved calls. It supplies a `provider_usage` row with
call-local quantities and proves that BML shape enters both accumulated state
and last-call pressure without a `token_count` label.

The now-live share panel reports one open-call pressure row:

```text
input=221419  cached-input=219904  uncached-input=1515
cache-write-input=0  output=1117  reasoning-output=345  total=222536
```

This is an observed provider-pressure fact, not a settled same-task baseline.
The exact original goal still has a zero-provider local Form answer receipt,
but this open provider call is not a comparable completed copy of that task.
The requested 10% ratio therefore remains `nothing`, not an inferred zero or
percentage.

I kept the exchange alive by trusting the call-local marker over a stale
transport label. The surprising teaching is that the remote spend was present
all along, yet invisible because the collector confused an old event name for
the meaning. The discomfort became one BML semantic door shared by history,
live pressure, and the future settled receipt.
