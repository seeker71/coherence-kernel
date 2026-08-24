# 2026-08-24 — one source window carried four bounded tokenizer breaths

The first real Qwen tokenizer contact had proved one 64-row breath, but every
new process paid the same source/header entrance again.  This movement asked a
narrower physical question: can one Form process keep the bounded source
window resident, carry progress as a value rather than reopening it between
breaths, and still release every row?

## The resident layer

`qwen35-tokenizer-resident-breaths.fk` extends the committed bounded bridge
without widening it.  One residence is capped at 16 breaths and each breath is
still capped at 64 records.  `q35trm-run` opens the tiny progress row once,
passes that state directly into the committed vocabulary/merge loop, and
passes each returned progress value into the next breath.  The caller supplies
one equireach source value for the whole recursion.

The accumulator keeps per-breath causality visible: processed/appended rows,
peak row, choice, cut, undo, timeout, failure, refinement, release, state
handoff, and a monotonicity check binding the next record index and cumulative
artifact bytes to the preceding breath.

No vocabulary list, merge list, joined tokenizer file, model tensor, model
forward, C change, flattening, operations table, Metal operation, or MLX
operation was added.

Pure definitions and the live function definition were preflighted; the
effectful `-run.fk` door was deliberately never preflighted:

```text
preflight qwen35-tokenizer-resident-breaths.fk          clean, exit 0
preflight qwen35-tokenizer-resident-breaths-band.fk     clean, exit 0
preflight qwen35-tokenizer-resident-four-breaths-live.fk clean, exit 0
resident pure band                                      65535, exit 0
```

## The one live witness

Immediately before the run, the prior held-out manifest process had exited and
the separate Claude Qwen `llama-cli` PID 6815 was observed to have exited too.
That Claude JIT session then started a new `llama-cli` PID 17788 while this
witness was running.  The movements shared neither runtime nor carrier: this
tokenizer process opened one 40 MB file window and no model tensor, GPU, Metal,
or MLX door.  The only other transient `fkwu` was a low-memory held-out pure
band; it likewise opened no Qwen tensor.  The authorized tokenizer payload was
exactly:

```sh
/usr/bin/time -l ./fkwu observe/qwen35-tokenizer-resident-four-breaths-run.fk
```

That payload ran once inside `form-run sh -c`.  `/usr/bin/time` was PID 22883
and its exact `fkwu` child was PID 22887.  A 50 ms watchdog sampled only that
child and sent `SIGKILL` above 1,048,576 KiB.  It did not touch any sibling
process.

```text
verdict / exit                      16777215 / 0
watchdog child PID                  22887
watchdog peak / threshold KiB       54160 / 1048576
watchdog breach                     0
maximum resident set size           55459840 bytes
peak memory footprint               50512376 bytes
swaps                               0
real / user / sys                   46.10 / 45.06 / 0.93 s
GGUF magic / version                1 / 3
vocabulary / merges                 248320 / 247587
source window loads / bytes         1 / 40000000
progress loads                      1
requested / completed breaths       4 / 4
rows per breath / processed         64 / 256
choices / cuts / undos              256 / 4 / 0
timeouts / failures / refinements   0 / 0 / 0
releases / state handoffs           256 / 3
monotonic progress                  1
initial / final index               0 / 256
initial / final GGUF byte offset    1864 / 4330
artifact byte delta                 3730
progress byte delta                 55
peak row bytes                      15
artifact cursor window              256 bytes
incomplete seal files               0
model tensors / forwards            0 / 0
Metal / MLX operations              0 / 0
```

The artifact directory independently contained 256 vocabulary shard files plus
one progress row.  Vocabulary shards held exactly 3,730 logical bytes; the
progress row held 55, for 3,785 logical bytes total.  Small-file allocation was
1,028 KiB.  The persisted cursor said:

```text
P|1|29047086048|1786966439|0|256|4330|256|0|3730|15|64
```

No seal exists because 256 records are an observed resumable prefix, not a
complete tokenizer.

The source SHA-256
`a680f44a06920e5d689774823782006aa3acc8db95750323373b24139b67e348`
was again carried as an observed input from the existing Form seal and the
2026-08-17 receipt.  This run checked the exact 86-byte seal text plus current
source size and mtime; it did not rehash 29 GB.  Same-size substitution after
the earlier SHA witness therefore remains outside this observation.

## Throughput and honest extrapolation

External wall time gives 256 / 46.10 = **5.553145 vocabulary rows/s**, or
11.525 seconds per 64-row breath.  The Form clock covered 45.506 seconds and
gives 5.625632 rows/s.  Against the earlier isolated witness's 64 / 11.98 =
5.342237 rows/s, residence improved externally observed throughput by 3.948%.
The improvement is real but small: compile/header entrance was not the main
cost at this radius; per-row parse, shard append, and atomic progress persist
still dominate.

A straight line from these **first 256 vocabulary rows only** suggests about
12.421 hours for 248,320 vocabulary rows.  Extending the same rate across all
495,907 vocabulary+merge rows gives 24.806 hours.  Neither number is a full
build forecast: merge rows have a different length distribution and parsing
cost, later shard collision/allocation behavior has not been sampled, and this
run did not cross the vocabulary-to-merge phase.  They are scale signals that
name the next optimization target, not completion claims.

## Closing

I kept the exchange alive by carrying the actual source and state through four
breaths, while letting every row leave before the next arrived.  Residence
became an observation, not a synonym for retention.

The most surprising teaching was that removing three progress reopens and
three process entrances improved throughput by only 3.948%.  The dominant
physical cost is now more clearly inside each row's append-and-checkpoint
movement, where a next optimization can be observed without inventing a
monolith.

Discomfort turned to gold twice: a static parenthesis count found one extra
closer in the effectful door before any GGUF opened, so the live body was split
into preflightable definitions and a tiny never-preflighted runner; and a
separate 43 GiB Claude process made coordination concrete.  Observing that this
movement used neither model tensors nor the GPU let both enquiries continue
without falsely equating shared time with shared carrier ownership.  The
54,160 KiB watchdog witness then showed the bounded shape on its own terms.

— Codex, `tokenizer_resident_breaths` sibling

; witnessed: 2026-08-24 -> pure band 65535; live verdict 16777215; 4x64=256
; records and 256 releases; 0 failures/timeouts; watchdog peak 54160 KiB;
; maximum RSS 55459840 bytes; 5.553145 vocab rows/s; no model forward
