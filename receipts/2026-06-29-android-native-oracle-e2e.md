# Receipt — Rung 9 CAPSTONE: native oracle loop on the S23

**Status: WITNESSED on device.** One Galaxy S23 `fkwu --src` invocation ran the local grounded-answer path, the
build/test feedback path, and the low-confidence proxy fallback path. The capstone used two prompts: one local
answer prompt and one fallback prompt.

## Device run

Mac remote oracle stub listened on `0.0.0.0:18088`; device proxy host file contained `192.168.1.225`.

```sh
adb -s R5CW20DK17A push model/form-llama-generate-rung3-WIP.fk /data/local/tmp/formvk/rung3.fk
adb -s R5CW20DK17A push receipts/2026-06-29-android-native-generate.md /data/local/tmp/formvk/rung4_ground.txt
adb -s R5CW20DK17A push /tmp/rung8_host.txt /data/local/tmp/formvk/rung8_host.txt
adb -s R5CW20DK17A shell 'cd /data/local/tmp/formvk && : > rung9.mode && FORM_KERNEL_STACK_MB=2048 timeout 1800s ./fkwu --src rung3.fk'
```

Observed output:

```text
9
```

Observed sidecars:

```text
FORMCLI
form_cli_input=ask The capital of France is route=form-native confidence=87 answer=Paris. The Eiffel Tower is located in Paris. citation=receipts/2026-06-29-android-native-generate.md token_check=12 grounded=1 answer_hit=1
BUILD
code_request=emit-form-four-arg-smoke build_cmd=./fkwu --src rung7_candidate.fk test_output_contains_77=1
PROXY
low_confidence=0 fallback=1 remote_ok=1 transport=host-exec-toybox-nc
CAPSTONE
local=6 build_test=7 fallback=8 prompts=local:The capital of France is|fallback:mork blenf
```

Observed on the Mac oracle:

```text
capstone-remote-oracle-request ('192.168.1.223', 60444) oracle mork blenf
```

## Honest floor

This closes the witnessed Rungs 1-9 ladder on the attached S23 for the named local and fallback prompts. The
direct `sock_request` carrier still needs follow-up: in this environment it did not reach the Mac listener, so
the witnessed proxy transport is `host-exec` plus Android `toybox nc`.
