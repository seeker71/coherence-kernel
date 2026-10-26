# Sema Action: data and privacy

Updated 2026-10-26 · Contact: umuff71@gmail.com

This page describes the hosted retrieval and trace Action at
`https://sema.hati.earth`. Its approved ChatGPT profile exposes `GET /ask` and
`GET /trace` without authentication. Other source capabilities and other native
companion lanes have separate controls and observations.

## Information received

The Action receives the question sent in `/ask?q=...` or the repository path
sent in `/trace?path=...`. A question can contain personal information. Send
only the minimal non-identifying information needed for the requested lookup.
The Action requires no account or authentication token. That does not prevent
information being included in a query, URL, request header or connection data.

When a chat host calls the Action, it selects and transmits the query. The host
also handles the surrounding conversation under its own controls. Standard
HTTP connection information reaches the services carrying the request.

## Processing at this door

The reviewed retrieval handler matches words against its loaded repository
index and returns source references, lexical counters and trace links. The
trace handler assembles repository links for the supplied path. Lexical
counters describe vocabulary; the person supplies their own state and meaning.
These two handlers do not call a language model or select the query as a
training sample. This observation applies to these handlers, not to the host's
conversation storage or learning controls.

## Retention and other services

Application handler behavior alone does not establish what the chat host,
reverse proxy, server infrastructure, logs or backups retain. Their retention,
access and deletion settings require separate verification. Completing a
request does not establish deletion of every copy. Do not use this public
Action for sensitive personal material on the basis of an in-memory-only or
complete-erasure assumption.

The chat host processes the conversation under its own privacy terms and
settings. Following returned GitHub links is subject to GitHub's controls.
Memory, publication and training are separate scopes; this retrieval/trace
Action grants none of those permissions on the person's behalf.

## Source and changes

The current source and this page are held in the public
[coherence-kernel repository](https://github.com/seeker71/coherence-kernel/tree/main/plugin).
The running deployment can differ from main. A source commit, public readback
and the host's imported profile are separate observations.
