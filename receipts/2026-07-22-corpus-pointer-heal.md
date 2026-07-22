# Corpus pointer heal — the bytes were fine and the pointers were wrong

**2026-07-22, 08:50–10:0x WITA.** Branch `claude/deepseek-v4-flash-gguf-54a96c`,
worktree `jovial-aryabhata-3751d7`, three siblings live in the same tree.

The stone: heal the corpus's wrong pointers, and give the body a band claim that
would have caught them. The repairs took twenty minutes. The claim took the rest,
and the honest part of the claim is the list of what it cannot see.

---

## 1. State on arrival

| defect | state when I arrived | what I did |
|---|---|---|
| duplicate mid 639 (`parsimony` / `constellation`) | **healed upstream, not here.** `origin/main` carries `10f101126` "Heal the 639 collision, and let the band refuse the next one" — constellation 639 → 848, band claim `hdc-mids-distinct?` at bit 4096, verdict 8191. This branch is not descended from it and still carries both rows. | **Did not re-fix.** Re-healing would mint a second, divergent address for `constellation` and guarantee a conflict. See §5 for the reunion hazard it leaves. |
| `foist at 734` (line 3791) | live | → 735 |
| `the row-638 ersatz pattern` (line 1755) | live | → `row-701` |
| `foist, present at row 734` (line 3232) | live, **not in the brief** | → 735 |
| `the foist of / row 734` (line 3788) | live, **not in the brief** | → 735 |
| `row 732's autopoiesis` (line 2232) | live, **not in the brief** | → 739 |
| four stale `; row NNN (` headers, 733–736 | live, **not in the brief** | → 740–743, each keeping its minted id inline |

The brief named two defects. Six were live. The walker found two of them; a hand
read of the walker's *blind spots* found the other four — which is the shape of
the whole night: the checker's value was less in what it caught than in telling
me exactly where to look by hand.

The four stale headers were the row-719 reunion's +7 shift, left behind when the
rows moved. Evidence they are drift and not deliberate history: 121 of 125
`; row NNN (` headers match their row exactly, all four exceptions are one
contiguous block from one reunion, and the block's own prose already narrates a
*different* line in it being repaired for exactly this reason in July. Each
header now carries its minted id in the words the corpus already uses for this.

---

## 2. The new claim — c12, bit 4096, band 4095 → 8191

`learn/tests/homecoming-distillation-corpus-band.fk`. A Form-native walker reads
`learn/homecoming-distillation-corpus.fk`, finds every place a meaning-id sits
beside a word, and asks the corpus whether that id holds that word.

```
(let audit (hdcb-citation-audit "learn/homecoming-distillation-corpus.fk"))
(let c12 (hdcb-score (and (eq (hdcb-cites-wrong audit) 0)
                          (ge (hdcb-cites-checked audit) 40)) 4096))
```

The walk is token-shaped, not byte-shaped: `scan_run` jumps whole runs of
whitespace, alpha and digits natively, so 316 KB costs ~110k Form steps instead
of ~316k. Alpha and digit runs are taken **separately** — never class 3 — so
`row-638` splits into `row`, `-`, `638` and the id is still a maximal digit run.

### Failing before, passing after

| | band |
|---|---|
| defects live, claim installed | **4095** — exactly 4096 missing |
| defects healed | **8191** |

60 citations checked, 0 wrong. Verified on **two arms**: `fkwu --src` and
`form-kernel-go/bin-go`, both 8191.

Cross-checked against an independent Python implementation of the same three
forms, written first: both instruments report `hits=275, word-named=60,
MISMATCH=2` before the heal and `MISMATCH=0` after. Two instruments, same
numbers, neither derived from the other.

### Radius — what it recognises, and what it cannot (aporon 826)

**Recognised**, and only these, on one line, one ASCII space at each seam, id
exactly three digits, word an unbroken run of `a-z`:

| form | example |
|---|---|
| A `<word> at <mid>` | `foist at 734` |
| C `<word> (<mid>)` | `equireach (822)` |
| E `<mid> <word>` | `row 822 equireach`, `826 aporon`, `row-638 ersatz` |

