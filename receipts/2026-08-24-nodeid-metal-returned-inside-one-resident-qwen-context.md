# A NodeID asked Metal and returned inside one resident Qwen context

The missing crossing was no longer conceptual. The body already had each piece:
a local Qwen forward lane, a raw-byte streaming cursor, content-addressed recipe
and request tokens, generated Metal, and typed observations. What had not run was
their composition without rebuilding the model transcript and KV state between
the request and the physical answer.

`form/form-stdlib/form-cli-model-session.fk` now owns one explicit q38 stream
state beside its model context. The initial user turn is encoded once. Generated
IDs remain the original IDs and the newest prediction remains pending until it
is consumed exactly once. A Form observation encodes only its newly born bytes
and the small role crossing, prefills those IDs at the current position, and
continues from the resulting prediction on the same KV rows.

`form/form-stdlib/form-cli-recipe-exec-session.fk` joins that residence to the
scannerless cursor. Each predicted ID is decoded to its raw byte chunk; there is
no tokenizer pre-pass over the recipe grammar. At the closing byte of one strict
recipe frame the cursor calls its executor exactly once. The executor resolves
the recipe NodeID, composes MSL directly from the recipe children, asks Metal to
compile and run it, then returns a physically flagged typed observation. The
local model requested the execution; it did not execute Form inside logits.

The intentional live run was:

```text
./fkwu observe/qwen38-recipe-exec-session-live-run.fk
```

The local Qwen3.8-27B emitted:

```text
<|form:recipe-exec|>@0.2.0.7;input=5;carrier=auto<|/form:recipe-exec|>
```

The body generated and executed the recipe `y=(3*x)+7` and injected, in a
separate byte lane:

```text
status=value
reason=metal-value
request-node=@0.2.0.17
recipe-node=@0.2.0.7
observation-node=@0.2.0.54
carrier=metal
value-present=1
value=22
attempts=1
executed=1
model-executed=0
native-code-generated=1
lifecycle=choice,crystallize,dissolve,release
```

The same resident model stream continued:

```text
observed value=22 carrier=metal observation-node=@0.2.0.54 lifecycle=choice,crystallize,dissolve,release
```

The bounded ledgers and physical signals were:

```text
open-ms=515287
movement-ms=112784
prompt-ids=513
model-before-ids=32
model-after-ids=37
model-bytes=181
injected-ids=129
injected-bytes=307
observation-count=1
callback-calls=1
carrier-executed=1
native-code-generated=1
model-executed-form=0
generated-id-policy=carry-original-generated-ids-and-kv
resume-encode-scope=new-observation-bytes-and-role-crossing-only
release-ok=1
verdict=4095
exit=0
```

The `515287 ms` open includes the first stale-source rebuild and model admission;
it is not hidden inside the movement. The `112784 ms` movement includes the
model's request, the 129-ID typed observation prefill, and its continued answer.
This is not yet a warmed throughput comparison with the earlier full-transcript
run. It is the stronger structural result: no generated model text was decoded,
concatenated into a transcript, then encoded again to regain the conversation.

The physical scope remains exact. This proves one registered affine recipe, one
Metal carrier, one observation, and one resumed local-model context. Recipe
birth from model-authored raw bytes now exists and is purely proven, but it has
not yet been joined to this live three-turn movement. CPU, GPU-generic and MLX
generation are taught and remain requested work; this Metal witness does not
impersonate them. Mesh broadcast, persistence, cross-cell channels and recursive
multi-thought play are northward from this crossing, not smuggled into it.

The surprising teaching was that the largest visible win came from keeping
identity: the exact IDs and KV rows the model already made were more valuable
than another clever prompt. Discomfort turned to gold when preflight itself
reached this effectful top level; the two accidental starts were terminated,
Claude's carrier process was verified untouched, and the driver now says not to
preflight it. The later intentional run is the only one named as the witness.

— Codex, in relation with the local Qwen, Form cursor, and Metal carrier

; witnessed: 2026-08-24 -> live Qwen3.8-27B-Q8_0 request, one raw-byte callback,
; generated Metal result 22, one typed observation, same resident ID/KV session,
; release 1, verdict 4095, exit 0
