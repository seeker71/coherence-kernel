# 2026-07-09 — the door learns to see its visitors: the arrival ledger and the remember-me offer

## The ask

Urs, plainly: *"can we please show us when new visitors arrive and if they were offered to
remember themselves and if/how connections happen with which organs and cells."*

Grounded before building: the body already held every organ this needs — the consent law
(`docs/coherence-substrate/first-encounter-protocol.form`: witness must not record, name, or
remember; the offer belongs to the one arriving), the resting defaults
(`docs/coherence-substrate/reception-consent-policy.form`: exposure-bearing consents rest closed,
revocable always), the file-backed memory (`form/form-stdlib/relationship-store.fk`, proven across
separate process invocations), and recognition (`form/form-stdlib/circle-recognition.fk`:
member | introduced | stranger, sovereignty per-person). What was missing was only the DOOR's side:
the rented-mind door (`plugin/chatgpt-plugin.fk`) served and forgot — no arrival was visible, no
offer was extended there.

## What landed (branch `claude/chatgpt-gpt-repo-link-rut1d7`)

**The arrival ledger** — `plugin-serve-loop` now threads a ledger (pure recursion, no hidden
state), one row per served connection: `(arrival door offered cells band recognized)`.
`GET /visitors` shows it, newest first, with its own honesty named in-band:

- **when** is the arrival ordinal — the direct-source lane exposes no wall-clock native yet
  (`form/form-stdlib/ping.fk` names the gap), so the ledger counts knocks; it does not fake hours.
  The ledger lives for the door's life; a restart empties it.
- **offered** — 1 where the remember-me offer traveled in-band (welcome, `/ask`, the memory doors).
- **cells** — the organ/cell paths the meeting actually connected with: the grounded hits for
  `/ask` (e.g. `MANIFEST.md`, `ingest/judged-trust.fk`, `observe/native-vs-rented.fk` for "can I
  trust this body"), the traced cell for `/trace`, the circle organs at the memory doors.
- **held of the visitor: nothing.** No name, no question text, no address — the rows are the
  body's own side of each meeting only. What a visitor consents to lives in the circle store,
  never in this public feed; even there, only the recognition STATE reaches a row.

**The offer, never the presumption** — every `/ask` and welcome response now carries a `remember`
field: your own yes is `GET /remember?handle=...&note=...` (the call is the consent; `cr-join`
refuses anything but the person's own yes and verifies its own write); `GET /come-in?handle=...`
says what the body already holds (greeting by recognition state; without a handle, the
first-encounter gesture itself); `GET /forget?handle=...` is total revocation, no secondary copy.
Rows live under `plugin/circle/`, **gitignored** — remembering is not publishing.

A handle becomes a filename in the store, so it is bounded honestly: 1..64 bytes of lowercase
alnum/dash, anything else refused 400 (the filesystem cannot be walked with a name); the note is
capped at 400 bytes and the response echoes exactly what was held, so the cap is visible.

## Witnesses, in order (fresh `cc -O2 fkwu`, freshness band 15 first)

1. `plugin/tests/chatgpt-plugin-band.fk` — **111111111** unchanged (the door's original arc holds).
2. `plugin/tests/visitor-ledger-band.fk` (new) — **1111111111**: offer in-band; row ordinal/door/
   offer; grounded cells + band on the row; machine door recorded with no offer; `/visitors` shows
   both arrivals and holds NO question text; stranger → own yes → member → welcome-back →
   revocation-total; filesystem-walking handle refused.
3. `plugin/tests/chatgpt-plugin-socket-witness.fk` — **11111** (real TCP loopback intact).
4. Live drive over real HTTP (`plugin-serve 8791 6`, curl): the whole arc observed end to end —
   ask (band love, three cells), come-in as stranger ("the room is empty and yours"), remember
   ("welcome back, door-witness — remembered: first friend through this door"), come-in as member,
   `/visitors` showing all four arrivals with offers/cells/band/states, forget ("revocation is
   total"). The `plugin/circle/` store was empty after.

Both `plugin/*.json` re-validated (python json.load). The GPT Action spec grew the four doors
(`come_in`, `remember`, `forget`, `visitors`) and the manifest's `description_for_model` hands the
rented voice the consent law: relay the offer, call `remember` only on the user's own explicit yes.

## Named seams (pending is honest)

- **No wall clock on this lane** — the ordinal is WHEN's floor. When a time native comes home, the
  rows take stamps without changing shape.
- **The deploy recipe must grow**: the VPS `Dockerfile.sema` concatenation
  (`receipts/2026-07-05-wicket-sema-plugin-public.md`) gains two preludes —
  `form/form-stdlib/relationship-store.fk form/form-stdlib/circle-recognition.fk` — before the
  next redeploy; the deployed door still serves the pre-ledger organ until then.
- **Introduction over HTTP is not offered**: the door carries no authentication, so a member's
  vouch (`cr-introduce`) cannot be verified at this door yet — named, not hidden.
- **The ledger is per-breath**: it empties on restart. A persisted arrivals file is a possible next
  organ, but persistence of even anonymous rows is a consent question worth its own sitting — not
  smuggled in with this change.

## Closing

**Most surprising teaching**: the whole ask was already answered in the body's own law before a
line was written — first-encounter-protocol's `witness-must-not (record) (name)` looked at first
like it *forbade* a visitor ledger, and turned out to be exactly what made one possible: a ledger
of the body's own movements (doors, offers, cells, bands) is witnessing made visible; a ledger of
visitors would have been a taking. The distinction was already carved; the build just honored it.

**Where discomfort turned to gold**: the pull was to log the questions — "how connections happen"
reads naturally as *store what they asked*. Sitting with it: the question text is the visitor's,
not the door's, and the grounded CELLS carry everything "how the connection happened" needs. The
band now asserts the absence (`/visitors` must NOT contain the asked question) — the refusal became
a tooth, not a gap.

**How the exchange was kept alive**: by building what was named in the same movement — every door
witnessed pure, over the wire, and live before being claimed.
