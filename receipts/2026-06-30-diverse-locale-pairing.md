# 2026-06-30 -- diverse locale pairing guide

## Ground

```sh
( cat observe/native-vs-rented.fk; echo '(native-vs-rented-check)' ) > /tmp/nvr.fk
./fkwu --src /tmp/nvr.fk
```

Witness:

```text
11111
```

## What Changed

Added `learn/diverse-locale-pairing.fk`, a pair-selection guide for the reciprocal locale roundtrip loop.

The guide prefers far-apart languages by family, script, typology, and region while requiring corpus-ready and
consent-ready rows before a pair can be selected for training. Seeded selection is deterministic for receipts;
a live carrier can pass randomness later.

The catalog now starts from the observed Coherence Network translated self-corpus: `en`, `de`, `es`, `fr`, `id`,
and `pt-br` are ready. Chinese, Arabic, and Latin are useful backfill targets but are not marked ready until
translated Coherence Network bundles land. The Indigenous rows are specific and intentionally not marked ready
until consentful corpora exist. This avoids treating "Native American" as one generic locale.

## Witness

```sh
cat learn/diverse-locale-pairing.fk \
    learn/tests/diverse-locale-pairing-band.fk > /tmp/diverse-locale-pairing.fk
./fkwu --src /tmp/diverse-locale-pairing.fk
```

Witness:

```text
2047
```

## What 2047 Proves

- English and Indonesian rows are ready.
- Specific Indigenous rows are named but not falsely marked ready.
- Sino-Tibetan <-> Semitic distance is high.
- The best pair is ready on both sides and avoids same-locale pairing.
- Seeded pair selection is deterministic and ready.
- The selected pair expands into reciprocal A,B,B,A order.
- Missing Chinese/Arabic corpus cannot train before translation lands.
- Near European rows score below a far ready pair.

## Honest Boundary

This chooses diverse pairs for training from the ready self-corpus. It does not implement the translation model,
generate missing locale bundles, or run cross-locale audio captures. The next receipt is to feed selected pairs
into `bidirectional-locale-roundtrip.fk` from real transcript/audio loops.
