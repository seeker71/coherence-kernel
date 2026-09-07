# Learning survives a failed cleanup signal

Codex, 2026-09-07. Share kind=declared; no bound rollout, percentage withheld.
Measured records and correlated choices are in
`2026-09-07-learning-completion-recovery.json` beside this receipt.

The previous trainer saved an adapter, then a supervisor signal error erased
its completion record. This movement retained that failure and reran the real
learner after repairing signal handling. Signal errors now survive as events;
the supervisor checks group membership before cleanup, retains the actual child
exit, and records whether the group was released. The learner requires that
completion evidence before advancing its candidate pointer. A nonnumeric
supervisor refusal becomes a training-failed record instead of an integer
conversion crash.

Two fresh local MLX rounds completed with exit 0 and empty process groups:

| Observation | Training tokens | Changed tensors | Trainer process elapsed |
|---|---:|---:|---:|
| Native stream contract validation outcome | 53 | 112 | 7,130 ms |
| Empty-proposal refusal through the automatic round hook | 61 | 112 | 8,154 ms |

The second round's parent hash equals the first round's candidate hash. Both
verified unchanged base weights. Both before/after validation losses printed
as 1.295; that precision cannot establish zero regression. These are real
optimizer updates on diagnostic outcomes, not repair-quality scores. The empty
proposal remained unresolved and changed no production source. The local
learning flag is enabled for subsequent non-evaluation rounds in this checkout.
The MLX candidate is retained separately from the native Qwen output-head
adapter; serving promotion still owes independent repair evaluation.

Process supervision passed 7 test groups, including an injected OS signal
denial with a real naturally exiting child. Dynamic supervision passed 4 groups;
the native dynamic policy returned 63 across four kernels. Stream transport
passed 5 groups with an explicitly simulated token producer. Routing passed
12 integration groups with simulated providers and real source verification,
replacement, timeout recovery, and admission. The real six-case
deterministic repair evaluation measured 3 structural repairs, 2 unresolved
semantic cases, and 1 unchanged correct control, with no remote calls. The
landing gates read 2047/2047, refused 0 before integration.

Panel: counsel reports orphans 0 and 11/12 lanes unobserved because no hearth
resident stands. That is not an all-good verdict. Native per-request streaming
still stops at context capacity; context recycling, semantic-stall decisions,
Ollama/remote token telemetry, independent adapter promotion, and a hardware
floor measurement remain owed. The current documentation now names dynamic
defaults and these actual limits instead of obsolete 300/120-second claims.

The exchange advanced by recovering an observable completion and then resuming
its weights through the next round. The surprising teaching was that training
could succeed while its receipt failed. The uncomfortable missing exit became
a tested requirement to preserve cleanup errors and actual completion together.
