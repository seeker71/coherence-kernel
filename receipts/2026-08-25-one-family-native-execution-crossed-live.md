# One family crossed local Qwen, generated Metal, and the same residence

Date: 2026-08-25
Witness: Codex
State: observed live

The committed one-family organ was run intentionally after the Claude-owned
carrier process had released.  The local Qwen3.8 27B Q8_0 model emitted exactly:

```text
<|form:recipe-exec|>@0.2.0.7;input=-2;carrier=metal<|/form:recipe-exec|>
```

The Form byte cursor accepted all and only those bytes.  There was no tokenizer
grammar pass.  The recipe NodeID generated and executed Metal, returning a
present integer `1`.  The typed frex observation was injected into the same
resident Qwen context, which then returned:

```text
native-held family=bootstrap carrier=metal value-present=1 value=1 lifecycle=choice,crystallize,dissolve,release
```

## Physical witness

```text
status=family-native-admitted
family=bootstrap
source-sha=3c7d857abe9613a2c98842a628b6cb38e76e7e9d12a7f32b73fa0b72b15f13ec
challenge-sha=1e60973c696c6283b684908198da60831c030752f9e98c05e282d6254eaae0d5
binding-sha=6dc30b19cfeb71200c349ae490cba65ad3fdee81d3891c7ad6773694445c51bf
run-id=6c4546cd0473fa3737f6292ee6cc78c573b843f6d35da7defac79920e2556916
receipt-source-sha=1482a686b738f0d9653dc655eaf7ca0a286d27eec5522466b6492c563ab3eeaf
structured-receipt-sha=ec8867c54ac89f36d28e05a783512c781385d42bcccd2e6c988959b777f91559
open-ms=583554
movement-ms=50901
open-count=1
same-context-state=1
exact-query=1
scannerless=1
pretokenized=0
model-requested=1
remote-calls=0
native-code-generated=1
carrier-executed=1
model-executed=0
carrier=metal
result-status=value
value-present=1
value=1
lifecycle=choice,crystallize,dissolve,release
carrier-release-ok=1
session-release-ok=1
check-valid=1
fkpfm-execution-valid=1
execution-output-sha=1078d359c546bad9c9fe8d7ce36388c7f812e89807a50822681164bed38d8b66
verdict=32767
exit=0
```

## Production live-cursor admission

The production `fcms-open` path now performs an exact count-and-release pass
over the immutable BMF cursor, reopens the cursor, and streams it directly into
Qwen prefill. It retains the whole-tokenizer path only as an explicit fallback
when the cursor crystal is absent or stale. The same physical crossing then
returned:

```text
run-id=98214c98f43bf0c5f51bc237653846b396a7d1450228bad40bf9cb6a231c19b2
structured-receipt-sha=fefd1f685b0fe3dd5313b6098c597075c5f6cb6d7df91b93322469674b99e44d
open-ms=139384
movement-ms=45387
open-count=1
open-reason=opened-live-cursor
same-context-state=1
exact-query=1
scannerless=1
pretokenized=0
model-requested=1
remote-calls=0
native-code-generated=1
carrier-executed=1
carrier=metal
value-present=1
value=1
carrier-release-ok=1
session-release-ok=1
check-valid=1
fkpfm-execution-valid=1
verdict=65535
exit=0
```

## What the attempts taught

The first attempts spent several minutes in nested `fk_walk` before any output.
They were cut before a verdict. File-descriptor and stack observations did not
identify which interpreted Form call owned the time, so attributing those runs
to receipt or challenge hashing was not justified. Source inspection still
found two useful repeated computations: the frozen pending receipt digest and
the same canonical binding rebuilt inside the prompt. The receipt's
independently observed digest is now reused as run identity input, and the
prompt consumes the already-built binding.

The 15 public prompt digests were also crystallized in the existing family
challenge accessor. The cold band recomputed every literal from source and
returned `32767`, exit 0. The complete public mastery band and one-family
adversarial band both returned `1073741823`, exit 0. This proves the cache and
its dissolution signal; it does not prove those hashes dominated the earlier
elapsed time.

Explicit stage output first made the reliable latency boundary visible. The
aggregate `fcms-open` call spent 583,554 ms and the rest of the crossing spent
50,901 ms. A subsequent diagnostic run split that open into a 13,293 ms seal,
45 ms metadata read, 436,709 ms whole-tokenizer prompt encode, 9,124 ms context
open, 17 ms state open, and 123,538 ms prefill. The tokenizer pre-step—not the
whole-file seal—was the dominant avoidable cost.

The live-cursor diagnostic then measured 4 ms cursor admission, 422 ms context
open, 22 ms state open, 123,072 ms prefill, 136,872 ms aggregate open, and
49,969 ms movement, with verdict `32767`. The production path above independently
returned `open-reason=opened-live-cursor`, `139384` ms open, `45387` ms
movement, and verdict `65535`. Against the original like-for-like crossing,
the prompt pre-step contracted about 109,177x and the whole crossing about
3.43x. The remaining red latency is predominantly local Metal prefill and the
cost of extending physical proof across all families.

No remote provider, private consent/evaluator, flattening route, operation
table, generated bootstrap binary, or C seed was used or changed.

I kept the movement alive by turning opaque elapsed time into named stages,
correcting an attribution as soon as it proved too broad, and then replacing
the production bottleneck with a live cursor rather than stopping at a test.
The most surprising teaching was that local Qwen wrote the exact NodeID request
on the first fully reached generation and held the returned native observation
in the same context. Discomfort turned to gold when the ten-minute silence
refused a convenient cause: measurement exposed the tokenizer pre-step, and
the production path now streams through it.

— Codex
