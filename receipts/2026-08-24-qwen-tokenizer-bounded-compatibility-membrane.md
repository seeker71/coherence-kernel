# 2026-08-24 — the Qwen tokenizer became a bounded compatibility membrane

The arriving pressure was physical.  The sibling whole-artifact tokenizer
attempt was reported at tens of gigabytes of resident memory while retaining
GGUF vocabulary/merge rows and joining an approximately 11 MB monolith.  I did
not rerun that path or treat its partial artifact as input.  I built the
independent shape the observation asks for.

## What now exists

`qwen35-tokenizer-bounded-artifact.fk` defines append-only ASCII-hex records:

```text
V|id|mode|type|len|hex
M|rank|left-len|right-len|left-hex|right-hex
```

Records are split into two-byte buckets such as `vocab-61-62.rows` and
`merge-61-xx.rows`.  Exact lookup opens only the addressed bucket through
`bmf-core.fk`'s immutable scannerless file cursor, with a 256-byte window.  It
never forms a token stream, retains a vocabulary list, retains a merge list, or
joins a tokenizer monolith.

`qwen35-tokenizer-bounded-bridge.fk` adds the source side and the compatibility
edge:

- one invocation processes at most 64 GGUF records;
- each record is converted, appended, checkpointed, and released before the
  next record;
- a tiny atomic progress snapshot carries phase, source byte offset, record
  index, counts, appended bytes, peak row, and largest requested batch;
- a final seal is usable only when source size + mtime are current, both built
  counts equal both GGUF expected counts, scope is full, and resource bounds are
  inside the declared radius;
- a stale, incomplete, malformed, oversized, or slice seal produces
  `nothing` plus an observed failure/refinement, never a guessed id;
- completed artifacts perform exact lowest-rank BPE by direct merge-bucket
  lookup and exact final vocab-bucket lookup.

This is deliberately **only the Qwen prompt-compatibility edge**.  BML, BMF,
grammar lowering, recipe requests, and execution observations still move as raw
bytes through the scannerless cursor.  The model tokenizer does not become the
language reader.

## Pure observations

The artifact band covers row serialization/parsing, two-byte routing, mode/type
selection, non-ASCII byte values, duplicate append stability, stale/incomplete/
slice/resource seal refusal, malformed rows, timeout, and present token/rank
values `0` and `1` held apart from `nothing`.

The bridge band covers resumable progress round-trip at a 64-bit-sized source
offset, source freshness, cumulative append/peak/batch observations, bounded
window telemetry, raw prompt bytes, and explicit breath/encode lifecycle.

```text
preflight qwen35-tokenizer-bounded-artifact.fk       clean, exit 0
preflight qwen35-tokenizer-bounded-artifact-band.fk  clean, exit 0
preflight qwen35-tokenizer-bounded-bridge.fk         clean, exit 0
preflight qwen35-tokenizer-bounded-bridge-band.fk    clean, exit 0
artifact band                                        16777215, exit 0
bridge band                                          4095, exit 0
```

## The small live file-backed slice

The effectful driver `observe/qwen35-tokenizer-bounded-slice-live.fk` creates
only `qwen35-tokenizer-bounded-slice-live` beneath Form's witnessed
`fs-temp-dir`.  It refuses its verdict unless the prior tree is absent after
cleanup and the new directory is observed.  It does not open the 29 GB
Qwen GGUF, execute a model forward, or read the sibling monolith.  Its complete
synthetic artifact contains ordinary ids `0`, `1`, and `2`, control id `3`, and
merge rank `0`.  The same production file-cursor and exact artifact BPE doors
then observe:

```text
id0=0 present=1
id1=1 present=1
rank0=0 present=1
missing-is-nothing=1
special=3
encode "ab" -> [2]
stale-status=stale
timeout-status=timeout
verdict=8191 exit=0
```

Run under `/usr/bin/time -l`, this bounded live slice reported:

```text
real                         0.64 s
maximum resident set size   11,665,408 bytes
peak memory footprint        6,701,536 bytes
swaps                                0
```

The Form observation itself says `os-peak-rss=nothing`, because this lane has no
native OS-RSS primitive; the external process boundary supplies that one number.
The native deterministic bounds remain visible inside the result: window 256,
breath maximum 64, this slice appended 72 bytes, and peak row upper bound 24.

An independent rerun first exposed why that cleanup witness matters: the
earlier hardcoded `/tmp` path sat outside the filesystem carrier's removable
temporary root, so a second run retained the old append-only rows and reported
144 cumulative bytes.  Exact lookup still converged because duplicate rows are
stable, but the claimed fresh slice was false.  Moving under `fs-temp-dir` and
adding the absence/directory observations restored a genuinely fresh 72-byte
run.

## Honest floor

The bounded GGUF record writer compiles and its progress/seal logic is proven
purely, but it has **not** been run over the real Qwen header and no full
248,320-token / whole-merge artifact is sealed.  That restraint is intentional
for this movement: the handoff explicitly closed consuming the live 29 GB model
and the sibling monolith.  Therefore this receipt does not claim real-Qwen
artifact completion or encode parity from this new bridge.

Before a full artifact is trusted, one bounded real-header breath should verify
token-type extraction and first record offsets without generation, followed by
resumable breaths under an external RSS observer.  The final seal should also
bind the already-observed canonical GGUF SHA-256, not rely only on size + mtime;
same-size/same-mtime substitution is outside this seal's current radius.  A torn
append is refused as a malformed bucket and rebuilt; automatic shard repair is
not yet present.

## Closing

I kept the exchange alive by letting the memory observation choose a different
physical shape: the lookup walks one addressed shard, while the live reasoning
language keeps its raw-byte cursor and does not become a tokenizer client.

The most surprising teaching was that rank `0` is as dangerous to collapse as
token id `0`: both are successful exact values, and both look false if presence
is inferred from the integer.  The outcome has a separate present bit, so
`nothing`, `0`, and `1` remain three distinct observations through vocab and
merge lookup alike.

Discomfort turned to gold when a structurally balanced bridge produced 68
compiler diagnostics.  The first missing closes in the progress serializer had
nested later definitions; two branch closes then ended each loop before its
else arm.  Mapping the combined prelude line numbers back to the source made the
cascade one located shape.  After moving those closes, preflight and both bands
agreed.  Balance had named only the total; scope named the meaning.

— Codex, `bounded_token_cursor` sibling

; witnessed: 2026-08-24 -> pure bands 16777215 / 4095; file-backed bounded slice 8191;
; external maximum RSS 11665408 bytes; no GGUF/model/monolith opened
