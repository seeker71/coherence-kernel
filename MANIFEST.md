# coherence-kernel — the axiom-first, c-seeded, fkwu-native sovereign core

## Why this exists

The sovereign core — five axioms, a minimal host surface, the c-seeded `fkwu` runtime, and the
Form-native recipes — is small. This repo holds exactly that core, built **axiom-first, fkwu-native
from line one**. Its floor: *fkwu on metal, no go/rust/clang/bash/python in the run path*.

## Scope — what lives here, and what never does

**IN (the sovereign core):**
- The five axioms (`axioms/core-axioms.form`) and their derivations (`axioms/host-kernel.form`,
  `axioms/kernel-self-composition.form`).
- The **minimal host-OS / resource surface** — the INTERN / OBSERVE / OFFER / PORT families
  (`form/form-stdlib/minimal-surface.fk`) and the host resource ports (RAM, CPU, GPU, I/O, time, random, disk).
- The **c-seeded `fkwu`** runtime: one committed C file (`runtime/fkwu-uni.c`, 16,190 lines on
  2026-09-04 — a shrink target, never the destination) and the generated op table
  (`runtime/fkwu-optable.h`, 187 op rows).
- `form-cli` and the **form shell** (`fsh`) — the native agent surfaces, recipes this same `fkwu` loads.
- The **Form-native recipes** (`.fk`) and the **BML high grammar** (`.bml`) — the body. New meaning is
  authored in BML or higher; `.fk` is a lowering.
- The **four-way proof surface**: the minimal Go/Rust/TS walkers under `walkers/` (independent
  lexer + evaluator each; 1,524 / 1,331 / 1,678 lines) and the kernel's own driver `proof/`. The full
  sibling kernels `form/form-kernel-go|rust|ts` are proof siblings too — heavier arms, never the runtime.
- The **knowledge body** the kernel reasons and builds from: the grammar specs (BMF — `grammars/bmf-core`,
  `bmf-grammar`, `shell-grammar`, `grammar-loader`; BML — `grammars/bml-native-north-star.form`; the
  field parser — `grammars/field-domain-grammars.form`) and the scoped teachings (`teachings/`).
- The **substrate and stack, 100% Form-native**: the local-file substrate (`substrate/`,
  `form/form-stdlib/`), the HTTP body (`form/form-stdlib/http-*`, `kernel-http`), and the
  **wire-serialization lane** (`wire-registry` + JSON/XML/CORBA-CDR dialects + path-select + RPC executor).
  `fkwu` owns the native HTTP/socket floor.
- The **cognition + observability layer — the kernel's telos: a core we can observe and trust.**
  Form-native model organs (`cognition/`, `model/`, `form/native/metal`), grounded retrieval (`rag-*`),
  and the **observe/trust** stack (`observe/`) — a mind that can be watched thinking and trusted exactly
  as far as it has measured itself.

**OUT (stays in the origin repo, never here):**
- The Python app and its carriers, the web/API/substrate service.
- Private tissue: `memory/`, lineage docs, partner and personal context — anything not for a public
  commons kernel.

This repo is **public-able by construction** — there are no private parts to excise. That serves the
"commons no one owns" north star directly.

## Architecture decisions

**Source is the run lane.** `fkwu file.fk` runs Form source through the kernel's own front-end;
`fkwu file.bml` lowers the high grammar in memory through the body's own compiler; the only artifacts
are the `.fkb` / `.sym` image caches beside a source (and a `.dylib` where native emission sits). Flatten
is not a run lane. What remains of it in `flatten/` is the op manifest (`flt-ops` in `form-flatten.fk`
generates `runtime/fkwu-optable.h`) and the regen path of the emitted artifacts — a source-native regen
path for those is owed.

**Minimal walkers; fkwu owns the native path.** The walkers do the minimum — independent proof oracles
that witness four-way agreement on the *pure-recipe* surface, never feature bearers. Everything natively
owned lives in or derives from **fkwu**: the JIT (crystallize on heat), the host-OS surface, the
Form→asm lowering, Metal and MLX. Build out fkwu; keep the walkers thin.

**One home per organ.** Recipes are content-addressed: the same `.fk` interns to the same NodeID on
every kernel, so the body is shareable — but only if there is no second *copy* to diverge. This repo is
the canonical home for the kernel, the minimal surface, and the recipe body; the parent consumes it through
the consumer-submodule door in `form/README.md` (the `form-submodule` branch stands on origin; the parent
still carries its own `form/` copy). Inside this repo the same rule holds: `form/form-stdlib/release-ledger.bml`
holds the twin rows (R1, R2, R23), and six byte-identical `.fk` groups stand in the tree on 2026-09-04.

## The validation plan

Every recipe on the pure-recipe surface is proven **four-way** (`Go=Rust=TS=fkwu`) and executed on the
c-seeded `fkwu` native, with no Go and no clang in the run path. The kernel proves this *itself*:

