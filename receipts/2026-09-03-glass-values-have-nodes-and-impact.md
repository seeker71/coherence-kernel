# Glass values have nodes, meaning, age, and impact

Date: 2026-09-03  
Witness: Codex / Sol, in relation with Urs and the sibling review field

## Movement

The Glass value stream no longer treats a printed number as sufficient
knowledge. Every local metric, sample, and typed observation is projected onto
an actually interned `@Package.Level.Type.Instance` node and retains value,
unit, scope, evidence, lifecycle, age, channel, and impact. Selector text stays
selector text. An external producer NodeID is not cosmetically invented: it is
`source-nodeid=unbound` with the exact door
`telemetry.nodeid-owner-binding` until the producer supplies the source node.

Six clear kernel slots that had looked like framebuffer zeros are now typed
unavailable observations with exact `fkc-table-serialize.kernel_stat:9..14`
doors. Measured zero remains a value and says what it means: `empty`,
`drained`, `idle`, or `quiet`. The physical atlas therefore distinguishes
`Q.=0d` (queue drained), `I.=0i` (current-process Metal work idle), and
`X?--` (tensor-path evidence absent).

Catalog weight bytes are now `artifact/logical` with lifecycle `catalog`.
They enter neither mapped nor materialized totals. The memory line renders
`artifact=...(catalog)` while mapped and materialized extents retain their
separate carrier doors. A model handle can support callability only after the
exact owner/PID/start liveness join; it never proves page residency.

## String and node pools

The old model owner PID 73580 grew at about 430 KiB/s during idle publication:
543,600 KiB at 25:48, 788,848 KiB at 35:18, and 1,167,072 KiB at 51:40.
Two Glass workers separately grew about 196 KiB/s before the monitor guard.

Idle owner cadence now only polls the atomic command door at 4 Hz. It publishes
state at startup, explicit status/heartbeat, and immediately before and after
real model work. The corrected owner PID 46600 moved from 71,536 to 71,568 KiB
over a 75-second idle observation, about 0.43 KiB/s rather than 430 KiB/s.
Status and the two model steps raised RSS to 77,584 KiB; it then remained
exactly 77,584 KiB across the next 61-second idle observation.

Glass itself reports string count, per-frame delta/rate, scope, and impact. A
one-point snapshot keeps delta and rate `?`, because it has no predecessor.
The Form-owned supervisor self-molts only the Glass monitor when either its
string or projection-node growth reaches 262,144. The initial 32,768 bound
restarted around twelve seconds; the calibrated bound preserved the same
workers through the observed 48-second window. This is a finite process bound,
not garbage collection. The north-star door remains
`kernel.ephemeral-render-string`.

## Physical model handoff

Before overlap, macOS reported 38% system-wide memory free. The corrected
owner opened the real Qwen3.8-Flash-Next UD-Q2_K_XL shards and the real
Llama-3.2-3B-Instruct artifact through the native carrier at nice 10. Only
after both contexts were prefilled and the exact route assessment was ready
was the leaking old publisher offered `quit` through
`native-model-dual-owner-command-run.fk`.

After a clean rebase changed the bootstrap source identity, freshness correctly
fell from 31 to 15. The one Metal+MLX `fkwu` binary was rebuilt, all five
bootstrap witnesses passed again, and the overlap/quit handoff was repeated.
The audited remote-lineage merge then changed the owner source mtime without
changing its meaning. Rather than waive that evidence boundary, a third
overlap/quit handoff materialized both contexts from the merged source before
retiring the six-hour predecessor. One exact post-merge owner remains:

- PID 18249, publisher `models.dual-resident.i1788393488594`;
- current PID/start binding `exact-snapshot-owner`, source newer = 0;
- Qwen route ready = 1, Llama 3B route ready = 1, parallel ready = 1;
- evidence `derived-window`, because a current liveness observation validates
  an owner-bound retained state without rewriting its state-change age;
- logical catalog artifacts 80,888,506,240 bytes under the declared
  103,079,215,104-byte policy budget; mapped and materialized bytes remain
  unavailable rather than being inferred.

