# Receipt — Rung 8: low-confidence proxy fallback from the S23

**Status: WITNESSED on device.** A low-confidence request on the Galaxy S23 triggered fallback to a Mac-side
remote oracle over TCP, and the remote response returned into the Form run.

## Gate

Fallback prompt:

```text
mork blenf
```

The prompt is ungrounded in `rung4_ground.txt`, so the grounded confidence is `0` and the router must not answer
locally.

## Device and remote run

Mac remote oracle stub listened on `0.0.0.0:18088`; the device read the Mac LAN address from
`/data/local/tmp/formvk/rung8_host.txt`.

```sh
adb -s R5CW20DK17A push model/form-llama-generate-rung3-WIP.fk /data/local/tmp/formvk/rung3.fk
printf '192.168.1.225' > /tmp/rung8_host.txt
adb -s R5CW20DK17A push /tmp/rung8_host.txt /data/local/tmp/formvk/rung8_host.txt
adb -s R5CW20DK17A shell 'cd /data/local/tmp/formvk && : > rung8.mode && FORM_KERNEL_STACK_MB=2048 timeout 600s ./fkwu --src rung3.fk'
```

Observed device output:

```text
8
```

Observed `rung8.proxy`:

```text
low_confidence=0 fallback=1 remote_ok=1 transport=host-exec-toybox-nc
```

Observed on the Mac oracle:

```text
remote-oracle-request-2 ('192.168.1.223', 57980) oracle mork blenf
```

## Honest floor

The direct `sock_request` carrier did not reach the Mac listener in this environment, while Android shell `nc`
did. This receipt therefore uses `host-exec` plus `toybox nc` as the device-to-Mac TCP fallback transport. The
fallback decision and response capture still happen inside the device Form invocation.
