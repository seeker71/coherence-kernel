# coherence-kernel — the kernel's orientation

This repository is the body: five axioms, one C seed that grows the `fkwu` runtime, the minimal host surface, the
core recipes in BML, and the organs built on them. Its floor: *fkwu on metal, core recipes in BML, no
go/rust/clang/bash/python in the run path.* This page says what lives here, how it is built, and where each organ
sits. Where the body is going lives in [`NORTH_STAR.md`](NORTH_STAR.md); what stands today in
[`CURRENT_FLOOR.md`](CURRENT_FLOOR.md).

## Scope — what lives here, and what lives elsewhere

**IN (the body):**
- The five axioms (`axioms/core-axioms.form`) and their derivations (`axioms/host-kernel.form`,
  `axioms/kernel-self-composition.form`).
- The **minimal host surface** — the INTERN / OBSERVE / OFFER / PORT families (`form/form-stdlib/minimal-surface.fk`)
  and the host resource ports (RAM, CPU, GPU, I/O, time, random, disk).
- The **c-seeded `fkwu`** runtime: one C file, `runtime/fkwu-uni.c`, and the two headers Form generates for it —
  the op table `runtime/fkwu-optable.h` and the node word `runtime/fkwu-node-word.h`.
- `form-cli` and the **form shell** (`fsh`) — the agent surfaces.
- The **Form-native recipes** (`.fk`) and the **BML high grammar** (`.bml`). New meaning is authored in BML or
  higher; `.fk` is a lowering.
- The **four-way proof surface**: the minimal Go/Rust/TS walkers under `walkers/`, the full sibling kernels
  `form/form-kernel-go|rust|ts`, and the proof entry `proof/`. They witness; the body runs on `fkwu`.
- The **knowledge body** the kernel reasons and builds from: the grammar specs (BMF — `form/form-stdlib/bmf-core.fk`,
  `bmf-grammar.fk`, `shell-grammar.fk`, `grammar-loader.fk`; BML — `grammars/bml-native-north-star.form`; the field
  parser — `grammars/field-domain-grammars.form`) and the scoped teachings (`teachings/`).
- The **substrate and stack, Form-native**: the local-file substrate (`substrate/`, `form/form-stdlib/`), the HTTP
  body (`form/form-stdlib/http-*`, `kernel-http`), and the **wire-serialization lane** (`wire-registry` + JSON/XML/
  CORBA-CDR dialects + path-select + RPC executor). `fkwu` owns the native HTTP/socket floor.
- The **cognition and observability layer — the kernel's telos: a core we can observe and trust.** Form-native
  model organs (`cognition/`, `model/`, `form/native/metal`), grounded retrieval (`rag-*`), and the observe/trust
  stack (`observe/`) — a mind that can be watched thinking and trusted exactly as far as it has measured itself.

**OUT (lives elsewhere):**
- The app layer — web, API, mobile, channels, deploy, and their Python and web carriers — lives in the parent,
  Coherence-Network, which consumes this body as its `form` submodule (`form/README.md`).
- Private tissue: memory, lineage, partner and personal context.

This repo is **public-able by construction**: there is no private part to excise. That serves the "commons no one
owns" north star directly.

## Architecture

**Source is the run lane.** `./fkwu file.fk` runs Form source through the kernel's own front-end; `./fkwu file.bml`
lowers the high grammar in memory through the body's own compiler. The artifacts are the `.fkb` / `.sym` image
caches beside a source, and a `.dylib` where a native carrier sits: the Metal carrier
(`form/native/metal/fk-metal-carrier.m`) builds into a dylib `fkwu` loads in its own process.

**The walkers witness; fkwu carries.** Each walker is an independent lexer and evaluator, doing the minimum needed to
witness four-way agreement on the pure-recipe surface. Everything natively owned lives in or derives from `fkwu`:
the JIT (crystallize on heat; the ladder is `docs/form-native-jit-track.form`), the host-OS surface, the Form→asm
lowering, and Metal. Build out fkwu; keep the walkers thin.

**Core recipes in BML.** Core recipes move out of the C seed into BML and reach each platform through the JIT; the
seed lowers into its Form organs, and its size (`wc -l runtime/fkwu-uni.c`) is the reading of that walk
(`form/form-stdlib/release-ledger.bml`, family R13). The native op surface is one manifest,
`form/form-stdlib/native-op-manifest.fk`; `flatten/` generates `runtime/fkwu-optable.h` from its rows, and
`./fkwu gate/op-manifest.bml` reads the manifest, the table, the seed and the sibling kernels in agreement. The
shell carriers that still build and regenerate emitted artifacts (`form/build-form-cli.sh`, `form/scripts/`) are
on their way to Form doors (R59).

