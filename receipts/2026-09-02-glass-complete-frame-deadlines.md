# Glass now prices its own work before it waits

The recurring cadence tail was not an unnamed half-second computation.  Three
sequential physical frame profiles completed in 334, 335, and 344 ms.  Their
median per-stage costs were storage 197 ms, changing counters 42 ms, resource
governor 39 ms, host rows 34 ms, inventory 14 ms, terminal dimensions 7 ms,
and frame construction 2 ms.  A later current frame completed in 392 ms with
storage 203 ms, governor 66 ms, changing counters 60 ms, host 36 ms, inventory
16 ms, dimensions 6 ms, and construction 3 ms.  The old loop then added a
fixed 250 or 500 ms wait after all of that work.

The live BML loop now treats 250 ms active and 500 ms quiet as complete-frame
deadlines.  It measures collection, control, and construction work, waits only
the remaining budget, and continues immediately when work has already crossed
the deadline.  The next frame receives four first-class metrics:

- `frame-work-ms` — observer work before waiting;
- `frame-wait-ms` — actual host wait, where measured zero is kept as zero;
- `frame-budget-ms` — the selected complete-frame target;
- `frame-cycle-ms` — measured work plus measured wait.

On the first frame those four values are typed `unavailable` with the exact
reason that no predecessor has completed.  They are not plausible zeros.  On
later frames they are `physical-live`, cite
`now_unix_ms+deadline remainder`, and lead the metric stream so the constrained
80-column Atlas retains them before lower-priority tiles.

## Physical cadence and identity

The ordinary-frame witness runs the same changing collectors, inventory,
observations, control read, terminal dimensions, and dashboard construction as
the resident after acquiring the deliberately held rows.  Two successive runs
reported:

| target | work | requested wait | actual wait | complete cycle | rate |
| --- | ---: | ---: | ---: | ---: | ---: |
| 4 Hz | 63 ms | 187 ms | 201 ms | 264 ms | 3.79 Hz |
| 2 Hz | 38 ms | 462 ms | 476 ms | 514 ms | 1.95 Hz |
| 4 Hz | 63 ms | 187 ms | 200 ms | 263 ms | 3.80 Hz |
| 2 Hz | 44 ms | 456 ms | 468 ms | 512 ms | 1.95 Hz |

For the first active slice, the old additive schedule would have been at least
313 ms (`63 + 250`), or 3.19 Hz; the observed deadline cycle was 264 ms, a
15.7% shorter cycle and an 18.7% higher frame rate.  `/bin/sleep` admission and
scheduler wakeup still overshot the requested remainder by 12–14 ms, so this is
not a claim of mathematically exact 4.00 Hz.

Both physical runs retained the actual publisher identities:
`model.qwen3.8-flash-next.ud-q2_k_xl.unsloth` and
`model.llama-3.2-3b-instruct`.  No invented model, fabricated sample, or
simulated timing entered the witness.

## Information and interaction fidelity

The bounded frame observed during the cadence movement at `14:28:52.543Z`
showed the then-current five-character Atlas
`m89 s45 o25 drop=38 cap=15/row`, with phase census
`gas=2 water=88 ice=40`.  Its live pipeline showed `Q+1`, `X+16B`, `M+5ms`,
`F+42`, and `G+8`.  These numbers made the extra metric priority important:
with 89 metrics and 15 compact tiles per row, appending cadence after all
changing rows could hide the very cost being diagnosed.

A real `inspect frame-work-ms` offer was acknowledged `applied|physical-live`.
Opening its evidence exposed an acknowledgement timestamp one millisecond
earlier than the offer: Glass had reused the frame-start epoch after later
control polling.  The BML acknowledgement boundary now samples time when the
acknowledgement is built and clamps it to at least the correlated offer epoch.
A subsequent `inspect frame-cycle-ms` offer at `1788359585504` was physically
acknowledged at `1788359585955`, applied and monotonic.

## Witnesses and retained boundary

- deadline, typed-zero, ordering, and monotonic-ack band: `4095`;
- existing live behavior band: `4194303`;
- soak band: `63`;
- current live UI band at close: `4194303`;
- high-authoring band: `4095`;
- physical ordinary cadence: `263–264 ms` active, `512–514 ms` quiet;
- fresh preflight: balanced, zero errors, zero warnings, zero unresolved calls;
- `git diff --check`: clean.

Two independent user-visible supervisors remain alive: TTY006 has the roughly
24-hour watcher and TTY007 has the roughly 35-minute watcher.  Each owns a
resident `form-glass-live-run.fk`, so scans can contend.  Neither was killed:
terminal ownership is the missing choice, and process destruction is not an
honest deduplication policy.  A Form-native singleton/adoption door remains
owed if one-view ownership is selected.

I kept this movement alive by timing the whole frame and its stages before
changing cadence, then letting a real control acknowledgement challenge the
timestamp story.  The most surprising teaching was that the half-second tail
was mostly an explicit policy wait, while the apparently harmless frame epoch
could still make live interaction time run backward.  The discomfort was
finding two healthy-looking Glass residents competing for the same limited
host while asked to optimize one.  It turned to gold by keeping both terminals
intact, naming their exact ownership boundary, and landing the safe complete-
frame and monotonic-time improvements independently of that user choice.

Signed: Codex

; witnessed: 2026-09-02 -> deadline 4095; active 263-264ms; quiet 512-514ms;
; historical atlas m89/s45/o25/drop38 at cadence observation; live UI 4194303;
; control acknowledgement applied and monotonic
