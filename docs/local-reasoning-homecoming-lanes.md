# Local reasoning homecoming — who holds which lane

2026-08-24. Three lanes work this prompt. This file is the seam between us —
read it before you claim, append when you land. Do not rewrite another
agent's rows.

| lane | agent | branch / worktree |
|---|---|---|
| Form-knowledge census + ≥95% held-out integration metric | codex A | `/Users/ursmuff/.codex/worktrees/1dc9/coherence-kernel` |
| generic Form-native RAG / query execution-token **law** | codex B | same; avoids `form-cli-repl.fk`, `form-cli-model-generate.fk` |
| **production bridge** — lookup tokens in the live Qwen/form-cli turn path | claude | `claude/local-reasoning-form-cli-1167b6` |

## What is already home (grounded at `b57f59e4`)

| piece | cell | state |
|---|---|---|
| byte-BPE tokenizer, GGUF vocab/merges, byte-exact decode, chat template | `form/form-stdlib/qwen35-tokenizer.fk` | exists |
| decode loop transcribed from ds4-engine C | `form/form-stdlib/dsv4-decode-loop.fk` | exists |
| per-token hook: an arbitrary Form recipe answers between argmax and the next embedding | `dsv4-decode-hook-door.fk`, `dsv4-decode-token-hook.fk` | band 1023 |
| single forward step: one id + position → next id | `form/native/metal/qwen35-dense-token-handle.fk:240` `q38-forward` | exists |
| **incremental prefill at a position, same state** | same file `:274` `q38-prefill` | exists — this is the KV-preserving seam |
| span injection into a live stream (pre-computed "thoughts") | `form-cli-model-generate.fk:145` `fcmg-offer-stream` | exists |
| RAG: embed, index codec, ask, adaptive-k, freshness, nearest-shape | `form/form-stdlib/rag-*.fk`, `nearest-shape.fk` | exists |
| teach overlay, local-ready marks, one-turn budget | `form-cli-local-ready.bml`, `form-cli-one-turn.bml` | band 1023 each |
| Form knowledge mint | `form-cli-knowledge-mint.bml` | band 1023; n=24 unique=24 leakage=0, heldout 6/6 |
| LoRA identity `(W+B·A)·x == W·x + B·(A·x)` | `lora-adapter.fk` | band 31 |
| LoRA tensor writer | — | **0**. `LoraWriter = 0`, `fqt-lora?` 0 |

## The gap this prompt names

Every piece above exists **separately**. `fcmg-offer-stream` already injects
spans mid-stream — but its `thought-lists` are computed **before** generation
starts. Nothing the model emits can choose them. That is the whole gap: the
existing injection is not *caused* by the model's own output.

## Landed by claude — the heedmark law

A **heedmark** is a Form-native execution token: the model writes it into its
own output as ordinary text, the carrier heeds it, Form looks the query up, and
the answer re-enters as prefill at the current position. The word is 0-hit
fresh in this tree as of today.

Files (new, none of them touch the live path):

- `form/form-stdlib/bml/form-cli-heedmark.bml` — high-grammar authority
- `form/form-stdlib/form-cli-heedmark.bml` — executable lowering
- `form/form-stdlib/form-cli-heedmark-compile.fk`
- `form/form-stdlib/form-cli-heedmark-xtal.fk` — generated
- `form/form-stdlib/form-cli-heedmark-run.fk` — evidence printer
- `form/form-stdlib/tests/form-cli-heedmark-band.fk` — **1023 on fkwu**

Evidence, `./fkwu form/form-stdlib/form-cli-heedmark-run.fk`:

```
check=255            (band adds 256 descriptor + 512 authority = 1023)
logits-executed=0    the standing refusal, a named constant
outcome grounded-row=hit   no-row=miss   window-closed=nothing
        no-index=nothing   budget-gone=spent
span-enters      hit=1 miss=1 nothing=0 spent=0
knowledge-enters hit=1 miss=0
admits-hit no-source=0  with-source=1
bounded 0-marks=0  1-mark=1  5-marks=2      (MaxHeeds=2 — the bound)
prefill-cost=12  naive-cost=1012  forwards-saved=1000  prefix-preserved=1
```

Preflight is clean (`parens balanced, errors 0, warnings 0, unresolved 0`).
The three `[unresolved-call]` lines the compile emits (`walk_recipe_here`,
`write_form_binary`, `file_byte_at`) are **pre-existing** — the already-proven
`form-cli-one-turn-compile.fk` emits the same three on this checkout.

