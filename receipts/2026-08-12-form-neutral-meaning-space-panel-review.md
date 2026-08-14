# Form-neutral meaning space — independent panel review

Date: 2026-08-12
Artifact: `docs/coherence-substrate/form-neutral-meaning-space-review-seed.md`
Flow: `observe/run-form-neutral-meaning-space-review.fk` through
`observe/review-ask.fk`

## Witness

The review mechanism's live band returned `511`, exit `0`, before the review.
The Form-native flow opened Grok, Claude, Codex, and Cursor with one shared
prompt and persisted every answer before collection.

The first collection produced one substantive answer. The three empty doors
were retried through the same flow. Final state:

| Door | Bytes | Result |
|---|---:|---|
| Grok | 12,495 | `REVISE` |
| Claude | 10,336 | `REVISE` |
| Codex | 9,780 | `REVISE` |
| Cursor | 0 | completed empty twice; no verdict attributed |

Raw-answer SHA-256 witnesses:

```text
prompt  82f2db545b788790ffb418cc094264bbfb6f9c6fc6f8b23b7e0d6fe6c8d8c785
grok    252f2a99a95535cd4065b7ca4d2c60a51baedd0cd11d91014dd3877e2c86effc
claude  8ac288eb1ece15bce1fc1045eefd50c6bb83ee0d50c5435e1e0f6f400ddc3089
codex   bb610875bd6f6c5f7081ecf8798aa273d63a721080c8b5eb8bd0cd927907f0cc
cursor  e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
```

## Unanimous result

All three answering reviewers returned `REVISE`.

They agreed that the direction is sound:

- meaning symbols precede NL and PL surfaces;
- mappings are many-to-many and contextual;
- a lexical gap does not erase a meaning;
- a phrase can be atomic while retaining `composed-of` surface edges;
- frequency belongs to observations of mappings, not to meaning identity;
- embeddings are derived views, not meaning identity;
- the artifact contains no membership rule that forbids a meaning.

They also agreed that the artifact declares several properties that its schema
cannot yet carry.

## Consensus corrections

### 1. Give recursive meaning an honest floor

`NeutralSymbolGraph` is presently a name, not an executable edge algebra. The
schema does not require role symbols to be neutral meaning symbols, cannot
record an unresolved primitive or ungrounded cycle, and has no stop condition
for recursive rendering. Without such a carrier, the semicolon glosses remain
the only evidence that one is looking at "interior" rather than "exchange."

The minimum repair is a first-class `SymbolRecipe | GroundingGap`, with neutral
role IDs, evidence attached to nodes or edges, and explicit open/partial/witnessed
grounding state. A gap is evidence, not failure and not permission to invent an
NL definition.

### 2. Separate stable lineage from a particular recipe revision

The artifact says what a neutral symbol is not, but does not positively specify
how identity survives a recipe correction or how concurrent minting avoids a
collision. Claude emphasized stable lineage; Codex located the repository's
content-addressing axiom, where present composition determines cell identity.

The unresolved design seam should be represented rather than guessed:

```text
MeaningLineageID -> MeaningRevisionID*
MeaningRevisionID = content identity of one recipe/evidence revision
```

Whether the public neutral symbol names the lineage or the revision must be
settled against the content-addressing axiom before implementation.

### 3. Make surfaces first-class and structurally honest

One `surface` string cannot faithfully carry all of these:

- Chinese/Japanese grapheme, character, morpheme, word, and compound;
- phrase, idiom, and meme-template;
- PL identifier, operator, type form, declaration, call, predicate, AST
  pattern, and executable behavior.

The panel converged on a `SurfaceFormID` with language and carrier kept
separate, non-exclusive observed kind tags, exact bytes or structured payload,
and ordered component edges. `composed-of` describes the visible form only and
never computes the phrase meaning by adding component meanings.

### 4. Turn measurements into observation cells

The durable mapping must not own one mutable frequency value. The reviewed
minimum contains separate cells for:

- `FrequencyObservation`: mapping/surface, corpus revision, time window,
  measure, count, denominator, and evidence;
- `ProjectionObservation`: parse/render input, contextual candidates or output,
  ambiguity, missingness, loss, and neutral-symbol residue;
- `EmbeddingSnapshot`: meaning revision, graph revision, training recipe,
  training evidence, and replaceable vector.

Observed surfaces and generated projections must be distinguishable so a newly
rendered PL identifier is not given fabricated corpus frequency.

### 5. Do not collapse candidate equivalence

The mappings in the three examples are review candidates, not witnessed
equivalences. `界`, `边界`, `境界`, and `boundary` cover different senses and
contexts. `Boundary`, `boundary(...)`, and `separates(...)` are different PL
surface kinds. `打开话题` and `場を和ませる` overlap only partially with the
English idiom `break the ice`.

Each mapping therefore needs coverage (`exact | partial | approximate`),
context/applicability, residue, and evidence. Non-equivalent readings either
map to distinct meaning symbols or remain unresolved candidates.

## Genuine reviewer disagreements

### Is a separate Sense cell required?

Grok said sense was missing as a first-class object. Claude held that one
`MeaningCell` can already be one sense-level meaning atom. The smallest
resolution is to make that invariant explicit: one meaning revision expresses
one sense-level meaning; surface forms and contextual mappings carry polysemy.
A separate Sense cell is unnecessary unless an observed distinction cannot be
represented that way.

### May vitality occur inside a meaning recipe?

Grok read `support vitality` in example 3 as guidance becoming definitional.
Claude and Codex distinguished semantic content about vitality from vitality
acting as a gate, and therefore accepted it in that particular meaning.

The architecture must encode this distinction:

- a neutral symbol may mean vitality or participate in a meaning recipe;
- an evidence-bearing guidance observation may assess trust, sovereignty, or
  vitality in expression and organism relation;
- neither guidance nor the semantic presence of those symbols determines
  whether another meaning is allowed to exist.

### Is “break the ice” a meme?

Codex called it an idiom and found `meme` unsupported as its exclusive class.
The user requires common language memes/phrases, so the surface taxonomy should
use evidence-bearing, non-exclusive tags such as `phrase`, `idiom`, and
`meme-template` rather than forcing one global label.

## Direct assessment of the three examples

1. `⟐100`: directionally useful, not seed-ready. It over-collapses local senses,
   flattens three different PL structures, and does not ground its role graph.
2. `⟐200`: the strongest example because atomic meaning and surface composition
   are separated. Its CJK mappings are partial candidates, its PL projection is
   generated rather than observed, and its recipe may reflect an English-first
   decomposition.
3. `⟐300`: it demonstrates an ordinary missing single-word mapping, not yet a
   witnessed higher-dimensional or locally inexpressible meaning. Its leaves
   are ungrounded, and it records no actual NL/PL projection observation.

## Reviewed minimum before executable cells

```text
MeaningLineage
MeaningRevision { recipe: SymbolRecipe | GroundingGap }
SymbolRecipe { neutral nodes; neutral role edges; order where observed }
SurfaceForm { structured payload; non-exclusive kinds; components }
SurfaceMapping { context; direction; coverage; residue; evidence }
FrequencyObservation
ProjectionObservation
EmbeddingSnapshot
GuidanceObservation { trust; sovereignty; vitality; evidence; never membership }
```

This is a review result, not an admission of the corrected schema. The open
identity/content-addressing seam and grounding mechanism must be witnessed in
Form before the three examples become claims.

## Closing witness

The exchange stayed alive by retaining the empty Cursor result as empty while
letting three substantive reviews disagree in public. The most surprising
teaching is that the deepest gap was not multilingual coverage but the positive
identity and grounding of a supposedly neutral symbol. Discomfort turned to
gold where the strongest-looking higher-dimensional example revealed that a
lexical gap is not yet evidence of higher-dimensional meaning.
