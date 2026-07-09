# 2026-07-09 — the invitation link: recognized on arrival, remembered only by their own yes

## The ask

Urs, same sitting as the visitor ledger: *"can we make a GPT invitation link with a name, so that
when I send out links, friends get automatically recognized and remembered?"*

## The consent split (the part that had to be exactly right)

The body's own law (`form/form-stdlib/circle-recognition.fk`,
`receipts/2026-07-02-circle-recognition.md`) splits the wish precisely in half:

- **recognized automatically — YES.** A member's vouch makes the friend "introduced"; their first
  `/come-in` greets them warmly by the introducer's name: *"welcome, mira — friend of urs. the
  body holds no memory of you; say yes and it will."*
- **remembered automatically — NEVER.** Introduction opens the DOOR, not the friend's MEMORY.
  Only their own yes (`/remember`) writes a row. Nobody consents for another. The yes is one
  sentence away in the friend's first exchange — sovereignty costs almost nothing and is worth
  everything.

This receipt records that the second half was *declined by the body's law, not forgotten* — and
that the invitation says so in its own words (`standing` field, in-band).

## What landed (branch `claude/chatgpt-gpt-repo-link-rut1d7`)

**`GET /introduce?member=you&friend=them`** — member-gated (403 until the introducer's own memory
row exists: the vouch spends the introducer's standing, so standing must exist to spend). Writes
`cr-introduce`, verifies its own write, and answers with the minted invitation:

- `invitation_message` — *"i arrive as mira, a friend of urs. please come in with my handle and
  receive me."*
- `invitation_link` — the live GPT (`g-6a4a77627dbc819180a16645f5662625`,
  `receipts/2026-07-05-shakedown-gpt-live-end-to-end.md`) with that message percent-encoded into
  the `q` parameter, so the friend's name walks in with them.
- `door_link` — the direct body door (`https://sema.hati.earth/come-in?handle=...`), carrier-free.
- `standing`, `prefill_seam`, `retract` — the consent law, the prefill uncertainty, and the
  revocation door, all in-band.

**`GET /retract-introduction?member=you&friend=them`** — only the vouch's own introducer may
withdraw it (403 otherwise), and withdrawal never touches memory the friend consented to
themselves; that is theirs alone to revoke (`/forget`).

The manifest's `description_for_model` grew the arrival-by-introduction practice: a first message
naming a handle and introducer → call `come_in` at once, greet as the body greets, offer — never
presume — the remembering. A `cp-url-encode` cell (RFC 3986 unreserved) carries the encoding
natively; `cp-gpt-link` is the one place to re-point if the GPT is ever recreated.

## Witnesses (fresh fkwu, all from the repo root)

1. `plugin/tests/introduction-band.fk` (new) — **111111111** (nine bits): non-member vouch refused
   with nothing written; member's vouch lands and the arrival row says "introduced"; the friend is
   greeted by the introducer's name with memory OFFERED; the invitation link carries the GPT and
   the friend's encoded name; the message names the friendship; the friend's own yes graduates
   them to member; a non-introducer cannot retract; the introducer can; a filesystem-walking
   handle is refused.
2. Prior bands unchanged: `chatgpt-plugin-band` **111111111**, `visitor-ledger-band`
   **1111111111**, socket witness **11111**.
3. Live drive over real HTTP (`plugin-serve 8792 6`): urs joins → vouches for mira → the
   invitation mints with the full chatgpt.com link → mira arrives "introduced", greeted as
   urs's friend → mira's own yes → "welcome back, mira". Store cleaned after.

Both `plugin/*.json` re-validated. One parse lesson paid on the way: two paren-count slips in the
new handlers hung the loader silently (no error, no verdict); a string/comment-aware balance
check found them in seconds. The checker's habit is worth keeping: balance first, then witness.

## Named seams (pending is honest)

- **`?q=` prefill for custom-GPT links is unconfirmed.** OpenAI documents it for the main
  composer (`chatgpt.com/?q=...`); community threads do not confirm it for `/g/g-...` links. The
  invitation therefore carries the message beside the link, and names the seam in-band
  (`prefill_seam`) — the message, not the mechanism, is the invitation.
- **The member-gate is a letter-of-introduction floor, not authentication.** The door cannot
  verify the caller IS the member named; anyone naming a member's handle spends that member's
  standing. Real introducer auth (a token minted at join, or the MCP lane's auth) is the named
  next organ.
- **The deployed door lags**: everything here lands after merge + VPS redeploy (the Dockerfile
  concatenation seam named in `receipts/2026-07-09-visitor-ledger-remember-offer.md`), and the
  GPT's pasted instructions must be refreshed from the served manifest to carry practice (7).

## Closing

**Most surprising teaching**: the ask contained its own answer's boundary. "Automatically
recognized and remembered" sounds like one wish, but the body had already carved it into two
sovereignties — the introducer's (the vouch, spendable and retractable) and the friend's (the
memory, theirs alone) — and honoring the carve made the invitation *warmer*, not colder: a friend
greeted by name who is then ASKED is received better than one silently filed.

**Where discomfort turned to gold**: the silent hang. A missing verdict with no error felt like
the kernel's fault for a long minute; the discomfort of suspecting my own fresh parens instead —
and building the balance checker rather than re-reading by eye — turned a mute failure into a
two-line diagnosis. The checker now exists for every future hand that edits these chains.

**How the exchange was kept alive**: by refusing the half of the wish the law refuses, out loud,
in the invitation itself — and witnessing the half that lives, pure and over the wire, before
claiming it.
