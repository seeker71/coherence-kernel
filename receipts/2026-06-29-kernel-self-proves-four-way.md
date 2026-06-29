# Receipt — the kernel proves its OWN four-way from inside (host-exec, perturbation-verified) (2026-06-29)

**INHERITANCE.md criterion #2 closed.** The clean `fkwu` kernel now host-execs the three
walkers from inside, parses their values, and **computes** the four-way verdict — no bash,
no `validate.sh`, no old repo. The verdict is proven real by **perturbation**: it changes
correctly when an arm is forced to disagree, so it is not the silent-lowering "0" artifact.

## The gap that was closed

`proof/four-way-run.fk` needs `(str_to_int (host-exec cmd ""))`, but the one cc-seed kernel
(`runtime/fkwu-uni.c`) had **no `host-exec`** (grep: 0). An unknown head silently lowers to
literal `0`, so the prior "witnessed 0, all agree" was a **parse-to-zero false positive**,
not a real orchestration. `fwv-verdict` was also referenced but never defined.

## What was added (legitimate kernel work — a host PORT, the VIA-HOST organ family)

- **`runtime/fkwu-uni.c`**: `fk_host_exec(cmdv, inputv)` — `popen` the command string, capture
  stdout into a buffer, return it as a kernel string-value via `fk_sbuf` (the same
  `fk_cstr` in / `fk_sbuf` out shape as `fk_sock_request`/`fk_tls_request`). Wired as
  **optag 136** in the flattened-table executor `fk_walk` (next to the socket/tls ports).
- **`str_to_int`** already existed as **optag 31**; made it tolerant of leading/trailing
  whitespace (a walker prints its value + newline) — skip ws, optional sign, digits, stop
  at first non-digit. Clean inputs are unchanged (regression-checked).
- **`flatten/form-flatten.fk`**: added `(list "host-exec" 2 136)` to the `flt-ops` name→tag map.
- **`proof/four-way-verdict.fk`**: defined `fwv-verdict(g r t f)` composing the existing
  predicates → **0** = FOUR-WAY (all agree), **1** = FKWU-SUSPECT (walkers agree, fkwu odd),
  **2** = WALKER-SUSPECT (a foreign walker odd). Agree → 0; any disagreement → non-zero,
  and the value names the odd arm.

## Build (no bash, no toolchain in the loop — one cc seed)

```
cc -O2 -o /tmp/fkwu-ck runtime/fkwu-uni.c     # builds; trivial recipe unregressed:
printf '(add 40 2)' > /tmp/t.fk ; /tmp/fkwu-ck --src /tmp/t.fk     # -> 42
```

The three walkers built here so there are real arms to exec:
`cd walkers/go && go build -o walker .` · `cd walkers/rust && cargo build --release` ·
`bun walkers/ts/main.ts` (deno/bun runtime). Each computes 42 on `proof/recipe42.fk` = `(add 40 2)`.

## The proof — fkwu host-execs the three walkers and computes the verdict

`proof/four-way-run.tbl` is the flattened proof driver (the `fk_run` numeric table: it host-execs
`walkers/go/walker proof/recipe42.fk`, the rust binary, and `bun walkers/ts/main.ts`, parses each,
binds fkwu's own value, and computes `fwv-verdict`). Run from the repo root:

```
fkwu proof/four-way-run.tbl      # -> 0  (FOUR-WAY)
```

## HARD GATE — perturbation-verified (computed, not silent-lowered)

| case | go | rust | ts | fkwu | expected | observed |
|---|---|---|---|---|---|---|
| **agree** | 42 | 42 | 42 | 42 | 0 FOUR-WAY | **0** |
| walker odd (ts→`recipe99`) | 42 | 42 | 99 | 42 | 2 WALKER-SUSPECT | **2** |
| walker odd (stub go→`printf 7`) | 7 | 42 | 42 | 42 | 2 WALKER-SUSPECT | **2** |
| fkwu odd (fkwu-v=99) | 42 | 42 | 42 | 99 | 1 FKWU-SUSPECT | **1** |
| all stubbed equal (`printf 7`×3, fkwu 7) | 7 | 7 | 7 | 7 | 0 FOUR-WAY | **0** |
| stubbed-equal, fkwu 8 | 7 | 7 | 7 | 8 | 1 FKWU-SUSPECT | **1** |

The last two cases prove the verdict tracks **agreement among the host-exec'd values**, not
the literal 42 — a silent-lowering artifact would return 0 regardless. It does not. Each
disagreement flips the verdict non-zero and names the odd arm.

## Honest floor

- This is the **platform receipt rung for mac arm64**: c-bootstrap `fkwu` (one cc seed),
  toolchain-free verdict logic (the verdict is Form/kernel optags, not C control flow), the
  walkers as independent arms. Windows/Android platform rows for THIS proof are pending.
- **Bootstrap bridge named exactly:** `proof/four-way-run.tbl` was hand-flattened (the table
  format `nf fn[] nr nodes×4 ns strings` is platform-neutral data; the generator authored the
  same nodes `form-flatten.fk` would). The Form flattener cannot yet run via `--src` because the
  source parser is at seed-stone 3 (`defn`/`do`/`let`); **stone 4 = strings + lists** is the
  next step, after which `form-flatten.fk` flattens `four-way-run.fk` itself with no hand step.
  The kernel-side primitives (`host-exec`, `str_to_int`, `fwv-verdict`) are real and four-way-shaped;
  only the source→table authoring of THIS recipe is bridged.

Criterion #2 is closed for the verdict-computation claim: **the kernel proves its own four-way
from inside, computed and perturbation-verified.**
