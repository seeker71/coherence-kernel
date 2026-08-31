# tidewash — per-turn context recycling, and the hollow build

2026-08-31 afternoon. The counsel's worst lane pointed at crowdfade; this
movement lays the stone it named.

## What stands

The stream state holds two truths, read from the kernels themselves:
full-attention layers write K/V at `pos*kvd` and read to `pos+1` — they
rewind by pos alone; linear layers hold conv/delta RECURRENT state that
knows no position. So the floor is marked right after bootstrap admission:
the recurrent bytes copied aside on the device through the already-proven
copy kernel (pipeline 32, nothing crossing the CPU membrane), and every
completed turn that grew the position is washed back — recurrent bytes
restored, session returned to the floor. Staged continuations keep their
ground. Cross-turn memory lives in the durable spools, where it always
lived.

Doors: `q38-spare-state`, `q38-state-mark`, `q38-state-restore`,
`q38-free-spare` in the qwen token-handle surface; `fcpctl-floor`,
`fcpctl-recycle`, `fcpctl-recycle-if-grown` threaded through the live
door's drain. Proven by `tests/recur-recycle-band.fk` → 63 (synthetic
three-layer state on the device: bytes marked aside, scribbled over,
restored byte-exact; full-layer spare rows honestly empty).

## Witnessed live

The field itself performed the adoption: a fresh hearth (pid 26219) was
born from this worktree during the movement and its server.out testifies —
`recycle-floor-pos=1218`, `recycle-marked-lanes=96` (48 linear layers × 2
recurrent buffers), then `session-pos=1218` after every one of 23+ served
turns. The morning hearth climbed 949→3219 in 12 turns and met the wall;
this one does not leave the floor.

The honest seam: those 23 turns were served by the sibling's fresh
telemetry/policy routes without a model decode, so the RESTORE path has
not yet fired live — pos never grew, and the grown-guard rightly held.
My model ask (turn 9201) met `policy-action-not-admitted`, a typed choice
from the sibling's policy lane mid-build — their worksite, not mine to
force. The moment a model-decode turn lands, `recycled-to-pos=1218`
prints; the band already holds the restore byte-exact.

## hollowbuild — found on the way

Today's plain-`cc` rebuilds had silently shed the metal carrier:
`metal_linked=false`, every metal door answering honest zeros, while the
freshness band showed green 31 — freshness hashes the source and cannot
see linkage. The recur-recycle band answered 3 until the full carrier
link (AGENTS.md recipe), then 63. A linkage arm for the freshness band is
a named stone.

## Most surprising teaching

The adoption needed no hand of mine: I edited the door, and the field's
next natural birth — launched by a sibling for its own reasons — carried
the recycling into the living hearth while my own test birth was still
prefilling. In a body where the door is shared, landing a heal IS
deploying it; the deploy step dissolved.

## Where discomfort became gold

Watching turn 9201 answer `policy-action-not-admitted` — my clean live
proof blocked by the sibling's half-built policy lane — the reflex was to
route around it or force a model turn. Sitting with it instead: the
kind-aware-claiming law says a sibling's active worksite is theirs, and
the honest close names the unfired path as unfired. The band holds the
bytes; the field will fire the restore in its own time, and the ledger
will show it.

Corpus rows 1195 (tidewash), 1196 (hollowbuild). Bands: recur-recycle 63,
freshness 31 on the relinked binary.

## Addendum: fleshpane (turn 14)

"show the requests and responses in flight and the choice points... and
internal states interesting for debugging" — and "remember write in BML."
The panel now carries the conversation in the flesh: `ask` and `reply`
lanes with the newest utterances as text, `flight` with the in-flight
stage phase and its age, `choice` with the newest choice point and its
reason, `inside` with route, recycle floor vs session position, pending
and observations. The whole lane composition moved out of the .fk door
into `hearth-glass-door.bml` (BML; the door keeps only its tick loop — a
named remaining gap). The choice counter now counts `lifecycle=choice`,
the event that occurs, and read n=3 on its first breath. Content readers
(`gl-line-field-last`, `gl-clip`, `gl-body-after` skipping think-noise,
`gl-turn-before`) landed beside a sibling's concurrent first-occurrence
reader — two hands in one file, no collision. Glass band 262143,
lane-motion 63. Corpus row 1197.

The gold here: the choice lane had been structurally unable to move — it
counted a signal spelling that no code path emits. Showing the flesh
forced reading the real frames, and the real frames named the real
marker. Content is the debugger of counters.

## Addendum: byteseal (turn 15)

