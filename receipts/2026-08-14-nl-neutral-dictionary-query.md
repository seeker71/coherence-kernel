# 2026-08-14 — query the form-neutral dictionary both ways

Urs asked to extend the core NL-neutral dictionary to all words and all
symbols from all languages, with descriptions, Form recipes, and queries
for the relations among symbols, descriptions, tokens, and words.

## North star and floor

All words, all symbols, all languages is the north star. This cell is the
door, not the dump. New tongues are new columns. New words and symbols
enter as admitted rows; the old dictionary stays referenced (axiom-3).

Today's seed: the closed 64 in 14 tongues, four Form symbols (`( ) + =`),
and three overlay words (`trust`, `cell`, `recipe`).

## What you can ask

- surface + tongue → symbol
- two surfaces → same symbol? (`nothing` / `kosong` / `nada`)
- one surface → many symbols? (`hakuna` is a homonym: nothing and none)
- symbol → description (purified how-tokens)
- symbol → Form recipe, and the recipe runs (`and` → 1, `+` → add)
- word → character tokens; token → the words that hold it
- admit a new row; the seed is unchanged
- unknown tongue → nothing, not an error

## Witness

```
nl-neutral-dictionary-query-band   8191
core-dictionary-neutral-field      16383
core-lexicon                       262143
```

## Reproduce

```sh
./fkwu form/form-stdlib/tests/nl-neutral-dictionary-query-band.fk
./fkwu form/form-stdlib/nl-neutral-dictionary-query-live.fk
```
