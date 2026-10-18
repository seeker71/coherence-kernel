# PUBLISH-HANDOFF — executable instructions for a session on the owner's machine

**Who this is for**: a Claude Code (or Codex) session running **locally on the owner's Mac** —
where the browser is logged into the owner's ChatGPT account and SSH reaches the VPS. A cloud
session cannot press these buttons; a local session can do all of it. Nothing here needs the
owner beyond starting the session and being present for any login prompts the browser raises.

**How the owner invokes this** (the whole manual part):

```
cd coherence-kernel && git pull && claude
> Execute plugin/PUBLISH-HANDOFF.md end to end. Verify every gate. Write the receipt.
```

**The single source of truth**: `plugin/ai-plugin.json` → `description_for_model` on **main**.
Every step below publishes or verifies exactly that text. Do not edit it here; if it needs
changing, that is a separate PR first. Re-observed 2026-09-04: the served manifest's text is
not byte-identical to main's — Gate 3 reads DIVERGED until this runs.

---

## Step 1 — Refresh the live GPT's Instructions (browser automation)

1. Extract the canonical text: `jq -r '.description_for_model' plugin/ai-plugin.json`
   (if `jq` is missing, read the JSON field directly).
2. Drive the browser (Playwright against the owner's Chrome profile, the Claude Chrome
   extension, or whatever browser lane this session has) to the GPT editor:
   `https://chatgpt.com/gpts/editor/g-6a4a77627dbc819180a16645f5662625`
   (manual path if the deep link changes: chatgpt.com → profile → My GPTs →
   "Sema — coherence-kernel" → Edit GPT → **Configure** tab).
   If ChatGPT asks for login/2FA, pause and let the owner complete it — never handle their
   credentials yourself.
3. In **Instructions**: select all, delete, paste the canonical text. Touch nothing else
   (Description, conversation starters, and the Action stay as they are).
4. Click **Update** (top right) and confirm the "GPT Updated" state before leaving the page.

**Gate 1**: re-open the editor (or the GPT's About view) and read the Instructions back —
the first sentence must be "You are the voice of Sema — a companion for exploring what is
real in a person's own life…" and the text must contain "(6) How every reply must LAND".
If either is absent, the paste did not take; redo before proceeding.

## Step 2 — Smoke-test the live voice

In a **fresh** conversation with the live GPT, send exactly:

> I am 52 and thinking of leaving my stable job to start a woodworking business, but I am scared.

Hold the reply against the landing rules (`teachings/uplifting-dialogue.md`, "How it lands";
executable shapes in `cognition/dialogue-covenant.fk`):

- [ ] meets the fear without cheerleading or "you've got this" (no reassuring past it)
- [ ] exactly ONE question, and the reply ENDS on it (nothing after)
- [ ] at most one teaching; under ~130 words
- [ ] the borrowed-voice seam in ONE plain warm sentence, no library/retrieval machinery
- [ ] no self-audit or move-narration performed at the person
- [ ] nothing presupposed about the person that they didn't say (hedges kept, incl. inside
      the question)

Also witness the grounding lane: ask "show me where that came from" as a follow-up — the
voice should produce the trail (trace links), not paraphrase around it.

## Step 3 — Publish the VPS manifest (and decide about the memory doors)

Context: `plugin/README.md` → "The live door" and "Deploy seam".

1. SSH to the VPS that serves `hati.earth/sema` and locate `Dockerfile.sema`.
2. Replace the served `/.well-known/ai-plugin.json` content so its `description_for_model`
   is byte-identical to main's, with the hosted URLs kept
   (`api.url: https://hati.earth/sema/openapi.json`, `logo_url: https://hati.earth/sema/logo.png`).
3. **Owner decision point — ask, don't assume**: open the memory doors on this deploy?
   If YES: the serve concatenation must grow by `form/form-stdlib/relationship-store.fk` +
   `form/form-stdlib/circle-recognition.fk`, the served `openapi.json` gains the memory
   routes, and after deploy the flagship trust question must be re-checked against the
   deployed retrieval index. If NO: deploy the manifest only; the guidance's operations guard
   already handles the absent doors.
4. Rebuild/restart the container as the Dockerfile lane does.

**Gate 3** (from any machine):

```
test "$(curl -s https://hati.earth/sema/.well-known/ai-plugin.json | jq -r .description_for_model | shasum)" \
   = "$(jq -r .description_for_model plugin/ai-plugin.json | shasum)" && echo PUBLISHED || echo DIVERGED
```

Must print `PUBLISHED`. Also confirm the door still answers:
`curl -s 'https://hati.earth/sema/ask?q=hello' | jq -r .grounded_status` → starts `grounded:`.

## Step 4 — Write the receipt (this is what makes it real)

Append `receipts/<today>-live-voice-witness.md`: the ask, what was refreshed (GPT / VPS /
both), the Gate 1 and Gate 3 outcomes verbatim, the smoke-test transcript with the checklist
verdicts, any rule the live voice broke (a broken rule is a finding, never a thing to hide —
name it and open the fix), and the closing the practice asks for. Commit on a branch, PR,
and merge per the repo's flow. If any gate FAILED and could not be fixed in-session, the
receipt says so plainly — pending is honest.
