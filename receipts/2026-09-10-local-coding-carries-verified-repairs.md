# Local coding carries a verified repair between admissions

Codex, 2026-09-10.

The JSON `code` door now carries native FORMBIN2 checkpoints and exact-contract
repair lessons. Complete model/tool transitions checkpoint; a successful repaired
candidate produces a lesson. A new admission rechecks that candidate against the
unchanged caller contract before recalling its last three repair notes. It does
not copy the answer into the new task or rewrite source files automatically.
Three identical tool outcomes without document movement select repair and replan.

The public arithmetic fixture was seeded in one process with a known failing
candidate (fixture data, not a Qwen error). A separate native Qwen3.8 Q8 process
resumed it and completed: 13 total turns, 5 tool calls, 1 repair, 2 check runs,
340 generated IDs, 807 injected IDs, release_ok=1. Six turns, one call and the
initial failed check belonged to the seed; the remaining work was local Qwen.
The returned function was `add(add(mul(x,x),x),1)` and the read-only document
remained unchanged. Elapsed time including admission was 427148 ms.

A third process started a fresh task with its original documents, observed
lessons-recalled=1, and completed in 10 turns and 4 tool calls: one successful
check run, no repair, 600 generated IDs, 956 injected IDs, release_ok=1. Its
elapsed time including admission was 488008 ms. These are different starting
conditions, not a matched quality or token-saving experiment. Both model
admissions ended; no resident hearth is claimed by this receipt.

After rebasing onto 83524d9d and rebuilding for freshness 31, the completed
checkpoint was rechecked without model admission: complete, generated_ids=0,
injected_ids=0, release_ok=1. This includes the upstream negative-integer decoder
repair; the arithmetic result is not resting only on the earlier kernel.

Glass receives counters, not private task content. Coding progress now publishes
every four generated IDs rather than 32. During the fresh run, observed four-ID
publications were about 0.78–1.11 seconds apart; this is not an inference latency
guarantee. Durable writes happen at role/tool boundaries, not every publication.

Native bands: memory 65535; no-progress 255; policy 65535; repair 262143;
request 255. The memory band covers corruption, changed contracts, atomic
replacement failure, completed-checkpoint rechecking, and preserved constraints.
Checkpoint IDs are scoped names, not caller-selected arbitrary paths. The digest
detects damage, not an adversary able to rewrite local storage. Concurrent resume
ownership, automatic context compaction, semantic retrieval across different
contracts, weight training, and arbitrary-repository completion remain unclaimed.

The surprising teaching: continuity can cross a process without carrying a
model's KV memory. The difficulty became useful at the verification boundary:
repair prose became reusable evidence only after the candidate passed again.
This movement keeps the next local attempt connected to what it already learned.