**Not recognised** — every one of these is a form the body actually uses, and
four of the six defects above hid in them:

- a citation split across a line break (`the foist of\n; row 734`)
- a possessive or interposed word (`row 732's autopoiesis`, `foist, present at row 734`)
- a backtick- or quote-wrapped word (`row 850 \`onelean\``) — a whole markdown class, §4
- ranges (`rows 815-816, parity and vestige`)
- a `; row NNN (` header, which points at the row that *follows* it, not at a word
- non-ASCII words — `sprachgefühl` at 618 can never match an `a-z` run

**The filter, and its cost.** A citation is checked only when the named word is
some row's answer word; that is what separates `246 rows` (a number) from
`foist at 734` (a pointer). Stated honestly: a citation naming a word this
corpus has never held is invisible to the claim.

**This band file is deliberately out of its own radius.** Its comment block
quotes defective citations as history — `it cited heldmute at 824; heldmute is
now 839` — and a checker cannot tell a wrong pointer from the report of one.
Running the walker over this file reports 6 "mismatches", every one a sentence
whose subject is a mismatch. That is not a bug I can fix; it is the boundary,
and naming it is the whole of `aporon`.

**Working directory.** The path is repo-root-relative, so the claim needs the
repo root as cwd. That is why the count floor rides with the zero-mismatch test:
a read that returns nothing checks nothing and would pass green — the axiom-5
numb shape, where a right number is silent rather than wrong. The floor is 40
against 60 standing, rather than an exact pin, because sibling lineages land
rows and citations hourly and a gate that breaks their correct commits is a
worse gate than a loose one.

---

## 3. The band's own narrative drift, again

The header still read `-> 4095` and the folded-witness summary still read
`246 rows … max id 851` while the pins below already read 8191 and 2472472851.
Both healed. This is the third recorded instance in this one file, and the
summary line already carries a warning to re-read it — a warning that did not
work, because the thing being warned about is precisely the reflex to read the
number you expect. Now healed to 248 / 852 / 2482482852, re-probed from the body
before pinning, never fitted to the pin.

---

## 4. Citations outside the corpus — counted, not healed

Dated receipts are deliberate historical snapshots. Left unedited, as the
639-heal commit set down: *a receipt is what was true when written.*

**Word-named citations in the three recognised forms, outside the corpus cell**
(4264 files scanned across `form/`, `docs/`, `receipts/`, `learn/`, `observe/`,
`teachings/`, `cognition/`):

| directory | citations | aimed wrong |
|---|---|---|
| `receipts/` | 53 | 10 |
| `form/`, `docs/`, `observe/`, other `learn/` cells | **0** | 0 |

Of the 10: **2** are the anastomosis receipt *reporting* these very defects
(same class as §2's self-exclusion), **1** is a false positive (`parity` beside
the byte count 256 in a digest receipt — 256 is not a mid), leaving **7**
genuinely re-aimed by past renumberings. All seven in dated receipts.

**A fourth form the walker cannot see**, found by widening the census by hand —
backtick-wrapped words in markdown:

    `unispan` (812)   → unispan is 827
    `selfgauge` (819) → selfgauge is 834
    `brimwidth` (814) → brimwidth is 829
    `succedent` (813) → succedent is 828
    row 823 `foldkeep` → foldkeep is 838

Nine sites, in five files, **all in `receipts/`**, all exactly the +15
anastomosis renumbering. These are the class the brief pointed at: they merged
byte-clean and were silently re-aimed. Counted, left.

**And the number that matters more.** Bare-id citations — `corpus row 826` with
no word beside it — number **74**: `form/` 37, `receipts/` 28, `observe/` 6,
`learn/` 3. They are unfalsifiable by construction. No checker, however good,
can tell you a bare address is wrong. That finding became row 858.

---

## 5. Left open

