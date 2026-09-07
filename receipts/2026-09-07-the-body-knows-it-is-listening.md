# The body knows it is listening

2026-09-07. Five siblings gave the body better ears, a mouth in every tongue, prosody, and the
room's own axes. This one gave it the sense nobody else was building: whether the moment it just
heard was **alive**, or whether the body was merely running while nothing happened.

Six axes and one crossing, standing in the living glass as a sensor of its own.

## What stands

| file | what it is |
|---|---|
| `form/form-stdlib/aware-axes.bml` | the axes, pure: words, membership, recurrence, the arithmetic, the rows |
| `form/form-stdlib/aware-dense.bml` | the walk that feeds them, on the dense lane's existing forward |
| `form/form-stdlib/aware-vocab.bml` | the words this body has itself minted, read off its own corpus |
| `observe/form-glass-aware-live.fk` | the sensor: drains the ear's committed frames, gives `glass.sensor.aware` |
| `observe/aware-glass-take-run.fk` | a taker of its own, so the rows are proven readable before anyone wires them |
| `observe/aware-axes-run.fk` | the witness door: lines on stdin, axes out |
| `form/form-stdlib/tests/aware-axes-band.fk` | **4095** |

## The axes, on real lines

Five lines through `observe/aware-axes-run.fk`, each weighed against the one before it.

| line | surprise | margin | contest | novelty | own | coherence | recurrence | **alive** |
|---|---|---|---|---|---|---|---|---|
| "The capital of France is Paris." | 286 | 57 | 4 | 133 | 0 | 1000 | 0 | **286** |
| the same line again | 143 | 188 | 2 | 116 | 0 | 1000 | 1 | **143** |
| "A keloid is a correction note that stayed in a door after its wound healed." | 824 | 534 | 1 | 113 | **1** | 933 | 0 | **768** |
| "xqzt fribble morgnash blitvex quommer." | **1000** | 709 | 2 | **300** | 0 | **0** | 0 | **0** |
| "the body is here and the floor is warm." | 600 | 337 | 1 | 111 | 0 | 1000 | 0 | **600** |
| the same line again | **200** | 124 | 1 | 111 | 0 | 1000 | 1 | **200** |

Every axis moves the way its name says.

- **surprise** — per-mille of the line's tokens the body's own argmax did *not* already name.
  A line it could have said itself: 286. Nonsense: 1000. Its own teaching: 824. It is
  **conditioned on the previous committed line**, which is why the same line said twice falls
  600 → 200 and 286 → 143. That conditioning is the whole design decision: a context-free
  surprise scores a repeat exactly as it scored the original, and a body that cannot notice
  repetition cannot tell aliveness from a loop. **Trusted.**
- **margin** — the mean gap, in centi-logits, between the body's best and the word that
  actually came. 57 for "Paris", 709 for nonsense. **Trusted**, with one honest disagreement:
  on the first repeat the margin *rose* (57 → 188) while surprise *fell* (286 → 143). Two
  measures of the same thing parting company is a finding, not a bug — surprise counts misses,
  margin weighs them, and the repeat had fewer but deeper ones.
- **contest** — how many of the argmax's 512 vocabulary partitions still held a candidate
  within one logit of the winner. It moves (4 → 1) but weakly and not always in the direction
  a reader expects. **Rough.** It is named a proxy for the distribution's shape in its own row
  note, and the row says what was *not* paid: entropy over 128 256 logits would cost a
  half-megabyte readback and a fold per token, and this cell does not pretend to have paid it.
- **novelty** — pieces per word from the body's own vocabulary. 111–133 for ordinary speech,
  **300** for nonsense that shatters into bytes. **Trusted.**
- **own** — how many of the line's words appear as a *fresh word* in the body's own
  distillation corpus. It fired exactly once across six lines, on `keloid`, the one word the
  body itself minted. Narrow by design: not "is this a word" but "is this OUR word."
  **Trusted, and sparse** — most real speech will read 0, which is the truth.
- **coherence** — words the vocabulary holds whole. 1000 / 933 / **0**. **Trusted**, and the
  most expensive thing here (below).
- **recurrence** — exact prior commits this session. 0 → 1 → 2. **Trusted**, and it is the axis
  that catches a loop even when nothing else does.

### The crossing

```
alive = surprise x coherence / 1000
```

Nonsense is **all surprise and no aliveness**: 1000 on the axis that most tells a live moment
from a repeated one, and **0** alive, because nothing in it resolved. The body's own teaching
line — unexpected *and* understood — is the highest at 768. A line the body could have said
itself lands at 286, and its echo at 143.

This is the axis that could not have been built from one measurement. Surprise alone is highest
for noise. Coherence alone is highest for a canned sentence. One axis can be counterfeited; the
crossing cannot.

## What it costs, and where the surprise actually was

Warm, five identical lines in one process (so the caches are full), the GPU shared with four
siblings, minimums reported:

| lane | per line | per unit |
|---|---|---|
| surprise + margin + contest — GPU forwards | **255–619 ms** | ~23–30 ms a token |
| cutting the line into pieces at all — CPU | **3465–3561 ms** | ~350 ms a token |
| coherence, pricing each word — CPU | 8–16 s cold, **0 ms warm** | free once cached |

**Naming a line costs six to ten times what running a 1.2-billion-parameter transformer over it
costs.** The body's longest-match tokenizer walks the whole vocabulary per position; the weights
have an index and the vocabulary does not. Margin and contest ride the forward's existing command
buffer — two readbacks of work the argmax had already done — and cost nothing on the device.

