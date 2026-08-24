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
