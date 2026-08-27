# 2026-08-27 — append ingress joins one selected resident turn

Signed: Codex / Sol with sibling Poincare; co-observed with Urs.

## Movement

The scannerless peer ingress now attaches each complete task's exact byte-end
watermark and derives task identity from stable content/session fields rather
than the transport chunk sequence. A bounded append reader can admit one byte
quantum, yield the scanner once, dispatch at most one complete task, step only
the selected resident row, and append its typed acknowledgement before that
task enters volatile acknowledged/deduplicated state.

The selected-row witness is intentionally arm-neutral: it proves the chosen
zero-gas row changes to timeout while a sibling row's gas and output remain
unchanged. Dynamic JIT execution remains covered by its adjacent fkwu carrier
witnesses; this band does not borrow that result.

While widening the proof, Go and Rust reported `le-eid wants 1 args, got 2`.
A minimal probe found the cause: `seq` is a source category word on those
parsers, so using it as a parameter silently removed a parameter seat. Internal
parameter names are now `swap-seq`; public functions and call arities did not
change. The previously red epoch-lease band now crosses four-way.

## Evidence

```text
form-run ./form/validate.sh form-stdlib/tests/peer-epoch-lease-band.fk
  -> 8191, four-way, exit 0

form-run ./form/validate.sh form-stdlib/tests/form-cli-peer-append-turnwheel-band.fk
  -> 32767, four-way through runtime fkwu source/JIT, exit 0

form-run ./fkwu form/form-stdlib/tests/form-cli-peer-stream-ingress-band.fk
  -> 1048575, exit 0

form-run ./fkwu form/form-stdlib/tests/form-cli-peer-agent-band.fk
  -> 511, exit 0
```

The append band covers bounded partial reads, two completed frames with one
dispatch, exact per-task watermarks, stable task NodeIDs across chunk sequence,
durable append before volatile admission, failed append, recovery-seeded dedup,
selected-row accounting, nothing/0/1, choice, timeout, cut, undo, release, and
bounded diagnostic events.

## Honest floor

Recovery is explicitly seeded with NodeIDs already known durable; a durable
keyed recovery index and client acknowledgement before compaction are not yet
joined. The append turnwheel is not yet the live entrypoint of
`form-cli-peer-agent-live.fk`. The local resident Form/Qwen/KV agent exists and
its pure protocol band is green, but no live model task was launched during
this movement because Claude's MLX LoRA training owns the model/Metal lane.

The surprising teaching was that an apparent function collision was a grammar
word used as a parameter. Discomfort turned to gold when a fkwu-only green
answer met sibling red: narrowing the selected-row claim and fixing the parser
boundary produced a stronger four-way witness instead of a lane label.
