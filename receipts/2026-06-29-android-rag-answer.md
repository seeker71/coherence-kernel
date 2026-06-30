# Receipt — Rung 5: grounded native answer on the S23

**Status: WITNESSED on device.** The Galaxy S23 / Adreno path generated the known answer tokens, checked them
against the Rung 3 reference vector, retrieved a real grounding source, and wrote a cited answer sidecar.

## Gate

Query/prompt:

```text
The capital of France is
```

Grounding source pushed to the device:

```text
receipts/2026-06-29-android-native-generate.md -> /data/local/tmp/formvk/rung4_ground.txt
```

The device recipe required all of these to pass:

```text
generated token vector matches the 12-token Rung 3 reference
grounding source contains the query text
grounding source contains the generated answer text
```

## Device run

```sh
adb -s R5CW20DK17A push model/form-llama-generate-rung3-WIP.fk /data/local/tmp/formvk/rung3.fk
adb -s R5CW20DK17A push receipts/2026-06-29-android-native-generate.md /data/local/tmp/formvk/rung4_ground.txt
adb -s R5CW20DK17A shell 'cd /data/local/tmp/formvk && : > rung5.mode && FORM_KERNEL_STACK_MB=2048 timeout 1600s ./fkwu --src rung3.fk'
```

Observed output:

```text
5
```

Observed `rung5.answer`:

```text
answer=Paris. The Eiffel Tower is located in Paris. citation=receipts/2026-06-29-android-native-generate.md token_check=12 grounded=1 answer_hit=1
```

## Honest floor

This witnesses a grounded native answer on the device. It does not yet witness the `form-cli` one-invocation
local route, build/test loop, proxy fallback, or capstone; those remain Rungs 6-9.
