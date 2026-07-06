# Source Artifact Runtime Release Receipt -- 2026-07-05

## What changed

The checkout runtime now closes the source artifact gap for normal source
execution:

- `fkwu file.fk` and `fkwu --src file.fk` derive `file.fkb` and `file.sym`.
- `.fk` source can declare `.fk` dependencies through `import "path.fk"` in the
  direct source runner or `; import "path.fk"` where comment-safe files still
  need to pass through older sibling proof walkers. Legacy `; preludes:`
  remains compatibility input during migration. Dependencies are loaded
  recursively before the importing source, relative paths resolve from the
  importer with Form-root fallbacks, and duplicate/cyclic imports do not loop.
- Direct imports now load fresh version-3 `.fkb` program images when available.
  The import loader merges function roots, nodes, string-table rows, exported
  function names, and arities into the current compile image, then parses only
  the importing root source. If an import image is missing, stale, malformed, or
  old-format, the runtime first compiles that imported `.fk` into its own
  `.fkb/.sym` and then imports the image. The source-composition path remains
  the fallback when the import image cannot be made or trusted.
- A fresh `.dylib` is tried first when it exports the `fkwu_main_v1` ABI.
- If `file.fkb` is at least as fresh as `file.fk` and its embedded source
  identity matches the current source-unit identity, the runtime loads the
  program image and skips source reparsing. The source-unit identity includes
  the root file plus each imported `.fk` dependency's path, mtime, and size.
- If source is newer or the artifact is missing, `--src` compiles source and
  writes fresh `.fkb/.sym`.
- `./fkwu file.fkb` executes the program image directly.
- `./fkwu file.dylib` executes the native ABI directly when the symbol is
  present.
- The written `.fkb` carries the table payload plus the embedded symbol image:
  function roots, node rows, string table, symbol rows, and node dependency rows.
- `.sym` is a presentation lens; executable dependency truth is in `.fkb`.
  The `.sym` lens now also records the source-unit dependency closure for
  observation.
- `.tbl` execution is retired. `fkwu file.tbl` reports an error.
- Native `.dylib` emission is not installed yet; source compilation warns and
  emits `.fkb/.sym` when no fresh native artifact exists.

This is an installed runtime behavior, not only a modeled Form policy.

## Runtime proof

Build:

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
```

The compiler emitted the existing warnings about `fread` declaration and
`getsockname` pointer sign; no new fatal build issue.

Required checkout witnesses:

```text
./fkwu --src bootstrap/ground.fk                              -> 42
./fkwu --src bootstrap/ground-recursive.fk 10                 -> 55
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk  -> 15
./fkwu --src /tmp/nvr.fk                                      -> 11111
```

Fresh-cache and direct-artifact proof:

```text
first=42
fkb_direct=42
fresh_without_source_read=42
identity_mismatch_recompile=999
new_fkb_direct=999
fresh_recompiled_without_source_read=999
artifacts=cache.fk cache.fkb cache.sym
```

Plain `.fk`, direct `.fkb`, and native ABI selection:

```text
./fkwu bootstrap/ground.fk                -> 42
./fkwu bootstrap/ground-recursive.fk 10   -> 55
./fkwu bootstrap/ground.fkb               -> 42
./fkwu bootstrap/ground-recursive.fkb 10  -> 55
./fkwu add2.dylib 40                      -> 42
./fkwu native.fk                          -> 777  (fresh fkwu_main_v1 dylib wins)
```

`.fk` dependency proof:

```text
main.fk:
  ; preludes: lib.fk
  (inc2 40)

lib.fk:
  ; preludes: base.fk
  (defn inc2 (x) (add (base x) 1))

base.fk:
  (defn base (x) (add x 1))

./fkwu main.fk  -> 42
./fkwu main.fkb -> 42

after editing base.fk so base adds 2:
./fkwu main.fk  -> stale main.fkb ignored, rebuilt, 43
./fkwu main.fkb -> 43
```

The emitted `main.sym` recorded:

```text
source-hash fk-unit-v1|...main.fk@...|...lib.fk@...|...base.fk@...
dependency 0 ... path .../main.fk
dependency 1 ... path .../lib.fk
dependency 2 ... path .../base.fk
```

Import-image proof:

```text
./fkwu lib.fk                         -> emits lib.fkb
FK_IMPORT_TRACE=1 ./fkwu main.fk      -> trace: lib.fkb loaded import .fkb; 42
./fkwu main.fkb                       -> 42
```

Auto-materialized import proof:

```text
lib-fkb-before=absent
FK_IMPORT_TRACE=1 ./fkwu main.fk      -> trace: lib.fkb loaded import .fkb; 42
lib-artifacts=present                 -> lib.fkb and lib.sym were emitted
FK_IMPORT_TRACE=1 ./fkwu main2.fk     -> trace: lib.fkb loaded import .fkb; 43
./fkwu main.fkb                       -> 42
./fkwu main2.fkb                      -> 43
```

Proper import declaration proof:

```text
main-bare.fk:
  import "lib.fk"
  (inc1 41)