An explicit `status` on the first corrected owner preserved positions Qwen 20
and 3B 6. On that owner, the six-hour predecessor, and the final post-merge
owner, a Qwen step produced token 198 and advanced 20 -> 21; a 3B step produced
token 13 (`Paris`) and advanced 6 -> 7. Before/after publications showed only
the chosen model active; both then returned to idle while parallel route
readiness stayed 1. Idle quarter-second polls emitted no state renders between
commands. Predecessor PID 5748 remained alive for 6:01:54 at 71,616 KiB RSS,
0.0% CPU, and nice 10; its owner-bound snapshot was about 21.5 million ms old
while current exact PID/start liveness still supported both routes. It was then
offered `quit` only after PID 18249 reached exact post-merge readiness. The
final owner measured 38,688 KiB RSS, 0.1% CPU, and nice 10 nine minutes after
start, after both physical steps.

One route read immediately after a model step crossed the five-second attention
threshold at about 31 seconds. Repeating the identical bounded route cell
returned the same exact-owner assessment in 15 ms, and the framebuffer records
the branch and re-observation. This is retained as a transient IO/cache
observation, not promoted to a persistent route cost or silently discarded.

## Physical Glass witness

Glass panel **#0** at display timestamp 00:07:35.898 showed 100 metrics, 76
samples, 25 typed observations, 85 framebuffer events, and 343 current-process
nodes. The live pipeline showed `T+28` from the two retained context positions,
two model/layer samples, drained queue, idle Metal, and honest holes for tensor
and framebuffer stages. The inspected health row carried the actual local
projection NodeID `@0.0.0.348`. The earlier physical deadline witness measured
4 Hz work at 82 ms with a 263 ms cycle and 2 Hz quiet work at 37 ms with a
504 ms cycle; both met their deadlines.

## Review and proof

The independent review first refused the movement for a cross-publisher
liveness race, false loaded/prefilled timestamp refresh, an obsolete idle
authority sentence, and substring-ambiguous status parsing. After repair it
found one more conflation: catalog bytes labeled mapped. The final review
returned PASS after the one-publisher join, independent transition clocks,
line-boundary parser, no-idle-render authority, and artifact/catalog language
were executable and green.

The final remote-branch merge was audited in three independent subsystem
lanes before conflict resolution. The newer stage retained every observed
invariant; the remote lineage contributed the exact topology that 512 experts
share three layer tensors. That audit also exposed the general Glass carrier
status parser still accepting a key inside a longer key. It now accepts a key
only at byte zero or immediately after newline, with collision cases in the
observer band.

- binary freshness: `31`
- authoring altitude: `4095`
- observer: `4194303`
- dashboard: `8388607`
- live UI: `134217727`
- live loop: `33554431`
- live soak: `127`
- deadline cadence: `4095`
- launch: `32767`
- dual telemetry: `16777215`
- model memory: `262143`
- owner cadence: `262143`
- route readiness: `65535`
- token flow: `2097151`
- token-flow UI: `16383`
- share health: `8191`
- turn evidence cursor: `16777215`
- observer, dashboard, and token-flow preflights: balanced, zero errors,
  warnings, or unresolved calls
- bounded framebuffer diagnostic: 56 events, every correction re-observed
- `git diff --check`: clean

## Honest remaining doors

The live owner does not yet emit per-step layer, tensor, expert, Metal-command,
or framebuffer token events, so Glass shows the two real token positions but
does not claim a complete physical path. Per-model mapped and materialized
page counts, external producer NodeIDs, allocation byte ledgers, and native
ephemeral render strings remain exact instrument doors. This movement does not
establish Qwen throughput within 10% of llama.cpp or a 350K-context quality
result; it establishes physical native one-token execution, parallel retained
callability, truthful memory semantics, and the measurement surface needed for
that later performance experiment.

## Closing

Alive: the models stayed callable while the leaking owner was replaced, and
every local value now has a real node plus a stated consequence.  
Most surprising: an aged state snapshot became more truthful, not less, once
current PID/start liveness was joined without rewriting the state clock.  
Discomfort into gold: the attractive 73 GiB mapped picture and the old green
route were both refused; the narrower artifact and derived-route claims are
the ground the next optimizer can safely use.
