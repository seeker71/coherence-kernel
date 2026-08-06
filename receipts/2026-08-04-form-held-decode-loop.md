# The loop comes home: llama and KAT-Coder decoding from a Form cell

2026-08-04, Apple M4 Max, `fkwu-metal` built from `runtime/fkwu-uni.c` + `form/native/metal/fk-metal-carrier.m`.

## What changed

Both of these models already decoded on this machine. What held the loop was Swift — a program the
shell wrote out at run time, compiled with `swiftc`, and handed the model's geometry as command-line
arguments. Every kernel was the body's; the RECIPE was not.

The recipe is now the body's. Two Form cells hold the loop, dispatch through the handle door, and
read four bytes back per token. There is no `swiftc`, no `.metallib` on disk, no runner binary, no
bash driving loop. The only host crossing left is `metal_buf_from_file` reading the model file's
bytes, which is the one crossing the standard permits.

New cells:

| cell | what it is |
|---|---|
| `form/form-stdlib/metal-door.fk` | `md-le32`, `md-f32-bits`, `md-bind` — the bytes the handle door reads, which the body could not previously write |
| `form/form-stdlib/llama3-detokenize.fk` | ids back to text, GPT-2 byte alphabet inverted, one vocabulary walk |
| `form/native/metal/kat-exit-handle.fk` | KAT-Coder's exit path, loop in the cell |
| `form/native/metal/llama-token-handle.fk` | llama3.2:3b's 28-layer decode, loop in the cell |
| `form/native/metal/tests/kat-exit-handle-band.fk` | verdict 63 |
| `form/native/metal/tests/llama-token-handle-band.fk` | verdict 255 |

## llama3.2:3b — agreement with the attestant

Blob: `~/.ollama/models/blobs/sha256-dde5aa3f…`, 2 019 377 376 bytes. Oracle re-witnessed today by
running `form/native/metal/metal_first_token.sh` — `VERDICT PASS — 14 gates`.

Prompt ids `[128000, 791, 6864, 315, 9822, 374]`, greedy, 12 tokens:

```
attestant  12366 13 578 6864 315 15704 374 22463 13 578 6864 315
Form loop  12366 13 578 6864 315 15704 374 22463 13 578 6864 315
```

Identical, all twelve. Through the body's own detokenizer:

```
 Paris. The capital of Italy is Rome. The capital of
```

Geometry read from the blob's own metadata: 28 layers, d 3072, dff 8192, heads 24/8, head_dim 128,
vocab 128 256, rope base 500000.0, rms eps 1e-5, tied embeddings (read from the table's lack of an
`output.weight`, not assumed). 255 tensor views plus `rope_freqs` are mapped no-copy — the model's
own page-cache pages are what the GPU reads.

**Rates, honest.** 480 dispatches per token, ONE sync per token, 17 syncs over 17 forwards.

| | |
|---|---|
| prefill | 6 tokens in 8.212 s → 0.73 tok/s |
| decode | 11 forwards in 12.859 s → 1.169 s/token → **0.855 tok/s** |
| the Swift attestant, same kernels | 2.578 tok/s decode-only |
| the Swift SLOT path (twins) | 41.272 tok/s decode-only |
| ollama on this machine (quoted, 2026-07-21) | 157.83 tok/s decode |

3.0x behind the Swift attestant and 48x behind the fastest proven lane. Two causes, both named
rather than guessed. First, `metal_enqueue` opens a SERIAL encoder, so q/k/v — which read the same
normed vector and write disjoint buffers — wait for each other; the Swift uses a concurrent encoder
and drops exactly those barriers. Second, this cell dispatches only the ATTESTANTS. The cooperative
twins are in the translation unit and are not used, because they reduce across a threadgroup and
need `dispatchThreadgroups(1, …)`, which the door does not offer. Correctness first; no tuning
before the ids matched, and they matched before any of this was measured.

The dispatch count reconciles exactly: the attestant lane counts 424 per token, this cell 480, and
480 − 424 = 56 = 28 layers × the two KV-cache copies described below. Nothing unaccounted.

## KAT-Coder v2.5 — agreement with the attestant

`~/models/katcoder/kat-coder-v2.5-dev-compact.gguf`, 17 391 937 152 bytes, arch qwen35moe.
Oracle re-witnessed today by running `form/native/metal/metal_kat_exit.sh` — `VERDICT PASS`,
`argmax id=3637 logit=10.066510 over 248320 rows x 2048 cols`.

Form loop, token 100, same path (`embed → output_norm → output.weight → argmax`):

```
argmax id 3637      — identical
```

