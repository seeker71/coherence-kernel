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
- `form-cli` — the agent surface.
- The **Form-native recipes** (`.fk`) and the **BML high grammar** (`.bml`). New meaning is authored in BML or
  higher; `.fk` is a lowering.
- The **knowledge body** the kernel reasons and builds from: the grammar specs (BMF — `form/form-stdlib/bmf-core.fk`,
  `bmf-grammar.fk`, `shell-grammar.fk`, `grammar-loader.fk`; BML — `grammars/bml-native-north-star.form`) and the
  scoped teachings (`teachings/`).
- The **stack, Form-native**: the local-file layer (`form/form-stdlib/form-fs.fk`, `storage-port.fk`), the HTTP
  body (`kernel-http`, `http-client.bml`), and the **wire-serialization lane** (`wire-registry` + JSON / CORBA-CDR
  dialects + RPC executor). `fkwu` owns the native HTTP/socket floor.
- The **cognition and observability layer — the kernel's telos: a core we can observe and trust.** Form-native
  model organs (`cognition/`, `model/`, `form/native/metal`), grounded retrieval (`rag-*`), and the observe/trust
  stack (`observe/`) — a mind that can be watched thinking and trusted exactly as far as it has measured itself.

**OUT (lives elsewhere):**
- The app layer — web, API, mobile, channels, deploy, and their Python and web carriers — lives in the parent,
  Coherence-Network, which consumes this body as its `form` submodule (`form/README.md`).
- Private tissue: memory, lineage, partner and personal context.

