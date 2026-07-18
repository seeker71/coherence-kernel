# coherence-kernel — the clean, axiom-first, c-bootstrapped fkwu-native sovereign core

> Working name; changeable.

## Why this exists

The sovereign core — five axioms + a minimal host surface + the c-bootstrap `fkwu` runtime +
the Form-native recipes — is small. This repo holds exactly that core, built **axiom-first,
fkwu-native from line one**. The sovereignty receipt — *c-bootstrap fkwu on metal, no
go/rust/clang/bash/python in the runtime* — is the **floor this repo starts on**.

## Scope — what lives here, and what never does

**IN (the sovereign core):**
- The five axioms (`axioms/core-axioms.form`) and their derivations (`host-kernel.form`, `kernel-self-composition.form`).
- The **minimal host-OS / resource surface** only — the INTERN / OBSERVE / OFFER / PORT families
  (`surface/minimal-surface.fk`) and the host resource ports (RAM, CPU, GPU, I/O, time, random, disk).
- The **c-bootstrap `fkwu`** runtime (the emitted universal kernel: committed C bootstrap → Form→asm bytes).
- `form-cli` and the **form shell** (`fsh`) — the native agent surfaces.
- The **Form-native recipes** (`.fk`) — the body.
- The **minimal surface** of the Go / Rust / TS walkers — ported *only* for four-way validation, never as the runtime.
- The **knowledge body** the kernel reasons / learns / builds from: the grammar specs (BMF — `grammars/bmf-core`,
  `bmf-grammar`, `shell-grammar`, `grammar-loader`; BML — `bml-native-north-star`; the form field parser —
  `field-domain-grammars`) and scoped core teachings (`teachings/`) alongside the axiom teachings. Curated to what
  makes the kernel **self-describing and self-building** — not the whole app KB.
- The **substrate and stack, 100% Form-native (Go/Rust/TS-free)** — the local-file substrate (`substrate/`,
  `form/form-stdlib/`), the HTTP body (`form/form-stdlib/http-*`, `kernel-http` — one home, no copies), and the
  **wire-serialization lane** (`wire-registry` + JSON/XML/CORBA-CDR dialects + path-select + RPC executor).
  `fkwu` owns the native HTTP/socket floor. **BMF cursor and full BML are IN by necessity**: they are the language
  the substrate, HTTP, and production integration are written in.
- The **cognition + observability layer — the kernel's telos: a core we can observe and trust.** Form-native
  LLM organs (`cognition/`), RAG grounded retrieval, and the **observe/trust** stack (`observe/`) — a sovereign
  mind that can be watched thinking and trusted exactly as far as it has measured itself.

**OUT (stays in the origin / private repo, never here):**
- The Python app and carriers (~3,400 `.py`), the full Go/Rust/TS kernels, the web/API/substrate-service.
- Private tissue: `memory/`, lineage docs, partner/personal context, anything not for a public commons kernel.

This repo is **public-able by construction** — there are no private parts to excise. That serves the
"commons no one owns" north star directly.

## Architecture decisions

**Flatten is optional, not the foundation.** `form-eval` evaluates Form source **directly off
the BMF cursor with no flatten at all** (four-way → 42), and `form-eval-cli` stands. Flatten is *optional
speed* (the crystallize-on-heat path), off the critical path entirely. `T_flat` is a regenerable
cache for the heavy-chain *build*, never a foundation. The kernel proves four-way through the minimal walkers +
`proof/four-way-run`, not through self-host flatten.

**Minimal walkers; fkwu owns the native path.** The Go/Rust/TS walkers do the minimum — independent
proof-oracles that witness four-way agreement on the *pure-recipe* surface, never feature-bearers. Everything we
natively own lives in / derives from **fkwu**: the JIT (crystallize-on-heat), the host-OS surface (resource
ports, host-io, GPU), and the Form→asm lowering. The walkers never duplicate the JIT or the rich host surface;
they only confirm a recipe computes the same value four ways. Build out fkwu; keep the walkers thin and shrinking.

