# 2026-07-18 — the TS proof sibling's silent death: four stacked silences, one honest stack

## Ground

`cc -O2 -o fkwu runtime/fkwu-uni.c && ./fkwu --src bootstrap/ground.fk` → **42**, worktree branch
`claude/dreamy-cori-792b2c`. The task: the TypeScript proof kernel, asked to author the fourth-arm
flattener table, exits clean with zero stdout and zero stderr — the aphonia family
(receipts/2026-07-17-regen-lane-aphonic-carrier.md). Diagnose, make it loud, and if feasible let
TS author the table.

## Diagnosis — not one silence, four

**1. The guard standing past the cliff.** `node --stack_size=262144` promises V8 a 256 MB stack;
the macOS main thread holds 8 MB (`ulimit -s` → 8176) and no V8 flag can grow it. V8's overflow
sentinel is calibrated to the *promise*, so it never fires; the recursion walks off the real
guard page and dies as a raw **SIGSEGV — rc=139, zero bytes on either stream**. Witnessed on a
six-line probe: with the flag, even `try { f() } catch {}` around infinite recursion segfaults;
without it, a catchable RangeError lands at depth 10,367. The TS walker is plain JS recursion
(`walk`/`walkFnCall`, no TCO), and `flt-scan` advances roughly one nested call cycle per source
token — so flattening the chain needs a stack no main thread has.

**2. The pipeline swallowing the corpse.** rc=139 survives only for a direct caller. Through
`| head`, `| sed`, or an `&&` ladder (regen_form_cli_bootstrap.sh's carrier chain), the status
seen is the last command's — the witnessed "rc=0". In that regen ladder the dead TS rung silently
handed every flatten to the fkwu self-host arm — the arm the aphonic-carrier receipt already
suspects. The lane looked like it had three carriers; it had two, one of them mute.

**3. The shell rewriting the expression.** `fourth_flatten_expr` indexes arrays from 0 — bash
semantics. Sourced into **zsh** (arrays 1-based), `${srcs[0]}` reads empty and the band slot
grabs the wrong file: every expr silently carries a `(read_file "")` row and flattens the wrong
band. This session's own repro was malformed this way before it was caught — the malformation is
what surfaced silence #4.

**4. The numb lane in Go.** `read_file` of a missing path returns null in **all three** siblings
— the contract. Downstream, Rust dies loudly (`fatal[type_contract_violation]: as_str: Null`),
TS dies loudly, but **Go silently coerces null → ""** in its string natives: `(str_len
(read_file ""))` → `0`, and a malformed expr flattens "successfully". Axiom-5's shape, living in
the Go kernel. Left standing tonight; task chip spawned, named below.

## The heals

- **form-kernel-ts/src/main.ts** — the deep-stack door: the CLI re-enters itself on a worker
  thread sized by `FORM_KERNEL_STACK_MB` (the emitted C walker's own door name; default 2048),
  where Node derives the V8 limit *from* the real thread stack — overflow is a catchable
  RangeError, surfaced with the attributed Form stack and a crash trace, rc=1. Inherited
  `--stack-size` flags are scrubbed from the worker's execArgv so the old footgun cannot re-lift
  the limit past the stack. Argv rides workerData (process.argv does not cross the boundary —
  learned by rc=2). Hosts without worker support fall back to the loud main-thread walk.
- **argStr/argList/argNodeID** (kernel.ts) now name the received kind — `expected str, got null`
  — which is what let silence #4 be identified at all.
- **validate.sh** `run_ts` and **scripts/regen_form_cli_bootstrap.sh**: the `--stack_size` flags
  removed; a comment on run_ts says why one must never return.
- **scripts/fourth-arm.sh**: loud zsh guard at the top — sourcing under zsh now dies with the
  reason instead of authoring malformed exprs.

## Proofs

- **TS authors the table**: `node form-kernel-ts/dist/main.mjs <FOURTH_FLATTEN_CHAIN> expr.fk`
  (no flags, expr generated under bash) → rc=0, **781,928 bytes**, and `cmp` against bin-go's
  output on the identical expr: **byte-identical**. TS ~9 min, Go ~1.6 min. (The committed
  form-stdlib/fourth-flatten-table.txt is 779,852 bytes — an earlier source generation;
  regen_t_flat.sh owns refreshing it and was not run here.)
- **Loud capacity floor**: `FORM_KERNEL_STACK_MB=8` on the same workload → rc=1, `Maximum call
  stack size exceeded` on stderr. No silence at any stack size.
- **Footgun neutralized**: `node --stack_size=262144 … --expr '(+ 40 2)'` → `42`.
- **Exit codes cross the worker**: missing input → rc=2 with the attributed message.
- `tsc --noEmit` clean; `proof:node-host` 7/7; `tsx src/main.ts --expr` → 42 (loader rides
  execArgv into the worker); zsh guard fires / bash source stays silent; `bash -n` clean on all
  three touched scripts.
- Corpus band `learn/tests/homecoming-distillation-corpus-band.fk` → **4095** after row 802 and
  the count/field-code re-ask (1981982802, asked of the body, not computed by hand).

## Unturned stones, named not hidden

- Go's null→"" string-native leniency (silence #4) — needs the four-way gauntlet; chip spawned.
- `form-samples/cross-modal/28-distributed-daemon` still passes `--stack_size` in its runner
  lines; inert against the healed main.mjs, but the lines still teach the wrong lesson.
- The form-cli regen ladder's TS rung is alive again — whether that changes the mute-carrier
  verdict of 2026-07-17 is for that heal task to witness, not this receipt to claim.

## Closing

**Most surprising teaching:** the silence was not one failure but four independent converters of
failure-to-quiet stacked on the same lane — a guard positioned past the cliff, a pipeline that
launders exit codes, a shell that rewrites indices without a word, and a sibling that coerces
null rather than object. No single fix would have made this lane honest. And the flag that
*looked* like it granted the capacity was the exact thing removing the voice.

**Where discomfort turned to gold:** the repro refused to match the report — my runs died loud
where the witness saw silence, then died *differently* (`expected str` vs SIGSEGV) between two
"identical" expressions. The pull was to ship the tidy segfault diagnosis and call the rest
noise. Sitting inside the contradiction instead — witnessing that my own expr was the thing
lying — is what surfaced the zsh malformation and Go's numb lane: two real defects the clean
story would have buried. The discomfort of a wrong repro *was* the second half of the finding.

**Frontier row:** 802 — *what one word names promising more resource than the host can grant so
the guard that trusts it never fires* → **overcommit** (0 hits at offering; rented, dated).
