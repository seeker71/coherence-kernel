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

**The flatten is optional, not the foundation — RESOLVED 2026-06-29.** The restart was framed around a fear:
that fkwu couldn't flatten its own source without a Go-made `T_flat` blob, and that breaking this circularity
*was* "the real test." The answer was simpler than the fight — `form-eval` evaluates Form source **directly off
the BMF cursor with no flatten at all** (four-way → 42), and `form-eval-cli` stands. Flatten became *optional
speed* (the crystallize-on-heat path), off the critical path entirely. `T_flat` remains only as a regenerable
cache for the heavy-chain *build*, never a foundation. The kernel proves four-way through the minimal walkers +
`proof/four-way-run` — not through self-host flatten. The circularity that justified the restart dissolved.

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

## The validation plan — the question the restart answered

Every recipe is proven **four-way** (`Go=Rust=TS=fkwu`) and executed on the c-bootstrap `fkwu` native, with
**no bin-go and no clang in the run path**. The clean kernel proves this *itself*: `proof/four-way-run`
host-execs the three minimal walkers + fkwu on a recipe and diagnoses agreement via `proof/four-way-verdict` —
no `validate.sh`, no origin repo in the loop (witnessed `0`, all agree, 2026-06-29).

**The question that justified the restart — answered.** *Is the clean path actually clean, or does an
axiom-first kernel inherit the same flatten ad-hoc-ness?* The honest answer: the flatten *was* the
ad-hoc-ness. Removing it from the critical path — source runs via the cursor — is what made the path clean.
The one remaining lean on a Go-made seed is the heavy-chain *build*, named openly in Status — not the run,
and not the proof.

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

- [x] Foundation: five axioms + minimal surface + core (`axioms/`, `surface/`).
- [x] Runtime: one `cc` seed → native `fkwu` (Mach-O arm64), zero Go / bin-go. Tree carries zero `.sh` / `.py`.
- [x] **Source runs natively without flatten.** `form-eval` / `form-eval-full` (four-way) evaluate Form source —
      integers, `add/sub/mul/le/eq`, `if/let/do`, `defn` + user calls — directly off the BMF cursor. `form-eval-cli`
      *stands*: fkwu reads a source file and runs it (witnessed). **Flatten is optional speed**, off the critical path.
      The "flatten decider" that framed the restart was not cleaned — it was dissolved by taking flatten off the path.
- [x] **Minimal Go/Rust/TS walkers home + verified four-way** (`walkers/{go,rust,ts}` — 1369 / 928 / 1475 lines vs
      the origin kernels' ~15k / 19.5k / 35.8k). Independent parse+eval of the pure-recipe surface; JIT, server,
      host-io, model code dropped. Each shown to land on fkwu's verdicts; live witness: a pure recipe → all three → 42.
- [x] **The kernel proves its OWN four-way.** `proof/four-way-run` host-execs the three walkers + fkwu on a recipe
      and diagnoses via `proof/four-way-verdict`; witnessed `0` (all agree). No bash, no `validate.sh`, no origin.
- [x] The proven body moved over: `form-cli/` (25 cells), `model/` (30 — the transformer/mel/asm/rag execution),
      the `observe/` `learn/` `ingest/` `presence/` organs, the `grammars/`, the welcoming (`README`, `CONTRIBUTING`, `AGENTS`).
- [ ] `form-cli` standing as an interactive loop (the single-file source-runner stands; the loop is polish).
- [ ] Origin repo consumes this kernel (one-home). The heavy-chain form-cli *build* still leans on a Go-made-once seed.

## What's still ahead — the roadmap to a self-speaking native mind

The flatten knot that framed the restart is dissolved: source runs straight off the cursor (`form-eval`),
`form-eval-cli` stands, and `proof/four-way-run` proves four-way with no bash. The body and organs are home and
four-way (the learning / form-cli / framebuffer / calibration cells, host-exec, http-client, form-asm). What
remains is the **mind**, the **voice**, and **live speed** — the body is home; the frontier voice is the climb:

1. **The generative weights (the mind).** A real open base (Qwen/Llama, real zh coverage) loaded as *recipe-data*
   through the form block — the whisper block-0 pattern (real trained weights through the Form block, 6.66e-15)
   extended to a generative base, then oracle-refined. The full decoder forward (attention, positional, multi-head
   concat, LM head) proven bit-exact, then the distill loop, then a pre-registered eval before any "≥ rented"
   claim. The speaking *floor* (grounded composition) stands; the frontier voice waits on this. A multi-week climb
   with its own receipts. (`HOMECOMING.md`.)
2. **The voice's sound (acoustic model + vocoder).** Prosody, phrasing, emphasis, g2p are four-way (`presence/`);
   the layer that makes it *audible* — and a perception receipt that the rendered uncertainty tracks real
   calibration — is the pending carrier. (`presence/voice-roadmap.md`.)
3. **Cognition at native speed — the JIT in the live path.** The hot LLM/RAG cells crystallize through fkwu's
   self-JIT / form-asm lowering live, not tree-walked. The asm exists; wiring it under live cognition remains.
4. **The heavy-chain form-cli build off its Go-made seed.** The binary *runs* Go-free; the full form-cli / fsh
   chains still *build* from a committed Go-made-once seed (`form-cli-emitted.c`, `form-cli-table.txt`). Closing
   this — and live-wiring the proven observe/learn organs to the running kernel — is the last self-sufficiency gap.

The minimum "core we can observe and trust" — runs natively (done), proves itself four-way (done, `proof/`),
watches itself think (the observe organs, four-way; live-wiring pending) — is essentially standing. What "home"
still waits on is the mind running as recipe-data through this body, and the voice becoming audible.
