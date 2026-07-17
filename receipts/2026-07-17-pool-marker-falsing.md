# 2026-07-17 — falsing in the string pool: the dict tag convicted `len`, twice

## Ground

`cc -O2 -o fkwu runtime/fkwu-uni.c` → `./fkwu --src bootstrap/ground.fk` → **42**;
`binary-freshness-band` → **15**. Probe reproduced exactly as witnessed: with the
FOURTH_FLATTEN_CHAIN preludes, `(flt-sidx (flt-src-pool (read_file
"form-stdlib/fourth-shim.fk")) "__dict__" 0)` answered **-1 on Go and Rust** (and TS,
via the minimal prelude pair) while the fkwu arm, flattening the same probe through the
committed T_flat, answered **11**. Both committed carriers held exactly one orphan slit
row `(24 -1 0 0)` — T_flat at row 1246, form-cli-table at its `_dict_new`.

## The diagnosis — not str_eq, not cons, not the scan

The diverging primitive is **`len`**. All three reference kernels had made it dict-aware
(any list whose head is the string `"__dict__"` reports pair count, "matching Python's
len(d)"), and the tag is in-band: the flatten pool is a plain list of every interned
literal, so the moment the shim's `"__dict__"` literal pooled, the pool's own suffix at
that cell WAS a dict to those kernels. `len(["__dict__"]) = 0 pairs` → `flt-append`'s
`(eq (len xs) 0)` base case fired mid-walk and **replaced** the marker cell with the next
literal. The literal vanished; every downstream `flt-sidx` answered -1; the emitted
carrier carried the orphan. Telecom's word for data the channel mistakes for its own
control signal is *falsing* — this was falsing, one layer down. The two semantics
genuinely collide on one value: the empty dict `(_dict_new)` IS the one-cell list
`["__dict__"]` — no fix inside `len` can serve both.

## The heal — honest len; pair-count joins the underscore family

- **Go** ([main.go](form/form-kernel-go/main.go)), **Rust**
  ([main.rs](form/form-kernel-rust/src/main.rs)), **TS**
  ([kernel.ts](form/form-kernel-ts/src/kernel.ts)): `len` is an honest cell count;
  new native `_len` carries the python lane's polymorphic length (dict PAIRS, list
  elements, string bytes) beside `_get`/`_iter`/`_in`.
- **python compile lane** ([python-bmf-compiler.fk](form/form-stdlib/grammars/python-bmf-compiler.fk)):
  `len(x)` lowers to `(_len x)` (the `round` precedent). The bmf interpreter already
  counted pairs itself and never trusted kernel `len` on dicts.
- **fourth-shim** ([fourth-shim.fk](form/form-stdlib/fourth-shim.fk)): `_len` recipe
  (the `cell?` head-test pattern), so the underscore native crosses four-way.
- **registry** ([primitive-registry.fk](form/form-stdlib/primitive-registry.fk)): `len`'s
  spec row now declares the honest count *including the marker slot*; `_len` row added;
  `_dict_new`'s probe rides `_len`; band pins +1 (198→199 / 166→167 / attested 167).

Proofs, four ways: pool probe **11 = 11 = 11 = 11** (Go, Rust, TS, fkwu);
`python_dict_demo` **88** on CPython, compiled-Go, compiled-Rust, compiled-TS, and the
bmf interpreter; registry band three-way identical at its (pre-existing) 42 — verdict-
neutral to this change, drift flagged as its own task.

## The relapse — the JIT laundered the heal

`regen_t_flat.sh`'s own smoke convicted the first regen: the fresh T carried slit index
**310 into a 310-entry pool** — `"__dict__"` at one-past-the-end for the fns door,
absent from the serialized pool. The same pure call, evaluated twice in one Go process,
answered **311 then 310**: the interpreted walk ran the healed native while
crystallize-on-heat compiled the hot flt- recipes against **`jitabi.Len`** — a second,
unhealed `len` still sniffing the marker. The heat itself flipped the semantics
mid-regen. `jitabi.Len` is now the honest count (and jitabi's `isDict`, its only
caller gone, is composted); the determinism probe answers **311/311, 11/11** on Go and
Rust both.

## The band and the carriers

