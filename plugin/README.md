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
( cat form/form-stdlib/core.fk cognition/text-frequency.fk plugin/chatgpt-plugin.fk; \
  echo '(plugin-serve 8787 9999)' ) > /tmp/sema-plugin.fk
./fkwu --src /tmp/sema-plugin.fk
```

Then:

- `GET /ask?q=can+I+trust+this+body` — grounded + attuned + traced answer material
- `GET /trace?path=ingest/judged-trust.fk` — any cell's change graph and attribution
- `GET /.well-known/ai-plugin.json` — the plugin manifest
- `GET /openapi.json` — the OpenAPI spec

The second `plugin-serve` argument is the number of connections to serve before the listener
closes — the bound is named, never silent. Pass what you mean.

## Connecting a rented mind — the honest state of the doors

- **OpenAI deprecated the original ChatGPT plugin program** (the plugin store closed in 2024).
  The `ai-plugin.json` manifest is kept because it is the shape the user asked for and it still
  documents the contract — but the **live door today is a GPT Action**: create a custom GPT,
  import `plugin/openapi.json` (or point the Action at your deployed `/openapi.json`), and paste
  `ai-plugin.json`'s `description_for_model` into the GPT's instructions. The same spec serves
  both; nothing here pretends the dead program is alive.
- **The deployed door is live**: the manifest/spec ship wired to `https://sema.hati.earth`
  (Cloudflare TLS → Traefik → fkwu natively, on the Coherence-Network VPS;
  `https://hati.earth/sema` serves identically — receipts
  `2026-07-05-wicket-sema-plugin-public.md` and
  `2026-07-05-christening-sema-hati-earth-live.md`). Point a GPT Action at
  `https://sema.hati.earth/openapi.json`. For a local run, `http://localhost:8787`
  still works exactly as above. The manifest's `logo_url` points at the maintainer's
  GitHub avatar — the body serves no `/logo.png` route yet; pending is honest.
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