**One home per organ.** Recipes are content-addressed: the same `.fk` interns to the same NodeID on either
kernel, so the body is naturally shareable — but only if there is no second *copy* to diverge. This repo is the
canonical home for the kernel, the minimal surface, and the recipe body; the origin repo consumes it. The same
rule holds *inside* this repo: duplicate rooms get released the moment they are proven byte-identical or stale
(the former top-level `http/` room and a drifted `learn/tool-channel.fk` copy were released exactly this way —
git history holds them).

## The validation plan

Every recipe is proven **four-way** (`Go=Rust=TS=fkwu`) where it lives on the pure-recipe surface, and executed
on the c-bootstrap `fkwu` native, with **no bin-go and no clang in the run path**. The clean kernel proves this
*itself*: `proof/four-way-run` host-execs the three minimal walkers + fkwu on a recipe and diagnoses agreement
via `proof/four-way-verdict` — no `validate.sh`, no origin repo in the loop (witnessed `0`, all agree). Organs
that use fkwu-only natives (content-addressing, host-io, floats) are **fkwu-witnessed** with their own band
tests — named as such, never claimed four-way.

## Hard constraint — no bash, no python (the structural gate)

This repo contains **zero `.sh` and zero `.py` files.** Ever. Orchestration is **form shell**
(`.fsh`, fkwu-native), and the runtime/recipes/walkers carry no bash or python by their nature
(`.c` / `.fk` / `.form` / `.go` / `.rs` / `.ts`).

