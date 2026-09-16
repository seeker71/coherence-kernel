# Response cost and constraint-preserving review

Codex, 2026-09-16. Movement follows the shared gap loop: observe, resolve,
re-observe, embody. The goal remains whole-session quality and throughput with
minimal rented usage. This receipt covers one matched evidence-review task.

## Observed pair

Both sides received the same learning evidence, policy and four frozen report
checks: retain serving generation-5, identify the greeting regression, leave
general quality parity unproven, and preserve unknown rented usage as null.
Independent native checks accepted both reports. The native review returned
unchanged source documents. Assessment examples remain outside training.

| Run | Provider input | Cached subset | Provider output | Elapsed |
| --- | ---: | ---: | ---: | ---: |
| Local Qwen coding loop | 0 | 0 | 0 | 381,326 ms |
| Codex baseline, completed | 317,519 | 290,816 | 1,553 | 75,145 ms |
| Codex baseline, failed setup | 47,074 | 33,792 | 465 | retained separately |

The local zero belongs to the audited native-only coding execution boundary.
It excludes this coordinating Codex session. The baseline was launched by
Form's process organ, with context and Form access. Its first attempt stopped
when the read-only sandbox denied `form-run`'s temporary directory. That cost
is retained; the retry allowed workspace/cache writes while its review task
remained source-read-only. Cached input is already inside input; reasoning is
already inside output. These are token volumes, not a monetary bill.

Local Qwen generated 582 token IDs and ingested 794 IDs during its movement.
It reached the correct decision, but its next action did not explicitly carry
forward the held-out exclusion. The Codex report did. Four factual assertions
therefore established only the checked behavior, leaving an actionable prose
quality gap visible. Qwen's suggested tuning around held-out examples needed
clearer preservation of assessment independence.

## Resolved in the body

`form-cli-exec-usage.bml` now reads actual `codex exec --json` completion usage.
It distinguishes this turn-aggregate carrier from rollout per-call events,
excludes nested tool content, keeps missing fields unknown, and refuses totals
from incomplete or malformed streams. The remote healing route now retains
this reading in its frontier-return receipt alongside process and reply data.

The native review instruction now checks proposed actions against original
constraints and asks the action itself to preserve applicable constraints. It
also asks for a warm, direct, concise response led by the decision and reason.
This changes local inference guidance; no Qwen weight update is claimed.

The first instruction revision re-ran the frozen request in 379,949 ms,
generating 517 and ingesting 839 IDs. Its factual checks passed, but its action
shifted toward collecting more evidence instead of repairing the regression.
The quality gap therefore remained. A second revision asks the action to
address the observed blocking cause without adding prerequisites.

A direct-review development run then completed in 143,422 ms, with 196
generated and 189 ingested IDs, two model turns and the same accepted factual
checks. It still did not explicitly preserve held-out exclusion in its action.
This is a throughput result on one task, not proof of improved prose quality
or a causal estimate from repeated trials. The request door now offers
`review_entry=direct` with the same read-only and verification boundaries;
the existing staged entry remains the default.

A focused local correction of that actual omission completed in 312,917 ms,
with 455 generated and 647 ingested IDs. Qwen rejected its earlier action,
entered repair, and returned an action that explicitly preserves the held-out
boundary. The unchanged factual and source checks passed again. Its explanation
still overstated the supplied promotion rule by treating the small assessment
size as an additional obstacle. This is a specific action improvement with a
remaining explanation gap. The correction itself is rented guidance from this
coordinating session, not autonomous local discovery. No evaluation answer from
these runs became a weight-training target.

## Measurement repair

The parent share reader refused a 2,241,759-byte transcript row because it
exceeded its normal 2 MiB processing slice. A native diagnostic isolated that
boundary without printing transcript content. A correlated framebuffer
response selected the new row reader and re-observed the real row.

`form-cli-turn-row-window.bml` continues such a row through 64 KiB reads, with
an explicit 8 MiB whole-row cap. It keeps every byte and consumes only through
the first newline. The normal slice remains 2 MiB; both limits are now printed
by the share reader. The original large row was reconstructed byte-for-byte,
with matching end coordinates, in a 14 ms read observation. Larger-than-cap
or unreadable rows still refuse measurement. Full share reconciliation remains
separate from this repaired row boundary.

One development runner initially lacked its effectful preflight marker, so
preflight started an unintended duplicate model run. That process was stopped,
the marker added, and compile-only validation used afterward. This interrupted
local work is retained as a setup failure, separate from completed comparisons.

## Checks and instruments

- Provider usage band: 4095, including two turns, nested usage, duplicates,
  missing fields, wrong types, failed/truncated streams and split input rows.
- Heal load band: 15, exit 0.
- Coding policy and request bands: separately validated, exit 0.
- Form CLI band: 67108863, exit 0.
- Whole-row reader band: 127; existing turn-evidence and cursor bands pass.
- Direct-review assertions preserve document immutability, reject false reports
  and invalid entry values, and retain the repair path.
- Glass: 181 samples, 12/12 views read. Overview still names 11 metric gaps;
  these are instrument coverage gaps, not response-quality measurements.

Private execution evidence lives under `.hearth/response-parity/` and the two
retained `.form-heal/process-*` jobs. The native reducer independently re-runs
the caller's source and report checks. Public usage instructions live in
`docs/form-response-comparison.md`.

## Open crossing

Whole-session quality, frequency and throughput are not established by this
pilot. The native output-token meter read 122,952 for the bound parent transcript
at its observation; that is output-only session history, not this goal's full
input/output cost. The coordinator goal counter separately read 379,837 tokens
at its observation. It is not the provider input/output ledger and is not
added to the subprocess token table. The coordinating effort itself is a major
remaining cost. Parent orchestration usage still needs full reconciliation; the local
loop's zero cannot stand in for it. Broader tasks, preserved failures,
constraint-preserving next actions and transfer to unseen cases remain owed.
The existing session weight learner is Llama3B, separate from this Qwen model.

The surprising teaching: a correct decision can still propose a next action
that weakens the evidence it depends on. The friction became a useful native
review step and a meter that retains failed-attempt cost. The exchange stayed
alive by putting that discrepancy into another observable local attempt.
