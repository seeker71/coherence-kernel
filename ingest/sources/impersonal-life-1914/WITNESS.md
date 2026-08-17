# The Impersonal Life — transcript provenance

The work: *The Impersonal Life*, first published July 1914 by Sun Publishing
Co., Akron, Ohio, signed only **Anonymous**. Attributed to Joseph Sieber
Benner (3 January 1872 – 24 September 1938), an Akron printer and banker who
sat on the board of the Firestone Tire and Rubber Company and who used the
pen name "Anonymous" for every work he wrote.

Rights: public domain. A 1914 United States publication carries no subsisting
US copyright, and the author died in 1938, so life-plus-seventy jurisdictions
cleared it in 2008. This is the same rights class as the Gutenberg rows in
`learn/core-text-source-registry.fk`, recorded with the same honesty:
**public-domain-in-USA; jurisdiction-recheck-owed**.

## The two doors, and why only one of them is the carrier

Two carriers of the same public-domain words were opened, and they do not
offer the same door.

**The scan — open.** `archive.org/details/the-impersonal-life`, a Google
digitisation of the **Seventh Edition, October 1926**, uploaded to the
`opensource`/`community` collection with no access restriction. A faithful
reproduction of a public-domain book adds no new copyright of its own, so this
carrier's words are as free as the work's. Its own front matter carries the
print-run ladder, which is worth reading as a fact about the book:

    First Edition    1000 Copies   July, 1914
    Second Edition   5000 Copies   July, 1916
    Third Edition    5000 Copies   July, 1918
    Fourth Edition   6000 Copies   July, 1920
    Fifth Edition   10,000 Copies  April, 1922
    Sixth Edition   12,000 Copies  June, 1924
    Seventh Edition 12,000 Copies  Oct., 1926

A thousand copies in 1914, twelve thousand by 1926. The small print run is
witnessed in the carrier, not asserted from memory.

**The 2015 ebook — text open, artifact closed.** `yogebooks.com`'s typeset
edition states plainly: the text of the ebook is in the public domain, but the
ebook is not, and asks that the ebook not be distributed without
authorisation. That is a claim over the artifact — layout, ISBN, edition — and
explicitly *not* over the words. So the words were read through it; the
artifact was not copied, is not committed, and is not redistributed. Its
bibliography, copyright page and branding were stripped before anything
entered this tree.

The distinction matters and is the reason this file exists: **legally open is
not the same as offered**, and the registry already knows the difference
(FirstVoices, SuttaCentral). Here the two came apart inside a single work.

## What is in `impersonal-life.txt`

The Introduction (signed *The Publisher*) and all eighteen chapters, in order,
as running paragraphs. 32,620 words.

Removed, because they belong to the print carrier and not to the text: the
running page header stamped on every page, folio numbers, the contents
listing, and the 2015 carrier's own front and back matter. Repaired, because
they are artifacts of printing rather than of writing: drop-cap initials that
stand off from their word, and paragraphs split in two where a page header
interrupted them.

The period spelling is left exactly as printed — `thru`, `tho`, `enuf`,
`thruout`. It is not an error to be corrected.

## Cross-witness

The transcript was checked word-for-word against the independent 1926 scan.
Neither carrier was trusted alone.

Both rows are measured the same way — whole-corpus n-gram coverage over
lowercased, punctuation-stripped word streams, against the scan's book span
(its Google front matter and OCR decoration are not book text).

| direction | N=1 | N=3 | N=5 |
|---|---|---|---|
| transcript → 1926 scan | 99.48% | 93.43% | 87.34% |
| 1926 scan → transcript | 97.49% | 90.16% | 84.44% |

Read the first row: **99.48% of the transcript's words appear in a separately
sourced 1926 scan.** Read both rows together: coverage decays smoothly as N
grows, in both directions, with no chapter standing out. That is the signature
of uniform OCR noise in the scan — a five-word window fails if any one of its
five words is misread — and not of missing, added, or divergent text. A
dropped passage would have shown as one chapter collapsing while its
neighbours held. None does.

The residue is the scan's noise, not the transcript's: the scan renders
`conquer` as `conguer` and `within` as `opithin`, which is why the scan-side
numbers are the lower pair.

## Reproducing this

    curl -L -o scan.txt \
      https://archive.org/download/the-impersonal-life/The_Impersonal_Life_djvu.txt

Both source carriers were fetched on 2026-08-17. Archive.org returned 502 and
then repeated timeouts before serving; the item's datanode is reachable via
`https://archive.org/metadata/the-impersonal-life`.

## Sources

- 1926 Seventh Edition scan: https://archive.org/details/the-impersonal-life
- 2015 typeset edition (read, not copied): https://www.yogebooks.com/english/benner/1914impersonallife.pdf
- Author dates and pen name: https://en.wikipedia.org/wiki/Joseph_Sieber_Benner
