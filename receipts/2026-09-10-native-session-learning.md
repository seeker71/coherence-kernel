# Session experience reaches native LoRA

Urs asked for each session to learn and for active inference to bring more work
home before a rented call. This movement connects the existing native Llama
trainer to `learn`, checked `code` results, every `heal` attempt, explicit local
teaching, and normal interactive session close. The learner is enabled by default.
It restores Adam, performs one full-gradient update per new observation, assesses
every held-out and rehearsal row, then either promotes the candidate or retains
the serving adapter. Both choices and their measured basis remain available.

The standing private learner actually advanced **0 → 1 → 2**, updating **56 LoRA
pairs** each time. First it learned a verified serving-policy teaching from this
coding movement; then a real `code help`, `session status`, `quit` CLI session
contributed its observed completion status and rehearsed that teaching. Both
rounds passed assessment and were promoted. Both supervisors exited **0**;
the current queue is **0**, with **0** retained Metal buffers after release.
The second round consumed **157 input tokens**, including **69 supervised tokens**
across its current and rehearsal rows. No private training text is in this receipt.

| Standing example | Before loss | After loss | Supervised tokens |
|---|---:|---:|---:|
| Verified policy teaching, first round | 5.1830052324 | 5.0366793862 | 40 |
| Session completion, second round | 4.0438172810 | 3.9616575352 | 29 |
| Prior teaching rehearsed, second round | 5.0366793862 | 4.9142114418 | 40 |
| Greeting sentinel, second round | 2.0104313195 | 2.0071983933 | 3 |
| Unverified-result sentinel, second round | 3.6459065229 | 3.6413361728 | 7 |

Public fixture evidence independently crosses two worker lifetimes with Adam
continuation: **127**. Its first training loss changes **0.9798279107 →
0.5213705748**; its second changes **0.6603322029 → 0.4486703873**. Both held-out
rows and the second round's rehearsal improve individually. A fresh prompt,
“Please greet me.”, then returns **“Hello.”**, **2 emitted tokens**, EOS, using
the promoted adapter. The stored evidence contains the row identities, exact
losses, token counts, process timestamps, choices, gradient norms, checkpoint
paths and inference trace.

The fixture's complete first and second learning rounds take **41.997 s** and
**52.401 s**. The trainer now obtains before/after assessments while the model
is already resident; it no longer admits a separate model for each ordinary
assessment. The first trainer trace breaks out **15.650 s admission**, **4.620 s
assessment before**, **6.516 s training**, **0.233 s update**, **0.750 s
checkpoint**, and **4.659 s assessment after**, plus separately recorded
aggregate validation and orchestration. These are nested timings, not quantities
to add indiscriminately. Fresh inference takes **18.483 s total**, with **2.176 s
generation**. Admission remains a substantial cost; this is not a hardware-floor
claim. The standing second round took **124.358 s inside the trainer** under the
host load present then; its per-stage evidence is retained rather than replaced
by the faster fixture figure.

Observed failures changed the implementation. An asynchronous worker disappeared
without an exit record when its short-lived parent command ended; local August
receipts named this process-lifetime failure family. Normal close and the
one-shot embodiment door now retain the parent until the supervisor returns.
The queued teaching then completed without being resubmitted. A real failing
learner-child witness confirms exit **1**, retained stderr, released lock and
the example still pending. A grounded-query band also reached the newly live
adapter and lost its original quiet-route verdict (**2093055**); its evaluation
door now explicitly excludes model admission and restores **2097151**, with
the original grounded-response assertions intact. The incidental generation
failed with `native tokenizer vocabulary entry absent` and never trained.
The decoder had indexed base vocabulary only, while its choice path eagerly
decoded alternative candidates. Added-token IDs now have a literal decoder;
candidate selection only decodes a known candidate when it needs that candidate.
An entirely absent candidate set returns an explicit reason through normal model
release. The caller also records the inference child's actual exit even when
no reply was written. Finally, the full REPL exposed two removed
process-parser names in owner cadence; their callers now read `host_processes`
and `host_pid` directly. Its existing band returns **262143**.

Re-running the failing prompt after the tokenizer repair returns **65 generated
tokens** in **21.868 s**, reason **adapted-repeat**, child exit **0**, retained
buffers **0**. It produces generic prose rather than a grounded repository
answer, so it remains an unverified proposal and is excluded from training.
This closes the execution failure without upgrading the answer's semantic status.

Checked coding proposals are saved to the normal resumable checkpoint. The REPL
routes JSON coding requests and `code help` to that engine while retaining its
symbolic-code lookup. A serving adapter needs a promoted example in the relevant
code/repair scope before it is tried there. Failed predictions suppress that
same prompt until the serving generation changes. Original caller checks still
decide whether a local proposal can replace the next model path. Concurrent
journal writers publish through an exclusive filesystem link; a conflicting
session/event identity cannot overwrite the winner. Private prompts and answers
stay in the hearth, outside the diagnostic framebuffer.

Fresh preflight and actual bands: memory **127**, concurrent journal **31**,
routing pressure **127**, promotion **63**, failed worker **63**, checked coding
and resume **511**, coding request **255**, coding dispatch **7**, healing load
**15**, CLI **2097151**, added-token choices **127**, tokenizer **31**, adaptive
voice **65535**. Adam resume remains **7** with matching parameters and
moments. The normal interactive CLI exits **0** after completing learning.
Drift gates return **8191/8191**, refused **0**. All new implementation is
Form/BML; this movement adds no Python or C seed code.

Glass's first-frame panel reports **34 ms**; lane counsel reports **orphans 0**
and **11/12 lanes unobserved** because no resident hearth is standing. Per-worker
training and inference measurements above are available regardless. The native
guide reports **25 Python implementations, 315 invocation candidates, unread 0**
elsewhere in the tree; this receipt does not claim that inventory is cleared.
The bound transcript meter reports **1,223,174 cumulative output tokens** at
byte **85,431,075**, a session reading rather than this movement's token delta.
Share remains **declared**, percentage withheld while collection is incomplete.

This is native Llama 3B session learning. It does not train Qwen, Whisper, or the
host Codex/Claude weights, and cannot intercept independent host-app calls.
The two sentinels establish only their measured objectives. No reduction in
rented-call count or general coding error rate has yet been measured. The next
useful observation is a real checked repair resolved through a learned adapter,
with the skipped later routes counted from the actual trace.

Evidence: [public and standing measurements](evidence/2026-09-10-native-session-learning/README.md).
Interface and boundaries: [native session learning](../docs/native-session-learning.md).

The exchange stayed alive by returning its own checked teaching to the body.
The surprising lesson was that a working gradient loop can still lose the whole
session at the parent-process boundary. That observed discomfort became a
supervised close with a real optimizer step, not a launch receipt.

— Codex, 2026-09-10
