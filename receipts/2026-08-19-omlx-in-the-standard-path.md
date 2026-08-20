# oMLX's lessons land in the standard form-cli path — measured, then measured again

Date: 2026-08-19, Hati Suci. Apple M4 Max, 128 GiB unified memory, 40 GPU cores.
Worktree `.claude/worktrees/jit-lane-performance-d77568`, branch `claude/jit-lane-performance-d77568`.
Model: `~/models/qwen38-27b/Qwen3.8-27B-Q8_0.gguf`, 29,047,086,048 bytes, sealed
`a680f44a…67e348`. Binary: the repo-root `./fkwu` that `form/validate.sh` builds with the Metal
carrier linked. Source read: <https://github.com/jundot/omlx>. Companion:
[the ingest and its refutations](2026-08-19-omlx-and-the-jit-lane.md).

Urs asked for the JIT and MLX lessons integrated and embodied in the **core standard** form-cli model
inference path — no side lane, no special handling, and no environment variables.

## First, the binary that could not say no

`generate` refused with *"selected artifact has no current Form-native SHA-256 seal"*. The seal was
current. What had actually happened is that `fkwu-metal` — a stale binary sitting in this worktree —
answers `metal_pipeline` with 0 for the CPU-JIT image, so `saj-file-digest` returned an empty string
and the empty string failed the comparison. A numb organ was reported as a bad artifact. The
canonical `./fkwu` runs it: `pipe 1`, digest matches, generation proceeds. Nothing was changed for
this; it is written down because the shape is the one oMLX's README warns about in its own house —
a lane that degrades silently reads as an ordinary refusal.

## The standard path, measured before touching it

```
prompt_tokens=54  generated_tokens=10  forward_passes=63
weight_bytes_per_forward=27,233,914,176
gpu_busy_us=7,396,756   prefill=6,053,313   decode=1,343,443
prefill 8.920 tok/s   decode 6.699 tok/s   effective 231.9 GB/s
wall 84 s
```

Decode moves 27.2 GB per token at **231.9 GB/s of this machine's measured 447 GB/s stream** — 52%,
not the 0.7%-of-peak of the llama3.2 lane. This decode is bandwidth-bound, and 27.2 GB per token is
the ceiling: 447/27.2 ≈ 16.4 tok/s is the most any kernel work can reach without reading fewer bytes
per token.

Then the wall clock, phase by phase (`observe/qwen38-generate-phase-run.fk`, one run):

```
seal re-verification (SHA-256 over 29 GB)   13,362 ms
tokenize 54 prompt tokens                   13,586 ms
open (tensor views, kernels, tables)        15,615 ms
state allocation                                 8 ms
GPU work of the whole generation              7,397 ms
```

**Setup was four times the inference, and every `generate` paid it again.** That is the finding that
chose the work.

## What landed

### 1. Prefill stops asking a question nobody reads

`form/native/metal/qwen35-dense-token-handle.fk`. A forward was embed → blocks → head → read back the
argmax, and prefill ran that for every prompt token while discarding all but the last answer. The
head is `output_norm`, the vocab-wide `output.weight` projection — the single largest tensor in the
file — and the argmax fold. Split into `q38-forward-state` (embed + blocks) and the head;
`q38-advance` runs the state half and lets `metal_sync` stand exactly where the argmax read-back
stood, so the batch is submitted and waited on before the next position is encoded. Same ordering,
one fewer question.

Bit-exact by construction — what each position leaves behind is unchanged — and witnessed: the same
prompt returned **the same sentence, word for word**.

```
prefill_gpu_busy_us   6,053,313 -> 5,785,502      prefill 8.920 -> 9.333 tok/s   (+4.6%)
decode                unchanged
```

### 2. A model admitted once answers many sentences

`form/form-stdlib/form-cli-model-generate.fk` and `form/form-stdlib/form-cli-repl.fk`. A **residence**
is the admission held: the sealed path, the position ceiling it was opened for, the opened context,
and the header window and architecture read from it. The stream state is deliberately *not* part of
it — state is what one conversation writes, so it is made and released per generation, with its own
handle-count check.

There is no mode and no switch. `fcmg-generate-with` is handed whatever the caller holds; an empty
residence, a residence for another file, or a prompt that outgrows the ceiling all end in the same
place — release, then admit. A caller that holds nothing gets the old open-run-close exactly, which
is what `fcmg-generate` now is. The REPL turn answers a pair — what to say, and what the session is
still holding — and only `generate` can change it. Leaving releases, and a release that cannot
account for every handle says so.

Witnessed live, two sentences through one admission (`observe/qwen38-resident-run.fk`):

```
turn 1  admission + generation   56,882 ms wall   gpu_busy 7,312,610 us
turn 2  resident                 22,465 ms wall   gpu_busy 7,054,793 us
release_ok 1                        43 ms
```

