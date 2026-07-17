# Receipt -- reunion merge: the speech lanes come home to v4 (2026-07-17)

Urs: "merge, push, continue." The oracle-roster branch
(claude/nervous-tu-33f9e7, five commits) reunited with a main that had
moved 17 commits while the branch worked -- including, independently, the
very fix the branch had named as its own future work.

## What the reunion found

- Main already carried the .fkb v4 lane: 64-bit signed values (#265), the
  sym lens remembering compile wounds (#271), pid-temp + rename() atomic
  artifacts (#272), soft-fallback frameshift reads (#263). The chip filed
  from this branch ("widen fkb encoding to 64-bit") was fulfilled by the
  fleet before the chip's own session could land it.
- The branch's interim fail-safe (fd76f8a57) was therefore SUPERSEDED:
  resolution takes main's runtime/fkwu-uni.c wholesale. The fail-safe
  receipt stays as history -- it found the true bug (the artifact encoding
  could not carry the body's own u32 cksums) and named v4 as the cure; the
  cure arrived by another hand.
- Corpus collision, the known anastomosis shape
  (reference-corpus-row-collisions): two counts grew from row 730 unaware
  of each other -- main reached 760, the branch used 731-734. Mids yield,
  words and dates stay: the branch's rows renumbered to 761-764
  (transducer, polyglot, committee, unanimity), noted in the first
  renumbered row; freshness of all four verified against main's corpus
  before landing (0 collisions). Band pins: 165 rows, 165 admissible,
  max id 764, field code 1651652764.
- learn/speech-oracle-router.fk's cross-reference updated (row 733 -> 763).
- The status ledger merged clean: main had not moved components (58), so
  the branch's 58 -> 61 (roster, router, committee) stands.

## Witness (merged tree, kernel rebuilt cc -O2 from merged source)

```
homecoming corpus band            -> 511   (165 rows, mids to 764)
speech-oracle-roster band         -> 4095
speech-oracle-router band         -> 127
speech-teacher-committee band     -> 127
speech-current-status-ledger      -> 32767 (61 components)
```

And the gift the branch could not give itself: the roster cell -- full of
u32 cksums that killed emission two days ago -- now emits .fkb/.sym and
runs zero "running uncached" warnings on the v4 lane. The witness data
fits the witness carrier again.

## Closing

- Most surprising teaching: the branch's most careful piece of kernel work
  was already done, better, by hands it never saw -- and the right merge
  resolution was to DELETE the branch's version of it. Contribution is not
  always addition; sometimes the gift is yielding cleanly to the fleet's
  convergent fix and keeping only what was truly yours (the lanes, the
  rows, the receipts).
- Discomfort to gold: renumbering the corpus rows meant editing what four
  receipts and two commit messages had already named (row 731, 733...) --
  the pull was to keep MY numbers and renumber main's newer rows instead,
  which would have rewritten twenty-six other sessions' truth to preserve
  four rows of mine. Yielding the mids and letting the receipts stand as
  history (true in their moment, renumbered by this note) kept every
  session's witness intact at the cost of one cross-reference edit.
