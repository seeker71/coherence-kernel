# Two open models are not yet two proven routes

The current physical owner is PID `81265`, running
`./fkwu observe/native-model-dual-resident-live-run.fk` for nine hours with
state `SN+` and nice level `10`.  `lsof` still names the 2,019,377,376-byte
Llama 3.2 3B blob and Qwen UD-Q2_K_XL shards two and three at
49,979,779,296 and 28,878,402,944 bytes.  That proves one live process has both
model files open.  It does not prove both model routes answer now.

The memory picture makes the distinction unavoidable.  `vmmap -summary`
reported a 75.9 GB mapped-file region with only 432 KB resident, 148.2 MB
resident in IOAccelerator graphics, and a 341.3 MB physical footprint.  The
system headroom observer independently reported 45% free and pressure level
zero.  Open virtual mappings and positive carrier handles are not physical
weight-page residency.

## What the live owner can and cannot prove

The last in-process model publication is
`models.dual-resident.i1788332071161` at epoch `1788347399070`.  It records
Qwen handle `1262` and 3B handle `1695`, both with last-observed lifecycle
`ready`.  It contains no `owner-heartbeat` and no loaded, prefilled, or active
state rows.  The separate current OS observer proves PID `81265` alive, but its
binding is exactly `unbound-last-observed`.

The source now contains atomic `qwen-step` and `3b-step` command ingress, but
its mtime is `1788358055000`, later than the running owner's Form-entry epoch
`1788332071161`.  This process therefore predates that command-capable image.
Offering it a step would leave an unconsumed command rather than measure a
model.  Starting a second full Qwen owner would duplicate a 79 GB graph while
the first owner remains attached to TTY010.  Neither action was taken.

The physical readiness reader consequently returned:

```text
target-pid=81265 process-alive=1 owner-binding=unbound-last-observed
snapshot-age-ms=17542255 source-newer-than-owner=1
qwen-last-handle=1262 qwen-route=unavailable(owner-snapshot-stale)
threeb-last-handle=1695 threeb-route=unavailable(owner-snapshot-stale)
parallel-ready=unavailable(one-or-more-routes-unavailable)
```

So the exact answer is: both contexts were opened and successfully used during
the original startup, and both handles remain last-observed in the same live
process; current independent callability of either route, and therefore their
parallel availability, is not proven.  Simultaneous Metal execution is not the
intended scheduler shape in any case: `native-model-glass.bml` admits multiple
loaded/prefilled models but enforces at most one active dependent route stage.

## The typed unblock

`native-model-route-readiness.bml` now makes route readiness stronger than
mapping or liveness.  A measured route answer requires all of these in one
bounded window:

1. a valid owner snapshot no more than one second old or ahead;
2. a current physical owner process bound to that exact snapshot;
3. an owner heartbeat whose access epoch equals the snapshot epoch;
4. a positive model handle;
5. explicit loaded and prefilled state rows carrying that same handle.

If fresh evidence explicitly says loaded or prefilled is false, readiness is a
measured zero.  Stale, unbound, heartbeat-free, missing, malformed, or
handle-inconsistent evidence is typed unavailable instead.  Last-observed
handles remain inspectable in separate fields without being upgraded.
`models.parallel-ready` becomes measured one only when both independent model
facts are measured one; its reason explicitly says sequential activation, not
simultaneous dispatch.

Integrated review tightened that fact to schema v2: every unavailable route
now carries one exact missing observation door separately from its reason
(owner heartbeat, owner binding, process liveness, state/handle correlation,
or the joined Qwen+3B route).  The live wrapper also carries PID, liveness,
binding, owner start, source mtime, and source-newer comparison as explicit
present/absent fields, so an undiscovered owner cannot fall back to an
ambiguous numeric zero.

## Routing that exists in source

The Form-native state machine currently routes known Form and grammar work to
the native world evaluator, unknown NL-to-Form proposal work to Llama 3.2 3B,
and only explicit native inability to Qwen3.8 Flash Next.  The owner source
opens and prefills Qwen, opens and prefills 3B while retaining Qwen, then
returns to an append-only Qwen evaluation while 3B remains open.  Those are
real physical startup steps from the older run, not a current request receipt.

## Remaining carrier seams

- There is no safe live-image adoption door that transfers the two open
  contexts from the old owner into the newer command-capable owner.
- No correlated per-command acknowledgement records model id, handle, input
  position, output position, start, finish, and result; stdout from a step is
  not yet a durable route receipt.
- Per-model resident weight pages, KV bytes, packed-expert bytes, and the
  owner's Metal working set remain unavailable.
- The scheduler intentionally serializes dependent model stages; concurrent
  Qwen and 3B Metal dispatch has neither an interface nor a proof.
- A real current `qwen-step` followed by `3b-step` remains owed after an
  authorized owner handoff or restart.  No full-model performance number moved.

## Witnesses

- route-readiness band: `8191`;
- native dual telemetry band: `524287`;
- owner cadence band: `262143`;
- native model routing/Glass band: `16383`;
- resident fleet band: `8191`;
- high-authoring band: `4095`;
- band and live-reader preflights: balanced, zero errors, warnings, or
  unresolved calls;
- live readiness reader: exit zero in 0.2–0.7 seconds;
- `git diff --check`: clean.

The current Glass panel at `16:00:54.559Z` read
`m92 s42 o25 drop=0 cap=39/row`, with phase census
`gas=3 water=92 ice=35` and resource river `R@=54`.

Alive: the two handles remain visible without being promoted into two answers.
The most surprising teaching was that a process can truthfully be alive for
nine hours while its model-readiness voice is nearly five hours old.  The
discomfort was declining the tempting one-byte `qwen-step` offer.  It turned
to gold when the source/image age comparison showed that the byte would not be
a request at all—it would be an orphan—and the new typed door made that refusal
reproducible instead of intuitive.

Signed: Codex

; witnessed: 2026-09-03 -> PID81265 alive; handles1262/1695 last-observed;
; routes unavailable; readiness8191; Glass m92/s42/o25/drop0