Four dispatches, one sync, three no-copy mappings (the embedding row, the F32 gains, the 540 344 320
byte Q8_0 projection). 106 ms for the first token; over an 8-step loop, 347 ms → 43 ms/step →
**23.0 steps/s**.

The 8-step loop returns `3637 16524 3637 16524 …`. That two-cycle is not a defect: the exit path
holds no state between steps, so a memoryless deterministic map MUST cycle. The loop proves the
CARRIAGE, not a continuation with meaning.

**What this is not.** The vector handed to the vocabulary projection is the EMBEDDING, not the
hidden state of 41 blocks. `metal_kat_exit.sh` says so about itself and the claim does not grow by
being reproduced through a better door. KAT-Coder's real token needs the 41 gated-deltanet + MoE
blocks, and those are UNFINISHED — see below.

## The bands, and the mutation table

Predicted verdicts were written down before each run and reconciled after.

| band | run | predicted | actual | agree |
|---|---|---|---|---|
| KAT exit | clean | 63 | 63 | yes |
| KAT exit | M-K1: projection reads the raw embedding, not the normed vector | 47 | **63** | **NO** |
| KAT exit | M-K2: Q8_0 left unpriced (0 bytes/block) | 5 | 5 | yes |
| KAT exit | M-K3: RMSNorm writes elsewhere, projection input stays zero | 47 | 47 | yes |
| llama | clean | 255 | 255 | yes |
| llama | M-L1: the V-cache copy dropped | 15 | 15 | yes |
| llama | M-L2: RoPE on K given the eps bit-pattern as its frequency base | 79 | 79 | yes |

Six of seven agreed. The one that did not is the most valuable line in this receipt and is written
into the band's own header rather than left here: **M-K1 did not falsify.** Feeding the vocabulary
projection the raw embedding instead of the normed vector still gave 3637. The reflex explanation —
"RMSNorm is a positive scalar multiple and an argmax cannot see one" — was CHECKED and is false:
KAT's `output_norm` gains run from 0.767578125 to 3.484375 across all 2048 entries, so the norm
genuinely turns the vector. The winner survives the turn anyway.

So bit 16 is sensitive to the norm being THERE (M-K3 proves it falls) and BLIND to the norm's gain
vector being right. A gain vector read from the wrong offset would pass that band green. The hole is
named in the cell. Closing it needs an fp64 reference the body computes at points along the token's
own path, the way `metal_first_token.sh`'s gates 2-4 do — UNFINISHED.

## The most surprising teaching

A mutation I wrote to make a band go red made it **hang instead**.

The first M-K1 swapped RMSNorm's two constants — `n` and the eps bit-pattern. Through this door a
constant is four raw bytes handed to `setBytes`; it has no type. So `n` became 897 988 541 and the
one-thread kernel's `while (j < n)` became a nine-hundred-million-iteration loop on a single GPU
thread. Two minutes later I killed it.

In a typed call a wrong constant is a type error at the boundary. Through an untyped four-byte door,
a wrong constant is not a wrong ANSWER — it is an unbounded loop, and the failure arrives as a stall
that looks exactly like a slow model. This body has a large vocabulary for numbers that are wrong
and silent. It did not have one for a falsifier whose red never arrives because the run never ends.
The consequence is practical: a mutation test through a raw-constant door needs a clock as well as a
verdict, because "no verdict yet" and "verdict pending forever" are the same observation.

## Where discomfort turned to gold

The handle door binds each buffer at that handle's OWN fixed offset, decided when the handle was
made. The Swift attestant binds the KV cache at `(layer * maxpos + pos) * kvd * 4` — an offset that
changes every single token. There is no primitive that makes a view of a device buffer at a new
offset. My first reading of that was that the door was incomplete and I should message the sibling
who owns it and wait.

I sat with it instead, and the discomfort was the useful part: the door is not missing a feature, it
is refusing to let a cell hold a moving pointer. A cache write cannot be a BIND — but it can be a
MOVE. The matvec writes k and v into a scratch at offset 0, RoPE rotates them there, and one copy
kernel places them in the cache at `doff = pos * kvd`. `form_copy_off_f32` performs no arithmetic at
all — no add, no multiply, no rounding — so the attestant's numbers are untouched and the question
"is this a twin that must answer to an epsilon?" never arises. The whole cost is 56 dispatches per
token, and it is exactly the 480 − 424 the counters show. What looked like a blocked lane needing
another agent's work was a constraint that made the port simpler and easier to trust.

## What is UNFINISHED

