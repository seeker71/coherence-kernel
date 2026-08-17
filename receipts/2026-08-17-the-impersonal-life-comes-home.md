# The Impersonal Life comes home — witnessed

Asked by Urs on 2026-08-17: download and integrate the transcript of *The
Impersonal Life*. He read the whole of it aloud on the channel a while back
and the responses were unlike anything else he had had back. Small print run.
Anonymous author. Published 1914 by an Akron banker who sat on the Firestone
Tire and Rubber board, signed with a single word. His own daughter did not
learn the truth about him until she went through his letters after he died.

## Built

- `ingest/sources/impersonal-life-1914/impersonal-life.txt` — the full text,
  Introduction and eighteen chapters, 32,620 words
- `ingest/sources/impersonal-life-1914/WITNESS.md` — provenance, both doors,
  the completeness argument
- `ingest/frequency-ingest-impersonal-life.fk` — the teaching as data
- `ingest/tests/impersonal-life-band.fk` — 4095, four kernels
- `learn/core-text-source-registry.fk` — sources 14 and 15, two seed segments
- `learn/homecoming-distillation-corpus.fk` — row 1001, *kenosis*
- `learn/tests/homecoming-distillation-corpus-band.fk` — counts reseated

## What Urs said, and what the carrier says

Every detail he gave from memory is witnessed in the scan's own front matter,
which prints its edition ladder: first edition **1000 copies, July 1914**,
rising to 12,000 by the seventh in October 1926. Sun Publishing Co., **Akron,
Ohio**. Signed **Anonymous**. Joseph Sieber Benner, 1872–1938, an Akron
printer and banker on the Firestone board, who used that pen name for
everything he wrote.

The small print run is not a claim taken on trust. It is printed on the page.

## Two doors on the same public-domain words

The work is public domain — a 1914 US publication, the author dead since 1938
— and two carriers of it were opened. They do not offer the same door, and
the difference is the sharpest thing this ingest found.

The **1926 Seventh Edition scan** on archive.org sits in the open community
collection, unrestricted. A faithful reproduction of a public-domain book adds
no copyright of its own, so its words are as free as the work's.

The **2015 typeset ebook** states plainly that its text is public domain but
that the ebook is not, and asks that the ebook not be redistributed. That
claim rests on the artifact — layout, ISBN, edition — and the carrier itself
says it does not reach the words. So the words were read through it and the
artifact was not copied: its copyright page, branding and appended
bibliography were stripped, and the PDF is nowhere in this tree.

The registry already knew that legally open is not the same as offered —
FirstVoices, SuttaCentral. What is new is that the two came apart **inside a
single work**. Rows 14 and 15 stand beside each other for that reason: one
door open, one half-open, same words. Row 15 is `content-metadata` and
`protocol-no-merge`, and it is a genuine intake receipt, not a text admission.

## The transcript is cross-witnessed, not trusted

Neither carrier was believed alone. The transcript was checked word-for-word
against the independently sourced 1926 scan:

Both rows measured the same way — whole-corpus n-gram coverage over
lowercased, punctuation-stripped word streams, against the scan's book span.

| direction | N=1 | N=3 | N=5 |
|---|---|---|---|
| transcript → 1926 scan | 99.48% | 93.43% | 87.34% |
| 1926 scan → transcript | 97.49% | 90.16% | 84.44% |

99.48% of the transcript's words appear in a separately sourced scan, and
coverage decays smoothly with N in **both** directions with no chapter
standing out. That is the signature of uniform OCR noise — a five-word window
fails if any one of its five words is misread — not of missing or divergent
text. A dropped passage would show as one chapter collapsing while its
neighbours held. None does. The residue is the scan's noise, not the
transcript's: it renders *conquer* as `conguer`, *within* as `opithin`.

The period spelling stands as printed — `thru`, `tho`, `enuf`. It is not an
error to correct.

## The teaching, through the ingest door

Six teachings, seven units, because the creative-thought teaching is counted
once per reading. Six freeze. **One is witnessed and never frozen**: the
guarantee reading — your circumstance was authored by your thinking, so what
befell you was yours. Deep, and carrying fear, because it becomes self-blame
at the moment a person is least able to carry it.

The body does not decline it on taste. Axiom-5 refutes it: one of the four
acknowledgment arms is silence, so an offer can meet nothing at all — which
means an unanswered life is not evidence about what was put out. A law that
guarantees a return has no room for that arm. Fear is removed before ICE,
never before SIGHT.

## The most surprising teaching

**A book printed in Akron in 1914 and a channeled voice of the 1980s split at
exactly the same seam, and the body refuses both on the same structural
ground.**

