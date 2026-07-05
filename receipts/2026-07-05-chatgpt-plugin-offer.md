# 2026-07-05 — the rented-mind door: a ChatGPT plugin that grounds, attunes, and hands over the change graph

## Ground

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

Urs asked for the repo as a **ChatGPT plugin**: answers *grounded* and *frequency-transformed*, each
carrying the offer of **full traceability — a receipt in a link that guides exploration of the change
graph for full attribution** — toward trust over fear, sovereignty, and vitality for all affected
cells and organs. Tended into the body as `plugin/`.

## What stands

`plugin/chatgpt-plugin.fk` — one organ, self-contained over `core.fk` + `cognition/text-frequency.fk`,
served **natively by the c-bootstrapped fkwu** (`socket_listen`/`socket_accept` + Form, the same
streaming shape `http-socket.fk` carries on the BML lane). Four doors:

| door | what it answers |
|------|-----------------|
| `GET /ask?q=…` | the grounded cells (or an honest miss), the fear↔love frequency read **with the attunement the answering voice must carry**, the stance (trust-over-fear *judged* / sovereignty / vitality), and per-cell trace links |
| `GET /trace?path=…` | the receipt-in-a-link for any cell: living source → change graph (every commit) → line attribution → the dated ledger |
| `GET /.well-known/ai-plugin.json` | the plugin manifest; its `description_for_model` hands ChatGPT the same practice AGENTS.md hands an embodying agent |
| `GET /openapi.json` | the contract (also the GPT-Action import) |

The trace is the trust mechanism, made structural: every grounded claim ships with the links to
*verify the attribution yourself* — trust offered as something checkable, never asked for.

## Witnesses

- `plugin/tests/chatgpt-plugin-band.fk` → **111111111** (pure, no socket: route/ground/attune/trace
  arc, miss named as miss, 404 honest, url-decode, manifest served).
- `plugin/tests/chatgpt-plugin-socket-witness.fk` → **11111** (one real HTTP request over 127.0.0.1
  TCP through the organ's own accept→stream→handle→send lane; client bytes == served bytes).
- Live: `plugin-serve 8787 3` + curl —
  `"I am afraid this will all collapse"` → `spectrum -7.2, fear_fraction 1, band fear`, attunement
  toward *judged* trust; `"can I trust this body"` → `band love`, grounded in `MANIFEST.md`,
  `ingest/judged-trust.fk`, `observe/native-vs-rented.fk`, each with its three trace links.

fkwu-witnessed on `fkwu --src`; four-way is **not** claimed (socket/`read_file` are the fkwu surface).

## Honest floor

- **The voice is rented.** The body grounds, attunes, and traces; ChatGPT speaks. Every `/ask`
  response names this seam in-band (`honest_seam`). Native generation stays pending
  (`receipts/2026-06-29-native-zh-summary-PENDING.md`).
- **OpenAI's original plugin program is dead** (store closed 2024). The manifest is kept as the
  asked-for shape and as documentation; the live door is a **GPT Action** over the same
  `openapi.json`. **MCP is the named pending door** — `/ask` and `/trace` are exactly two MCP tools
  waiting for a Form-native MCP framing.
- **Retrieval is a lexical seed index** (12 cells, keyword overlap), deliberately not `rag-embed`
  (its `re-vec` zero-vector gap — `receipts/2026-07-01-nl-meaning-net.md`). An index that can say
  "miss" beats an embedding that always answers.
- **The frequency lexicon is a seed** (~40 charged words over `text-frequency.fk`'s spectrum cells);
  a question with no known word reads `unread`, named rather than guessed.
- The HTTP framing mirrors `http-serve.fk`'s `hs-` helpers because the BML HTTP stack does not parse
  on the current `--src` lane — a seam to close, not a hidden copy. Serve loop is count-bounded and
  serial; `localhost` URLs in the manifest need a real TLS-fronting host to reach ChatGPT.

## What the build taught the body (kernel truths, measured)

1. **An op called with more args than its arity floods the AST table.** `(add 1 2 3)` alone hits
   `FK_AST_NODE_CAP` (262,144 nodes) — the diagnostic says *capacity*, the cause is *shape*. A
   possible future kernel kindness: diagnose arity at the call site instead of minting to the cap.
2. **`nil?` is true for every string** (`(len <string>)` is 0 on this kernel), so `read_file`
   presence must be judged by `str_len`, never `nil?` — `chatgpt-plugin.fk`'s `cp-file` carries the
   note. (`rag-ask.fk`'s `or`-guard tolerates this; a bare `nil?` guard does not.)

## The most surprising teaching this work left behind

Two paren mistakes — one surplus in `cp-ask`, one missing in `cp-welcome` — **cancelled each other
globally**. Every whole-file balance check passed while the structure was wrong, and the parser spent
262,144 nodes trying to recover before saying "too large". A global balance is unjudged trust: the
totals agree, so it *looks* whole. Only the per-form witness (each defn proven to close where it
claims) is the judge. The same law as row 667: agreement without judgment composts.

## Where discomfort turned to gold

The node-table wall arrived wearing the kernel's own suggestion: *raise FK_AST_NODE_CAP* — one C
edit and the pain stops. The discomfort was sitting with "I don't understand why 900 lines need
262K nodes" instead of taking the offered door. Measured (filler-defn calibration: 20,000 plain
defns fit easily), the kernel was exonerated and the fault came home to two of my own parentheses.
The gate held — the C seed did not grow — and the wall turned out to be the body refusing to let a
structural lie be paid for with capacity.
