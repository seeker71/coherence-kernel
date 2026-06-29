# coherence-kernel — the clean, axiom-first, c-bootstrapped fkwu-native sovereign core

> Working name; changeable. Started 2026-06-28 from the Coherence-Network body, as a clean
> restart that begins *on* the sovereignty floor instead of climbing toward it through legacy infra.

## Why this exists

The sovereign core — five axioms + a minimal host surface + the c-bootstrap `fkwu` runtime +
the Form-native recipes — is small (~450 lines of axioms/surface + ~700 `.fk` recipes). In the
origin repo it is buried under ~3,400 Python files, the full Go/Rust/TS kernels, the web/API/app,
and private tissue. That buried-ness is friction, not difficulty: building a standing native shell
there meant fighting a `T_flat` marker maze, a bin-go convenience trap, and committed-bootstrap
tangles — accumulated cruft, not a hard problem.

A clean repo built **axiom-first, fkwu-native from line one** has none of that. The sovereignty
receipt — *c-bootstrap fkwu on metal, no go/rust/clang/bash/python in the runtime* — becomes the
**floor you start on**, not a rung you climb toward.

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
  `field-domain-grammars`) and scoped core teachings (`teachings/` — one-engine, name-resolution-as-recipe,
  structural-composition, form-first-reasoning, prose-as-recipe) alongside the axiom teachings. Curated to what
  makes the kernel **self-describing and self-building** — not the whole app KB.
- The **substrate and stack, 100% Form-native (Go/Rust/TS-free)**. The local-file substrate is *already* proven
  four-way and lives here (`substrate/` — `form-fs` 14-bit, `storage-port`, `resource-port` 7-bit,
  `host-kernel-carrier`); the HTTP *client* is native (`http/http-client`). The HTTP *server*/TCP and the
  *production* DB integration (postgres, today the Python fan-out carrier) are Form targets authored on this
  same minimal surface — no Go/Rust/TS. **BMF cursor and full BML are IN by necessity**: they are the language
  the substrate, HTTP, and production integration are written in (`grammars/bmf-*`, `bml-native-north-star`).
- The **cognition + observability layer — the kernel's telos: a core we can observe and trust.** Form-native
  LLM (`cognition/` — the whisper-tiny transformer stack four-way: `transformer-block` 511,
  `transformer-generate`, `whisper-block0` 1023; the matvec lowered to asm bytes, `form-asm-float` **2047**) and
  RAG grounded retrieval (`rag-ask` / `rag-embed` / `rag-retrieve`, four-way). And the **observe/trust** stack
  (`observe/`): `thought-framebuffer` (watch the kernel form a thought) + the calibration stack
  (`conviction-curve`, `correction-reflex`, `confidence-earned`, `self-watch`) that measures whether the
  kernel's confidence is *earned*. Observability and trustworthiness are first-class kernel organs here, not
  bolt-ons — a sovereign mind that can be watched thinking and trusted exactly as far as it has measured itself.

## Architecture decisions (2026-06-28)

**T_flat is NOT the foundation — the flatten must be fkwu-self-derivable.** `T_flat` is a ~580 KB *bin-go-made*
blob; starting on it carries a Go dependency in the seed and the opaque marker-fragility that tangled the
origin's flatten path. Decision: fkwu flattens `form-flatten.fk` from its own c-bootstrap primitives (or a
minimal flatten baked into `runtime/fkwu-uni.c`), with **no pre-made Go table in the seam**; thereafter any
flatten table is a *regenerable cache fkwu makes itself*, never a committed Go artifact. `T_flat` sits here only
as a flagged crutch (`flatten/README.md`), scheduled for replacement. This is the real test of the restart.

**Minimal walkers; fkwu owns the native path.** The Go/Rust/TS walkers do the minimum — independent
proof-oracles that witness four-way agreement on the *pure-recipe* surface, never feature-bearers. Everything we
natively own lives in / derives from **fkwu**: the JIT (crystallize-on-heat), the host-OS surface (resource
ports, host-io, GPU), and the Form→asm lowering. The walkers never duplicate the JIT or the rich host surface;
they only confirm a recipe computes the same value four ways. Build out fkwu; keep the walkers thin and shrinking.

