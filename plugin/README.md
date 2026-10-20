# plugin/ — Sema's hosted companion surface

This directory owns the native HTTP Action used by **Sema — coherence-kernel**
and its canonical companion instructions. The host's model helps people learn,
explore business ideas, research, design, create and collaborate; Form supplies
the evidence and operations its connected tools actually carry.

`ai-plugin.json` → `description_for_model` is the canonical instruction payload.
The GPT editor and the served manifest are separate copies to verify after
publishing. [`PUBLISH-HANDOFF.md`](PUBLISH-HANDOFF.md) carries that operation.

## The observed hosted profile

The owner's GPT editor was read on **2026-10-20**:

- GPT: `https://chatgpt.com/g/g-6a4a77627dbc819180a16645f5662625`.
- Imported Action server: `https://sema.hati.earth`; operations: `GET /ask`
  and `GET /trace`; authentication: None.
- Web Search, Canvas, Image Generation and Code Interpreter were enabled.
- Instructions still carried the earlier counsel-only rules. This source
  update does not establish that the editor or server has been updated.
- The editor displayed a migration-to-plugins notice naming December 11.
  The notice is an observed migration requirement, not a verified package
  format or a completed migration.

Preserve this read-only Action profile when publishing the companion update.
The repository's broader `openapi.json` is a source catalog, **not the approved
hosted profile**; importing it wholesale would add memory and public-write
operations the current GPT does not expose.

## What the native source carries

[`chatgpt-plugin.fk`](chatgpt-plugin.fk) owns the socket serve loop, request
dispatch, lexical seed lookup, frequency hints, trace links and manifest/schema
reads. It imports its dependencies, including
[`public-dialogue.fk`](public-dialogue.fk). `plugin-serve(port, count)` serves a
named number of connections; the deployed startup owner must supply that call
and the current source dependencies. Inspect its actual configuration before
changing a running service.

| Source surface | Evidence and boundary |
|---|---|
| `GET /ask?q=...` | Seed-index matches or an explicit miss, lexical frequency hints, source traces. Send a minimal non-identifying query. |
| `GET /trace?path=...` | Source, change history and attribution links for a body path. |
| Manifest and OpenAPI reads | Committed contract files served from the running checkout; these do not prove the GPT's imported configuration. |
| Handle-based circle routes | Local relationship-store operations without caller authentication; they are not private family memory. |
| Public dialogue routes | Deliberately unlisted public turns, directed replies and per-turn removal capabilities; these are not imported in the current Action. |
| `POST /mcp` | A partial JSON-RPC dialogue bridge, described below. |

The grounding index is lexical and small. A hit establishes the returned
source's relevance, not a complete answer or a measure of someone's feelings.
A miss leaves the host free to help through its own clearly attributed
reasoning and available tools. Ordinary task outputs need their requested
scope; reflective counsel can stay brief without imposing that shape on every
artifact or research answer.

## Memory and public sharing

The manifest declares `auth: none`. The source memory and introduction routes
use caller-supplied handles; membership checks do not authenticate the caller.
Keep them unadvertised in the hosted companion until authenticated ownership,
per-person isolation and revocation have been observed. Gitignored files and
an unlisted link are not access-control guarantees.

The source arrival ledger omits names, query text and addresses from its own
rows. That property does not establish what the chat host, proxy, backups or
other services retain. A successful deletion describes the store and fields
actually changed, not erasure of copies everywhere.

Public dialogue creation requires `public-unlisted-thread-v2` acknowledgement.
It returns a per-turn removal token that must stay with the author. Removal
leaves a content-free tombstone; connected public turns can remain. Current
source contracts are in `public-dialogue.fk` and `openapi.json`. Publishing,
saving a personal preference and authorizing a learning sample remain distinct
actions.

## Plugin migration prerequisites

The current `/mcp` source supports `tools/list` and `tools/call` for five public
dialogue tools. It has no implemented `initialize` negotiation in this owner
and does not list `ask` or `trace`. A successful JSON-RPC call alone does not
establish an installable MCP integration.

Before migrating the hosted GPT, verify the target platform's current package
and transport contract, complete the lifecycle in the existing native owner,
and prove a connection with the target client. Preserve the read-only
`ask`/`trace` scope first. Public writes, authentication and private storage
are separate capability changes with their own evidence. The legacy
`ai-plugin.json` filename by itself does not establish the new plugin format.

## Verification and remaining source seams

Instruction and documentation changes receive JSON, source-diff and realistic
conversation review. Existing runtime observations live under `tests/`,
including `chatgpt-plugin-band.fk`, `visitor-ledger-band.fk`,
`introduction-band.fk`, and the public-dialogue contract/socket/restart
witnesses. Run the smallest relevant observation when its owner changes;
historical verdict numbers are not current proof.

Two runtime strings still need care in their owning source: `cp-seam` and the
seed index refer to a June native-voice-pending receipt, while
[`HOMECOMING.md`](../HOMECOMING.md) describes the body's current native lanes;
memory responses also overstate total revocation. This instruction update
keeps those strings from becoming hosted capability promises. A runtime repair
must observe the changed route before claiming those source seams resolved.
