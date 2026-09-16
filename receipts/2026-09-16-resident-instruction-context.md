# Reuse instructions already held in the native context

Signed: Codex, 2026-09-16.

## Executing change

Tool feedback repeated the complete role instruction even when that exact text
was already present in the resident model context. The policy now holds the
instructions supplied to that context and uses a shorter reference when it
saves bytes. Bootstrap still supplies full instructions. A newly encountered
instruction travels in full; it joins the record only after an observation
completes and increments the native observation counter. Failed, partial and
absent observations do not establish delivery.

The record belongs to the current admission. A resumed admission resets it and
supplies its current role instruction in full. Goal, documents, write boundaries,
pending work, tool results, failure evidence and checks keep their existing
owners. Turn exhaustion still reaches the existing loop without injecting
unusable feedback. This adds no C seed or external runtime dependency.

## Paired actual executions

Two requests are byte-identical to retained successful runs. No reasoning,
context or turn allowance changes. Provider assistance remains disabled and
assessment content remains excluded from training.

| Case | Elapsed ms, before → after | Generated IDs | Injected IDs |
| --- | ---: | ---: | ---: |
| Direct coding | 249,405 → 211,478 | 430 → 344 | 557 → 511 |
| Read-only review | 411,051 → 350,164 | 1,433 → 1,406 | 234 → 75 |

Both preserve the original assertions and successful release. Coding keeps
five replies, three tools, two checks and zero repairs. Its candidate documents
are byte-identical to the prior checked candidate. The actual trace retains the
explicit `verify` call and a read of the changed source during review. The final
model acceptance is shorter; the returned checked candidate is unchanged.

Review keeps two replies, one tool, two checks and zero repairs. The source
documents remain identical. Both actual reports support the current promotion,
read the numerical changes correctly, preserve unknown rented usage, keep
broader quality claims open, and preserve assessment targets outside training.
The new report proposes filling the usage gap before the next evaluation window;
that timing is not required by the supplied rule, and does not block the current
promotion. Report bytes differ, so unchanged wording or universal semantic
equivalence is not claimed. This is the coordinator's reading of both answers.

The two-case session completes in **561,664 ms**, with **zero provider calls**.
The timing pairs are observations, not a general speed ratio. The original
controller-review and dialogue quality gaps remain open, as does parity with the
context-equipped provider across whole sessions.

## Boundary checks

Policy preflight and native policy/request validation pass. Executing assertions
cover full bootstrap instructions, a new repair role, retained failure evidence,
successful admission, failed/partial observation, unadmitted state, resume reset
and report updates preserving context ownership. The existing workflow checks
continue to cover write boundaries, checks, repair and completion.

A real private checkpoint test edits a synthetic document, enters review,
records the observed instruction, saves and resumes. Goal, phase and candidate
survive; the previous two-instruction record resets to the current instruction
and bootstrap supplies that full text. All observed boundaries return 1.

For the frozen requests, the initial coding observation changes from **440 to
424 bytes**, while the review observation changes from **1,147 to 260 bytes**.
This is a serialization observation, separate from the actual token counts above.

## Evidence and accounting

Private evidence is under `.hearth/response-parity/`:

- `context-reference-manifest.json` and `context-reference-prior.json` identify
  unchanged requests and their actual baselines.
- `context-reference-process.log` retains per-stage and per-reply observations.
- `context-reference-comparison.json` rechecks request bytes, assertions,
  candidate/document equality, reports, release and counters.
- `context-reference-checkpoint.bml` is the real save/resume observation.
- `context-reference-*-preflight.log` and `context-reference-*-validation.log`
  retain compile and execution checks.

Session root:
`.hearth/response-sessions/55993e56b2c061eb7ad6dda094b6189f268639ec2518817d54500e85b3057175-49985-1789552133016`.

Coordinator cost remains separate from native provider counts. The goal counter
starts this continuation at **3,168,308** and reaches **3,309,143** at the recorded
observation. This is an intermediate counter observation, not final turn usage
or a provider API usage breakdown; the native comparison computes its delta as
**140,835**. Minimal overall rented spend is not proven.
The transcript-bound meter separately reports **785,505 cumulative session output
tokens**, through byte **39,229,017** of **39,229,049**. This is a different scope
and metric, not added to the goal counter. The share reader remains declared
and withholds a percentage because no completed evidence row is available.

The useful surprise is that the same coding candidate and required tool sequence
can arrive with a much shorter final acceptance. The previous experiment's
mixed quality results directed this change toward context reuse instead of more
instruction text. The remaining discomfort is measurable: the local run is still
slow and coordinator cost is high. That stays visible beside the improvement.

Panel: **0 orphans; 11/12 counsel lanes unobserved**, no standing hearth.
The glass door refreshed its dependent images and displayed its live panel.
A pre-existing monitor was already running; the extra process tree launched
for this closing check was identified by parentage and terminated, preserving
the existing monitor. Its exit 143 is an intentional stop, not a test pass.
The verified context-delivery and resume teaching is retained under session
`resident-instruction-context-v1`, event `verified-context-delivery-and-resume-v1`.
Its worker was launched; changed serving behavior is not yet observed. This
learning lane is Llama, separate from the evaluated Qwen model.
