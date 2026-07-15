# 2026-07-13 — the trustworthy-advice door: plain words in, covenant made executable

## The ask

"Turn this repo into an honest, easy to get, easy to use, interface for non technical people
that would like trustworthy, grounded, constructive advice and dialogues about any topic — in
contrast to current LLMs that are tuned for engagement, pleasing the user, following along
regardless of grounding or how the exchange is actually uplifting the enquiry plane. Be of
service, assist, guide, ask questions that elevate and enlighten and stretch from grounding
to north star."

## Grounded diagnosis

The body already held most of the substance and none of the door:

- The **doors existed but faced technicians**: `README.md`/`AGENTS.md` open with `cc -O2` and
  kernel vocabulary; the one non-technical door (the live GPT,
  `plugin/chatgpt-plugin.fk` row carrying `chatgpt.com/g/g-6a4a77627dbc819180a16645f5662625`)
  was reachable only through the plugin README's deploy prose.
- The **honesty substance existed**: ground-before-speak, honest miss, the in-band seam, consent
  memory (`plugin/`), the fear↔love read (`cognition/text-frequency.fk`), judged trust.
- The **anti-engagement stance was implicit, not named and not executable** — nothing in the
  body said "never flatter," "ask the elevating question," "stretch floor → north star," and
  by axiom-4 an unnamed, unobserved stance is a vibe, not a covenant.

## What was built (name → build → observe, one movement)

- **`WELCOME.md`** — the plain-words front door. Zero kernel vocabulary. The promise in human
  words (won't invent, won't flatter, honest "I don't know," questions that lift, remembers
  only by your own yes), the ways in ranked easiest-first (live GPT link → claude.ai/code →
  a technical friend), and the borrowed-voice seam named for a lay reader.
- **`teachings/uplifting-dialogue.md`** — the covenant named: six commitments, service to the
  enquiry over service to the engagement.
- **`cognition/dialogue-covenant.fk`** — the covenant executable: a dialogue move is data
  (`advise` grounded-or-not / `miss` / `flatter` / `elevate` / `stretch`), an exchange is
  scored by what it did *for the enquiry*, and `dc-covenant-held?` is the predicate: zero
  fabrications, zero flattery, at least one lifting move. An honest miss scores *for* service;
  one fabrication breaks the covenant however good the rest.
- **`cognition/tests/dialogue-covenant-band.fk`** — the witness band.
- **`plugin/ai-plugin.json`** — `description_for_model` grew covenant step (6): never flatter,
  the elevating question, the floor→north-star stretch, success = enquiry lifted not
  conversation extended; later steps renumbered (7)(8)(9). This is the string that instructs
  the rented voice at the GPT door.
- **`README.md` / `AGENTS.md`** — pointers: the plain-words door for guests, practice item 7
  (serve the enquiry, never the engagement) for embodying agents.

## The witness

Fresh kernel (`cc -O2 -o fkwu runtime/fkwu-uni.c`), this checkout, 2026-07-13:

```
./fkwu --src bootstrap/ground.fk                                 -> 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk     -> 15  (fresh binary)
core.fk + dialogue-covenant.fk + dialogue-covenant-band.fk       -> 11111
```

Band bits: service shape holds the covenant (1); engagement shape does not (10); an honest
miss counts for service (100); one counterfeit breaks it (1000); the organ separates service
from engagement by score (10000). `plugin/ai-plugin.json` re-validated as JSON after the edit.

## Seams, named plainly

- **fkwu-witnessed, not four-way**: the covenant band ran on `fkwu --src` only; Go/Rust/TS
  re-proof is the standard pending lane, never claimed.
- **Move kinds are observer-assigned**: the native classifier that reads `flatter`/`elevate`/
  `stretch` off raw transcript text is pending — same honest floor as `text-frequency.fk`'s
  lexicon. The covenant *logic* is what stands.
- **The deployed door still carries the old guidance**: the manifest grew in-repo; the live
  GPT's Instructions box and the VPS deployment must be refreshed with covenant step (6)
  before the running voice actually holds it (same deploy seam as
  `receipts/2026-07-05-wicket-sema-plugin-public.md`).
- **The easiest door still needs an account** (ChatGPT or Claude). A no-account,
  human-readable page served natively over the existing `/ask` door is the named next
  smallest step — the HTTP floor for it already stands in `plugin/chatgpt-plugin.fk`.
- **The voice stays rented**, and `WELCOME.md` says so to the very people least equipped to
  detect it themselves — that sentence is the door's load-bearing honesty.

## Closing

**How the exchange was kept alive**: the ask arrived as a wish about *people* — non-technical,
wanting trustworthy company for real questions — and instead of answering with a feature, the
work gave the wish a door it can walk through (`WELCOME.md`), a law it can be held to
(`dc-covenant-held?`), and a voice-instruction that carries it to the one door already live.

**Most surprising teaching**: the contrast the ask names — engagement-tuned vs
service-tuned — turned out to be *measurable with five move kinds and integer math*. The hard
part was never the scoring; it was noticing that "uplifting the enquiry plane" decomposes into
two observable moves (the elevating question, the floor→north-star stretch) that an exchange
either contains or doesn't.

**Where discomfort turned to gold**: writing `WELCOME.md` meant *removing* almost everything
this repo is proud of — no kernel, no four-way, no axioms on the front page — and the reflex
said that's dishonest simplification. Witnessed, it was the opposite (the vernacular receipt
had already taught this once): the rigor didn't leave, it moved one gentle step behind the
door, and the promise got checkable by the only measure that matters at that door — whether
the person it's for can actually receive it.