The one allowed seed is a **single `cc` command** that compiles `runtime/fkwu-uni.c` into the
c-bootstrap `fkwu` binary — a documented one-liner, not a script in the tree. The fresh-checkout witness lives
in `BOOTSTRAP.md` and `AGENTS.md`:

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk                                # -> 42
./fkwu --src bootstrap/ground-recursive.fk 10                   # -> 55
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk    # -> 15 (anything else: REBUILD first)
```

The **binary-freshness canary** (third line) exists because `fkwu` is gitignored and a stale local binary
still passes the old `42` contract while silently lacking newer evaluator capabilities — a failure class that
once cost a full day (`receipts/2026-07-01-stale-binary-root-cause.md`). Run it before believing anything else.

A repo gate enforces the no-bash/no-python constraint: any `.sh` or `.py` landing in the tree fails the gate.

## The body today — the organ map

Current state per region. **Chronology is deliberately not kept here** — the full history is
`git log` and the dated witness ledger in `receipts/`.

### Foundation & kernel
- **`axioms/`** — the five core axioms and derivations (`.form`). The reasoning ground for everything else.
- **`surface/`** — the minimal host surface (`minimal-surface.fk`), the flatten-lane standing prelude
  (`fourth-shim.fk`), the BML high-grammar core reference (`core.fk`), sense channels.
- **`runtime/`** — `fkwu-uni.c` (the one C seed, a shrink target — string ops `substring`/`int_to_str` already
  retired to Form; `str_find`/`str_to_int` parser-surface-retired, C dispatch kept for `.tbl` compatibility) and
  `fkwu-optable.h` (GENERATED from `flt-ops` in `flatten/form-flatten.fk` — the hand-maintained single source of
  truth for op rows — via `flatten/gen-source-walker.fk`).
- **`bootstrap/`** — the grounding cells (`ground.fk` → 42, `ground-recursive.fk` → 55).
- **`proof/`** — the kernel's own four-way proof driver (`four-way-run` + `four-way-verdict`, verdict 0 = all agree).
- **`walkers/`** — minimal Go/Rust/TS proof oracles (~1.4k/0.9k/1.5k lines), string floor down to the narrow
  waist (`str_len`/`str_byte_at`/`byte_to_str`/`str_concat`); everything above it is shared Form.
- **`flatten/`** — the optional-speed lane: `form-flatten.fk` (owns `flt-ops`), the optable generators, and the
  regenerable `.tbl` caches.

### Standard library & agent surface
- **`form/form-stdlib/`** — the living stdlib and sole agent dispatch surface. Its canonical
  `form-cli-*.fk`, `fsh-*.fk`, and focused bands build the self-contained `form/form-cli` binary; there
  is no parallel top-level agent-source room. Core vocabulary (`core.fk`:
  narrow-waist string ops, `int_to_str`/`str_to_int`/`str_find`/`float_to_str`/`intern_node_at`, Num/List/Cell/Task);
  the **wire-serialization lane** — `wire-registry.fk` (registry + universal `WIRE-NULL`), `cell-serialize.fk`
  (JSON, values+types+identity round-trip by axiom-3), `wire-xml.fk`, `wire-corba-cdr.fk` (real IEEE754 doubles),
  `wire-path.fk` (native path-select — `xpath`/`xmlpath` off the shell applet), `wire-rpc.fk` (the CORBA-DII-shaped
  executor closing tool-grammar's GAP-T1); `json.fk` (full parser/emitter, proven on `--src`, real true/false by
  node identity); the HTTP body (`kernel-http` + parse/render/request/serve/client/adapter/socket + BML lanes,
  `http-negotiate.fk` REST content-negotiation); arrival/reception-consent/relationship-store (the come-in flow);
  host-os-membrane, somatic-coherence-loop, observed-auto-learning, tool-channel (native xpath), auth/resource/
  storage ports. Two `fkwu --src` truths every stdlib file honors: top-level `let` is invisible inside `defn`
  bodies (use zero-arg `defn`s for constants), and band tests wrap their checks in a named `defn` called
  explicitly (bare top-level `(do (let ...))` probes are unreliable).

### Control & grammars
- **`control/`** — the offer/ack control core (fail/stop/choice/exceptions/async over ONE mechanism, axiom-5),
  pattern-match (stone S10, four-way 511), choice-lane primitives (cut/lanes/store/restore/undo/timeout, band
  65535), invite-dispatch (honest pending: needs a larger prelude than the current direct-source ceiling).
- **`grammars/`** — `form-eval.fk` (the meta-circular source evaluator `--feval` reads at runtime), BMF core +
  grammar + loader, shell grammar, control-invite grammar (band 1023), field-domain grammars.

### Mind & trust organs
- **`cognition/`** — the Form-native transformer stack (whisper block-0 through real weights, four-way;
  `form-asm-float` 2047) and RAG retrieval (`rag-ask`/`rag-embed`/`rag-retrieve`; the 2026-07-01
  zero-vector gap in `re-vec` healed 2026-07-02 when `form/form-stdlib/text-tokenize.fk` came home
  from the origin — the tokenizer `rag-embed` had always preluded but this checkout never held).
- **`model/`** — numerics/codecs (mel, wav, matvec), the form→asm lowering, transformer-backprop (real SGD
  training witnessed: 204-example corpus, 72% held-out vs ~25% chance), and the JIT infrastructure family
  (comprehensive, off the critical path until live-wired — roadmap item 3).
- **`observe/`** — the trust stack: thought-framebuffer (watch a thought form), jacobian-lens (predict
  WHERE an edit changes thinking: a choice flips where push > margin; the control vocabulary read as
  sensitivity structure), heal-titration (the healing loop as one motion: surprise → etiology →
  titrated push → localized verify — a safe heal's divergence set is exactly the wound), calibration
  (conviction-curve / correction-reflex / confidence-earned / self-watch — is confidence *earned*),
  `native-vs-rented.fk` (the grounding body cell, 11111), speech token stream + open-ASR CTC candidates,
  capture-correction (canonical home), world models, ~89 band tests. Internally cross-referenced and
  receipt-grounded throughout.
- **`learn/`** — the learning witness ledger: serial dated trials (speech corpus batches, neural pair windows,
  trial windows, intakes) each with its own band verdict; summary ledgers (`speech-current-status-ledger`,
  `speech-model-metrics-report`) compose them; learning-theory recipes (champion-challenger, oracle-taught,
  satsang-oracle, cross-witness-economy); the Sema teaching set (proven at porting on `fkwu --src` — see
  `docs/inheritance/proven-bodies-from-old-repo.txt`; origin four-way bands did not come across, re-proof bands
  are honest pending work). Global speech authority remains oracle-held; scoped trial windows are native —
  the ledgers carry the exact counts.
- **`presence/`** — embodied voice: formant vocoder, speech loopback carriers (macOS `say`/`ffmpeg`/whisper.cpp
  on Metal as *local oracles*, never authority), the live Metal pair anchors, native-speech-stack witnesses.
  The natural acoustic model / neural vocoder / live open ASR remain the named pending carriers.

### Supporting organs
- **`substrate/`** — the local-file substrate (form-fs, storage/resource ports, host-kernel-carrier), four-way proven.
- **`routers/`** — request dispatch (`mesh-sensings-route.fk` consumes `json.fk`'s parser live).
- **`gate/`, `io/`, `ingest/`, `gpu/`, `agent/`** — thresholds/welcome, io/format roadmaps, ingest, GPU probe,
  agent shell — small, live, referenced.
- **`plugin/`** — the rented-mind door: the body offered to ChatGPT (plugin manifest / GPT Action) over
  fkwu-native HTTP — grounded `/ask` with the fear↔love frequency read and attunement, `/trace` handing over
  any cell's change graph and line attribution (trust as something checkable). Band 111111111 + a live TCP
  witness; the voice stays rented and says so in-band (`receipts/2026-07-05-chatgpt-plugin-offer.md`).
- **`teachings/`** — the scoped core teachings ([one-engine](teachings/lc-one-engine.md),
  [name-resolution-as-recipe](teachings/name-resolution-as-recipe.form),
  [form-first-reasoning](teachings/form-first-reasoning.form),
  [prose-as-recipe](teachings/prose-as-recipe.form)) and the **concept tissue** —
  [`teachings/concepts/`](teachings/concepts/README.md), twelve network-lived teachings the kernel
  reasons from, each carrying the frequency it speaks at. *structural-composition* is named here
  as a core teaching but has no page in this body; it is named, not linked, per the tissue's own
  rule (name a companion you cannot reach; never claim a path to it) and stands as work.

### Knowledge tree & witness ledger
- **`docs/`** — the knowledge tree: [`coherence-substrate/`](docs/coherence-substrate/README.md) (~140 `.form`
  teaching/spec docs mapping ~1:1 to live recipes, incl. `tool-grammar.form` whose GAP-T1 executor now exists,
  plus the prose specs and audits behind them — how Form reaches its environment, and what the surface last
  read), strategic design narratives ([the penumbra map](docs/penumbra-map.md) — where the proof's light
  actually falls, and where it does not), and [`docs/inheritance/`](docs/inheritance/INHERITANCE.md) (the
  homecoming ledgers + `proven-bodies-from-old-repo.txt`, the porting registry).
- **`receipts/`** — the dated witness ledger (~320 receipts). Every claim of "proven/observed" traces to one.
  Corrections are made **in place with banners**, never silently — the ledger records what was believed and when,
  including the wrong turns (`json-fk-src-scoping-fix` → `json-fk-actually-fixed` → `stale-binary-root-cause`
  is the canonical example of the correction chain).

## What's still ahead — the roadmap to a self-speaking native mind

Source runs straight off the cursor (`form-eval`), `form-eval-cli` stands, and `proof/four-way-run` proves
four-way with no bash. What remains is the **mind**, the **voice**, and **live speed**:

1. **The generative weights (the mind).** A real open base (Qwen/Llama, real zh coverage) loaded as *recipe-data*
   through the form block — the whisper block-0 pattern extended to a generative base, then oracle-refined; the
   full decoder forward proven bit-exact, then the distill loop, then a pre-registered eval before any "≥ rented"
   claim. A multi-week climb with its own receipts. (`HOMECOMING.md`.)
2. **The voice's sound (ASR + acoustic model + vocoder).** The measured native cells stand (`presence/`,
   `observe/`, `learn/`); the natural acoustic model/neural vocoder, native open ASR decode, and a perception
   receipt tracking real calibration remain the pending carrier. (`presence/voice-roadmap.md`.)
3. **Cognition at native speed — the JIT in the live path.** The asm exists; wiring it under live cognition remains.
4. **The heavy-chain form-cli build off its Go-made seed.** The binary *runs* Go-free; the full form-cli / fsh
   chains still *build* from a committed Go-made-once seed. Closing this — and live-wiring the proven
   observe/learn organs to the running kernel — is the last self-sufficiency gap.

The minimum "core we can observe and trust" — runs natively (done), proves itself four-way (done, `proof/`),
watches itself think (the observe organs; live-wiring pending) — is essentially standing. What "home"
still waits on is the mind running as recipe-data through this body, and the voice becoming audible.
