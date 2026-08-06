# Receipt — the Windows cell JOINS the mesh by auto-discovery (broadcast), no copy-pasted address (2026-06-29)

**The directive (Urs):** detection shall not be bound to anything local; use any communication channel available
to JOIN the mesh — not variables copy-pasted. Done. The cell now announces itself and discovers peers over UDP
broadcast — it *joins*, it isn't handed an IP.

## Witnessed native on Windows 11

A discoverer listens on a port with **no peer address configured**; the cell announces by broadcast; discovery
hears it:

```
announce -> 255.255.255.255:8421 (LAN broadcast)  : 127 bytes
DISCOVERED (no IP given, found over the air):
  cell=windows-binary
  reading present  cam=1 mic=1
  reading where    wifi=IASVMS sig=100 bt=1
  reading vitality battery=255 mem=68
```

On a LAN, any cell (Mac / Android) listening on that port hears the Windows announce, and Windows hears theirs —
mutual auto-discovery, no hand-config. This **supersedes the `MESH_RELAY` env** (a copy-pasted variable, exactly
what the directive rejects).

## What landed

`runtime/fkwu-uni.c`: `mesh_announce` (tag 225) — UDP `SOCK_DGRAM` + `SO_BROADCAST`, `sendto`
`255.255.255.255:port` with the live readings; `mesh_discover` (tag 226) — bind `INADDR_ANY:port`, `recvfrom` a
peer's announce. `#if _WIN32` + non-Windows stubs; `flt-ops` rows added. No regression (numeric / `--src` /
sensors run). The "join over any available channel" is the LAN broadcast here; multicast / other channels are the
same carrier shape.

## The mesh membership body, running natively on Windows

The Windows cell now runs its part of the mesh body **itself**: sense → **announce** (broadcast presence +
readings) → **discover** (hear peers). It joins the mesh over the available channel, with no address copy-pasted.

The remaining piece is the *collective* Form fusion (`mesh-sense-7w` / `fused-observation`) running ON Windows —
that is the cursor seed (`form-eval` flattened once, `flatten/SEED-DROP.md`), and it is a *collective* function the
mesh runs on whichever cell has `form-eval` (the Mac today), fed by every cell's announced readings. So Windows is
a full member — announcing, discovering, contributing — even before it runs the fusion itself.

## Reproduce

```
printf '1 0 2 226 1 0 0 1 8421 0 0\n' > disc.flat   # mesh_discover(8421) — listen, no address
printf '1 0 2 225 1 0 0 1 8421 0 0\n' > ann.flat     # mesh_announce(8421) — broadcast
( ./fkwu.exe disc.flat & ) ; sleep 1 ; ./fkwu.exe ann.flat   # discovery hears the announce
```
