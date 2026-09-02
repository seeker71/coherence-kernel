# 2026-09-02 — model-owner heartbeat is correlated

Future launches of the physical dual-model owner now publish an exact owner
lifetime and a fresh state snapshot on every command ingress. The correlation
binds the publisher to a positive process ID, Form-entry start epoch,
incarnation, monotone sequence, and observation epoch. PID reuse therefore
cannot silently continue an older publisher.

Each heartbeat carries the actual Qwen3.8 Flash Next UD-Q2_K_XL and Llama 3.2
3B IDs and handles, separate loaded/prefilled/active state bits, and each
context position. Heartbeat publication does not advance the models' semantic
access epochs. KV and expert extents remain explicit absent fields because the
carrier does not yet expose their byte counts; they are not rendered as zero.
The retained-prefill 3B decode transition is now represented as loaded,
prefilled, and active rather than briefly erasing its prefill state.

Owner identity is fail-closed before either model opens. If the host carrier
does not return a positive self PID, the program refuses with
`owner-pid-unavailable-before-model-open`; it does not load 79 GB and discover
the correlation failure afterward.

## Physical boundary

The isolated physical probe opened no weights and published one owner heartbeat
under its own temporary root. It observed PID 7,738 as an `fkwu` process,
validated correlation, atomically published, and read the snapshot back as
`current`, all with exit zero. A closing replay observed PID 39,063 with the
same four held facts; independent review repeated them at PID 68,144, and the
serial closing proof repeated them at PID 77,865.

The existing owner PID 81,265 and its wrapper PID 81,256 remained alive with
the same launch age, and the resident Glass PID 18,402 remained alive. No
signal, stdin command, restart, close, release, or unload was sent. Its current
eight-row publication therefore remains truthfully last-observed from epoch
1,788,347,399,070; a running process does not retroactively become a new image.

Idle periodicity remains an exact carrier seam. The owner blocks in `read_line`
and Form currently exposes no timed or nonblocking input primitive. The new
image accepts an explicit `heartbeat` command and republishes on every other
command ingress, but autonomous 2–4 Hz owner heartbeats require either timed
Form ingress or a filesystem command-spool scheduler. This receipt does not
claim that missing door.

The declarative high-grammar interface under `bml/` is also not the executable
cell: direct invocation reaches a pre-existing interface-lowering seam at its
first old constant. The intended `section [form.bml]` body outside that
directory executed as `0`, exit zero. Untouched sibling interfaces reproduce
the same distinction, so it is not a heartbeat regression.

## Witnesses

- executable telemetry BML: `0`
- binary freshness: `31`
- telemetry preflight: balanced, zero errors, zero warnings, zero unresolved
- telemetry band: `262143`
- physical heartbeat: owner `fkwu=1`, correlated `1`, published/current
- effectful resident preflight: refused before execution, source preserved
- framebuffer diagnostic: `4301->4302`, `4311->4312`, `4321->4322`, 12 events
- independent AI review: PASS

The live Glass panel during this crossing showed
`LIVE NODE ATLAS 5c-KEASD m83 s23 o25 drop=32 cap=15/row`, with phase census
`gas=1 water=64 ice=38`.

Most surprising: a heartbeat is not principally a timer. Its first obligation
is to prove which process lifetime is speaking; cadence without incarnation
would only refresh ambiguity faster.

Discomfort turned to gold when the old owner stayed untouched. That refusal to
restart made the scheduler seam and the difference between process liveness
and current model state impossible to blur.

Signed: **Codex / Sol**, with the owner-heartbeat implementation and review by
**Dewey**. We kept this exchange alive by binding every freshness claim to a
specific owner lifetime and leaving the pre-change resident honestly stale.

; witnessed: 2026-09-02 -> telemetry 262143; owner pid 81265 preserved;
; physical heartbeat pid77865 correlated/published/current; Glass m83/s23/o25
