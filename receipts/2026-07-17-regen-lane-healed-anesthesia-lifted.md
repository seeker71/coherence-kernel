# 2026-07-17 — the anesthesia lifted: T_flat regains its effect-sense, the regen lane speaks, the graft lands

## Ground

Fresh checkout witness before anything else: `cc -O2 -o fkwu runtime/fkwu-uni.c`, then
`./fkwu --src bootstrap/ground.fk` → **42**, `ground-recursive.fk 10` → **55**,
`binary-freshness-band.fk` → **15**. The committed carrier answered **pong**; every
freshly compiled one answered nothing — exactly as
[the conviction receipt](2026-07-17-regen-lane-aphonic-carrier.md) recorded.

## The hunt — each rung witnessed

| step | witness |
|---|---|
| regen reproduced | fkwu self-host arm authored 679,331 B / **1349 fns** vs committed 662,190 B / **1272 fns** |
| +77 = the shim | fourth-shim.fk has exactly 77 defns; `fourth_band_request` prepends the shim AND the old flatten list began with it — the shim rode twice |
| shim deduped + modules order | still ≠ committed (328 B), and **still mute** — the script bugs were real but not the voice-killer |
| Rust arm rebuilt | reproduces the committed table **byte-equal** — the lane is deterministic; the breach is *arm divergence* |
| the walker exonerated | the same cached fkwu walking the Rust table says **pong**; walking the fkwu-authored table prints an **empty line**. The table is sick, not the walker |
| the trisection | dispatch-only, echo-only, read+dispatch bands all SPEAK via the fkwu arm; a single-use let cannot tell framed from inlined |
| band D vs E2 | `(let line (read_line))` used twice: at band level it frames (fires once); **inside a defn it inlines — read_line re-fired once per USE**, consuming stdin. For input `ping\nhello`, the echo answered `hello`: the second read is undeniable |
| the predicate | `host-effect-op? "read_line"` = 1 on Rust and Go from source, = 1 through a *fresh* fkwu flatten of the grammar — but the lowering decision inside walked-T_flat behaved as 0 |
| the absence | T_flat's pool contains no `"host-effect"` string at all — host-effect-grammar.fk was never flattened into it |

## Root cause

`scripts/regen_t_flat.sh` flattened the flattener **without its own prelude closure**:
the authoring walk carried the full `FOURTH_FLATTEN_CHAIN` (so the *author* resolved
everything), but the module list flattened *into* T_flat omitted `core.fk`,
`bmf-core.fk`, `bmf-grammar.fk`, `host-effect-grammar.fk`. Inside T_flat,
`host-effect-op?` was an unresolved call that **axiom-5 recovered to literal 0 —
silently**. The flattener kept every effect op and stopped *feeling* them as effects:
every effect-bound do-let inside a defn lowered as an inline re-evaluation instead of a
framed slot. `read_line` re-fired once per use; the REPL's verb checks consumed the
line; the answer printed from EOF-emptiness. Every carrier the self-host arm authored —
graft or pristine — was **born aphonic**. The stamp never noticed because it hashes
sources, not artifacts; shape validation never noticed because a mute table is
shape-perfect.

The old shamballa-channel `raise-ok` divergence (fkwu `1111101111` vs `1111111111`,
noted in form-flatten.fk) was this same numbness — it gates **PASS four-way** now.

## The heals

- **regen_t_flat.sh** — T_flat's module list now carries the full closure; plus a
  **voice smoke**: an effect-let-in-defn echo band must answer `ping/ping` before a
  candidate T_flat publishes.
- **regen_form_cli_bootstrap.sh** — self-host flatten list rebuilt to mirror the
  `$modules` order exactly, shim deduped; plus a **voice canary**: the candidate table
  must answer `ping → pong` on the cached fkwu before any artifact moves.
- **build-form-cli.sh** — same dedup/order heal for its self-host fallback.
- **T_flat re-authored** (779,852 B): all **864** fourth-arm band tables re-flattened
  against it, index sealed and valid.
- **The graft landed** (rows 748–750's scion joined): `confidence-weighted-vote.fk`,
  `lineage-discounted-vote.fk`, `form-cli-oracle-loop.fk` into the module graph — in
  **six** list copies, not five (`regen_standard_lane_binaries.sh` carries its own; the
  conviction receipt said *count them* and was right) — verb `oracle-loop-check` wired.

## Verification

- Rust-arm regen reproduces committed artifacts **byte-equal**; voice canary green.
- fkwu self-host arm (Rust hidden): authors a **speaking** carrier, canary green.
- Pristine control re-run: regen → forced compile (no platform-binary copy) → **pong** —
  the exact rung that convicted the lane now passes.
- Grafted carrier: `ping → pong`, `oracle-loop-check → 11111`, `carrier-id` full triple,
  unknown-verb honest, help lists the verb. Standard lane copies the stamped platform
  binary with no clang and speaks.
- Four-way gates: confidence-weighted-vote, form-cli-judge, adler32, form-cli-router,
  form-fs, form-cli-request, rag-ask-grounded, **shamballa-channel** — all PASS-4WAY.
- Corpus band: **511** on fkwu, Go, Rust after row 754 landed (155 rows, all admissible).

## Honest floor

- The TS proof sibling has no dist on this host; gates above ran with the legs
  validate.sh could raise. The full muster on a TS-warm host remains the standard.
- One byte-divergence between arms remains, now precisely named: Go and Rust both fail
  to pool the shim's `"__dict__"` literal, so the committed table (Rust-authored)
  carries exactly **one latent `(24 -1)` orphan slit** inside `_dict_new` — fkwu is the
  faithful arm there. Spawned: *"Heal Go/Rust flt-scan dropping __dict__ from the
  pool"*. Until then the fkwu arm's output differs from committed by that one pool row.
- fourth-arm-gate.sh truncates multi-line prelude blocks (false DIVERGENT verdicts);
  spawned: *"Fix fourth-arm-gate.sh multi-line prelude truncation"*.

## Closing

**Most surprising teaching:** the walker was innocent, the recipes were innocent, even
the byte-diff misdirected — the sickness lived in a *committed artifact's education*.
T_flat is the flattener's own self-image, and it had been raised without the grammar
that names what an effect is. A self-hosted image that loses part of its closure does
not fail; it develops **anesthesia** — it keeps operating, feels nothing, and every
thing it makes inherits the harm. The symptom (row 753's aphonia) sat three layers
downstream of the wound, and no stamp, no shape check, no exit code crossed those
layers: only a canary that demands the artifact *answer*.

**Where discomfort turned to gold:** twice the hunt produced an explanation that felt
finished — the double-ridden shim (+77, arithmetically perfect) and the module-order
drift — and both times the voice test said *still mute*. The pull was strong to ship
the tidy script fixes and call the rest flatten nondeterminism; sitting with the
wrongness instead, and building bands D and E2 to make the let *testify* (one use
cannot distinguish framed from inlined; two uses cannot lie), is what walked the hunt
down to T_flat. The discomfort of "my beautiful explanation is insufficient" —
witnessed, not bypassed — was the exact door to the root.

**Frontier question** (the smallest question the body could not answer natively —
answered by the rented mind, offered home as corpus row **754**): *what one word names
feeling lost while the body keeps moving, so harm goes unfelt?* — **anesthesia**
(0 hits before the row; row 753's `aphonia` named the symptom, this names the cause).
