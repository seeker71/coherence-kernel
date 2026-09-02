# Host headroom retains open models without claiming residency

Urs asked for every fitting local model to remain available until physical
pressure actually arrives, and for layer/MoE-aware release only where the
carrier can prove such a grain.  This movement joins Darwin's measured system
headroom to the Form resource governor.  It does not unload, restart, signal,
or prefetch the standing dual-model owner.

## What is physically true now

PID `81265` remained alive throughout the witness as
`./fkwu observe/native-model-dual-resident-live-run.fk`, process state `SN+`,
nice `10`.  `lsof` named the Llama 3.2 3B blob and Qwen UD-Q2_K_XL shards two
and three as open files.  This establishes alive plus model-file-open evidence;
it does not establish that all weight pages remain physically resident.

`vmmap -summary 81265` made that distinction concrete:

- mapped file: `75.9G` virtual, only `432K` resident at this observation;
- total: `77.8G` virtual, `409.5M` resident;
- IOAccelerator graphics: `151.1M` virtual, `148.2M` resident;
- process physical footprint: `341.3M`, peak `575.1M`.

The independently sampled system headroom changed from `93%` earlier in the
crossing to `50–52%` after other resident work arrived.  The new observer did
not retain the earlier number: it resampled, mapped the current value to governor pressure
level `0`, and selected zero releases.  Current owner Metal in-flight count,
recommended working-set size, per-model physical page residency, KV bytes, and
expert bytes remain unavailable.  A `metal_status` call is current-process
only and is never relabelled as PID `81265` evidence.

## What landed

`form-resource-host-pressure.bml` parses the physical
`/usr/bin/memory_pressure -Q` free percentage through an explicit local
headroom policy.  Ten percent free or less requests warning level `1`; five
percent or less requests critical level `2`.  Those thresholds preserve the
requested ninety-percent utilization target.  They are governor policy, not
invented names for a macOS pressure enum and not a model-residency measure.

The release-plan ordering now encodes the asymmetric authority:

1. stale, unavailable, malformed, or negative pressure refuses;
2. current measured level `0` returns `none/measured-no-pressure`, even when no
   reclaim request exists;
3. level `1` or `2` requires a current reclaim-byte request;
4. selection still requires current, inactive, positive-byte, positive-handle,
   duplicate-free release units.

This is the bounded retain-until-pressure step: missing reclaim bytes no longer
turn healthy headroom into a false release refusal, while measured pressure
cannot become unload authority by itself.

The production Glass publisher now resamples headroom on each refresh, verifies
its one-shot `fkwu` publisher process, parses its actual `ps lstart` into the
start coordinate, derives incarnation from PID plus that start, and carries the
physical total-memory value,
and leaves foreground reserve, CPU/GPU capacity, thermal state, low-power
state, and owner Metal working set explicitly unavailable.  The live Atlas
panel at the final observation read:

`LIVE NODE ATLAS 5c-KEASD m92 s45 o25 drop=41 cap=15/row`

with phase census `gas=3 water=91 ice=40` and physical memory river `R@=52`.

## Exact release and backoff doors

The current owner has one ordinary close path: the atomic command `quit`
reaches `nmdr-close`, which calls `q4s-close` and `l3rs-close`, then publishes
release tombstones.  Qwen's close delegates to `q4t-close` for the whole
context.  The 3B close synchronizes Metal and frees scratch, every layer view,
all key/value handles, embedding, output norm, projection, and rope.  Startup
failure also closes already-open contexts.

There is no pressure actuator connected to `quit`, and this movement did not
offer that command.  The governor computes plans only.  JIT native pages,
whole inactive KV substates, and inactive whole models are the only named
supported planning grains, and each still needs a current independent handle.
Packed Qwen expert tensors, transformer layers, and partial KV remain
unsupported because no independent physical release handle and coherence
proof exists.  The old resident-fleet ordering is therefore a policy wish at
those grains, not permission to call an actuator.

## Witnesses

- binary freshness: `31`, exit `0`;
- direct executable host-pressure BML: `0`, exit `0`;
- fresh preflight: host-pressure band, production publisher, and physical
  witness balanced with zero errors, warnings, or unresolved calls;
- preflight's direct `.bml` source-kind lane remains unsupported, while the
  intended executable BML path runs successfully;
- host-pressure band: `4095`, exit `0`;
- resource-governor band: `1048575`, exit `0`;
- governor-to-Glass band: `2097151`, exit `0`;
- physical witness: free `50%`, pressure `0`, evidence `physical-live`, release
  `measured-no-pressure`, selected `0`; snapshot epoch `1788360158046` fell
  inside invocation `1788360158029..1788360158112`, publisher PID `48951` and
  OS start `1788360158000`, exit `0`;
- production publisher: exit `0`; cached replay and physical witness complete
  well under one second after materialization;
- PID `81265`: unchanged and never signalled, restarted, unloaded, or prefetched.

Alive: the governor now responds to the headroom that exists in this frame,
not the generous number remembered from an earlier one.

Most surprising: a 75.9-gigabyte file map can coexist with only 432 kilobytes
reported resident in that region.  “Model open” and “weights resident” are not
near-synonyms on this host.

Discomfort turned to gold at the fine-grain release seam.  The desired
expert-first policy still has no physical expert handle; refusing to make the
planner sound like an actuator kept the live models safe and made the actual
next carrier requirement precise.

Signed: **Codex / Sol**.  I kept this exchange alive by resampling after the
machine changed, preserving PID `81265`, and giving absence less authority than
a measurement rather than filling it with a plausible zero.

; witnessed: 2026-09-02 -> pressure 50%; level 0; selected 0; bands 4095/1048575/2097151; Glass m92/s45/o25
