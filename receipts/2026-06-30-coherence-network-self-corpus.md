# 2026-06-30 -- Coherence Network self-corpus training material

## Ground

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk
./fkwu --src bootstrap/ground-recursive.fk 10
```

Witness:

```text
42
55
```

## Source Observation

Observed in `~/source/Coherence-Network`:

- `web/messages/{en,de,es,fr,id,pt-br}.json`: 2019 aligned key paths per locale.
- `cli/lib/messages/{en,de,es,fr,id,pt-br}.json`: 45 aligned key paths per locale.

That gives 2064 shared key paths and 10320 EN-to-other parallel text pairs across the six ready locales.

## What Changed

Added `learn/coherence-network-self-corpus.fk`, a Form row that treats translated Coherence Network content as
consentful self-corpus training material. Existing translated rows (`en`, `de`, `es`, `fr`, `id`, `pt-br`) can
train now. Useful but missing languages (`zh`, `ar`, `la`) are backfill targets, not ready rows. Specific
Indigenous rows stay held until consentful corpus support exists.

## Witness

```sh
cat learn/coherence-network-self-corpus.fk \
    learn/tests/coherence-network-self-corpus-band.fk > /tmp/coherence-network-self-corpus.fk
./fkwu --src /tmp/coherence-network-self-corpus.fk
```

Witness:

```text
8191
```

## What 8191 Proves

- The observed web and CLI translated sources are represented with their measured key counts.
- Six locales are ready now: `en`, `de`, `es`, `fr`, `id`, `pt-br`.
- `zh`, `ar`, and `la` are backfill targets and cannot train until translated rows land.
- German <-> Indonesian is a ready diverse pair now.
- Missing target pairs and pending Indigenous rows cannot be selected as training pairs.

## Honest Boundary

This does not yet parse the JSON values through this repo's direct-source runner or generate missing locale
bundles. It creates the native readiness and selection row that the next JSON-loader and loopback receipts can
consume.
