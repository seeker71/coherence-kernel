# Speech locale coverage matrix

Date: 2026-06-30

This receipt adds a per-locale coverage matrix for the speech stack.

Witness:

```sh
cat learn/speech-locale-coverage-matrix.fk \
    learn/tests/speech-locale-coverage-matrix-band.fk > /tmp/speech-locale-coverage-matrix.fk
./fkwu --src /tmp/speech-locale-coverage-matrix.fk
# 32767
```

Current matrix:

- `13` locales tracked.
- `11` locales ready now.
- `8` locales have live anchor or carrier-live rows.
- `2` specific Indigenous rows remain consent-pending: `nv`, `chr`.
- `12` pair rows tracked: `10` native, `2` oracle-guided.
- Unicode live anchors: `en<->zh` is `10/12 = 83%`; `en<->ar` is `12/12 = 100%`.
- Oracle-held rows remain live open dictation and Sema live voice.
