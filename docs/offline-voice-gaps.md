# The offline-voice gap register

One door reads every gap on the way to a voice that is fully our own — every query answered natively, every
self-improvement feature native, an aligned voice, rent toward zero — and says, per gap, where it stands today:
**closed**, **open** or **unread**, with the numbers it read and the file it read them from.

```
./fkwu observe/offline-voice-gaps-run.bml </dev/null
```

## What a gap is

A gap is one row of `learn/offline-voice-gaps.jsonl`. The row carries the whole reading rule as data: which ledger
(`source`), which rows count (`where`), which field is read (`field`), how it is compared (`compare`) and with what
(`threshold`, or `against`, another field of the same row), and the goal line it serves (`goal`). The door holds no
code per gap. It holds one reader per shape, and each `kind` names its shape:

| kind | shape | what is read |
|---|---|---|
| flow-review, turn-ledger, alignment, guide, lora, readings, walk | last | the last row that holds `where`; its `field` against the threshold or the `against` field |
| coverage, voice-availability | share | among the rows that hold `where`, the permille that also hold `match` |
| rent | trend | the last `window` rows: the series printed with its direction, the latest decides |
| plan-doc | table | a markdown table in a document: two cells, as decimals in milli, the last row of each key |

A new gap is a new row. A new ledger is a new row naming it. A new shape is the only reason to touch the reader.

## The three verdicts

- **closed** — the comparison holds in what was read.
- **open** — the comparison does not hold. The observation says by how much.
- **unread** — nothing could be read: the source is absent, no row holds `where`, the field is absent or not a number,
  the comparison or the kind is unknown, or (a walk source) no walk checkout is named. Unread is never closed: a gap
  the body cannot see is not a gap the body has crossed.

## The gaps today

The rows stand for: open band-first gaps (the review's `open_gaps`); the lane's last real option band green; the walk's
last class (`begun` is nothing to heal); the alignment predicates at full; the native fraction of lookup and of plan
queries; the voice's answered share of local runs; the `total_share_ppm` trend of the rented sessions; the remote
dependence of the last guided gap; the 3B lift (the child's loss under the parent's); draft readings rows written; the
Q4 decode rate against Q8 through the voice door (`receipts/native-training-plan.md`); and the 27B last-block rung, read
as a promoted adapter on `receipts/lora-promotion-ledger.jsonl` whose child beats its parent — unread until the ladder
in `docs/qwen-last-block-lora-design.md` writes its first promotion row.

The thresholds are the goal lines, not instruments to tune: a share reads closed at 1000 pm because the goal is every
query native, and the rent trend reads closed at 1,000,000 ppm because the goal is every token local.

## The register

Each run prints one line per gap

```
[offline-voice-gaps] <id>: <verdict> — <title>; <observation> (<kind> <source>; serves: <goal>)
```

and one summary line, which is also the door's value, and appends one row to
`receipts/offline-voice-gap-register.jsonl`: `at_unix_ms`, the gaps file, the counts (`gaps`, `closed`, `open`,
`unread`) and `verdicts`, each gap's id, kind, verdict, observation, source and goal. Two rows side by side are the
movement: a gap that was open and reads closed, a count that fell. The register is the only file the door writes; the
ledgers it reads belong to the doors that write them (`observe/local-flow-review.bml`, `observe/native-turn-run.bml`,
`observe/alignment-read-run.bml`, `observe/query-coverage-run.bml`, `observe/session-rent-run.bml`,
`observe/lora-lift-run.bml`, `observe/draft-readings-to-rows.bml`, `observe/walk-retry.bml`).

## Reading it from a band

The door takes one optional JSON line on stdin: `gaps` (another gaps file), `root` (a directory the sources resolve
under), `walk` (the walk checkout, in place of the one the day-turn plist names; `""` is none) and `register` (another
register path). `form/form-stdlib/tests/offline-voice-gaps-band.bml` proves every shape over fixture rows that read
closed, open and unread, the summary counts, the register row, and the refusal of a missing gaps file (verdict 255).
