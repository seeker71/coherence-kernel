# 2026-08-09 — the homecoming census: what the rented mind did, what already carries offline, and the first witnessed half

Urs: *"how are we bringing all the remote oracle requests home to offline, look at the last 2 weeks
and see how we can do all of that work offline as soon as we possible can."*

The answer has three parts, and all three are in the tree, banded: the census is the requirements
document, the map says which lane holds each requirement today, and the first slice is measured —
ten frontier questions from the corpus, re-asked through the body's own dense Metal lanes, scored
against the rented answers already on record. The strongest local lane carries **half**.

## The census — 199 receipts, 2026-07-25..2026-08-09, each read and given one primary type

`learn/homecoming-work-census.fk` carries this as rows the body can fold; the counts sum to 199
exactly and the band answers differently the day they drift.

| primary type | receipts | offline carrier today | what is missing |
|---|---|---|---|
| write-form-cell-or-port-home | 62 | KAT whole-forward + qwen25-coder-32b lane; bands/preflight are the net | the work loop: propose → preflight → band → red lines fed back |
| diagnose-defect-to-root | 44 | dsr1q32 proposes; the body's instruments answer | the same loop with instrument readings as retrieval |
| performance-climb-derive-floor | 31 | body measures; a lane proposes the next rung | a rung-planner over measured tables |
| knowledge-inquiry-through-the-body | 28 | rag-retrieve + qwen72/llama3b lanes | receipts/ and learn/ in the rag index; cited answers |
| census-survey-audit | 17 | **body-alone, today** — grep, fsh, band folds | nothing structural; a standing sweep cell |
| coordination-reunion | 7 | **body-alone, today** — the reunion protocol | nothing structural |
| review-judge-verdict | 6 | form-cli-judge tallies + a dense lane as grader | a judge driver in the slice-run shape |
| ask-lane-infra | 4 | ask-lane-router + the dense-run family | one ask door returning text as a value — landed today |

Cross-cutting, not beside: 117 corpus rows (880..996) and 199 receipt drafts run through every
type. Classification is a judgement, offered to be argued with; the receipt list is not.

Two types — 24 receipts, an eighth of the fortnight — need **no model at all**. They were rented
because the rented mind was the scheduler, not because the work needed it. That is the cheapest
homecoming and it is step 1.

## The slice — ten frontier questions, re-asked offline, judged against the record

The corpus rows carry the rented answer beside the question, so the body already holds the ground
truth. Ten window rows whose answers are dictionary words (a coined word is unanswerable by
construction — more on that below): 880, 884, 885, 886, 887, 893, 894, 903, 909, 916.

