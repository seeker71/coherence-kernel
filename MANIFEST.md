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
- [ ] One `cc` seed → standing `fkwu` binary (documented one-liner).
- [ ] Minimal Go/Rust/TS surface walkers for four-way validation (`.go/.rs/.ts`, no build scripts — `fsh` drives builds).
- [ ] **Orchestration as form shell** (`.fsh`): flatten + run + four-way validate — replacing the bash harness.
- [ ] **First proof: one recipe four-way end-to-end on the clean fsh flatten** ← the decider.
- [ ] `form-cli` + `fsh` native surfaces; origin repo consumes this kernel (one-home enforced).
