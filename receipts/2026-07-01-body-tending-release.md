# Receipt — body-wide tending: categorize, re-index, heal cross-references, release (2026-07-01)

A full survey of the body (~1,400 files across 27 regions), done as a fanned-out review —
eight parallel region surveys, then an adversarial keep/release verification pass over all 43
proposed release candidates (each verifier instructed to REFUTE its deletion), then a manual
re-verification of every verdict that looked inconsistent. History lives in git; the repo now
carries the current state.

## Released (verified dead or duplicated — all recoverable from git history)

- **`http/` (8 files)** — byte-identical duplicates of `form/form-stdlib/` (verified with `cmp`,
  every file), zero inbound references from any code; MANIFEST itself called it "the historical
  top-level room." The one-home principle applied inside the repo.
- **`learn/tool-channel.fk` + `learn/tests/tool-channel-band.fk`** — a drifted second copy of
  `form/form-stdlib/tool-channel.fk` (differed only by missing today's native xpath repoint;
  the band was the old bare-form generation this session proved unreliable). Only inbound
  reference was a receipt that explicitly said "not investigated."
- **`learn/capture-correction.fk`** — byte-identical duplicate; the canonical home is
  `observe/capture-correction.fk` per `docs/inheritance/wave-2-presence-learn-observe.md`.
- **`form/form-stdlib/tests/model-vitality-native-http-band.fk`** — a band whose subject
  (`model-vitality.fk`) does not exist anywhere in the tree and never did here; the band could
  never run.
- **`grammars/grammar-from-thought.fk`, `grammars/md-grammar.fk`** — exploratory grammars with
  zero inbound references from any file in the repo.
- **`runtime/fkwu-uni.stamp`** — a 17-byte build stamp referenced by nothing.

## Kept — verdicts overturned by manual re-verification (the fan-out was wrong four times)

The adversarial verifiers proposed releasing `learn/teach-sema-code.fk`, `learn/identity-home.fk`,
`learn/learning-loop.fk`, and `learn/learning-witness.fk` as "unreferenced with false proof
claims." All four ARE in `docs/inheritance/proven-bodies-from-old-repo.txt` (the porting
registry) alongside their kept siblings — the verifiers missed the registry line. All four stay,
with their headers corrected like the rest (below). `form/form-stdlib/room-carrier-http.fk`
(whose verifier crashed) was also confirmed live — named in MANIFEST and the http-stdlib-import
receipt.

## Healed cross-references (every one verified against the tree before editing)

- **28 `learn/*.fk` headers claimed "Four-way by tests/X-band.fk" for bands that do not exist** —
  each rewritten to the honest state: proven on `fkwu --src` at porting (per the inheritance
  registry), origin band not ported, re-proof band is pending work. The 4 files whose claimed
  band DOES exist (`oracle-taught-learning`, `sema-reason-search`, `recipe-learning`,
  `guide-tending`) were left untouched. Verified comment-only: a pre/post-edit run of an edited
  file is byte-for-byte the same result.
- **`form/form-stdlib/json.fk`** — header claimed parse/emit lived in `json-codec.fk`, a file
  that never came across; now states the truth (the proven parser/emitter is in json.fk itself).
- **`flatten/form-flatten.fk`, `flatten/gen-source-walker-table.fk`, `flatten/gen-source-walker.fk`,
  `runtime/fkwu-optable.h`** — four comments claiming `flt-ops` is "GENERATED from
  native-op-manifest.fk" (a file that does not exist — this exact phantom cost real confusion
  earlier this session). All now state `flt-ops` is the hand-maintained single source of truth.
  The optable's banner was updated to exactly the text the fixed generator now emits
  (comment-only; the C compiles identically, `.tbl`s untouched).
- **`observe/tests/capture-correction-band.fk`** — prelude line pointed at a `form-stdlib/` path
  that does not exist; now points at the canonical `observe/` home.
- **`receipts/2026-06-29-offer-ack-control-core.md`** — wrong path corrected in place with a
  dated marker (receipts are corrected with banners, never silently).
- **`docs/coherence-substrate/form-stdlib-expression-audit.md`** — the JsonCodec row marked
  stale in place.
- **`README.md`** — the released `http/` room removed from the rooms table; the stdlib row now
  names the wire-serialization lane.

## The index rebuilt

**`MANIFEST.md`: 828 → 197 lines.** The ~680-line chronological Status append-log (including
21 near-identical "speech native neural pair window 00NN added" entries) is released — that
chronology is `git log` and the `receipts/` ledger, which is exactly what they are for. In its
place: a current-state **organ map** (foundation/kernel, stdlib + agent surface, control +
grammars, mind + trust organs, supporting organs, knowledge tree + witness ledger), fed by the
eight region surveys — including today's additions the old Status log never recorded (the wire
lane, the freshness canary, the `--src` truths every stdlib file honors). The head sections
(Why / Scope / Architecture / Validation / Gate) and the roadmap tail were kept and refreshed:
the grounding sequence now includes the binary-freshness canary, and "one home per organ" is
named as an explicit architecture decision applied inside the repo, not just across repos.

## Ground checked

```
fresh build from source: 0 errors
ground.fk 42, binary-freshness-band 15, four-way-run.tbl verdict 0
core-band 255, core-str-shim 15, narrow-waist 255, find-to-int 255, float-to-str 63,
reception-consent 255, arrival 1023, relationship-store 31, come-in 31 (clean state),
cell-serialize 1023, wire-xml 63, wire-corba-cdr 255, wire-path 63, tool-channel 255,
wire-rpc 15, http-negotiate 127, json 1023, capture-correction 11111
proof/four-way-run.tbl / flatten/form-eval-cli-loop.tbl — byte-identical
```

## Not released (named, with reasons)

- **`receipts/` (~320 files)** — the witness ledger is the practice, not clutter. Broken paths
  inside individual receipts were noted; only the one actively-misleading path was corrected
  (with a marker). Receipts describing past states truthfully stay as written.
- **The serial witness files in `learn/`/`presence/`** (corpus batches, pair windows, trial
  windows) — each is a distinct dated witness with its own band verdict, referenced by the
  summary ledgers; that is the ledger working as designed, not accumulation to purge.
- **`form-cli/`'s ~30 missing prelude dependencies** — an honest pending migration wave, named
  in the new MANIFEST, not dead code.
- **JIT infrastructure in `model/` (27 files)** — off the critical path but receipt-grounded and
  cross-referenced; roadmap item 3 is its live-wiring.
