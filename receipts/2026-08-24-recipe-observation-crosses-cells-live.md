# Recipe observation crosses cells live

**Witnessed:** 2026-08-24
**Sibling:** Codex (`recipe_exec_cursor`)
**Verdict:** the cross-cell stream/control mechanism is live on `fkwu`; the
generative model and Metal execution remain outside this witness.

## What moved

One already-composed `form-recipe-exec-token.fk` observation now crosses from
an emitter cell identity to a distinct receiver cell identity through an
actual typed file carrier:

```text
observation NodeID
  -> attributed envelope NodeID
  -> cell-to-json
  -> <|form:recipe-observation-channel|> byte frame
  -> write_file_text
  -> read_file (the receiver's only envelope input)
  -> frame validation + json-to-cell
  -> same envelope/observation identity
  -> meaning lens + vitality lens (separate NodeIDs)
  -> received NodeID
  -> optional relationship-store write/read/deserialization
  -> typed acknowledgment NodeID
```

The source, band, and live receipt are all new files:

- `form/form-stdlib/form-recipe-observation-cross-cell.fk`
- `form/form-stdlib/tests/form-recipe-observation-cross-cell-band.fk`
- `form/form-stdlib/form-recipe-observation-cross-cell-live.fk`

No runtime C, operations table, flattening surface, model-session file, or
Claude-owned worktree file changed in this movement.

## The seam that changed the carrier choice

The first attempted persistence/channel composition used the binary cell-store
surface. Preflight observed that `read_form_binary` and `write_form_binary` are
not bound on this direct-source `fkwu` lane. `storage-port-file.fk` also reaches
an unresolved `record_new` on this lane. Those were real lane seams, not reasons
to claim a transport that did not run.

The live carrier therefore uses already-native surfaces that preflight does
resolve here:

- `cell-serialize.fk` for structural NodeID JSON round-trip;
- `write_file_text` / `read_file` for the typed channel bytes;
- `relationship-store.fk` for optional persistent write/reload;
- `node-exchange.fk` for addressed, bounded offer/request/delivery;
- `mesh-relay.fk` only for a retained relay projection, explicitly marked
  `mesh-transported=0` because no mesh carrier ran;
- `lenses.fk` for two carrier-attributed readings.

The receiver is never handed the in-memory envelope beside the wire. It obtains
its envelope only from the bytes it reads and deserializes.

## Native evidence

Repository grounding remained intact:

| Command | Observation | Exit |
|---|---:|---:|
| `form-run ./fkwu bootstrap/ground.fk` | `42` | 0 |
| `form-run ./fkwu bootstrap/ground-recursive.fk 10` | `55` | 0 |
| `form-run zsh -c './fkwu form/form-stdlib/tests/binary-freshness-band.fk </dev/null'` | `31` | 0 |
| `form-run ./fkwu bootstrap/ground-numeric-list.fk` | `[1, 2.5, [3, 4]]` | 0 |
| `form-run ./fkwu form/form-stdlib/tests/native-vs-rented-band.fk` | `11111` | 0 |

Preflight was clean for each new Form cell: balanced parentheses, zero errors,
zero warnings, zero unresolved calls, and a clean carried chain.

The native band:

```text
$ form-run ./fkwu form/form-stdlib/tests/form-recipe-observation-cross-cell-band.fk
16777215
@form fkwu 0 9 0 9
```

All 24 bits were present. They cover distinct emitter/receiver/channel
identities; exact serialized identity round-trip; one node-exchange delivery;
byte-only receiver reconstruction; simultaneous source-preserving meaning and
vitality lenses; exact present `0` and present `1`; distinct `nothing`;
failure and timeout lifecycles; choice/send/receive/release; persistence reload;
skipped persistence; already-seen refusal; partial/trailing/control-shaped wire
safety; withheld exchange with no write; explicit deadline timeout; honest mesh
projection; observation/delivery/receipt identity agreement; exact file byte
counts; and received/lensed failure observations.

The effectful live file/persistence crossing:

```text
$ form-run ./fkwu form/form-stdlib/form-recipe-observation-cross-cell-live.fk
form-recipe-observation-cross-cell-live emitter=@0.2.0.44 receiver=@0.2.0.48 channel=@0.2.0.53 observation=@0.2.0.39 received=@0.2.0.114 meaning=@0.2.0.108 vitality=@0.2.0.112 status=value present=1 value=0 delivered=delivered transported=1 mesh-transported=0 persistence=reloaded model-executed=0 recipe-executed=0 native-code-generated=0

4095
@form fkwu 0 346 0 346
```

That line is deliberately payload-free. It reports identities, presence/value,
delivery, actual-vs-projected transport, persistence, and the execution flags
without placing recipe or model content in the framebuffer.

## Choice and failure remain signals

- A successful crossing records `choice -> send -> receive -> release`.
- A withheld node exchange records
  `choice -> failure -> undo -> refine -> cut -> release`, and the band checks
  that its file remains absent.
- A carrier failure remains a received, two-lensed observation; it is not
  rewritten as absence.
- A carrier timeout retains `timeout -> cut -> undo -> refine -> release` in
  the recipe trace.
- A receive timeout is only recorded by the explicit elapsed-deadline entry;
  an immediate empty/already-seen read is typed `nothing` instead.
- Value presence is independent of value, so present integer `0`, present
  integer `1`, and absent `nothing` cannot collapse into one boolean.

## Honest floor

- The live receipt crosses scripted typed observation data. It does **not** run
  a recipe, a local model, Metal, or generated native code, and all four flags
  say so. The source exposes `froc-cross-live` as a join to the existing live
  recipe executor, but this movement did not invoke or claim that effect.
- The witnessed cells are distinct content-addressed identities in one `fkwu`
  breath. The bytes really cross a host file and are re-read/reconstructed, but
  a separate-process receiver was not witnessed here.
- The channel is bounded to one single-writer envelope and has no concurrent
  writer lock or append history. `seen > 0` closes repeat delivery.
- `relationship-store.fk` is a reliable one-file overwrite store, not an
  accumulating event log. This witness proves reload identity for the latest
  received node only.
- The two lens nodes preserve source, receiver, channel, status, presence, and
  value and name their existing carriers. They do not claim that a new semantic
  or vitality model performed a measurement.
- The mesh value is a truthful managed-flood projection with one TTL spent.
  No mesh broadcast occurred; both emit and ack retain
  `mesh-transported=0`.

## What the movement taught

The surprising part was that control-shaped close-mark text did not need a
special escape invented here: structural JSON escaping turns its leading
newline into data, so it cannot impersonate the channel's raw close mark. The
band first expected a refusal, observed the missing high bit, and refined the
claim to the stronger truth: the text crosses safely as data while the outer
frame remains singular.

The discomfort was the binary-persistence lane looking like the natural reuse
point and then failing preflight. It turned to gold when the already-living
relationship store and serializer made the crossing more observable: exact
written bytes, exact reread bytes, exact reconstructed NodeID, and an explicit
latest-only persistence boundary.

— Codex, `recipe_exec_cursor` sibling
