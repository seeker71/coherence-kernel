# Form-cli can attempt a checked repair

The `heal` verb turns a failing source/band pair into a checked repair attempt.
The policy, candidate edits, resource order, verification decision, and learning
live in executable BML. The working tree changes only after verification.

`heal guide` reads current Python detours and available native references;
`heal guide|<proposed command>` guides a command without running it. The acting
agent carries the native replacement and its checks. This includes temporary
helpers. [Native authoring practice](native-authoring-guide.md) keeps that
responsibility with us.

Inside form-cli:

```text
heal lesson
heal model/example.fk|model/tests/example-band.fk|255|30|local
```

The paths above illustrate the syntax; supply the actual production source,
its existing checker, and its exact expected stdout. The final `local` field
omits remote generation. Leaving it off enables one final Codex CLI attempt
after the offered local routes have completed or reported their inability.
The checker timeout defaults to 30 seconds and accepts 1–120 seconds.

The same native recipe has a stdin door:

```sh
form-run sh -c 'printf "%s\n" "lesson" | ./fkwu observe/form-cli-heal-run.fk'
form-run sh -c 'printf "%s\n" "model/example.fk|model/tests/example-band.fk|255|30|local" | ./fkwu observe/form-cli-heal-run.fk'
```

Read the returned state: `healed`, `already-passes`,
`unresolved-local-evidence-retained`, or a named refusal. A process exit of zero
from the text command is not a repair claim. Its evidence directory is returned
alongside the state.

## What it does

1. Copy the current tracked and untracked source files into a private snapshot,
   including uncommitted changes. Record hashes, preserve the original target,
   and run the existing band to establish the failure.
2. Read the repair teaching, search current source for the unresolved symbol,
   inspect recent target history and symbol changes across git refs, and ask the
   existing native RAG index. RAG evidence includes a bounded current source
   excerpt when the reference resolves inside the repository. Missing resources
   and timeouts remain recorded.
3. Replay matching verified repair examples. Try at most eight deterministic
   candidates, including closers before nested definitions and at EOF. Each is
   a hypothesis; the checker decides whether it survives.
4. Read the model registry and build a native resource plan. Models with
   `answer`, `fast-answer`, or `code` roles are eligible when their artifact,
   seal, tokenizer index, and wired native session lane are present. Each
   ineligible row records its reason; duplicate eligible artifact paths share
   one attempt. Try a registered, evaluated, compatible Qwen output-head
   adapter for any eligible model, then each eligible base model in registry
   order. Admission and generation use dynamic observation without a lifetime
   deadline. The existing
   session and adapter APIs own model state and release it.
5. Query the local Ollama service and assign diagnosis, patch, and review roles.
   Selection prefers names containing `reason`, `coder`, and `qwen`, respectively,
   and otherwise uses the first observed local model. Roles may share a model;
   that is not independent-model validation. The patch is eligible only after
   the reviewer returns exactly `ACCEPT`. The outer process uses dynamic
   observation. Form reads the loopback NDJSON stream through curl and retains
   partial responses as they arrive. This client sets no generation lifetime
   or token limit; the service and model retain their own context limits.
6. Require a valid native inventory, a completed candidate check or refusal
   for each planned native model, and receipts for the adapter and local model
   roles before using the existing exact frontier-admission predicate. A model
   starting or returning text does not establish completion. An invalid or
   oversized inventory leaves remote fallback closed. An invocation-specific claim allows at most
   one remote call, including after failure. Codex receives the bounded repair
   prompt in an empty directory with read-only permissions, ephemeral mode,
   and no user configuration. Its answer is another proposed local edit.

Accepted model responses contain one exact unique old/new replacement. Ambiguous
matches, commentary around the replacement, empty output, and oversized edits
are refused. Models cannot select the target file or change the expected result.

## What counts as a repair

