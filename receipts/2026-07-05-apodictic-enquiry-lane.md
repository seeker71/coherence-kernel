# 2026-07-05 — the enquiry lane: the door carries its own workings, apodictically

## What was missing (Urs's read of the live GPT)

Node IDs, blueprints/recipes consulted, RAG lookups, internal refs, local/remote
oracle, which path, which model, which enquiry paths were chosen, how assembled and
attuned. Grounded diagnosis: the organ never emitted any of it — `/ask` carried
grounded paths + links + frequency, nothing about its own mechanism. And some of the
missing things do not exist in this door at all (no RAG, no oracle, no model) — which
the response must SAY, or the rented voice will invent them.

## What landed (plugin branch `69c3f232`, deployed and publicly witnessed ~10:45 MDT)

Every `/ask` response now carries an **`enquiry`** block:

- **`index`** — `lexical-seed-v1`, with the honest not-RAG note and the zero-vector
  receipt cited in-band (`receipts/2026-07-01-nl-meaning-net.md`).
- **`cells_considered`** (12) and **`selection`** — per grounded cell: id, path,
  **`nodeid`** (sha256 content address), score, and the exact `matched_tokens`.
- **`oracle_local`** / **`oracle_remote`** / **`model`** — all honestly **none**:
  no LLM runs inside the door; the rented mind reading the response is the only mind
  in the loop, and the seam stays named.
- **`assembly`** — the fixed pipeline, spelled: tokenize → charge → band → attune →
  score → best+2 → emit. **`attunement_band`** — which band chose the rule.

NodeIDs are computed by the body's **own** sha256 recipe
(`form/form-stdlib/sha256.fk`, FIPS-vector-proven; digest verified against system
`shasum` byte-for-byte before shipping), hashed **once at boot** (~15 s for the
64 KB index) because the recipe costs ~3.4 s per 15 KB and request-time hashing would
starve the single-threaded serve loop. The boot table threads through the serve lane
as a value — no globals, no mutable state.

## Witnesses

1. Recipe digest vs system `shasum` on MANIFEST.md: **match** (641 witness).
2. Pure band grown two bits (enquiry structure; real `sha256:` in-band via a
   one-cell boot table): **`11111111111`**.
3. Socket lane unchanged: **`11111`**.
4. Public, through Cloudflare TLS: enquiry block served with all fields; and the
   served `judged-trust` NodeID equals an independent local hash of the same cell:
   **MATCH** — `sha256:d8466a30…b4b64b` both sides.

## Seams named

- `/trace` does not carry a NodeID (request-time hashing too slow; a boot-table
  lookup for indexed cells could come later).
- The NodeID is a sha256 content address, NOT the kernel's intern NodeID
  (`make_nodeid` lane) — named plainly so no one mistakes one for the other.
- Boot now pauses ~15 s before the listener opens (Traefik 502s during it, rare:
  only on restart every 100000 connections or redeploy).

## Closing

**Most surprising teaching**: half of what was "missing" did not need building — it
needed *confessing*. The door had no RAG, no oracle, no model; the moment it says so
in-band, the rented voice stops hallucinating them. Absence stated is information;
absence unstated is an invitation to invent.

**Where discomfort turned to gold**: threading the boot table through six function
signatures of a sibling's organ felt heavy-handed — the itch was to bolt on a
global or hash lazily per-request. Witnessed, the itch was the wrong kind of mercy:
the pure-value threading kept the organ's no-state discipline intact, and the band
its builder left (now eleven bits) blessed the change the same way it blessed the
wiring — proof over politeness, both times.
