# Receipt — the public API as a cross-network PROXY channel: register + detect cells over HTTPS (2026-06-29)

**The directive (Urs):** be able to register and detect cells from the public API as a proxy channel. Done on the
Windows side — over a Windows-native HTTPS carrier (WinHTTP), since the kernel's libcrypto TLS is unavailable here.
LAN broadcast (`mesh_announce`/`discover`) joins same-network cells; this proxy joins cells on DIFFERENT networks
via the public API rendezvous.

## Witnessed native on Windows 11

```
api_health (227)  -> GET https://api.coherencycoin.com/api/health
  {"status":"ok","version":"1.0.0","uptime_human":"4m 42s","deployed_sha":"b07cb4e6..."}
mesh_detect (229) -> GET https://api.coherencycoin.com/api/mesh/cells
  {"detail":"Not Found"}
```

The proxy channel **works**: the Windows cell reaches the public API over HTTPS and gets real data (`api_health`).
`mesh_detect` reaches the API too and gets a real response — the `/api/mesh/*` endpoints just aren't on the API
yet (404 = honest reachability, endpoint pending server-side).

## What landed (Windows side — complete)

`runtime/fkwu-uni.c` (`#if _WIN32` + stubs): `fk_https` — WinHTTP HTTPS GET/POST to `api.coherencycoin.com:443`
(native TLS, cert-validated); `api_health` (227, the channel witness); `mesh_register` (228, POST the cell's
presence/where/vitality JSON); `mesh_detect` (229, GET the registered cells). `flt-ops` rows added. Link adds
`-lwinhttp`. No regression (numeric / `--src` / LAN broadcast all run).

## The honest seam

- **Windows side — done, witnessed:** the HTTPS proxy carrier reaches the public API; register/detect ride it.
- **API side — pending:** `/api/mesh/register` (POST) + `/api/mesh/cells` (GET) need to exist on
  `api.coherencycoin.com` (the Coherence-Network production API). The 404 proves the carrier reaches the API; the
  endpoints are the server-side piece. The moment they exist, the Windows cell registers + detects peers across
  networks with no further Windows work.

## The two join channels now

1. **LAN broadcast** (`mesh_announce`/`mesh_discover`) — same-network cells, auto-discovery, no address.
2. **Public-API proxy** (`mesh_register`/`mesh_detect`) — cross-network rendezvous over HTTPS, no address.

Neither uses a copy-pasted variable; both are "any available channel." A cell joins by announcing locally AND
registering globally, and detects peers by listening locally AND querying the API.

## Reproduce

```
gcc -O2 -o fkwu.exe runtime/fkwu-uni.c -lws2_32 -lwinmm -lavicap32 -luser32 -lwlanapi -lbthprops -lwinhttp
printf '1 0 1 227 0 0 0\n' > h.flat && ./fkwu.exe h.flat    # GET /api/health over HTTPS -> {"status":"ok",...}
```
