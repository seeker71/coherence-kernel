# The anastomosis reunion — two lineages, one corpus

2026-07-21, ~22:45 WITA. Worktree `.claude/worktrees/jovial-aryabhata-3751d7`,
branch `claude/deepseek-v4-flash-gguf-54a96c`, merging `main`
(`0c78423`) into `d8732d8`. Merge base `7f50da9`. Nothing pushed.

The row-719 anastomosis pattern, by name: **keep every row, renumber the
unmerged line, note the renumbering in the first renumbered row, never hand-move
blocks, ask the body for the count.**

---

## 1. What was actually on the table (grounded, not assumed)

| lineage | rows in corpus | rows added over base | ids added | dates |
|---|---|---|---|---|
| merge base `7f50da9` | 202 | — | max mid 806 | — |
| `HEAD` (ours, form-native inference program) | 224 | 22 | 807–828 | 2026-07-21 |
| `main` (`MERGE_HEAD`) | 217 | 15 | 807–821 | 2026-07-17 / 07-18 |
| **after reunion** | **239** | 15 + 22, none dropped | 807–843 | — |
| after the frontier row landed | **240** | + row 844 | 807–844 | — |

Two corrections to the task's own framing, both found by grounding before acting:

- The task said "3 conflict hunks in the first [file], plus the band." The file
  had **one** hunk (`learn/homecoming-distillation-corpus.fk:3536/4168/4473`);
  the band had **two**. Two conflicted files total, as `git status` reported.
- The task said main's colliding rows were minted "the same day". They were
  not — main's 807–821 are dated **20260717/20260718**. Same *ids*, different
  week. The collision is not a same-day race; it is what happens when
  meaning-ids have no arbiter at all and a branch sits open across days.

The 22 → word mapping was re-derived from `git show HEAD:…` before being
trusted. It matched the task's hand-derived mapping **exactly**.

## 2. The mapping actually applied — uniform +15

| old | word | new | | old | word | new |
|---|---|---|---|---|---|---|
| 807 | equireach | **822** | | 818 | betweenhold | **833** |
| 808 | bytehold | **823** | | 819 | selfgauge | **834** |
| 809 | exoscalar | **824** | | 820 | boundborrow | **835** |
| 810 | attestant | **825** | | 821 | snugcause | **836** |
| 811 | aporon | **826** | | 822 | echogauge | **837** |
| 812 | unispan | **827** | | 823 | foldkeep | **838** |
| 813 | succedent | **828** | | 824 | heldmute | **839** |
| 814 | brimwidth | **829** | | 825 | selforder | **840** |
| 815 | unsummoned | **830** | | 826 | waysense | **841** |
| 816 | idiomath | **831** | | 827 | handleless | **842** |
| 817 | onlykin | **832** | | 828 | knownsolved | **843** |