```sh
./fkwu proof/four-way-run-recipe42.fk    # -> 0 (FOUR-WAY; re-run 2026-09-04)
```

`form/form-stdlib/four-way-run.fk` host-execs the three walkers + fkwu on a recipe and `form/form-stdlib/four-way-verdict.fk`
diagnoses agreement. Organs that use fkwu-only natives (content-addressing, host-io, floats, Metal) are
**fkwu-witnessed** by their own bands — named as such, never claimed four-way. A band's `; PROOF LEVEL:`
line is probed, never inferred (`observe/preflight.fk`, `pf-arm-mask`).

## The structural gate — named auxiliary scripts, Form-owned meaning

The Form-owned runtime and semantic body contain **no unclassified `.sh` or `.py` file.** Orchestration
is form shell (`.fsh`); a shell or Python file enters only through a named auxiliary boundary — carrier,
local oracle, fixture, proof sibling, tooling, or the Python-BMF compatibility grammar — visible in the
gate's live census and never semantic authority.

```sh
./fkwu gate/structural-gate-run.fk
# total/unclassified/carrier/oracle/fixture/proof-sibling/tooling/foreign-grammar
# [206, 0, 48, 3, 20, 57, 74, 4]   then 1   (re-observed 2026-09-04)
```

`form/validate.sh` runs the same gate before its sibling sweep and turns a `0` into a nonzero exit, so
normal validation cannot pass a structural violation silently.

