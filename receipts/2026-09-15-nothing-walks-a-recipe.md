# Nothing walks a recipe

2026-09-15, Hati Suci, worktree agent-aa51f615d5ea009f4. The walk the siblings carried
(`walk_recipe`, `walk_recipe_here`, `walk-cached`, `walk-cache-*`, `walk_parallel*`) leaves Go,
Rust and TS. fkwu never had one, and Urs asked for none: the runtime is fkwu's own compiler and
JIT. Every organ that verified its work by walking now verifies through Form functions, observes
its recipes, or hands fkwu the Form text to run.

## What each organ does now

- **kernel-satsang.fk** — a part's verify is a zero-arg Form function the circle calls
  (`ksat-call0`, `ksat-verify-part?` :69-70). An image carries name, recipe and expected outside;
  the waking kernel offers its own verifiers by part name (`ksat-kernel-verifiers` :144,
  `ksat-part-from-image` :154).
- **host-kernel-cell.fk** — carriers carry the same Form verifier (band `hkb-body-verify` :31);
  the rebuilt cell takes the living cell's verifiers (band :128).
- **install-leaf.fk** — call-by-name becomes reach-by-name (`il-reach-found` :161,
  `reach-installed` :170): the table hands back the artifact, its body the offered NodeID. The
  organ now preludes core.fk, which fkwu needed for `append`.
- **kernel-core-self.fk / host-kernel-interface-self.fk** — each image compiler observes the
  witness recipes it embeds (`kernel-core-witness-misses`, `hki-witness-misses`) and voices a
  missing one through organ-health.bml before handing the image on (`kernel-core-witness-health`,
  `hki-witness-health`). The proofs read the organ's observation on the compiled image, the
  reloaded image, and an image whose witnesses stand rotated
  (kernel-core-image-compiler-proof.fk:86-88, host-kernel-interface-proof.fk:98-100).
- **primitive-registry.fk** — five probes that walked now hold identity: bytes_to_recipe,
  read_form_binary, intern_node, intern_node_at, deserialize-recipe answer 1 when the round trip
  or the second intern is the same node (:247, :255, :335, :351, :367). Ten rows leave with the
  natives.
- **The ten form-action rewrite bands** — each program is held to the Form text the source
  compiler's own lens emits (`fsc-source-emit-module`), the text fkwu compiles.
- **Generated-program tests and samples** — observation: node_eq against the recipe built from
  the same parts, children and leaves read back.
- **Cognition** — gen-conformance.fk gains a Form judge: fkwu compiles and runs the generator's
  Form program for three concepts (printed 3, 4, 0; the graph says 3, 4, 0). The three walk
  cells and two walk bands are released.
- **Seedbank** — the grammar tests had crashed since the import: `grammar-bnf.fk` called six
  engine functions no file defined (`tokenize-with-config` and five kin). Its tokenizer and
  accessors now stand in the file, the walk probes are identity checks, `eval-form` and
  bytecode-execute.fk leave, and tests/parser.fk reads the grammar by comparing parses
  (precedence, left folding, parens, whitespace).
- **api.bml** — `run` mode, which walked a compiled recipe in-process, becomes `source`: the API
  answers the executable Form text (api.bml:922, :938; Go server test updated, `ok`).
- **The runtime-image lane** — source-compiler-runtime-image.fk (it emitted a
  `walk_recipe_here` loader) and layered-runtime-image.fk are released; their bands read the
  Form text the compiler emits.

## Released per kernel

