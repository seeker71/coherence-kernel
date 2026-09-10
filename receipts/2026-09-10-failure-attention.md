# Failures direct the next observation

Urs asked to use failure and error as attention signals. Form's native healing
process runner now turns a completed failure into a bounded evidence exchange:
observe the original status, offer evidence acquisition or deferral, correlate
the response, acquire and reread the evidence manifest, then observe again.
The manifest retains `resolved=0`. A subsequent checked attempt can proceed;
collecting evidence does not convert the failed attempt into a success.

The source teaching was already present in
`teachings/error-is-loving-attention.md` and
`cognition/error-loving-attention.fk`. The new BML policy uses that distinction
and the existing bidirectional framebuffer. Local RAG returned `grounded:miss`;
git and filesystem references supplied the implementation context. The hearth
returned `no-standing-hearth`. Codex supplied the implementation reasoning;
the tests below executed locally without a model call.

## Observed behavior

The real-process band passed all 18 checks, verdict **262143**, exit 0 after a
clean preflight. Its retained output is
[`band.txt`](evidence/2026-09-10-failure-attention/band.txt), and the original
process files are exported under
[`process/`](evidence/2026-09-10-failure-attention/process/).

| Observation | Actual response |
| --- | --- |
| Child exits 1 | Evidence exchange; status remains 1 |
| Native compiler reports an unresolved call | Original stderr retained; evidence exchange |
| Next child succeeds | Returns 0; earlier failure remains visible |
| Native `forward-refused`, no trailing newline, exit 0 | Error retained separately from process status |
| Provider error followed by `done`, exit 0 | Earlier error remains an attention signal |
| Ordinary text mentions “error” | No invented failure |
| Explicit one-second deadline | Status 124; actual terminated-child status 143 retained |
| Explicit cancellation | Status 130; next direction belongs to caller |
| Missing, stale, short, or unsupported control | Evidence acquisition selected |
| Same stage starts a new run | Current attention and signal files reset |

The failure exchange ran from parent elapsed **96 to 100 ms**; that whole
process observation took **101 ms**. The deadline process took **1222 ms**,
including owned-process termination and evidence collection. These are wall
observations from this run, not hardware-floor measurements. The JSON timing
report reconciles every retained stage. Selected and applied choices retain
their shared exchange identities. The evidence manifest references the
original stdout/stderr rather than copying their content into the framebuffer.
Its file sizes are explicitly a snapshot at its observation timestamp.

The exported files preserve their original temporary-directory paths. The
same basenames under `process/` provide the durable public fixture evidence;
no private prompt or model generation was involved. Shell launch carriers are
local OS transport and are not exported as executable source.

Regression checks passed with exit 0: observed-process **4095**, quick-child
process **4095**, token-flow **31**, and dynamic control **63**. All four had
clean preflights. An initial test helper omitted its `do` scope; preflight
reported 33 compiler errors. Correcting that source scope produced the clean
18-check witness; no gate or expected result was weakened.

## Scope and current instruments

This exchange runs at process completion. Live silence still uses the existing
dynamic inspection policy; no generation budget was added. Unreported numeric
corruption and semantic errors still require their own probes. The policy
does not claim to discover or repair a root cause merely by collecting evidence.
Existing stage-name reuse replaces that stage's current files; historical
choices remain in the job ledger. Callers needing separate attempt evidence
must use distinct stage names, as the failure/next-attempt witness does.

The bounded Glass panel measured first frame **92 ms**, **33 kernel operations**.
Counsel observed **0 orphans** and **11/12 serving lanes unobserved** because no
hearth stood. The native authoring inventory read **25 Python implementations,
307 execution candidates, 50 grammar inputs, 0 unread files**. This movement
added only Form/BML implementation and native tests; legacy inventory remains.
Share was `kind=declared`, traversal incomplete, percentage withheld. The
session output-token meter read **1,340,941**, a cumulative session counter.

The local Llama 3B session learner completed one real round, optimizer step
**4 → 5**, updating **56 LoRA pairs** over **286 input / 191 supervised tokens**
(the new method and one previously promoted method). The training round took
**116081 ms**. The new-method loss improved **4.7379545308 → 4.7112905776**,
and replay loss improved **5.1288941210 → 5.0867920652**. The greeting sentinel
regressed **2.0021616817 → 2.0054118335**. Form therefore retained serving
generation **4**, kept candidate **5**, and reported **promoted=0, pending=0**.
The worker completed with exit 0; no threshold was relaxed. The other sentinel
improved **3.6342880726 → 3.6330841035**.

That observed regression selected a real action: preserve the serving adapter.
It does not invalidate the source tests, and the weight update does not prove
broader repair competence. The full per-row assessment and training timings
are in [`learning/result.json`](evidence/2026-09-10-failure-attention/learning/result.json)
and [`learning/training-events.jsonl`](evidence/2026-09-10-failure-attention/learning/training-events.jsonl).
The teaching contains the verified method, not the error fixtures or evaluation
answers. This trains the Llama dialogue adapter, not the Qwen base or Codex.

The drift gate returned **8191/8191, refused=0**.

Signed: Codex. The exchange stayed alive by turning a failure into a concrete
next observation. The surprising teaching was that a missing newline can hide
an error behind a successful process exit. That uncomfortable ambiguity became
a witnessed distinction between transport success and a model's error signal.
