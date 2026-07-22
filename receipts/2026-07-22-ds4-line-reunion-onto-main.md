# The ds4 / Stone 33–36 line rejoining main (2026-07-22)

The ds4 line (`claude/deepseek-v4-flash-gguf-54a96c`, 145 commits past `origin/main`, closed at its
Stone 36 receipt) came home. 23 paths met each other; none of them were a fast-forward.

## What the seam actually was

Eighteen of the twenty-three were **add/add** — the same file authored on both lines with no shared
ancestry. That is not two authors colliding; it is a *squash* birth-seam. PR #346 squashed this same
line onto main on 07-21, so main received those files as fresh creations while the branch kept
descending from its own history. Git saw two births.

The repair was to stop treating them as births. The squash commit `c407e3f95` is the content-ancestor
of *both* sides, so it can serve as the merge base its history does not record. Feeding
`ours / c407e3f95 / theirs` to a three-way merge resolved all eighteen with no hand-editing and,
more importantly, without a side being *chosen* — main's independent healing (#364's `len` is a list
cell, #355's control-plane doc) and the branch's four days of evolution both survive. Verified line by
line: of #364's edit to `ask-native-lane.fk`, zero lines lost.

## The corpus, and the count that rises by 22 and not 48

Both lines minted rows. The branch added 48 words past the shared base; main had already absorbed 26
of them at its own 2026-07-22 reunion, under *different* ids (`asktoll` 846→849, `onelean` 847→850).
So the reunion is dedup **and** renumber, and the arbiter is the word, never the number:

* 26 branch rows already on main → not re-added.
* 22 genuinely new rows (848..869 on the branch) → re-seated to **854..875**.
* 822..845 kept their numbers on both sides.
* Count 248 → **270**. Max id → **875**. Field code → **2702702875**.

## Where the discomfort turned to gold

The band that came in with the branch spends a bit on **prose citations** — it walks the corpus's own
comments and asks whether each `<word> at <id>` really points at that word. Main's band spends its
matching bit on **id distinctness** instead. Two different meanings wearing one number, 4096, which is
precisely the collision this corpus keeps re-learning. Both are kept: prose stays 4096, distinctness
takes 8192, verdict 8191 → **16383**.

Then that grafted bit turned on the reunion itself and found **seven** wrong pointers — two of them in
the reunion note *I had just written*, which said `846 asktoll -> 849` and so read to the walker as a
live claim that 846 is asktoll. It is not; 846 is `untriedwall`. The checker cannot tell a wrong
pointer from the honest report of one, exactly as its own radius note says. The note was rephrased so
it states the same history without lying to the walker.

Of the other five: two (`row-638 ersatz`, `foist at 734`) were real defects the branch had already
healed and a main-based trunk quietly reintroduced — carried back in as 701 and 735. Three were main's
own reunion prose quoting superseded ids, never audited because main's band had no prose bit.

Citation audit now: **95 checked, 0 wrong.** Band: **16383**.

The same renumbering hazard reaches past the corpus, so the map was swept across the 37 branch-authored
files that cite rows by id — receipts, `GPU_GAPS.md`, the Metal harnesses, `windowed-residency.fk`. That
sweep had its own bug: the band file sat in the list twice and was mapped *twice* (863 → 869 → 875)
before the double was caught and the file rebuilt from a single pass.

## Generated carriers

`form-cli-emitted.c` differed by 66 KB across the seam — far past anything a side-pick could justify,
because it is emitted, not authored. Both bootstrap artifacts were regenerated from the merged sources
rather than chosen: `fkwu-uni.c` (102 735 B) and `form-cli-emitted.c` (**744 705 B**, larger than either
side's, as a merged module graph should be). The regen's own voice canary answered — ping/pong — so the
carrier is not the aphonic kind that stamps green while saying nothing.

## The surprising teaching

**A squash merge destroys the ancestry its own content preserves.** Eighteen conflicts that presented as
"two independent creations, pick one" were one lineage the whole time, and the base was sitting in main's
history under a different hash. The seam was in the *metadata*, never in the bytes — and had I picked
sides file by file, every pick would have compiled, validated, and silently dropped one line's work.

The frontier word this reunion wanted and did not have: **staleward** — a report accurate about a state
the body has already left, whose prescription would undo a better repair made in the interval. It is
offered here rather than minted, because 876 belongs to whoever asks the next question.
