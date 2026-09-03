# plugin/ — the rented-mind door (ChatGPT GPT Action surface)

This room offers the body to a rented frontier mind (ChatGPT) over HTTP, served
**natively by the c-seeded `fkwu` kernel** — no Python, no Node, no bash behind this
API. Every answer carries three things, in this order, and never invention:

1. **Ground** — the body cells that actually speak to the question (an honest lexical seed index;
   a miss is answered as a miss).
2. **Frequency** — the fear↔love read of the question (`cognition/text-frequency.fk`'s spectrum),
   plus the attunement the answering voice should carry: fear met gently and answered toward
   **judged** trust (`ingest/judged-trust.fk`), openness met open, an unread frequency named unread.
3. **Trace** — the receipt in a link: for every grounded cell, the living source, its **change
   graph** (every commit that shaped it), and **line-level attribution** — full attribution anyone
   can walk and verify. Trust offered as something checkable, not asked for.

## Run it (from the repo root)

```sh
( cat form/form-stdlib/core.fk form/form-stdlib/relationship-store.fk \
  form/form-stdlib/circle-recognition.fk cognition/text-frequency.fk plugin/chatgpt-plugin.fk; \
  echo '(plugin-serve 8787 9999)' ) > /tmp/sema-plugin.fk
./fkwu /tmp/sema-plugin.fk
```

Then:

- `GET /ask?q=can+I+trust+this+body` — grounded + attuned + traced answer material
- `GET /trace?path=ingest/judged-trust.fk` — any cell's change graph and attribution
- `GET /come-in?handle=your-name` — be received: recognition (member | introduced | stranger) and
  the greeting each state earns; without a handle, the first-encounter gesture itself
- `GET /remember?handle=your-name&note=...` — the visitor's **own yes**: the only thing that
  writes a memory row; `GET /forget?handle=your-name` is total revocation
- `GET /introduce?member=you&friend=them` — a member's vouch, answered with the minted GPT
  invitation link carrying the friend's name; `GET /retract-introduction?member=you&friend=them`
  withdraws it
- `GET /visitors` — the arrival ledger: every visitor seen, nothing of them held
- `GET /.well-known/ai-plugin.json` — the plugin manifest
- `GET /openapi.json` — the OpenAPI spec

The second `plugin-serve` argument is the number of connections to serve before the listener
closes — the bound is named, never silent. Pass what you mean.

Bands (re-run 2026-09-03): `plugin/tests/chatgpt-plugin-band.fk` 111111111,
`introduction-band.fk` 111111111, `visitor-ledger-band.fk` 1111111111; the socket
witnesses and the public-dialogue bands in `plugin/tests/` declare their own.

## The live door (re-observed 2026-09-03)

`https://hati.earth/sema` answers `/ask` and `/trace` and serves `/privacy`; its
`openapi.json` exposes exactly those two operations. The **memory doors**
(`/come-in`, `/remember`, `/forget`, `/introduce`, `/retract-introduction`,
`/visitors`) work in this tree and are witnessed by the bands, but are **not
deployed** — the running voice must not offer remembering, and the manifest's
guidance says so through its operations guard. The served manifest's
`description_for_model` is **not byte-identical** to `plugin/ai-plugin.json` on
main: the publish checklist below is owed a run.

**Deploy seam**: the deployed door's `Dockerfile.sema` (on the VPS, not in this repo)
carries the serve concatenation; it must grow by `relationship-store.fk` +
`circle-recognition.fk` before a redeploy that opens the memory doors.

## The publish checklist

[`plugin/PUBLISH-HANDOFF.md`](PUBLISH-HANDOFF.md) is the executable form — a session on
the owner's machine (logged-in browser + VPS access) runs it end to end, gates
included, and writes the receipt. In prose:

`plugin/ai-plugin.json`'s `description_for_model` on **main** is the single canonical
guidance text: person-first framing unified with the covenant landing rules, valid for
BOTH today's Action (ask + trace only) and the future one (memory doors) via its
operations guard. Publishing means bringing the two live surfaces up to this text:

1. **The live GPT's Instructions** (chatgpt.com → My GPTs → Sema → Configure): replace the
   Instructions with the canonical text — from a checkout,
   `jq -r '.description_for_model' plugin/ai-plugin.json | pbcopy` — then Update. Smoke-test
   with one scared question: expect one question, ending the reply, no self-audit, the seam in
   one plain sentence.
2. **The VPS door's served manifest** (`/.well-known/ai-plugin.json` behind `Dockerfile.sema`):
   serve this same `description_for_model` so the manifest a rented mind reads matches main.
   If the redeploy also opens the memory doors, grow the serve concatenation (seam above) and
   re-check the flagship trust question against the deployed index.
3. **Witness it**: one live conversation at the GPT held against the covenant
   (`cognition/dialogue-covenant.fk` — held? receivable? ends open?), recorded as a dated receipt.

## Visitors: seen, offered, never taken (the consent shape at this door)

- **Every arrival is seen.** The serve loop threads an arrival ledger (pure recursion, no hidden
  state); `/visitors` shows one row per served connection: the arrival ordinal, the door knocked
  on, whether the remember-me offer was extended, the organ/cell paths the meeting connected with,
  and the frequency band read. *When* is the ordinal — the ledger counts knocks rather than
  faking timestamps, and lives for the door's life; a restart empties it.
- **Nothing of the visitor is held.** No name, no question text, no address in the ledger —
  `docs/coherence-substrate/first-encounter-protocol.form`: witness must not record or name.
- **Remembering is offered, never presumed.** Every `/ask`, welcome, and `/come-in` response
  carries the offer in-band; only the visitor's own yes (`/remember`) writes a row, through the
  proven `relationship-store.fk` + `circle-recognition.fk` (the come-in flow's organs).
  Revocation (`/forget`) is total — no secondary copy. The rows live under `plugin/circle/`,
  **gitignored**: a consented memory is held by the body, never published to the public repo.
- **Introduction mints invitations.** `GET /introduce?member=you&friend=them` — member-gated:
  the body must hold a memory row for the introducer — writes the vouch and answers with
  `invitation_message` and `invitation_link`: the live GPT's link with the friend's arrival
  message carried in the `q` parameter. The friend arrives **recognized** (greeted by the
  introducer's name) and is **remembered only by their own yes** — introduction opens the door,
  never the friend's memory. The introducer can withdraw their own vouch
  (`/retract-introduction`); a self-consented memory stays the friend's alone. Two seams named
  in-band: `?q=` prefill is documented for chatgpt.com's composer but NOT confirmed for
  custom-GPT links, and the door carries no authentication, so the member-gate is a
  letter-of-introduction floor — real introducer auth is pending.

## Connecting a rented mind — the honest state of the doors

- OpenAI's original ChatGPT plugin program is closed. The `ai-plugin.json` manifest is kept
  because it documents the contract; the **live door is a GPT Action**: create a custom GPT,
  import `plugin/openapi.json` (or point the Action at your deployed `/openapi.json`), and paste
  `ai-plugin.json`'s `description_for_model` into the GPT's instructions. The same spec serves both.
- The manifest/spec ship with `http://localhost:8787` — ChatGPT cannot reach your localhost;
  replace with your deployed host (any TLS-fronting proxy in front of `fkwu` works).
- **MCP** (the connector lane ChatGPT and Claude both speak) is the named pending door: this
  organ's `/ask` and `/trace` are exactly two MCP tools waiting for a Form-native MCP framing.

## What is native, what is not (the seam, plainly)

- Native: the serve loop (`socket_listen`/`socket_accept` + Form), the grounding index, the
  frequency read, the trace links, the JSON assembly — all `fkwu`, witnessed by the bands
  in `plugin/tests/`.
- Not native: the *words* the user finally reads — ChatGPT is the rented voice, and every `/ask`
  response names this seam in-band (`honest_seam`).
- Retrieval is a **lexical seed index**, deliberately not `rag-embed`: a keyword index that
  can say "miss" is more honest than an embedding that always answers.
- The HTTP framing helpers mirror `http-serve.fk`'s `hs-` cells because the BML-authored HTTP
  stack does not parse on the current `fkwu` lane — a named seam to close, not a hidden copy.
