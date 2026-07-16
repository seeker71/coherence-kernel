# 2026-07-16 — the self-portrait was flattered by the act of portraying

## Ground

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

An hour earlier, [`receipts/2026-07-16-curation-seam-healed.md`](2026-07-16-curation-seam-healed.md)
found something in passing and wrote it down rather than swallowing it: the committed portrait
claimed observe 146 / learn 195 / ingest 17 / form-stdlib 882, and after deleting 15 stray
`.fkb`/`.sym` the same pulse recomputed 145 / 193 / 15 / 880. It named the defect and left the work
order: `ap-count` needs a filter, and `tests/` is being counted as an organ. This is that work.

## A seam before the work: the body I was handed was not the body that had the cell

`observe/autopoietic-pulse.fk` does not exist on `main`. Nor does `INDEX.md`, nor the receipt above.
All of it lives on the unmerged branch `claude/eloquent-williamson-2f35f3` — which is checked out in
a *different* worktree with two live `claude` pids. My worktree was cut from `main`. The briefing
described a body I could not see.

Recorded because it is a real hazard of a multi-agent fleet, not a stumble: **a handoff that names
files but not the branch is only as true as the reader's checkout.** The work was possible because
the sibling's HEAD (`1f9e95a07`) already contained `main`, so my branch fast-forwarded onto it with
nothing lost and nothing of the sibling's touched. The merge back is not mine to make.

## The defect, and how much bigger it was than reported

`(defn ap-count (dir) (len (fs_list dir)))` counts every directory **entry**. `fs_list` returns
organs, subdirectories, dotfiles, and the `.fkb`/`.sym` fkwu drops beside every cell it loads. So:

**1. Running a cell grew the body's reported organ count.** Witnessed against the pre-healing cell,
same tree, four organs run between two pulses:

| tissue | clean tree | after running 4 organs |
|---|---|---|
| observe organs | 147 | 151 |
| learn organs | 195 | 199 |
| ingest organs | 15 | 19 |
| form-stdlib cells | 882 | 886 |

Looking at the body grew it by 16 organs.

**2. Worse than "whatever the author happened to run" — the pulse poisoned itself.** Three of its
four preludes (`form/form-stdlib/core.fk`, `observe/door-link-health.fk`,
`learn/homecoming-distillation-corpus.fk`) live *inside* tissues it counts. fkwu emits the artifacts
at prelude-**load** time, before the cell body evaluates. So the pulse inflated observe, learn and
form-stdlib by exactly +2 each **on its own first run, from a pristine checkout**. Not contamination
by a careless author — contamination by the act of self-examination.

