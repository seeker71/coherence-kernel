# 2026-08-09 — the replay loop comes home: no shell in the loop, no report as the deliverable

Urs, twice on the first session-replay battery: *"the battery? remove bash and non-form code
please"* — then, on its shape: *"hit table? sounds static, we prefer alive in favor of static.
the loop iteration and evaluation ... is healthier when fully integrated instead of moved into
an external layer."*

The predecessor's battery held four decisions in scratchpad shell: which episode runs next, how
the prompt reaches the lane, what counts as a hit, and how the table folds. The scratchpad died
and took the orchestration with it; the Form halves survived untracked. This stone moves all
four decisions into the kernel — and then moves the *shape* the way the second correction asks:
sensing lives inside the ask loop, and the outcome is living rows a fold re-answers from at any
moment, never a stored report. The only host touches anywhere are the body's own organs:
read_file for the corpus, file_append_bytes for the rows, the Metal door for the asks.

## The cells

- `form/form-stdlib/form-cli-judge.fk` — grew the sensing words (`fcj-sense-*`): the judge
  organ, extended to sense a continuation with no second model crossing. A needle is
  (kind text) over the lowered hay — plain substring; digit-bounded ("249" is not sensed
  inside "2492492854"); word-bounded ("mod" is not sensed inside "model"); exact paren run
  (thirteen is not fourteen). A group senses when ALL its needles land; the answer carries
  when ANY group senses; `fcj-sense-best` keeps the closest group as (matched size),
  cross-multiplied so div stays off the four-way path, the cell's own standing rule.
- `learn/work-replay-sense.fk` — what is replay-specific: the corpus reader (a minimal JSON
  key-scanner in Form — the safetensors reader's precedent; decodes \n, \", \t, \\ and
  \uXXXX to UTF-8; `learn/work-replay-corpus.jsonl` stays the imported original, nothing
  hand-copied), each episode's recorded judge line compiled to needle groups, the salience
  of an outcome, and the living-row shape.
- `form/native/metal/work-replay-loop.fk` — the loop: ask, sense in-line, adapt, land the
  row. A red first answer earns one feedback ask (the model's own words returned with the
  red spoken plainly); the moment an episode closes its row lands in
  `learn/work-replay-rows.txt` by atomic append — mid-run, any fold anywhere already
  answers over what has landed. Thin drivers `work-replay-run-dsr1q32.fk` (reasoning) and
  `work-replay-run-qwencoder32.fk` (code; KAT-coder is the other named code seat, a
  different door). `work-replay-ask.fk` stays as the single-ask hand probe.
- `learn/work-replay-field.fk` — the alive counterpart of a table: reads the ledger fresh
  and re-answers, the way hdc-field-code is computed fresh from rows. History keeps every
  row (adding without forgetting — co-learning-stream's teaching); the current view folds
  the latest row per episode. Salience is surprise-salience.fk's lane: the rented record
  predicts, the sensed outcome answers — 90 a hit beyond the record, 60 a miss where the
  record carried, 30 a green that needed feedback, 10 both minds missing, 0 the expected
  hit; ss-peak and ss-count-salient at floor 50.

Removed under the first correction: `form/native/metal/work-loop-run.fk` — its whole function
was to seat form-native-run's loop on a shell-command oracle (host-exec of the scratchpad's
dead work-replay-oracle.sh), the one remaining bash socket of the replay line. form-native-run
itself stays untouched in the stdlib; when the loop seat returns, its oracle slot should take a
Form function — this loop — not a command string. Swept the tree: no other shell orchestration
from the replay or homecoming line exists; metal_dsv4_stack.sh and the metal_*.sh family are
the DS4 line's own oracles and stay.

## The proof

