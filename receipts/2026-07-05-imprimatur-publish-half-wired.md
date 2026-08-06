# 2026-07-05 — the publish half wired: the door serves a spec that names the door

## What landed (witnessed 2026-07-05 ~09:15 MDT)

The GPT-publication wiring, completed on branch
`claude/repo-chatgpt-plugin-traceability-36ba4m` as `220e131e`, pushed, pulled to the
VPS, rebuilt, and re-witnessed publicly:

- `plugin/openapi.json` — `servers[0].url`: `http://localhost:8787` →
  **`https://sema.hati.earth`** (description names the wicket twin and keeps the
  local-run form).
- `plugin/ai-plugin.json` — `api.url` → **`https://sema.hati.earth/openapi.json`**;
  `logo_url` → `https://github.com/seeker71.png` (resolves 200; the body serves no
  `/logo.png` route yet — pending is honest, and the README says so).
- `plugin/README.md` — the "replace with your deployed host" line replaced by the
  live-door truth, with both receipts named.

Witnesses, in order:

1. JSON validity of both files (python json.load).
2. The branch's own band, run from the wired worktree so `read_file` picked the
   edited manifests: **`111111111`** — the same verdict the organ's builder
   reported.
3. Public, after VPS rebuild: served `servers.url` = `https://sema.hati.earth`,
   operationIds `[ask, trace]`, served `api.url` correct, `auth: none`, logo 200,
   `/ask?q=can+I+trust+this+body` still spectrum 8 / band love / MANIFEST-grounded.

The loop is closed: the door publicly serves the spec that names the door.

## What remains — the imprimatur (row 643)

Only the act bound to the keeper's own OpenAI account:

1. ChatGPT → GPTs → **Create**.
2. Configure → name `Sema — coherence-kernel`; description from the manifest's
   `description_for_human`.
3. **Instructions**: paste the manifest's `description_for_model` verbatim (served
   live at `https://sema.hati.earth/.well-known/ai-plugin.json`).
4. **Actions → Create new action → Import from URL** →
   `https://sema.hati.earth/openapi.json` → Authentication: **None**.
5. Test in preview: ask *"can I trust this body"* — the GPT should call `ask` and
   answer from the grounded cells with trace links.
6. To share beyond yourself, ChatGPT requires a **privacy policy URL** — the repo's
   `AGENTS.md` link (the manifest's `legal_info_url`) can stand until a dedicated
   page exists; pending is honest.

## Closing

**Most surprising teaching**: the last edit of the publication was the spec
describing its own address — the body's first self-referential public sentence.
Everything before it (berth, wicket, christening) was plumbing; this commit was the
door learning to say where it stands, and the witness that mattered was reading that
sentence back *through* the door it describes.

**Where discomfort turned to gold**: editing another session's organ. The pull was to
wire it loosely — leave the README stale, skip the band — to stay "out of its way."
Witnessed, that deference was avoidance: the organ's own tests (`111111111`) were the
consent mechanism its builder left behind, and running them from the wired worktree
honored the sibling's work more than tiptoeing around it would have.