main-comment.fk:
  ; import "lib.fk"
  (inc1 41)

main-prelude-comma.fk:
  ; preludes: lib.fk,
  (inc1 41)

FK_IMPORT_TRACE=1 ./fkwu main-bare.fk          -> trace: lib.fkb loaded import .fkb; 42
./fkwu main-bare.fkb                           -> 42
FK_IMPORT_TRACE=1 ./fkwu main-comment.fk       -> trace: lib.fkb loaded import .fkb; 42
FK_IMPORT_TRACE=1 ./fkwu main-prelude-comma.fk -> trace: lib.fkb loaded import .fkb; 42
```

Repo recursive import fixture:

```text
form/form-stdlib/import-statement-runtime.fk:
  ; import "import-statement-lib.fk"

form/form-stdlib/tests/import-statement-runtime-band.fk:
  ; import "form-stdlib/import-statement-runtime.fk"

FK_IMPORT_TRACE=1 ./fkwu --src form/form-stdlib/tests/import-statement-runtime-band.fk -> 42
./fkwu form/form-stdlib/tests/import-statement-runtime-band.fkb                        -> 42
cd form && ./validate.sh form-stdlib/tests/import-statement-runtime-band.fk            -> 42
```

Arity/string remap proof:

```text
lib.fk exports:
  (defn add3 (a b c) ...)
  (defn word () "hello")

FK_IMPORT_TRACE=1 ./fkwu main.fk      -> trace: lib.fkb loaded import .fkb; 42
./fkwu main.fkb                       -> 42
```

Retired table input:

```text
./fkwu proof/four-way-run.tbl -> error: .tbl execution has been retired
```

Interpretation:

- first run compiled source and wrote `.fkb/.sym`;
- direct `.fkb` execution returned the same value;
- removing source read permission still returned `42`, proving a matching fresh
  `.fkb` wins without opening source text;
- source text changed to `999` with an artificially old mtime returned `999`,
  proving the embedded source identity check rejected the stale image and
  recompiled;
- dependency text changed from `base + 1` to `base + 2` returned `43`, proving
  the source-unit freshness check rejects stale root images when imported `.fk`
  files change;
- a precompiled direct import traced `loaded import .fkb`, proving the importer
  used the dependency program image instead of reparsing that dependency source;
- a direct import with no prior `.fkb` emitted `lib.fkb/lib.sym` during import,
  and a second root loaded that generated image, proving import materializes the
  dependency artifact for future runs;
- bare `import "lib.fk"` is stripped from the root source before Form parsing,
  while comment-safe `; import "lib.fk"` stays compatible with sibling walkers;
- the validator now expands comment-safe import declarations recursively, so a
  band can import a file that imports another file and still preserve the
  four-way proof path;
- direct `.fkb` execution after recompile returned `999`;
- removing source read permission after recompile still returned `999`, proving
  the new matching image wins;
- no `.tbl` was produced or required;
- `.tbl` is now rejected as an input.

Binary header check:

```text
00000000: 464b 5049 4642 3100 0000 0002 0000 0013  FKPIFB1.........
```

Direct argument artifact check:

```text
./fkwu bootstrap/ground-recursive.fkb 10 -> 55
```

## Form-layer witnesses

Focused release-path bands:

```text
cd form && ./validate.sh form-stdlib/tests/source-compiler-health-band.fk              -> 65535
cd form && ./validate.sh form-stdlib/tests/source-compiler-fkb-file-emission-band.fk   -> 2147483647
cd form && ./validate.sh form-stdlib/tests/program-image-fkb-byte-decode-band.fk       -> 536870911
cd form && ./validate.sh form-stdlib/tests/runtime-program-image-fkb-micro-walker-band.fk -> 16777215
cd form && ./validate.sh form-stdlib/tests/runtime-program-image-fkb-symbol-walk-band.fk  -> 268435455
cd form && ./validate.sh form-stdlib/tests/source-runner-admission-band.fk             -> 2097151
cd form && ./validate.sh form-stdlib/tests/source-compiler-grammar-bridge-band.fk      -> 32767
cd form && ./validate.sh form-stdlib/tests/import-statement-runtime-band.fk            -> 42
```

The table-text band is intentionally not part of this runtime-release proof
anymore because `.tbl` is retired as an executable input.

## Guide

The C file changed because `fkwu` is still the current checkout witness and the
front door being fixed lives there today. This pass did not add new language
semantics to the C parser. It installed artifact IO/selection for the existing
program-image format so the runtime can stop depending on standalone `.tbl`.

The shrink direction remains: keep `.fk` as source authority, keep `.fkb` as
program-image authority, keep `.sym` as lens, use `.dylib` only when fresh and
callable, retire `.tbl`, and move the artifact door into the native body as the
C seed recedes.
