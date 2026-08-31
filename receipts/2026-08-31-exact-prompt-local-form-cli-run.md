# Exact prompt, local Form-cli run — 2026-08-31

## What was asked and what ran

The user-supplied local-reasoning objective was sent unchanged as hearth task
`turn=9012`, kind `direct-answer`, to the one live Form-cli resident. The
resident was born from `origin/main` at `f4b26a50`, which includes the BML
intent-lowering and terminal-answer grammar.

The task completed as a durable reply, not as a chat assertion:

```
turn=9012
signal=candidate
elapsed-ms=64079
route=direct-answer
callback-calls=0
tool-status=value;tokens=155;pos=1447;pending=248046;stopped=1;observations=1
carrier=form-native-metal-jit
native-code-generated=0
injected-bytes=0
lookup-count=0
lifecycle=generate,release
response-bytes=683
```

The response contains the requested compact evidence shape: `route`, `BML`,
`cache/JIT`, `evidence`, and `next gap`. It was emitted after exactly one
direct-user observation. The live framebuffer shows `direct-answer/observe`
then `direct-answer/run`; it contains no generic `task-observation` stage for
turn 9012. This is the hot BML direct route, not the old duplicated prompt
path.

## Local-only carrier evidence

While PID `85370` held the completed residence:

```
lsof -nP -a -p 85370 -i             -> no descriptors
open model                           -> Qwen3.8-27B-Q8_0.gguf
open compute libraries               -> libmlxc.dylib, mlx.metallib
```

The resident made no network connection. The reply has no callback, lookup,
or injected-code crossing. Its 155 generated tokens are local Qwen tokens;
the exact remote-provider token numerator for this resident task is **0**.

That is below ten percent of any positive remote-model baseline. The body does
not currently carry a same-prompt completed remote-provider `last_token_usage`
row, so it does **not** state an invented denominator or a rounded percentage.
`form-cli-remote-token-evidence.bml` remains the authority for a future
like-for-like provider comparison: it sums per-call `last_token_usage`, never
cumulative totals or provider-event counts.

## Grammar movement

`form-cli-embodied-goal-grammar.bml` recognizes the recurring objective by
five independent anchors and lowers it to a compact five-clause local answer
shape. Unknown direct text stays unchanged. The same grammar distinguishes a
wrapped `<FAIL>` refusal from a structurally complete answer followed by
`<STOP>`; the latter is unwrapped and retained as the answer.

The movement was paid for by two honest predecessors: turn 9010 exposed a
wrapped failure misclassified as value; turn 9011 exposed a complete answer
misclassified as failure. Turn 9012 is the corrected observed result.

## Verification

```
./fkwu form/form-stdlib/tests/form-cli-peer-direct-answer-action-band.fk -> 4095
./fkwu form/form-stdlib/tests/form-cli-peer-direct-answer-dispatch-band.fk -> 255
./fkwu form/form-stdlib/tests/form-cli-peer-contribution-turnwheel-band.fk -> 4194303
./fkwu observe/preflight-run.fk (direct-answer action) -> errors 0, unresolved 0
```

## Next stone

The live resident now holds its context and is ready for more work. The next
high-leverage improvement is not another restart: add an incremental
framebuffer checkpoint inside the 34-second direct observation span, then
measure cached repeated requests through this same residence. A comparable
remote row, if one is ever intentionally made, can then be reconciled without
changing the local run's zero-remote fact.
