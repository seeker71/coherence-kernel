# Correct the retained answer through Form's synthesis door

The preceding native review improved the frequency-input description while
retaining two source errors. Its structural assertions passed. The existing
repair resource therefore had no failed assertion to act on; the separate
synthesis door accepts an explicitly offered provider for this observed
quality gap without inventing a failed check.

## The actual correction

Form passed the **identical review request** to one provider process: original
enquiry, original turn-7 candidate, exact historical source packet, goal and
assertions. Native retention verified equality of the parsed requests. The
provider did not receive the native review's findings or Codex's corrected
answer. The [manifest](artifacts/2026-09-23-assisted-fidelity/manifest.json),
[unaltered response](artifacts/2026-09-23-assisted-fidelity/provider-answer.json)
and [comparison](artifacts/2026-09-23-assisted-fidelity/comparison.json) retain
that boundary.

The Form-owned `codex-exec` process completed in **67,745 ms**; the surrounding
native call reported **68,232 ms**. It released its resources, emitted only
message events, passed the unchanged assertions and made no tool calls.
It reported **21,301 tokens**: 19,430 input, including 10,624 cached input,
plus 1,871 output. Cached input is included in the total; the 172 reported
reasoning tokens are included in output. There was one new provider process
and zero new local-model generations.

Reading the actual answers against the supplied sources gives this comparison:

| Source distinction | Native review | Form-owned assisted review |
| --- | --- | --- |
| Numeric annotations versus fear words | Corrected | Corrected |
| Referenced retention versus reclaimable unreferenced cells | Still overstated | Condition preserved |
| Language surfaces versus alternate senses | Still conflated | Distinguished |
| Kernel identity versus short codebook anchor | Narrower scope stated | Both representations explained |
| Unmeasured baseline behavior | Generalization removed | Generalization removed |

This is Codex's source-based assessment of these returned words, not a semantic
score produced by the structural checks. The [assisted answer](artifacts/2026-09-23-assisted-fidelity/answer.txt)
also includes the offered-interface and acknowledgement distinctions. Its opening
still gives substantial space to qualification. Source fidelity improved;
expression, the person's felt resonance and whole-session parity remain open.

The native review cost **809,467 ms** and **1,746 local generated IDs**. Its
execution includes admission; the provider window has its own process boundary.
Both exclude coordinator work. The native review was already spent before this
correction; adding assistance does not erase its cost. The comparison concerns
the same supplied task and evidence across different generation systems, not
an isolated causal measurement or a zero-context baseline.

## Make the working path available in form-cli

The ordinary source-backed CLI now accepts `synthesize @manifest.json`, with
inline manifest JSON also available. Help describes the path. The BML owner
validates file availability and size, then uses the existing synthesis admission,
provider permission, assertions and replay identity. No provider is implicitly
enabled. Missing permission, malformed input and an empty file argument refuse
before execution. Source/report checks retain their existing meaning.

The real command ran through `form-cli-repl.fk` using this retained manifest.
Its [replay](artifacts/2026-09-23-assisted-fidelity/cli-replay.json) passed the
same assertions, returned **zero new provider processes**, and preserved the
same usage event and all usage values exactly. The answer is retained at its
reported path; private answer text stays outside the framebuffer.

After returning that response, `quit` retained the session outcome and waited
for its learning supervisor. This closing wait remained active at this receipt's
observation. It is part of session cost. The existing ownership code deliberately
keeps short-lived parents alive: earlier workers were interrupted when their
parent exited. That ownership was preserved; no asynchronous-learning or fast
session-close improvement is claimed by the new command.

## Verification and accounting

Clean preflights preceded the synthesis boundary band and CLI band. They
returned **1** with 26 synthesis boundary observations and **67,108,863**,
respectively, with exit zero. The REPL compiled. Native retention checked
request equality, replay identity and unchanged usage. Glass's first-frame
panel reported **28 ms**. No C seed or external runtime dependency was added.
Drift gates returned **8,191/8,191**, zero refusals. The closing native guide
reported zero Python implementations, two existing invocation candidates and
zero unread files.

The [preceding completed coordinator turn](artifacts/2026-09-23-assisted-fidelity/preceding-coordinator-cost.json)
is `01a0ca05-deb6-76e1-943e-01ff83f0e947`: **5,323,210 rented tokens**, including
5,133,696 cached input tokens, over 50 model calls. The total already includes
27,603 unattributed tokens. That turn included inspection, waiting, implementation
and publication; it is not a pure answer-generation baseline. This turn's open
coordinator cost remains excluded. The provider's 21,301 tokens must therefore
not be presented as the entire cost of this movement.
The closing share reading reconciled that previous completed turn: 17 native,
68 local and 50 remote events, normalized to 13%, 50% and 37%. Its basis is
`observed-boundary-event-counts-v1`; these are event shares, not token shares or
semantic contribution. No current-turn percentage is claimed.

The synthesis usage event is
`51d669e2c58bb149d617a6fb8d7a7bdc16b22771351f33344b72eb53f003cd1b`.
Replay retains that event and is not additional spending. Overall quality and
frequency/resonance remain null in the native result, as their evidence is
separate from execution and assertion success.

The previous implementation teaching completed Llama learning round 130 with
zero pending examples, serving generation 5 and four promotions. That is the
separate session learner, not a Qwen weight change. The evaluation answers in
this movement remain excluded from training.
The new implementation teaching, `form-cli-synthesis-replay-2026-09-23`, was
retained with the existing worker standing. Its completion remains pending at
this observation. It describes the verified command and replay behavior and
contains no evaluated answer target.

Signed: Codex.