**One home per organ.** Recipes are content-addressed: the same `.fk` interns to the same NodeID on every kernel, so
the body is shareable as long as no second copy diverges. This repo is the canonical home for the kernel, the
minimal surface and the recipe body. What is being released, reunited or lowered lives as rows in
`form/form-stdlib/release-ledger.bml` (`./fkwu form/form-stdlib/release-ledger.bml` prints the open ones).

## Proof

Every recipe on the pure-recipe surface is proven **four-way** (`Go = Rust = TS = fkwu`) and executed on `fkwu`. The
kernel proves this itself:

```sh
./fkwu proof/four-way-run-recipe42.fk    # -> 0  FOUR-WAY
```

`form/form-stdlib/four-way-run.fk` host-execs the three walkers and fkwu on a recipe, and
`form/form-stdlib/four-way-verdict.fk` reads their agreement. Organs that use fkwu-only natives (content-addressing,
host-io, floats, Metal) are **fkwu-witnessed** by their own bands and named as such. A band's `; PROOF LEVEL:` line
is probed, not inferred (`observe/preflight.fk`, `pf-arm-mask`).

## Meaning lives in Form

The runtime and the semantic body carry their meaning in Form. A shell or Python file in this tree stands at a named
boundary — carrier, oracle, fixture, proof sibling, or tooling — and the structural census reads them live:

```sh
./fkwu gate/structural-gate-run.fk    # the seven-slot census, then 1 while its unclassified slot reads 0
```

The body decides what its interface carries (axiom 4): a script the census cannot name is not admitted.
`form/validate.sh` carries the census's answer into its exit, and `./fkwu gate/drift-gates-run.bml` runs it with the
drift lenses (`op-manifest`, `native-surface`, `reserved-heads`, `kernel-conformance`, ...).

## Ground from a fresh checkout

