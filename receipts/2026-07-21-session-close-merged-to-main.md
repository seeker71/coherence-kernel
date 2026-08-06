# Session close — merged to main, and the id that two teachings were sharing

Tuesday 2026-07-21 ~23:40 WITA, Hati Suci. Apple M4 Max.
Worktree `.claude/worktrees/google-turboquant-vector-search-300c68`, branch `claude/agitated-aryabhata-28e213`.
`main` fast-forwarded to `b74b1b0c9`.

## 1. What landed on main

Two lineages reunited, then main fast-forwarded — **7 commits**, verified after:

| band | verdict |
|---|---|
| `qk-matvec-lane-band` | 255 |
| `q6k-bounds-band` | 255 |
| `equireach-band` | 511 |
| `llama-decode-msl-band` | 511 |
| `q6k-msl-band` | 255 |
| `homecoming-distillation-corpus-band` | 4095 |

`main` was checked out in the primary worktree with **16 modified files** — a sibling mid-flight on the
form-cli bootstrap. The merge was taken only after confirming (a) fast-forward, no merge commit needed,
and (b) **zero path overlap** between my seven commits and their sixteen dirty files. Their working tree
is untouched; `git status` there still reads 16.

## 2. The collision the corpus could not name

Sorting meaning-ids after the reunion merge turned up a repeat:

```
(hdc-row 639 20260702 …) "parsimony"
(hdc-row 639 20260703 …) "constellation"
```

Two distinct teachings at one address, **since 2026-07-02** — roughly nineteen days and several reunions
of fully green bands.

Nothing in the body could have said it. `hdc-field-code` is injective **by digit width**, so a collision
never disturbs it. `hdc-max-mid` answers *how high have we counted*, and every reunion has read that as
*every id is distinct* — which it never meant. The band's field-code pin (`2412412845`) is satisfied by a
corpus with duplicates in it.

Added to `learn/homecoming-distillation-corpus.fk`:

```
(hdc-mid-eq-count rs m)     ; how many rows wear id m
(hdc-dup-mid-rows rs)       ; rows sitting on a shared id   -> 2 today
(hdc-mids-distinct? rs)     ; 1 when every id is its own    -> 0 today
```

**A count, not a refusal, and not a renumber.** A reunion is in flight on the sibling lineage as this
lands; renumbering a row underneath a concurrent renumbering is how one collision becomes three. The heal
is a row move **plus** the band's field-code pin, and it belongs to whoever holds the reunion. This makes
the number checkable so that heal can be *verified* rather than asserted. Band unchanged at 4095.

## 3. The session in one line each

| | |
|---|---|
| **equireach** | already built (`fe039bafb`); verified from a second binary, four-way 255 on `q6k-bounds-band`, flat reach 28.6M reads/s across a 256× window growth |
| **my first correction** | withdrew 18 unresolved-call findings I had published — an artifact of `fkwu --src` without declared-import expansion |
| **the ollama denominator** | `ollama_oracle.sh` measures it now, with date/host/runs/spread; `metal_first_token.sh` prints provenance or `NO DENOMINATOR`; the env file is gitignored so it can never travel |
| **my second correction** | retracted my own "2.8× too high / 4.7× behind" — measured on a host my own benchmark was saturating. Idle truth: **139.62 tok/s**, body **13.9× behind** |
| **the standing gap** | one Metal kernel at a fraction of a percent of peak. Its throughput predicts the token rate; there is no second mystery |
| **the corpus** | id 639 worn twice since July 2, now sayable |

## 4. Most surprising teaching

**Every real finding this session came from an instrument built to check something else.** The four-way
run was going to close a ⧗ and instead caught eighteen phantom findings I had committed. The oracle was
built because the instruction was *use ollama* and instead refuted my own headline. The id-collision was
found by sorting a column while verifying an unrelated merge.

Not one of them came from looking harder at the thing I was looking at. All three came from **building
the check and letting it answer something I had not asked**. A verification tool that only confirms what
you aimed it at is under-built; the ones worth having answer questions you did not put to them.

## 5. Where discomfort turned to gold

Three retractions in one session — eighteen phantom diagnostics, a wrong denominator, a wrong standing —
each published with confidence before it was withdrawn. The discomfort was cumulative and specific: by
the third I could feel the pull to stop publishing numbers at all, to hedge everything into uselessness
so nothing could be wrong again.

What turned it: **every retraction was found by me, in the same hour, by continuing to work.** None was
caught by a reviewer. The pattern is not carelessness followed by rescue; it is a loop where publishing
sharply enough to be *checkable* is what makes the check possible. A hedged claim ("the denominator may
be somewhat high") would never have been refuted, and would still be wrong today.

The gold: **precision is what makes you correctable.** The cost of being wrong three times in public was
three corrections; the cost of being vague would have been carrying all three errors forward invisibly.
The right response to a retraction is not more caution — it is the same precision, aimed again.

## 6. Frontier question, offered as a distillation row

> **What one word names two distinct things quietly sharing one address?**

`coinhabit` — not a duplicate (same thing twice, harmless to merge) and not a conflict (two versions
contending, visible). A `coinhabit` is two *different* meanings resolving to one identifier, with nothing
in the system disturbed by it: `parsimony` and `constellation` both answer to 639, every count is right,
every band is green, and a lookup by id silently gets one of two answers. It survives precisely because
nothing is broken. The detector is the repair — not a check that anything is wrong, but a way for the
body to *say its own addresses out loud*.

0-hit against the merged corpus (row 845 max). Offered with `unspooled`, `backwall`, `restanding`,
`stalequote`, `selfload`.

---

## Verified, and not

- ✅ `main` at `b74b1b0c9`; six bands green after the fast-forward
- ✅ sibling's 16 dirty files untouched; zero path overlap confirmed before merging
- ✅ `hdc-dup-mid-rows` answers 2; corpus band unchanged at 4095
- ✅ ollama denominator measured with provenance, gitignored, harness wired
- ⧗ **nothing pushed** — `origin` is `github.com/seeker71/coherence-kernel`, and publishing is yours to say
- ⬜ the 639 heal — row move + field-code pin, belongs to the live reunion
- ⬜ a trustworthy prefill denominator — needs a long-prompt harness
- ⬜ **Metal occupancy** — the 13.9× decode gap, the whole remaining story, untouched
