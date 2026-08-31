# Contribution resident birth becomes BML-owned observation

Authored by Codex on 2026-08-31.

`form/form-stdlib/bml/form-cli-peer-contribution-live-birth.bml` is now the
high-grammar authority for the durable contribution resident's observable
birth packet.  It names the existing entry source, derives only the entry
cache state, and holds these measured fields:

```text
cache-state | source-context-ms | native-program-ms | resident-setup-ms
```

The existing `observe/form-cli-peer-contribution-live.fk` remains the effect
door because it owns the long-lived spool, bell, model lease, and release.
It now asks the BML cell to emit birth, prefill, and release framebuffer
frames and the corresponding public lines.  No new model, HTTP service, or
sidecar is added.

`native-program-ms` is deliberately the elapsed time while the resident
constructs its caller-owned transport/program route.  It is not a claim that
all work in that interval is JIT compilation.  `source-context-ms` is
`nothing()` when the recovered state has no source context; an observed
duration of `0` remains `0`.

## Receipt

```text
./fkwu form/form-stdlib/bml/form-cli-peer-contribution-live-birth.bml
-> 0

./fkwu form/form-stdlib/tests/form-cli-peer-contribution-live-birth-band.fk
-> 255
```

The focused band proves the typed packet, the exact entry/cache coordinates,
all six lifecycle stage codes, six emitted events, and the distinct `0` and
`nothing()` cases.  Its preflight reports balanced parentheses, zero errors,
zero warnings, and zero unresolved calls.

The generic preflight reader currently reports BML as an unsupported source
kind, so the direct BML compiler is the compile witness for this source.  It
also cannot preflight a permanent resident entry: that entry correctly waits
for its ten caller-owned input lines.  A model-absent smoke attempt therefore
entered the BML-cache compiler and was terminated before model admission; it
did not touch the active resident.  This is a tooling boundary, not a claimed
resident result.

The active local Qwen resident remains PID `36364` with its older loaded
image.  It was not restarted or released.  These BML events become live only
at the next orderly successor birth, after that current lease ends.

The self-watch counsel panel is still `admit nothing / unobserved`; every
currently judged performance lane reads `0`.  That is why this receipt makes
no prefill-speed claim before the successor produces a real BML birth frame.

I kept the exchange alive by placing the new observation in BML and attaching
it to the already-owned resident door, rather than creating another watcher
or process.  The surprising teaching is that a useful speed field must name
the precise interval it measured.  The discomfort around an unobserved birth
became a clean successor boundary instead of a fabricated live trace.
