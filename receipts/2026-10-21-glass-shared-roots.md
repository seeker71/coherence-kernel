# Glass shared roots, 2026-10-21

Urs asked to finish shared node identity, ownership, lifetimes and retirement,
remove the serialization path, and observe the actual running Glass.

## Movement

`node_gift_write` now publishes a native owned generation in the shared field.
The frame holds eight reference words (64 bytes), not a recursive value image.
`node_gift_read` returns the actual shared root; `ggf-cell` selects its payload.
The full root ID contains the field incarnation and publication generation.
The incarnation journal survives field reset; reused storage never reuses its ID.
An index alone cannot resolve a reusable owned root. A current full ID can.

The named frame owns its current publication across an author's exit. The root
also records the author's PID and kernel start, distinct from frame ownership.
Explicit private observation scopes retire when their process lifetime ends.
Replacement retires the previous generation. Expected-root withdrawal cannot
withdraw a newer publication. Copied descriptors cannot acquire another frame's
ownership. Reader holds are tied to PID and kernel start; native GC traces held
descendants, framebuffer entries and transitive generation dependencies. A dead
reader loses its holds. Retired, unheld storage becomes reusable.

Publication, raw replacement and scoped withdrawal share the ownership lock
and interruption journal. An interrupted odd frame restores its prior reference;
scoped cleanup can retry withdrawal. The lock inode is stable and never unlinked.
Private heaps refuse native publication; there is no serialization fallback.
Field version 6 and `/fgn-` names separate this carrier from the old images.

The recursive `fk_cross_emit`/`fk_cross_decode` codec is removed. Glass callers
use the native root/payload API. Unused snapshot/control acknowledgement codecs
are removed. Model token-flow snapshots use the same typed native carrier; the
old disk snapshot encoder, decoder and candidate flow are removed.

The physical shared allocation, lock and host process-lifetime carrier remains
in the C seed. Its shrink path is the native field allocator/arena. Publication
validation, allowances, observation and care remain in Form. Pool capacity is
reserved graph storage, not a measurement of resident memory.

## Execution evidence

All commands below were invoked through `form-run`. Scratch probes and their
small child traces remain in this checkout's ignored `.hearth`; the existing
node-gift and membrane bands carry the reusable regression conditions.

- `./fkwu .hearth/glass-owned-lifetimes.bml`: 64-byte reference;
  held descendant `[descendant,2.5]` survived collection and replacement,
  including transitive retention; 506 storage reuses, zero new pools in the
  measured replacement window. Stale retirement and copied-frame ownership
  were refused. Framebuffer retention also survived collection.
- `./fkwu .hearth/glass-owned-fault-observe.bml`: writer PID 754 was observed
  building generation 39564, stopped and killed (exit 137). The next reader
  recovered the prior root `[63,0,2,39548]`, value `[prior,42]`, reference 64
  bytes. This physically exercises interruption during building, not every
  instruction of each journal phase.
- `./fkwu .hearth/glass-owned-death-observe.bml`: reader PID 2694, start
  1792542332177, held one generation before termination and zero after; its
  private frame retired. An independent held value remained valid.
- `./fkwu .hearth/glass-owned-flow-observe.bml`: typed root
  `[63,0,2,88553]`, projected root `[63,0,2,88554]`, 64-byte reference,
  one unchanged model-flow event, no disk snapshot created. Both test
  publications were retired afterward.
- `./fkwu .hearth/glass-owned-live-proof.bml`: actual production `organs`
  root `[63,0,2,111757]` was held in two independent native processes.
  Both reported that exact full ID and zero reader node mints. The publication
  belonged to `/fgn-d7d37ccb`, author PID 5577/start 1792542636872, observed
  age 776 ms. The proof frame retained the actual primary node, not its copied
  payload, and was withdrawn afterward.

The existing bands observed node-gift 4095, shared field 255, cell-store 255,
Glass wait 255 and telemetry membrane 2097151. These are verdict bitmasks,
not counts of tests. The node-gift checks include full-ID resolution, stale-ID
refusal, held descendants, compare-by-root retirement, dotted-tail promotion
and storage reuse. The membrane checks include held/one-shot malformed parity
and withdrawn roots classified as absent.