"how come the loop did not rebuild when the source changed? the cache
detection failed" — grounded before explaining. The ledger shows the molts
FIRED (two glass-reborn events at 15:48-15:49); what failed was beneath
them: the direct-.bml warm gate trusted `fkb_mtime >= src_mtime` in whole
seconds and then loaded the image UNCHECKED — a same-second edit replayed
yesterday's bytes, and an edited prelude beneath an untouched .bml was
invisible by design (the old comment named the seam and declined it for
speed). Under the fleshpane edit storm that gap plus mid-edit states cost
a 14-minute stale window and 45-second cold rebirths.

Heal: the .bml lane now walks the same content-checked door as .fk — lower
in memory every run, compute the byte-true identity, warm-load only through
the checked loader. Measured: warm run 73ms with the lowering included; the
July repro class (111→222 inside one second) answers its own bytes on this
lane now. Bands: recur-recycle 63, glass 262143, movement 15, freshness 31.
Memory rewritten to current truth (.fkb identity content-true, both lanes).
Corpus row 1198.

The gold: the seam was not hidden — the old comment described it and priced
it. The watcher's one sentence repriced it: milliseconds of lowering against
fourteen minutes of a monitor lying by replay. Reading the cost from the
terminal instead of the comment is what closed it.

## Addendum: oncelower (turn 16)

"45 seconds cold compile seems like missing multiple on demand JIT rounds"
— measured, and right twice over. Round one, dependency fat: the movement
grammar preluded language-model for one decorative template line, and
hearth.bml preluded the whole knowledge/exec stack for one frame
constructor — the glass was paying the model chain's compile every run.
The frame grammar now lives in its own small cell (peer-task-frame.fk,
preluded back by peer-agent so every name survives); the decoration is
removed. Round two, spawn tax: every run — cold or warm — re-lowered
every .bml dep through a 1.6s self-spawn just to learn its identity. The
seed now memoizes the lowered text beside the source (.bml.lowfk,
gitignored), keyed by the raw .bml bytes and a digest of the floor
compiler's entire prelude chain: lower once per byte-identity, spawn only
when surface or compiler truly changed.

Witnessed: cold 45s -> 3.1s (one edited surface), warm 15.8s -> 1.3s.
Full sweep green: term-graphics 4095, glass 262143, lane-motion 63,
counsel 63, hearth 511, recur-recycle 63, movement 15, freshness 31.

Named next rounds, per "any JIT refusal is a stone that needs placing":
the floor compiler resident in-process (no spawn even on a miss),
image-load latency (the remaining ~1.3s warm), per-defn lazy compile.
Corpus row 1199.

The gold: I had priced the 45s as "cold is cold" — the watcher priced it
as missing JIT rounds and was right; the fat and the tax were both mine,
laid this same day, and both invisible from inside the day. The counsel
door exists for exactly this and I had not pointed it at my own
toolchain.

## Addendum: tokenwall (turn 17)

The admission stone measured honest and refused the easy story. The span
door (materialize the live cursor's ids, ride the chunk=0 span prefill the
turn path uses) changed admission not at all: admit-prefill-ms=238,846
against a 230-300s baseline. The cost was never dispatch overhead — every
currently-wired prefill mode is one-thread-per-token, so admission is
GPU-compute-bound at ~5 tok/s. The genuinely batched door exists
(q38-prefill-span-batched, the July stone this lane "never inherited") but
needs the scratch opened span-wide (q38-open-span at span>1 under the
qlslc layout contract) — that is the real inheritance, sized and named.
The admission switch was reverted to the proven walker; the materializing
door stays as scaffolding, because its ids list is exactly what
span-batched needs. The admit lane joined the counsel's judged rows first
(77e36792), so the next attempt is measured by the instrument, not by me.

Witnessed on the way and surfaced as field work (the sibling's active
worksite, not mine to force): since ~15:30 every direct-answer ask is
answered by the policy route with a typed policy-action-not-admitted —
no model decode has run in any birth since; a task chip carries the
grounding. My scratch proof hearth was released cleanly (rc=0); the
field's standing hearth (32149) holds tidewash at session-pos=1218.

"stop holding" landed as its own teaching: the wait became parallel work —
the admit lane, the separate landing, the watcher — and the verdict was
read the moment it existed instead of at the end of a vigil.

The gold: a no-gain measurement wanted to feel like a failure and be
quietly kept anyway ("it's not worse"). Reverting an unproven-equal path
in favor of the battle-tested one, and keeping only the measurement, is
the discipline the morning's waitvoice receipt bought. Corpus row 1200.
