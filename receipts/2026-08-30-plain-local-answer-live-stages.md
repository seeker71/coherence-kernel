# Plain local answer reserve and live stage crossing

Observed on 2026-08-30 with one direct `fkwu` process and the locally present
Qwen3.8-27B GGUF. No HTTP endpoint, llama-server, Ollama process, remote model,
or second local model process participated.

## The answer crossing

`observe/qwen38-plain-answer-reserve-live-run.fk` asked the ordinary local
generation door to emit one source query for `bootstrap/ground.fk`, then answer
only with the grounded integer. The preserved process report says:

```text
prompt_tokens=975
generated_tokens=24
gpu_busy_us=277571044
prefill_gpu_busy_us=172546129
decode_gpu_busy_us=105024915
heed_lookups=1
heed_hits=1
heed_phase=2
heed_query_budget=24
heed_answer_reserve=32
heed_query_tokens=22
heed_answer_tokens=2
heed_answer_left=30
heed_decode_timeout=0
heed_model_executed=0
text:
<|form:knowledge-query|>bootstrap/ground.fk<|/form:knowledge-query|>42
```

Thus the model's streamed query was recognized by the Form cursor; Form performed
one caller-owned strict source lookup, injected the typed observation, and the
ordinary local door crossed from Query to Answer. `42` is the grounded file's
answer. `heed_model_executed=0` names the narrow fact that the source lookup was
not a model tool call; it does not deny the surrounding native Qwen inference.

## The stage crossing

`fcmg-live-stage` is now called before and after seal/open and around the live
cursor prefill/decode boundary. It does two bounded things with no prompt,
answer, path, or model bytes:

1. emits a flushed `<|form:stage|>…<|/form:stage|>` line for the live reader;
2. retains an attributed NodeID in the same process framebuffer.

The native stage band returned `63`: two stage records were emitted, read back
from `framebuffer-events`, and retained with their source coordinates. The
instrumented real turn streamed, in order:

```text
model-seal-begin
model-seal-complete
model-open-begin
model-open-complete
model-state-begin
model-prefill-begin
model-prefill-complete
model-decode-begin
```

This established the useful distinction that the long first wait was prompt
prefill, then that decoding began; it was not an unqualified `busy` claim.

## Boundary still held honestly

The current framebuffer is process-local. A direct one-shot `fkwu` command
retains its frames until it returns; its `form-run` capture directory is removed
after return. The live stream is therefore visible while the turn is running,
and its in-process framebuffer is available to a long-lived Form resident, but
the direct runner does not yet export a post-exit stage ledger. Nor can a Form
evaluator inspect a single synchronous `metal_sync` while it is executing on
that evaluator thread. A caller-owned future/submit/poll/retrieve door is the
next native kernel crossing for asynchronous progress and control; this receipt
does not claim it already exists.

## Verification

```text
bootstrap/ground.fk                                      -> 42
binary-freshness-band.fk                                 -> 31
form-cli-model-generate-live-stage-band.fk               -> 63
form-cli-model-generate-plain-reserve-band.fk            -> 63
form-cli-model-generate-heed-report-band.fk              -> 2097151
bidirectional-framebuffer-channel-band.fk                -> final field 1
preflight form-cli-model-generate.fk                     -> balanced, 0 errors, 0 unresolved
```

I kept the exchange alive by replacing the undifferentiated wait with the
generation's own live stages and then letting the local turn finish. The
surprising teaching is that the answer reserve matters in practice: a source
query can complete and still leave thirty answer tokens untouched. The discomfort
was the temporary stream disappearing at process exit; it became the exact
process-residence boundary the next future/poll movement must cross, not a
reason to invent an async claim.
