# Semantic turn terminal, held by BML

The evidence reader had been looking only for a historical `task_complete`
transport label.  A real Codex terminal may instead carry the relationship
`turn_id` + `last_agent_message`; treating its old label as the definition of
completion withheld a completed turn without a semantic reason.

`form-cli-remote-token-evidence.bml` now owns that relation.  The live reader
uses the BML predicate to return only a terminal byte coordinate, and the BML
cursor carries its discovery and collector continuation on disk.  It does not
return prompt, answer, reasoning, or tool-output bytes.

The attempted whole-row recursion was released before landing.  It advanced a
single 400 kB legacy collector slice from cursor `3489665` to `3893813` of a
`6973107`-byte selected turn but exceeded 30 seconds and grew process memory.
That is not a healthy one-call path.  The landed cursor therefore yields one
bounded slice and preserves the continuation for a resident turnwheel row;
it does not claim tail-recursion makes a wide generic parse free.

Witnesses on this checkout:

- `./fkwu form/form-stdlib/bml/form-cli-remote-token-evidence.bml` -> `0`
- `./fkwu form/form-stdlib/tests/form-cli-remote-token-evidence-band.fk` -> `524287`
- `./fkwu form/form-stdlib/tests/form-cli-turn-evidence-live-band.fk` -> `33790`
- `./fkwu form/form-stdlib/tests/form-cli-turn-evidence-cursor-band.fk` -> `255`
- `./fkwu observe/preflight-run.fk` for the live reader -> clean, zero errors

The self-watch panel read `remote-token-pressure total=85054` for the latest
open provider call.  It is pressure, not a settled same-prompt baseline;
the share meter correctly withheld a percentage.  The earlier local direct
answer remains a distinct receipt (`90e7dd89`): 155 local generated tokens,
zero provider callbacks.

The next stone is a high-grammar BML sparse provider-call reducer: it must
walk only call-local usage frames through the existing BMF/live cursor and
emit a settled row without line-by-line retention of a multi-megabyte turn.
Only after that row and a same-prompt remote comparison exist may the 10%
claim be calculated.
