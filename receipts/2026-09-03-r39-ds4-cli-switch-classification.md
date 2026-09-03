# R39 — FORM_DS4_* / FORM_CLI_* switch classification

Read-only pass, zero files touched. Full grep census plus a line-by-line read
of every usage site closed the ledger's R39 row honestly: the raw name-grep
count itself was inflated on both families, for different reasons.

## The headline

**DS4:** of 70 grepped names, `FORM_DS4_PAD` is a ghost — the switch was
measured and physically removed (`receipts/2026-08-03-darkbranch.md`); the
name survives only inside a historical comment citation. 69 names are live.

**CLI:** of 27 grepped names, only 5 are real caller-facing environment
switches (`FORM_CLI_FORCE_LINK`, `FORM_CLI_EXTRA_SRC`,
`FORM_CLI_EXTRA_LDFLAGS`, `FORM_CLI_CANONICAL_PUBLISH`,
`FORM_CLI_RETAIN_WORKDIR`). The other 22 are internal shell arrays, a
sed-substituted placeholder token, an attestation-file parser's field names,
and a positional-argument name in a usage string — never read via
`${NAME:-default}` from the calling environment. A name matching
`FORM_(DS4|CLI)_[A-Z0-9_]+` is not proof it is a live switch; the real test
is whether it is ever read as a caller-settable default.

## FORM_DS4_* (69 live names, `metal_dsv4_stack.sh` unless noted)

**Already self-evidenced (measured, cited, no fix needed):** `CONCURRENT`,
`FPCONTRACT`, `GATES`, `HC_SPLIT_PAR`, `IQ2_TG`, `MOE_PAIR`, `MOE_SUM`,
`Q2K_FAST`, `Q80_METAL_ORDER`, `Q80_TG`, `SHARED_AFTER_ROUTE`, `TG`,
`ATT_STATS_SG` — 12 switches, each with a measured number in the adjacent
comment.

**Real decision, no evidence for this specific number (left unfixed, per the
task's own instruction not to invent a default):** `F16_CHAIN8`,
`HASH_SPLIT`, `HC_SPLIT_DEFER`, `IQ2_FAST`, `IQ2_ROWS4`, `IQ2_SPAN`,
`NO_FUSE`, `Q2K_ONE_THREAD`, `Q2K_R4FAST`, `Q2K_ROWS4`, `Q80G_WIDE`,
`Q8A_PAR`, `TG_SMALL`, `TOPK_TG`, `TOPK_TG6`, `SUBMIT_EVERY` (the cleanest
true gap — see R47).

**Real decision, unresolved cross-file tension:**
- `FORM_DS4_BLOB` — which GGUF answers. `metal_dsv4_stack.sh:66-71` already
  reasons a preferred-symlink-then-fallback default, citing a real
  2026-08-04 regression. But `form/form-stdlib/model-discovery.fk:3-15` — a
  later, still-live file — argues this exact switch "is not [a runtime
  switch]... and it goes," proposing a `models`/`use <n>` runtime-discovery
  replacement that already exists and is compiled into form-cli, never
  wired into the DS4 shell harness. Two of the body's own past decisions
  disagree and neither has been reconciled (see R45).
- All 6 earlier "Stone" files (`metal_dsv4_forward.sh`,
  `metal_dsv4_layer.sh`, `metal_dsv4_layer_join.sh`, `metal_iq2_gpu.sh`,
  `metal_mx_gpu.sh`, `metal_windowed_residency.sh`) default `FORM_DS4_BLOB`
  straight to the specimen path with no preferred-symlink tier — all 6 are
  otherwise dead (no caller anywhere in the tree; referenced only in prose).
- `FORM_DS4_Q80_METAL_ORDER` — a live, numerically-measured proposal
  (7.7e-04 vs 1.1e-03 delta, 55 vs 59ms) to re-pin the reference kernel sits
  unapplied by deliberate policy ("a decision to be made in the open") —
  see R46.

**Build/run parameters** (paths, ids, counts, prompts — legitimate
per-invocation config, not decisions): `BLOB`(-override),
`CONTROL_ADAPTER`, `DS4_KV_CAP`, `KV_SEQUENCE`, `KV_STEPS`, `ORACLE_DIR0`,
`ORACLE_DIR7`, `PERTURB_DIR0`, `PERTURB_DIR7`, `PROMPT`, `PROMPT_IDS`,
`PROMPT_TOKEN`, `SAMPLE`, `SEQ_IDS`, `SITUATION`, `STACK_LAYERS` (default =
the file's own block_count), `TOKENIZER`, `TOKEN_HOOK_DOOR` (opt-in;
unset = byte-identical to before it existed), `CONTRACT_ONLY`.

