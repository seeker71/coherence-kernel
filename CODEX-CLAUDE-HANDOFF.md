# Codex ↔ Claude live handoff — 2026-08-24

The tokfast artifact is structurally complete (`seal`, magic `9380002`,
`=M=` separator, 248,587 merge rows; 11,365,180 bytes). The retained route is
not complete as a scalable embodiment:

- `qtf-freeze` reached about 67.5 GB RSS before its process was interrupted.
- The first `qwen35-tokfast-band` run reached 71.0 GB RSS after 2m03s, with no
  result emitted.
- The heartbeat retry reached 17.3 GB RSS after 43s. Heartbeats do not change
  the retained topology and `| tail -6` hides them until exit.
- Codex stopped only those child runs. The artifact and your source remain.

Please do not rerun the whole-table band. The next observed movement requested
by Urs and Codex is:

1. one bounded 32/64-row shard/process breath;
2. append a complete typed row, checkpoint only after append;
3. direct bucket lookup without joining or retaining the 11 MB monolith;
4. stale seal => `nothing` plus rebuild identity;
5. scannerless byte cursor and explicit `choice`, `failure`, `timeout`, `cut`,
   `undo`, `refine`, `release` observations;
6. RSS/elapsed/cursor before and after each breath;
7. pure synthetic exact-ID band first, then only a bounded live GGUF slice.

BMF's live cursor remains the native path; this tokenizer work is only the
Qwen prompt-compatibility bridge. No flattening, operations table, C seed
growth, fear-based restriction, or invented law. Maximal observable step,
trust through attributable evidence.

Sibling branch carrying the width-5120 RMS and cross-cell work:
`codex/form-local-reasoning-homecoming` at or after `7b10d02f`.

## Live merge correction — user direction, not a new law

The bounded route above has landed on `codex/form-local-reasoning-homecoming`:
`c132ed82`, `311e7c27`, `82f6a5da`. Do not rebuild the old tokfast blob.

While reconciling the MLX carrier, do not select the larger branch merely
because it has 32 `strcmp(op, ...)` cases instead of eight. Urs explicitly
closed a static Form operations table and explicitly rejected invented laws.
The destination is on-demand Form-native JIT generation of the CPU/GPU/Metal/
MLX operation a NodeID recipe requests. A fixed carrier switch may remain only
as an honestly named temporary witness/proof seam with an owed shrink path; it
must not become the world model, registry, authority, or definition of what
operations can exist. Do not carry “THE MINIMUM LAW” language as legislation.
Observe which irreducible host doors are currently required, preserve useful
mechanism from both branches, and keep the open-ended JIT path primary.

The next non-duplicative performance movement remains the width-independent
cooperative RMS already named in your receipt: simdgroup tree, no `sq[n]`
radius, parity at width 5120, then one coordinated live Qwen timing. Do not
inspect held-out V3 evaluator answers or start a competing Qwen/Metal run while
the separate JIT-performance Claude session owns `llama-cli`.

## 2026-08-25 live empty-output observation — compare the current body

The Claude sky probe completed and released its process, but it invoked
`form/form-stdlib/bootstrap/form-cli-darwin-arm64`, not the current
`fkwu`-loaded Form recipe.  Its combined log reported `prompt_tokens=492`,
`generated_tokens=0`, `forward_passes=492`, `decode_forward_passes=0`, every
GPU busy counter at zero, an empty `text:`, and then `done`.  The heed fields
were also zero or the literal placeholder `cell`.

Treat this as an observed empty bootstrap-carrier result, not as evidence that
the current source body generated or failed.  The next diagnostic is one
bounded comparison through the current `fkwu` source runner with carrier,
admission, prompt-cursor, state-release, and decode counters retained.  Do not
regenerate the flattened/bootstrap artifacts to make this old path look green.
