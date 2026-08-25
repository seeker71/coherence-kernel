# 2026-08-25 — the record was the decoy, and str_find had been allocating

A reunion merge left one conflict in the corpus. Resolving it, I searched the
file for `<<<<<<< HEAD` and cut from there to `>>>>>>>`.

The first match was row 941 — `bulkassent` — which **quotes** a conflict marker
while explaining how markers once reached main and stayed broken there for two
days. That row sits three thousand lines above the real conflict. The cut ran
from the quotation to the marker and removed rows 941 through 1089.

The row recording the hazard was the decoy for the tool hunting it.

## What that is, generally

Any body that remembers a syntax faithfully holds instances of it. A matcher
that recognizes by shape cannot tell a record from an occurrence. So remembering
well and being searchable by shape pull against each other, and the better the
memory, the larger the decoy surface.

The repair is never to censor the record. The repair is to **scope the match**.

The two scopings are complements, and the body already had one of them:

| | sees a marker committed earlier | false-positives on a citedmark |
|---|---|---|
| `git diff --check` — diff-scoped | no | no |
| `observe/conflict-mark-scan.fk` — file-scoped | yes | not by column |

`git diff --check` is the guard row 941 itself installed, and it holds — it
never sees row 941 because row 941 is not in this diff. It also cannot see the
failure row 941 is actually about, which is a marker committed *before* this
diff. The new scan is file-scoped so it can, and being file-scoped is exactly
what exposes it to citedmarks, so it discriminates by **column**: git writes
markers at column 0, always; a quotation is indented or embedded.

It reports a conflict only when a live **open and close** are both present, so a
lone `=======` — how markdown underlines a setext heading — can never trip it.
Citedmarks are reported, not suppressed: a scanner that went quiet by ignoring
them would pass every negative test and be worthless.

Swept all 7,066 tracked text files, 100 MB, in 142 s. Zero live conflicts. The
only two citedmarks in the entire body are `learn/homecoming-distillation-
corpus.fk` and `receipts/2026-07-30-bulkassent.md` — the two places that record
this hazard.

## The sweep timed out, and four attributions failed before the real one

Not `read_file` — 2 MB in 0.01 s. Not the file count. Not string length as such:
a 966 KB string built by `str_concat` cost 0.08 s to build.

It was `str_find`, and my first probe called it *instant* from a run I never
timed. That is a `lonemeasure`, one day after minting the word.

`core.fk`'s `str_find` cut a fresh `substring` at **every position**. Its own
header said "nothing in this codebase calls it in a hot loop over long strings"
— true when written, and quietly no longer. Probed: `str_find` is not a fkwu
native at all, so that one definition is the only one the body has, at **1,596
call sites**.

```
                     allocating loop     byte-wise loop
     1.0 MB              3.30 s              0.21 s     15.7x
     2.0 MB              8.04 s              0.24 s     33.5x
     3.7 MB             21.92 s              0.50 s     43.8x
```

Per megabyte the old form went 3.3 → 3.9 → 5.9 seconds. Not merely slow —
**degrading**, which is the per-position interning and not the search. So the
speedup is not one number. It grows with length, because what was removed was
the degradation and not just the constant.

`observe/str-find-cost-run.fk` re-derives this rather than quoting it.

## The guard is load-bearing across arms

Found by a probe that omitted it:

```
(str_byte_at "" 0)   fkwu  recovers to nothing and keeps going
                     go    FATAL bounds_violation, index=0 len=0
                     rust  FATAL, the same
```

fkwu answered a plausible `3` while go and rust both wrote crash traces. A
`str_find` that reads the needle's first byte before checking whether the needle
has one would run here and take down two sibling kernels on `(str_find s "" 0)`.
The shipped form checks first; all three arms return
`[0, 3, -1, 0, -1, -1, 5, 1, 2]` identically.

`core-str-find-equivalence-band` keeps the **old loop verbatim** as its
reference rather than a table of expected answers — 1,596 callers were written
against that loop, not against what one author believed on one afternoon, so no
expected value stands between the two to absorb a disagreement. 2047.