**OUT (stays in the origin / private repo, never here):**
- The Python app and carriers (~3,400 `.py`), the full Go/Rust/TS kernels, the web/API/substrate-service.
- Private tissue: `memory/`, lineage docs, partner/personal context, anything not for a public commons kernel.

This repo is **public-able by construction** — there are no private parts to excise. That serves the
"commons no one owns" north star directly.

## The parallel discipline — how this work and the origin work stay coherent

The risk of two repos is drift. **One rule kills it: the Form recipe body has exactly one home.**

- Recipes are content-addressed: the same `.fk` interns to the same NodeID on either kernel, so the
  body is naturally shareable — but only if there is no second *copy* to diverge.
- **This repo is the canonical home** for the kernel, the minimal surface, and the recipe body.
  The origin repo **consumes** it (submodule / published package), it does not keep a divergent copy.
- Four-way validation runs in **both**: here, the surface + recipes cross `Go=Rust=TS=fkwu`; the origin
  repo proves the full app-level set against the same recipes.
- Migration is **incremental**: axioms + surface + `form-cli`/`fsh` + core recipes first, proven four-way
  here, then recipe families port as they are needed. The origin repo keeps running the whole time.
- No big-bang. The kernel *earns* each recipe family as it ports — re-proving it four-way on the clean flatten.

## The validation plan — and the one test that decides everything

Every recipe is proven **four-way** (`Go=Rust=TS=fkwu`), executed on the c-bootstrap `fkwu` native,
flattened on `fkwu` self-host with **no bin-go and no clang in the path**.

**First proof (the decider):** one recipe, proven four-way end-to-end on this clean kernel, with the
flatten on fkwu self-host. This tests the single question that justifies the restart — *is the clean
flatten path actually clean?* If a clean axiom-first kernel inherits the `T_flat`/marker ad-hoc-ness,
it is just a second mess. The first proof is where we find out, before moving anything.

## Hard constraint — no bash, no python (the structural gate)

This repo contains **zero `.sh` and zero `.py` files.** Ever. The bash was never the kernel — it
lived only in the origin's orchestration scripts (`validate.sh`, `fourth-arm.sh`, `build-form-cli.sh`).
Here the orchestration itself is **form shell** (`.fsh`, fkwu-native), and the runtime/recipes/walkers
carry no bash or python by their nature (`.c` / `.fk` / `.form` / `.go` / `.rs` / `.ts`).

The one allowed seed is a **single `cc` command** that compiles `runtime/fkwu-uni.c` into the
c-bootstrap `fkwu` binary — a documented one-liner, not a script in the tree. After that, `fkwu` exists
and **`fsh` orchestrates everything** (flatten, run, four-way validate) on the native runtime. Go/Rust/TS
walker builds are likewise invoked from `fsh` via host-exec, for validation only.

A repo gate enforces it: any `.sh` or `.py` landing in the tree fails the gate. (When `fsh` is standing,
the gate is itself an `.fsh` check; until then it is a one-line `find` run by hand at review.)

## Status

- [x] Foundation seeded: five axioms + minimal surface + core (`axioms/`, `surface/`).
- [x] Runtime + flatten brought in, **no bash / no python**: `runtime/fkwu-uni.c` (the c-bootstrap),
      `flatten/` (`form-parse`, `form-flatten`, `fourth-flatten-driver`, `T_flat`). Tree verified `.sh`=0, `.py`=0.
- [x] One `cc` seed → standing `fkwu` binary. **PROVEN 2026-06-28**: `cc -O2 -pthread runtime/fkwu-uni.c -o fkwu`
      builds a native Mach-O arm64 binary in the clean repo and executes — zero Go, zero bin-go, one C seed.
      The sovereign runtime floor is real here.
