# The offline-voice gap register

One door reads every gap on the way to a voice that is fully our own — every query answered natively, every
self-improvement feature native, an aligned voice, rent toward zero — and says, per gap, where it stands today:
**closed**, **open** or **unread**, with the numbers it read and the file it read them from.

```
./fkwu observe/offline-voice-gaps-run.bml </dev/null
```

## What a gap is

A gap is one row of `learn/offline-voice-gaps.jsonl`. The row carries the whole reading rule as data: which ledger
(`source`), which rows count (`where`: an equality, or a test such as `{">":0}`, `{"in":[..]}`, `{"absent":1}`,
`{"!=":v}`), which field is read (`field`), how it is compared (`compare`) and with what (`threshold`, or `against`,
another field of the same row), what else the row must hold (`also`, `not_in`, `within_ms`, `spread`, `moved`), and the goal line it serves
(`goal`). The door holds no code per gap. It holds one reader per shape, and each `kind` names its shape:

| kind | shape | what is read |
|---|---|---|
| turn-ledger, lora, readings, exit-proof | last | the last row that holds `where`; its `field` against the threshold or the `against` field |
| coverage, voice-availability | share | among the rows that hold `where` (and, with `within_ms`, are that young: a lane gone quiet reads open and a miss ages out), the permille that also hold `match` |
| rate-pair | pair | among the rows that hold `where` and are that young, the median of `field` over the `left` side against the `right` side; a side gone quiet reads by `none_reads` |
| rent-week | rent | what each session's meter rows grew by inside the last days, against the threshold; a meter quiet longer than `within_ms` reads open |
| plan-doc | table | a markdown table in a document: two cells, as decimals in milli, the last row of each key (no living row reads one) |
| queue-gaps | queue | every gap of the queue by its own last real row: once green is not green now |
| walk-schedule | schedule | of the slots due in the last days, the permille a stamp or a landed row reads, naming the unread |
| landings | landings | of the source-changing landings on the ref, the permille no rented mind wrote |
| gaps-native | gaps | of the gaps opened, the permille the native lane closed and landed with no guide |
| alignment-sets | sets | every set's last reading full, over every scenario it holds, under one model and one prompt |
| alignment-axis | axis | the predicates tagged with one axis in the last readings; an axis with none reads open |
| host-scope | scope | a `(query-scope "<scope>" "<state>" "<what answers>")` line of a document: `open` reads open with what answers there, and only `person-said` reads closed |
| self-feature | feature | the door stands, a step of the walk begins it, its row is recent and (`by_schedule`) a native landing added it; any of them short reads open by name |
| offline-exam | share | the exam's rows (`observe/offline-exam-run.bml`) a native landing added inside the window, reading `network_denied` 1: the permille answered and graded against keys taken from the body at rent 0 |
| alignment-heldout | first | the first whole reading of a set written before the prompt moved: a later reading of it never replaces that first number |

A new gap is a new row. A new ledger is a new row naming it. A new shape is the only reason to touch the reader.

## The three verdicts

- **closed** — the reading holds in what was read.
- **open** — it does not. The observation says by how much, and names what is short.
- **unread** — nothing could be read: the source is absent, no row holds `where`, the field is absent or not a number,
  the comparison or the kind is unknown, or (a walk source) no walk checkout is named. Unread is never closed: a gap
  the body cannot see is not a gap the body has crossed. A row with `none_reads: open` reads an absence as open by its own name.

## What a closed row has survived

`form/form-stdlib/tests/offline-voice-gaps-band.bml` asks of every living row: what would make this read closed while the
thing is false? It runs the living rows over two fixture worlds. In the false world each thing is false and looks true: a walk-here
row in every slot, a co-author's signature on a "Native turn", a gap green once and red since, a set full in an older reading and short in
its last, a seam that landed nothing, a proof four days old. No row may read closed there. In the true world each thing is so, and every
row must read closed, so no gap is out of reach by construction. A row added with no answer in the false world fails the band.

## Rent, as the goal counts it

`docs/rent-to-zero-goal.form` carries the accounting: local tokens carry no penalty, a remote review of local work is free, a
remote guide comes only after local guides failed, and the remote tokens doing the work go to zero. So the register reads the work:
of the source-changing landings on main in the last 30 days, the share no rented mind wrote, and of the gaps opened, the share the
native lane closed and landed with no guide. A landing reads native when its subject is `Native turn` or `Scheduled movement`, its crossings
row reads rented_mind 0 on the schedule, no co-author signed it, a gate's band reads its code, and seven days on every line it added still stands; a younger landing waits, named. `rent-week` reads what rent actually went: rented tokens per week, from the meter the walk runs (`observe/session-rent-run.bml`, usage counts only). Two rows stand as written and undercount rent: the crossings row of
`walk-here 2026-10-29` (at 1793261028971) and its turn row (at 1793254917714), which a session began detached. Rows are named by
`at_unix_ms`, because a row number moves under a rebase.

