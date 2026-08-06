# 2026-06-30 — HTTP stdlib stack imported

## What moved

Imported the missing HTTP/std-stack body from `~/source/Coherence-Network` into the clean repo:

- `form/form-stdlib/http-*`, `kernel-http`, `http-layer`, `http-socket`, `room-carrier-http`
- `auth-port`, `resource-port`, `host-kernel-carrier`, `storage-port`, `storage-port-file`
- `tool-channel`, `tool-channel-grammar`, `form-native-resource-interfaces`
- supporting stdlib cells: `sha256`, `hex`, `json`, `language-model`, `cell-log-store`, `hati-os-targets`,
  `bml-native-interface-package-import`
- focused witness bands under `form/form-stdlib/tests/`
- substrate docs for HTTP service/layers, resource ports, tool channels, and the fourth-kernel baseline

The existing top-level `http/` files already matched the origin stdlib HTTP files byte-for-byte; the import restores
the historical stdlib path and the missing adjacent ports/tests.

## Live witnesses in this checkout

Grounding:

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
( cat observe/native-vs-rented.fk; echo '(native-vs-rented-check)' ) > /tmp/nvr.fk
./fkwu --src /tmp/nvr.fk
```

Witness: `11111`.

Native socket loopback on the current direct-source runner:

```sh
./fkwu --src form/form-stdlib/tests/fkwu-src-socket-loopback-band.fk
```

Witness: `111111111`. The bits cover listen, assigned port, connect, accept, client->server bytes, server->client
bytes, and clean closes.

Native HTTP loopback against a one-shot local TCP peer:

```text
11
```

The bits mean `http_get` parsed status `200` and body `OK`. The peer received:

```text
GET / HTTP/1.0
Host: 127.0.0.1
Connection: close
```

## Honest seam

The rich `kernel-http` / parse / render / request / server / adapter / socket bands are BML-authored body cells. They
are imported with their old proven evidence (`docs/inheritance/proven-bodies-from-old-repo.txt`), but they are not
re-proven here through the direct `fkwu --src` runner. That direct runner covers raw Form well enough for grounding
and current socket/HTTP smoke; full BML/source lowering is the correct lane for the high-grammar HTTP bands.
