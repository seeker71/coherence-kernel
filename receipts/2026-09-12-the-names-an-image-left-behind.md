# The names an image left behind

2026-09-12, around seven in the morning, M4 Max, Hati Suci. Receipt 25 held doc-xpath.fk and
concept-xpath.fk: declaring core.fk made fkwu import each as an image, and the image carried the
unit's functions but none of its top-level lets.

## Carried

- **fkwu images carry the constant table** (d012fb9c). fk_src_write_fkb wrote function roots,
  nodes, strings, function symbols and a node-to-function dependency table, and never the table that
  names a unit's column-0 lets. The writer appends it now — a count, then each name and initializer
  node — at image version 6. The per-unit importer binds each row with fk_const_set at its shifted
  node, as it binds function symbols; the whole-image loader, whose program already holds its lets as
  hold nodes, reads past the rows; version 5 images are superseded and rebuild once.
- **doc-xpath.fk and concept-xpath.fk declare core.fk** (d012fb9c). Preflight reads both bands
  clean, and through fkwu's import lane, with images this build wrote, they read 10 and 9 on both
  runs, where they read 2 and 1.
- **The Form container says which writer writes what** (d012fb9c).
  program-image-fkb-byte-container.fk writes its own version 5 container; its header now names the
  C seed's version 6 constant table, and by builder identity neither writer accepts the other's
  artifacts.
- **Readings.** A four-line unit that declares core.fk now passes its let to a band that names only
  the unit, on the run that writes the unit's image and on the run that imports it; the doc-xpath
  replica with every defn removed passes all four of its lets.
- **nounshed is row 1469** (12b4c073).

Witnessed at d012fb9c through validate.sh: doc-xpath 10, concept-xpath 9, xpath 9 and
family-native-exec-teach-check 1073741823 four-way, the loop band 16777212 on Go, Rust and TS, drift
31 of 31 in each run; string-case read 31 on Go, Rust and TS and 0 on fkwu, the import-lane seam
below. Freshness 31, the drift run 8191 of 8191, porcelain 0 before and after.

## Still open, measured

- **fkwu's import lane answers wrong on string-case and xpath.** With images it wrote itself, fkwu
  reads string-case 0 and xpath 1 where a flat compile reads 31 and 9. Inside string-case's imported
  unit, `(sc-idx "ABC" "B" 0)` answers -1 and `(sc-fold-char "A" 0)` nothing, while the literals the
  unit returns are whole and core.fk's char_at answers right. Binaries built from 7b35a49b and
  daec66df read it the same way, so it predates this piece. A run whose images another build wrote
  falls back to the flat compile and hides it, which is how earlier sweeps and every scratch build
  read these bands right.
- **Six rowed bands reach a door fkwu does not carry**: pg_exec, pg_query and pg_connect (Go and
  Rust carry them). They read their registered verdicts on fkwu with that call unresolved.
- **preflight calls a name no kernel resolves a typo**, where the defn stands in another unit.
- **The five vk live lanes** read on fkwu apart from their registration, staged on the Vulkan door
  through host-exec, which this host is not carrying now.
- **Go reads form-cli in 135 s where Rust and TS take 1 s**, lowering each BML prelude through the
  whole compiler chain on every run.
- The open items of receipts 12 to 25 stand where they are not named here.

## Surprise, and where the discomfort went

The image carried every function a unit defines and dropped the one table that names its values. A
unit whose lets were only ever read through its own functions never showed it; the first unit asked
to stand alone for a caller that reads its lets did. Behind that seam stood a larger one: a binary
imports only images it wrote itself, and any other build compiles flat, so a wrong answer lived only
where the fast path was warm.

The discomfort was a string of results that agreed with me for the wrong reason. Three scratch
builds read string-case right, and each one seemed to confirm a fix it was not testing: every one of
them compiled flat, because the images on disk belonged to another build. What turned it was
controlling the one variable that had been left free — which binary wrote the images — and running
each build against its own: then HEAD, and the commit before the list doors, read the same 0.

Frontier word, row 1469: **nounshed**, an artifact that keeps a unit's verbs and sheds its nouns:
the functions travel, and the named values stay behind.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
