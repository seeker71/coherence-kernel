# Receipt — the day's deepest root cause: a stale ./fkwu binary, not evaluator bugs (2026-07-01)

**One artifact explained most of a day's "evaluator constraints."** `./fkwu` is gitignored — a
local build product. This morning's rebase pulled ~116 upstream commits that moved
`runtime/fkwu-uni.c` far ahead (two-pass defn prescan, `true`/`false` literals, general-arity
calls, distinct trivial-bool interning). The local binary was never rebuilt after that merge —
and `ground.fk → 42` is an old enough contract that the stale binary kept passing the grounding
check all day. Every "constraint" then discovered through it was tested against a binary that
predated the source being read.

Found while chasing the "trivial-bool kernel bug" into the C dispatch: the tag-112 constructor
plainly used two distinct sentinels, the parser plainly mapped `true`→1 — the code could not
produce the observed behavior. An instrumented **fresh** build then behaved correctly, and the
diff wasn't the instrumentation: `-O0`, `-O1`, `-O2` fresh builds all agreed with each other and
disagreed with `./fkwu`. (One detour inside the detour: the first "instrumented binary run" was
actually a *stale leftover debug binary from this morning* executing after the instrumented
compile had silently failed — the same failure class, nested. Checking `gcc`'s exit rather than
grepping its warnings is what un-stuck it.)

## What was a stale-binary artifact (all re-verified against a fresh build, by direct repro)

- **"Forward references between top-level `defn`s never resolve."** They resolve fine — current
  source pre-scans all defns (name, index, arity) before parsing bodies.
- **"Mutual recursion between two `defn`s never resolves."** Resolves fine (`is-even`/`is-odd`
  repro returns 7/7 on a fresh build).
- **"`intern_trivial_bool true` and `false` construct the same node."** Distinct on a fresh
  build. The stale binary parsed `true` as an unbound symbol → 0, so both calls received 0.
- **arrival-band's 895-vs-1023 discrepancy.** 1023 on a fresh build — the band was never broken.
- **json.fk's tokenizer "forward-reference breakage."** Same artifact; the original json.fk's
  number parsing works on a fresh build.

## What survives — real constraints of CURRENT source, re-confirmed on a fresh build

- **Top-level `let` is invisible inside `defn` bodies** (root cause #1 of
  `2026-07-01-json-fk-src-scoping-fix.md`). Still true. The zero-arg-`defn` pattern for
  Blueprint constants and `core.fk`'s `intern_node_at` addition both remain necessary and
  correct (`intern_node_at` re-confirmed absent from the native surface).
- **Bare top-level `(do (let ...))` probes are unreliable vs. defn-wrapped-and-called** — the
  pre-wrap `tool-channel-band.fk` still returns garbage (7254) on a fresh build where the
  wrapped form returns 255. The band-file convention stays load-bearing.

## What shipped

- **`./fkwu` rebuilt** from current source (local artifact; nothing to commit for the binary
  itself). Full regression on the fresh build: every band at its documented verdict, including
  `arrival-band` back at 1023 and `proof/four-way-run.tbl` verdict 0 (FOUR-WAY).
- **`tests/binary-freshness-band.fk`** (new, verdict 15, deliberately zero-prelude): checks
  exactly the four capabilities the stale binary lacked (true/false literals, forward reference,
  mutual recursion, distinct trivial bools). Fresh build → 15; the stale binary → `nothing`,
  loudly. Added to `AGENTS.md`'s grounding sequence so every future arrival runs it before
  believing anything else — `ground.fk` alone provably cannot catch this class.
- **json.fk emits real `true`/`false`** — the known-wrong "both emit false" placeholder is
  lifted: trivial bools are distinguished by node identity (axiom-3 — `(intern_trivial_bool
  true)` IS one well-known node), since `node_value` of either holds a raw sentinel that is
  truthy for both. `json-band.fk` 255 → 1023 with distinct true/false round-trip checks,
  including the full `{"deep":{"nested":[true,false,null]}}` document.
- **Overclaiming comments corrected in place** — `wire-registry.fk` (the lane's shared header),
  `cell-serialize.fk`, `wire-xml.fk`, `wire-corba-cdr.fk`, `wire-path.fk`, `json.fk`: the
  mode-keyed single-recursive-function shape is kept everywhere as the lane's uniform convention,
  but no file claims it as a hard runtime constraint any more.

## Corrections to prior receipts (same-day, honest, in place)

`2026-07-01-json-fk-src-scoping-fix.md` (root cause #2 as stated), `2026-07-01-json-fk-actually-
fixed.md` (attributing json.fk's breakage to forward references), and the arrival-band notes are
each banner-corrected to point here. Root cause #1 and the paren-placement correction in
`json-fk-actually-fixed.md` both stand unchanged.

## The lesson, named

Two failures of the same shape in one day: a stale repo binary trusted because *an old check
passed*, and a stale debug binary trusted because *a compile was assumed to have succeeded*.
The fix in both cases was the same: verify the tool before believing what it shows you. The
freshness band makes that verification permanent for the repo binary.
