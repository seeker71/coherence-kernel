# Native review source-line resolution

Signed: Codex. Observed 2026-09-17 in `codex/native-arrival-bootstrap`.

## Observation and repair

The retained `grounded-semantic-review-v1` exhausted its eight-turn allowance
while repeatedly omitting Markdown characters from a source quotation. Its
exact-link checker correctly refused that quotation. The original report,
failed checks and model replies remain private under `.hearth/response-parity/`.

`fcqe-lines` now selects exact bytes from caller-owned physical source lines.
`form-cli-review-source-lines.bml` supplies an atomic proposal and native repair
callback. Findings may carry `source_lines: [first,last]` with an omitted or
empty quotation. The proposal preserves Markdown, Unicode bytes and line
terminators, recording source path, line indexes and byte offsets. It refuses
conflicting quotations, ambiguous paths, duplicate keys, invalid ranges and
source byte-limit violations. Raw findings and immutable sources remain with
the caller. No kernel change or external runtime was required.

`fcrl-check` validates coordinates before calling the unchanged `fcrb-check`.
This matters even when an explicit quotation is already a valid substring:
its separately claimed line numbers can still be wrong. The controller checks
the resolved report again and attributes native repairs. Public usage is in
`docs/form-native-coding.md`; callers install these helpers explicitly.

## Re-observation

A narrower local task asked Qwen only to select lines for the failed citation.
It received native numbered source observations and the retained finding.
Form replaced that rejected quotation through an explicit caller-owned repair,
retained the original report and reran every original source, report and exact
review-link check. Candidate answers remained excluded from source authority.

| Observation | Result |
| --- | --- |
| Selection | `teachings/uplifting-dialogue.md`, lines 18–19 |
| Original byte interval | `[1226,1410)` of 6365 source bytes |
| Elapsed, including admission/context | 208933 ms |
| Generated / injected token IDs | 40 / 0 |
| Model turns / repair attempts | 1 / 0 |
| Extra model tool calls | 0; numbered source queries admitted initially |
| Original source / report checks | passed / passed |
| Exact review links | passed, two applied links |
| Documents / answer / next action | unchanged / unchanged / unchanged |
| Model release | passed |
| Provider processes inside this trial | 0 |

Private evidence: `.hearth/response-parity/generalization-v2/source-line-repair-v1/`.
The runner is `.hearth/response-parity/source-line-repair.bml`. Its request,
original report, selection, resolved report, source transition and receipt are
retained separately. The source quotation is now exact. The full answer still
contains defensive self-description, conflates translation with enrichment,
and asks for authorization already supplied. Link acceptance changes none of
those observations. The narrower task is not a matched throughput comparison
with the earlier full review and establishes no response-quality parity.

## Checks and embodiment

- Freshness: 31, exit 0.
- Expanded review-bindings band: 1, clean preflight and exit 0. Includes
  controller acceptance, rejected application claims, conflicting coordinates,
  literal/reference mixtures, exact bytes, invalid inputs and atomic failure.
- Existing quotation restoration band: 262143, clean preflight and exit 0.
- Drift gates: 8191; whitespace check passed.
- Native guide: zero Python implementations, two existing invocation candidates,
  zero unread files. Counsel: orphans 0; 11/12 lanes unobserved without a hearth.

Development preflight exposed an extra parenthesis, an invented
`json-node-number` name, a wrong test callback shape and unsupported lambda
syntax in the private runner. Each was repaired; the checks above were rerun.
Several exploratory searches also named absent files; their failed exits were
retained. The live glass was opened, then its owned display process terminated
after observation; the bounded counsel reading supplied the panel above.

Verified mechanics were retained as procedural teaching under event
`2026-09-17-source-line-review-procedure-v1`. The learner was launched only after
Qwen released. Assessed answers were excluded; retention does not establish
promotion or Qwen training. The preceding learner completed candidate 53,
with serving generation 5 unchanged.

The parent goal counter read 9,205,881 tokens after the trial; the native
transcript meter's earlier checkpoint was 1,889,744 cumulative output tokens.
These are different accounting scopes. No minimal-rental claim follows from
the trial's zero provider calls. The share meter withheld its percentage.

The useful surprise was that a 40-token selection resolved a mechanical loop
that a much longer review had repeated. The difficulty yielded a reusable
native boundary: the model selects a passage, Form carries its bytes, and the
answer's reasoning remains open to examination. The next useful observation
is whether a complete review using this boundary makes a substantive,
source-grounded improvement to the actual answer within a bounded cost.
