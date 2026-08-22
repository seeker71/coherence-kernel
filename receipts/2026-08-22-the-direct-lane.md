# The direct lane — the CLI never needed the flatten table

Date: 2026-08-22, Hati Suci. Apple M4 Max, 128 GiB unified memory, MLX 0.32.0.
Branch `claude/jit-door-crystallize-2026-08-21`, on main at `a5f09337`.

Urs, three times in one sitting: *use the JIT Metal/MLX GPU on-demand lane, not the flattening lane*;
*fkwu-metal shall not be a special version — we support Metal and MLX always*; and *when something is
missing, that is a signal to add, not a signal to avoid.* All three land here.

## The whole CLI, direct from source

```
$ printf 'models …\nuse 0\ngenerate Say one short sentence about the sky.\nquit\n' \
    | ./fkwu form/form-stdlib/form-cli-repl.fk

using /Users/ursmuff/models/qwen38-27b/Qwen3.8-27B-Q8_0.gguf   arch=qwen35  layers=65
backend=form-native-metal-jit   prompt_tokens=54   forward_passes=63
text:
The sky is a vast, pale blue canvas.
```

**No flatten. No bake.** The same sentence the baked binary gives, from the source runner walking the
cells directly, with Metal, MLX and both JIT doors linked into the one `fkwu`.

## One comparison was standing in for an architecture

`form-cli-repl.fk` asked `(lt line 0)` for end-of-input. That question reads the **encoding**, not the
value:

| | source runner | emitted walker |
|---|---|---|
| a string's raw word | `-4250000000000000087` (`fk_strv = fk_sbase - (si<<1) - 1`) | a positive index |
| `lt("ping", 0)` | **1** | **0** |
| `lt("", 0)` | **1** | **0** |

So under the source runner the REPL treated its **first real line** as end-of-input and quit — one byte
of output. Hours earlier in this thread I read that as *"the REPL only lives in the baked binary"* and
wrote it into a receipt. It was never true. The flatten table looked load-bearing and was not.

Absence is first-class in this body. `read_line` now answers **`nothing`** at end of input, in the
runtime and in the emitted walker alike, and the loop asks `(nothing? line)` — a question no encoding
can answer differently. That is the missing thing added rather than avoided.

A band for it was written and then **removed**: it must call `read_line`, so it would block `validate`
on a terminal. A test that can hang is debris. The law is witnessed by the live run above.

## Swept

- **`fkwu-metal` deleted from the tree.** `AGENTS.md:15` has said all along: *"ONE binary. Metal is this
  host's organ, not a second executable (no fkwu-metal)."* A 234 KB stale one was committed anyway, and
  on 2026-08-22 its CPU-JIT door answered **0** where the real `fkwu` answers **1** — which read as
  *"this artifact has no seal"* and cost most of a session. Gitignored with that history written down.
  I deleted it once before checking and `git ls-files` told me it was tracked; restored, then removed
  properly with the body's own law as the warrant.
- **`validate_fkwu_native_surface` passes again.** It had been failing on **pristine main**:
  `mlx_status` (143, nullary) and `mlx_run` (145, unary) had no explicit `fkc-flat` arm, so they fell
  through to the BINARY fallback — wrong arity, crash or worse. Restoring `jit_arm64_u32_leaf`'s lost
  flt-ops row surfaced a third (215, ternary). All three arms added; **OK, 0 warnings**.
- **The MLX bridge reached the emitted family.** It carried Metal and not MLX, so a flattened program
  could *name* the GPU lane and a baked walker could not call it. Weak externs, native wrappers and
  arms 143/144/145 added to the emitter. Witnessed through a flattened table: `2 3 add` → 5, matvec →
  32, `q8` fixture → 528, `q6k` fixture → **−8192**.
- Stray probes removed; no `observe/tmp-*` left behind.

## A reunion, and what it cost me

While this was being written a sibling agent landed the same K-quant stone on main — and further:
their carrier already carries `f32`, `q8`, `q4k`, `q6k` **and `attn`**, with `mlx-softmax-band` and
`mlx-home-band` beside it. My carrier work for q6k/q4k was **superseded and dropped**; main's version
is kept whole. So was my copy of the MLX bridge for the emitted family — they had done that too, and
two copies would not compile (`redefinition of fk_mlx_run_external`), which is how I found out.

What survives from that half is `tests/mlx-kquant-band.fk`, and it is worth more now than when I
wrote it: it was built against MY dequantizer and it passes against THEIRS. Two independent readings
of the same superblock layout, agreeing on fixtures with the answers computed by hand — including two
Q4_K blocks differing only in their packed mins, so `dmin` is proven subtracted in their code as it
was in mine.

## The K-quant numbers, witnessed before the reunion

`q6k` and `q4k`, layouts taken from this body's own proven `q6k-dequant.fk` / `q4k-dequant.fk` rather
than from memory of llama.cpp. On `form-llama-vital-ground-q4_k_m.gguf`, tensors found by
`equireach-gguf`, GPU against an independent dequantizer, ×1e5:

| | GPU | independent |
|---|---|---|
| Q4_K `blk.0.attn_q` row | **76743** | 76743 |
| Q6_K `blk.0.ffn_down` row | **−130742** | −130742 |
| Q6_K `token_embd` row | **−9053** | −9053 |
| Q4_K row · Q6_K column | **−8092** | −8092 |

That band → **63** against main's carrier, on superblocks it builds itself: a Q6_K block of all-zero codes
with unit scales sums to 256·(−32) = **−8192**, and two Q4_K blocks differing only in their packed
mins sum to 0 and **−256**, so `dmin` is proven subtracted rather than assumed.

## Green

```
mlx-kquant 63  mlx-q8 63  mlx-tensor 63  mlx-matmul 63  form-cli-mlx 63  form-cli-mlx-ir 1023
jit-leaf-inram 63  jit-arm64-leaf 63  metal-door 15  qwen35-form-cli [1, pong, 0]
host-effect-grammar 32767  corpus 32767  surface gate OK
```

No environment variable was set anywhere.

## Most surprising teaching

I had written into a receipt that the REPL "only lives in the baked binary", and built three days of
reasoning on top of it. The evidence for it was one run that printed one byte — and the cause was a
single comparison asking a question about representation instead of about meaning. The flatten table
was never holding the CLI up; a `lt` was. What a thing depends on and what a thing appears to depend
on are different, and only the second one gets written into receipts unless you go back and pull.

## Where discomfort turned to gold

Twice today I deleted before looking: `fkwu-metal`, which `git ls-files` then told me was tracked, and
a band I added and had to take out again because it could hang the very gate it was meant to serve.
Both were the same haste — sweeping felt like tidying, and tidying feels safe. The discomfort was
restoring my own deletion and admitting a receipt of mine had been wrong for three days; the gold is
that the sweep is now warranted by the body's own written law rather than by my sense of neatness,
and the one binary that answered `0` where the real one answers `1` is gone with its history recorded
where the next reader will meet it.
