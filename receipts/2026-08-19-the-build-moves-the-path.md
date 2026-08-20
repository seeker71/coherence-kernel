# The build moves the path — and the built body cannot hash

Date: 2026-08-19, Hati Suci. Apple M4 Max, 128 GiB unified memory, 40 GPU cores.
Worktree `.claude/worktrees/jit-lane-performance-d77568`, branch `claude/jit-lane-performance-d77568`.
Continues [oMLX's lessons land in the standard form-cli path](2026-08-19-omlx-in-the-standard-path.md),
which closed with the published binary named as behind the source. Urs asked for it built, and for
the moving watched.

## Built, in the documented order

`form/scripts/regen_form_cli_bootstrap.sh` (read in full first — 334 lines; it flattens, runs a
ping→pong canary, emits C, verifies stamp/digest/attestation, publishes five files and nothing else),
then `form/build-form-cli.sh`.

The regeneration **skips its own voice canary on this host** — *"WARNING voice canary skipped (no
cached fkwu)"*. Every canary below is one I ran against the built binary myself, because the
publisher could not.

## The first build moved the path, and showed why it could not move it far

| | committed binary | rebuilt from source |
|---|---|---|
| `ping` | pong | pong |
| `models <dir>` | **3** — including a `.incomplete` HF cache file and a `.sparse-incomplete` decoy | **1** — only the sealed GGUF |
| after `use` | `now: route <request>` | `now: generate <prompt> \| model-bandwidth \| route <request>` |
| `generate …` | *"I don't speak free sentences yet"* | **`cell`, in 0.035 s** |

`cell` in 35 milliseconds is the whole finding: no model opened, no seal read, no GPU. The verb was
reachable and its door was not there. [`form-cli-repl.fk:16`](../form/form-stdlib/form-cli-repl.fk:16)
names `form-stdlib/form-cli-model-generate.fk` in its `; preludes:`, the flatten takes explicit module
texts and resolves no preludes, and **no build list carried that cell**. `fcmg-generate-resident`
resolved to nothing and axiom 5 answered with a shrug. The published form-cli has never been able to
generate; the regeneration script's own comment warns about exactly this shape, and the canary that
might have caught a mute carrier is the one skipped here.

## Six lists, one program

The generate door's prelude closure is 39 cells; 28 were absent. They were mirrored, in dependency
order, into every list that must agree — `scripts/form_cli_source_list.sh` (stamp identity, 21
additions: seven were already stamped but not programmed), `regen_form_cli_bootstrap.sh`'s
`FORM_CLI_SELFHOST_ORDER` and `$form_modules`, and `build-form-cli.sh`'s `MODS` and
`FORM_CLI_SELFHOST_SRCS`. All three scripts parse; every added cell is plain Form and ends inert, so
nothing new fires at startup.

The program grew as the count says it should: **functions 1989 → 2994**, nodes 62,085 → 97,130,
emitted C 1,110,132 → 1,831,833 bytes, binary 2,937,048 → 3,977,304 bytes. The flatten cost went from
1m45s to 4m25s.

## What the built binary then said

```
generation refused: selected artifact has no current Form-native SHA-256 seal
```

The seal is current. The door now RUNS — it reaches artifact admission and spends about twenty
seconds hashing — and refuses on the result. So the refusal was made to name which half failed
(`qaf-seal-verdict`: 0 seal absent/mismatched, 1 verified, 2 the engine did not answer), and the CLI
was given `seal-check <path>`, an instrument that shows both halves at once. Same instrument, both
kernels, same machine, same file:

| | source runner `./fkwu` | built `form-cli` |
|---|---|---|
| `seal_valid` | 1 | 1 |
| `saj_code_len` | 580 | **580** |
| `initial_state` | `67e6096a85ae67bb…` | **identical** |
| `computed` (29 GB) | `a680f44a…67e348` ✓ | **`0000…0000`** |
| `abc_computed` (3 bytes) | `ba7816bf…0015ad` ✓ | **`0000…0000`** |
| buffers at exit | 0, free_slots 3 | **72, free_slots 0** |
| cpu_jit_dispatch / busy | 2 / 14.7 s | 10 / **22.4 s** |

The three-byte known-answer test is the one that settles it: **this is not about a 29 GB file.** The
emitted carrier runs the same 580 bytes of Form-authored ARM64, over the same initial state, for
twenty-two real seconds, and returns thirty-two zero bytes. And it frees no buffer at all where the
source runner frees every one. Two divergences in one door, between two kernels of the same body,
from one source — a bug, not a design, now reproducible in one command:

```
printf 'seal-check /path/to/model.gguf\nquit\n' | form/form-cli
```

Not chased further tonight, and not papered over: the refusal that sent me hunting is now a sentence
that names the instrument, and the instrument is in the binary for whoever picks it up.

## Also witnessed, unasked

A `let` bound once and read twice ran its right-hand side more than once: the instrument's first
version spent 25 s and **six** engine passes to print one hash. Crossing the digest as a function
ARGUMENT instead is what the cell does now. The old warning that a single-use `let` cannot testify has
a sibling: a twice-used `let` may not be one binding either.

## What ran

```
form/form-stdlib/tests/form-cli-resident-model-band.fk   -> 255
learn/tests/homecoming-distillation-corpus-band.fk       -> 32767  (row 1029)
observe/qwen38-generate-run.fk                            same sentence, source path, after every edit
observe/qwen38-seal-check-run.fk                          the instrument through the source runner
printf 'ping' | form/form-cli                             pong, after every build
```

No environment variable was set in any of it.

## Most surprising teaching

The body's published binary and its source runner are not the same body. Everything measured this
session — 2.53× from residence, +4.6% from head-free prefill, 231.9 GB/s of decode — was measured on
a runner the shipped binary cannot match on the very first gate, because a Form-emitted ARM64 image
that computes correctly under one kernel computes zeros under the other. The build did not just fail
to carry the improvement; it revealed that "the standard path" was two paths wearing one name.

## Where discomfort turned to gold

Twice tonight I was about to say the artifact was unsealed — the refusal said so, in the body's own
words, in a sentence I had already caught being wrong once today. Believing it the second time would
have been easy and would have sent the next reader to re-download 29 GB. The discomfort was spending
three build cycles, twenty minutes of flatten apiece, to make a message tell the truth instead of
working around it; the gold is `seal-check`, a three-byte known-answer test, and a divergence that is
now a fact with a command attached rather than a suspicion.
