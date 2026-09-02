# BML demand JIT closes the scalar cache, not model parity

Date: 2026-09-02
Scope: caller-held lowerable scalar graphs; Qwen parity observation

## What physically moved

`bml-demand-jit.bml` now owns one first-answer-safe specialization protocol.
A BML caller offers a scalar graph as ordinary recipe data. The first request
is answered immediately by the Form evaluator and leaves one candidate
pending; it never waits for a cache or makes acceleration a readiness gate.
One later step performs one bounded cache read, loads or compiles one image,
challenges the native answer against the Form answer, publishes through a
leased lock-directory candidate rename, and installs the page with
`jit_leaf_inram`. The retained registry makes later calls in the same process
reach the same page.

The effectful band returned `16777215`. A clean live root then produced:

```text
process 1: first-frame-ms=0 route=form cache=not-inspected
           specialization-ms=4 status=compiled-published-installed
           same-process route=native born=0
process 2: first-frame-ms=0 route=form cache=not-inspected
           specialization-ms=0 status=disk-installed
           same-process route=native born=0
```

Missing source, missing cache, stale mtime, stale epoch, stale program, corrupt
schema/bytes/image fold, publish failure, native disagreement, and the wider
lowering frontier all have distinct statuses. A source-missing request can
still install its caller-held graph in RAM while withholding disk publication.
Every step measures its own duration against the 5,000 ms attention boundary.
The v3 disk record carries `jonb-flat`'s exact length-delimited integer
structure, root, epoch, and a runtime fingerprint covering the ARM64 leaf ABI
plus the current `fkwu`, emitter, and BML authority size/mtime. A runtime or
emitter change therefore expires the old page. Cache reads are capped at 1 MiB
before parsing. The band also holds a live writer lease, heals a stale lease,
refuses an oversized cache without reading it, and replaces current ice with a
different valid program between request and step. The single-read inspection
classifies that image `stale-program` and compiles the pending identity instead
of executing mismatched bytes.

The Glass publisher projects the real movement as four related nodes: first
answer gas, specialization water, disk image ice, and resident CPU page water.
After publication the live Atlas showed `m92 s27 o25 drop=41`, with
`gas=3 water=73 ice=40`, at 2 Hz. The four JIT nodes appeared together as the
fresh `e/r/C/c` sequence; no model owner was restarted or unloaded.

## Identity boundary

No NodeID or external registry was added. RAM identity reuses
`jonb-identity` and the disk membrane stores its exact length-delimited
structure. The cache path and image checksum are validation lenses, never
identities. Operator/type capacity belongs in the shared type abstraction,
not in this cache protocol.

## Five-second attention signal

The model-free Flash-Next Metal fixtures were physically reobserved without
mapping the 78.9 GB model. With stale Form images, the quant and combined graph
runs took 5.43 s and 6.17 s, crossing the attention boundary. Those runs
refreshed their images. Immediate reuse took 0.13 s and 0.41 s, returning live
verdicts `127` and `511`; the combined graph compiled 90 pipelines and executed
six dispatches with zero buffers left resident.

The first syntax and dependency failures, the over-5-second pair, and the
clean re-observation were correlated through eight framebuffer events in
`bml-demand-jit-diagnostic-run.fk`. Final preflight for the authority band,
parity authority, diagnostic, and live publisher was balanced with zero errors,
warnings, and unresolved calls.

## Performance claim remains failed

This movement changes JIT readiness and reuse, not Qwen end-to-end scheduling.
The prior same-host, exact-artifact physical decode window remains the only
strictly comparable full-model pair:

| lane | Form | llama.cpp boundary | 90% floor | result |
| --- | ---: | ---: | ---: | --- |
| decode, 64 tokens | 12.188 tok/s | 26.580 tok/s | 23.922 tok/s | fail by 11.734 tok/s |
| prefill | 12.340 tok/s over 64 | 318.830 tok/s over 5,632 | 286.947 tok/s | threshold failed; sample lengths differ |

The user's M5 Max figures are classified as incomparable here because host,
context depth, window size, and speculative decode differ. They remain useful
targets, not measurements this M4 run may appropriate.

A second full 78.9 GB context was withheld because this lane made no full-model
schedule change and the resident owner was alive, so another run could not
answer a new performance question. The snapshots are kept distinct: the free
page pool was about 1,391 MiB, swap use was about 3,223 MiB of 4,096 MiB, while
`memory_pressure -Q` reported 93% system-wide memory available. None is renamed
as another. Model-free fixture success cannot be promoted into token/s parity.
The exact remaining performance work is full-model schedule fusion for the
1,879 decode dispatches/token and token-batched prefill, followed by a quiet,
same-size, same-context A/B run. Strings, lists, cross-calls, and arbitrary
whole-program in-process image installation are also still outside the proven
scalar lowering frontier.

## Closing

I kept the movement alive by letting the first Form answer exist before asking
ice to form around it. The surprising teaching was that the existing resident
NodeID page already had both halves of cross-process reuse; only the typed disk
image and staged admission were missing. Discomfort became gold when the two
over-five-second fixture runs separated stale installation from 130/410 ms
reuse, while the full-model parity failure stayed untouched and honestly red.

— Codex

; witnessed: 2026-09-02 -> bml-demand-jit-band 16777215;
; qwen4exp-flash-next-parity-observation-band 4095;
; Glass m92/s27/o25/drop41, gas3/water73/ice40
