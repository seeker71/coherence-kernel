**REVISE**

The direction is right: Form should own interpretation, publication, and choice; C should keep ABI carriers; leases must outlive policy changes; hosted work must not be sold as a kernel. The packet as written would still let a merge land the wrong ownership cut, a disconnected lease model, and an OS map that reads like implementation. Fix those couplings, then this can be one large coherent movement.

## Aspiration / evidence / inference

**Evidence (treat as measured, not as proof of a finished runtime):**

- Lexical C census at `a31a89c2` (649 functions, 374 global names, 395 declarations, 58 local-static units, zero unresolved top-level units). Census ≠ correctness.
- Last movement grew the seed by 301 lines; reduction is owed.
- Form already emits the bootstrap table and CLI C; parsing, evaluator, executable allocation, host dispatch, and process-global contexts still live in C.
- Metal is `dlopen`/`dlsym` once per process; root/CLI link only libSystem; `FKWU_METAL_CARRIER` is process-lifetime adapter selection, not live adapter replacement.
- GPU program A/B is real: live `4095`, archive capture/reuse `63`/`63`, refusal `127`, choice `2047`. Archive reuse still re-processes MSL into a library.
- Buffer handles cap at 65535 slots; pipelines live until process exit; read/write quiesces all adapter work; timeout retains ownership; live work blocks release.
- Child supervision already has argv, private dirs, retained stdio, returncode, monotonic deadlines, correlated stop, group cleanup.
- `frame-luma.bml` exists and `frame-luma-band` is 127. That is a functional witness, not a bandwidth witness.

**Inference (likely, not measured here):**

- A Form interpreter pixel fold on a >1 MiB BMP is slower than the C island, and may copy if pixels are not kept as a span.
- “Growable C loader last movement” was scaffolding for that comparison, not a reason to keep a second decoder.

**Aspiration (do not merge as if done):**

- Maximum native hardware integration, complete Form-native OS, hot-swappable adapters, per-buffer overlap, unbounded tables, native-speed decode without a specialized CPU image.

## Located objections

**1. Item 1 mis-states the carrier cut.**
C should keep `open` / `map` / `read` into an owned span. Form should own BMP layout, overflow checks, and the luma fold. “The body should now own file reads and pixel interpretation” collapses those. Moving POSIX reads into the interpreter fights low crossing cost and the north star’s carrier list. Retire C BMP *interpretation* and the synthetic sensing stream; do not reimplement file I/O in Form.

**2. Deleting the C decoder without a span + specialization rule fights the user constraints.**
Form-owned meaning does **not** imply “interpreter is the native floor.” If the Form path walks nested lists, retirement raises copy cost while claiming a smaller seed. Forbidden: keep C as a silent fallback. Allowed: measure C vs Form on one captured fixture, then either (a) admit a Form-generated CPU-native fold as an explicit selectable version, or (b) retire C with the interpreter as the only path **and** record the gap as the new floor. Do not describe (b) as maximum-bandwidth native execution.

**3. Comparative witness is C vs Form, then two Form policies — not the same experiment.**
Item 2 is the retirement gate. Item 3 (two threshold policies, one captured byte string) is live A/B after C is gone. Do not use policy A/B as a substitute for the C-vs-Form timing/layout witness. After tags 213/214 are tombstoned, C cannot remain a selectable primitive.

**4. Item 5 is not mergeable as “executable async lease” if only a pure state model lands.**
The proposed fields are the right contract. A BML machine that no adapter drives is a spec, not runtime meaning. Metal already has versioned programs, fences, timeout-retains-ownership, and release-refused-while-live. Child supervision already has deadline, stop, returncode, and group cleanup. Either consumer can feed the machine with **no new C**. Pure-model bands must stay labeled; they do not satisfy this item.

**5. Item 6 is north-star text, not a runtime organ.**
An OS capability map is justified only as hosted / missing / next witness. It must not grow C, invent firmware/MMU/IRQ work, or treat documentation rows as organs.

**6. Do not grow the seed in this movement.**
No C lease table, no second decoder, no new sensing stream, no new primitive numbers in the 213/214 slots.

## Implementation recommendation

Treat this as one ownership movement with three landing surfaces: **retire a C meaning island**, **make publication/retirement executable on an existing host path**, **tell the truth in the north star**. Skip a tiny BMP-only patch; also skip a speculative kernel map.

### A. BMP: map in C, meaning in Form

1. Keep file open/map/read as the existing carrier. Feed `frame-luma.bml` a captured byte span, not a synthetic C pixel stream.
2. Repair overflow **before** `width * height * bpp` (and row-stride) multiplication; refuse extreme dimensions with the same malformed/truncated/top-down/missing-input witnesses already required.
3. On the same >1 MiB fixture, record C path vs Form path: wall time, whether pixels remain a span, and any list materialization. That record is the compiler specialization gap.
4. If the gap is large and pixels stay in a span, the coherent extra work is Form emission of a CPU-native fold (RAM image, same nine-value row), admitted beside the interpreter, **explicitly selected**. If that emission is not in this movement, retire C anyway only with the gap written down and no native-bandwidth claim.
5. Make the nine-value thresholds a Form policy. Default row stays for explicit callers. A/B two policies on **one** captured byte string (no second file read, no second device effect).
6. Remove primitives **213** and **214** from seed registry and live effect/census mirrors. **Reserve the numbers.** Stale images that name them get an explicit missing-operation / tombstone status, never a different op. Live callers import Form. No second C interpretation path.

