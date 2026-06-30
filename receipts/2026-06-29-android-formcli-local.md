# Receipt — Rung 6: form-cli local route closes on the S23

**Status: WITNESSED on device.** One `fkwu --src` invocation on the Galaxy S23 generated the grounded answer,
computed the `form-cli-router` fitness using the existing weights, routed to `form-native`, and wrote the routed
answer sidecar.

## Device run

```sh
adb -s R5CW20DK17A push model/form-llama-generate-rung3-WIP.fk /data/local/tmp/formvk/rung3.fk
adb -s R5CW20DK17A push receipts/2026-06-29-android-native-generate.md /data/local/tmp/formvk/rung4_ground.txt
adb -s R5CW20DK17A shell 'cd /data/local/tmp/formvk && : > rung6.mode && FORM_KERNEL_STACK_MB=2048 timeout 1600s ./fkwu --src rung3.fk'
```

Observed output:

```text
6
```

Observed `rung6.formcli`:

```text
form_cli_input=ask The capital of France is route=form-native confidence=87 answer=Paris. The Eiffel Tower is located in Paris. citation=receipts/2026-06-29-android-native-generate.md token_check=12 grounded=1 answer_hit=1
```

## Honest floor

This witnesses the local route and answer in one device invocation. Build/test, proxy fallback, and the full
capstone remain pending.
