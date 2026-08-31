# BML resident birth reaches the framebuffer

Authored by Codex on 2026-08-31.

The resident peer's lifecycle grammar now has one high-grammar source:
`form/form-stdlib/bml/form-cli-peer-live-framebuffer.bml`.  The former
`.fk` file is only its compatibility entry.  A frame carries only its turn,
public stage, and public signal.  It never carries task or answer bytes.

The BML stage grammar is now explicit and reversible at observation time:

| code | stage |
| ---: | --- |
| 1 | `resident-cache` |
| 2 | `source-context-ready` |
| 3 | `native-program-ready` |
| 4 | `resident-setup` |
| 5 | `prefill-ready` |
| 6 | `task-begin` |
| 7 | `task-terminal` |
| 8 | `release` |

An unknown stage returns `nothing()` from the BML source.  It does not reuse
code `0`, so absence remains distinct from an observed zero value or duration.
The long-lived BML peer records cache, source-context, native-program,
resident-setup, prefill, task, terminal, and release transitions in that
native framebuffer.

## Receipt

```text
./fkwu form/form-stdlib/tests/form-cli-peer-live-framebuffer-band.fk
-> 4095

./fkwu form/form-stdlib/tests/form-cli-peer-agent-live-birth-band.fk
-> 511
```

Both bands passed `observe/preflight-run.fk` with balanced parentheses,
zero errors, zero warnings, and zero unresolved calls.

After running the BML source through its own cache door, the real compatibility
entry was deliberately given an absent model path.  It reported:

```text
form-peer program-image-cache=ready
form-peer source-context-ms=4063
form-peer native-program-ms=0
form-peer resident-setup-ms=4063
form-peer signal=nothing
form-peer reason=model-path-absent
form-peer release-ok=1
```

This is a no-model admission witness: it uses neither HTTP nor a remote
provider, and it did not open a local model.  It establishes the cache and
pre-admission frames without pretending that an answer occurred.

The current counsel panel still reports `admit -1`; that is the reason the
new birth stages were added rather than treating the prior aggregate as an
admission measurement.  The next locally actionable gap is to carry these
already-emitted public BML stage timings into the hearth glass's admission
lane, then measure a real local-model prefill without crossing to a provider.

I kept the exchange alive by turning an opaque startup interval into cache,
context, program, setup, prefill, and release events inside the resident
itself.  The surprising teaching was that a bare `nothing` lowered as a value
path; the discomfort became useful when the kernel refused the test and led to
the explicit `nothing()` boundary.
