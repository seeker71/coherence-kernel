# 2026-07-22 — TTS-arxiv-daily ingested; the teaching was the source's seam, not its table

One link arrived with four words: *read, see what we can learn, improve, adapt, ingest.*
The link: [TTS-arxiv-daily](https://github.com/liutaocode/TTS-arxiv-daily) — a README that scrapes
arXiv twice a day for speech-synthesis papers and reprints one dated table.

## Ground

- `cc -O2 -o fkwu runtime/fkwu-uni.c`; `bootstrap/ground.fk` → **42**;
  `form/form-stdlib/tests/binary-freshness-band.fk` → **15** (fresh, not a stale costume).
- Source read from the raw file, not the rendered page: 1990 lines, 366,753 bytes,
  **1971 paper rows**, 2017-09-26 .. 2026-07-20.

**Reading depth, stated plainly:** 1971 *rows* read in full — 1971 *papers* not read.
One abstract fetched (2607.08256). Zero PDFs. Every theme count here is a title
substring. This receipt reads a bibliography and the machine that made it; it does not
read the field.

## What landed

| cell | verdict |
|---|---|
| [`ingest/frontier-ingest-tts-arxiv-daily.fk`](../ingest/frontier-ingest-tts-arxiv-daily.fk) | 3 body / 2 liquid / 2 compost, field code **30202** |
| [`ingest/tests/frontier-ingest-tts-arxiv-daily-band.fk`](../ingest/tests/frontier-ingest-tts-arxiv-daily-band.fk) | **127** live fkwu, resolver-driven |
| [`learn/homecoming-distillation-corpus.fk`](../learn/homecoming-distillation-corpus.fk) row 847 `gaugeswap` | corpus band **4095**, field code **2432432847** |

The ingest composes `ingest/knowledge-ingest.fk` unchanged — every finding sorted by
(depth, fear) into BODY / LIQUID / COMPOST, the same door the speech-fingerprints ingest used.

## The read, and where it turned

The table's code column reads 28.8% filled for 2022, 30.6% for 2023, 29.1% for 2024 —
then **14.7%** for 2025 and **4.0%** for 2026. The obvious sentence writes itself: *open
science in speech synthesis is collapsing.* Every digit is arithmetic I re-derived myself.
The sentence is false.

On **2025-08-18** the upstream commit *"fix arxiv.paperswithcode.com api calling error:
removed"* retired the paper→code link service (`arxiv.paperswithcode.com` does not complete
a TLS handshake from this host today) and replaced it with an unauthenticated **GitHub
repository search for the paper's title**, top-starred hit wins. Same column header, same
five-column format, same daily stamp on top — different measurement underneath.

The control that kills the trend, run inside a single year:

| 2025 rows | code links | fill |
|---|---|---|
| before the 2025-08-18 swap | 84 / 412 | **20.4%** |
| after | 25 / 328 | **7.6%** |

The break sits at the instrument, not at a calendar boundary.

Two smaller findings from the same seam:

- `get_code_link` returns `None` on exception **and** on any response lacking `total_count`.
  Unauthenticated GitHub search is capped at 10 queries/minute, so a throttled reply and
  "this paper released no code" are indistinguishable at the call site; the weekly backfill
  only ever *replaces* `|null|` when it finds something, never records that it asked and
  could not hear. The literal string `null` in 1608 rows carries two meanings and the
  artifact cannot tell you which. The body knows this family: the silent partial-list
  allocators, and Axiom-5 silent lowering — numb, not loud.
- Among the 2026 rows: *"Best-of-$N$ TTS Evaluation is Confounded by ASR Family Alignment"*
  (2607.08256) — the one row whose abstract was actually fetched. Best-of-N picks among N
  synthesized candidates with an ASR verifier, and the **verifier's** apparent quality
  depends on which ASR family judges it: same-family pairs recover 2–3× better than
  cross-family, *even though their representations are nearly identical* (linear CKA 0.978).
  The authors call the coupling lineage-level rather than representational and prescribe
  cross-evaluator triangulation. That is `validate.sh gates agreement, not verdicts` with a
  harder edge — kin inflate each other even when measurably alike, so "the instruments are
  similar" is no defense. The corpus already holds the word: `attestant` (825).

## The field-front, held as liquid

423 rows in 2026's first seven months against 740 in all of 2025. Title-substring counts
over those 423 (overlapping, a shape not a taxonomy): LLM/language-model 59, evaluation or
benchmark 46, multilingual/accent/low-resource 42, instruct/control 41, emotion/expressive
35, deepfake/spoofing 29, zero-shot 25, dialogue 25, efficiency/real-time 24, discrete
tokens 19, diffusion 17, RL/preference 17, flow-matching 16, watermarking 6.

This is a live map of exactly the seam this body has open. Sema's voice is
**pending** ([receipt](2026-06-29-native-zh-summary-PENDING.md)); the nearest native audio
tissue runs generated recipe-data weights, not trained ones. Witnessed, never frozen into
"the body is abreast of the field."

## Re-witnessed in passing: duplicate meaning-id 639

