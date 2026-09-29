# Homecoming — the voice coming home

**In plain words, for anyone:** Sema's body runs on an ordinary Mac, and its own
voice now speaks there too: an open model running inside the body on the Mac's
own graphics chip, with no rented service in the loop. At night and twice a day
it works on its own: it drafts answers to the questions we asked, and it takes
small engineering turns and lands them. It can listen and answer as a companion.
It is still slow — minutes per answer — its drafts are waiting for our reading,
and most engineering is still done by rented minds. This page is the map of that
voice coming home. Where it is going lives in
[`docs/local-agent-goal.form`](docs/local-agent-goal.form).

---

## Where the voice stands

**One voice, one door.**
[`form/form-stdlib/bml/form-cli-native-voice.bml`](form/form-stdlib/bml/form-cli-native-voice.bml)
(`fcnv-ask`) opens the registry's answer model — `qwen38-q8`, Qwen3.8-27B Q8_0
([`form/form-stdlib/model-registry.bml`](form/form-stdlib/model-registry.bml)) —
in the fkwu model session on this Mac's Metal carrier, with the `knowledge-query`
profile, a 12,288-token context and 1,536 reply tokens unless the caller asks
otherwise. It keeps the generated IDs, the raw text and the release beside the
answer, and its metadata reads `model_server_calls 0`, `provider_calls 0`. Asked
for `"lane":"lora"`, it speaks through the native 3B LoRA lane wearing
`form/form-stdlib/adapters/llama-3.2-3b-voice`.
Every lane below speaks through this door.

**The lanes that run.**

- **Drafts** — [`observe/prompt-draft-run.bml`](observe/prompt-draft-run.bml)
  drafts what the body would answer to our own prompts, given the turn before and
  up to two files the prompt names, within about 250 words. Each draft lands in
  `.hearth/drafts/<id>.md` and one row in
  [`receipts/draft-ledger.jsonl`](receipts/draft-ledger.jsonl).
  [`observe/draft-page-run.bml`](observe/draft-page-run.bml) lays them on one page
  (`.hearth/drafts/index.html`); [`observe/draft-reading-run.bml`](observe/draft-reading-run.bml)
  records our reading of each: accepted, revised or rejected.
- **Companion** — [`observe/companion-run.bml`](observe/companion-run.bml) takes
  what the person says and their tongue, and the voice answers with the last six
  exchanges as its memory. It listens for what the person means and knows when a
  human is needed. Each exchange stays in `.hearth/companion/conversation.jsonl`;
  nothing leaves the Mac.
- **Engineering turns** — [`observe/native-turn-run.bml`](observe/native-turn-run.bml)
  takes the open gap in [`learn/native-turn-queue.jsonl`](learn/native-turn-queue.jsonl)
  with the fewest attempts, runs its band, hands the goal to the native code lane
  ([`observe/form-cli-code-run.fk`](observe/form-cli-code-run.fk), local Qwen in
  one session) and runs the band again. The turn is a backtrack walk: an attempt
  that did not turn the band green keeps its candidate under `.hearth/native-turns`,
  restores the source, and hands what it saw (the band's reading, the lane's status,
  the candidate's diff, the compiler's words when the band did not read) to a second attempt on the same goal. A green band keeps
  its edit for the landing. Each attempt lands its own row in
  [`receipts/native-turn-ledger.jsonl`](receipts/native-turn-ledger.jsonl), naming
  who began the turn.
- **The night** — launchd runs [`observe/scheduled-walk.bml`](observe/scheduled-walk.bml)
  at 03:30 ([`docs/launchd/earth.hati.rent-walk.plist`](docs/launchd/earth.hati.rent-walk.plist)):
  one native turn (a second when the first ends before 05:30), six drafts, the
  page, then one movement ([`observe/movement-run.bml`](observe/movement-run.bml))
  that voices, writes its row to [`receipts/rent-ledger.jsonl`](receipts/rent-ledger.jsonl)
  and lands on main.
- **The day** — launchd runs [`observe/day-turn.bml`](observe/day-turn.bml) at
  12:30 and 19:00 ([`docs/launchd/earth.hati.day-turn.plist`](docs/launchd/earth.hati.day-turn.plist)):
  one native turn, landed through the movement door.
