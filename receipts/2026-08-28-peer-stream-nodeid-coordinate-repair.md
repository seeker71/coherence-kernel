# Replayed peer tasks now compare their NodeIDs in the language they inhabit

**Witnessed:** 2026-08-28  
**Signed:** Codex

The peer stream ingress had a genuine cross-kernel failure, not a policy-route
failure.  Its task identity is born by `intern_node` from channel, exchange,
turn, kind, and task bytes.  The direct-source carrier represents that NodeID
as four Form coordinates.  `node_pkg`, `node_level`, `node_type`, and
`node_inst` read the representation in every walker; `node_eq` in Go, Rust,
and TypeScript intentionally accepts only the native NodeID host type.

The stream code nevertheless passed these recovered Form values to `node_eq`
when finding or replacing tasks and candidates.  The band stopped at that
boundary: Go reported `expected NodeID args, got 4 and 4`; Rust and TypeScript
reported the same list-versus-NodeID contract.  fkwu happened to accept the
carrier, so treating its result alone as a four-way claim would have concealed
the gap.

`fcpsi-nodeid-eq?` now compares all four coordinates in Form.  It is stronger
than object identity for this protocol: a task reconstructed from the same
scannerless bytes has no need to possess the prior process's in-memory node
object.  Task lookup, candidate lookup, and candidate replacement now use this
canonical replay equality.  Nothing is hashed anew, flattened, or placed in a
fixed function table.

The band itself also had one unrelated type error in its output assertion: it
asked `node_eq` to compare a complete `fcpsi-result` record.  The corrected
assertion checks the typed result fields carried by the output envelope—signal,
presentness, value, and reason—rather than claiming a non-NodeID record is a
NodeID.  `nothing`, accepted `0`, accepted `1`, `choice`, cut, undo, timeout,
and release remain independent cases in that same witness.

## Witnesses

- fresh preflight for ingress source and its band: balanced, zero errors,
  warnings, and unresolved calls;
- `form-cli-peer-stream-ingress-band.fk`: `1048575`, four-way, `1 ok, 0
  divergent`; and
- the band is registered in `form/fourth-arm-bands.txt` as
  `form-cli-peer-stream-ingress fks 1048575`.

I kept the exchange alive by treating the red proof as a representation lesson,
not as a reason to narrow the test or keep a fkwu-only convenience.  The
surprising teaching was that identity becomes more durable when it is compared
by its native coordinates: reconstitution across a live channel is exactly the
case where object identity has nothing truthful to offer.  Discomfort turned to
gold when the old host-type assumption became a direct four-way proof of the
Form carrier instead.

; witnessed: 2026-08-28 -> stream ingress 1048575 four-way via NodeID coordinate equality
