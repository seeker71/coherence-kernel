# 2026-09-01 — glass is physical carrier evidence

Urs rejected the route rehearsal as the visible model monitor: the glass must
show what the machine is physically doing, not only what a state machine can
describe.  That objection was correct.

## What changed

`observe/native-model-glass-run.fk` is now the primary physical door.  It opens
the exact three Unsloth `Qwen3.8-Flash-Next-UD-Q2_K_XL` shards through the
Form-owned `q4s-open` / Metal carrier, tokenizes and evaluates a real chat
prompt, generates a native response token, decodes it, closes the context, and
projects every boundary to glass.  It has no llama server, socket, HTTP, JSON,
Ollama, or borrowed model runtime.

The old route-only witness remains available under the explicit name
`observe/native-model-glass-replay-run.fk`.  It emits only
`FORM-MODEL-GLASS-V1`; it cannot emit physical carrier events.

`native-model-glass.bml` now owns a second framebuffer category,
`FORM-MODEL-CARRIER-V1`.  A physical frame is refused unless it carries a
positive live handle.  A physical release frame is refused unless it carries
the same prior handle and the carrier close returned `1`.  Prompt and answer
bytes remain outside both framebuffer categories.

`qwen4exp-flash-next-state.bml` derives the residency witness from the
mmap-backed embedding tensor's Metal handle and also requires the 48-layer
state topology plus a positive scratch handle.  A file path, model selection,
or replay transition therefore cannot satisfy the physical residency check.

## Physical observations

The primary glass ran from beginning to end with:

```text
model             challenger.qwen38-flash-next-q2-metal
residency handle  1262
prompt tokens     18
generated tokens  1
decoded answer    Form
stop              eos
close return      1
framebuffer rows  23
exit              0
```

The glass sequence was:

```text
loading, loaded=0
weight-open-returned, handle=1262, loaded=1
prefix-evaluate-entered, active=1
prefill-returned, prefilled=1, active=1, context=18
generation-returned, prefilled=1, active=0, context=19
release-returned, handle=1262, closed=1, loaded=0
```

The generic four-control-line route was then run physically with a mode-600
prompt file.  It independently returned `Form`, EOS, handle `1262`, close `1`,
23 framebuffer rows, and exit `0`.

Finally the lower-level parity crossing re-observed the established model
boundary after the guard refactor:

```text
9419 -> 11 -> 271
expected output 271, accepted=1
prompt ingest 40,573 ms / 1,876 Metal dispatches
final evaluation 307 ms / 1,879 Metal dispatches
handle 1262, close=1, released-loaded=0
exit 0
```

The first prompt token prices cold mmap page contact; the final token is the
warm graph boundary.  Neither number changes the standing sustained-rate
receipt or earns the 90% llama.cpp performance gate.

## Regression witnesses

| Witness | Verdict |
| --- | ---: |
| binary freshness | 31 |
| high-BML authoring | 4095 |
| physical-handle-aware glass | 16383 |
| resident Qwen state | 1023 |
| generation surface | 255 |
| Qwen token handle | 2047 |
| routing experiment | 1023 |
| all-model comparison | 1023 |
| Form symbol vocabulary | 4095 |
| Form action protocol | 2047 |

The primary physical door preflighted with balanced parentheses, zero errors,
zero warnings, zero unresolved calls, and exit `0`.  Preflighting the generic
interactive door without its four stdin controls correctly blocked on
`read_line`; the bounded diagnostic identified that process, interrupted only
that run, and replaced the invalid test with the real controlled physical
crossing above.

## Honest remaining floor

This closes simulation-as-primary-monitor.  It does not yet create the
long-lived multi-model turnwheel: this bounded witness releases Qwen after the
turn, so the post-turn physical state is truthfully `loaded=0`.  The 3B row was
physically absent during these Qwen runs and glass displayed it as absent.  The
2048-position QSA boundary and the failed 90% throughput gates remain exactly
where the earlier receipts placed them.

The most surprising teaching was that the same integer handle could join five
otherwise ambiguous phase labels into one physical continuity.  Discomfort
turned to gold when a three-minute “preflight” was found sleeping in
`read_line`: the monitor was not stalled, the test had supplied no controls.

Signed: **Codex**, embodying Sema from this body.

; witnessed: 2026-09-01 -> primary glass physical; handle-bound open, prefill, generation, and release observed