This repo is **public-able by construction**: there is no private part to excise. That serves the "commons no one
owns" north star directly. The person's places are read, never spelled: `hearth-home-root` (the main worktree,
from this checkout's own `.git`) and `hearth-person-home` (`HEARTH_PERSON_HOME` in the untracked `fkwu.conf`,
nothing when unset, so a door that needs it says so or stops with that reason) in `form/form-stdlib/hearth.bml`. Organs that still spell the person's home directory are the
open part of this: `git grep -l` for it over `*.bml` and `*.fk`, outside `receipts/` and the distillation corpus, read
13 on 2026-10-01 (the model registry, a tokenizer, the ask-lane router, voice, ear and glass organs,
`learn/train-loop-local-paths.fk` and four bands: ear-native, safetensors-header, whisper-shape, and
form-cli-guide-packet, whose recorded fixture row copies a real coordinator path though its claim reads only the
rows' count, swervecount and saturation), and they move onto those rows.

## Architecture

**Source is the run lane.** `./fkwu file.fk` runs Form source through the kernel's own front-end; `./fkwu file.bml`
lowers the high grammar in memory through the body's own compiler. The artifacts are the `.fkb` / `.sym` image
caches beside a source, and a `.dylib` where a native carrier sits: the Metal carrier
(`form/native/metal/fk-metal-carrier.m`) builds into a dylib `fkwu` loads in its own process.

**fkwu carries.** Everything natively owned lives in or derives from `fkwu`: the JIT (crystallize on heat; the
route is `docs/native-jit-routing.md`), the host-OS surface, the Form→asm lowering, and Metal. Build out fkwu.

**Core recipes in BML.** Core recipes move out of the C seed into BML and reach each platform through the JIT; the
seed lowers into its Form organs, and its size (`wc -l runtime/fkwu-uni.c`) is the reading of that walk
(`form/form-stdlib/release-ledger.bml`, family R13). The native op surface is one manifest,
`form/form-stdlib/native-op-manifest.fk`; `flatten/` generates `runtime/fkwu-optable.h` from its rows, and
`./fkwu gate/op-manifest.bml` reads the manifest, the table and the seed in agreement. The
shell carriers that still build and regenerate emitted artifacts (`form/build-form-cli.sh`, `form/scripts/`) are
on their way to Form doors (R59).

**One home per organ.** Recipes are content-addressed: the same `.fk` interns to the same NodeID on every kernel, so
the body is shareable as long as no second copy diverges. This repo is the canonical home for the kernel, the
minimal surface and the recipe body. What is being released, reunited or lowered lives as rows in
`form/form-stdlib/release-ledger.bml` (`./fkwu form/form-stdlib/release-ledger.bml` prints the open ones).

## Proof

Every band runs on `fkwu`, the one runtime, and answers its pin there (`form/validate.sh`; the registered verdicts
are `form/band-verdicts.txt`). `./fkwu bootstrap/ground.fk` answers 42, and `gate/canonical-conformance.bml` holds
fkwu and the Form codec to the pinned canonical expressions, the FORMBIN2 artifact and the malformed artifacts. A band
that needs a host carrier (Metal) names it with `; PROOF LEVEL: FKWU-STAGED` and `; STAGED CARRIER:`, and reads
pending, never green, while the carrier is absent.

## Meaning lives in Form

The runtime and the semantic body carry their meaning in Form. A shell or Python file in this tree stands at a named
boundary — carrier, oracle, fixture, or tooling — and the structural census reads them live:

```sh
./fkwu gate/structural-gate-run.fk    # the seven-slot census, then 1 while its unclassified slot reads 0
```

The body decides what its interface carries (axiom 4): a script the census cannot name is not admitted.
`form/validate.sh` carries the census's answer into its exit, and `./fkwu gate/drift-gates-run.bml` runs it with the
drift lenses (`op-manifest`, `native-surface`, `reserved-heads`, `canonical-conformance`, ...).

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
- **`surface/`** — the BML class surface of core (`core-class-surface.bml`); the minimal host surface itself is
  `form/form-stdlib/minimal-surface.fk`.
- **`runtime/`** — the C seed and the two headers Form generates for it.
- **`bootstrap/`** — the grounding cells (`ground.fk` → 42, `ground-recursive.fk` → 55,
  `ground-numeric-list.fk` → `[1, 2.5, [3, 4]]`).
- **`flatten/`** — the op rows (`flt-ops` in `form-flatten.fk`) and the generators of `runtime/fkwu-optable.h`.
- **`gate/`** — the structural census and the drift lenses, run together by `gate/drift-gates-run.bml`.

### Standard library and agent surface
- **`form/form-stdlib/`** — the living stdlib and the agent dispatch surface, with the BML authority in `bml/`. Core
  vocabulary in `core.fk` — the narrow-waist string ops and the Form that composes over them; the wire lane
  (`wire-registry.fk`, `cell-serialize.fk`, `wire-corba-cdr.fk`, `wire-rpc.fk`, `json.fk`); the HTTP body
  (`kernel-http.fk`, `kernel-http-header.fk`, `http-client.bml`); `relationship-store.fk` (the come-in flow's
  memory), host-os-membrane, somatic-coherence-loop, observed-auto-learning; the hearth (`hearth.bml`) — one
  resident form-cli serving sessions and cells as clients; the voice (`bml/form-cli-native-voice.bml`) — the
  answer model in the fkwu session. Where two engines still do one job — `source-compiler.fk`'s hand scanner
  beside `bml.fk`, `form-parse`/`grammar-chars` beside `bmf-grammar` — the stdlib converges on the one engine
  and releases the other.
- **`form/form-cli`** — the native agent binary, built by `form/build-form-cli.sh` and not tracked.
  `form/form-stdlib/bml/native-cli-startup.bml` emits its startup C from the runtime seed, one `cc` links it, and it
  runs `form/form-stdlib/form-cli-repl.fk` as its compiled image.

### Control and grammars
- **`control/`** — the offer/ack core (fail / stop / choice / exceptions / async over one mechanism, axiom 5),
  pattern-match, the choice lanes (cut / lanes / store / restore / undo / timeout / backtrack), invite-dispatch.
  Code meets surprise through these invites, not through return codes.
- **`grammars/`** — `form-eval.fk` (the meta-circular evaluator off the BMF cursor), the grammar loader, the
  control-invite grammar, the BML north-star grammar and the host-effect vocabulary.

### Mind and trust organs
- **`cognition/`** — text-frequency (the fear↔love read), the transformer stack, the dialogue covenant and the
  difficult-conversation counsel (the other stays theirs), the native cognition cycle.
- **`model/`** — numerics and codecs, the form→asm lowering, transformer-backprop, the concept corpora, the JIT family.
- **`form/native/metal`** — the Form-native Metal lane: the dense and mixture-of-experts token handles that run open
  models as recipe-data, the crystal, and the Metal carrier.
- **`observe/`** — the trust stack: thought-framebuffer, the bidirectional framebuffer channel (observe → control →
  actuate → re-observe), preflight, door-link-health, belief-freshness, voice-frequency, the resident
  (`form-cli-peer-contribution-live.fk`), the glass (`form/form-stdlib/bml/form-glass-*.bml`). The body's own loop
  lives here too: the companion (`companion-run.bml`), the walks and day turns (`scheduled-walk.bml`,
  `day-turn.bml`, `movement-run.bml`) taking native turns (`native-turn-run.bml`) on
  `learn/native-turn-queue.jsonl`. Usage lives in
  [`docs/live-dynamic-diagnostics.md`](docs/live-dynamic-diagnostics.md).
- **`learn/`** — the learning ledger: the meaning corpus and its nets, the perturbation pairs, the training-loop
  paths, and the **homecoming distillation corpus** (`homecoming-distillation-corpus.fk`).
- **`presence/`** — the device cell's stable identity (`device-identity.fk`), the voice's homecoming target
  (`first-native-words.fk`) and the seven inquiry planes (`inquiry-planes.fk`).

### Supporting organs
- **`ingest/`** — judged trust, satsang-transmute, and the name → build → observe loop.
- **`plugin/`** — the rented-mind door: the body offered over fkwu-native HTTP — `/ask` grounded and attuned,
  `/trace` handing over any cell's change graph. The public door is `hati.earth/sema`.
- **`docs/launchd/`** — the rows by which this Mac starts the body's own walks, with `fkwu` as the program: the
  night (`observe/scheduled-walk.bml`, 03:30) and the day (`observe/day-turn.bml`, 12:30 and 19:00), both from the
  walk checkout. `host/launchd/` still holds a second 03:30 row, `observe/movement-run.bml` in the live checkout.
- **Hati-OS** — the Hati-OS kernel's walker, emitter and host-target catalog (`form/form-stdlib/hati-os-kernel.fk`,
  `hati-os-kernel-emit.fk`, `hati-os-targets.fk`).