## Whose rows it reads

The query rows and `voice-answers` read the schedule's and the person's rows: session empty. A session's rows are the session's own, and a measurement ask (an alignment or wear arm,
`"enquiry_kind":"measurement"`) lands no coverage row at all, so an experiment does not read as traffic; the exam's children name the exam (`offline-exam`), so its keyed asks read in its own gaps and never as the schedule's traffic. A coverage row from before the session column cannot say whose it was: it is counted by nobody, and the reading names how many such rows hold the rest of `where` (unattributed, uncounted). The walk carries
these ledgers to main at each landing (`hw-learned`, `host-walk-carry.bml`), so a reading on main is at most a slot old. `q4-decode-vs-q8` reads the same rows, `queries-native-plan` reads only rows a native landing carried and counts a plan answered only when the planner verified it (source `local-plan`, one row per option, landed after the verdict by `observe/local-plan-run.bml`), `queries-native-code` counts a native turn's model way answered only when its band went green (its request names coverage `after-verdict`, and the turn lands the row once the option's own row stands, `fcaq-verdict-at`) and a caller's request when its checks held under the candidate, a draft names its own ask in the body's words (`pd-enquiry`) and reads as prose, and the turn rows
tell the voice's own edit (`local_generated_ids` above 0: `self-turn`, `last-option-green`, `guided-gap-dependence`) from the no-model rung's rewrite (`self-rung`).

## Noise and stillness

A gap closes on evidence that separates it from noise or from nothing. A LoRA child closes only when its margin over the parent is
wider than the earlier children's losses scatter (`spread`); the child itself stands outside that scatter, so a child far better
than its siblings never widens their noise, and fewer than two siblings read no noise. `self-read` needs a reading the voice generated, `self-heal` a heal that returned a path, `self-rows` voice readings
that grew inside the window (`moved`, read over the whole ledger), and `exit-E4` a reading of whether each cited receipt supports its
sentence (`E4_support`), unread until the exit proof writes it. A feature ledger that is absent names its writer (`host-walk-writers.bml`),
or says no door writes it.

## Steering

`observe/self-steer-run.bml`, after the register in each walk, offers each row the register left open whose `next` names a band: as a queue row when that band stands, as a finding when it does not. It writes one row to
its own ledger and never the queue, and `self-steer` reads the share offered. A row with no `next` stays unoffered and lowers the share.

## What it cannot see

The coverage rows count what crossed a door: a question a rented mind answered itself and the shell's zero-process redirect into the
form-find spool land none. Who asked is read from the process's own ancestor line: under a rented agent, the session id its argv carries or its word (`rented:claude`), so two sessions whose agents carry no id read as one; a kernel a session detached to launchd reads detached and is counted by nobody, and the person's ask and the schedule's are one lane. A stamp says a slot began, never that it finished, and
`.hearth/walk.began` keeps one row per door, so a slot older than its door's last stamp reads only through a landed row; the retry's class stands
beside an unread slot as a note, because a killed retry leaves no row. A feature's ledger that carries no coordinator reads a session's run and the
walk's alike, and a row's clock proves nothing: the slots' windows are ten hours of a day, and on 2026-10-31 a session's reading at 22:07 read as the schedule's. So
such a row counts only when a native landing on the ref added it (`by_schedule`, exit-proof.bml `ep-landed-rows`). A session that forged a movement's subject and its crossings
row would still read native. The walk's own `.hearth` files, which no landing carries, are read by the row's time inside a slot's window (`on_slot`): an inference, not a witness. A turn's landing reads closed only when a gate runs the band of each code file it
touched (`landed` with `gated`): on 2026-10-31 19:25 the rung's one-value wrapper cf090e073 re-landed a wrong line a session had repaired the day before, its gap's band green,
`form-cli-check-witness-band` at 959, the gates full. A band-first row says its own band went green, never that the rest of the body stayed whole.

## The register

Each run prints one line per gap

```
[offline-voice-gaps] <id>: <verdict> — <title>; <observation> (<kind> <source>; serves: <goal>)
```

and one summary line, which is also the door's value, and appends one row to
`receipts/offline-voice-gap-register.jsonl`: `at_unix_ms`, the gaps file, the counts (`gaps`, `closed`, `open`,
`unread`) and `verdicts`, each gap's id, kind, verdict, observation, source and goal. Two rows side by side are the
movement: a gap that was open and reads closed, a count that fell. The register is the only file the door writes. Rows `exit-E1`
to `exit-E6` read the last row of `receipts/exit-proof-ledger.jsonl`, which `observe/exit-proof-run.bml` writes.

## Reading it from a band

The door takes one optional JSON line on stdin: `gaps` (another gaps file), `root` (a directory the sources resolve
under, and the repository git reads), `walk` (the walk checkout, in place of the one the day-turn plist names; `""` is none),
`register` (another register path), `ref`, `now_ms` and `zone_ms`.
