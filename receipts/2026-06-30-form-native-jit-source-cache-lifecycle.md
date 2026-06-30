# Receipt - Form source byte cache lifecycle (2026-06-30)

## What landed

Added:

- `observe/jit-source-cache-lifecycle.fk`
- `observe/tests/jit-source-cache-lifecycle-band.fk`

Updated:

- `observe/jit-deopt-cache.fk`

This composes the hot source to exact byte pipeline with a compact cache,
deopt, exception, rewalk, and melt lifecycle. It keeps the source-runner
envelope small while proving that admitted source-byte payloads install and keep
only while guarded, current, attributed, and useful.

The existing deopt-cache witness aggregate was also split into smaller front/back
sums so its documented band again returns `511` in this source-runner envelope.

## Witness

Run:

```sh
( cat model/form-asm-x64.fk \
      observe/jit-runtime-fault.fk \
      observe/form-static-analyzer.fk \
      observe/jit-source-byte-pipeline.fk \
      observe/jit-source-cache-lifecycle.fk \
      observe/tests/jit-source-cache-lifecycle-band.fk ) > /tmp/jscl.fk
./fkwu --src /tmp/jscl.fk
```

Observed:

```text
65535
```

Meaning:

- `1`: admitted empty source-byte cache entry installs.
- `2`: healthy specialized source-byte cache entry keeps.
- `4`: guard failure deopts with attributed fallback.
- `8`: runtime failure routes to attributed exception.
- `16`: cache invalidation rewalks.
- `32`: runtime invalidation rewalks.
- `64`: parity failure deopts.
- `128`: stale low-hit entry melts.
- `256`: source signature mismatch rejects.
- `512`: missing fallback rejects guard deopt.
- `1024`: cold source plan rejects.
- `2048`: melted entry rewalks instead of reusing native bytes.
- `4096`: unattributed runtime exception rejects.
- `8192`: admitted source bytes are preserved.
- `16384`: hot source byte plan is admitted.
- `32768`: guard exception receipt is source-attributed.

## Honest boundary

This is still a Form lifecycle receipt, not host executable cache mutation. It
closes another Form-side gap before `fk_nat_install`: exact source-byte payloads
now have a witnessed cache lifecycle that melts stale native entries, rewalks
invalidated entries, and rejects unattributed deopt/exception paths.