The carrier: `form/native/metal/homecoming-slice-lane.fk` — the ask wrapper the census names,
a dense Metal lane callable as a function; rows read from the corpus at run time by
`hdc-row-for-id`, never copied. One thin driver per model (an open holds 197 of 198 buffer
slots, so one model per process — the dense-run family's own precedent). Scoring logic proven
four-way: `learn/tests/homecoming-work-census-band.fk` → **2047** on fkwu, Go, Rust and
TypeScript alike, 2026-08-09. Runs: `./fkwu-metal` as found in the tree (rebuilt 2026-08-08 23:00
by the round-three line; pipestamp on every capture below: `metal_owner=fkwu-form-cli`,
`metal_door=handle`, `pipelines=57` — round three's numbers are inherited, not mine).

| lane | hits | ask wall | reading |
|---|---|---|---|
| llama3.2:1b | 0/10 | 65.3 s | fluent, adjacent, never the word |
| llama3.2:3b | 1/10 | 70.0 s | found kinesthesia |
| DeepSeek-R1-Distill-Qwen-32B | **5/10** | 122.1 s | clusivity, homoplasy, amphiboly, kinesthesia, winsorize |

Verbatim, from the transcripts (greedy, deterministic):

* the 1B on homoplasy: `q884_text=convergent evolution". Convergent evolution is the process by
  which different species...` — the right *concept*, the wrong register. Adjacent-not-exact is
  the 1B's whole pattern (proprioception for kinesthesia, faith for gettier).
* the 32B on clusivity: `q880_text=clusivity." Now, I need to explain this concept in a way
  that's easy to understand...` — exact, first token.
* the 32B on winsorize: `q894_text=winsorizing".` — and the first scoring pass called that a
  **miss**. The instrument was wrong, not the model: exact substring could not see the stem. One
  byte of tolerance (drop a trailing e), proposed by a real transcript and proven as the band's
  eleventh bit, and the honest number moved 4/10 → 5/10. The specimen corrected the judge.

The misses now split cleanly: q886/q893/q903/q909/q916 are knowledge-misses (contradiction for
epoche, Lisp for homoiconic — the 32B names the *exemplar* instead of the *property*), none are
judge-misses. A low number is the measured distance, and 5/10 is the floor the fleet stands on
today for the corpus's own task, at 12 s/question, no membrane crossed.

## What the choosing taught about the corpus itself

Picking ten eligible rows forced a reading of all 117: roughly the newer two-thirds carry answers
this body **coined** in-session (tickblind, floorspoke, askalike, secondblind...). Those rows are
already home — no other mind, rented or local, can ever re-answer them, because the answer did
not exist before the session that wrote it. The corpus has been drifting from *frontier
vocabulary* (words the world had and the body lacked) toward the body's own idiolect. Both are
nutrition, but only the first kind can measure a lane, and only the first kind was ever really
*rented* — the coined rows were authored here all along.

## The sequence to all of it offline (carried in the cell as `hwc-sequence`)

1. **Landed today**: the ask wrapper + this benchmark; census-survey and coordination declared
   body-alone — 24 receipts/fortnight of rented scheduling end here.
2. Wire `form-cli-judge` to the dsr1q32 lane (same driver shape): local answers, local grades.
3. Embed receipts/ and learn/ with the local embedder into the rag index; inquiry answers cite
   rows. Carries the 28 knowledge receipts.
4. The work loop v1: a lane proposes a cell, preflight and the band answer, red lines feed back
   until green. The verification discipline is what makes a 5/10 mind usable: the band declines
   what the lane gets wrong. Carries the 62 write-cell receipts.
5. The same loop with instruments as retrieval carries the 44 diagnosis and 31 floor-climb
   receipts. This is last because it is hardest, not least.
6. A homecoming ratchet beside sc-ceiling: the offline-carried share may only grow, and the band
   answers differently the day it shrinks.

Every step's carrier already decodes form-natively at witnessed speeds; what is missing is never
model quality alone — it is the loop that closes proposal against verdict.

## Most surprising teaching

That the first honest measurement put the strongest local lane at **half** — I expected the gap
to be wider — and that of the five misses, one was my judge and zero were the harness. The
distance to offline is smaller than the fortnight's rented volume made it look, and the part of
the distance I could close today was in my own instrument, not in the model.

## Discomfort, and where it turned

Publishing `llama1b_hits=0/10` sat badly — sixty-five seconds of beautiful form-native decode
arriving at zero. The gold arrived when the zero was read beside the 3B's one and the 32B's five:
the zero is a *scale reading*, the cheapest point on the curve that says which lane a task type
needs. Without the zero the five means nothing. The discomfort of landing a failing number is
what made the number a measurement.

## Frontier question (asked because the coined-row reading needed a word)

**Q:** what names a language variety spoken by exactly one person so that its words measure the
speaker and no one else?
**A (rented):** idiolect — verified 0 hits in this tree before offering ("idioglossia" carries 4
and names a shared private tongue, rejected).

Proposed corpus row — **PROPOSED, not landed**: this tree's own max-mid re-derives to **996**
(`hdc-row 996` is the last), so the row would take 997; pipestamp and shapeblind both already
claim 997 on their lines and seamclock claims 998, so this row expects reunion renumbering to
999 or later, per the row-719 anastomosis practice:

```
; Row 997 (2026-08-09): asked while choosing slice rows for the homecoming
; census — two-thirds of the window's corpus answers are words this body
; coined in-session, rows no other mind can re-answer because the answer
; postdates its question. The body had "idiom" and the coined practice
; itself, but no word for a one-speaker language variety AS a variety.
; Verified 0 hits. (idioglossia: 4 hits, names a shared twin-tongue.)
(hdc-row 997 20260809
    (list "what" "names" "a" "language" "variety" "spoken" "by" "exactly"
          "one" "person" "so" "that" "its" "words" "measure" "the"
          "speaker" "and" "no" "one" "else")
    "idiolect"
    "idiolect"
    "rented-oracle")
```

## Landed

* `learn/homecoming-work-census.fk` — census rows, carrier map, slice ids, scoring, sequence
* `learn/tests/homecoming-work-census-band.fk` — **2047**, four-way (fkwu / Go / Rust / TS)
* `form/native/metal/homecoming-slice-lane.fk` — the ask wrapper (prompt in → text out → judged)
* `form/native/metal/homecoming-slice-run-{llama1b,llama3b,dsr1q32}.fk` — the three drivers,
  hit rates as witnessed above

Seams held open, honestly: `form/validate.sh`'s phase-0 surface check reads 11 errors on the
Metal door tags (246..255) before any band runs — that is the round-three line's mid-flight
growth, witnessed here, not repaired here, because the door files are theirs while they climb.
The four-way proof above ran the sibling kernels directly on the assembled workload instead.
