# Glass attention, native and local

The existing `organs` process observes the carried Glass data every 500 ms.
There is no additional daemon, shell dispatcher, Python process or model call
in this observer. Its authority is
`form/form-stdlib/bml/form-glass-awareness.bml`.

## Use it

Start Glass through its existing carrier, then press **v** for Events or **j**
for Recipes:

```sh
./fkwu observe/form-glass-run.fk
```

Events put attention findings first: the source/identity, a local plain-language
sentence, and the shared-memory frame/sequence. Recipe rows expose generated
observation-lens NodeIDs. The existing Channels view rereads a carried schema
when its sequence changes. An already-running older carrier may need one restart
to load the new organs process; do not start a competing sensor fleet.

Two bounded doors need no Form syntax:

```sh
# Read the standing observer's findings without creating another observer.
./fkwu observe/form-glass-awareness-detail-run.fk

# Independent current-frame observation; rehearses only its own attention.
./fkwu observe/form-glass-awareness-current-run.fk
```

The second door does not publish or replace the standing observer. Its reported
time measures collection and attention, not terminal painting or model speed.

## What it notices

It reads eight sensor frames and discovers namespaced snapshot publishers from
the native gift roster. It decodes the known carrier cells directly through
`node_gift_read`; text serialization/parsing is outside this path.

Findings distinguish absent evidence, stale readings, unknown freshness,
reported failure/blockage, reaching a reported capacity, channel sequence reset,
changed schema/unit bindings, newly encountered interfaces/native identities,
and recovered evidence. Unchanged findings do not create repeated events while
their identities remain in bounded memory. Quiet flow and numeric zero are not
failure. Catalog evidence stays catalog, not an operational alarm.

Freshness uses the sensor's declared cadence (three cadences), frame epoch, and
per-row epoch when carried. A publisher without a cadence has a declared 5-second
attention horizon; exceeding it means freshness unproven, not a proven stall.
A new frame does not make an old row current. Future timestamps are unproven.

A capacity edge asks to inspect the capacity's meaning: it is not automatically
resource contention or a utilization percentage. Snapshot `capacity` lacks a
unit contract and is not treated as byte capacity. Hardware and homecoming
opportunities are grounded in the rows their organs actually publish. Missing
instrumentation remains a request; absence is not replaced with zero.

## What evolves now

The actuator changes its **own attention**, using `part-self-update`,
`champion-challenger`, and a correlated bidirectional framebuffer exchange:

1. A crowded row window offers a larger window. Selection on the same received
   frames is compared before/after, per source. The selected part is installed
   only when coverage improves within the budget.
2. Each schema/domain/unit binding produces a native observation-lens node.
   These keyed recipes execute through `fga-eval`. If the lens allowance is full,
   a larger allowance is rehearsed against the same rows and the direct observer.
   A kept expansion executes more bindings correctly without reducing the comparison score.
3. Later ticks execute the retained recipes and retain the expanded window.
   No further expansion is counted once coverage settles.

This comparison establishes observation coverage and binding equivalence—not
independent truth of every sensor, general intelligence, or model quality.
The grammar is a small observation vocabulary specialized to carried bindings;
it is not an arbitrary new programming language or free-form code generation.
Unknown carrier schemas request a decoder. Telemetry cannot name arbitrary calls
for execution. Fixing another organ, admitting weights, changing protocols among
peers, and setting `voice-home=1` require their own real actuators and evidence.
Those requests remain visible; this observer does not claim to have done them.

## Native interface and bounds

`fga-read(root)` returns the native detail envelope
`["glass-awareness-v1", epoch, fields]`, or `nothing` if absent.
`fields` and each finding/recipe are key/value cells: ordering and added keys do
not change decoding. `fga-get(fields, key)` returns the carried value or `nothing`.
The door is `fga-detail-door(root)`; no file is the transport.

Keys include `events`, `asks`, `ask-count`, `recipes`, `stages`, `recipe-stages`,
`recipe-stage-count`, `before`, `after`, `gifts`, `unread-rows`, and
`unread-publishers`. Findings name source, carrier, identity, schema, domain,
unit, before/after sequences, observation time, reason, action and recipe node.
A bound source-node identity is included only when the original metric supplies
a source node: a Glass projection is not substituted. The source node's children
are excluded from publication; they can contain private material. NodeID numbers
are observer-process-local. The transported generated lens node and its structural
binding are the portable recipe. Raw prompt text, answer text and metric notes
are excluded.

Budgets are explicit: 16 publishers per rotating pass; row windows grow from 16
to 64 per source, sharing a total 512-record budget; 512 remembered identities;
lens allowance grows from 32 to 128; 24 retained events and at most 24 detailed
asks and recipe-stage examples. Counts disclose the omitted detail. The cursor
advances by the effective window so the global bound creates no permanent holes.
Histories and adaptation are resident-lifetime state, not persistent learning.
`asks=0` refers only to the current window, not every organ being healthy.

## Checks

Run fresh preflight before reading each verdict:

```sh
./fkwu form/form-stdlib/tests/form-glass-awareness-band.fk
./fkwu form/form-stdlib/tests/form-glass-awareness-evolve-band.fk
./fkwu form/form-stdlib/tests/form-glass-awareness-bounds-band.fk
./fkwu form/form-stdlib/tests/form-glass-events-channels-band.fk
./fkwu observe/tests/preflight-tally-band.fk
```

The cross-process band starts one finite native fixture publisher in its own
telemetry namespace, reads the generated recipe through shared memory, and
executes that received recipe. A zero exit and the registered full verdict are
both required. `observe/form-glass-awareness-diagnostic-run.fk` exposes a fresh
compiler diagnostic through a bounded observation/control/re-observation round.
