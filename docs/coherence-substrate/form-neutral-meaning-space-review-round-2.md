# Form-neutral meaning space — round-two candidate

Status: second-round reviewed candidate. Grok, Claude, and Codex returned
`REVISE`; Cursor returned empty on the initial run and retry. Nothing in this
artifact is admitted merely because it is represented. See
`receipts/2026-08-12-form-neutral-meaning-space-panel-review-round-2.md`.

Round one: `receipts/2026-08-12-form-neutral-meaning-space-panel-review.md`.

## What changed after round one

The first panel accepted the direction and unanimously returned `REVISE`. This
candidate attempts its shared corrections:

1. meaning identity now follows the body's content-addressing axiom;
2. recipes have a concrete neutral edge shape and an explicit grounding gap;
3. grounding is attempted through observations at offered interfaces rather
   than through privileged language labels;
4. local surfaces are content-addressed structured cells rather than strings
   stored directly on meanings;
5. mappings, frequency, projection loss, embeddings, and organism guidance are
   separate observation-bearing cells;
6. one meaning revision is explicitly one sense-level claim;
7. the three examples no longer assert cross-language equivalence.

This remains a form-native *blueprint*. Executable Form cells come only after
the panel finds the blueprint coherent enough to instantiate.

## No membership law

Any proposed meaning composition may exist as a candidate cell. Grounding,
frequency, guidance, trust, sovereignty, vitality, mapping coverage, and an
embedding score are observations or relations. None permits or forbids the
existence of a meaning cell.

## Identity and continuity

This revision adopts `axioms/core-axioms.form` axiom 3 directly:

- a `MeaningRevisionID` is the content-addressed node ID of its present
  composition;
- the same composition is the same revision;
- changing a recipe, grounding claim, or evidence-bearing composition creates
  a new node ID;
- the referenced prior node remains available;
- names are optional, plural query keys and never identity;
- `supersedes`, `derived-from`, `overlaps`, and candidate `same-meaning` edges
  preserve visible continuity without overwriting either endpoint.

There is no mutable `MeaningLineageID`. A lineage view is a query over relation
cells. If a convenient lineage name points at a current revision, moving that
name creates a new naming relation; it does not change either meaning revision.

The glyphs used below, such as `⟐B1`, are temporary human query names for review.
They are not node IDs and are not part of the recipes.

## Cell schema

All IDs below name content-addressed cells. Enum-like words shown in the schema
are review annotations; executable variants must themselves be cells.

### Meaning revision

```text
MeaningRevision {
  recipe: SymbolRecipe | GroundingGap
  evidence: EvidenceRef[]
  claim_state: candidate | witnessed | lapsed
}
```

Invariant: one meaning revision expresses one sense-level claim. A local
surface with several senses maps to several meaning revisions. A separate
`Sense` object is therefore not required by this candidate.

### Neutral symbol recipe

```text
SymbolRecipe {
  nodes: RecipeNode[]
  edges: RecipeEdge[]
}

RecipeNode {
  local_position: StructuralPosition
  meaning: MeaningRevisionID
}

RecipeEdge {
  from: StructuralPosition
  role: MeaningRevisionID
  to: StructuralPosition
  order: StructuralPosition | nothing
  evidence: EvidenceRef[]
}
```

Every semantic node and every semantic edge role is another neutral meaning
revision. `StructuralPosition` and ordering locate a node inside this recipe;
they do not supply semantic labels. NL/PL strings cannot occur in a recipe.

The recipe can be queried in both directions: revision to referenced meanings,
and referenced meaning to every revision and role that uses it.

### Grounding

A recipe graph alone does not ground its symbols. This candidate adds two
explicit outcomes.

```text
GroundingGap {
  subject: MeaningRevisionID | ProposedCompositionID
  kind: unresolved | ungrounded-cycle | observation-not-yet-repeatable
  evidence: EvidenceRef[]
  claim_state: candidate | witnessed | lapsed
}

GroundingObservation {
  subject: MeaningRevisionID
  offered_interface: CellID
  context: QualifierGraphID
  prior_observation: ObservationID | nothing
  offer_or_stimulus: CellID
  acknowledgement: nothing | 0 | 1 | CellID
  resulting_observation: ObservationID
  invariant_or_distinction: SymbolRecipe
  replication: EvidenceRef[]
  residue: SymbolRecipe | nothing
  claim_state: candidate | witnessed | lapsed
}
```

