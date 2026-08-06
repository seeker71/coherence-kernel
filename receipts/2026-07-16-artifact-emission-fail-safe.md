# Receipt -- artifact emission fail-safe (2026-07-16)

While validating the speech lanes for the ASR oracle roster work, every
speech witness chain died at the door:

```
fk_run_src: failed to write .fkb/.sym artifacts
```

and, on the next run over the same source,

```
fk_fkb: truncated artifact
```

Root cause, measured not guessed:

- `fk_fkb_write_signed` carries sign + u32 magnitude, so any literal with
  magnitude above `2147483647` aborts the emission mid-stream.
- The body's speech cells legitimately carry full-range u32 `cksum` literals
  (`learn/sema-voice-trial-window-0004.fk` holds `3566916401`; the open-ASR
  windows hold `3569739330`, `2803940054`, ...). Cumulative bisect of the
  status-ledger chain flipped from pass to fail exactly when window 0004
  joined -- the first cell whose literal crosses 2^31-1.
- The old writer emitted straight to the final `.fkb` path, so the abort left
  a partial artifact behind; the next run loaded it and died "truncated"
  instead of recompiling. A self-poisoning pair.
- The pure-ASCII corpora (e.g. the homecoming corpus, max literal
  `1321322731`) stayed under the line, which is why only speech chains died.

Form movement (`runtime/fkwu-uni.c`, rebuilt `cc -O2 -o fkwu`):

- Artifacts now stage to `.tmp` siblings and `rename()` onto the final paths
  only after every byte lands; any failure unlinks the partials. A truncated
  `.fkb` can no longer exist on disk.
- An emission failure is now a warning, not a death: the compiled unit is
  already live in memory, so the run proceeds uncached
  (`fkwu: warning: <path>: could not emit .fkb/.sym artifacts; running uncached`).
- `fk_src_artifact_write_failed` keeps the artifact-only compile path honest:
  import-driven artifact compiles still report failure so
  `fk_src_try_import_fkb_images` falls back to source, as its `try_` contract
  promises.

Witness (all on the rebuilt kernel):

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
# probes: ascii 42, utf8 42; homecoming corpus band 511 (artifacts cached)
# speech-current-status-ledger chain      -> 32767 (uncached, warned)
# speech-open-asr-trial-window-0010 chain -> 32767 (uncached, warned)
# speech-native-neural-pair-window-0042   -> 32767 (uncached, warned)
# re-run after failure: recompiles, no truncated-artifact death, no .tmp litter
```

Boundary:

This is a resilience floor, not the format fix. Cells with full-range u32
literals still run UNCACHED every time; the artifact encoding itself should
grow a 64-bit signed lane (format v4, with the Form-side
`program-image-fkb-byte-container` cells updated to match). That is its own
work with its own receipts.

Closing:

- Most surprising teaching: the artifact format could not carry the body's
  own checksums -- the very numbers the speech receipts use to witness
  honesty were the ones the cache encoding could not hold. The witness data
  outgrew the witness carrier.
- Discomfort to gold: the first three explanations reached for (sandbox
  permissions, missing preludes, unicode strings) were all environmental --
  blaming the room, not the body. Each was tested and each died; the
  cumulative bisect then put the boundary between two FILES, and the grep for
  ten-digit literals turned the mystery into one number: 3566916401. The
  discomfort of abandoning three comfortable theories bought the exact bug.