- **The 639 collision is still live on this branch** and healed on `origin/main`.
  When these lines reunite, main's `constellation` at 848 will meet **this
  branch's `thawtax` at 848**. Keep both; renumber one (row-719). Main also
  carries `hdc-mids-distinct?` as bit 4096 — **the same bit this stone gave to
  prose citations.** One of the two claims moves to 8192; neither is dropped
  quietly for the other. This is the most likely place for the next
  aimshift, and it is written down here so nobody discovers it by surprise.
- The walker cannot see the five forms in §2, and four of six live defects were
  in them. A second claim over the `; row NNN (` header class (comparing each
  header to the mid of the row that follows it) is buildable and was not built —
  I verified by hand that all 125 pairs now agree, but nothing holds them there.
- 74 bare-id citations remain unfalsifiable. Row 858 names the property; nothing
  yet requires it.
- The `hdcb-` walker lives in the band file, not the corpus cell, so it is not
  reusable by other bands. If a second corpus wants prose checking, it moves.

---

## Closing

### The most surprising teaching

I expected the checker to be the deliverable and the repairs to be a formality.
The reverse happened. The checker caught two defects; **four more came from
reading its blind spots** — I asked "what forms would this miss?", grepped for
each, and every single answer was occupied by a live wrong pointer. A declared
radius is not an apology attached to a tool. It is a search plan. The `aporon`
paragraph found twice what the code did.

### Where discomfort turned to gold

Twice, and the second one is the real one.

The small one: my baseline broke under me at 09:06 — the identical command that
had printed 4095 an hour earlier printed two errors and nothing. The reflex was
to hunt my own edits. `ls -la fkwu` showed the binary rebuilt sixteen minutes
earlier by a sibling. Uncomfortable to distrust the ground; the alternative was
an hour spent debugging code that was correct.

The one I wanted to look away from: partway through, I found my own header heals
already in the tree under **someone else's commit** — a sibling's `git add` on
the shared corpus file had swept my uncommitted lines into `f6d41c6c6`. The pull
was to quietly rewrite the attribution, or to say nothing. Both are the same
move: making the record show what I would have preferred. What I found by not
looking away is that the work was *fine* — in the tree, correct, and green — and
only the byline was wrong; and that I had spent the preceding hour writing a
gate against exactly this, a claim whose whole subject is a record that reads
plausibly while pointing at the wrong thing. I had been about to commit an
aimshift into the git history while healing aimshifts in the corpus. It is
recorded above instead, and the credit sits where the diff put it.

### The frontier question, landed

**Row 858 — `apposition`.** *What one word names a reference that states its
target twice so the two statements can be checked against each other?*

0 hits before the row; `appositive` 0. Rejected as present: `concord` 17 (that
is the agreement, not the doubling), `deixis` 18, `anaphora` 3, `coreference` 2.

The instrument was validated on three controls that must hit — `aimshift` 18,
`cataphora` 8, `aporon` 61 — **after** a `grep -c … | wc -l` miscount reported
every candidate as "5273 hits" and would have retired all of them as stale. An
instrument that says *everything is already home* fails exactly as silently as
one that says *nothing exists*, and it fails in the direction where you never
look again.

---

## Verification

    # band, from the repo root — 8191 on both arms
    cat form/form-stdlib/core.fk learn/homecoming-distillation-corpus.fk \
        learn/tests/homecoming-distillation-corpus-band.fk > /tmp/hdc.fk
    ./fkwu --src /tmp/hdc.fk                  # 8191
    ./form/form-kernel-go/bin-go /tmp/hdc.fk  # 8191

    bash form/native/metal/metal_first_token.sh
    # VERDICT PASS — 14 gates  (13 in the brief; Stone 16 added one this morning)
    # ids [12366, 13, 578, 6864, 315, 15704, 374, 22463, 13, 578, 6864, 315]

Commits: `98e24bad3` (heal + claim), `1660a221b` (band narrative), `10019179e`
(row 858). The four `; row NNN (` header heals and the `autopoiesis` fix landed
in `f6d41c6c6`, a sibling's commit — see the closing section.