Before each candidate, restore the snapshot's original dependency bytes and
remove generated program images. Preflight the checker through its FK entry
point, then run it. Acceptance requires a clean preflight, successful process
status, exact expected stdout, empty checker stderr, and unchanged dependency
and test hashes. Preflight's import-fallback warnings remain in its own log;
they do not erase errors or substitute for the separately checked band.

The carrier checks the live inputs again immediately before an atomic target
replacement. A stale source or altered dependency refuses the replacement.
Failed candidates never replace the live source. Their source bytes, stdout,
stderr, status, deadline result, and stage order remain available.
The large working snapshot is released at the end of the command; the original
target, checker, proposals, manifest, and observations remain in the evidence directory.
`native-plan.tsv` records each registry name, artifact path, eligibility reason,
and attempt stage. `attempts.tsv` records model-specific admissions, checks, and
completion. These show which resource remains owed after an interrupted attempt.

The source limit is 64 KiB; model replacements total at most 4 KiB, and model
prompts at most 24,000 bytes. Larger or multi-file repairs need a separate
contract. Only production `.fk` and `.bml` targets are admitted; test targets,
managed source symlinks, and explicit `DO NOT REPAIR:` exclusions are refused.
No source, test expectation, historical recording, or model weight is changed
merely to make a verdict look successful.

## Dynamic observation and explicit deadlines

Native generation and the local learner have no default lifetime deadline.
Form adapts the observation interval to the last progress gap; silence offers
process inspection, and observed progress continues. Quick processes finish
immediately. Observation cadence ranges from one to thirty seconds, not a
requirement to consume that time. CPU time and RSS are process observations,
not proof of useful reasoning. A silent live process currently needs explicit
stop control; the policy does not yet classify semantic stalls automatically.

Write `stop` to the recorded `<stage>.control` file to cancel a dynamic stage.
The supervisor records Form's offered, selected, and applied action, signals
the owned process group, and verifies its release. Explicit positive process
deadlines remain supported, including the caller's checker bound. A failed
signal is retained with its command status; it cannot erase the child's actual exit
or turn incomplete cleanup into a completed process.

The stdin door accepts `stream` followed by requests until EOF or `release`.
Native generated bytes arrive incrementally in `<stage>.reply.partial` while
metadata identifies route, model and base seal, frozen adapter path/hash/state,
prompt count, cumulative output count, and position. Counts are cumulative,
not a sum of successive readings. Completion publishes `.reply` only after EOS
and resource release. Context capacity still bounds each native request;
continuous context recycling and a resident model across these requests remain
unimplemented. Ollama exposes its actual token totals on its final response;
earlier stream chunks remain byte observations, never invented token counts.
Remote-provider token telemetry remains a gap.

Each bounded process records its wall-clock start/end, monotonic elapsed time,
budget, stage transitions, deadline signal, and reaped process status. Native
admission separates seal verification, tokenization, model opening, pipeline
compilation, tensor mapping, state allocation, prefill, and decoding. Prompt
position counts and prefill checkpoints show how much work completed.

Session admission opens scratch for up to 64 prompt positions and uses the
existing batched prefill implementation. The scratch width and slice width
come from the same BML admission policy. The cached model path carries that
width too; its ordinary one-position context interface remains available.

`timing-report.json` and `timing-report.md` contain the measured intervals and
policy choices. The BML timing authority checks ordered, non-overlapping
intervals and requires their sum to equal the process duration. These are
elapsed times between parent-observed events, including waits and transport
latency, rather than CPU utilization. Child timestamps are retained separately;
a child clock adjustment cannot change monotonic accounting.

`choices.jsonl` records offered actions, selection reason, correlation ID,
selection time, and the applied action's returned result. The actor is the
running Form policy. A single-model evaluation offers only its requested model
and finishes that case after failure; the repair queue can advance to its next
eligible resource. A timeout does not extend its own budget. The final report
includes both the initial timeout and the subsequent recovery result.

Timeout summaries appear in command output. Re-read retained evidence with:

