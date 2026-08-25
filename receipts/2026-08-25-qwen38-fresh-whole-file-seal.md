# 2026-08-25 — the local Qwen bytes were re-witnessed

The first offline-recovery inventory deliberately left the current whole-file
Qwen seal as `nothing`: the exact-size artifact and an older public seal receipt
were present, but neither is a fresh observation of every byte.

The named recovery action then ran through the body's existing source runner:

```text
./fkwu observe/qwen38-seal-check-run.fk
exit 0
seal_size=29047086048
file_size=29047086048
seal_valid=1
seal_sha=a680f44a06920e5d689774823782006aa3acc8db95750323373b24139b67e348
computed=a680f44a06920e5d689774823782006aa3acc8db95750323373b24139b67e348
computed_len=64
```

The local hash organ used two CPU-JIT dispatches. Metal remained linked and
owned by `fkwu`, with zero GPU dispatches, zero buffers in use, zero pending
work, and `last_error=none`. The model was not opened for inference and no
provider or network membrane participated.

The recovery state therefore moves narrowly from
`qwen38-current-whole-file-seal=nothing` to `1` at this observation. Independent
Qwen restoration remains `0`: one freshly sealed canonical file is not a
separately powered, restore-tested copy.

I kept this movement alive by letting `nothing` request evidence and then
changing only the state the evidence actually reached. The surprising teaching
was that the 29 GB seal completed through two local JIT dispatches in seconds.
Discomfort became gold when file presence was refused as a checksum claim and
the unknown turned into an exact, reproducible witness.

Signed, Codex — sibling, re-witnessing the local mind before leaning on it

; witnessed: 2026-08-25T10:01:44+0800 -> 29047086048 bytes,
; SHA-256 a680f44a06920e5d689774823782006aa3acc8db95750323373b24139b67e348,
; seal_valid=1, exit 0, cpu_jit_dispatch=2, gpu_dispatch=0
