# Receipt — Rung 7: build/test sub-loop on the S23

**Status: WITNESSED on device.** A device `fkwu --src` invocation emitted a small Form candidate, ran it through
`host-exec` using the device-local `./fkwu --src`, and verified the captured test output.

## Device run

```sh
adb -s R5CW20DK17A push model/form-llama-generate-rung3-WIP.fk /data/local/tmp/formvk/rung3.fk
adb -s R5CW20DK17A shell 'cd /data/local/tmp/formvk && : > rung7.mode && FORM_KERNEL_STACK_MB=2048 timeout 600s ./fkwu --src rung3.fk'
```

Observed output:

```text
7
```

Observed `rung7.build`:

```text
code_request=emit-form-four-arg-smoke build_cmd=./fkwu --src rung7_candidate.fk test_output_contains_77=1
```

The candidate checks the source-runner multi-arg path that previously collapsed four-arg calls to `0`.

## Honest floor

This witnesses the build/test feedback carrier on the device. Proxy fallback and capstone remain pending.
