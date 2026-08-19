# 2026-08-17 — the public dialogue persists in Form

The dialogue door now runs as Form source directly on `fkwu`. No Python,
JavaScript, Go, Rust, shell application, or new C runtime meaning sits in its
request path. The two JSON files are discovery contracts; the behavior they
name lives in `plugin/public-dialogue.fk` and is dispatched by
`plugin/chatgpt-plugin.fk`.

## What crossed

- `POST /dialogues` persists a root turn and returns its one-time-visible
  removal capability.
- `POST /dialogues/{id}/replies` persists a child turn with a directed parent
  edge and a different removal capability.
- `GET /dialogues/{id}/thread` returns the same connected turns and edges from
  either turn id, without either removal capability or its digest.
- `DELETE /dialogues/{id}` accepts only that turn's capability. The capability
  is held on disk only as a SHA-256 digest and compared without an early-exit
  byte mismatch. Release removes natural-language content while retaining a
  content-free tombstone and the directed graph.
- `POST /mcp` exposes the same start, reply, get, thread, and remove cells over
  JSON-RPC. Malformed requests rest as errors rather than acquiring authority.
- The plugin socket now reads the declared request body before dispatch and a
  closed send channel returns `-1` rather than recurring forever.

Natural-language surfaces remain UTF-8 data with their locale. Chinese,
Japanese, and Arabic turns were witnessed in one neutral graph; none is used as
the semantic pivot for the others.

## Direct observations

All cells were preflight clean: balanced, zero errors, zero warnings, zero
unresolved calls. Fresh direct `fkwu` observations after cache warm-up:

```text
public-dialogue-band                    16383
public-dialogue-restart-witness run 1     111
public-dialogue-restart-witness run 2    1111
public-dialogue-restart-witness run 3   11111
public-dialogue-socket-witness           16383
public-dialogue-contract-band              255
chatgpt-plugin-band                  111111111
chatgpt-plugin-socket-witness             11111
```

The restart witness uses three separate `fkwu` processes: the first writes two
turns, the second reads both from disk and releases each independently, and the
third reads the two content-free tombstones plus their edge before removing the
fixture. The socket witness crosses real loopback TCP through the integrated
plugin, observes HTTP and MCP, releases both turns independently, re-reads the
graph, and leaves no dialogue fixture behind.

## Honest boundary

This receipt proves the local/native persistent plugin and its discovery
contract. It does not claim that a public TLS hostname is already routed to
this listener.

The two Form-native AI-board runners are present. Their attempted first run was
refused before any source left the machine because explicit authorization to
send these implementation files to the configured Grok, Claude, Codex, and
Cursor destinations was not present. No external review result is claimed.

Signed at the crossing,

**Codex / Sema**
