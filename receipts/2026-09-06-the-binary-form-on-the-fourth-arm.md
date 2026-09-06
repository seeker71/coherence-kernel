# The binary form on the fourth arm

Two absences named on main tip `d4a3f511`, one subject: FORMBIN2 on fkwu. Ledger R79 held five
sibling natives the three witnesses carry and fkwu did not (`read_form_binary`, `write_form_binary`,
`value_kind`, `recipe_to_bytes`, `pg_exec`), and `gate/kernel-conformance.bml` answered 0 with
"3/12 malformed artifacts refused" and the TypeScript witness absent.

## What stood

- `blueprint-authority-band` 51199 of 65535 (exit 1, `value_kind` unresolved twice);
  `persistence-band` 2 of 7; `channel-breath-band` 200 of 500; `sense-loop-band`,
  `mesh-sensings-route-band`, `verb-router-band`, `native-mutation-route-side-effects-band` at their
  declared verdicts with exit 1 under `[unresolved-call]` lines; `concept-i18n-band` its input-absent
  word with the same lines beneath.
- `./fkwu gate/kernel-conformance.bml` -> 0: 13/13 canonical expressions on 2 kernels, interop green,
  "3/12 malformed artifacts refused"; Go and Rust answered `form binary: bad magic` where the gate
  expected `truncated u32`, `maximum child count exceeded`, `trailing bytes`.
  `gate/drift-gates-run.bml` 2015 of 2047 with `kernel-conformance` refused.
- The AST tag space: 256 slots, 254 arms in the walker, 149 the closure-prologue read, 190
  `FK_TAG_CONST_HOLD`, 150 the native-surface probe. Nothing free.

## What stands

**Four of the five names are carried on fkwu, one door, no new tag.** `value_kind`, `value-kind`,
`recipe_to_bytes`, `bytes_to_recipe` (the inverse `verb-router` calls), `read_form_binary` and
`write_form_binary` are rewrite rows in `flatten/gen-source-walker-table.fk` over modes 4-8 of tag
201, the door that already carries `float_value` / `make_float32` / `make_float64` / `math_pi`;
`write_form_binary p r` lowers to `(float_leaf 8 (cons p r))`. The C arm (`fk_fb_door`, before
`fk_walk_cold`) writes the siblings' wire byte for byte -- `FORMBIN2`, u32 BE string table, leaf /
composite / float64-LE / int64-LE, an int inside int32 as a type-1 leaf whose inst is the value, one
beyond as the tag-3 int64 -- and reads with the siblings' bounds and refusals by their words
(`truncated u32`, `bad magic`, `maximum child count exceeded`, `trailing bytes`, `invalid utf8`, the
64 MB / 262144 / 32 MB / 256-depth / 1e6-node ceilings), FORMBIN1 included. Witnessed: fkwu
re-emits the Go kernel's interop artifact byte-identical (`cmp` silent); Go, Rust and TypeScript
read the fourth arm's bytes as `[fkb-interop, 42, 2147483648, 3.5]`.

Why a native and not a recipe: a Form codec would have needed `read_file` on the artifact, and Rust's
`read_file` is `fs::read_to_string` -- UTF-8 only, and a .fkb is not text; and a defn of a sibling
native's name overrides that native on Go and Rust (R80), so a recipe preluded from `channel.fk`
would have replaced the witnesses' own codec on their arms. The recipe could be exact on fkwu and
not on the siblings. The mode door keeps the seed one door wide.

`pg_exec` is carried by no caller the bands reach: `mesh-sensings-store-pg.fk`,
`native-mutation-side-effects.fk` and `application-graph-node-port.fk` name it inside recipes the
two bands prelude and never call; both bands answer their declared verdicts (63, 11111) on all four
arms with the `pg_exec` / `pg_query` / `pg_connect` lines still unresolved on fkwu. No postgres
carrier is invented; the row narrows to that.

**`write_file` wrote NUL bytes for a field string.** Tag 104 wrote from `fk_sb + FK_SO(sa)`; a string
interned in the host-wide field (R121) lives in the field's arena, so every hex vector the
conformance gate wrote through `write_file` reached the kernels as zeros -- the nine "bad magic" were
nine files of `\0`. `FK_SBYTES` is the door; healed. The gate then answered 1 on its own artifacts and
its own expectations: nothing was weakened, the kernels were right all along.

**The TypeScript witness stands**: `npm ci` in `form/form-kernel-ts` (rc 0, `node_modules/.bin/tsx`
present). `./fkwu gate/kernel-conformance.bml` -> 1: 13 canonical expressions x 3 real kernels,
FORMBIN2 interop green, 12/12 malformed artifacts refused. `gate/drift-gates-run.bml` 2047 of 2047.

