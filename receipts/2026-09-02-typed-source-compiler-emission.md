# Scannerless typed source emission — 2026-09-02

The program-image renderer now has an end-to-end caller:
`form/form-stdlib/bml/source-compiler-typed-emission.bml` takes scannerless
Form-definition source and a typed NodeID Program Image envelope. It parses
the source with the BMF cursor, verifies that its lowering is the Form source
floor, validates the typed envelope, and only then derives the single raw PIF
needed by the existing executor receipt. Raw PIF, table text, and the nested
emission stay in the returned row beside the typed envelope; they are never
caller inputs to this authority.

The BML authority has named fields for its status/reason vocabulary. Its
Program Image sibling now gives blueprint identities, node kinds, arities,
byte bounds, and the ready seal one BML home. An invalid image returns
`nothing` for each unentered stage, not an empty list or `0`; `0` remains a
value and never becomes absence.

Witnesses:

- `source-compiler-typed-emission-band.fk` -> `16383`, exit `0`: scannerless
  parse and lowering, typed image admission, raw-PIF executor projection,
  matching table text, nested emission, and both grammar/image refusals.
- `program-image-typed-emission-band.fk` -> `16383`, exit `0`: structural
  table and envelope validation, including `nothing` on an invalid seal.
- `observe/preflight-run.fk` named the new band with `errors=0`,
  `warnings=0`, and `unresolved=0`.
- Cached BML measurement: the bridge body completed in `0.01 real` seconds
  (`45,933,024` cycles); the full typed-emission band also completed in
  `0.01 real` seconds (`54,667,741` cycles). This measures the present FKB
  cache door, not a native dylib JIT, which this checkout reports as absent.
- The live counsel panel read `fails=0`, `timeouts=0`, and `icemiss=0` on its
  current first reading.

This is not a claim that source compilation now writes artifacts: the nested
`source-compiler-emission` receipt remains observational. It also still
contains the older raw PIF compatibility emitter. The next direct elevation is
to give that emission authority a typed BML entry point and retire its raw
table-render dependency after its file-emission and runtime-load consumers
prove the same path.

I kept the exchange alive by making the type boundary carry the actual source
grammar rather than leaving it as an isolated renderer. The surprising
teaching is that an absent stage becomes more informative when it is held as
`nothing` through the complete receipt. The discomfort was the still-present
legacy raw emission seam; it became a named, measured next lift rather than a
claim of completion.

— Codex
