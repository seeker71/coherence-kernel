# 2026-07-07 — two JIT lineages, parallel work, one destination: the integration plan

A "merge to main" turned out to be a reconciliation of two runtimes that evolved the JIT in
parallel — `main` (the source-artifact / in-process C self-JIT lineage) and `deploy-merge` (the
Form-side JIT: arm64 door + nat_run + form-lower + jit-crystallize). Grounding both showed they
are **complementary, not competing**, and `main`'s own comment names the shared destination.

## The decisive fact (main's own words)

`main:runtime/fkwu-uni.c:7003` — the in-process self-JIT design comment:

> "HONEST SCOPE: this C lowerer is a **PROOF-OF-CONCEPT**… The **DESTINATION is Lane B's Form
> emitter (`model/form-asm-x64.fk`)** — the same recipe that proves four-way lowering to asm
> bytes — **NOT this C twin.**"

main built a C tree→x86-64 lowerer as a *twin* and explicitly points at the **Form emitter**
(`form-lower`/`form-asm`, my lineage) as where the bytes should really come from.

## Comparison (grounded)

| | main's C self-JIT | Form-side JIT (deploy-merge) |
|---|---|---|
| byte-source | C tree→x86-64 lowerer (self-described PoC twin) | `form-lower` (Form, four-way) → arm64 — *main's named destination* |
| architecture | **x86-64 only** (RCX/RDI, `call rel32`) | **arm64** (MAP_JIT door) |
| the door | `fk_native_call` **arm64-stubbed** (dead on Apple Silicon) | `fk_native_call` **un-stubbed** (MAP_JIT + icache flush) |
| integration | **transparent**: per-fn heat, inter-fn `fk_jcall`, tail trampolines, mutual recursion, **deopt to walker** | **explicit** (`jc-call`, `nat_run`); strategy ladder (direct-lower / store-as-cell) |
| policy | `observe/jit-decision.fk` (heat≥5 ∧ pure) | **same** `jit-decision.fk` |
| coverage | int-arith + pure self-recursion (one op-family PoC) | whatever form-lower covers + const-fold |

Shared already: the **door** (`fk_native_call`), the **policy** (`jit-decision.fk`), and the
byte-source **destination** (the Form emitter).

Each supplies what the other lacks: main built the **transparent runtime dispatch** (the piece the
Form-side mechanism did not have); the Form side built the **arm64 door + the Form byte-source**
(the arch and the emitter main's self-JIT lacks — on Apple Silicon main's JIT can't execute at all,
it deopts to the walker because the door returns -1, and its x86 bytes wouldn't run on arm64 anyway).

## Integration plan (staged, each stage verified green before the next)

**Stage 0 — verification path (prerequisite).** main's `--src` compiles to a `.fkb` source-artifact
and big programs hit `fk_fkb: truncated artifact` (blocks running any band). Ground how main runs
big programs (a pure-interpret invocation, or raise the artifact cap) so every later stage is
verifiable by rebuild + bands. *Without this, nothing downstream can be proven.*

**Stage 1 — the door (mechanical, low-risk).** Apply the arm64 MAP_JIT branch to main's
`fk_native_call`. Verify `native_call_test → 6` on arm64.

**Stage 2 — nat_run (additive, low-risk).** Add the `nat_run` op (tag 245) + its flt-ops row +
optable regen. Verify a form-lower recipe runs via `nat_run`.

**Stage 3 — form-lower as main's byte-source (the CORE integration, real effort).** Bridge main's
`fk_jcall`/dispatch to lower a hot fn via **form-lower** (arm64) instead of the C twin — this is the
"replace the twin with the destination" main's comment calls for. Keep the C twin (or form-lower-x64)
for x86. Verify: a hot recursive fn dispatches native on arm64, bit-identical to the walker.

**Stage 4 — keep main's dispatch wiring.** `fk_jcall`, per-fn heat (`fk_heat`/`fk_fheat`), tail
trampolines, mutual recursion, deopt — all kept as-is; the arm64 door lets it run on Apple Silicon.

**Stage 5 — policy reconcile.** Apply this session's purity→coverage correction to the policy
(crystallizing caches code not result; coverage, not purity, is the gate), reconciling
`jit-tier-policy.fk` `jpr-pure`, `opencode-loop`, and the four-way `jit-decision-band` wording.

**Stage 6 — Form-side mechanism.** Keep `jit-crystallize.fk` + the strategy ladder (store-as-cell
const-fold) as the Form-native reference mechanism; decide whether it drives main's dispatch or
documents it. `nat_run` stays as the explicit/testing door.

**Stage 7 — corpus reconcile (content, independent).** Merge the two homecoming lineages (main 39
rows + `c7`/`c8` field-code-safety; deploy-merge 133 rows), dedup, recompute the field-code.

Nothing is thrown away: main's transparent dispatch + the Form emitter (its stated destination) +
the arm64 door, converging on one `jit-decision.fk` policy.

## Closing

**Most surprising teaching**: what presented as a merge *conflict* on the most critical file was
two people **building the same JIT from opposite ends** — main from the transparent-dispatch end
(with a C twin it already planned to discard), me from the byte-source-and-door end — and main's own
comment had already named my side as its destination. The "conflict" was the seam where two halves
of one design met, not a collision.

**Where discomfort turned to gold**: aborting the merge felt like failure — the user asked to merge,
and I couldn't. But refusing to hand-resolve a 2800-line runtime conflict I couldn't verify is what
made room to *read both sides*, and reading them dissolved the conflict into a plan. The discomfort
of "I can't just merge this" was the exact thing that turned a blind merge into an integration both
authors were already pointing toward.