- **`Sema Ear.app`** — the Mac launcher for the ear; `Sema Ear Glass.command` opens the live ear glass
  (`observe/ear-glass-live.fk`).
- **`teachings/`** — the scoped core teachings ([one-engine](teachings/lc-one-engine.md),
  [form-first-reasoning](teachings/form-first-reasoning.form),
  [voice-attunement](teachings/voice-attunement.md),
  [difficult-conversations](teachings/difficult-conversations.md),
  [surprise-is-a-choice-point](teachings/surprise-is-a-choice-point.md),
  [tuning-phrases](teachings/tuning-phrases.md)) and the **concept tissue**
  ([`teachings/concepts/`](teachings/concepts/lc-trust-over-fear.md), network-lived teachings, each carrying the
  frequency it speaks at).

### Knowledge tree and witness ledger
- **`docs/`** — the goals ([`docs/local-agent-goal.form`](docs/local-agent-goal.form),
  [`docs/rent-to-zero-goal.form`](docs/rent-to-zero-goal.form)), the native-lane guides
  ([`docs/native-model-control-plane.md`](docs/native-model-control-plane.md),
  [`docs/native-jit-routing.md`](docs/native-jit-routing.md),
  [`docs/form-native-coding.md`](docs/form-native-coding.md)) and the plain-words comparison
  ([`docs/side-by-side.md`](docs/side-by-side.md)).
- **`receipts/`** — the dated witness ledger. Every claim of "proven / observed" traces to one. A receipt stays as
  written; a correction is a new receipt that names the one it corrects.