```text
heal report|.form-heal/<eval-native-run>/<case>/evidence
```

Reports retain lifecycle and opaque stage information without copying prompt
or answer text into diagnostic events. An older run without process boundary
events needs a fresh observation; the reporter does not fabricate its timeline.
Cancellation observes descendants before termination and signals children in
their separate groups too. An incomplete evaluation retains its source tree
and unfinished evidence. Evaluation traces separate snapshot preparation,
each checker and model call, snapshot release, and report generation.

## What it remembers

`.form-heal/learned.tsv` points to verified before/after examples and checker
contracts. A later matching failure offers that repair again and rechecks it
against the current snapshot. Each accepted example includes a training-candidate
record; failed candidates remain counterexamples in their attempt directories.
The shared teaching names the successful and refused repairs from 2026-09-07.

Every candidate/check round now offers its observed outcome to the shared
native session learner by default. Verified replacements become supervised
repair targets; failed proposals teach only the observed execution status.
Evaluation rounds are excluded. `learning.jsonl` links the private example and
worker state; queued work is never labelled a completed weight update.

The Llama learner uses full next-token LoRA gradients and restores Adam state
from the previous generation. Every completed learning round updates all loaded
LoRA pairs. Current-example loss must improve, and every held-out and rehearsal
row must avoid regression against both the preceding candidate and the serving
adapter before promotion. The per-row scores, checkpoints, source seals, actual
process exits and timing stay under `.hearth/session-learning/`.

After deterministic repair and verified memory, healing attempts the evaluated
session adapter before the existing native/local model inventory. Its generated
replacement still has to pass the unchanged isolated checker. Failure continues
through local resources and models; the rented CLI remains the final fallback.
`session status`, `session pause` and `session resume` expose the shared learner.
See [Native session learning](native-session-learning.md) for the exact scope
and the distinction between validation loss and demonstrated repair quality.

## Measure repair behavior before training

```text
heal eval
heal eval|native|qwen38-q4|positive-boundary
```

`heal eval` runs the six-case deterministic curriculum in a disposable
repository: one development case, four held-out cases, and one correct control.
The cases cover an outer closer, comments and escaped strings, a nested
definition, a recursive base value, and a comparison boundary. The boundary
checker includes empty input, zero, mixed signs, negatives, and fractions.

The native form measures one named case using one eligible registered model.
Generation and the outer native case use dynamic observation. It does not
invoke a remote provider. Native generation receives the
broken source, checker contract, and failure observation. RAG, git retrieval,
and repair-memory replay are excluded from this measurement so their effects
are not attributed to the model. This measures the generator and guarded
repair verifier; the integration suite measures the wider routing workflow.

Read the measured outcomes: `repaired`, `unresolved`, or `preserved` for a
correct control. A repair case that already passes is invalid; a changed or
failing correct control is a regression. An unfinished process or invalid
evaluation contract produces an incomplete report. A successful command exit
does not mean that all repair cases succeeded.
Semantic cases also require a clean baseline preflight, successful checker
execution, and empty diagnostic stderr before admitting a model. A compiler
failure cannot stand in for a semantic defect merely because it prints a number.

Reports live under `.form-heal/eval-native-*/`. They retain the curriculum hash,
per-case source and checker hashes, before/after evidence, process records,
durations, and outcomes. The disposable source tree is released. Evaluation
records are marked `evaluation-only`, omitted from repair memory, and refused
by replay even if a pointer is inserted into the memory index.

“Held-out” here means locally withheld from repair memory and training. It
does not claim that a model has never encountered similar material during
pretraining. The small public curriculum is a diagnostic benchmark, not a
general repair-quality estimate. It neither trains weights nor creates the
adapter evaluation attestation described below.

An optional `.form-heal/adapter.tsv` registration contains six tab-separated
fields: model registry name, base SHA-256, adapter path, adapter SHA-256,
evaluation-receipt path, and receipt SHA-256. The receipt is:

