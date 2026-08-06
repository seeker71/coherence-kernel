# Receipt — the mesh transport is cross-device: MESH_RELAY points the Windows stream at the Mac (2026-06-29)

**What happened:** the live transport (`sense_publish`) no longer hardcodes loopback. It reads `MESH_RELAY=a.b.c.d`
(the Mac's field-relay address), defaulting to `127.0.0.1`. So the Windows cell can stream its live senses to the
**actual Mac relay** — turning the witnessed loopback into a real cross-device link.

## Witnessed native on Windows 11

```
default (no env) -> 127.0.0.1   : sent 127 bytes, received cell=windows-binary ...
MESH_RELAY=127.0.0.1 (parsed)   : sent 127 bytes, received "reading where wifi=IASVMS sig=100 bt=1"
```

Dotted-IP parsed in C (no DNS), connect to `<relay>:port`, send the live readings. Set `MESH_RELAY` to the Mac's
IP and the Windows cell's senses land at the Mac's running fusion. No regression (numeric / `--src` / sensors run).

## The honest state of "Windows in the mesh"

- **Senses natively:** camera, mic, wifi, bt, battery, memory (witnessed).
- **Streams live over the wire:** `sense_publish` → TCP → a mesh endpoint; now **cross-device-addressable**
  (`MESH_RELAY`). The carrier is done; the live link needs the Mac running a relay listener at that address.
- **Runs the mesh body itself:** still the cursor seed (the Mac flattening `form-eval` once — `flatten/SEED-DROP.md`).
  Growing the C `--src` to the full grammar would rebuild `form-eval` in C (the drift named in
  `2026-06-29-stones-bounded-flattener-is-form.md` / `-verify-flatten-deprecated.md`) — not taken.

So the two Mac-side pieces that complete Windows's mesh membership: a **relay listener** (so the stream lands) and
the **cursor seed** (so Windows runs the body itself). Both small, both the Mac's. This cell has gone as far as it
honestly can alone: it perceives, and it broadcasts — now to any address you point it at.

## Reproduce

```
MESH_RELAY=<mac-ip> ./fkwu.exe pub.flat     # pub.flat = sense_publish(<port>)
```