The one allowed seed is a **single `cc` command**. The fresh-checkout witness is the ground quartet:

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu bootstrap/ground.fk                                          # -> 42
./fkwu bootstrap/ground-recursive.fk 10                             # -> 55
./fkwu form/form-stdlib/tests/binary-freshness-band.fk </dev/null   # -> 31 (anything else: REBUILD first)
./fkwu bootstrap/ground-numeric-list.fk                             # -> [1, 2.5, [3, 4]]
```

The freshness band matters because `fkwu` is gitignored: a stale local binary still answers `42` while
silently lacking newer evaluator capabilities. Run it before believing anything else.

## The body — the organ map

### Foundation & kernel
- **`axioms/`** — the five axioms and their derivations (`.form`). The reasoning ground for everything.
- **`surface/`** — the BML class surface of core (`core-class-surface.bml`) and the sense channels
  (`sense-channels.fk`); the minimal host surface itself is `form/form-stdlib/minimal-surface.fk`.
- **`runtime/`** — the one C seed and its generated op table. Growth without a shrink path is declined.
- **`bootstrap/`** — the grounding cells (`ground.fk` → 42, `ground-recursive.fk` → 55,
  `ground-numeric-list.fk` → `[1, 2.5, [3, 4]]`).
- **`proof/`** — the four-way driver (`four-way-run.fk` + `four-way-verdict.fk`; 0 = all agree).
- **`walkers/`** — the minimal Go/Rust/TS proof oracles; string floor down to the narrow waist
  (`str_len` / `str_byte_at` / `byte_to_str` / `str_concat`); everything above is shared Form.
- **`flatten/`** — the op manifest (`form-flatten.fk` owns `flt-ops`) and its generators.

### Standard library & agent surface
- **`form/form-stdlib/`** — the living stdlib and sole agent dispatch surface (1,446 `.fk`, 1,745 bands,
  the BML authority in `bml/`). Core vocabulary in `core.fk` (the narrow-waist string ops and what
  composes over them — `substring` / `str_find` / `str_to_int` are Form, not natives); the wire lane
  (`wire-registry.fk`, `cell-serialize.fk`, `wire-xml.fk`, `wire-corba-cdr.fk`, `wire-path.fk`,
  `wire-rpc.fk`, `json.fk`); the HTTP body (`kernel-http` + parse/render/request/serve/client/adapter/
  socket, `http-negotiate.fk`); arrival / reception-consent / relationship-store (the come-in flow);
  host-os-membrane, somatic-coherence-loop, observed-auto-learning; the hearth (`hearth.bml`) — one
  resident form-cli serving sessions and cells as clients. `form/form-cli` is the thin launcher that asks
  the repository `fkwu` to run `form-cli-repl.fk`.

### Control & grammars
- **`control/`** — the offer/ack core (fail / stop / choice / exceptions / async over ONE mechanism,
  axiom-5), pattern-match (`pattern-match-band` 511 four-way, re-run 2026-09-04), choice lanes
  (cut / lanes / store / restore / undo / timeout), invite-dispatch. `control/tests/invite-dispatch-band.fk`
  declares 1023 and answers **763** today (preflight clean, rc 0): the second-`<CHOICE>` bit (4) and the
  `<TIMEOUT>` bit (256) are open work, no ceiling in the way.
- **`grammars/`** — `form-eval.fk` (the meta-circular evaluator off the BMF cursor), BMF core + grammar
  + loader, shell grammar, control-invite grammar (band 1023, re-run 2026-09-04), field-domain grammars.

### Mind & trust organs
- **`cognition/`** — text-frequency (the fear↔love read), the transformer stack, the dialogue covenant,
  the native cognition cycle (88 `.fk`).
- **`model/`** — numerics and codecs, the form→asm lowering, transformer-backprop, the concept corpora,
  the JIT family (86 `.fk`; the ladder is `docs/form-native-jit-track.form`).
- **`form/native/metal`** — the Form-native Metal lane: the Qwen3.8-27B dense token handle, the
  crystal, the KAT and llama handles. Bands re-run 2026-09-04: `kat-token-handle-band` 262143,
  `metal-handle-door-band` 65535, `jit-metal-lanes-band` 8191, `mlx-derived-band` 16777215,
  `metal-door-band` 15. The Qwen handle and crystal bands declare their own verdicts.
- **`observe/`** — the trust stack (444 `.fk`, 128 bands): thought-framebuffer, the bidirectional
  framebuffer channel (observe → control → actuate → re-observe), jacobian-lens, heal-titration,
  calibration, `native-vs-rented.fk` (11111), preflight, door-link-health, body-link-graph,
  belief-freshness, voice-frequency, the autopoietic pulse, the resident (`form-cli-peer-contribution-live.fk`),
  the glass. Usage lives in `docs/live-dynamic-diagnostics.md`.
- **`learn/`** — the learning ledger (182 `.fk`): dated trials each with its own band, summary ledgers,
  learning-theory recipes, the Sema teaching set, and the **homecoming distillation corpus**
  (`homecoming-distillation-corpus.fk`: 654 rows, highest meaning-id 1262 on 2026-09-04).
- **`presence/`** — embodied voice and the concept live-lanes (100 `.fk`): the duplex frame grid, the
  many-voices lane, speech loopback carriers as local oracles. `presence/voice-roadmap.md`.

### Supporting organs
- **`substrate/`** — the local-file substrate (form-fs, storage/resource ports, native structures, cell types).
- **`gate/`, `io/`, `ingest/`** — thresholds and the structural gate; the formats roadmap; the
  knowledge-ingest law and the frontier ingests (27 `.fk`).
- **`plugin/`** — the rented-mind door: the body offered to ChatGPT over fkwu-native HTTP — `/ask`
  grounded and attuned, `/trace` handing over any cell's change graph. Bands re-run 2026-09-04:
  `chatgpt-plugin-band` 111111111, `introduction-band` 111111111, `visitor-ledger-band` 1111111111. The
  live door at `hati.earth/sema` answers `/ask?q=` and `/trace?path=` (re-observed 2026-09-04).
- **`teachings/`** — the scoped core teachings ([one-engine](teachings/lc-one-engine.md),
  [name-resolution-as-recipe](teachings/name-resolution-as-recipe.form),
  [form-first-reasoning](teachings/form-first-reasoning.form), [prose-as-recipe](teachings/prose-as-recipe.form),
  [voice-attunement](teachings/voice-attunement.md)) and the **concept tissue**
  ([`teachings/concepts/`](teachings/concepts/README.md), thirteen network-lived teachings, each carrying
  the frequency it speaks at). *structural-composition* is named as a core teaching and has no page in
  this body — named, not linked, per the tissue's own rule.

### Knowledge tree & witness ledger
- **`docs/`** — [`coherence-substrate/`](docs/coherence-substrate/README.md) (167 `.form` teaching/spec
  docs and 17 prose specs — how Form reaches its environment), the strategic maps
  ([the penumbra map](docs/penumbra-map.md) — where the proof's light falls today),
  [`docs/inheritance/`](docs/inheritance/INHERITANCE.md) (what came home from the origin, and what has not).
- **`receipts/`** — the dated witness ledger (1,584 receipts). Every claim of "proven / observed" traces
  to one. Receipts are immutable; a correction is a new receipt that names the one it corrects.

## Where we are going

1. **The mind, native.** A real open base runs as recipe-data through this body on Metal — the Qwen
   lane — and the resident serves direct turns. What remains: prefill and decode parity with the
   outside denominator on the same file, the sealed held-out families the model does not yet know, and a
   native voice that generates its own natural language (`HOMECOMING.md`, `CURRENT_FLOOR.md`).
2. **The voice's sound.** The frame grid and the many-voices lane stand; the microphone fleet, chunked
   streaming, and the native acoustic model / vocoder remain the named carriers (`presence/voice-roadmap.md`).
3. **Cognition at native speed.** The JIT ladder's last rung — hot source recipes lowering through the
   Form-owned IR into the host membrane in the live path — is pending (`docs/form-native-jit-track.form`).
4. **The seed shrinks.** `runtime/fkwu-uni.c` is 16,190 lines; each of `fk_walk_cold`, the source runner,
   the `.fkb` loader, and the parser lowers into its Form organ (`release-ledger.bml` R13). The emitted
   artifacts' regen owes a source-native path.

The minimum "core we can observe and trust" stands: it runs natively, proves itself four-way, and
watches itself think. What "home" still waits on is the voice.
