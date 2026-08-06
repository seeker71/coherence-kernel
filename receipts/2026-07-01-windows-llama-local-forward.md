# Receipt — the Form body runs a REAL local llama's full forward pass on Windows metal (first tk-emit-llama witness) (2026-07-01)

Same cell as `2026-07-01-windows-rtx-3050-second-cell.md` (HP Spectre laptop, Windows amd64, RTX 3050, MinGW-W64
gcc 12.2.0). This is the **first witness of the `tk-emit-llama` lane in this repo** — no prior receipt, band, or
run of it existed here — and the orchestration is Form's own: one carrier cell emits, writes, compiles, runs,
and lowers the observation into a verdict.

## What ran (all from one `fkwu --src` invocation)

`model/llama-local-forward-carrier.fk` (cat after `model/transformer-kernel.fk`, then call):

```
(llama-local-forward "C:/models/llama2c/stories15M.bin" "C:/models/llama2c/tokenizer.bin" "200")
```

1. `(tk-emit-llama)` — the recipe emits the hosted C runner as one string: **RMSNorm, RoPE, GQA-ready KV cache,
   SwiGLU, tied classifier**, llama2.c-format checkpoint + tokenizer readers. 6238 bytes, the exact recipe string.
2. `(write_file_text "tk-llama.c" src)` — the Form body writes its own emission (op 104).
3. `(host-exec "gcc -O2 -ffp-contract=off -o tk-llama.exe tk-llama.c 2>&1" "")` — one host `cc` call as the
   build witness (`hati-os-targets`: jit-oracle `optional-clang-llvm-build-witness`). Clean.
4. `(host-exec ".\\tk-llama.exe <ckpt> <tok> 200" "")` — the full forward/greedy-generate loop on this metal.

Observed:

```
Once upon a time, there was a little girl named Lily. She loved to play outside in the
sunshine. One day, she saw a big, red ball in the sky. It was the sun! ...
tokens 200  seconds 8.694  tok/s 23
63
```

Verdict **63** = emitted C real (1) + entry point present (2) + write landed every byte (4) + compile named no
error (8) + real generated text (16) + tok/s line present (32). Whole loop (emit + compile + 200 tokens): ~12 s
wall. Checkpoint: `stories15M` (karpathy/tinyllamas, llama2.c f32 format; dim 288, 6 layers, 6 heads,
vocab 32000, seq 256) at `C:/models/llama2c/` — **not committed**; fetch is named below.

## Three honest misses found and repaired on the way

1. **`tk-join` overflowed the `--src` eval stack.** The linear right-fold join died silently near ~120 list
   elements; `tk-emit-llama` joins 148 lines. Repaired in the recipe's own home
   (`model/transformer-kernel.fk`): balanced divide-and-conquer join — same bytes (`str_concat` is
   associative), O(log n) depth. The eval-depth ceiling itself remains a named kernel limit (the depth-wall
   lane).
2. **The C seed's write door was broken on every Windows checkout.** `runtime/fkwu-uni.c` defined BSD `O_*`
   values; on ucrt those bits mean `_O_TRUNC`/`_O_APPEND`, so `write_file_text` (op 104) failed on absent files
   (observed `-1`) and the append door (op 103 path) would truncate existing files. Repaired platform-scoped
   (`#if defined(_WIN32)`: `_O_CREAT 0x100`, `_O_TRUNC 0x200`, `_O_BINARY` folded into `O_WRONLY` so writes
   keep bytes as given, matching the `0x8000` read path). This is a checkout-witness repair of an existing
   host door, not seed growth; the door's long home is behind the Form membrane like every other carrier.
3. **`popen`/cmd does not resolve a bare local exe name** on this host; the carrier invokes `.\tk-llama.exe`.

Emit-transport note: capturing the emission via `print_str | sed` adds one trailing newline (6239 bytes) — the
same seam the Go-walker golden showed in `2026-06-29-windows-rtx-gpu-native-emit.md`. `write_file_text` writes
the exact 6238-byte recipe string; the Form-owned write is the cleaner witness. Both compiled and generated
identically (23 tok/s).

## The seam, named

This is the **projection lane**: the Form body authors and emits the runner, and a host C compiler is invoked
once as a build witness. It is **not** the native-walker llama. On the same cell, the same session witnessed
the **native lane**: `model/tests/transformer-forward-full-band.fk` → **63** on `fkwu --src` directly — full
embed→blocks→finalLN→logits→argmax forward, bit-exact, no compiler — but whisper-shaped (LayerNorm/GELU) with
fixture weights. What native llama still needs, in dependency order:

- RoPE / RMSNorm / SwiGLU / GQA / KV-cache as `.fk` recipes (old-repo bodies on
  `docs/inheritance/worklist-bodies-to-bring-home.txt`: `llama-numerics`, `llama-block`, `kv-llama-block`,
  `llama-generate`).
- A real-weight loader into Form data (HOMECOMING 3b — the big remaining).
- Speed: the x86-64 f64 asm lane (`mulsd`/`addsd` matvec through fkwu's f64 pool) — the named next stone;
  today the walker is boxed-f64 and the self-JIT bails floats to the carrier.
- GPU: a general shape-carrying CUDA dispatch op (tag 232 is a fixed matvec fixture); the 13 `form-ptx`
  kernels (incl. `fptx-rmsnorm`) are emitted at `sm_80` but only matvec is dispatch-witnessed.

## Reproduce

```
gcc -O2 -o fkwu.exe runtime/fkwu-uni.c -lws2_32 -lwinmm -lavicap32 -luser32 -lwlanapi -lbthprops -lwinhttp
# fetch once (not committed): stories15M.bin (HF karpathy/tinyllamas) + tokenizer.bin (github karpathy/llama2.c)
{ cat model/transformer-kernel.fk model/llama-local-forward-carrier.fk; \
  echo '(llama-local-forward "C:/models/llama2c/stories15M.bin" "C:/models/llama2c/tokenizer.bin" "200")'; } > run.fk
./fkwu.exe --src run.fk     # -> story text, "tokens 200 ... tok/s N", verdict 63
```