**2.53× on the wall clock for the second sentence**, 34.4 s saved, with GPU time unchanged — the
whole saving is admission no longer paid twice. Turn 2 answered a different prompt correctly ("The
sea stretches endlessly under the open sky."), so this is residence, not a cached answer.

### 3. Everything that called the old shape was healed, not stepped around

`form/form-stdlib/tests/qwen35-form-cli-band.fk` and `observe/qwen38-form-cli-flow-run.fk` both called
`fc-turn` with the old arity. Both now thread the residence, and the scripted flow witness releases it
at the end and reports whether the release accounted for every handle.

## What ran

```
form/form-stdlib/tests/form-cli-resident-model-band.fk   -> 255   (the residence law)
form/form-stdlib/tests/qwen35-form-cli-band.fk           -> [1, pong, 0]
ingest/tests/frontier-ingest-omlx-band.fk                -> 127   go / rust / ts / fkwu, four runs, one number
learn/tests/homecoming-distillation-corpus-band.fk       -> 32767
observe/qwen38-generate-run.fk                            same sentence before and after both changes
observe/qwen38-resident-run.fk                            the two-turn witness above
```

The three sibling kernels were each run **directly** on the ingest band's expanded chain — `bin-go`,
`form-kernel-rust`, `node --experimental-strip-types main.ts` — rather than through `validate.sh`
under `FORM_ALLOW_THREE_ARM=1`. No environment variable was set anywhere in this work. The reason
that flag existed in the earlier receipt is a real, pre-existing gap named here rather than waved
past: this checkout's committed fourth-arm seed is stale — `form-stdlib/bootstrap/fkwu-uni.stamp`
holds `fef950f52b65eeb5` while the current emitter chain stamps `13af5abe0572e61e`. Its remedy,
`form/scripts/regen_fkwu_bootstrap.sh`, was read (43 lines; it emits `bootstrap/fkwu-uni.c` through
the Go sibling and rewrites the stamp, and touches nothing else) and deliberately **not run**:
republishing the trunk's bootstrap seed is a maintainer act that belongs to its own change, not
smuggled inside a lane's performance work.

The same is true of `form/form-cli` itself: the committed binary predates this branch — its verb list
has no `generate` at all — and `build-form-cli.sh` answers `source stamp stale … regeneration
required`, pointing at `scripts/regen_form_cli_bootstrap.sh`. The source path is where every cell,
band and probe here runs; the published binary needs that maintainer regeneration to catch up, and
that is stated rather than assumed.

## What is named and not yet built

Each of these is now a measured number, not an opinion:

- **The tokenizer is the largest remaining non-GPU cost**: 13.6 s of a 22.5 s resident turn.
  `tkz-id-of` is a full linear vocab pass **per symbol** — its own comment says so — over Qwen's
  ~151k tokens, and `tkz-cands` walks every merge record per encode. A single pass cannot fix it
  (the work is per token-symbol pair either way); it needs a real index over the GGUF strings.
- **Prefill is still one full 27.2 GB sweep per prompt token.** The batched-matmul lever measured
  4–5× on the llama lane (`qk-matmul-batch.fk`, ceiling 7× from D = 6L). It does not port unchanged:
  Qwen3.5's hybrid stack is three Gated DeltaNet blocks to one full-attention block, and a recurrence
  has to be chunked before it can be batched — which is exactly what oMLX carries a `gdn.py` for.
- **The drafter is in this very file.** `q38-geo` already reads `nextn_predict_layers` and
  `q38-open` subtracts it from `block_count` — the multi-token-prediction block is parsed and
  excluded. Verified speculation is bit-exact, but it needs the multi-token forward above before a
  verify pass can be cheaper than the drafts it checks.
- **Cross-prompt prefix reuse** — the teach overlay makes every prompt share a long prefix — needs a
  state snapshot at the prefix boundary, because the GDN state is recurrent, not a row store.

## Most surprising teaching

The standard path spent 90% of its wall clock not inferring. Every number this program has argued
about for a month — 0.7% of peak, 231.9 GB/s, 4–5× batching ceilings — is about the 7.4 seconds, and
nobody had measured the 76 seconds around them. The largest available speedup was not in a kernel; it
was in asking the same 29 GB file to prove itself again before every sentence.

## Where discomfort turned to gold

The plan going in was fusion — fewer, fatter kernels, the thing oMLX's 30× headline is actually
about. Measuring first said the decode already runs at 52% of this machine's stream bandwidth and
moves 27.2 GB per token, so fusion could not win what I wanted it to win, and the honest lever was
the unglamorous one: stop re-admitting the model. The discomfort was setting down the interesting
work and picking up the boring work because the measurement pointed there; the gold is 2.53× on the
second sentence and a lane that now names its next four stones with numbers instead of intentions.