After the final seed build, `./fkwu .hearth/glass-owned-final-checks.bml`
observed all ten expected bitmasks, zero errors and no stopped result:
freshness 31; node-gift 4095; field, cell-store and Glass wait 255 each;
membrane 2097151; observer 67108863; dashboard 16777215;
sensor rows 2047; live 2147483647. `./fkwu .hearth/glass-preflight.bml`
checked fourteen source/closure units clean. Lifetime, dead-reader, typed-flow
and interrupted-writer probes were repeated on that seed and held again.
Transitive pinning and marking use worklists, so dependency chains do not
consume C stack per retained generation.

The final-source live identity read again preserved the same root in two
processes with zero mints, but its source was old because the viewing fleet
had ended during source reload. That is identity evidence, not a fresh-view
claim. The operator was asked to reopen the public Glass door; Terminal UI
automation was unavailable. The reopened view and publication result are
recorded in the subsequent live observation.

## Publication

`./fkwu observe/land-run.bml < .hearth/glass-owned-land-request.json`
retained its continuation after the rebase refreshed source mtimes: the witness
freshness verdict was 15, while its other nine checks passed. The exact failure
is retained in `.hearth/walk-io/land-witness.out` and the landing continuation
under `.form-heal/landing-800914fcee86e064770c9982b1315c680b188c9a7c2538d3767bbdb81e1bcdf3/`.
An honest `cc -O2 -o .hearth/fkwu-next runtime/fkwu-uni.c` build was published
as `fkwu`; the same continuation then reran its witness and all fifteen required
gates, including the complete form-cli build and BML cursor coverage:
`drift-gates pass=32767 full=32767 refused=0 names=-`.

The native landing published `519edaed54710937d2e24abad24f3448ce9623bf`.
`git ls-remote --heads origin main codex/glass-live-understanding` observed
that exact commit on both refs; the working tree was clean.

The final worklist implementation also ran
`./fkwu .hearth/glass-owned-chain-observe.bml`: an independent native reader
adopted a dependency chain of depth 256, held 257 generations, crossed a
collection and read the complete chain afterward. Its scoped publication was
withdrawn and both processes released their holds.

At the last view observation the keeper was ended and presentation/sensor
ages were stale. This is an outstanding viewer reopen, not a clean live-view
verdict. The pending operator question names the public command
`./fkwu observe/form-glass-run.fk`; a fresh publisher and presentation observation
must follow that event. Native identity, lifetime and recovery observations
above are complete independently of that viewing step.

## Reopened live observation

The operator reopened Glass. Keeper PID 54927 was alive; presentation age was
61 ms initially and 97 ms on a later observation. The current source frames
adopted owned roots without reader node mints. The later overview had 392
gauges, zero missing metrics, zero stale gauges and zero typed gaps. These are
point observations; the Glass sensor sometimes exceeds its declared cadence.

The actual `organs` publication was held in two independent processes as
`[63,0,2,153776]`, author PID 54914/start 1792560552189, age 743 ms. Both
readers minted zero nodes. A subsequent production replacement retired
`[63,0,2,163817]` for `[63,0,2,163853]`: the old ID stopped resolving, its held
root and descendant survived collection, and that reader's two holds returned
to zero after release and collection. No production frame was changed by the
observer. Independent lifetime, dead-reader, dependency-chain, typed-flow and
interrupted-building probes held again on the running seed.

One real obsolete owner remained: launchd's `earth.hati.day-turn`, PID 27518,
start 1792557001594, in the walk checkout. It rebuilt the executable on disk
but continued running the older admitted kernel. Its `qwen.coding.27518`
frame still contained a 2664-byte serialized image, correctly refused by the
native reader. No fallback decoder was added. The other live model owner,
PID 96934, and the Glass source publishers use current shared roots.

The owning native-turn boundary now refuses model admission when its executing
kernel lacks owned generations and retains the original options and window.
The day-turn manager can claim a dead owner's continuation under its existing
checkout lock, preserving its monotonic deadline through sync and admission.
The active continuation remains recoverable through a crash, safe start,
conflicted sync or memory interruption. An explicit checkpoint handoff keeps
the exact original request, model/entry settings, allowance, source/check
contract and pending normal publication/ledger path. Before admission, both
the current source and checkpoint must agree with that contract. Pre-renewal
token usage that was not retained remains unknown.