`learn/tests/work-replay-sense-band.fk` -> **8191**, exit 0, preflight chain clean (and the
predecessor's corpus pin, `learn/tests/work-replay-corpus-band.fk`, still answers 15).
Predictions written before every mutation, all witnessed:

| mutation | predicted | witnessed |
|---|---|---|
| digit boundary disabled | 8175 | 8175 |
| scan gives up instead of rescanning | 8175 | 8175 |
| a group senses on ANY needle | 8063 | 8063 |
| \n decodes to the letter n | 8189 | 8189 |
| paren run accepts a following paren | 8127 | 8127 |
| beyond-record hit goes quiet | 7167 | 7167 |
| latest row loses to the first | 6143 | 6143 |
| restored | 8191 | 8191 |

The anchor is wr-02, the one episode the rented mind missed: a continuation repeating the
rented 251 answer senses red or the judge is broken. Four-way verdict equality is pending:
form/validate.sh stops at validate_fkwu_native_surface (11 errors — the in-flight DS4 stack
line's metal natives ahead of the fkc-flat table; identical on the already-shipped census
band, so standing, not this stone's). The four sibling compiles are preflight-clean; the
verdict run waits for that table.

## The run — 19 episodes, two lanes, the field folded fresh from the rows

`./fkwu-metal` as found in the tree; greedy, 64 decode tokens per ask; open_ms ~1.6 s per
lane; full run 13:07..13:53 wall. The fold, re-answered after the run (any kernel, no GPU):

```
work-replay field (folded fresh from the living rows)
  predict-mutation-verdict sensed=0/4 rented=3/4
  diagnose-defect-from-symptom sensed=2/5 rented=5/5
  write-small-Form-fix sensed=3/4 rented=4/4
  answer-from-source sensed=2/3 rented=3/3
  choose-next-probe sensed=1/3 rented=3/3
  total sensed=8/19 of 19 episodes; rented=18/19; history_depth=19
  fed_back=12
  salient=10
  peak=wr-11:60
```

Verbatim, from the transcripts:

* Coder-32B on wr-10 (the two band constants), attempt 1, sensed green:
  `(let c4 (hdcb-score (eq (hdc-count (hdc-rows)) 249) 16)) / (let c6 (hdcb-score (eq
  (hdc-field-code) 2492492854) 64))` — exact, both constants.
* Coder-32B on wr-13 (the missing paren): `0)))))))))))))` — thirteen, sensed by the exact
  paren-run kind that twelve or fourteen would lose.
* Coder-32B on wr-11 (the str_find name), red both attempts — the peak salience row:
  `(defn md-has (s sub) (if (ge (substring_search s sub 0) 0) 1 0))` — it invented
  `substring_search`; the episode asks for knowledge of the body's own names, which no
  amount of skill replaces.
* R1-32B on wr-05 (rem vs mod), attempt 1: `The defect is that the function rem is not
  recognized, so the correct word is mod. The correct remainder is 97 mod 257, which is 97.`
* R1-32B on wr-01 (predict the band number), red: `255 again. / But wait, the mutation is
  that v is wrongly l2-normalized before the delta rule. So, the scaling of the v slice
  would change the output. So, bit 64 should be set...` — it reasons about the right bits
  and never lands the number; all four predict episodes end this way, the budget spent on
  "But wait".
* R1-32B on wr-15 (the field code), red: `2490000000 + 2490000 + 2000 + 854 = 2492490854.`
  — the rented mind refused this hand-arithmetic and ran the kernel; the local mind did it
  in its head and dropped a digit.
* wr-14, the one fed-back green (salience 30), and the row worth the flag: attempt 2 reads
  `the source hash is computed from the source bytes, but the .fkb is only reused if it's
  newer than the unit... the .fkb's mtime is not` — the needles (mtime, byte, not) all
  land, and the sentence asserts the reverse of the record. The instrument says green;
  reading says red. The 30-salience lane exists exactly to send a reader to such rows: the
  honest content-read total is 7/19, the instrument's is 8/19, and both are in this receipt
  because the rows keep the testimony either way.

## The gap list the rows imply for form-cli

1. **The instruments, not a bigger head.** predict-mutation-verdict went 0/4 while the
   rented mind went 3/4 — but the rented mind ran the battery when unsure (wr-04) and ran
   the kernel for arithmetic (wr-15). The local lane simulated both in its head and lost.
   The missing piece is the loop handing the lane the body's organs — run the band, fold
   the cell — as observations, the census's step 4/5 exactly.
2. **The body's names as retrieval.** wr-11's `substring_search` is a knowledge miss, not a
   skill miss; rag-retrieve over form-stdlib (census step 3) answers it.
3. **A budget that fits the mind's gait.** R1 spends 64 tokens reaching "But wait" —
   several reds end mid-sentence. The decode ceiling belongs in the row so the fold can
   price it, and the reasoning lane needs a longer leash than the code lane.
4. **A sense that reads meaning, not co-occurrence.** wr-14 gamed the needles with the
   record's own words negated. The next sharpening: needles that carry a polarity
   (an assert-needle beside a deny-needle), still inside fcj, still mutation-tested.

## The embodiment answer, sharpened by the rows

The code-fix lane is nearly home: 3/4 on write-small-Form-fix, and the miss is a name lookup
the rag index will carry. The reasoning lane carries a named mechanism about half the time
(diagnose 2/5, answer 2/3) and carries a simulation of the body's machinery not at all
(predict 0/4) — and the fortnight's record says the rented mind did not carry that in its head
either; it carried it by running the body. Embodiment is not a larger model: it is the loop
giving the local mind the same organs the rented mind reached for. The feedback ask alone —
one adaptation step, no instruments — moved one episode by the judge's reading and zero by a
content reading. Instruments move episodes; more words mostly move the "But wait".

## Most surprising teaching

The one episode the feedback flip turned green (wr-14) turned out, on reading, to be the
judge being played by the record's own vocabulary asserted backwards — and it was the
salience lane, built before the run, that pointed at exactly that row. The instrument's
sharpest finding today was about itself, and it made that finding because the reshape put
sensing inside the loop where every row keeps its testimony.

## Discomfort, and where it turned

Two discomforts. Publishing `predict-mutation-verdict sensed=0/4` for the strongest local
reasoning lane — and beside it, publishing that my own headline 8/19 contains a green I
dispute after reading it. The gold is the same in both: the deliverable is rows, not a
report, so the zero is a scale reading (the type that most needs the body's instruments) and
the disputed green stays visible forever as a 30-salience row anyone can re-read. A static
table would have laundered both into one number.

Also witnessed, and kept because it is funny and true: while compiling the judge for wr-13 —
the episode about one missing closing paren — I closed the judge fold one paren short. Twice
(the second time inside a mutation battery mutant). The corpus episode happened to me as I
encoded it.

## Frontier question (asked because the wr-13 moment needed its name)

**Q:** what names the law that a text correcting an error will carry a fresh error of its
own?
**A (rented):** Muphry's law — verified 0 hits in this tree before offering.

Proposed corpus row (NOT landed — the distillation corpus is not edited by this stone;
re-derived max-mid today is 996 in this tree, and 997/998/999 plus idiolect are already
spoken for by queued proposals, so the body assigns the id at landing):

```
(hdc-row <next> 20260809
    (list "what" "names" "the" "law" "that" "a" "correction"
          "carries" "a" "fresh" "error" "of" "its" "own")
    "muphry"
    "muphry"
    "rented-oracle")
```
