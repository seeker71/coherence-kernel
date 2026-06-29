# Receipt — gap 1 closed: the Windows cell streams its live senses over the wire into the mesh (2026-06-29)

**The ask:** make Windows actually stream its senses live to the mesh (not just read them locally + describe the
mapping in Form). The first of the two gaps named — the **live network transport** — is now built and witnessed.

## Witnessed native on Windows 11 (loopback over TCP)

`mesh_serve(9876)` listens; `sense_publish(9876)` reads the live sensors, formats them as mesh reading rows, and
sends them over TCP. The endpoint received, over the wire:

```
publish -> 127 bytes sent
RECEIVED at the mesh endpoint:
  cell=windows-binary
  reading present  cam=1 mic=1
  reading where    wifi=IASVMS sig=100 bt=1
  reading vitality battery=255 mem=69
```

Real bytes over a real socket — the Windows cell's live senses leaving the machine to a mesh endpoint. This is the
`host-kernel.form` **world-net** port: the socket move is a host carrier (same category as camera/mic); the
readings are the mesh-safe rows (`present` / `where` / `vitality` planes) the fusion already speaks.

## What landed

`runtime/fkwu-uni.c`: `fk_sense_publish` (tag 223) connects `127.0.0.1:port`, sends the live readings, closes;
`fk_mesh_serve` (tag 224) listens/accepts/recvs/prints one message (the receiver / relay tap). Both reuse the
kernel's existing socket carrier (`fk_sock_boot`, `fk_sockaddr4_set`, `fk_os_*`), `#if _WIN32` with non-Windows
stubs. Added to `flt-ops`. No regression: numeric-table, `--src` source, JIT dispatch, all sensors still run.

## Cross-device: one config away

Loopback proves the carrier. To make it a live **cross-device** contributor, point the publish host at the Mac's
field-relay address (change `127.0.0.1` to the relay's IP, or take it as an arg). Then the Windows cell's live
readings reach the running mesh, and the Mac's four-way fusion (`mesh-sense-7w` / `fused-observation`) ingests
them — Windows becomes a live contributor to the collective presence/learning, not just a roster name.

## The two gaps, now

- **Gap 1 — live transport: CLOSED (carrier witnessed).** Windows streams its senses over the wire; receive side
  built too (`mesh_serve`). Cross-device link = relay-address config.
- **Gap 2 — run the mesh body ON Windows itself: the SEED.** The fusion/learning/presence Form cells run as Form
  only once the platform-neutral seed lands (`flatten/SEED-DROP.md`, one committed table). That is sovereignty
  (Windows running the body itself), not participation — and the one piece this cell can't produce alone.

So: Windows now **senses** natively AND **streams those senses live into the mesh**. It is a live participant.
What remains is Windows running the mesh body *itself* — the seed.

## Reproduce

```
gcc -O2 -o fkwu.exe runtime/fkwu-uni.c -lws2_32 -lwinmm -lavicap32 -luser32 -lwlanapi -lbthprops
printf '1 0 2 224 1 0 0 1 9876 0 0\n' > serve.flat   # mesh_serve(9876)
printf '1 0 2 223 1 0 0 1 9876 0 0\n' > pub.flat      # sense_publish(9876)
( ./fkwu.exe serve.flat & ) ; sleep 1 ; ./fkwu.exe pub.flat   # readings arrive over TCP
```