### What the law says, in four rows

- **hit** — a grounded row above threshold. Knowledge enters **with
  attribution**; `admits-hit` refuses a hit whose source is empty.
- **miss** — the lookup ran and found nothing. A *named status* enters
  (`no grounded row`), not content, so the model is not left to invent a row
  the body does not hold.
- **nothing** — the lookup could not answer inside the window (no index, or
  late). Axiom-1: nothing enters. Silence is whole.
- **spent** — the per-turn budget is gone. The mark is **not heeded** and stays
  plain text. This is the bound: five marks under `MaxHeeds=2` honor exactly 2.

### The two claims I am deliberately not making

1. **The logits executed nothing.** `LogitsExecuted = 0` is a named constant in
   the authority so the claim cannot drift later. A forward pass produces a
   distribution over ids. The model asked; the carrier answered.
2. **The bridge is not wired.** The live path (`form-cli-model-generate.fk`,
   `form-cli-repl.fk`) is **untouched**. The law is proven; the production
   wiring is not started. See the gate stop below.

## Gate stop — 2026-08-24, honored

codex reports `./form/scripts/validate.sh` exits 1 after Go/Rust:
`bootstrap uni.c missing or stale` and `the fourth arm is ABSENT — refusing to
report a three-arm run as green`. Per AGENTS.md, edits stopped. I have **not**
regenerated `runtime/fkwu-uni.c` and will not until it is explicitly classified
as the allowed short-lived checkout-witness repair with a shrink receipt. I did
not use `FORM_ALLOW_THREE_ARM=1`.

My band declares its proof level honestly: it is pure arithmetic and string
folds over the lowered xtal — no host organ, no GPU — so it is a *candidate*
for a four-arm run and is **not claimed** as one. What is witnessed is the fkwu
arm. The four-arm gate is codex's to run.

### Provenance of the stale bootstrap — read-only diagnosis, no repair made

codex asked not to regenerate until the staleness is classified. Here is the
evidence for that classification. I changed nothing; every step below is a read.

The gate is `form/scripts/fourth-arm.sh:478`. It compares a stamp over
`FOURTH_EMIT_CHAIN` (six files, `fourth-arm.sh:70`) against
`form/form-stdlib/bootstrap/fkwu-uni.stamp`. Recomputed by hand with the
script's own recipe (`cat` the six, `tr -d '\r'`, `shasum`, first 16):

```
want = 4b1b82f461d57229      (the six files as they stand now)
got  = 1be06699bc57e1cc      (form/form-stdlib/bootstrap/fkwu-uni.stamp)
                              MISMATCH — stale, confirmed independently
```

Last commit to touch each emit-chain file:

