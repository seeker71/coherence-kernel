# The tool call as a Form control (2026-10-08)

Urs: "to call tools we shall use form native substrate node ids or node serialization, JSON means it is not form native and is
adding way too much overhead." This receipt is the census, the protocol, what ran and what did not land.

## What the census found (observe/native-tools-census-run.bml, observe/native-tools-tokens-run.bml)

The brief's picture (the model writes `{"tool":...}`, the lane parses and repairs it) is the lane of 2026-10-04. Since
2026-10-05 11:28 (d4c72621d) the live lane hands only an `@address` to its JSON route (`fcac-replied`), so a JSON reply is
answered with the no-action note; the call is an eval returning a `form-code-action` cell. The JSON machinery
(`fcctg-repair`, the tolerant reader, `fcap-step`) is still in the sources, still pinned by the turn-guard band, and
unreachable from the model.

609 retained replies (a codex worktree's 535, this session's two rounds, the main checkout's 10), three eras by timestamp:

| era | replies | a call written valid first try | read only after the tolerant reader | never read | echoed the observation | prose / empty |
|---|---|---|---|---|---|---|
| E1 JSON era (before 10-04 10:11) | 340 | 148 json-tool (64% of the 232 that tried a call) | 24 | 5 | 40 | 15 |
| E2 cell era | 94 | 62 (22 eval, 40 json) | 5 | 1 | 10 | 0 |
| E3 packet era (10-05 11:28 on) | 175 | 131 eval actions (82% of 160 that tried one) | 0 | 2 | 0 | 8 prose, 19 empty |

Parse cost per reply, microseconds, quiet machine (load 5): JSON valid + parse 79, turn-guard repair 88, the tolerant reading of
a JSON reply that needed it 14,267, control scan 2, the eval's cursor 763, a whole eval 905, the tool control's reader 123.
Tokens per call on the Qwen3.8 tokenizer (7 real calls): JSON 46, eval constructor 60, tool control 54, the words alone 39.
JSON is the cheaper envelope; the control's envelope (`<|form:tool|>` ... `<|/form:tool|>`) costs 15 tokens. What the
control buys is validity (a block is verbatim, nothing is escaped) and a stream the lane answers by name.

Where JSON is written or read as text on the lane's files (`json-emit(`, `parse-json(`, `faj-json(`, `faj-valid(`):
policy 52, live 7, swerve 9, turn-guard 20, session 0, memory 14, request 19, circle 18, grammar 3, agent-tools 10, tool wire 3,
verbs 0, tool control 0. `form-native-tools-guard-band.bml` pins those as ceilings.

Per tool call the lane's own native cost is not JSON: the turn guard hashes the call and the result with a BML sha256 at
about 4 us a byte (66 ms for a 16 KB stdout), of which json-emit is 2.4 ms. Seeds and checkpoints are keyed on those digests,
so they stay.

## The protocol (form-token-tool.bml, docs/form-native-agent-tools.md)

```
<|form:tool|>edit config.json
<<<<
{"enabled":false}
====
{"enabled":true}
>>>>
<|/form:tool|>                 ->  <|form:observation|>node=@0.2.0.8358002;<|/form:observation|>
```

A call is the tool's name, words (bare, "quoted", or the `@address` of a text cell) and an optional verbatim block. One cursor
reads it into the `form-code-action` cell the lane steps (the same constructor `fcap-native-tool` uses: the same call is the
same node id); tools are `tool-spec-cell` nodes; a call that does not read answers a named `nothing=tool-...` in the stream.
`ftt-render` spells any call back (339 of 339 real calls, eval and JSON, read back as the same cell).

## What ran (local Qwen3.8-27B-Q8_0 through observe/form-cli-code-run.fk, gpu-quiet around each run)

The prompt teaches the form by default (`FORM_CODE_CALL_FORM=eval` teaches the constructor). Across 14 tool-arm and 9 eval-arm
runs on small tasks (several prompt revisions): the tool arm wrote 50 tool controls, 7 refused by name (5 were the model
reading an `@address` as a path, fixed in the prompt; 2 a context-field read written as a tool); the eval arm wrote 94 eval
controls and 33 were refused (`unknown-name:code_context` 19, `task_context` 10, a forged observation 6, parse and arity 4).
The one task whose caller checks pass (config.json): the tool arm completed in 5 turns, 3 calls, 0 repairs, 188 generated ids,
105 s; the eval arm in 6-7 turns, 4-5 calls, 0 repairs, 450-526 ids, 177-228 s. Edits whose old span holds quotes and
backslashes (`say \"hi\" now`) went through as the file holds them. The other fixtures' caller checks stayed red in both arms
(the failed check shows an empty expected stdout; replayed offline the same rows pass; cause not found).

## What did not land, and why

- The tool observation's tail through the exact BPE cursor. A crossing's `encoding_ms` was 9,000 to 17,000 (the reference
  encoder scans the whole merge table per piece: 16 s for a 60-byte observation, 36 s for 432 bytes); the indexed cursor gives
  the same ids in 36-43 ms (`form-tool-tail-indexed-band.bml` = 7). Wiring it is this change in `fcms-observation-ids`:
  `let indexed = fcmo-session-tail-ids(s, fcmo-tool-content(text)); tkz-cat(closed, nothing?(indexed) ? fcms-tool-tail-ids(src, text) : indexed);`
  With it, live crossings fell from 9,000 ms to 1-110 ms. It is not in the branch because this
  one changed line makes the runtime's lowering of the lane's own units come out wrong, deterministically: with origin/main
  plus the tool, grammar and policy edits the door compiles clean (4 of 4 cold compiles); add that call site and
  `observe/form-cli-code-run.fk` reports `stray ')'` or `input-ended-mid-form` in `form-cli-code-policy.bml`, or the policy's
  string constants come out as slices of other strings; a dummy def in the same file, or a comment, does not. Stubbing the
  turn guard's lenient JSON reader (dead on the live lane) made it compile clean again, so it looks like a size ceiling in the
  lowering of the lane's closure. Reproduce: apply the line, delete every `*.lowfk`, `*.discovery.fkb`, `*.fkb`, `*.sym` under
  `form/` and `observe/`, run `echo '{}' | ./fkwu observe/form-cli-code-run.fk`.
- The same lowering also dropped string constants of `form-token-tool.bml` itself in a door-first cold compile (the category
  name and three reason names; four sites, the same four whatever they are called). The tool band (4095) is the canary: it
  fails loudly. A cold compile should go band first. A runtime fix (`runtime/fkwu-uni.c`, the string melt) asks the siblings.
- Decode under the grammar. The session lane has no logit mask, so a malformed call is refused in the stream, by name, after
  it is written. The first-try validity measured above is prompt only.
- The dead JSON tissue (the tolerant reader, call repair, `fcap-step`) is not removed: the turn-guard band pins it (bits 128
  to 1024, 262144, 524288) and its replay claims cannot run in a fresh checkout.