### B. Leases: Form state machine, existing completions

Implement the machine in Form only:

| Field | Rule |
|---|---|
| context | Foreign context cannot complete or free |
| resource identity + generation | Stale generation cannot revive a slot |
| version | Chosen at submit; immutable for that submission |
| submission id | Duplicate completion is a no-op / refuse, not a second free |
| completion | Only matching context+generation+submission |

Publication rules:

- **Publish/select** affects **new** submissions only.
- **In-flight** work keeps the version it was granted.
- **Timeout** = still pending; ownership retained (already true for Metal buffers).
- **Cancel** = request until acknowledgement; not an implicit free.
- **Rollback** = another selection, not a rebuild, not a destructor of live work.
- Evidence may expire; policies may retire; **leases do not**.
- Multiple belief sets coexist by **named domain + epoch** (already the `metal-jit-choice` / `belief-freshness` shape). A retired policy must not rewrite another domain’s evidence.

**Bind it now, without new C, to one real path — Metal first:**

Use live JIT admission (`4095` already has two in-RAM programs, equal outputs, continued use, separate fences). Form stores lease rows keyed by fence/submission. Selecting version B must not complete or reclaim version A’s outstanding work. A timeout row stays pending. A completion with the wrong generation or a duplicate fence id cannot free the other lease.

Optional same-machine second consumer: child-process deadline/stop/returncode. Same rules, different resource kind. Do not add a C child-lease table.

Do **not** implement per-version Metal pipeline `Release` in C unless an existing adapter destructor already exists and Form only *decides* when it is legal. Today pipelines live until process exit; that remains an explicit unproved reclamation gap even after the Form machine is correct.

### C. Immutable lifetime vs replaceable belief

Three layers, different mutability:

1. **Tombstones and ABI slots** — immortal. 213/214 never mean something else.
2. **Submission leases** — immutable for their lifetime. Version, generation, context, submission id frozen at grant.
3. **Policies and evidence** — replaceable. Thresholds, program text, belief sets, freshness stamps. Stale evidence refuses **selection**, it does not mutate in-flight leases.

That is how several domain-specific belief sets can coexist with one resource world: choice names the domain; the lease does not carry the belief, only the version granted under it.

### D. OS map: honesty table, not a bootloader

Expand the north star into the requested capability list. Each row: **current hosted organ**, **missing freestanding capability**, **next observable witness**. Highest-value hosted omissions for *this* runtime (not a fantasy board):

- process-global contexts vs one explicit context object
- adapter not live-replaceable
- pipeline objects until process exit
- global quiesce vs per-buffer wait
- 65535-slot handle wire
- parsing / eval / executable allocation still in C

Freestanding boot, MMU, IRQ, drivers, filesystems, net, power: **missing**. Next witness for those is “none in this movement.” A hosted runtime only controls what the host delegated (Metal device, files, child processes, time).

## What would justify merge

Merge when **all** of these are true:

- Net C reduction (the ~240-line island and sensing stream gone); **zero** new C mechanism.
- File data crosses as a handle/span; Form owns decode policy and overflow refusal.
- Same >1 MiB fixture timed for old C vs Form (and CPU-native fold if emitted). Numbers recorded; no max-bandwidth claim.
- Malformed, truncated, top-down, extreme-dimension, missing-input, overflow-refusal witnesses still fail closed.
- Two Form policies compared on one captured byte string; default row preserved when requested.
- Tags 213/214 removed and reserved; stale image → explicit missing-operation; no live C decoder.
- Lease machine exists in Form and is driven by at least Metal fence/admission (child supervisor allowed as a second kind). Duplicate, wrong-context, stale-generation, and foreign completion cannot free another submission. Timeout pending ≠ free. Cancel pending until ack. Pure-model tests labeled.
- North-star OS map is hosted/missing/next-witness only.

## What must remain explicitly unproved

- Native-speed BMP / maximum-bandwidth anything
- Compiler specialization gap closed, unless a CPU-native fold actually lands and is compared
- Live Metal **adapter** replacement (`dlopen` once ≠ hot-swap)
- Per-version GPU pipeline reclamation and multi-context C ownership
- Per-buffer overlap; handle counts above 65535
- Archive path as “skip MSL parse” (it is a pipeline binary archive; Metal still builds a library from source)
- Freestanding boot, firmware, MMU, interrupts, scheduler, drivers, block/FS, net, display/audio as raw hardware, power/hotplug, crash recovery, update as an OS
- General reasoning, identity, health, remote answers, consciousness
- Lexical function census as semantic proof

A later movement can specialize the luma fold, reclaim pipelines under this lease contract, or split quiesce per buffer. This movement should not pretend those are included.