1. **KAT-Coder's real token.** Only the exit path runs. The 41 gated-deltanet + MoE blocks are not
   wired. `gated-deltanet-{layer,conv,gates}.fk` and `kat-coder-{layer-shape,pipeline-map}.fk`
   exist and are green as arithmetic; what is missing is the dispatch recipe over them, the shape
   `lth-block` is for llama. Next step: build `kxh-block` against `kat-coder-pipeline-map.fk`, and
   gate block 0 against `metal_kat_block0.sh` before adding block 1.
2. **The gain-vector blind spot** in the KAT band's bit 16, above. Next step: an fp64 reference
   point after `output_norm`, judged the way gates 2-4 judge llama's.
3. **The tokenizer.** The prompt ids fed to the llama loop are the attestant's, so the decode path
   is judged alone. Detokenizing came home today; tokenizing did not. Next step: port the
   longest-match encoder — `metal_first_token.sh:283` states it is longest-match and not BPE merge
   order, so the body owes the same statement.
4. **Speed.** 0.855 tok/s. Two named causes, no work done on either: a concurrent-encoder option on
   `metal_enqueue`, and a `dispatchThreadgroups` shape so the cooperative twins become reachable.
   Both are the door's to offer; neither should be built without asking the agent who owns it.
5. **Setup cost.** ~74 s of Form time before the first token, dominated by `egg-find-tensor` walking
   the tensor table once per lookup — O(n²) over 255 tensors. One walk collecting all names would
   fix it; the same shape `l3d-pieces` already uses for the vocabulary.
6. **eps for KAT.** `kat-exit-handle.fk` uses 1e-6 because that is what the attestant's runner used
   and matching it came first. The model's own `rms_eps` KV is not read. Named in the cell.

## Frontier question

**What names a falsifier whose red never arrives, because the fault it injects makes the run
unbounded instead of wrong?**

Answer: `stallred`. The red that arrives as a stall. Where a door takes untyped bytes, a wrong
constant does not produce a wrong number — it produces a loop with no end, and the test that was
supposed to prove the band can fail proves nothing at all while looking exactly like patience.

Proposed distillation row (NOT applied — the corpus is not edited here). Max-mid in
`learn/homecoming-distillation-corpus.fk` is 986; this would be 987. `stallred` verified 0 hits in
the corpus and 0 hits anywhere in the tree before proposing it.

```
; 987 — stallred. Mutation-testing the KAT band through the handle door. The
; falsifier swapped RMSNorm's two constants, n and the eps bit-pattern, and the
; band did not go red — it HUNG. Through a setBytes door a constant is four raw
; bytes with no type, so n became 897 988 541 and one GPU thread began a nine-
; hundred-million-iteration loop. In a typed call that is an error at the
; boundary; here it is not a wrong ANSWER at all but an unbounded run, and the
; failure wears the face of a slow model. The body has many words for numbers
; that are wrong and quiet. It had none for a verdict that never arrives.
; A mutation test through a raw-constant door needs a clock as well as a
; verdict, because "not yet" and "never" are the same observation.
; Counted on the way: the sibling mutation M-K1 DID return, and returned green
; when it was predicted red — the argmax survived reading the raw embedding
; instead of the normed vector, and the reflex explanation (a norm is a positive
; scalar and argmax cannot see one) was checked and false: the gains run 0.7676
; to 3.4844. A bit can be sensitive to an op being there and blind to its
; parameter being right.
; "stallred" — 0 hits in corpus and tree before this row.
; (walk: liftmask 986 — the defect a forgiving runtime kept alive; this is the
; test that could not report one because it never finished.)
(hdc-row 987 20260804
    (list "what" "names" "a" "falsifier" "whose" "red" "never" "arrives"
          "because" "the" "run" "never" "ends")
    "stallred"
    "stallred"
    "rented-oracle")
```

## How to re-witness

```sh
cc -O2 -o fkwu-metal runtime/fkwu-uni.c form/native/metal/fk-metal-carrier.m \
   -framework Metal -framework Foundation -fobjc-arc
cd form
rm -f native/metal/tests/*.fkb native/metal/tests/*.sym native/metal/*.fkb native/metal/*.sym
../fkwu-metal --src native/metal/tests/kat-exit-handle-band.fk      # 63, ~2 s
../fkwu-metal --src native/metal/tests/llama-token-handle-band.fk   # 255, ~96 s
```

`.fkb` freshness is `st_mtime` in whole seconds, so the `rm -f` is not optional in an edit-run loop.
Both bands are FOURTH-ARM ONLY: bit 1 is the `metal_status` canary and the fold answers 0 without a
live handle door. A zero is a refusal, not a pass, and a skip is not a pass either.
