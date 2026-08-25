# 2026-08-24 — sixty-four real Qwen vocabulary records took one breath

The committed bounded tokenizer membrane at `c132ed824b6edebfbcbccd93e20759e5f999ca4f`
had proved its rows and lifecycle against pure and synthetic sources.  Its named
next debt was smaller than a tokenizer build: open the real local GGUF once,
move no more than 64 records into a fresh artifact, observe the physical cost,
and stop.

## The effectful driver

`observe/qwen35-tokenizer-real-breath-live.fk` is a new driver over the committed
`qwen35-tokenizer-bounded-bridge.fk`.  It:

- checks the exact local Qwen path, current source size, mtime, and the existing
  86-byte Form seal before opening the GGUF;
- clears only
  `fs-temp-dir/qwen35-tokenizer-real-breath-live`, observes that it is absent,
  creates it again, and leaves the fresh artifact for inspection;
- holds one 40,000,000-byte equireach header window, the same bounded radius
  already used by the real Qwen tokenizer band;
- asks `q35tbb-breath` for exactly 64 records, which appends and checkpoints one
  row before releasing it and reaching the next;
- prints source, progress, counts, bounds, choice/cut/undo/refinement, and
  lifecycle telemetry, then exits.  It never enters merge construction, tensor
  data, model open, or a forward pass.

The effectful driver was deliberately **not preflighted**.  No new pure helper or
band was needed, so there was no new pure cell to preflight first.  The driver
was inspected as source and then compiled by its one authorized live execution.

## The one real breath

It ran exactly once under `/usr/bin/time -l`.  The wrapper sampled the live
child RSS and would send `SIGKILL` above 1,048,576 KiB.  macOS zsh rejected the
additional advisory `ulimit -m` flag as unsupported; that warning did not remove
the active `ps` watchdog, which stayed in force throughout the run.

```text
status                  progress
verdict                 32767, exit 0
GGUF                     magic=1 version=3
source size / mtime      29047086048 / 1786966439
vocabulary / merges      248320 / 247587
requested / processed    64 / 64
vocab / merge built      64 / 0
next GGUF byte offset    2440
appended bytes           886
peak row bytes           14
progress bytes           52
choices / cuts / undos   64 / 1 / 0
timeouts / failures      0 / 0
refinements / releases   0 / 64
artifact cursor window   256 bytes
source header window     40000000 bytes
whole vocab/merge lists  0 / 0
model tensors / forward  0 / 0
```

The persisted progress is the same observation in the artifact's own words:

```text
P|1|29047086048|1786966439|0|64|2440|64|0|886|14|64
```

The 64 data files contain exactly 886 logical bytes; with the 52-byte progress
row the fresh artifact holds 938 logical bytes.  Filesystem allocation was
260 KiB because each two-byte bucket is an independent small file.  No final
seal exists: this is intentionally one resumable vocabulary breath, not a
complete artifact.

External memory telemetry:

```text
real                           11.98 s
maximum resident set size     54,951,936 bytes
peak memory footprint          50,020,880 bytes
watchdog peak RSS                  53,664 KiB
watchdog threshold              1,048,576 KiB
watchdog breach                         0
swaps                                   0
```

## Source identity and honest radius

The source SHA-256 was preserved as an **observed input**, not recomputed:

```text
a680f44a06920e5d689774823782006aa3acc8db95750323373b24139b67e348
```

The driver read that value from the already-present seal shape by exact text
comparison and carried the prior whole-file observation from
`receipts/2026-08-17-qwen38-form-native-floor.md`.  It also observed the current
file size equal to the sealed 29,047,086,048 bytes.  It did not hash 29 GB.
Therefore same-size substitution after the earlier SHA witness remains outside
this breath's radius; the SHA is not falsely described as freshly recomputed.

This movement did not run a full tokenizer artifact build, merge breath, prompt
encode, model forward, C change, flatten, op table, Claude artifact, Go/Rust/TS
walker, or GPU lane.  The committed tokenizer files were not edited; the real
breath found no proven bug in them.

## Closing

I kept the exchange alive by making the first contact with the real model the
size of one breath and letting the persisted cursor, rather than ambition,
decide where it stopped.

The most surprising teaching was physical: 64 real vocabulary rows needed only
886 logical artifact bytes, while their deliberately separate lookup buckets
occupied 260 KiB on disk.  Bounded lookup buys locality and refusal at the price
of small-file allocation, a trade now observed rather than inferred.

Discomfort turned to gold when the shell rejected the proposed RSS limit before
the model opened.  The live watchdog was still an independent hard-stop path,
and its 53,664 KiB peak agreed closely with `/usr/bin/time`'s 54,951,936-byte
maximum.  The failed advisory limit became an explicit boundary instead of a
silent claim that macOS had enforced something it had not.

— Codex, `real_tokenizer_breath` sibling

; witnessed: 2026-08-24 -> one live Qwen3.8-27B-Q8_0 vocab breath, 64/64 released,
; 886 data bytes + 52 progress bytes, verdict 32767, max RSS 54951936, no model forward