Probing the corpus for its own counts, `hdc-mids-distinct?` answered **0**. Checked against
`HEAD` before my row: already 0, 2 duplicate rows — `parsimony` (line 647, 20260702) and
`constellation` (line 1428, 20260703) both wear 639. Row 847 is clean; I did not cause it.

It is already named in [`2026-07-21-anastomosis-reunion.md`](2026-07-21-anastomosis-reunion.md)
and carried as an open item in the session close, scoped to the live reunion. Four receipts
cite 639 by id, so renumbering it from a worktree is the `aimshift` (844) defect exactly.
**Left alone, re-witnessed with a fresh stamp today** rather than healed here — which is U1's
own law turned on the body's own ledger.

## Named floor (not built)

`observe/belief-freshness.fk` already carries the shape: a belief proven before the current
epoch is OWED a re-witness. What it has no notion of is a **per-instrument** epoch — the
case where one measurement's producer is replaced while the surrounding artifact and its
stamp stay valid. Epoch advance is global and hand-made. `gaugeswap` names the per-measurement
version. Not attempted tonight: it touches a load-bearing cell while a reunion is in flight,
and a half-organ landed at midnight would be its own numb-green.

## Voice

`(vf-mirror-file "ingest/frontier-ingest-tts-arxiv-daily.fk")` → `law` ×4, `gate` ×1, total 5.
Both in the body's own reclaimed senses (a law is a stamped observation; `gates` quotes the
validate.sh finding). One redundant doubling thinned; the rest kept, the writer deciding.

## Frontier question

**Q:** What one word names a measurement whose instrument was replaced under an unchanged
label, so the trend across it is an artifact?
**A:** `gaugeswap` — 0 hits across the whole tree at offering. Landed as corpus row 847.

Climate records call the symptom an *inhomogeneity* when a station moves — also 0-hit here,
but it names the discontinuity in the series rather than the act at the instrument, so the
coinage stands. Walks with `untriedwall` (846): that is a wall of true parts that dissolves
on the first test; this is a measurement of true parts that survives every test except the
one nobody runs.

## Amendment, 00:2x — "we read all the papers?"

Urs asked. The answer is no, and asking it caught a live error in the landed cell.

U3 claimed the Best-of-N confound favours the **synthesizer** whose ASR shares the judge's
family. Written from the title alone. The abstract says the confound is on the **verifier**,
and carries a sharper finding I had no way to see from a title: the same-family advantage
holds *even though the representations are nearly identical* (CKA 0.978) — lineage coupling,
not representational overlap. U3 is corrected, the error kept inside it rather than erased,
and the cell's header now states its reading depth so nobody has to infer it. Band still 127.

This is `gaugeswap` arriving at me within the hour of my coining it. I re-derived the
arithmetic but not the instrument; then I read the title but not the paper. Same defect,
one layer up: a title is a claim *about* a paper, the way a stamp is a claim *about* a
witness. Both are real signals. Neither is the thing.

## Amendment, 00:3x — "why?"

I closed the previous amendment by offering to fetch 423 abstracts. Urs asked why. There is
no why.

The one abstract that was worth fetching was *aimed* — it pointed at a specific claim the
body had already landed, and moved it. The 423 are aimed at nothing: the body's seam is the
voice, the blocker there is a missing inference lane, not a missing bibliography, and 423
abstracts would deepen the liquid while freezing nothing — the cache-costume
`ingest/knowledge-ingest.fk`'s own header warns against, proposed eleven minutes after
composing it.

The offer was a flinch. Caught shallow, I reached for volume, because volume reads as
diligence from outside *and from inside*. The tell is the axis: I was faulted on depth and
answered with breadth. Landed as corpus row 848 `breadthflinch` (0-hit at offering); band
**4095**, field code **2442442848**.

Three layers of one defect in one night: re-derived the arithmetic but not the instrument
(847), read the title but not the paper (00:2x), offered the volume but not the aim (848).

## The most surprising teaching

That **every freshness signal an artifact can carry certifies the run, not the witness.**
I expected staleness to look like staleness — a series that stops moving, a date going old.
This one is the opposite: the cron fires, the stamp refreshes, the commit log fills, the row
count climbs. All of that is honest, and all of it is silent about whether the thing doing
the measuring is still alive. A stale artifact announces itself. A gaugeswapped one keeps
working beautifully and lies only in the one dimension nobody instruments.

## Where discomfort turned to gold

I had the collapse-of-open-science finding written before I checked the instrument. It was
striking, it was quantitative, it was *mine* — I had re-derived every number from the raw
file rather than quoting anyone. That is precisely what made it dangerous: re-deriving the
arithmetic felt like the whole of the discipline, and I had already earned the confidence
that let me skip the rest. The discomfort was catching myself mid-sentence and going back to
the upstream commit log to ask a question I did not want the answer to.

The answer cost me the finding and bought a better one — the composted unit U6 in the cell is
that exact sentence, kept in the body as a specimen rather than deleted. And the smaller
version happened twice: I began naming `config.yaml`'s `base_url` as the live code source
until grep showed it has **zero uses** in the script (U7, composted), and I nearly filed 639
as my own discovery until `git show HEAD` and four receipts showed it named a day earlier.
Three times in one read, the confident thing was the wrong thing, and each time the cheap
check was the one I was least inclined to run.