The actual owner's checkpoint was valid at repair, 18 of its 24 replies and
17 calls. Its exact request, current documents, writable set, checks, saved
state and original open window agreed in the private handoff preparation.
The owner is still running at this preparation observation; renewal and final
live verification follow below.

An inspection helper initially entered day-turn through its unconditional
top-level entry. Only that helper (PID 78979) was terminated, before model
admission. Its set-aside patch was restored and its own lock released through
the existing owner check. Day-turn imports are now effect-free. Both source
closures check clean; the native-turn green band reads 511 and host-walk reads
8796093022207. The renewal check preserves the exact original deadline and
refuses an unavailable generation resource.

## Owner renewal and final native overview

The renewal repair landed as `bb2b842d0ab725f2961b414b51a9885a5a8595bc`.
Its current required release checks, including the complete sealed-source CLI
build, passed `pass=63 full=63 refused=0`; both remote refs agreed afterward.

Before changing the real owner, the configured launchd job, executable, cwd,
PID and kernel start were checked. It was paused in its private GPU phase,
with no candidate publication marker. The source and latest checkpoint were
revalidated while paused: repair, 20 of 24 replies, 19 calls. The exact request,
checkpoint, owning gap, checks, original deadline and pending normal
publication/ledger path were retained in the walk's private continuation.
Pre-renewal generated/injected usage was not retained and remains unknown.

PID 27518 then ended. Its still-matching old frame `/fgn-60575efc`, sequence
560, 2664 bytes, was withdrawn only after confirmed death and matching raw
length/digest/sequence. Sequence 562 carried zero bytes and read absent.
No obsolete snapshot was decoded. The existing checkout lock was taken from
the dead owner, source/binary freshness checked, and the configured launchd
job restarted.

Replacement PID 21239, kernel start 1792563059056, executes field version 6
and resumed the same checkpoint at reply 20/call 19. Its original monotonic
deadline remains 1637109067; neither the reply allowance nor that window was
extended. Its actual prefill root `[63,0,2,314878]` was read in two independent
processes with that exact ID and zero reader node mints. Its descriptor was
64 bytes and origin matched the current owner lifetime. The observation frame
retained the actual model root and was withdrawn after the second reader.
The real coding task continues in its native owned pipeline.

The overview now projects this native prefill source into the existing model
owner section, including last slice phase, settled positions and positions in
flight. It binds the root to the current admission, exact host start and
kernel start, verifies mapping release, and retains the actual source NodeID
in the metric. The source timestamp is preserved through owner-frame
publication: it is an event, not a heartbeat or a prompt percentage.

The live overview proof observed `session-owner.21239.prefill`, 4705 settled
positions and 64 in flight, linked to `[63,0,2,343039]` from
`[21239,1792563059056]`. Its original source epoch 1792563918670 remained older
than the owner-frame epoch 1792563919954; the rendered row showed source age
1 s. Later readings advanced to 5292 settled positions. Keeper 35015 stayed
alive across readings, presentation age 127 ms, with zero missing gauges,
stale gauges or typed gaps in that sample. Source changes can renew the
viewing fleet; the open supervisor owns that renewal.

After the fleet renewed, actual `organs` root `[63,0,2,308251]`, origin
`[21028,1792563049717]`, again matched across two independent readers with
zero mints. Its replacement `[63,0,2,308294]` made the old ID unresolvable
while the held root and descendant survived collection. The observer's two
holds returned to zero after release and collection.

The unused sensor text encoder/parser had only its old band as a caller.
Those definitions and their field-count authority are removed. The existing
sensor band now observes the real 64-byte native frame, including source-node
identity and original timestamp, and retains verdict 2047. Standalone runtime
holds now use PID/start/task-scoped continuations; only the owning day-turn
can write the manager continuation. Their paths have one home in `hearth.bml`.

The final native witness checks freshness 31, renewal closures and scope 0,
native-turn green 511, sensor rows 2047, observer 67108863, and fourteen Glass
source closures clean. These are observed values/bitmasks, not test counts.
The public final landing is invoked through `observe/land-run.bml` with
`.hearth/glass-final-visible-land.json`; its native continuation retains the
current gate and publication results.
