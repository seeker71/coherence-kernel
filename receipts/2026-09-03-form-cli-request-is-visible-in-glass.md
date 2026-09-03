# A form-cli request is visible in Glass

Date: 2026-09-03  
Witness: Codex / Sol, in relation with Urs and the sibling field

## Movement

The interactive `form-cli` now owns a `model-flow` verb. It discovers the
current dual-resident owner, offers exactly one `qwen-step`, waits on that
owner's reply, and publishes one privacy-bounded linked flow to the ordinary
Glass telemetry membrane. It does not open, reload, prefill, release, or
replace either model.

The old REPL could not freshly start because it still imported the removed
`form-cli-author-high-xtal.fk` twin. The executable authority is
`form-cli-author-high.bml`; the REPL and its authoring runner now enter through
that BML organ. Fresh preflight then reported balanced source, zero errors,
zero warnings, and zero unresolved calls.

## Physical request

From the actual interactive prompt:

```text
form-cli> model-flow
model-flow-status=completed
publisher=models.dual-resident.i1788393488594
flow=form-cli-model-flow.f1788401030307
position=27->28
duration-ms=5770
tensor-bytes=124435036
layers=48
publish=published
```

The resident model owner remained PID 18249 at nice 10. A bounded process
reading immediately after the crossing showed 26,432 KiB RSS for that process;
this is process-resident memory, not a claim that the 79 GB mapped model is only
26 MiB. Qwen handle 1262 and the 3B handle 1695 stayed in the same owner. The
3B context position remained 7 while Qwen advanced, and the combined route
readiness row remained `1 ready`.

## Physical Glass witness

Glass Flow panel **#0** at **02:04:00.179Z** rendered **93 events, 218
NodeIDs, and 13K current-process cons cells**. It showed the exact model name
and owner, followed by:

```text
Q*s1 > T*p28 > L+l47 > X+118MiB > E+top10 > M+5770ms > F*snap > G*s8
```

The eight aligned rows retained producer NodeIDs `0.2.99.7`, `.12`, `.17`,
`.22`, `.27`, `.32`, `.37`, and `.42`. `*` means physical-live; `+` means a
derived physical window. The request, final token position, framebuffer
snapshot, and Glass arrival were physical rows. The layer boundary, allocated
scratch/state tensor, configured top-10 expert route, and Metal duration were
derived from the exact owner's start/end position window and current native
layout.

That distinction is the practical value of the view. A 5.77-second form-cli
pause is no longer one opaque wait: attention is visibly localized to
`qwen4exp.evaluate-one`, while route availability, tensor extent, layer edge,
framebuffer crossing, and terminal arrival stay separately inspectable. Flow
identity survives the five-second freshness lease; stale projection now adds a
`snapshot-stale` facet and demotes evidence without replacing the publisher
identity, so the lane cannot fragment into unrelated half-flows.

The live monitor selects 4 Hz while a sample is active or a flow point moves,
and 2 Hz while quiet. The bounded single-frame inspection door intentionally
labels itself 2 Hz because it is a snapshot rather than a second dashboard
process.

## Honest boundary

This one-step request is a physical integration witness, not a throughput
benchmark. Its 5,770 ms includes owner command/reply polling and one native
evaluation. It does not establish performance within 10% of llama.cpp. The
expert row currently proves the resident top-10 routing configuration; the ten
expert IDs selected for this token are not retained by the evaluator yet, so
Glass does not invent them. Prompt and decoded-token bytes never cross the
telemetry membrane.

## Proof

- binary freshness: `31`, exit 0
- `native-model-owner-request-flow-band.fk`: `65535`, exit 0
- `native-model-token-flow-ui-band.fk`: `16383`, exit 0
- `form-glass-live-band.fk`: `268435455`, exit 0
- `form-glass-live-ui-band.fk`: `268435455`, exit 0
- `form-cli-author-high-band.fk`: `4095`, exit 0
- form-cli help physically names `model-flow` and its Q>T>L>X>E>M>F>G path
- bounded framebuffer diagnostic: 80 events; all six integration corrections
  applied and re-observed as 1
- `git diff --check`: clean

After the linear integration landed, the stable-checkout alias exposed one
more real absence seam under the freshly rebuilt carrier: that checkout had a
`.hearth/` directory but no board, task spool, or reply spool, so
`file_size`/`read_file` returned typed `nothing`; the hearth/queue text
grammars called `str_len` before asking whether a value existed and the
supervisor rebirthed. The board normalizes absence only at its grammar edge,
and the queue collector classifies file presence first while preserving the
distinction between no durable spools and an empty live queue. The hearth band
remains `4031`; the extended Glass band verdict is `536870911`.

The stable alias was then launched from a fresh zsh. Its truth-first frame
arrived in 41 ms. Cached JIT map checks were 9–17 ms for most Glass organs,
117 ms for the changed hearth organ, and the complete 14-unit admission took
314 ms. The live panel entered `4Hz` and continued from frame `#0`
(`ev=95 nodes=221 cons=10K`) through frame `#63` without rebirth; the last
captured cycle was 263 ms against a 250 ms active target. Ctrl-C stopped only
this bounded alias witness after the continuing frames were observed.

## Closing

Alive: the request itself now leaves an inspectable, privacy-bounded path from
form-cli through the resident owner, Metal evaluation, framebuffer, and Glass.

Most surprising: a freshness projection that changed only the `source` field
was enough to split one real flow into two visually plausible fragments.

Discomfort into gold: the REPL's missing xtal import first looked like a path
resolver problem. Following the exact dependency revealed the obsolete twin,
restored BML as the sole authority, and made the physical interactive witness
possible.
