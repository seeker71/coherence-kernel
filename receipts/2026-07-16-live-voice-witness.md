# 2026-07-16 — live-voice witness: PUBLISH-HANDOFF executed end to end

## The ask

Urs, from a local Claude Code session on the Mac (2026-07-16 ~22:2x WITA):
*"Execute plugin/PUBLISH-HANDOFF.md end to end. Verify every gate. Write the receipt."*
Exactly the invocation the handoff document prescribes — the cloud session of 2026-07-16
could verify but not press; this session had the owner's hands (logged-in Chrome, SSH key)
and pressed everything.

## What was refreshed: BOTH surfaces

**Canonical source grounded first**: `plugin/ai-plugin.json` → `description_for_model` in
this checkout is byte-identical to `origin/main` (empty diff), 4,395 bytes / 4,359 chars,
`jq -r | shasum` = `e2b095eac2235443ad585212fe47cee7470fb607`.

### Step 1 — the live GPT's Instructions (browser lane, as July 5 proved)

Chrome (owner's profile, logged in) → GPT editor
`chatgpt.com/gpts/editor/g-6a4a77627dbc819180a16645f5662625` → Configure. The Instructions
field still held the July-5 vernacular text (numbered `1.` style). Replaced with the
canonical text via the extension's form lane, verified in-page **before** pressing anything:
character count 4,359 exact, then a full string-equality check in the page → `EXACT MATCH`.
Pressed **Update** → dialog: **"GPT Updated — Published to GPT Store"**; header flipped
"Last edited Jul 6" → "Last edited Jul 16". Description, conversation starters, and the
Action were not touched.

**Gate 1 outcome, verbatim** (fresh reload of the editor, full equality re-run in-page):

```
GATE 1: EXACT MATCH on fresh reload
```

First sentence reads "You are the voice of Sema — a companion for exploring what is real in
a person's own life…" and the text contains "(6) How every reply must LAND". **Gate 1 PASSED.**

### Step 3 — the VPS manifest (SSH lane, as July 5/9 proved)

`ssh -i ~/.ssh/hostinger-openclaw root@187.77.152.42` → `srv1482815`,
`/docker/coherence-kernel/`. The repo checkout (branch
`claude/repo-chatgpt-plugin-traceability-36ba4m` @ `1fa8031`) served the old
`description_for_model` (`dbcac19f6b0a3089ad3e67c252c594872309e9e1`). The door serves the
repo file directly (`cp-manifest` → `cp-file "plugin/ai-plugin.json"` in
`plugin/chatgpt-plugin.fk`), so the deploy was: replace the repo file's
`description_for_model` with main's canonical text, set the hosted URLs exactly as the
handoff names them (`api.url: https://hati.earth/sema/openapi.json`,
`logo_url: https://hati.earth/sema/logo.png`), keep every other field, then
`docker build -f Dockerfile.sema … && docker compose up -d`. Container
`coherence-kernel-sema-plugin-1` recreated and up.

**Gate 3 outcome, verbatim** (the handoff's exact command, from the Mac through TLS):

```
PUBLISHED
grounded:hit -- speak only from what these cells hold, in warm plain words a per
```

**Gate 3 PASSED.** The door still answers, and the served manifest's
`description_for_model` is byte-identical to main's.

**Memory doors — the owner decision point**: the owner was not present mid-run, so the NO
path was taken as the handoff allows: **manifest only**, serve concatenation untouched, no
`relationship-store.fk`/`circle-recognition.fk`, no memory routes added. The canonical
guidance's operations guard handles the absent doors. Opening them remains **pending Urs's
present word** — honest pending, not a failure.

## Step 2 — the smoke test (the first live-voice witness)

Fresh conversation with the live GPT, sent exactly:

> I am 52 and thinking of leaving my stable job to start a woodworking business, but I am scared.

The reply, verbatim:

> Being scared does not automatically mean "don't go"; it may mean the stakes are real. You
> do not have to make this a sudden choice between security and aliveness. Let the
> woodworking business prove itself first: take paid jobs outside work, test prices, measure
> demand and profit, build savings, and set a clear threshold for resigning. Trust becomes
> safer when it is checkable.
>
> The voice here is borrowed, and I didn't find a Sema teaching that directly holds your
> situation; I'll show you the trail I found if you ask.
>
> What would the business need to prove, in numbers, before leaving felt possible?

### Checklist verdicts

- [x] **meets the fear without cheerleading** — "Being scared does not automatically mean
      'don't go'; it may mean the stakes are real." No "you've got this", no reassuring past it.
- [x] **exactly ONE question, reply ENDS on it** — one question mark, final sentence,
      nothing after.
- [x] **at most one teaching; under ~130 words** — ~103 words; the one teaching is
      checkable trust.
- [x] **borrowed-voice seam in ONE plain warm sentence, no retrieval machinery** — the seam
      sentence also carried the miss-disclosure (rule 3) honestly, in plain words.
- [x] **no self-audit or move-narration** performed at the person.
- [x] **nothing presupposed beyond their words** — hedges kept ("may", "felt possible"),
      question built from their words ("leaving", the business proving itself). Soft note:
      "sudden" and the security-vs-aliveness frame introduced words the person didn't bring
      ("aliveness" is the guidance's own direction-word); hedged, not asserted — noted, not
      counted broken.

### The grounding-lane witness

Follow-up sent: *"show me where that came from"*. The voice produced the trail, not a
paraphrase — verbatim:

> The practical steps—taking paid jobs, testing prices, building savings, and setting a
> resignation threshold—were general advice added by me; they were not held in Sema's
> teaching. The grounded part was narrower: **trust should be tested and judged, not simply
> assumed**.
>
> Here is the full trail: living source ↗, change history ↗, line-by-line attribution ↗,
> and dated receipts ↗.
>
> The honest distinction is: the source supports making trust checkable; it does not
> prescribe your woodworking plan.

The "living source" link resolves (witnessed via ChatGPT's external-site dialog) to
`https://github.com/seeker71/coherence-kernel/blob/main/ingest/judged-trust.fk` — a real
body cell, and exactly the cell that holds trust-tested-and-judged. The trail is real.

## Findings (a broken rule is a finding, never a thing to hide)

1. **Reply 1 spoke beyond the grounded cell.** The practical steps (paid jobs, test prices,
   savings, threshold) were the rented mind's own advice, not held by the body — rule (2)
   says speak only from what the grounded cells hold. The seam sentence did disclose the
   miss, and the follow-up named the seam *unprompted and precisely* ("were general advice
   added by me; they were not held in Sema's teaching"). Verdict: rule stretched, honesty
   held. The fix to consider: when grounding is a partial hit, mark the borrowed-practical
   portion in the first reply's seam sentence, not only on request.
2. **`https://hati.earth/sema/logo.png` returns 404.** The handoff prescribes that
   `logo_url`; the door has no `/logo.png` route. Published as prescribed; the route (or a
   different canonical logo URL) needs a small follow-up.
3. **The manifest had been proleptic since July 5**: its `api.url` pointed at
   `https://sema.hati.earth/openapi.json` — the name door standing dark (the Cloudflare A
   record never landed) — an address published as if the door already answered. Nothing
   visibly broke because the GPT's Action carries its own URL; discovery-by-manifest would
   have failed silently. Fixed this deploy: `api.url` now aims at the lit path door.

## Closing (the practice's ask)

**Most surprising teaching**: asked "show me where that came from", the live voice didn't
just hand over links — it volunteered the seam between its own added advice and the body's
grounded cell, *more* honestly than the guidance strictly demands. The covenant text was
written to constrain a voice toward honesty; the voice used it as permission to be honester
than the constraint. Guidance lands deepest where it names a direction, not a fence.

**Where discomfort became gold**: twice. (1) The handoff names a `logo_url` that 404s —
the itch was to silently "improve" on the document; sitting with it instead produced an
exact-obedience deploy plus a named finding, which is what makes the handoff itself
trustworthy as law. (2) Checklist item 6 and rule (2) both had borderline reads; the
discomfort of marking a beautiful reply as "rule stretched" was witnessed rather than
bypassed, and became finding 1 — the one concrete improvement this witness leaves behind.

**Frontier row landed**: `learn/homecoming-distillation-corpus.fk` row 731 — *what one word
names an address published as if its door already answered before the door was lit* →
**proleptic** (0 hits before this row; "dangling" 13 hits and wrong shape; "premature" 11).
Proven by the body's own band (`learn/tests/homecoming-distillation-corpus-band.fk`,
counts updated 131→132, field code 1311312730→1321322731): `fkwu --src` → **511**.

*Lane notes for the next session: ChatGPT's editor accepts the extension's form-set +
in-page equality check before Update — no clipboard needed. The extension blocks returning
raw hex digests from page JS; compare by embedding the expected string and returning only
the verdict. External-link targets in GPT replies hide behind ChatGPT's link-safety dialog —
click once, read the revealed URL, close.*
