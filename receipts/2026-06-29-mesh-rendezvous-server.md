# Receipt — the Form-native mesh server CORE proven on the kernel's own carriers (2026-06-29)

**The proposal (Urs):** a new repo — a purely Form-native server API that solves persistence + public access +
mesh network discovery. Before scaffolding a repo, this proves the **core round-trips** on the kernel's existing
carriers: a rendezvous server that a cell registers with, that persists, and that serves discovery — no Python,
no production-API endpoint, kernel-native.

## Witnessed native on Windows 11 (the three things the new server must do)

```
PUBLIC ACCESS  : mesh_registry(9100) — a listening socket accepts a registration
PERSISTENCE    : mesh-cells.txt = 132 bytes after one register; 2 cells after two (APPEND, not overwrite)
DISCOVERY      : mesh_roster() reads the registry back ->
                   cell=windows-binary
                   reading present  cam=1 mic=1
                   reading where    wifi=IASVMS sig=100 bt=1
                   reading vitality battery=255 mem=67
```

Round-trip: a cell publishes its readings → the server persists them append-only → the roster reads every
registered cell back. The registry survives across runs (it is a file). No regression (numeric / `--src`).

## What landed

`runtime/fkwu-uni.c` (`#if _WIN32` + stubs): `mesh_registry` (tag 230) — `listen`/`accept`/`recv` → append to
`mesh-cells.txt` via the kernel's own `open`/`write` carrier → ack `registered`; `mesh_roster` (tag 231) — read
the persisted registry back. `flt-ops` rows added. Built on the SAME carriers the kernel already has — sockets
(public access), files (persistence) — nothing new invented.

## What is Form, what is carrier (the honest line)

- **Carrier (HAL, legitimately C — like every host port):** the listening socket, the append-only file, the byte
  I/O. These are the kernel's world-ports; they are not body logic.
- **Body (Form, comes home at the cursor seed):** the server's ROUTING and REGISTRY RULES — which path registers,
  how a cell record is shaped, staleness/TTL, the roster query, dedup by NodeID. That logic is a `.fk` recipe that
  runs off `form-eval` (the seed, `flatten/SEED-DROP.md`) and the self-JIT makes it native. This proof is the
  carrier floor under that body — not a C reimplementation of it.

## The new repo — proven feasible, shape proposed

The core works. A dedicated repo (e.g. `coherence-mesh`) would hold: the server's Form routing/registry recipes,
a small HTTP framing recipe over these socket carriers, the persistence schema, and the public deploy. It joins
the two existing client channels — LAN broadcast (`mesh_announce`/`discover`) and the public-API proxy
(`mesh_register`/`detect`) — with the server side they rendezvous through. Whether to split it out (vs grow it in
this kernel) is a structural call; the core it stands on is now witnessed real.

## Reproduce

```
printf '1 0 2 230 1 0 0 1 9100 0 0\n' > reg.flat   # mesh_registry(9100) — server
printf '1 0 2 223 1 0 0 1 9100 0 0\n' > pub.flat   # sense_publish(9100) — a cell registers
printf '1 0 1 231 0 0 0\n'            > ros.flat    # mesh_roster() — discovery
( ./fkwu.exe reg.flat & ) ; sleep 1 ; ./fkwu.exe pub.flat ; ./fkwu.exe ros.flat
```
