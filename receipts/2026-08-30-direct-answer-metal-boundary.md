# 2026-08-30 — Direct-answer boundary: local execution observed, Metal wait named

## What crossed

The direct-answer action now carries a content-free generation-status tuple at
durable egress:

```
signal;tokens=<n>;pos=<n>;pending=<id>;stopped=<0|1>;observations=<n>
```

`observe/form-cli-peer-contribution-live.fk` also emits the resulting resident
position, pending ID, stopped bit, and observation count after each committed
turn.  Those fields hold no task or response bytes.  They let a future turn
locate a local model failure at the stream/carrier boundary rather than
misdescribe it as a retrieval, policy, HTTP, or remote-model failure.

The direct action is selected by the existing dynamic action route.  The
scannerless default action now names `direct-answer` explicitly instead of
letting it drift into generic model selection.  The actual model/KV session,
task identity, append transaction, and reply frame remain caller-owned.

## Live evidence

The earlier resident was released through its own FIFO bell before a fresh
local successor admitted the sealed local artifact:

```
/Users/ursmuff/models/qwen38-27b/Qwen3.8-27B-Q8_0.gguf
```

One sealed `v303` row ran through `route=direct-answer`.  Its durable
metadata was:

```
model-executed=1
callback-calls=0
injected-bytes=0
lookup-count=0
carrier=form-native-metal-jit
elapsed-ms=1334
tool-status=value;tokens=3;pos=743;pending=248046;stopped=1;observations=1
response-sha256=ec001fe0f6360da2774c68418705beca9a404953a1d3d9b282051e0a847a53d3
exact-ppm=0
error=1
```

The tokenizer witness identifies pending ID `248046` as Qwen's
`<|im_end|>`.  Thus the turn ended normally after three generated IDs; neither
the spool, policy action, nor an external server manufactured the result.  It
is still a held-out model miss, not learned knowledge.

There was no `llama-server` or `ollama` process and no HTTP endpoint in either
run.  The only model process was the Form-launched `fkwu` resident.

## The user-role experiment and its refusal

An isolated successor was born to test the hypothesis that an ordinary
direct-answer task should cross as a ChatML `user` message rather than the
existing typed tool observation.  The one durable sealed task frame was
accepted, but no reply frame arrived before the test process was ended. Its
cold admission printed only when the terminal pipe closed:

```
admit-prefill-ms=142564
```

The bounded macOS sample was taken only seconds after that cold admission and
placed the resident in:

```
fk_metal_sync_external -> fk_wait_observed
```

with its command buffer still incomplete. The process was this run's own fresh
test resident and was terminated before its carrier watchdog or a
caller-provided execution deadline could answer. Its 1.2 GB mapping was
released; the task spool remained at 181 bytes and replies at 0 bytes. This is
an **interrupted, unscored experiment**, not a demonstrated user-role failure.
The candidate user-role code was not landed as the default action.

The carrier's existing watchdog is a fixed five-minute poll in
`form/native/metal/fk-metal-carrier.m`; cold admission itself consumed 142.564
seconds in this run. Neither event was visible durably to a client while the
terminal pipe remained open. The next repair is a caller-controlled
Form-visible readiness/sync deadline plus typed admission and command-buffer
status frames, threaded from the resident action/control plane. It must
preserve completed, error, timeout, and release as distinct observations; it
must not turn a wait into a guessed success or retry the same task.

## Verification

```
bootstrap/ground.fk                                      42
binary-freshness-band.fk                                  31
preflight direct-answer action / stream ingress / live    clean
form-cli-peer-direct-answer-action-band.fk                255
form-cli-peer-stream-ingress-band.fk                      2097151
form-cli-peer-policy-route-band.fk                        32767
```

## Fresh local-health map

| Surface | Observed floor | Next locally actionable gap |
| --- | --- | --- |
| Local reasoning | One native Qwen/KV direct turn executed; sealed held-out score remains 0 | Durable cold-admission status, then a fresh one-row observation |
| Form-native JIT/carrier | Dynamic direct-action route and native carrier are live | Surface command-buffer timeout/error as Form data |
| Scannerless BMF ingress | Explicit direct route passes its 2097151 band | Re-witness after the carrier deadline exists |
| Diagnostics | Stream position/pending/stopped/observations are durable and answer-free | Include admission readiness plus carrier completion/error/elapsed state without raw prompt/answer bytes |
| Dependencies | Qwen artifact is local; no llama/Ollama process is needed | Preserve artifact/toolchain receipt across restart |
| Persistence/recovery | One append row and one reply frame were length-safe and scored | Define timeout recovery that retains the uncommitted staged row exactly |
| Landed work | This receipt and diagnostic action are ready for commit | Rebase and push this coherent movement |

I kept the exchange alive by treating the zero-score, cold admission, and the
interrupted experiment as different signals, then changing the durable evidence
so the next attempt can see which it meets. The surprising teaching was that an
apparently small direct action could prove the local model/KV route while a
terminal pipe hid a 142.564-second admission from the client. The discomfort
of terminating too early became useful only by correcting the record, leaving
the task and reply ledgers intact, and refusing to call interruption a timeout.