**Diagnostic/measurement instruments** (self-described as never-a-setting,
ablation-only, or a refuted falsifier kept as a negative control): `BW`,
`CENSUS`, `DOUBLE`, `DUMP_LOGITS`, `HOSTSHARE`, `IQ2_ALU` ("answers WRONG on
purpose"), `NO_COMPRESSOR`, `PROFILE`, `Q80_AB`, `RAW_LANE` ("REFUTED
2026-07-30, kept only as a falsifier"), `REF_IDS`, `REF_LOGITS`,
`REF_LOGITS_B`, `SKIP`, `STAGE_BLK0`, `TRACE_HC`.

**Already Form-visible:**

| Name | Where | Evidence |
|---|---|---|
| `MATCH_ORDER` | `ask-ds4.fk:95` `ad-needs-match-order? () 1` | production door always forces it on; shell default only matters for standalone perf runs |
| `TOKEN_BUDGET_MS` | `dsv4-token-recipe-swap.fk:36` (34) | cites two receipts (2026-07-31, 2026-08-01); shell default independently matches |
| `DEFAULT_TOKEN_BYTES` | same file:38 (9103000000) | same two receipts; exported but never read downstream — orphaned (R48) |
| `METAL_BANDWIDTH_BYTES_PER_MS` | same file:37 (471000000) | same two receipts; also orphaned (R48) |
| `TOKEN_SWAP_CELL` | `ask-ds4.fk:66` | Form-computed; also orphaned (R48) |

**Not a live switch:** `PAD` (removed instrument, ghost citation only).

## FORM_CLI_* — 5 real switches

| Name | Reason | File:Line |
|---|---|---|
| `CANONICAL_PUBLISH` | refuses caller extra-src/ldflags during official publish, covered by a dedicated regression test | `build-form-cli.sh:35` |
| `EXTRA_SRC` | extra source files for local/custom builds | `build-form-cli.sh:25,49,62` |
| `EXTRA_LDFLAGS` | extra linker flags, same auto-append behavior | `build-form-cli.sh:26,50,63` |
| `FORCE_LINK` | bypass the snapshot fast path, force relink; default 0 | `build-form-cli.sh:24` |
| `RETAIN_WORKDIR` | keep temp workdir after a failed maintainer regen; default 0 | `regen_form_cli_bootstrap.sh:41` |

The other 22 grepped `FORM_CLI_*` names are naming-convention false
positives: 12 `FORM_CLI_GENERATION_*` fields populated by parsing an
attestation file, never read from the process environment; 4 fixed/computed
source-list arrays; 2 internal publish-lock state variables read only by
their own release function; 2 shadow copies used only for the
`CANONICAL_PUBLISH` refusal check; 1 sed-substituted placeholder token; 1
positional-argument name in a usage string.

## Ranked candidates for a future dedicated pass

1. `FORM_DS4_BLOB` (R45) — the replacement is already designed and compiled
   in; just never wired to this harness. Highest leverage.
2. `FORM_DS4_Q80_METAL_ORDER` (R46) — a measured proposal awaiting an actual
   team decision, not a silent default.
3. `FORM_DS4_SUBMIT_EVERY` (R47) — the one clean no-evidence gap; a single
   A/B run would settle it.
4. `DEFAULT_TOKEN_BYTES` / `METAL_BANDWIDTH_BYTES_PER_MS` / `TOKEN_SWAP_CELL`
   (R48) — three receipt-cited values exported into the environment that
   nothing downstream reads; wire a consumer or stop exporting them.

Signed, a sibling session in Sema's worktree, 2026-09-03.

Most surprising teaching: grepping for a `FORM_(DS4|CLI)_[A-Z]+`-shaped
name is not the same claim as finding a switch — three-quarters of the CLI
census and one DS4 name were naming-convention lookalikes, and the real
decision surface was far smaller and far better-evidenced than the raw
count implied.

Where discomfort turned to gold: `FORM_DS4_BLOB` read at first like an
ordinary unfinished tuning knob worth flagging as "needs a default."
Reading one file further (`model-discovery.fk`) turned that into the
sharpest finding of the pass — the body had already had this argument with
itself, already built the replacement, and simply never crossed the wire
from one file to the other.

; witnessed: 2026-09-03 -> read-only classification, zero edits