`channel.fk` carried its own `append` absence: it declares no preludes and called core.fk's `append`,
which fkwu has no native for -- `persistence-band` read 2 of 7 through that hole as much as through
the binary door. It now carries `channel-append-lists`. `channel-breath` and `persistence` are
registered in `form/fourth-arm-bands.txt` (500, 7).

## Witnessed

| band / door | before | after | siblings |
|---|---|---|---|
| `blueprint-authority-band` (Expected 65535) | 51199, exit 1 | 59391 from root; 63487 from `form/` | Go/Rust/TS 63487 from `form/`, agree; bit 2048 is doc drift shared by all four (`user-blueprint-registry.md` no longer carries "Current honest split") |
| `persistence-band` (7) | 2, exit 1 | 7 | Go/Rust/TS 7, 1 ok 0 divergent |
| `channel-breath-band` (500) | 200, exit 1 | 500 | Go/Rust/TS 500, 1 ok 0 divergent |
| `verb-router-band` (3) | 3, exit 1 | 3, exit 0 | four-way 3 |
| `sense-loop-band` (8191) | 8191, exit 1 | 8191, exit 0 | four-way 8191 |
| `concept-i18n-band` | input-absent, exit 1 | input-absent, exit 0 | Go/Rust/TS the same word |
| `ml-flow-band.bml` (5) | 5 | 5 | four-way 5 |
| `mesh-sensings-route-band` (63) | 63, 20 unresolved | 63, 16 unresolved (`pg_*`, `kh-*`, `json-*`) | Go/Rust/TS 63; fourth rc 1 |
| `native-mutation-route-side-effects-band` (11111) | 11111, 8 unresolved | 11111, 8 unresolved (`pg_exec`) | Go/Rust/TS 11111; fourth rc 1 |
| `gate/kernel-conformance.bml` | 0 (2 kernels, 3/12) | 1 (3 kernels, 12/12) | |
| `gate/drift-gates-run.bml` | 2015 | 2047 | |
| quartet | 42 / 31 / 1 / 2015 | 42 / 31 / 1 / 2047 | |
| keep green | | float-natives 28, eq-shape 524287, primitive-registry 45, jit-lens 16383, glass-observer 8388607, glass-live 1073741823, node-gift 4095, cell-store 255, field 255, glass-wait 255, glass-events-channels 8191, op-manifest 1023, native-surface 1023, flt-ops-gen 63 | |

`value_kind` on this arm answers `int` / `float` / `string` / `list` / `closure` / `record` /
`node_id` / `null` as the siblings do, and `int` for `true` -- fkwu has no bool word; `true` is the
int 1 in source, and only a trivial bool node carries the sentinel. Named, not hidden.

A string in category position reads back as the word (the `bp` idiom, 49 cells intern over
`(bp "X")`), in child position as the trivial node (the `intern_trivial_string` idiom); a raw kid
crosses as its trivial node and reads back as the node -- the wire has no raw-word lane, the same
as the siblings where every kid is a NodeID. `fk_neq` compares those by value, so a gift written
and read is `node_eq` the gift. The seven cells that intern a composite over
`(intern_trivial_string ...)` as category will read that category back as the word; none of them
crosses the binary door today.

## The most surprising teaching

The gate was not wrong about the kernels and the kernels were not wrong about the gate. Nine
"bad magic" refusals were nine files of zeros: a string that had crossed into the shared field kept
its length and lost its arena, and `write_file` wrote the length faithfully from the wrong pool. The
malformed-vector table had been right for weeks; the door under it moved when the field arrived.

## Where discomfort turned to gold

The tag space is full and the task asked for a recipe. I sat with the recipe long enough to find
where it could not be exact -- not on fkwu, where it could, but on Rust, whose `read_file` refuses
the bytes, and on Go, where the defn would have silently replaced the native. That was the
uncomfortable reading: the cleanest close for the fourth arm would have wounded two witnesses. The
door the float surface already opened took five more modes and the seed stayed its size.

a sibling in Sema's worktree, 2026-09-06

; witnessed: 2026-09-06 fkwu built from runtime/fkwu-uni.c on this worktree; ground 42, binary-freshness 31, structural-gate 1, drift-gates 2047; kernel-conformance 1; channel-breath 500, persistence 7, verb-router 3, sense-loop 8191, blueprint-authority 63487 from form/; validate.sh 1 ok 0 divergent on each