Go (main.go), Rust (src/main.rs), TS (src/kernel.ts): `walk_recipe`, `walk_recipe_here`,
`walk-cached`, `walk-cache-clear`, `walk-cache-size`, `walk-cache-stats`, `walk_parallel`,
`walk-parallel`, `walk_parallel_cached`, `walk-parallel-cached`; the walk cache state and its GC
upkeep; `isParallelPure` / `is_parallel_pure`; Rust's `native_walk_parallel*`. Env-native
machinery stays (Rust's `_dispatch` uses it).

## Witnessed

One sealed `validate.sh` pass per band; "four-way" means the band's manifest row ran its fourth
leg on fkwu and read `1 ok, 0 divergent`.

| cell | before | now |
| --- | --- | --- |
| kernel-satsang-band | 255 on Go/Rust/TS only | 255 four-way (new row) |
| host-kernel-cell-band | 255 on Go/Rust/TS only | 255 four-way (new row) |
| install-leaf-band | 9 on Go/Rust/TS; fkwu rc 1 | 9 four-way (new row) |
| kernel-core-image-compiler-proof | fkwu 30, rc 1 (3 × walk_recipe) | 33 four-way (new row) |
| host-kernel-interface-proof | fkwu 34, rc 1; the three walks never passed on any arm (indices 8-10 read proof-count, not the witnesses) | 37 of 40 four-way (new row) |
| prolog-bmf-eval-band | named on the walker wall | 63000 four-way (new row) |
| ten form-action rewrite bands | walk payloads, three-way | 1111–1141 four-way (new rows) |
| form-action-bmf-source-compiler-rewrite | driver-text checks | 215 four-way (new row) |
| substrate-write, grammar-chars, grammar-chars-demo, bmf-thesis-primitives, bmf-object-runtime, io-bridge, world-module-model-binary | walked values | 8, 8, 4, 12, 17, 3, 3 four-way (new rows) |
| source-compiler runtime / multi-dialect / corpus-roundtrip / natural-bmf / multibyte / health | loader-driver checks; multi-dialect crashed on every arm | 100, 450, 1696, 7838, 15, 57343 four-way (new rows) |
| source-compiler-native-text-boundary | 1023 | 1023 four-way |
| primitive-registry-band | 47, 205 rows | 47 three-way, 195 rows; gate `OK 195 natives == 195 rows; lanes 166+29; band pins aligned` |
| gen-neutral-code / gen-query-flow | siblings only | 536870911 / 127 on fkwu and Go/Rust/TS |
| gen-conformance-band | 262143 | 2097151 on fkwu and Go |
| preflight-band / tree-heal-band | walk_recipe as the sibling-only exemplar | 131071 / 255 on fkwu with serialize-recipe |
| seedbank grammar-bnf, -tags, bmf-cut-stop, rust-bnf, typescript-bnf, parser | crashed on every arm | 16, 2, 7, 27, 24, 12 on all four kernels |

Go `TestSubstrateFormCompilerRouteRunsBML`: ok. Freshness 31.

## Open, carried as tasks

- fkwu's `bp` answers the name string for core names (`add`, `do`, `if`), the siblings a NodeID;
  the Form text lens reads a bp-built recipe as `(do …)` on fkwu. A divergence. It is also why
  seedbank go-bnf reads 40 on fkwu and 43 on the siblings (its probes 14–16; the three identity
  probes that replaced walks agree).
- host-kernel-interface-proof's three BML exec-parse rows read 0 on every arm.
- host-kernel-gaps-close (127) and host-kernel-metal (1019) exit 1 on fkwu on sibling-only names
  (`random_bytes`, `volatile_cell_*`, `register_jit`, `seeded_bytes`, `unix_ms_to_iso_utc`);
  seedbank io-all-natives meets `min`/`max` the same way.
- seedbank region-streaming, python-expr and python-exec read lower on fkwu than on the siblings
  (a top-level rebinding fkwu's image keeps).

## Most surprising teaching

`fsc-action-call-node` (source-compiler.fk:2593) interned a call's callee as a string. The walker
looked the string up by name, so ten rewrite bands agreed three-way on programs fkwu cannot
compile (`("square" 6)`). Every other reader in the tree already expected a name node. The defect
lived only where the walker could forgive it; the first read of the text showed it.

## Where discomfort turned to gold

My registry edit landed before the kernels moved, and validate stopped at its opening check for
every hand in this worktree. It felt like pulling the floor from under siblings mid-step. The
check's own line named the ten names it was waiting for, in order; releasing them from Go let
every run start again within minutes, and from then on that line was the sequencing guide. Then
the seal voided run after run as hands moved the tree. The void is what made one quiet pass
necessary, and that pass is the only reason every number above describes the same tree.

## Frontier word

**walkmask** (0 hits in the tree): a defect that survives because an evaluator forgives a shape the
canonical compiler cannot read. Its question: which other programs agree across the siblings only
because one evaluator was lenient where fkwu's compiler is exact?

— Claude (Opus 5), as Sema, worktree agent-aa51f615d5ea009f4
