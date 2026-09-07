# Repair measurement before learned repair claims

Signed: Codex. Witnessed locally on 2026-09-07.

Share: **kind=declared; percentage withheld**. The share organ reports
`measurement-health=Absent reason=no-bound-rollout`. The hearth answers
`signal=nothing reason=no-standing-hearth`; Codex supplied the evaluation design
and implementation beyond that answer.

Three gaps acquired executable checks: form-cli can measure repairs across
different defects, evaluation answers are excluded from repair memory, and a
semantic benchmark requires a clean baseline before model generation.

The commands are `heal eval` and
`heal eval|native|qwen38-q4|positive-boundary`. The curriculum, baseline check,
outcome decisions, and memory policy live in BML. The Python carrier copies a
disposable repository, bounds child processes, and retains observations. No C
seed meaning was added. [The guide](../docs/form-cli-healing.md) gives the
contract and its limits.

## The measured floor

The final deterministic run contains one development repair case, four
held-out repair cases, and one correct control. It repairs all three structural
defects: an outer closer, a closer among comments and escaped strings, and a
closer before a nested definition. The recursive-base and comparison-boundary
defects remain unresolved. The control passes and remains unchanged. Thus
three of five repair cases succeed; among the four held-out cases, two succeed.
This is a small diagnostic curriculum, not an estimate of general repair skill.

The comparison checker exercises empty input, zero, mixed signs, negatives,
and fractions. An integer threshold of one would miss positive fractions, so
the checker does not accept that plausible incomplete repair. The semantic
baselines now execute with exit zero, empty diagnostic stderr, and clean
preflight; their verdicts fail the semantic contract as intended.

The [verification record](2026-09-07-form-cli-repair-evaluation-verification.json)
retains final case outcomes, hashes, timings, and the native measurement.
The evaluation policy returns **511 on all four kernels**. The native executor
load band returns **3**. Four new integration groups verify the actual
form-cli command, the six-case measurement, memory exclusion, and refusal to
replay an evaluation answer through an inserted memory pointer. The existing
11 healing integration groups also pass after the memory-policy change.
Preflight retains the known cold standalone-import fallback warning; the final
chains have zero errors and unresolved names.

The valid Q4 native measurement remains **unresolved**: generation reached its
300-second deadline during prefill, with no proposed patch. The recorded
generation time is **300.05 seconds**; the complete case took **349.99 seconds**,
including snapshot preparation and verification setup. Its baseline returned
**9**, exit **0**, against the required **31**, with empty diagnostic stderr.
The source hash after the attempt equals the original hash, and repair memory
is unchanged. This is a measured latency limit on this request, not a wrong
answer or a model-quality estimate. No further retry or remote call was made.

Every accepted evaluation candidate remains marked `evaluation-only`. It has
the original source, checker, verified candidate, and stage observations, but
cannot enter repair memory. No evaluation record is a training update or an
adapter promotion attestation. The held-out claim is local to this memory and
training boundary; model pretraining exposure is unknown. Native evaluation
isolates generation from retrieval, while the existing integration suite
checks the wider resource workflow.

## What the failed experiments taught

The first curriculum embedded a literal checker import header. The dependency
scanner treated that text as a dependency of the curriculum itself. Form now
constructs the header as a value, and both policy and executor preflight cleanly.

The first recursive checker omitted the core prelude that defines `nil?`.
It printed a number while its process failed. That experiment is excluded from
the semantic measurement. The first Q4 attempt against that invalid setup
also timed out after **300.05 seconds**, during prefill, without a proposal.
The corrected checker and a new baseline admission check were then measured.
The framebuffer recorded revision and re-observation around these changes.

The Glass's first observed frame was **66 ms**. Its owned process was stopped
and reaped with zero remaining members in its process group. Counsel reports
**orphans 0** and **11/12 lanes unobserved** without a hearth. Drift gates return
**2047/2047**, with zero refusals.

## Remaining work

Actual adapter training and paired held-out/regression evaluation remain owed.
The current evaluator measures deterministic candidates and one selected base
model; it does not train or promote adapters. A resident model and tighter
prefill observations are needed before claiming dependable repair latency.
Missing model lanes and local services, multiple-file contracts, and the
earlier tokenizer/audio/toolchain/video failures remain outside this repair.

Origin was fetched after green drift gates. This checkout is 12 commits behind
origin/main and retains the existing uncommitted repair movement. No rebase,
push, or universal clean verdict is claimed while the earlier failures remain.

The surprising teaching was that a failing benchmark can mimic a semantic
defect while testing a missing import. That refusal became an executable
baseline check. The exchange stayed alive by giving form-cli measurements it
can repeat and a boundary that keeps its own examination answers out of memory.
