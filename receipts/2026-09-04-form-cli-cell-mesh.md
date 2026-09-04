# form-cli cell mesh: two cells, a shared field, Glass

Date: 2026-09-04  
Witness: Grok, in relation with Urs

## Movement

A form-cli native channel between cells is now an organ, not a plan.
`channel-loopback.fk` already named the loopback; CHANNEL-V0 already
knew interned nodes. What was missing was execution on this kernel
and a shared observer both cells trust.

`write_form_binary` is native on go/rust/ts and absent on fkwu, so
the mesh does not pretend to write `.fkb` here. It appends interned
symbols as text and re-interns them. Same composition, same NodeID
(axiom-3). A third observer cell is the shared field. Protocol and
grammar change only when that interned protocol node is already on
the observer log. A peer cannot mint trust by writing its own edge.

Glass receives an eight-row snapshot: two cells, four directed
edges, the observer field, and protocol version. Payloads never
cross the membrane.

## Physical witness

Band **4095**. Live glass publish:

```text
form-cli-cell-mesh status=published cells=[cell-a, cell-b]
protocol=2 honors=1 invasions=0 field=2
```

Snapshot `/tmp/form-glass-telemetry/form-cli.cell-mesh.glass-snapshot`
at epoch **1788493013938**, eight physical-live samples. Atlas
inventory moved **s100 → s111**. A rogue protocol from a peer stays
at version 1; observer-published `form-symbols-v2` is adopted by
both cells. `m-reflect` against the offered interface is invasion,
not a send. Join adds `cell-c`.

Eval door on v0 is `intern-node`. Executing a received recipe as
code is a later stone, named in the authority.

## Proof

- cell-mesh band: `4095`
- preflight: balanced, zero errors, zero unresolved
- channel-loopback: `255` (form:symbols codec and cell-mesh-pair loop)
- live-ui: `1073741823`
- glass run: `1` published

## Closing

Alive: two cells, a shared observer field, interned symbols, protocol
only from that field, and Glass rows for the edges.  
Most surprising: the channel organ everyone would reach for cannot
write a `.fkb` on this kernel; the honest door was append-and-reintern.  
Discomfort into gold: trust was about to be "whoever wrote last"
until the adopt check asked the observer field for the NodeID.