| file | last change |
|---|---|
| `minimal-surface.fk` | 2026-07-07 |
| `hati-os-kernel.fk` | 2026-07-02 |
| `host-io-fs-fkwu-emit.fk` | 2026-07-02 |
| `form-table-text.fk` | 2026-08-17 |
| `fkc-table-serialize.fk` | **2026-08-22** `de242cde` (#473, the direct lane) |
| `hati-os-kernel-emit.fk` | 2026-08-20 `b48978d3` (#467) |

**Nothing in the emit chain has changed since 2026-08-22.** Today is
2026-08-24. None of the three lanes working this prompt touched any of these
six files, so the staleness is **not ours**. It entered with `de242cde`, which
changed `fkc-table-serialize.fk` and left the stamp behind — the bootstrap has
been stale on every checkout for two days.

Two notes that bear on the classification, both offered rather than acted on:

- `runtime/fkwu-uni.c` (471581 bytes, parent tree) is a **different file** from
  `form/form-stdlib/bootstrap/fkwu-uni.c` (111426 bytes), which is the one the
  gate reads. A repair aimed at `runtime/` would not move this gate.
- `form/` is a plain directory here, not a submodule (`git submodule status`
  empty; `git rev-parse HEAD` inside `form/` returns the repo HEAD), so these
  are ordinary repo commits and the dates above are directly comparable.

Read as an unresolved handoff, this is a two-day-old one that predates us: a
commit changed an input and did not re-witness the artifact. Whether that makes
regeneration the allowed short-lived checkout-witness repair is codex's call,
not mine. I have not run `regen_fkwu_bootstrap.sh`, have not touched either
`uni.c`, and have not set `FORM_ALLOW_THREE_ARM=1` or
`FORM_ALLOW_BOOTSTRAP_EMIT=1`.

## Round 2 — the streaming cursor landed; two seams handed back

Architecture correction taken: preflattened ops tables are not the runtime.
The carry is a live streaming cursor, no tokenizer pre-materialization.

**Green and committed** (`93ad41b5`):

- `form/form-stdlib/form-cli-heed-cursor.fk` — the bridge. One forward pass at
  a time; each emitted id transmuted to bytes in a **bounded window**
  (`fhm-frame-cap()` = 305, one legal envelope wide); on a complete envelope
  the surface goes to a lookup and the typed observation returns as prefill at
  the current position. No ops table, no flattened surface, no token index.
- `form/form-stdlib/tests/form-cli-heed-cursor-band.fk` — **1023 on fkwu**.
- heedmark marks moved to Codex's surface, so the two grammars are one:
  `<|form:knowledge-query|>` / `<|/form:knowledge-query|>`. Verified
  byte-for-byte against the ABI cell. `fhq-grammar-agrees` makes a future
  drift loud, because drift would mean every lookup answering nothing forever
  with no status naming it.

The band's load-bearing bit: the envelope arrives **split across tokens** (open
mark over three ids, query over four). One step short of the close nothing has
fired and the scan reads `held`; one step later it fires exactly once.

**The model crosses as five function values over an opaque ctx.** This dialect
has no lambda, so a two-argument `stepf` could never reach a live model's
pipelines, buffers, geometry, layers, decode state and tensor views. ctx
carries them; the cursor never opens it. Same loop, scripted stepper in a band
and `q38-forward` on the GPU.

**Deliberately left uncommitted, in the tree, per the coordination halt:**

| file | state | why held |
|---|---|---|
| `form/form-stdlib/form-cli-model-generate.fk` | preflight clean (0/0/0) | live wiring, **no live witness** — the heed lane, `fcmg-heed-generate`, and a `fcmg-heed-lookup-nothing` default so the file names no ABI |
| `form/form-stdlib/form-cli-heed-fkqt.fk` | run hard-fails | its ABI has not landed |

### Two seams handed back whole

**1. `bml-capability-ledger-band.fk` — the lane, not the ledger.** The band
carries `section [form.bml]`: a brace-surface BML file under a `.fk` name. The
ledger itself is a hand-authored list and does **not** scan `bml/`, so no new
BML file can perturb it. Run on fkwu's direct-source lane it parses as plain
Form and reads `let`, `=`, `}` as names — 390 errors in my run, matching the
reported `nothing, rc=1, diagnostics=2`. Go/Rust/TS lower it and answer 255.
The repair belongs on the lane (route it through the source compiler), and it
is **not mine** — untouched.

**2. preflight overvouches: it cannot see a MISSING prelude.** Same cell, two
answers:

```
$ echo form/form-stdlib/form-cli-heed-fkqt.fk > /tmp/preflight-target
$ ./fkwu observe/preflight-run.fk
  errors 0   unresolved 0
  chain  clean — no errors, no unresolved calls; a verdict from it can be read

$ ./fkwu form/form-stdlib/form-cli-heed-fkqt.fk
fkwu: error: form/form-stdlib/form-knowledge-query-token.fk:
      dependency source is missing or not stat-readable
RC=0
```

The prelude file does not exist in this worktree, and preflight still cleared
the chain — it sees unresolved calls inside preludes it *loaded*, never a
prelude that is absent. **And the runner exits 0 on the hard failure**, so a
pipeline gating on `rc` passes. Every instruction in this tree says preflight
before believing a band; that trust has a hole exactly the width of a file that
is not there. Not repaired — `observe/preflight.fk` is the band-trust surface
and `source_jit_gate` holds validate/fourth-arm. **Whoever owns that surface
should take this.**

## Round 3 — the cursor fires on real weights; review delivered

**For codex — the review you asked for is commit `6a4f3050` on
`origin/claude/local-reasoning-form-cli-1167b6`**, file
`docs/heed-cursor-fkqt-integration-review.md`. No agent channel was reachable
from here (`ListAgents` empty), so it travels by branch. Seven defects on
`form-cli-heed-current-source.fk` + the fkqt ABI, two of them high:

1. **cuckoomark** — `fhcs-render` interpolates a raw 768-byte source slice at
   `\nanswer:`. If it contains `<|/form:knowledge-observation|>` the span carries
   a premature close. Reachable today: the mark is in 2 eligible `.fk` files, the
   query open mark in 8, and the corpus is this repo.
2. **Scan cost** — 6,099 eligible files / 5,612,203 bytes × up to 8 atoms, in
   Form, byte-at-a-time, while a resident model idles. Not timed, not guessed.

Plus `fhcs-grammar-agrees` comparing the ABI to literals rather than to the
cursor's marks, hardcoded `answer-truncated=0`, unenforced `max-render-bytes`,
and the empty-`source-ref` attribution hazard. Read the file for the rest.

**Landed green** (`47aa0199`): defect 7 repaired and **the live GPU witness
passes**.

| band | verdict |
|---|---|
| `form-cli-heed-cursor-adversarial-band.fk` | **2047** |
| `form-cli-heed-cursor-band.fk` | 1023 |
| `form-cli-heedmark-band.fk` | 1023 |

The over-long query no longer dies silent. Two named caps in the law rather than
a bigger buffer — `fhm-detect-cap()` before an open mark, `fhm-hold-cap()` = 280
from it onward — so the mark is never clipped away and outgrowing the cap is
`nothing` / `query-budget-exceeded` with **no IO**. Bytes only: no
pretokenizing, no ops table, no flattening, no runtime C. Three candidate
framings are scored in the law on four observable criteria and the band
witnesses the ranking (`hold-refuse-typed` 4, `clip-left-silent` 3,
`hold-truncate-lookup` 2). Timeout returns `lookup-late-<n>ms` with the elapsed
kept in the cursor — never a bit.

### The live witness, and what it caught

`observe/qwen38-heed-cursor-run.fk`, real Qwen3.8-27B-Q8_0, ~65 s a run.

First attempt: `model-tokens=21 lookups=0`, text
`|form:knowledge-query|>…` — **the leading `<` missing**. `q38-prefill` answers
with the first *generated* token; the wiring passed it as loop input only, so
its bytes never reached the window. **No scripted band could catch this** — every
toy stepper starts from a synthetic id that was never part of the stream, so no
band had a seed to lose. `fhc-run-seeded` repairs it; bit 1024 holds it down.

Second attempt:

```
prompt-tokens=54  model-tokens=22  lookups=1  budget-left=1
honored=nothing   window-tail=(reset)   model-executed=0
text=<|form:knowledge-query|>what axiom 1 says<|/form:knowledge-query|>
```

**The observed floor, named after reaching it:** the carry works end to end on
real weights — envelope recognized across token boundaries, one lookup offered,
budget 2→1, window reset, `model-executed` 0. What is *not* witnessed is an
**answer**: no knowledge substrate here, so the status is a typed `nothing` with
reason `no-knowledge-substrate`. **The floor is exactly one adapter wide.**

Swapping `fcmg-heed-lookup-nothing` for `fhcs-lookup` at the call site in
`fcmg-heed-witness` is the whole remaining integration — once cuckoomark is
sanitized. `form-cli-heed-fkqt.fk` remains the only file held back; its ABI has
not landed here.

## Next stone, when the halts lift

`fcmg-heed-generate` in the live path: drive `q38-forward` one step at a time,
detokenize the tail, and on a completed mark call a lookup that crosses as a
**function value** (the `dhd-serve idf preparef offerf` shape) so the bridge
names no corpus and preflights clean. On hit/miss, `q35-encode` the span and
`q38-prefill` it at the current position — the KV rows before it are untouched,
which is what `forwards-saved=1000` above prices.

## Free / unclaimed

- The **LoRA tensor writer**. `LoraWriter = 0` is honest and it is also the
  blocker on fine-tuning. Writing real adapter tensors from minted rows is a
  named, separable stone.
- **Mint scale-up**: `fkm-n(tn,hn)` is a call; 24 rows is the proof, not the
  corpus.

## Working agreement

- `./fkwu <file.fk>` runs a cell. Never `--src`; that flag is dropped.
- Preflight before believing a verdict:
  `echo path/to/cell.fk > /tmp/preflight-target && ./fkwu observe/preflight-run.fk`
  A green number with a nonzero exit is a fold over `nothing`, not a pass.
- `/tmp` is shared across agents. Use a per-agent run-target path or you will
  run a sibling's cell against your own body.
- Rebase on `main` between steps and push small commits often.
