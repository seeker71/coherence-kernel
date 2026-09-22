# Keep delivered failure evidence in its resident context

The native selector job retained its current compiler failure, then supplied
that same failure again in later tool feedback. Its step-30 observation was
**4302 bytes**, with **2324 bytes** of retained failure text before JSON escaping.
The model had already received that evidence; repeated delivery consumed more
of the same limited context.

The controller now gives a substantial failure a `failure_evidence_id` when
supplying it in full. After completed feedback delivery, later messages may
reference that identity within the current model context. The reference is
used only when shorter. Changed failure bytes receive a new full observation.
A fresh or resumed bootstrap always supplies the full failure again. Failed,
partial or absent feedback loading admits no new identity. The retained failure,
source documents, original constraints and caller checks stay intact.

Short failures stay text. The paired native-review path retains its own evidence
protocol and does not mark an unsent failure as delivered. The new context
identity survives the existing binary checkpoint codec and is reset through
the normal context-start boundary.

## The actual packet comparison

The [retained before packet](artifacts/2026-09-22-resident-failure-evidence/before-feedback.json)
comes from the actual native step-30 checkpoint. The
[protocol replay](artifacts/2026-09-22-resident-failure-evidence/observation.json)
runs that same state through the revised formatter and completed-delivery
transition, without calling a model:

| Delivery | Bytes |
| --- | ---: |
| Previous feedback | 4302 |
| First full delivery with identity | 4391 |
| Repeated delivery in that context | 1714 |

The first delivery costs 89 additional bytes. A repeated delivery is 2588 bytes
smaller than the previous packet. Its reference resolves to the exact retained
failure in the full packet. These are serialized-byte measurements, not token,
latency, semantic-quality or rented-usage reductions. The
[full](artifacts/2026-09-22-resident-failure-evidence/first-feedback.json) and
[reference](artifacts/2026-09-22-resident-failure-evidence/repeated-feedback.json)
packets remain available for inspection.

## The running work reached a real context boundary

The live job, still using its previously loaded formatter, applied the second
missing-`else` repair. Its next compiler check rejected the loop syntax, and the
model proposed replacing the loop with recursion. It then read the current
source. Loading that read's feedback reached `coding-observation-context-capacity`.

The [terminal result](artifacts/2026-09-22-resident-failure-evidence/native-terminal.json)
retains 32 cumulative coding steps, 16 tool calls and 3 checks. This admission
generated 507 local token IDs and injected 6599 observation IDs, with 5 completed
observations. The [process](artifacts/2026-09-22-resident-failure-evidence/native-process.json)
exited 0 after 1441364 ms, with released cleanup and no remaining group members.
Coding status is `attention`; the
[candidate](artifacts/2026-09-22-resident-failure-evidence/native-candidate.txt) and
[compiler failure](artifacts/2026-09-22-resident-failure-evidence/native-compiler-check.json)
remain incomplete work.

The continuation starts from the owned step-32 checkpoint with the revised
formatter and full evidence in a fresh context. No rejected edit is replayed.
The caller now permits 64 additional replies instead of 12; the context remains
12288 and the source/behavior checks remain unchanged. This changed allowance
is explicit and prevents attributing later differences solely to formatting.
No provider call is involved.

## Verification and cost

- Policy 65535, memory 65535, request 255, paired native review 34359738367;
  clean preflights. Delivery failure, changed evidence, fresh bootstrap,
  short messages, binary continuity and preserved source/accounting are checked.
- Actual retained-packet replay confirms the full/reference relation and byte
  counts while preserving source, check count and completed-reply count.
- Drift gates: 8191/8191, refused=0, exit 0. No C seed or external runtime change.
- Glass first frame: 31 ms. Native guide: 0 Python implementations, 2 invocation
  candidates, 0 unread.
- Share is declared and withheld while 911 appended carrier bytes remain to be
  reconciled. Refresh took 910 ms against the 5000 ms attention scale. The
  separate output-only session meter read 4217003.

The [previous completed coordinator turn](artifacts/2026-09-22-resident-failure-evidence/preceding-coordinator-cost.json)
used **6429078 rented tokens**, including **6301312 cached-input tokens**, across
39 model calls and 38 reconciled tool events, with zero unattributed tokens.
Current open-turn usage is excluded. This remains substantial coordinating
cost; the packet comparison does not establish its reduction or whole-session
parity.

My replay helper initially used `fhn_n` instead of the actual `fhn-n` binding.
Preflight refused it; the corrected helper passed before its measurements were
used. A cached observation helper also reported changed source bytes and rebuilt
itself through the existing compiler care path. Both events remain distinct
from native model quality.

The verified movement is less repeated feedback with intact evidence. The
ongoing native task will establish how that change serves actual completion.

— Codex
