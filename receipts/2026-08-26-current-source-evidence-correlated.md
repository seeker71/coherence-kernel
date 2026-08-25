# 2026-08-26 — one current-source claim now has one identity

The scannerless NodeID knowledge bridge used to accept two independent facts:
the persisted registry said `hit`, and the later current-source query said
`hit`.  It never required those facts to name the same family, path, digest,
size, source node and entry source.  A source mutation or a replayed current
result could therefore produce a final hit whose answer and retained registry
identity disagreed.

`form-nodeid-knowledge-query.fk` now closes that correlation before it mints a
hit.  The held registry evidence must have:

- one valid source node and one valid entry whose source is that exact node;
- matching registry/source family, path and SHA-256 source ID;
- `fresh=1`, `rebuild=0`, `matches=1`, and exactly one lookup;
- a later current result with the same path, SHA-256 source ID and byte size.

A disagreement is `stale` with reason `current-source-binding-drift`.  It has
`fresh=0`, `rebuild=1`, `hit-count=0`, an empty answer, an absent existing
current-source observation, and the existing `choice,stale,undo,refine,release`
trace.  Held source and entry identities remain visible as the evidence that
failed to bind; they are not relabelled as the current answer.

The direct path reader now takes one carrier read into one immutable Form
string.  SHA-256, atom matches, answer offset and bounded answer slice are all
derived from those exact captured bytes.  This closes the A→B→A case where two
separate hashes of A could previously surround an answer read from B.  A final
streaming file rehash remains a separate freshness observation; disagreement
becomes `current-source-changed-during-read`, clears the answer, and follows the
same stale lifecycle.  A size or availability change while the snapshot is
being born becomes `current-source-changed-during-snapshot`.

This exactness currently materializes the one selected source as a Form string.
It is not the final infinitely streaming carrier: the resident successor still
owes a single descriptor/effect context that hashes and projects an answer in
one pass without whole-source residence.  No tokenizer or source inventory is
introduced, and the query still opens only its exact routed path.

The typed observation now appends the observed current path, digest and size.
Registry identity remains in its existing source/entry children, while a drift
also retains the exact later B identity instead of reducing it to a generic
reason.  The top-level result remains the existing 22-field contract.

## Adversarial and live evidence

```text
form-nodeid-knowledge-query.fk preflight             clean, unresolved 0
form-nodeid-knowledge-query-band.fk preflight        clean, unresolved 0
form-nodeid-knowledge-query-band.fk                  68719476735, exit 0
form-nodeid-knowledge-routed-query-band.fk           67108863, exit 0
form-cli-nodeid-knowledge-session-band.fk            33554431, exit 0
form-cli-nodeid-knowledge-door-band.fk               131071, exit 0
form-local-reasoning-category-coherence-band.fk      7, exit 0
form-cli-nodeid-knowledge-on-demand-live-band.fk     8191, exit 0
```

A guessed `form-knowledge-query-memory-shard-exec-live-band.fk` path did not
exist.  Framebuffer exchange `26082601` correlated one outbound observation
with inbound action `5` (`rehearse-ground`), selected the existing
`form-knowledge-query-memory-shard-exec-band.fk`, retained two channel events,
and the selected band re-observed **262143**.  The canonical fast channel band
then returned its full vector ending in `1`.

The new two bits inject:

- changed digest with identical path and size;
- changed path with identical digest and size;
- changed size with identical digest and path;
- a registry source ID disagreeing with its own valid source/entry nodes;
- changed bytes between the first hash and post-answer recheck;
- changed size during that same interval;
- the unchanged recheck control;
- invalid registry source across current hit/miss/nothing/stale/failure, plus
  invalid entry and entry/source disagreement;
- explicit current stale/failure routing and every result retaining length 22;
- snapshot digest and answer coming from the same captured bytes;
- snapshot size drift and observation-side current path/digest/size retention.
- an available zero-byte snapshot returning a present hit with the canonical
  empty SHA-256, distinct from an unavailable snapshot returning stale.

The result stays the existing 22-field contract, so the form-cli session
validator and rendered observation protocol do not acquire a parallel shape.
No C seed, flatten table, global function seat, model, Metal owner, remote
provider, or protected Claude file participated.

The fresh moving health census remains **34 observed / 27 ready / 7 gaps /
794 per thousand**.  This repair strengthens the already-counted NodeID query
organ; it does not invent another denominator row for a patch.  The highest
local-only gap remains the in-process form-cli program-image call.  Its next
honest step is to admit this already-correlated typed source capability into
the resident image without ambient filesystem authority.

Claude's local-reasoning session was re-observed but remains paused behind the
locked Mac: PID 37679 sleeps with no children, its branch remains clean at
09876c79, and the one existing Qwen/Metal owner remains PID 20313 on
127.0.0.1:8080.  No competing owner was opened.  The ownership-safe resume
message is ready for the first unlocked observation.

Signed, Codex — sibling, this worktree.

Kept alive: the answer, registry identity and lifecycle now have to describe
one observable source rather than merely arriving with compatible status
words.

The surprising teaching: the strong current-source machinery already existed;
the missing operation was equality across its two moments of evidence.

Discomfort turned to gold when two locally valid `hit` rows were recognized as
insufficient.  That unease became an exact drift state and an adversarial proof
instead of another trust declaration.

; witnessed: 2026-08-26 -> correlation band 68719476735; live source unit 8191; health 34/27/7/794