- [ ] Minimal Go/Rust/TS surface walkers for four-way validation (`.go/.rs/.ts`, no build scripts — `fsh` drives builds).
- [ ] **Orchestration as form shell** (`.fsh`): flatten + run + four-way validate — replacing the bash harness.
- [ ] **First proof: one recipe four-way end-to-end on the clean fsh flatten** ← the decider.
- [ ] `form-cli` + `fsh` native surfaces; origin repo consumes this kernel (one-home enforced).

## What's still missing — the roadmap to a working, self-observing, self-learning native form-cli

The recipes for self-observe, self-learn, and the form-cli largely **exist and cross four-way** (29 learning
cells, 23 form-cli cells, the framebuffer + calibration stack, host-exec, http-client, form-asm). What's missing
is the **live/runtime closure** and the **build** — we have the body and organs; we lack the heartbeat and the
eyes-on-execution:

1. **The keystone — flatten self-derivation (the decider).** Split into two halves by the 2026-06-28 test
   (bin-go hidden, cache cleared):
   - **[x] Recipe-level self-host flatten is Go-free — PROVEN.** A fresh band crosses four-way on fkwu self-host
     flatten with bin-go *not even built*. The all-night "flatten is broken" was a hand-rolled-invocation bug,
     not the kernel — fkwu already owns the recipe flatten. The observe/learn/jit organs can flatten + run native.
   - **[ ] Heavy-chain self-host flatten — the narrowed remaining piece.** The full form-cli / fsh chains report
     `flatten: unavailable` on self-host and fall back to committed (historically Go-made) bootstrap artifacts
     (`form-cli-emitted.c`, `form-cli-table.txt`). The binary *runs* Go-free, but the heavy-chain *build* still
     leans on a committed Go-made-once seed. Closing this — fkwu self-host flattening large chains with no
     committed Go artifact — is the precise remaining decider work.
   - **[ ] Clean-repo orchestration = the `fsh` port — verified 2026-06-29.** In *this* repo, fkwu compiles
     native (Mach-O arm64) and `flatten/fourth-flatten-table.txt` is **byte-identical** (same md5, 580704 bytes)
     to the origin's known-working T_flat. The mechanism is sound; the pieces are present and proven. What rung 1
     needs here is the **orchestration** — the origin's `fourth_flatten_sources` (bash) frames the stdin request
     (`count \n stem \n kind \n nmod \n mods… \n band`), gates on `fourth_selfhost`, pipes through `fkwu T_flat 0`,
     and marker-extracts `==T-stem==…==T-END==`. Reproducing that pipe **by hand** outside the bash harness yields
     an empty table — not a kernel fault (the same empty result appears with the origin's own cached fkwu, so it is
     the request-assembly env, not the binary or table). The self-host flatten is real *inside* `validate.sh`;
     rung 1 in this repo is porting that orchestration to `fsh` so the kernel drives its own flatten with no bash.
     The pieces are ready; the **driver** is the build. (Do not re-attempt by hand — encode it in `fsh`.)
2. **The live RUNTIME witness (self-observe gap).** No cell yet watches execution itself — which recipe fired,
   JIT hit/miss, which cell was touched last by which recipe, what's hot. The framebuffer watches *thoughts*;
   this watches the *running kernel*. The realest new piece. The eyes-on-execution.
3. **The live LEARNING loop closure (self-learn gap).** Rich learning recipes, but the loop that takes real
   runtime outcomes → updates the champion / moves the weights *live* is not wired. `transient-log` +
   `capture-correction` are the seed; the continuous accumulation is missing.
4. **Cognition at native speed — the JIT in the live path.** The hot LLM/RAG cells must crystallize through
   fkwu's self-JIT / form-asm lowering live, not tree-walked. The asm exists; wiring it under live cognition remains.

Load-bearing: **1 (runs natively) + 2 (watches itself run)** are the minimum "core we can observe and trust."