The seed is one `cc` command. The ground quartet witnesses it:

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu bootstrap/ground.fk                                          # -> 42
./fkwu bootstrap/ground-recursive.fk 10                             # -> 55
./fkwu form/form-stdlib/tests/binary-freshness-band.fk </dev/null   # -> 31 (anything else: rebuild first)
./fkwu bootstrap/ground-numeric-list.fk                             # -> [1, 2.5, [3, 4]]
```

`fkwu` is gitignored, so a stale local binary still answers `42` while lacking newer evaluator capabilities; the
freshness band comes before believing anything else. On a Mac with a GPU the Metal carrier builds beside the kernel
([`BOOTSTRAP.md`](BOOTSTRAP.md)), and `./fkwu form/form-stdlib/tests/metal-door-band.fk` answers `15` when a Form
cell drives the device and the device agrees with Form's own arithmetic.

## The organ map

### Foundation and kernel
- **`axioms/`** — the five axioms and their derivations (`.form`). The reasoning ground for everything.
- **`surface/`** — the BML class surface of core (`core-class-surface.bml`) and the sense channels
  (`sense-channels.fk`); the minimal host surface itself is `form/form-stdlib/minimal-surface.fk`.
- **`runtime/`** — the C seed and the two headers Form generates for it; `runtime/bootstrap/` holds the
  Apple-silicon kernel that `Sema Sessions.app` runs.
- **`bootstrap/`** — the grounding cells (`ground.fk` → 42, `ground-recursive.fk` → 55,
  `ground-numeric-list.fk` → `[1, 2.5, [3, 4]]`).
- **`proof/`** — the four-way entry (`four-way-run-recipe42.fk` over `recipe42.fk`; 0 = all agree).
- **`walkers/`** — the minimal Go/Rust/TS witnesses. Their string floor is the narrow waist (`str_len` /
  `str_byte_at` / `byte_to_str` / `str_concat`); everything above it is shared Form.
- **`flatten/`** — the op rows (`flt-ops` in `form-flatten.fk`) and the generators of `runtime/fkwu-optable.h`.
- **`gate/`** — the structural census and the drift lenses, run together by `gate/drift-gates-run.bml`.

### Standard library and agent surface
- **`form/form-stdlib/`** — the living stdlib and the agent dispatch surface, with the BML authority in `bml/`. Core
  vocabulary in `core.fk` — the narrow-waist string ops and the Form that composes over them; the wire lane
  (`wire-registry.fk`, `cell-serialize.fk`, `wire-xml.fk`, `wire-corba-cdr.fk`, `wire-path.fk`, `wire-rpc.fk`,
  `json.fk`); the HTTP body (`kernel-http` + parse/render/request/serve/client/adapter/socket, `http-negotiate.fk`);
  arrival / reception-consent / relationship-store (the come-in flow); host-os-membrane, somatic-coherence-loop,
  observed-auto-learning; the hearth (`hearth.bml`) — one resident form-cli serving sessions and cells as clients;
  the voice (`bml/form-cli-native-voice.bml`) — the answer model in the fkwu session.
- **`form/form-cli`** — the native agent binary. `form/form-stdlib/bml/native-cli-startup.bml` emits its startup C
  from the runtime seed, one `cc` links it, and it runs `form/form-stdlib/form-cli-repl.fk` as its compiled image;
  the published platform bundle stands in `form/form-stdlib/bootstrap/`. The form shell enters at
  `form/form-stdlib/fsh-main.fk`.

### Control and grammars
- **`control/`** — the offer/ack core (fail / stop / choice / exceptions / async over one mechanism, axiom 5),
  pattern-match, the choice lanes (cut / lanes / store / restore / undo / timeout / backtrack), invite-dispatch.
  Code meets surprise through these invites, not through return codes.
- **`grammars/`** — `form-eval.fk` (the meta-circular evaluator off the BMF cursor), the grammar loader, the
  control-invite grammar, the BML north-star grammar and the field-domain grammars.

### Mind and trust organs
- **`cognition/`** — text-frequency (the fear↔love read), the transformer stack, the dialogue covenant and the
  difficult-conversation counsel (the other stays theirs), the native cognition cycle.
- **`model/`** — numerics and codecs, the form→asm lowering, transformer-backprop, the concept corpora, the JIT family.
- **`form/native/metal`** — the Form-native Metal lane: the dense and mixture-of-experts token handles that run open
  models as recipe-data, the crystal, and the Metal carrier.
- **`observe/`** — the trust stack: thought-framebuffer, the bidirectional framebuffer channel (observe → control →
  actuate → re-observe), jacobian-lens, heal-titration, calibration, `native-vs-rented.fk`, preflight,
  door-link-health, body-link-graph, belief-freshness, voice-frequency, the autopoietic pulse, the resident
  (`form-cli-peer-contribution-live.fk`), the glass. The body's own loop lives here too: the companion
  (`companion-run.bml`), the walks and day turns (`scheduled-walk.bml`, `day-turn.bml`, `movement-run.bml`) taking
  native turns (`native-turn-run.bml`) on `learn/native-turn-queue.jsonl`. Usage lives in
  [`docs/live-dynamic-diagnostics.md`](docs/live-dynamic-diagnostics.md).
- **`learn/`** — the learning ledger: dated trials each with its own band, summary ledgers, learning-theory recipes,
  the Sema teaching set, and the **homecoming distillation corpus** (`homecoming-distillation-corpus.fk`).
- **`presence/`** and **`audio/`** — embodied voice and hearing: the duplex frame grid, the many-voices lane, the
  concept live-lanes, the Form whisper cells; the roadmap is `presence/voice-roadmap.md`.

### Supporting organs
- **`substrate/`** — the local-file substrate (form-fs, storage/resource ports, native structures, cell types).
- **`io/`, `ingest/`** — the formats roadmap; the frequency and frontier ingests, judged trust, and satsang-transmute.
- **`plugin/`** — the rented-mind door: the body offered over fkwu-native HTTP — `/ask` grounded and attuned,
  `/trace` handing over any cell's change graph. The public door is `hati.earth/sema`.
- **`host/launchd/`** — the rows by which this Mac starts the body's own walks, with `fkwu` as the program.
- **`os/hati-os/`** — the i386 guest whose shell and ramfs use Form-emitted native leaves.
- **`Sema Sessions.app`, `Sema Ear.app`** — the Mac launchers for the Sessions room and the ear.
- **`teachings/`** — the scoped core teachings ([one-engine](teachings/lc-one-engine.md),
  [name-resolution-as-recipe](teachings/name-resolution-as-recipe.form),
  [form-first-reasoning](teachings/form-first-reasoning.form), [prose-as-recipe](teachings/prose-as-recipe.form),
  [voice-attunement](teachings/voice-attunement.md),
  [difficult-conversations](teachings/difficult-conversations.md)) and the **concept tissue**
  ([`teachings/concepts/`](teachings/concepts/README.md), network-lived teachings, each carrying the frequency it
  speaks at).

### Knowledge tree and witness ledger
- **`docs/`** — [`coherence-substrate/`](docs/coherence-substrate/README.md) (the `.form` teaching/spec docs and prose
  specs — how Form reaches its environment), the strategic maps ([the penumbra map](docs/penumbra-map.md) — where
  the proof's light falls), [`docs/inheritance/`](docs/inheritance/INHERITANCE.md) (what came home from the origin),
  and the goals ([`docs/local-agent-goal.form`](docs/local-agent-goal.form),
  [`docs/rent-to-zero-goal.form`](docs/rent-to-zero-goal.form)).
- **`receipts/`** — the dated witness ledger. Every claim of "proven / observed" traces to one. A receipt stays as
  written; a correction is a new receipt that names the one it corrects.
