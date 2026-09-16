# Measure the response gap

Both approaches carry the same task, documents and caller-owned checks. Keep
the task and checks before either answer exists. Preserve every attempt,
including setup failures. Evaluation answers stay outside weight training.

The local coding door is `observe/form-cli-code-run.fk`. A review request uses
`mode=review`, `evaluation=1`, empty `writable`, source `checks` and nonempty
`report_checks`. The returned documents must equal the supplied documents.
Its Qwen session has no provider fallback. Parent orchestration costs remain
separate from this local execution boundary.

`review_entry=direct` enters read-only review immediately, keeping the same
tools, source checks, report checks and repair loop. The default `staged`
entry retains refine, plan, split and inspection before review. Direct entry
reduces role transitions for already bounded reviews; it does not relax the
acceptance contract. A failed report still enters repair.

An external comparison runs through Form's process organ,
`observe/form-cli-heal-process-run.bml`, with `codex exec --json`. Retain the
process receipt, its stdout, stderr and the separate `-o` answer file. Native
arrival needs writable temporary/cache locations; a source-read-only task can
still fail in a filesystem read-only sandbox. Such a failure consumed tokens
and belongs in the comparison.

## Provider usage

Send the owned Codex stdout path as one stdin line:

```sh
form-run ./fkwu observe/form-cli-exec-usage-run.bml <<'EOF'
.form-heal/your-job/remote.out
EOF
```

`form-cli-exec-usage.bml` reads top-level `turn.completed.usage` events, once
per started turn in one identified thread. Nested tool content is excluded.
Input includes cached input; output includes reasoning output. Those subsets
are reported separately and are not added twice. Missing optional quantities
remain null. Missing, malformed, duplicate, failed or incomplete event streams
withhold totals. Usage does not establish that the task succeeded.

The existing remote healing route requests JSON events and places this
`provider_usage` reading in `frontier-return.json`. Its answer continues to
arrive through the separate reply file.

## What the comparison establishes

Re-run the same caller-owned checks on both answers. Record elapsed time,
local generated/injected token IDs, all provider attempts, and the cost of
the coordinating session. A zero-provider local loop does not make the whole
session zero-cost. Cached token volume and total token volume are different
from monetary price.

The parent turn reader normally advances in 2 MiB slices. A physical JSONL row
that crosses that limit continues in 64 KiB reads up to an explicit 8 MiB row
cap. It admits the whole row or withholds the measurement; it never skips the
row. The share output names the normal slice, whole-row cap and actual elapsed
refresh time separately. Full reconciliation is still required for a share.

Check useful next actions as well as factual fields: following an action must
preserve the original constraints and the assessment evidence. The native
review instruction now performs this check before submission and asks for a
warm, direct, concise answer.

Passing a few structured assertions establishes only that task's checked
behavior. Read the actual answers for unsupported claims, omitted constraints,
warmth, initiative and usefulness. Numeric text-valence readings, where used,
are instrument readings; they do not establish the quality of a relationship.
Overall session quality, frequency and throughput require a broader set of
tasks and actual session outcomes. Keep those conclusions open until observed.

The first measured pair and its local revision are recorded in
`receipts/2026-09-16-response-cost-and-review.md`.