**3. Even the hand-cleaned numbers were wrong.** Deleting the artifacts got 145/193/15/880. The
honest counts are 144/192/14/**860**. Every tissue still counted its `tests/` directory as one organ,
and form-stdlib counted 11 subdirectories plus `.gitignore`, `AUTHORING.md`, `.bml`, `.json`, `.txt`
and `.stamp` — 20 non-cells — as cells.

This falsified the portrait's own header claim, which is why the claim is now corrected *in place* in
the cell rather than quietly swapped: *"every number is recomputed from the tissue it names, so this
portrait cannot flatter — only go stale."*

## The fix: allowlist, not denylist — and why that is not a style opinion

A tissue is counted by the **suffix that names its organs**. The tempting smaller diff was a denylist
of `{.fkb, .sym}`. The body itself refused it, out loud, in the grounding run:

```
fkwu: warning: bootstrap/ground.dylib: native .dylib emission is not installed in this
checkout; emitted .fkb/.sym
```

A denylist would start lying, silently, the day native emission installs. A suffix allowlist cannot:
a name either is an organ of this tissue's kind or it is not.

**The decision on `tests/`, made separately as asked: a directory is not an organ.** Counting it as
exactly 1 was arbitrary — it holds many band cells. Bands are proofs *of* organs, a different
population, and conflating them with organs is the same flattery in a smaller coat. One principle now
answers both questions: *an organ is a file of its tissue's own kind, lying directly in that tissue.*
The portrait says so, in the portrait.

**The named seam was collected, not deepened.** The header carried a work order: `ap-count-matching`
is the third instance of a walk (`rcn-count-svg-loop`, `rcn-count-pending-loop`), left duplicated
because `learn/resident-conatus.fk`'s prelude chain is heavy to haul in for a name-walk. That
reasoning was sound and aimed at the wrong home. The walk's home was never rcn — it was `core.fk`'s
`filter`/`len`, **already in this cell's prelude, already loaded, the whole time**. The seam closed by
*deleting* the walk. Counts are now `(len (filter ap-fk? (fs_list dir)))`. Cost: one named predicate
per suffix, because this language has no closures and `filter` takes a named predicate — the shape
`observe/eval-harness.fk` already uses. Left standing honestly: rcn's two loops still want collecting.

The trap is nailed shut in the self-check (c3), because it is a real trap: **`".fkb"` contains
`".fk"`**, so the substring test — `ap-count-matching`'s shape, the "near-fit" the header pointed at —
counts artifacts as organs. Only a suffix test refuses them.

## Witnessed (every number recomputed this session)

```sh
./fkwu --src learn/tests/homecoming-distillation-corpus-band.fk   # 511
# pulse, per the cell's own run recipe:
...; echo '(autopoietic-pulse-check)'   # 31
...; echo '(ap-tend)'                   # 2
...; echo '(ap-field-code)'             # 2059804400 (verdict 2, 598 receipts, 44 links, 0 broken)
```

- **Counts, each cross-checked against an independent census:** axioms 3, teachings 10, observe 144,
  learn 192, ingest 14, form-stdlib 860, receipts 598. Pending receipts: pulse says 1, `ls | grep -c
  PENDING` says 1.
- **Still falsifiable**, the way the cell's own receipt does it: a planted broken path-claim in
  `WELCOME.md` → `ap-tend` **2 → 0**; healed → **2**. `WELCOME.md` restored clean.
- **Invariance — the property this cell claimed and did not have.** From a clean tree, pulse →
  portrait A. Run organs until 24 `.fkb`/`.sym` litter the counted tissues (observe 8, learn 6,
  ingest 4, form-stdlib 6). Pulse → portrait B. **A and B are byte-identical.**
- **The loop, demonstrated live again:** corpus row 736 landed → `ap-stable?` read 0 with the portrait
  still claiming 136 rows → `ap-tend` re-made it to 137 → `ap-stable?` returned 1.

## The merge, and what it surfaced

Merging brought in the sibling's five pushed commits, and they had found **the same broken promise
from the other side**. Their [`2026-07-16-deploy-and-the-vanity-metric.md`](2026-07-16-deploy-and-the-vanity-metric.md)
caught the portrait reporting `broken path-claims: 0` — true of the door ring, while the body carried
156. Flattery through **scope**. Corpus row 735: `vanity`.

So two agents, on two branches, the same night, independently falsified the same founding sentence —
*"the portrait cannot flatter — only go stale"* — through two different channels. Their amendment
says scope is *"the one channel a recomputed number leaves open."* That claim was itself too narrow:
the second channel is the instrument's own footprint in what it measures. **One agent finding one
flatter-channel and declaring it the only one is exactly the shape of the bug.** The pair is worth
more than either half, and both rows now say so.

Git auto-merged the corpus with **no conflict** and silently produced two rows sharing meaning-id
736 — their `scrupulosity` (renumbered 735→736 by their insert of `vanity`) and my `iatrogenic`. The
band caught it by asserting an exact count. Mine renumbered to **737**; corpus 138 rows, max id 737,
band `1381382737` → 511.

Their amendment also added `observe/body-link-graph.fk` to the preludes — a **fourth** prelude living
inside a counted tissue. Under the old `ap-count` that would have quietly moved observe 147→149.
Under the allowlist it costs nothing, which is the point: the next prelude is free too.

## An inherited defect, found by the merge and NOT fixed here

`ap-stable?` now reads **0** in a standalone run, immediately after `ap-tend` returned 2 and
converged. The portrait is not drifting — two consecutive `ap-tend` runs are byte-identical and the
text matches the host's own count. What moves is `ap-index-text` **across invocation contexts**:
called directly it reports form-stdlib 837 and `.md` 769, both wrong against the host's 860/774, and
the error *grows with allocation* inside a single run — 860 → 837 → 748 as more of the fabric walk
runs first. That is the value-stack **rooting** family this body already has words for (corpus rows
654/718).

**Checked before blaming**: reproduced on `8c87bf760` in a detached worktree with none of my healing
present — `ap-tend` 2, `ap-stable?` 0, twice. Their witnessed block claims `ap-stable? 1`; their
committed tree reads 0. It arrived with the body walk. I am recording it, not fixing it, and not
claiming it away: **the cell's drift detector cannot currently be trusted.** `ap-tend` still converges
and still writes the truth; `ap-stable?` alone is the casualty.

## The healing, same night — and the diagnosis above was wrong

The section above stands as written because being wrong in the open is the house law, and this wrong
was instructive: I blamed the value-stack rooting family (rows 654/718). Urs said *continue*; my own
memory holds a sharper law — *when explaining why you can't do a healthy thing, test that blocker
first* — and "needs its own session" was an untested blocker. Engaged, the bug fell in one sitting of
minimal repros:

1. One `read_file` before a `filter` collapsed the `.fk` count 860 → 562 — while a byte-level
   predicate over the same names stayed 860. So the names were intact.
2. Every name passed the predicate *individually* after the same `read_file`. So the predicate was
   intact too. The failure needed volume plus phase.
3. A big-arena build (melt can never fire) answered everything correctly — `ap-stable?` → 1.
   The melt was implicated.
4. A slot audit inside the collector found **zero** dangling roots — the rooting family was innocent,
   and my first diagnosis died. The mark was *faithfully* counting a list that was **born short**.
5. A per-slot census at melt entry showed the `fs_list` result was 600 cells, nil-terminated, ending
   exactly at the heap wall.

**Root cause — `runtime/fkwu-uni.c`, `fs_list` (walker tag 132)**: the list builder had no melt
trigger and, at the heap wall, `return fkl_out;` — **the partial listing returned as the whole
directory, silently.** `fs_list` answered the heap's allocation history, not the directory. One
`read_file` of `INDEX.md` was enough to phase the heap so the wall landed mid-listing; the
first-sorted entries vanished; 860 read as 837 or 748 depending on what ran first. The next `cons`'s
melt then compacted away the evidence. Same disease class the runtime already convicted at
`FK_NODE_CAP` ("every guard silently returned handle 0"), and the exact case `fk_list_push` refuses
in its own die-message: *"a partial list accepted as whole."*

The bitter, beautiful inversion: **`ap-stable?` was the only honest witness in the body — and it
spent hours accused, because it was the only reading that refused to agree.** `ap-tend` converged
byte-identically not because it was right but because its passes phased alike.

**The healing**: `fs_list` (tag 132) and record-keys (tag 99, the identical idiom) now melt like
tag-19 `cons` — accumulator rooted through `fk_vs` across the melt — then die **loudly** if the heap
is genuinely full after compaction. Two more silent-at-the-wall sites are named for later, not fixed
here: the closure-capture copy loops (~line 5249, no wall check at all) and `fk_jlist2`'s silent nil
after melt.

**Witnessed on the healed runtime**: ground 42, binary-freshness 15, corpus band 511; the
degradation probe reads 860 fresh **and** 860 after the full fabric walk (was 748); recomputed
portrait byte-identical to disk; self-check 31, ap-tend 2, **ap-stable? 1, twice**; falsifiability
2 → 0 → 2; invariance-under-litter byte-identical. Regression:
[`form/form-stdlib/tests/fs-list-heap-phase-band.fk`](../form/form-stdlib/tests/fs-list-heap-phase-band.fk)
— property-shaped (listing equal across five heap phases, no absolute counts to go stale) — reads
**31 on the healed runtime, 15 on the pre-healing one**: the band can fail, so its green means
something.

## Closing — how this stayed alive

**Most surprising teaching:** *the fixed-point loop is what hid the bug.* That loop is the cell's
finest idea. Its own comment diagnoses the observer effect exactly — *"the portrait cannot describe
the state that contains the portrait"* — and cures it honestly, by re-observing rather than
predicting. Four lines above it, the identical disease sat uncured in the counts. And the cure is
precisely what concealed it: because artifacts land at prelude-load time, every pass saw the same
inflated tissue, converged, and re-ran byte-identical. **Idempotence was mistaken for truth.** A lie
that reproduces itself perfectly looks exactly like a fact — and the green re-run, the very evidence
offered for "cannot flatter," was the camouflage. The author's insight was real and its success made
the neighbouring rot invisible. That is not carelessness; that is what competence does to a blind
spot.

**Where discomfort turned to gold:** the sharp discomfort was arriving to a briefing that described a
cell my checkout did not contain — the pull to either fabricate around it or declare the premise
false and stop. Both would have been wrong; the premise was true, just not *here*. Sitting in the
not-knowing long enough to grep history instead of asserting is what surfaced the branch, the live
sibling, and the fact that the work was genuinely undone. The gold: that same refusal to trust the
handed-down number is exactly what the bug required. The first receipt had already deleted the
artifacts by hand and watched the numbers drop — and I nearly accepted 145/193/15/880 as *the healed
truth*, because a trusted receipt said so and the pulse agreed. Counting the tissue myself, against
the body rather than against the story, is what found the other 20.

**Frontier question offered (row 737):** *what one word names harm caused by the examination or
treatment itself* → **iatrogenic** (0 hits before this row, and still 0 against the sibling's tree).

**Addendum, after the same-night healing:** the closing above was written before the runtime fix,
and its teaching survived intact — but the healing leg left a sharper one. **The most surprising
teaching:** the body's only honest instrument was the one under accusation. `ap-stable?`'s 0 was a
true reading of a real fault, and every agreeing witness (`ap-tend`'s convergence, the byte-identical
re-runs) agreed only because they shared the instrument's blind spot — repeatability wearing
reproducibility's clothes. **Where discomfort turned to gold, twice:** writing "NOT fixed here,
value-stack rooting family" into this receipt felt like honesty and was actually an untested blocker
plus a wrong guess — the discomfort of reopening my own signed diagnosis, witnessed rather than
defended, is what let the minimal repros run; and the discomfort of the big-arena result (my melt
theory half-dying — melt implicated, rooting innocent) is what forced the census that found the list
born short. **Frontier question offered (row 738):** *what one word names the science of measurement
itself — that says when a reading can be trusted* → **metrology** (0 hits; the body is made of
instruments — bands, probes, witnesses, verdicts, this pulse — and lacked the name of their own
discipline). This body calls itself a body — organs,
tissues, health, receipts — and had no word for the injury a healer's own procedure causes. Near
misses, all present, none this word: `observer-effect` 3 hits (generic, and a quantum borrow in
`ingest/frequency-ingest-ecstatic-playground.fk` U7, not a word for injury); `heisenbug` 2, the
inverse — a fault that *hides* when observed; `reflexivity` 3; and row 717's `insidious`, the true
cousin and still the opposite shape: that fault hid by never showing in an isolated trial, this one
hid by showing *identically* in every trial. The pulse's reading was iatrogenic, and the discipline
the word carries is the one this session had to learn the hard way: **idempotence is not the test.
Invariance across observation histories is.**