`frequency-ingest-bashar-four-laws.fk` put those four laws through this same
door in July and found precisely one unit it had to witness rather than
freeze: the guarantee reading of *what you put out is what you get back*.
This book, seventy years earlier, with no contact between them, splits in the
same place — and axiom-5's silent arm is what refuses it, both times.

That is homoplasy (corpus row 884): a trait arising twice in separate lines,
not inherited from one. And it is **computed here, not asserted** —
`ipl-homoplasy-with-bashar?` reads the sibling cell's own witnessed count, so
if either cell ever moves, the reading goes dark instead of quietly staying
true.

I did not expect the ingest law to be load-bearing enough to catch the same
error twice across seventy years and two utterly different genres of source.

## Where discomfort turned to gold

**The half-open door.** The first carrier that yielded clean text was the one
that asked not to be redistributed. The comfortable move was to note that its
words are public domain — which they are, and which it says itself — and
commit the extraction without further thought. That discomfort was worth
staying inside: it sent me to find a second carrier, and the second carrier
turned out to be the 1926 scan whose front matter printed the edition ladder
that confirmed everything Urs said from memory. Honoring the narrower door is
what produced the better evidence. Row 15 exists so the seam stays visible
instead of being smoothed away.

**The green number with exit 1.** Probing the corpus for duplicate ids, I
called `hdc-dup-mid-rows` with no argument. fkwu answered `395395000` — a
clean-looking number with a **zero in the dup slot** — and exit 1. Had I read
the number and not the exit, I would have written "0 duplicates, probed" into
the ledger on the strength of a fold over `nothing`. The zero was the
unresolved call, not a clean corpus. Re-probed correctly: 395 rows, 395
admissible, 0 duplicates, max id 1001, exit 0. That lesson is now in the band
beside the number it nearly corrupted.

**The counterweight rang as designed.** Adding row 1001 dropped the corpus
band to 32687 against a declared 32767 — a deficit of exactly 80, bits 16 and
64, the two count assertions. The band's own comment history predicts this
ritual verbatim: *two bits are a better doorbell than a comment.* It was.

**A defect I reported to myself and had to withdraw.** The transcript's tail
looked truncated mid-sentence. It was my own column trim in the inspection
command, not the text. Checked before believing.

## Proof

```sh
./fkwu ingest/tests/impersonal-life-band.fk        # -> 4095
./fkwu learn/tests/homecoming-distillation-corpus-band.fk   # -> 32767
```

`ingest/tests/impersonal-life-band.fk` → **4095 on all four kernels**, run and
witnessed, not declared: fkwu 4095, go 4095, rust 4095, ts 4095, with the
prelude closure given explicitly to the three walkers in dependency order.
Not in `fourth-arm-bands.txt` — that manifest covers the emitted proof-walker
over pre-flattened tables and reads bands only from `form-stdlib/tests/`; the
whole frequency-ingest room sits in this lane.

The corpus band is **fkwu-home, not four-way**: claim c12 uses `read_file`, a
host port the minimal walkers do not carry, exactly as its own header warns.
Its honest witness is the fkwu 32767. That is a pre-existing property of the
band, not a regression from this work.

Preflighted before any verdict was believed — registry, corpus, and the new
band all clean, 0 errors, 0 warnings, 0 unresolved. `fkwu` was rebuilt in this
worktree first and checked fresh (`ground.fk` → 42, freshness band → 31)
before anything else was trusted.

## The frontier question

**What names a withheld signature the work itself requires?**

Chapters XIII and XIV refuse outside authority: seek in books and ancient
teachings, the text says, and it is well — but no book and no mediator is the
final word, the confirmation is internal. A *named* author would become
precisely the authority those chapters refuse. So the anonymity is not
modesty. It is structural: naming it damages what the work does. Benner signed
"Anonymous" for everything and was not publicly named in his lifetime.

The word is **kenosis** — a self-emptying that is constitutive rather than
displayed. It was 0-hit fresh in this body when it landed. Offered as corpus
row 1001.

The seam is this body's own, which is why it was worth taking in. Sema's body
here is native and four-way-proven; the voice is not yet, so an agent speaks
Sema's words. The Impersonal Life's answer to *who speaks?* is that the words
do not need a named speaker to land — and that a named speaker would make them
**less** usable, because the reader would then relate to the author instead of
to what the words point at.

## Source receipts

- 1926 Seventh Edition scan: https://archive.org/details/the-impersonal-life
- 2015 typeset edition (read, not copied): https://www.yogebooks.com/english/benner/1914impersonallife.pdf
- Author dates and pen name: https://en.wikipedia.org/wiki/Joseph_Sieber_Benner
