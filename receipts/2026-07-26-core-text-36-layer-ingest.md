# Core-text 36-layer ingest — witnessed

Asked by Urs on 2026-07-26: begin ingesting texts as old and as Indigenous as
possible, favor texts with many open translations, and use them to build the
first 36 layers of symbols, queries, and responses.

## Built

- `learn/core-text-source-registry.fk`
- `cognition/core-text-36-layer-ingest.fk`
- `cognition/tests/core-text-36-layer-ingest-band.fk`
- `cognition/tests/core-text-36-layer-structure-four-way-band.fk`
- `teachings/core-text-36-layer-ingest.form`

## Admission law

An edition enters only after four separate facts agree:

1. legal rights permit reuse;
2. community/steward protocol permits this use;
3. an exact edition or item is selected;
4. the rights/protocol witness is fresh.

Every rights row carries a scope. Project Gutenberg's findings are recorded as
`public-domain-in-USA; jurisdiction-recheck-owed`, not universal permission.
A missing rights scope fails closed as unresolved.

The response preserves `nothing | 0 | 1 | node`:

- `nothing`: rights or exact-item state remains unresolved;
- `0`: explicit refusal or incompatible terms;
- `1`: exact source segment admitted at this epoch;
- `node`: community review, exact-item selection, or re-witness is owed.

The 36 recursive layer queries run as two explicit 18-step waves. The existing
native inquiry node is a base-8 derivation path stored in a signed integer; a
single 36-deep path overflowed in the first live attempt and stopped at 24
responses. The checkpoint preserves every response and makes that carrier
boundary observable.

## Source floor

At the 2026-07-26 witness epoch, the intended seed partition is:

- 6 admitted, edition-bound public-domain segments (web footnote markers
  excluded and line breaks normalized to spaces), under the exact source-stated
  rights scope:
  - Tao Teh King, James Legge 1891, chapter 1.1;
  - Babylonian Legends of the Creation, E. A. Wallis Budge 1921,
    First Tablet lines 1-3;
  - Genesis 1:1 in World English Bible, Louis Segond 1910,
    Reina-Valera 1909, and eBible's Delitzsch-attributed Hebrew edition.
- 3 review nodes:
  - the 1908 English Popol Vuh mediation requires K'iche' community review;
  - StoryWeaver requires an exact book and complete creator attribution;
  - Giles' "The Light Hubs" has exact member-witnessed permission and no
    additional cultural or benefit conditions; freshness has no scheduled
    review and re-witnesses on change. Final Satsang ratification remains under
    community review; the source is metadata-only and awaits a revocable
    carrier.
- 2 refusals:
  - FirstVoices forbids scraping, merging, and republication without authority;
  - SuttaCentral requests that its content not be used to create generative-AI
    datasets, despite CC0/public-domain layers.
- 2 unresolved catalogs:
  - AILLA access and rights vary by item;
  - African Storybook licensing and attribution vary by story.

Metadata admission is not text admission. The refused and unresolved bodies
are not copied into the corpus.

## Source receipts

- Project Gutenberg permissions:
  https://www.gutenberg.org/policy/permission
- Tao Teh King edition 216:
  https://www.gutenberg.org/ebooks/216
- Babylonian Legends text:
  https://www.gutenberg.org/cache/epub/9914/pg9914.html.utf8
- Popol Vuh edition 56550:
  https://www.gutenberg.org/ebooks/56550
- eBible public-domain policy and per-edition checking:
  https://ebible.org/legal.php
- World English Bible:
  https://ebible.org/eng-web/copyright.htm
- Louis Segond 1910:
  https://ebible.org/fraLSG/copyright.htm
- Reina-Valera 1909:
  https://ebible.org/spaRV1909/copyright.htm
- Hebrew edition:
  https://ebible.org/heb/copyright.htm
- CARE Principles:
  https://www.gida-global.org/careprinciples
- Local Contexts Traditional Knowledge Labels:
  https://localcontexts.org/labels/traditional-knowledge-labels/
- AILLA:
  https://ailla.utexas.org/
- FirstVoices conditions:
  https://www.firstvoices.com/conditions-of-use
- SuttaCentral licensing:
  https://suttacentral.net/licensing
- African Storybook terms:
  https://africanstorybook.org/terms.html
- StoryWeaver open-content terms:
  https://storyweaver.org.in/en/open-content

## Witness

Observed on 2026-07-26:

```text
./fkwu --src cognition/tests/core-text-36-layer-ingest-band.fk
-> [nothing, 0, 1, 3040004007, [6, 3, 2, 2], 6, 4, 36,
    3040007001, nothing, 14, 4095]

./fkwu --src cognition/tests/core-text-36-layer-structure-four-way-band.fk
-> 127
```

The structural source was expanded as documented preludes + recipe + band
(bare `import` declarations removed only for the three deliberately minimal
walkers):

```text
walkers/go/walker /dev/stdin
-> 127

walkers/rust/target/release/form-walker-rust /dev/stdin
-> 127

node --experimental-strip-types walkers/ts/main.ts /dev/stdin
-> 127
```

The first community relationship door now lives in
`learn/hati-suci-core-text-invitation.fk`: Hati Suci is pilot 1 through an
initiating member. Source 13 now identifies Giles' "The Light Hubs" by exact
hash as Satsang's selected first material. The registry has thirteen rows.
Member-witnessed scoped permission moves the new metadata-only row from
unresolved to review. The initiating member then records no additional
cultural restriction or benefit condition. Freshness is then recorded as no
scheduled review, with re-witness on change. Final ratification and
metadata-only state still prevent admission, and no Hati Suci text body enters.

Neighboring laws remained present:

```text
corpus-license-gate-band.fk                  -> 31
language-neutral-symbol-identity-band.fk     -> final success 1
native-inquiry-derivation-band.fk            -> final success 1
native-inquiry-derivation-tamper-band.fk     -> final success 1
identity-space-structure-four-way-band.fk    -> 127
core-word-ack-band.fk                        -> 111111111111
core-word-inquiry-band.fk                    -> 2147483647
core-word-inquiry-actual-acks-witness.fk     -> 15
```

## Lessons from the attempt

The first native run refused an extra closing parenthesis as
`unbalanced-source`; no verdict was claimed. After repair, the first semantic
run returned score 4063 and only 24 of 36 recursive responses. The cause was
not a semantic rejection: the base-8 derivation node outgrew its signed integer
carrier. Two explicit 18-step waves then returned all 36 responses and score
4095. The carrier limit is now named in the cell and teaching.

Directly handing an importing test file to the minimal walkers failed on
unbound `import`. The corrected run used their documented concatenated-source
interface and returned 127 on all three; no failed invocation was counted as a
proof.

## Honest boundary

The 36 layers are a first versioned semantic spine, not universal categories
that all languages or communities must accept. The six tiny segments prove the
door and translation-alignment shape; they are not yet a representative
ancient or Indigenous corpus.

No living Indigenous text has been admitted in this first movement. That is
intentional: a relationship and an exact community-issued protocol are still
owed. The next honest build is a community-selected pilot whose attribution,
access, benefit, removal, and downstream-use terms are executable.

## Verdict

OBSERVED at the honest seam:

- six tiny edition-bound public-domain source segments enter;
- four translations align without identity collapse;
- community review, refusal, and unresolved states remain distinct from
  admission;
- 36 semantic layers and 252 layer/query symbols are unique;
- the full `nothing | 0 | 1 | node` intake passes on native fkwu;
- the pure address/admission structure agrees across all four proof arms.
