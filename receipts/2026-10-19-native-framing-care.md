# Native request framing

The reusable socket dispatcher imports its own line grammar. Standalone
admission no longer depends on a caller having supplied `trim` incidentally.

One incremental state retains Content-Length or chunked framing through split
TCP writes. Chunk data, split CRLF, extensions and trailers keep their distinct
states. Dispatch waits for the complete first request; ambiguous length/transfer
framing, malformed field names and routing/framing trailers receive 400 before
the response cell runs. `Expect: 100-continue` receives its protocol acknowledgement.
Header/body spans are held once and joined at dispatch rather than repeatedly
copying the entire growing request. The receiving organ still owns its wire
and the response cell receives that wire intact.

The actual standalone Linux observation sent headers before a five-byte body,
made an independent request while that peer waited, split a chunk's CRLF and its
trailers, then sent the final terminator. The response cell received `hello` and
the complete chunk/trailer wire. Ambiguous framing and a malformed field received
400. A deliberate source stop received 500; the same listener then answered 200.
No broad kernel suite ran. The response/framing implementation is Form/BML.

`native-posix.bml` shares one argument packet across Linux x86-64 and Darwin
ARM64 Form-emitted images. Socket meanings, errno, no-SIGPIPE ownership and
readiness stay in that reusable grammar. Completion is a pipe born before fork:
the parent owns its reader, the child its writer, and actual process exit closes
the writer and wakes the parent to reap. The Linux pidfd dependency and duplicate
Linux-only socket helpers are retired. No child executable or request/value
serialization was introduced.

The Darwin observation exposed a real ownership gap: disabling the global field
still left the per-process values and program pages MAP_SHARED. Forked JIT writes
could alter sibling execution, including the observed fork result. Three existing
allocation selectors now honor `FK_STORE_PRIVATE` and use their existing private
fallback. No C socket, fork, poll, protocol, or dispatch behavior was added. The
private selector includes program pages, not just value tables; the actual fork
reading then retained parent pid/flag 0 and child result 0/flag 1 independently.
Pulse selects that single context setting before admission. Earlier shared-page
observations are not evidence of isolation. The runaway private observation was
stopped, not allowed to continue producing repeated warnings.

Final Linux x86-64 and Darwin ARM64 packet observations retained the actual PID,
whole loopback packet, completion pipe, idle/nonblocking errno and owned closes.
The final split-request fixture on each observed complete length/chunk/trailer
wire, independent progress, 400/500 responses and a subsequent live 200. One
earlier Darwin aggregate assertion failed without per-clause detail; the focused
detail added at that same boundary subsequently observed all five predicates
true and all response statuses correct. It is not retroactively called a pass.
No sibling kernel or broad validation was run.

The first chunk observation ended in `nsd-hexadecimal` with `arith: only numbers
add, subtract, multiply and divide`. The guessed integer-ceiling expression was
removed: decimal/hex parsing now checks each multiply/add by its round trip and
returns absence on unrepresentable framing. The replacement observation above
passed. A separate literal-kind reading returned int, so it does not establish
the earlier arithmetic stop's deeper cause; no unsupported-literal claim is made.

Ingress ownership stays with the executing application rather than guessed fixed
caps in the syscall/dispatch resource. The current Pulse port was observed as
`8100/tcp: null` (not published); its existing Traefik/Cloudflare ingress owns public
connection and read allowances. Keep those allowances with that owner, and carry
actual pressure or a new direct-listener exposure into native care. This movement
adds no default size, connection-count, retention or request-age limitation.

Source findings: kernel PR 604 reviews 4312307426 and 4312307438 repaired here;
4312307447 remains ingress/resource-care guidance at its actual owner, not a
request to duplicate policy guesses as a second gate. OpenAI Codex supplied the
reasoning, Form the execution and observation; exact model/task tokens are not
exposed. The most useful teaching was that a correct import under one application
is not yet a standalone reusable contract.