```text
repair-heldout-v1|model-name|base-sha256|adapter-sha256|passed|total|regressions
```

Admission requires a positive, complete held-out count, zero recorded
regressions, matching actual hashes, and the native adapter's shape/device
check. Creating this registration is an evaluation attestation; the healer
does not manufacture one. The current native adapter door
supports the existing Qwen rank-one output-head format. Other adapter formats
need their own compatible inference transport and evaluation.

## Present limits

The snapshot is filesystem isolation, not an OS sandbox for arbitrary test
code. Run trusted repository checks: absolute-path effects in a checker can
still reach host resources. The healing path requires fkwu and ordinary host
utilities (git, tar, cp, ps, sh; curl for loopback HTTP). It runs no Python.
Form owns process supervision, snapshot hashes, guarded replacement, stream
parsing, choices, reports, evaluation, and native adapter fitting. No C seed
runtime meaning was added.

The local model registry is an inventory, not a claim that every listed model
has a working native lane. The healer offers every eligible native artifact
in that registry and three local model roles; it does not exhaustively discover
or try every model installed elsewhere. Dynamic stages may continue until
completion or explicit cancellation. Seal presence is a discovery fact; the native runner
still validates the seal on admission. An
offline Ollama service is reported as unavailable, not started or installed.
Base GGUF files and seed adapters remain unchanged; learning writes separate
LoRA candidates when enabled.

The remote CLI options are verified against the installed `codex exec --help`
and [official non-interactive documentation](https://learn.chatgpt.com/docs/non-interactive-mode).
Native policy bands check routing decisions. Process and snapshot witnesses
use real children and disposable files; the curriculum uses real Form checks.
These establish transport and refusal behavior, not model quality.

Verification doors (preflight each FK band before reading its verdict):

```sh
form-run sh -c 'printf "%s\n" form/form-stdlib/tests/form-cli-heal-policy-band.fk | ./fkwu observe/preflight-stdin-run.fk'
form-run ./fkwu form/form-stdlib/tests/form-cli-heal-policy-band.fk
form-run ./fkwu form/form-stdlib/tests/form-cli-heal-resources-band.fk
form-run ./fkwu form/form-stdlib/tests/form-cli-heal-load-band.fk
form-run ./fkwu observe/form-cli-heal-native-io-witness.bml
form-run ./fkwu form/form-stdlib/tests/form-cli-heal-eval-policy-band.fk
form-run sh -c 'printf "%s\n" eval | ./fkwu observe/form-cli-heal-run.fk'
form-run ./fkwu form/form-stdlib/tests/form-cli-heal-timing-band.fk
form-run ./fkwu observe/form-cli-heal-native-process-witness.bml
form-run ./fkwu form/form-stdlib/tests/form-cli-heal-dynamic-band.fk
form-run ./fkwu form/form-stdlib/tests/form-cli-heal-flow-band.fk
form-run ./fkwu form/form-stdlib/tests/qwen-lora-finite-band.fk
form-run ./fkwu form/native/metal/tests/qwen38-embedding-band.fk
form-run ./fkwu form/form-stdlib/tests/native-session-learning-band.fk
form-run ./fkwu form/form-stdlib/tests/native-session-worker-band.fk
form-run ./fkwu observe/native-session-homecoming-run.fk
```

Policy authority: `form/form-stdlib/bml/form-cli-heal-policy.bml`.
The separate `form-cli-heal-native-learning-witness.bml` measures the older
Qwen activation-probe objective; it is not the production session learner.
Native resource planning: `form/form-stdlib/bml/form-cli-heal-resources.bml`.
Evaluation curriculum: `form/form-stdlib/bml/form-cli-heal-eval-policy.bml`.
Executable movement: `form/form-stdlib/bml/form-cli-heal.bml`.
Repair teaching: `teachings/form-cli-healing.md`.