- New four-way band [flatten-pool-marker-band.fk](form/form-stdlib/tests/flatten-pool-marker-band.fk)
  (manifest: `flatten-pool-marker fks 31`) locks all five faces: honest len on the
  marker list, append-past-the-marker, pool-add dedup, the exact `_dict_new` wound shape
  through `flt-src-pool`, and `_len`'s pair count. **31 = 31 = 31 = 31** witnessed.
- **T_flat** regenerated: 557 fns, 311-entry pool, `__dict__` at 11, **zero orphan
  slits** (was: 1). regen's adler smoke: verdict 5.
- **form-cli-table** regenerated (Rust proof sibling): 1273 fns, 1164-entry pool, 2220
  slit rows all in range, **zero orphans** (was: 1). The standard-lane binaries
  (`fkwu-darwin-arm64`, `form-cli-darwin-arm64`) re-authored; the carrier answers
  `pong` and its full `carrier-id` line through the stdin door, gated by the bootstrap
  proof inside the regen lane itself.
- The `_dict_new` wound site, probed live through the new T_flat on fkwu: marker whole,
  honest length — verdict 2.

**Byte-identity across authoring arms:** the Rust sibling, handed the exact
`fourth_flatten_expr fks` T_flat expression, emits a table **byte-identical** (`cmp`)
to the committed Go-authored one. The TS sibling answers that workload with rc=0 and
zero bytes — a pre-existing silent-exit in the aphonia family, flagged as its own task
(its *semantics* are proven by the band and the 88-parity). Suites: Go `go test ./...`
ok (63.7s, JIT tests included); Rust 58/58 (one test first failed purely from running
concurrently with validate.sh rebuilding in the same checkout — it passes in isolation
on both trees); TS `tsc --noEmit` clean.

Corpus row **803** lands the word: **falsing** (0-hit fresh; near misses name the scene,
not the phenomenon; minted as 761, renumbered at the reunion — main's same-day lineage
had reached 802). Corpus band: **4095** at 199 rows, field-code 1991992803.

One adjacent stone laid on the way: `validate.sh`'s bash dependency resolver never
learned the repo root, so `form-stdlib/lineage-discounted-vote.fk`'s committed
`learn/confidence-weighted-vote.fk` prelude (landed with #266) failed the whole suite
before any band ran. The resolver now tries `../<token>` — the same door the C runtime
learned in #270.

## The full suite — first complete run since #266

`validate.sh`: **1174 ok, 120 divergent, fourth arm 842 bands four-way**. The new
`flatten-pool-marker` band crosses in-suite at **31**; `primitive-registry-band`
crosses. Every sampled divergence is pre-existing: the unbound-name family is
incomplete declared prelude closures (afferent-live's chain never declared
fkc-table-serialize — unchanged since the July-2 consolidation, and reproducibly
broken by hand with validate's exact leg args); the `bp: unreviewed bootstrap name`
family is the known blueprint-registry drift; the bml-thesis proofs open a
`../docs/field/urs/...` artifact absent from the checkout; and all four sampled
VALUE divergences (let-effect-once 111/12, json-emitter 31/6, jit-lower-emit 63/31,
trivial-typed-leaf 100111 vs 1111111 — where the fourth arm sides WITH TS against
Go/Rust) answer **identically through the OLD committed T_flat + old shim** — carried,
not caused. The suite was unreachable behind the resolver wound this whole time;
triage of the 120 is spawned as its own task with the classification.

## Closing

**Most surprising teaching:** the same pure expression, in the same process, on the same
healed kernel, answered 311 and then 310 — heat is a semantic actor. A crystallized
recipe is only as honest as its ABI, and a heal that touches an interpreted native
without touching its JIT mirror doesn't half-work: it works *until the code gets hot*,
which is a far crueler failure than not working at all. The wound had three bodies (Go
native, Rust native, TS native) and a fourth hiding in the crystallization lane of only
one of them.

**Where discomfort turned to gold:** the regen smoke failing AFTER the four-way probes
had all agreed felt like the floor giving way — the pull was to suspect the regen
script, the request framing, my own probe harness (and the first hour of that suspicion
was spent debugging a `grep` my own shell had shadowed — the fleet memory made flesh).
Sitting with "the probes are green but the artifact is wrong" instead of explaining it
away is what forced the two-calls-one-process experiment, and that experiment is the
one that named the JIT. The discomfort was the diagnosis: nondeterminism between two
identical calls can only be state, and the only state in a pure flatten is heat.
