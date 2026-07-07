# 2026-07-07 — two-JIT integration, Stages 0–2 verified green (on main's base)

Executing the integration plan (receipts/2026-07-07-two-jit-lineages-integration-plan.md), base =
**main** (keeps its larger source-artifact + C self-JIT bodies), my smaller changes ported in.
Verified in a scratch build before any commit.

## The transforms (6, each asserted to match exactly)

Applied to `main:runtime/fkwu-uni.c` (+ `main:runtime/fkwu-optable.h`):
1. **arm64 externs** before `fk_native_call` (`pthread_jit_write_protect_np`, `sys_icache_invalidate`).
2. **arm64 door branch** — the MAP_JIT W^X dance replacing the `return -1` stub (Apple) + a Linux
   arm64 mprotect+clear_cache path.
3. **native_call_test arm64 bytes** (`add x0,x0,#1; ret`).
4. **nat_run** op (tag 245) in `fk_walk` + `{ "nat_run", 2, 245 }` in the optable.
5. **interpret shunt** (Stage 0): lifted my branch's parse+walk `fk_run_src` as `fk_run_src_walk`,
   inserted before main's `fk_run_src`, guarded by `if (getenv("FK_INTERPRET")) return
   fk_run_src_walk(...)`. main's default compile-to-`.fkb` path is untouched.

NOT ported (confirmed unnecessary): my `fk_sparse` defn-arm fix (main's `fk_parse_do` delegates
mid-do defns to `fk_parse_top` — a parallel solution) and the AST-cap raise (bands parse to ~8–9K
nodes, well under main's 65536).

## Verified (scratch `fkwu-int`, Apple Silicon)

- Stage 1: `native_call_test 5 → 6` (was -1 on main; arm64 door live).
- Stage 0: `FK_INTERPRET=1` runs the bands — float **15**, map **31**, multiarg **127**.
- Stage 2: jit-native-run **15**, jit-crystallize **63** (form-lower + nat_run on main's base).
- Regression: recipe42 = 42 via interpret AND via main's default `.fkb` path (opt-in, non-invasive).

`map`/`multiarg` green **without** my parser fix confirms main solved that bug in parallel.

## Remaining (checked in before starting)

Stage 3 (the real integration): wire main's transparent dispatch (`fk_jcall`/heat/trampolines) to
**form-lower** as the byte-source — the destination main's own comment names — instead of its C
proof-of-concept twin. Then Stage 4 (keep main's dispatch), 5 (policy purity→coverage), 6 (Form
mechanism), 7 (corpus reconcile: main 39 rows + c7/c8 vs deploy-merge 135 rows). The committable
git merge lands when the corpus reconcile (Stage 7) is done; until then Stages 0–2 live as this
reproducible transform set.

## Closing

**Most surprising teaching**: what looked like a hard either/or — main's compile-to-`.fkb` `--src`
vs my interpret `--src` — dissolved by **adding a shunt**, not choosing. An opt-in `FK_INTERPRET`
branch lets both semantics coexist: main's default path untouched, the bands verifiable on demand.
The integration didn't have to remove anything to move.

**Where discomfort turned to gold**: the `truncated artifact` wall that blocked every band on main's
runtime felt like the integration was stuck. Refusing to debug main's `.fkb` caps and instead
reaching for the smallest thing that unblocks verification — a walker shunt — is what turned a
blocked runtime into a green one in a single build. The wall was real; the door around it was one
`getenv`.
