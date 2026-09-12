# A merge that saw lines

2026-09-12, evening, M4 Max, Hati Suci. Epic-edison's line kept growing while the first reunion landed on this
branch. Its last three commits come home on top of receipt 48.

## Carried

- **A recipe is a template** (b6d981743, rung 12, epic-edison). A defn carries a second compiled instance, and
  the door picks the one whose parameter bits match the live frame. **Templates to N** (2bf9ef137) allows up to
  four instances per defn. The instance is chosen before this line's hand-back reads its signature, and the
  parameter mask leaves the hand-back's bit out, so the two compose. loop-lane-template reads 63.
- **Structural list equality runs native** (bf71d54a3). It needed a band and no new code: loop-lane-list-eq
  reads 15.
- **The form-cli bootstrap follows the template instances** (f4fdf4bc7; the stamp stays 62a20dcb28b63763).
  Epic-edison's own regen, 23ed89d38, regenerated the same inputs to the same stamp as 2284a48f7, so it was
  not picked.
- **Their rows move past this line's,** each keeping a note of its first number: twinsig 1504 to 1507,
  shallownative 1505 to 1508. **cleanclash is row 1509.**

Witnessed on the tree with rows through 1509:
- **Bands:** loop-lane-template 63, loop-lane-list-eq 15, loop-lane-cons 511, loop-lane-closure 255,
  loop-lane-float-head 31, melt-long-list 3, loop-lane-string-door 2047, and 255 on every other loop-lane
  band. arena-melt 63, closure-capture-melt 31, hati-os-heap 7. Every JIT band reads as before. value-str 255,
  str-to-int-reading 127, once-hold 15, jit-lens 16383.
- **Answers:** list-build-diff-probe answers as e86210dc0's kernel does, all 21 lines. The loop carrying an
  int, a float, a string and a list answers as main did before the reset.
- **Before landing:** TestFkwu; freshness 31, the corpus band 32767, drift 16383 of 16383, porcelain 0.

## Still open, measured

- **Epic-edison's review of 0f47d16c6, the hand-back, is still owed.** Nothing reaches main before it.
- **The kind-19 direct-call arm dispatches a template's first instance only** (named by epic-edison).
- **A loop that both conses a fresh list and carries a list parameter** declines to the walker.
- **foldr** is not written (named by epic-edison).
- **A single pass that needs more than 4,194,304 pairs** still falls to the walker (receipt 46).
- **Lowering native-table-compile.bml interns more than 2 GiB of strings** (receipt 47).
- **value_str is still leaf mode 25 in C** (receipt 45).
- The open items of receipts 12 to 48 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was that three of this reunion's merges succeeded as text and still left two corpus rows with one
id each time: floatlend beside twicebound at 1502, shapeturn beside wordheat at 1503, and shallownative beside
shapeturn at 1505. The merge compares lines, and two rows written in two worktrees are different lines; their
ids clash only in meaning.

The discomfort was the pull, at each clean merge, to trust "Auto-merging" and go on to the next pick. Listing the
row ids after every pick, the clean ones too, is what saw the three twins before any band read them. The gold
is a reunion step that asks the keys, not the merge tool.

Frontier word, row 1509: **cleanclash**, a merge that succeeds as text while two of its lines claim the same key.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