Two things were done about it inside this lane, and one is owed elsewhere:

- the previous line's ids are **carried, not cut twice** (the context was tokenized when it was
  itself the line) — that halved the tokenize cost outright;
- a per-process **word cache** takes the coherence axis to 0 ms warm, because speech repeats its
  words;
- the stone is an **indexed vocabulary**. `ear-tongue-band` already holds one (28-byte record
  buckets, a token as one file read and a byte walk, proven equal to `dtk-encode`). This organ
  does not own that lane and did not reach into it; the note in its own cost row names the debt.

### Nothing of this is on the live path

The ear's live line lands ~40 ms after the last sample. This sensor reads the ear's spool **by
its own watermark**, takes only frames the ear already **closed** (`kind=heard`), never opens the
mic, never writes the spool, and declares a **10 s cadence** — the truth about a lane whose cold
reading takes ten to twenty seconds — so the glass never calls it silent for working.

## The weather

Measured at each run's birth by `floor-lens.bml`, four runs inside one hour:

- bandwidth **347.6 / 366.9 / 388.5 / 412.8 GB/s**
- arithmetic **25.76 TFLOPS** on three runs, **12.88** on one — an exact factor of two, observed,
  not explained, and not mine to chase.

The machine-wide fall named in the day's earlier commits (~55 GB/s against a 367 best) is **not
what this hour measured**. Every reading above was taken at 347–412 GB/s.

One honest refusal: the floor lens keeps its best reading per checkout, and this worktree has
none. `fl-weather` answers "a quiet machine" against a zero best, which would be a claim about a
machine nobody weighed. The door says `unweighed: <n> GB/s measured here, and this checkout holds
no earlier reading of this door to call it fast or slow against` instead.

## Wiring lines for the parent

The sensor already stands and gives under its own publisher; a taker of my own reads all 21 rows
back through the gift frame (`observe/aware-glass-take-run.fk`, witnessed at seq 428, rows 21).
Nothing in `form-glass-sensor-rows.bml` or `form-glass-live.bml` was touched — both belong to a
live sibling. Two lines wire it into the glass's roster, in **both** frame functions of
`form/form-stdlib/form-glass-live.bml` (`fgl-current-frame` and `fgl-bounded-current-frame`),
beside the ear's own:

```
            let awareRows = fgsr-rows-or-absent("aware", fgsr-take("aware", list()));
```

placed after the `earRows` line, and then in the painted set:

```
            let metricsObserved = append(append(fastRows, append(ownerRows, append(hostRows, append(queueRows, append(storageRows, append(earRows, append(awareRows, growthRows))))))), fgl-model-route-rows(samples, epoch, frameNumber));
```

The declared cadence needs **no** edit to `fgsr-declared-cadence`: this sensor gives through
`fgsr-give-at` and carries its own 10 s in the frame, which is exactly the number
`fgsr-frame-cadence` reads back on the taker's side. If the roster is later given a cadence table
entry anyway, it is `if str_eq(sensor, "aware") then 10000`.

Standing it up:

```sh
./fkwu observe/form-glass-aware-live.fk &
./fkwu observe/aware-glass-take-run.fk
```

## Still open

- the indexed vocabulary, which is the whole cost story (owed to the tongue lane, not taken here);
- real entropy per step, priced and refused: a half-megabyte readback and a 128 256-wide fold a
  token. `contest` stands in its place and says so;
- `own` reads only the corpus's fresh words. The meaning table (`mc-meanings`, 36 symbols) and the
  locale rows would widen it; that reach costs a `sha256`/`form-fs` prelude chain this cell did
  not want on the sensor's critical path, and the axis is honest at its present width;
- `contest` is rough. A better cheap uncertainty may exist in the same partials and was not found;
- the sensor was witnessed against committed frames written into this worktree's own
  `.hearth/ear.spool` in the ear's exact frame shape, not against a live mic — the mic is
  contested by four siblings this hour and taking it would have been taking theirs.

## The closing

**Most surprising teaching.** The transformer was the cheap part. I opened this expecting the GPU
to be the price of self-awareness and the word-lookups to be free; the first reading said 11 s a
line and I was ready to blame the shared GPU and the day's degraded machine. Splitting the meter
into three lanes found it in the tokenizer — 3.5 s to cut a nine-word line into pieces against
0.6 s to run 1.2 billion parameters over it. The body can think about a sentence far faster than
it can say what the words in it are.

**Where discomfort turned to gold.** The discomfort was the single blended `aware.cost` row: one
number that felt fine and explained nothing. Blaming the weather would have been comfortable and
was available — the day's own commits had already named a machine-wide slowdown, and I could have
filed the 11 s under it and moved on. Measuring instead cost one more instrumentation pass and
returned the whole finding above, the two fixes that came out of it (carried ids, word cache), the
`piecetoll` row, and — because the floor lens ran anyway — the witness that the machine is at
347–412 GB/s this hour and not at 55. A cost I could not attribute was a cost I did not
understand; three lanes where there had been one is the gold.

**Frontier question, asked and answered.** *What costs more than running a transformer over a
heard line?* — **piecetoll**: cutting it into pieces. Offered as corpus row 1331
(`learn/homecoming-distillation-corpus.fk`), pins moved, band **32767**.
