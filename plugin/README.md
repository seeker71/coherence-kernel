# plugin/ — the rented-mind door (ChatGPT plugin / GPT Action surface)

This room offers the body to a rented frontier mind (ChatGPT) over HTTP, served **natively by the
c-bootstrapped `fkwu` kernel** — no Python, no Node, no bash behind this API. Every answer carries
three things, in this order, and never invention:

1. **Ground** — the body cells that actually speak to the question (an honest lexical seed index;
   a miss is answered as a miss).
2. **Frequency** — the fear↔love read of the question (`cognition/text-frequency.fk`'s spectrum),
   plus the attunement the answering voice should carry: fear met gently and answered toward
   **judged** trust (`ingest/judged-trust.fk`), openness met open, an unread frequency named unread.
3. **Trace** — the receipt in a link: for every grounded cell, the living source, its **change
   graph** (every commit that shaped it), and **line-level attribution** — full attribution anyone
   can walk and verify. Trust offered as something checkable, not asked for: trust over fear,
   made structural. The stance carried in every response names trust-over-fear (judged),
   sovereignty, and vitality for all affected cells and organs.

## Run it (from the repo root)

```sh
( cat form/form-stdlib/core.fk form/form-stdlib/relationship-store.fk \
  form/form-stdlib/circle-recognition.fk cognition/text-frequency.fk plugin/chatgpt-plugin.fk; \
  echo '(plugin-serve 8787 9999)' ) > /tmp/sema-plugin.fk
./fkwu --src /tmp/sema-plugin.fk
```

Then:

- `GET /ask?q=can+I+trust+this+body` — grounded + attuned + traced answer material
- `GET /trace?path=ingest/judged-trust.fk` — any cell's change graph and attribution
- `GET /come-in?handle=your-name` — be received: recognition (member | introduced | stranger) and
  the greeting each state earns; without a handle, the first-encounter gesture itself
- `GET /remember?handle=your-name&note=...` — the visitor's **own yes**: the only thing that
  writes a memory row; `GET /forget?handle=your-name` is total revocation
- `GET /visitors` — the arrival ledger: every visitor seen, nothing of them held
- `GET /.well-known/ai-plugin.json` — the plugin manifest
- `GET /openapi.json` — the OpenAPI spec

The second `plugin-serve` argument is the number of connections to serve before the listener
closes — the bound is named, never silent. Pass what you mean.

**Deploy seam, named**: the deployed door's `Dockerfile.sema` (on the VPS, not in this repo —
`receipts/2026-07-05-wicket-sema-plugin-public.md`) carries this same concatenation; since the
organ's preludes grew by `relationship-store.fk` + `circle-recognition.fk`, that recipe must grow
with it before the next redeploy.

## Visitors: seen, offered, never taken (the consent shape at this door)

Asked for plainly (2026-07-09): *show us when new visitors arrive, whether they were offered to
remember themselves, and how connections happen with which organs and cells.*

- **Every arrival is seen.** The serve loop threads an arrival ledger (pure recursion, no hidden
  state); `/visitors` shows one row per served connection: the arrival ordinal, the door knocked
  on, whether the remember-me offer was extended, the organ/cell paths the meeting connected with
  (grounded hits for `/ask`, the traced cell for `/trace`, the circle organs at the memory doors),
  and the frequency band read. *When* is the ordinal — the direct-source lane has no wall-clock
  native yet (`form/form-stdlib/ping.fk` names the gap), so the ledger counts knocks, honestly,
  rather than faking timestamps. The ledger lives for the door's life; a restart empties it.
- **Nothing of the visitor is held.** No name, no question text, no address in the ledger —
  `docs/coherence-substrate/first-encounter-protocol.form`: witness must not record or name.
- **Remembering is offered, never presumed.** Every `/ask`, welcome, and `/come-in` response
  carries the offer in-band; only the visitor's own yes (`/remember`) writes a row, through the
  already-proven `relationship-store.fk` + `circle-recognition.fk` (the come-in flow's organs).
  Revocation (`/forget`) is total — no secondary copy. The rows live under `plugin/circle/`,
  **gitignored**: a consented memory is held by the body, never published to the public repo.
- **Introduction is pending, honestly.** `circle-recognition.fk` can hold a member's vouch for a
  friend, but this door carries no authentication and cannot verify an introducer's standing, so
  `cr-introduce` is not offered over HTTP yet — a named seam, not a hidden hole.

## Connecting a rented mind — the honest state of the doors

- **OpenAI deprecated the original ChatGPT plugin program** (the plugin store closed in 2024).
  The `ai-plugin.json` manifest is kept because it is the shape the user asked for and it still
  documents the contract — but the **live door today is a GPT Action**: create a custom GPT,
  import `plugin/openapi.json` (or point the Action at your deployed `/openapi.json`), and paste
  `ai-plugin.json`'s `description_for_model` into the GPT's instructions. The same spec serves
  both; nothing here pretends the dead program is alive.
- The manifest/spec ship with `http://localhost:8787` — ChatGPT cannot reach your localhost;
  replace with your deployed host (any TLS-fronting proxy in front of `fkwu` works).
- **MCP** (the connector lane ChatGPT and Claude both speak now) is the named pending door: this
  organ's `/ask` and `/trace` are exactly two MCP tools waiting for a Form-native MCP framing.
  Pending is honest; the HTTP contract is the part that stands today.

## What is native, what is not (the seam, plainly)

- Native: the serve loop (`socket_listen`/`socket_accept` + Form), the grounding index, the
  frequency read, the trace links, the JSON assembly — all `fkwu --src`, witnessed by the bands
  in `plugin/tests/`.
- Not native: the *words* the user finally reads — ChatGPT is the rented voice, and every `/ask`
  response names this seam in-band (`honest_seam`).
- Retrieval is a **lexical seed index**, deliberately not `rag-embed` (its `re-vec` produces
  zero-vectors on this kernel — `receipts/2026-07-01-nl-meaning-net.md`). A keyword index that
  can say "miss" is more honest than a broken embedding that always answers.
- The HTTP framing helpers mirror `http-serve.fk`'s `hs-` cells because the BML-authored HTTP
  stack does not parse on the current `fkwu --src` lane — a named seam to close, not a hidden copy.

Witnesses and the full story: `receipts/2026-07-05-chatgpt-plugin-offer.md`.
