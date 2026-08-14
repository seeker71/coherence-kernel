# Form-neutral meaning space — review seed

Status: independently reviewed candidate; three substantive reviewers returned
`REVISE`. This is not an admitted specification. See
`receipts/2026-08-12-form-neutral-meaning-space-panel-review.md`.

## Intent

The base is not any natural-language dictionary and not a universal list of
English-named concepts. The base is a form-neutral space of meanings.

Every meaning may have its own neutral symbol, including a higher-dimensional
meaning for which no natural or programming language has a local word, phrase,
operator, type, or idiom. A meaning symbol is not the spelling of its name.

A meaning's description is a Form-native recipe or graph made only from other
neutral meaning symbols. Structural roles used by the recipe are also neutral
symbols. Human-readable labels shown beside symbols are commentary and never
part of the identity or executable description.

Natural languages and programming languages are projections of this neutral
space. Their words, characters, symbols, phrases, memes, identifiers,
operators, types, and executable patterns map to and from meaning symbols.

There are no rules that decide which meanings are allowed to exist. Trust,
sovereignty, and vitality guide how a meaning is expressed, evaluated, and
related to an organism; they do not gate membership in the meaning space.

## Minimal cell families

### Meaning cell

```text
MeaningCell {
  symbol: NeutralSymbolID
  recipe: NeutralSymbolGraph
  provenance: Evidence[]
  state: candidate | witnessed | lapsed
}
```

`recipe` contains only neutral symbol IDs and ordered or role-bearing edges.
It contains no privileged NL/PL words. A primitive or presently unresolved
meaning can retain an explicitly named grounding gap; it must not acquire a
fabricated local-language definition merely to close the graph.

### Surface mapping

```text
SurfaceMapping {
  meaning: NeutralSymbolID
  plane: NL | PL
  language: LanguageOrCarrierID
  surface: exact word | character | symbol | phrase | meme | code form
  context: ContextID
  direction: parse | render | both
  applicability: qualifier graph
  frequency: corpus-scoped observation
  provenance: Evidence[]
  state: candidate | witnessed | lapsed
}
```

Mappings are many-to-many. One surface may select multiple meanings; one
meaning may have multiple surfaces. Dialect, register, programming-language
version, domain, and context qualify a mapping rather than changing the neutral
meaning's identity.

Frequency belongs to an observed surface/context/corpus mapping. It does not
define the meaning or silently rank one language above another.

### Surface-composition edge

A multi-character word or multi-word meme remains a first-class surface.
`composed-of` edges preserve its visible constituents, but the phrase's mapped
meaning is not inferred by merely adding the constituents' meanings.

Within a selected context, a common phrase/meme occurrence can map atomically
to one intended meaning. Other contexts may map the same spelling to another
meaning; the surface is therefore not declared globally monosemous.

### Directed graph

Meaning recipes and surface mappings are directed edges with inverse indexes.
The graph supports:

- meaning → recipe symbols;
- recipe symbol → meanings that use it;
- meaning → NL/PL surfaces;
- NL/PL surface + context → candidate meanings;
- phrase → visible components;
- component → phrases containing it.

The embedding is derived from witnessed graph structure and observations. The
numeric coordinate is not the meaning's identity, because coordinates may be
retrained while the symbol and its evidence lineage remain continuous.

## Three illustrative cells

The annotations after `;` are only review labels. The actual recipe is the
neutral-symbol structure on the left.

### 1. Meaning with several local words

```text
⟐100                                             ; boundary sense
recipe:
  (⟐010                                          ; relation/event frame
    (⟐011 ⟐101)                                  ; role: interior
    (⟐012 ⟐102)                                  ; role: exterior
    (⟐013 ⟐103)                                  ; role: distinction
    (⟐014 ⟐104))                                 ; role: meeting/interface

candidate surface mappings:
  Chinese:  界, 边界
  Japanese: 境界
  English:  boundary
  PL:       Boundary, boundary(...), separates(inside, outside)
```

The mapping rows must specify which senses and contexts each surface actually
covers. The example does not claim these words are interchangeable everywhere.

### 2. Common phrase/meme as an atomic surface

```text
⟐200                                             ; begin contact that lowers social tension
recipe:
  (⟐020
    (⟐021 ⟐201)                                  ; begin
    (⟐022 ⟐202)                                  ; social contact
    (⟐023 ⟐203)                                  ; tension
    (⟐024 ⟐204))                                 ; decrease

candidate surface mappings:
  English meme:  break the ice
  Chinese phrase: 打开话题
  Japanese phrase: 場を和ませる
  PL projection: begin_low_tension_contact(...)

surface composition:
  break the ice --composed-of--> break, the, ice
```

The three NL phrases are review candidates, not asserted exact translations.
Each must be accepted, narrowed, or rejected independently with corpus and
context evidence.

### 3. Meaning without a required local word

```text
⟐300                                             ; no local label is required
recipe:
  (⟐030
    (⟐031 ⟐100)                                  ; boundary
    (⟐032 ⟐301)                                  ; preserve self/identity
    (⟐033 ⟐302)                                  ; permit selectively
    (⟐034 ⟐303)                                  ; exchange
    (⟐035 ⟐304))                                 ; support vitality
```

A renderer may generate a language-local description by recursively rendering
the mapped recipe symbols. A PL renderer may generate a type, predicate, or
executable pattern. A missing single-word mapping is ordinary information, not
a missing meaning and not permission to fabricate a word.

## Round-trip behavior

Parsing a surface produces contextual candidate meanings with evidence; it
does not erase ambiguity. Rendering a meaning chooses an applicable mapping or
composes a description from recursively mapped recipe symbols. Round-trip
quality is observed and graded; exact surface equality is not assumed.

Loss, ambiguity, missing mappings, unresolved primitive grounding, and cultural
non-equivalence remain visible values in the graph.

## Questions for independent review

1. Does this actually put form-neutral meaning before every NL and PL surface?
2. Is the recipe genuinely symbol-only, or does English leak into identity?
3. Are meaning identity, sense, surface, context, and embedding coordinates
   separated enough to support polysemy and lexical gaps?
4. Can Chinese characters, Japanese words, common memes, operators, types, and
   executable PL expressions be represented without treating them as the same
   kind of local object?
5. Where would recursive definition become circular, ungrounded, or falsely
   precise, and what is the smallest honest grounding mechanism?
6. Do trust, sovereignty, and vitality remain guidance, or has the architecture
   accidentally turned them into ontology rules?
7. What must change before the three examples can become executable seed cells?