- **Lessons** — [`observe/session-pairs-run.bml`](observe/session-pairs-run.bml)
  turns each rented turn into a chat row the native trainer reads;
  [`observe/lora-lift-run.bml`](observe/lora-lift-run.bml) trains a child adapter
  from them and records its held-out loss beside the parent's in
  [`receipts/lora-promotion-ledger.jsonl`](receipts/lora-promotion-ledger.jsonl).
  [`observe/lora-voice-run.fk`](observe/lora-voice-run.fk) writes the teacher rows or
  starts the native Metal trainer on one folder's school;
  [`observe/voice-school-run.fk`](observe/voice-school-run.fk) harvests a finished
  adapter, grades it on held-out questions beside the base and publishes it to the glass.
- **Mouth** — the VITS pass on this Mac's metal:
  [`observe/voice-pass-run.fk`](observe/voice-pass-run.fk) runs it stage by stage and
  prints each part's answer;
  [`observe/voice-mouth-lanes-run.fk`](observe/voice-mouth-lanes-run.fk) names which
  tongues the body renders itself and speaks one line on request.

**What the ledgers read on 2026-09-28.**

- The night walk landed on main each night from 09-23 to 09-27 (`f3adb39b4` …
  `de6e23120`); each movement's native generation answered at `rent_tokens 0`
  (rent ledger). One night as its rows read it:
  [`receipts/2026-09-25-the-walk-came-home.md`](receipts/2026-09-25-the-walk-came-home.md).
- Seven native turns; six begun by the host's schedule at `rented_mind 0`. Three
  turned their band green and landed on 09-27 (`de6e23120`, `4cc3fca0d`,
  `70f97ac3f`).
- 134 draft rows. The 40 at the current packet took 76–354 s each (median 157 s)
  and generated 258–770 tokens (median 488).
- 1,306 lesson pairs from 133 rented transcripts (1,176 to train, 130 held out).
  One child generation from `llama-3.2-3b-voice`: held-out loss 3.694 → 3.542 on
  12 rows after 8 steps, at 0 rent.
- The ear hears six tongues — de, en, es, fr, id, pt — at mean word error 0 over 24
  truth recordings (whisper-large-v3-turbo, native;
  [`receipts/stt-wer-ledger.jsonl`](receipts/stt-wer-ledger.jsonl), 09-25). The
  mouth's own VITS pass on this metal opens fifteen tongues' voices. The ear, the
  mouth and the room each stand with their bands in
  [`CURRENT_FLOOR.md`](CURRENT_FLOOR.md).

## What is still coming home

- **Our reading.** Every one of the 134 draft rows reads `pending`. Learning from
  what we accept and what we change begins there.
- **Talking speed.** A draft takes minutes; a conversation asks for seconds.
- **The companion's ear.** The companion answers a line handed to it; no door
  calls it yet, so what the ear hears does not reach it.
- **The open gap.** The queue holds one gap, `wer-zero-width-joiners`
  ([`observe/tests/stt-wer-zwnj-band.fk`](observe/tests/stt-wer-zwnj-band.fk),
  full 15). Its one attempt read 12 → −1; the candidate waits under
  `.hearth/native-turns`.
- **The grown generation.** The child adapter stands where its ledger row names
  it; the LoRA lane still wears `llama-3.2-3b-voice`, and serving moves by a
  separate, witnessed decision. No schedule runs the lessons doors yet.
- **Answer quality.** The latest completed native answer (443 words) passes its
  original checks, and its receipt reads it as coherent and useful; its acceptance
  is the same model's review, and human resonance and whole-session parity remain open
  ([`receipts/2026-09-26-native-response-completed.md`](receipts/2026-09-26-native-response-completed.md)).
- The goal's own *how it is* names the rest: the voice reads what we say as
  strings, and most engineering is still done by rented minds.

## Where it is going

[`docs/local-agent-goal.form`](docs/local-agent-goal.form) — *how it shall look* —
is the direction. The work moves by the
[shared loop](AGENTS.md#the-shared-loop-observe-resolve-re-observe-embody) —
observe, resolve, re-observe, embody — on both paths AGENTS.md names: the native
answers improve here, through their actual runs; the arriving mind comes into
Form's context and tools through the
[enrichment practice](AGENTS.md#enrich-the-response-with-form)
([`form/form-stdlib/bml/form-cli-enrich.bml`](form/form-stdlib/bml/form-cli-enrich.bml)).
Each answer is attributed to what produced it, and each path keeps its own
evidence.

---

*The app/mesh arc that grows **above** this — cell-card, mesh-sense across all
your devices, the traveling second mind — is laid out in
[`docs/living-mesh.form`](docs/living-mesh.form).*