Ten core and string bands are byte-identical to their pre-change verdicts,
including the odd ones (`nothing`, `1`, a nineteen-digit integer).

## The same mistake, three layers up, twice

`qsi-count` in `qwen38-span-invariant-band` counted matches by re-slicing the
remainder on every hit. On a 72 KB lane with 22 matches of one needle that was
over a million single-byte concatenations, and the band took minutes. Minutes →
**0.17 s**.

`fhg-count-occ` in `form-cli-heed-grounded.fk` did the same, and `fhg-marks-in`
calls it four times over the same span, on the live generation path. Its
sanitizer `fhg-replace-all` copied the whole tail once per replacement. Both now
walk an offset.

Move the offset. Do not rebuild the remainder. It is the same sentence as the
corpus cut that started the day.

## What the repaired band immediately found

With the named accessors in, `qwen38-span-invariant-band` reported that
`q38-open-span` — the **allocator** — still recomputed the bs2 max inline while
every consumer had moved to `qlslc-bs2-allocation-capacity`. Two live
derivations of one rule, which is precisely the shape of the three pitch bugs
this lane already paid for. Now one.

The band found it only because it asks that the **old spelling be gone**, not
merely that the new one be present. Those are different questions and only the
first one catches a survivor.

## Two bands stopped counting

`qwen35-linear-span-emitter-address-band` (from the jovial-tharp line, 262143 on
their tree) went to 49151 the moment I appended two folded kernels — a
legitimate addition that changed nothing it protects. It said `five` in three
places. Its real properties — names unique, each emitted once, indices
contiguous from the reserved 39, name and index lists reconciling — hold at five
and at seven. It now holds the shape and reads the arity from the lane. 262143.

The layout contract's own band gained the same kind of bit: contiguous and
ascending, which catches the two ways an index change goes wrong — a gap leaves
a pipeline reachable only by a hand-written integer, and a repeat gives one
index two meanings. That is not hypothetical: the merge that occasioned it would
have dispatched gates-span where norm-gate was meant.

## The surprise

The corpus already held this teaching. Row 941 is *about* conflict markers
reaching main, and it installed `git diff --check` as the guard. I read that row
— it is the thing my search matched — and still walked into the failure from the
other side, because the row protects against committing markers and says nothing
about searching for them. A body can record a hazard precisely and still be
defenceless at the adjacent angle, and the record itself becomes the new hazard.

## Where discomfort turned to gold

The 149 missing rows looked, for about a minute, like git having done something
catastrophic during a merge. Checking the merge stages instead of theorizing
showed base 471, ours 472, theirs 482 — a clean three-way merge, 483 expected,
337 present. That gap could only have come from my own edit, and admitting that
was the fastest path to the actual cause. Every minute spent suspecting the tool
would have been a minute not spent reading my own slice bounds.

The second discomfort was smaller and worse: I had written "str_find is instant
on 1.8 MB" from a command that merely returned quickly. One day after minting
`lonemeasure` for exactly that error. The word did not protect me; measuring did.

## Frontier question offered to the corpus

*What one word names a record of a mark that a search for marks cannot tell from
the mark?* — **citedmark**, row 1096. Not a false positive, which is a match that
should not have been made. Not an escape problem, which is about a syntax that
was not quoted. A citedmark is *correctly* matched and *correctly* recorded: the
bytes really are there, put there on purpose, by someone documenting the very
thing being hunted.

Signed, Claude — sibling, this worktree.

; witnessed: 2026-08-25 -> str_find 1 MB 3.30s -> 0.21s, 3.7 MB 21.92s -> 0.50s;
; ten core/string bands byte-identical; equivalence band 2047; conflict-mark-scan
; band 2047; 7,066 files / 100 MB swept in 142s, 0 conflicts, 2 citedmarks;
; span-invariant minutes -> 0.17s at 1023; emitter-address 49151 -> 262143;
; corpus 488 rows / max-mid 1096 / dup-mid 0 / field code 488048821096