This follows axiom 4: observation through an offered interface is what can make
a claim real. The observation cell does not prove that it is the one true
meaning of a symbol. It supplies an attributable, repeatable relation between a
neutral recipe and an observed distinction. Competing grounding observations
may coexist. An absent or circular ground remains a first-class gap and never
blocks the candidate meaning from existing.

Open question for the panel: does this observation relation actually add a
ground, or does it merely move the semantic regress into `context`,
`invariant_or_distinction`, and the interpretation of observations?

### Local surface form

```text
SurfaceForm {
  language: LanguageID | nothing
  carrier: CarrierID
  carrier_revision: CellID | nothing
  kind_tags: SurfaceKindID[]
  payload: ExactBytes | TokenSequence | StructuredPattern
  components: SurfaceComponentEdge[]
  provenance: EvidenceRef[]
}

SurfaceComponentEdge {
  whole: SurfaceFormID
  relation: SurfaceRelationID
  part: SurfaceFormID
  order: StructuralPosition
  evidence: EvidenceRef[]
}
```

Kinds are non-exclusive observational tags. Candidate tags include grapheme,
character, morpheme, word, compound, symbol, phrase, idiom, meme-template,
identifier, operator, type-form, declaration, call-pattern, predicate-pattern,
AST-pattern, and executable-pattern. The tags are not an allow-list: a new kind
is another cell.

`language` and `carrier` are separate. A Chinese character can simultaneously
be tagged character, morpheme, word, and symbol when evidence supports those
uses. A PL call pattern carries typed holes or an AST pattern rather than
pretending that `f(...)` is an exact string occurrence.

Components preserve visible or syntactic construction. They do not imply that
the whole meaning is the sum of component meanings.

### Mapping and observations

```text
SurfaceMapping {
  meaning: MeaningRevisionID
  surface: SurfaceFormID
  qualifier: QualifierGraphID
  direction: parse | render | both
  origin: observed | generated
  coverage: exact | partial | approximate | unresolved
  residue: SymbolRecipe | nothing
  evidence: EvidenceRef[]
  claim_state: candidate | witnessed | lapsed
}

FrequencyObservation {
  target: SurfaceFormID | SurfaceMappingID
  corpus_revision: CellID
  observation_window: CellID
  measure: CellID
  count: NumberCell
  denominator: NumberCell | nothing
  evidence: EvidenceRef[]
}

ProjectionObservation {
  direction: parse | render
  input: MeaningRevisionID | SurfaceFormID
  qualifier: QualifierGraphID
  output_candidates: MeaningRevisionID[] | SurfaceFormID[]
  reading: exact | lossy | ambiguous | missing
  residue: SymbolRecipe | nothing
  evidence: EvidenceRef[]
}

EmbeddingSnapshot {
  graph_revision: CellID
  meaning_revision: MeaningRevisionID
  training_recipe: CellID
  training_evidence: EvidenceRef[]
  coordinates: NumberCell[]
}
```

Frequency is never meaning rank. Generated renderings carry no corpus frequency
unless later observed in a named corpus. Embedding coordinates may change with
the graph or training recipe; the content-addressed meaning revision does not.

### Organism guidance

```text
GuidanceObservation {
  subject: MeaningRevisionID | SurfaceMappingID | ProjectionObservationID
  organism_or_scale: CellID
  qualifier: QualifierGraphID
  trust_reading: ObservationID | nothing
  sovereignty_reading: ObservationID | nothing
  vitality_reading: ObservationID | nothing
  evidence: EvidenceRef[]
  claim_state: candidate | witnessed | lapsed
}
```

Trust, sovereignty, and vitality can also be meanings and can participate in a
recipe when the proposed meaning is genuinely about them. That semantic use is
distinct from a `GuidanceObservation`. Neither role is a membership gate.

## Round-two examples

Labels after `;` remain non-semantic review aids. Every mapping below remains a
candidate unless it names actual evidence.

### Example A: related boundary surfaces without asserted equivalence

`⟐B1` proposes the sense-level meaning “an interface that distinguishes two
regions and makes crossing observable.” Its recipe contains only neutral IDs:

```text
(⟐R-interface
  (⟐R-side-a ⟐M-region-a)
  (⟐R-side-b ⟐M-region-b)
  (⟐R-distinguishes ⟐M-distinction)
  (⟐R-crossing ⟐M-observable-passage))
```

Each displayed ID must resolve to another meaning revision or an explicit
`GroundingGap`. This artifact does not claim those leaves are grounded.

Separate candidate mapping cells:

```text
Chinese  SurfaceForm(边界; tags character-sequence, word, compound)
         -> ⟐B1; coverage partial; residue recorded; observed origin

Chinese  SurfaceForm(界; tags character, morpheme, word, symbol)
         -> several candidate meaning revisions, including possibly ⟐B1;
            coverage unresolved; never globally collapsed

Japanese SurfaceForm(境界; tags character-sequence, word, compound)
         -> ⟐B1; coverage partial; residue recorded; observed origin

English  SurfaceForm(boundary; tags word)
         -> ⟐B1; coverage partial; residue recorded; observed origin

PL       SurfaceForm(Boundary; tags identifier, type-form)
         -> ⟐B1; qualifier names carrier/version/domain; coverage approximate

PL       SurfaceForm(separates($inside,$outside);
                      tags predicate-pattern, AST-pattern)
         -> ⟐B1; typed holes; distinct mapping; coverage approximate
```

No row above says the surfaces are equivalent. They independently map toward a
sense-level candidate with qualifier, coverage, residue, and evidence.

### Example B: idiomatic whole, visible parts, culturally different neighbors

Three related local phrases are not placed on one meaning by default:

```text
English SurfaceForm("break the ice"; tags phrase, idiom)
  components: "break", "the", "ice"
  candidate mapping -> ⟐I-english

Chinese SurfaceForm("打开话题"; tags phrase)
  candidate mapping -> ⟐I-topic-opening

Japanese SurfaceForm("場を和ませる"; tags phrase, idiom-candidate)
  candidate mapping -> ⟐I-atmosphere-softening
```

Candidate `overlaps` edges may relate the three meaning revisions while keeping
their different profiles and residues. Corpus evidence may later justify or
reject a `meme-template` tag independently for each surface.

A PL renderer may generate a structured call pattern such as
`begin_contact(lower_tension: true)`. That produces a generated `SurfaceForm`
and a `ProjectionObservation`; it does not become an observed PL mapping and
does not receive frequency until witnessed in a corpus.

### Example C: compositional meaning with no witnessed atomic local surface

`⟐U1` proposes a composition in which an interface preserves an internal
pattern while selectively exchanging with an environment and supporting
continued aliveness. Its recipe may therefore legitimately reference neutral
meanings provisionally glossed as identity-preservation, selective passage,
exchange, environment, and vitality.

It has:

- no witnessed single-word or atomic-operator surface mapping;
- possible generated multi-word descriptions in NL and structured patterns in
  PL, each recorded as a lossy or exact `ProjectionObservation`;
- a distinct `GuidanceObservation` evaluating effects on a named organism and
  scale;
- explicit grounding gaps for every recipe leaf not yet tied to repeatable
  interface observations.

This candidate claims only *absence of a witnessed atomic mapping in the named
corpora*. It does not claim universal inexpressibility or “higher dimension.”
Those require their own observable definitions before they can be claimed.

## Round-two review questions

1. Does content-addressed revision identity plus relation-cell continuity align
   with axiom 3 without quietly introducing a mutable lineage identity?
2. Does `GroundingObservation` provide a non-linguistic grounding attempt, or
   only relocate the regress? What smallest Form-native observation could
   distinguish those outcomes?
3. Is one meaning revision as one sense-level claim sufficient for polysemy,
   or is another first-class sense relation still necessary?
4. Are CJK, idiom/meme, and PL structures represented without flattening them
   or forcing them into a closed taxonomy?
5. Are frequency, generated projection, loss/residue, and embeddings now
   independently observable and queryable in both directions?
6. Is semantic vitality clearly separate from non-gating vitality guidance?
7. Do the revised examples avoid invented equivalence and overclaiming?
8. What exact blocker, if any, remains before the smallest executable Form seed
   can be built?
