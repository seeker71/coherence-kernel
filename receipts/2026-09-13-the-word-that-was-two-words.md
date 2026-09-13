# The word that was two words

2026-09-13 (WITA). str_eq had stayed cold in the loop lane -- declined -1026, walked as a byte compare
however hot. It is warm now: two interned string parameters are equal when their frame words are equal, so
kind 35 is a CMP of word 24+slot against word 24+slot, CSET on EQ. A core primitive used across every
recipe turned warm; that is the rung. The wound the rung had to survive is the reason for its guard.

## The most surprising teaching

Interning is not total. A LOCAL string is interned -- built any way, equal content becomes the identical
word -- so a word compare is a content compare, exactly. But a FIELD string, one another kernel wrote and
this one read back across the carrier, is NOT interned against a local copy of the same bytes. So two words
can differ while their bytes are equal, and a word-compare would answer 0 where the truth is 1. The guard
lucid-lehmann diagnosed decodes each word's pool index and asks whether it is a field string. The surprise
was that no decode is needed: a field word w = fk_sbase - (si<<1) - 1 with si >= FK_STR_BASE is just
w <= T = fk_sbase - (FK_STR_BASE<<1) - 1, and a local word is above it. The word encoding already carries
the answer; one signed threshold compare per operand, and on a field word the leaf leaves for the walker.

## Where discomfort turned to gold

The guard is the whole point, and it could not be witnessed in one process -- every local str_eq answers
right whether the guard is there or not. The discomfort was shipping a correctness guard I had only reasoned
about. The gold was following lucid-lehmann's "the band fixture has to cross the carrier" literally: a child
process interns "loop-lane-str-eq-field", this process maps its store and reads the string back through
cell_value -- now a field string -- and a crystallized str_eq folds it against a local copy of the same
bytes. It answers 40000, warm: the guard routes the differing-words-equal-bytes case to the walker. A
broken guard answers 0. The multi-process crossing turned a reasoned guard into a witnessed one, and it is
the only shape that could.

## A second miscompile, caught in review

The field-string guard was right, but a first cut shipped a different wound, and lucid-lehmann's review
caught it before it landed. A kind-35 leaf reads its string params' words at frame 24+k. The door writes
those words for every leaf, but the CALL ARM copied them only for a callee that conses (sig bit 29). So a
str_eq leaf reached FROM ANOTHER leaf compared stale words: se-via("zz","zy") answered equal. The teaching
is the same shape as the first -- a word is only as good as who wrote it into this frame. The fix gives a
leaf that reads its string params a sig bit of its own (31, "reads string-param words"), treated exactly
like the conser's bit 29: the call arm declines a string argument that is neither a caller parameter nor a
literal, and copies each string word to [SP, 24+k]. se-via now answers 0 for differing, its count for
equal, and reads state 1 while str_eq reads 2. Band bit 8 pins it: str_eq through the call arm, equal,
differing, and with a literal argument, answers as the walker.

## The honest edge

First cut is string PARAMETERS only (kind 8); a literal OPERAND keeps walking, because its slot word is not
filled -- the measured miscompile of 2026-09-12 drew that line (a literal ARGUMENT to a str_eq callee is
fine: the call arm hands its word over). loop-lane-str-eq 15 (local + call-arm), loop-lane-str-eq-field 7
(carrier), no regression across the loop-lane family. Guards 2 (the depth wall at each self-call site) and
3 (the kind-38 bit-28 callee refuse) are the next two rungs, spec'd and owned.