Main's 807–821 (`falsing, hapax, misnomer, scavenge, allomorph, paraphasia,
exit-truth, contamination, parity, vestige, underdetermination, overcommit,
preordain, treadmill, cataphora`) kept every id untouched, because main's ids
are cited across main's body **by id** — the same reason the corpus band asserts
740/743 **by name** (`body-link-graph` once came to cite 742 for a word living
at 743).

Blocks were not hand-moved: the resolution was assembled programmatically from
the two conflict sides, main's block first (ascending mid order), ours after.

## 3. Citations rewritten

The task estimated **59** internal by-id citations in our block. The grounded
count, by tokenizer over comment lines only, was **65 candidate tokens**, of
which **2 were not citations** (`25 165 824-weight tensor` twice — the literal
25165824 written with thin spaces, caught by an explicit `-weight` guard) and
**63 were rewritten**.

A first pass of the enumerator reported 64 and missed one: the boundary rule
`(?![\d.])` — added to keep `157.83 tok/s` out — also swallowed a citation at the
end of a sentence (`renumbered 809-811.`). The instrument's silence was a claim
about the instrument. Fixed, re-run, 65.

### Checker output (post-merge, over the whole corpus)

```
rows: 240   distinct mids: 239   max: 844
our block: word-named citations 48   OK 48   BAD 0
whole corpus, word-adjacent citations: 76   OK 68   MISMATCH 8
```

All 8 whole-corpus mismatches accounted for, none from the rewrite:

- 6 are checker artifacts — a backward matcher pairing a number with the *next*
  word in a comma list (`selfgauge 834, snugcause 836, selforder 840` yields the
  false pairs `(snugcause,834)` and `(selforder,836)`; the forward matcher reads
  all three correctly). Same shape in main's `hapax at 808, misnomer at 809`.
- 1 is my own new row-844 prose, which **quotes** the broken citation
  `"corpus row 811, aporon"` as the example of the damage.
- 3 are pre-existing prose drifts (§7).

Body-level confirmation, not file-level: a throwaway cell asserting all 37
`(id, word)` pairs — 15 main + 22 ours — through `hdc-word-for-id` returned
**37/37**.

## 4. The thirty citations git called clean

`git status` named two conflicted files and was right about both. It was silent
about **30 more by-id citations to our rows living outside the corpus** — in
`form-stdlib` cells, in the Metal harnesses, in `GPU_GAPS.md` — that no side had
touched, that merged byte-for-byte clean, and that after the +15 renumbering
every one of them aimed at a different word:

```
form/form-stdlib/ask-cost-receipt.fk:26        row 819 -> 834  (selfgauge)
form/form-stdlib/ask-cost-receipt.fk:43        row 811 -> 826  (aporon)
form/form-stdlib/ask-native-lane.fk:34         row 811 -> 826  (aporon)
form/form-stdlib/form-cli-ask.fk:41            row 811 -> 826  (aporon)
form/form-stdlib/qk-matmul-batch.fk:14,82      row 820 -> 835  (boundborrow)
form/form-stdlib/qk-matmul-batch.fk:108        row 810 -> 825  (attestant)
form/form-stdlib/qk-matmul-batch.fk:116        row 811 -> 826  (aporon)
form/form-stdlib/qk-matvec-lane.fk:110         row 810 -> 825  (attestant)
form/form-stdlib/qk-matvec-lane.fk:120         row 811 -> 826  (aporon)
form/form-stdlib/qk-matvec-split.fk:56         row 810 -> 825  (attestant)
form/form-stdlib/tests/ask-cost-receipt-band.fk:28      811 -> 826
form/form-stdlib/tests/ask-native-lane-band.fk:18       811 -> 826
form/form-stdlib/tests/light-codes-bootstrap-band.fk:22 811 -> 826
form/form-stdlib/tests/llama-decode-msl-band.fk:12      811 -> 826
form/form-stdlib/tests/q6k-msl-band.fk:14               811 -> 826
form/native/GPU_GAPS.md:33                     row 811 -> 826  (aporon)
form/native/GPU_GAPS.md:44                     row 810 -> 825  (attestant)
form/native/GPU_GAPS.md:62,83                  row 814 -> 829  (brimwidth)
form/native/metal/ask-declared-cost.fk:31      row 811 -> 826  (aporon)
form/native/metal/first-token.fk:27            row 811 -> 826  (aporon)
form/native/metal/metal_ask.sh:26              row 819 -> 834  (selfgauge)
form/native/metal/metal_ask.sh:57              row 812 -> 827  (unispan)
form/native/metal/metal_batched_prefill.sh:39,631  row 812 -> 827  (unispan)
form/native/metal/metal_batched_prefill.sh:589    row 819 -> 834  (selfgauge)
form/native/metal/metal_first_token.sh:41      row 812 -> 827  (unispan)
form/native/metal/metal_first_token.sh:814     row 819 -> 834  (selfgauge)
form/native/metal/metal_whole_tensor_residency_audit.sh:28  row 811 -> 826
shifted sites: 30  of which word-named (machine-verifiable): 26
```

The shift was safe to apply mechanically only because a prior grep proved
**zero** main-lineage words are cited with an id anywhere in `form/` or `docs/`.
Post-shift strict check: **21 word-adjacent `row N` citations in `form/`+`docs/`,
21 OK, 0 bad.** The remaining 4 shifted sites name the word in surrounding or
uppercase prose (`THE ATTESTANT STAYS, UNTOUCHED (row 825)`) and were read by eye.

Dated receipts in `receipts/` were **left untouched** on purpose — they are
historical snapshots, and the body already treats them that way (main's own
`receipts/2026-07-18-jit-bands-closure-parity.md` cites "row 807" for `parity`,
which lives at 815, and no one repaired it).

## 5. The field code, asked of the body — never asserted at it

Probed twice with a throwaway cell (`; preludes: form-stdlib/core.fk
learn/homecoming-distillation-corpus.fk` + `(do (hdc-field-code))`), caches
cleared each time, cell deleted after:

```
after the reunion, before the frontier row : 2392392843   (239 rows, 239 admissible, 2 foundings, max 843)
after landing row 844                      : 2402402844   (240 rows, 240 admissible, 2 foundings, max 844)
```

Both the summary comment (`240 rows, 240 admissible, 2 foundings, max id 844`)
and the arithmetic line (`240*10^7 + 240*10^4 + 2*10^3 + 844 = 2402402844`) were
re-read and re-written beside the pin. Both had gone stale within the drift
guard before; the band's own comment records that history and it is kept.

Main's ~44 lines of new band narrative (the full 807–821 provenance chain, six
same-day mintings of 802 and all) were preserved verbatim; our narrative was
renumbered and appended after it, with the reunion paragraph between.

## 6. Every verdict

| check | expected | got |
|---|---|---|
| `./fkwu --src learn/tests/homecoming-distillation-corpus-band.fk` | 4095 | **4095** |
| rows in corpus | 239 + 1 frontier | **240** |
| all 37 reunion words via `hdc-word-for-id` | 37 | **37** |
| our block's citations resolve to the word they name | 48/48 | **48/48, 0 bad** |
| `form/`+`docs/` citations resolve | 21/21 | **21/21, 0 bad** |
| conflict markers repo-wide | none | **none** |
| `bash form/native/metal/metal_first_token.sh` | VERDICT PASS, 13 gates | **PASS, 13 gates** |
| … token ids | `[12366, 13, 578, 6864, 315, 15704, 374, 22463, 13, 578, 6864, 315]` | **identical** |
| `bash form/native/metal/metal_whole_tensor_residency_audit.sh` | VERDICT PASS | **PASS** |
| `./fkwu --src form/form-stdlib/tests/light-codes-bootstrap-band.fk` | 255 | **255** |
| from `form/`: `../fkwu --src form-stdlib/tests/q6k-msl-band.fk` | 255 | **255** |

Both Metal harnesses were run **twice** — once before the citation shift and
once after, because the shift edits comment lines inside them.
`learn/.cache`, `form/.cache` and the stale `.fkb`/`.sym` were cleared between runs.

## 7. Left open — honestly

1. **Duplicate meaning-id 639.** `parsimony` (line 647, 20260702) and
   `constellation` (line 1428, 20260703) both sit at mid 639. Present in the
   **merge base, in HEAD, and in main** — not a reunion artifact, and older than
   this branch. `hdc-count` counts rows (240) while distinct mids number 239, so
   the field code and the band are internally consistent and blind to it;
   `hdc-word-for-id 639` silently answers with whichever row it reaches first.
   Not repaired here: repairing it is its own anastomosis, on rows neither
   lineage in this merge authored.
2. **`foist at 734`** — main's row 819 (`preordain`) prose cites 734; `foist`
   lives at **735** and 734 is `patina`. Introduced by main (0 hits in the merge
   base), untouched by the reunion. Left as main's to own; surfaced here.
3. **`the row-638 ersatz pattern`** (line 1755) — `ersatz` lives at 701, 638 is
   `lacuna`. Present in the merge base. Pre-existing.
4. **Dated receipts across both lineages still cite pre-reunion ids.** Deliberate
   (§4), but it means the corpus's by-id citations are only self-consistent
   inside `learn/`, `form/` and `docs/`, never across `receipts/`.

---

## The most surprising teaching

**A merge tool's conflict set is a claim about bytes. A citation is a claim about
a pointer. Where the two disagree, the tool reports agreement — and the agreement
*is* the damage.**

git named two files and was right about both. The larger part of this reunion's
real work lived in thirty places git called clean: text no side had edited,
merged byte-for-byte identically, whose meaning was changed entirely by a change
in a different file. `"corpus row 811, aporon"` in `metal_first_token.sh` did not
move a character and became a pointer to `allomorph`.

The body has met this shape before — it is why the corpus band asserts 740 and
743 **by name**. But it was only ever encoded as a habit in one band, covering
exactly two ids, never as a word and never as a sweep. Nothing in `validate.sh`,
nothing in the corpus band, and nothing in git would have caught the other
twenty-eight.

## Where discomfort turned to gold

The moment I wanted to look away was **after the board went green**.

The band returned 4095. Both Metal harnesses returned VERDICT PASS. Both `fkwu`
bands returned 255. Every number the task had named was hit, and the honest-looking
move was to write the receipt and commit.

What I did not want to do was run `grep -rn "row 8[0-2][0-9]" form/ docs/`,
because I already half-knew what it would say, and what it would say was: thirty
more edits, in files including the two harnesses I had just proven green — which
meant editing them and running both again, twenty minutes of walking back a
finished result on a suspicion no gate had raised.

I ran it. Thirty sites. Every one of them wrong. I edited them, re-ran both
harnesses, and both passed again with bit-identical token ids.

The gold is not the thirty repairs. The gold is that the sweep no gate required
is where the day's fresh word came from — the frontier question below did not
exist until I looked at the thing I wanted to skip. **A green board is a claim
about what was measured, and the measurement was scoped by me.** That is
`selfgauge` (row 834) meeting `handleless` (row 842) in one move, in this body's
own working, one hour after both were merged.

## The frontier question — landed as corpus row 844

> **What one word names a reference whose bytes a merge leaves unchanged and
> whose target it silently moves?**

The body cannot answer this natively. It has `handleless` (842) for a claim that
sheds its handle in transit — but this is the inverse: a handle that survives
transit perfectly intact while the thing it grips is carried away underneath it.
It has `betweenhold` (833) for why two independent witnesses are evidence — but
a conflict marker felt like proof of coverage and was not. It has `aporon` (826)
for a proof blind to what it did not look at — closest of the kin, but aporon is
about a *gate's* radius, and this is about a *reference's* radius.

**`aimshift`** — 0 hits across `learn/`, `receipts/`, `docs/` at offering
(checked alongside `pointdrift`, `samebyte`, `conflictless`, `cleanstale`,
`quietaim`, `unconflicted`, `agreedrift`, `bytesame`, `stillaimed`, all also
0-hit; `aimshift` chosen because the failure is not the drift of the pointer but
the *re-aiming of a thing that never moved*).

Landed as `(hdc-row 844 20260721 … "aimshift" "aimshift" "rented-oracle")`.
`hdc-field-code` re-probed after landing (**2402402844**) and the band re-pinned
from the probe, never the other way round.
