# Scripture source corpus

This directory contains the source texts used by the native scripture ingestion
and re-emission witnesses. Source text is immutable evidence; generated model
state is written to ignored `.form-*` files and never overwrites these files.

## World English Bible

- Path: `sources/bible-web/`
- Edition: 66-book World English Bible protocanon, 2020 stable text edition
- Authority: [eBible.org](https://ebible.org/engwebp)
- Licence: public domain. The bundled `copr.htm` carries the upstream notice.
- Boundary: a changed or derived edition must not be called the World English
  Bible. This repository therefore stores interpretations as separate graph
  nodes and preserves the source bytes.

## SARIT Sanskrit texts

- Path: `sources/sanskrit-sarit/`
- Works currently present: Aṣṭāvakragītā, Pātañjalayogaśāstra, and the
  Devanāgarī Mahābhārata.
- Authority: [SARIT — Search and Retrieval of Indic Texts](http://sarit.indology.info)
- Licence: Creative Commons Attribution-ShareAlike 3.0 Unported, as declared in
  each TEI header. Attribution and source descriptions remain embedded in the
  XML files.
- Boundary: derived interpretations are separate model nodes. They do not
  replace the Sanskrit text or claim endorsement by SARIT or its contributors.

## Coverage semantics

“Work coverage” means that a locally present work was admitted and evaluated by
the named Form cell. It does not mean that every semantic distinction in the
work is understood. Frequency counts are witnessed lexical observations;
semantic promotion requires additional context, morphology, lineage, and human
witness.

