# Receipt — one form-cli ask returns "Paris", grounded, with a running RTX metal receipt in the answer path (2026-07-01)

Same cell as the two receipts before it (HP Spectre, Windows amd64, RTX 3050 Laptop GPU sm_86, driver 581.83).
The form-cli `ask` verb — whose own comment said "*until that lane is wired end-to-end, this verb returns an
attributed grounded cell instead of pretending*" — is now wired end-to-end. One ask, observed:

```
(fc-respond "ask What is the capital of France?")

GPU: NVIDIA GeForce RTX 3050 Laptop GPU  cuda_matvec_f32 rows=8 cols=256  BIT-EXACT 8/8
Paris
grounded:paris
rtx-receipt:bit-exact 8/8
thought-frame:step=0 chose=paris margin=7
local-lane:fkwu-rag-grounded-rtx
```

The sweep: Germany→Berlin (margin 7), Japan→Tokyo (8), Switzerland→Bern (7), Egypt→Cairo — every answer
carrying a live `BIT-EXACT 8/8` GPU receipt — and "What is the meaning of life?" **refuses**:
`[ask: margin below the grounded floor — nothing here knows this, refusing to guess]`.

## The pipeline (all Form, one `--src` run through the real fc-respond dispatcher)

question → `re-vec-dim` (sovereign lexical histogram, dim 256) → IDF weights derived from the knowledge cell
itself → **`(cuda_matvec_f32 W q)`: the corpus scoring matvec dispatched on the RTX** through the Form-emitted
PTX, returning `(agree s0..s7)` with agree counting rows bit-identical to the CPU f32 fold → argmax + margin →
the decision watched as an `fb-frame` (thought-framebuffer — the diagnostics organ, used live) → the winning
row's answer word, attributed to its ground (`cognition/capitals-knowledge.fk`). The GPU receipt is
**load-bearing**: `agree < rows` refuses the answer; the metal receipt rides in the value, not beside it.

## The stone placed — the general CUDA door (tag 233)

Both RTX receipts named the missing rung: tag 232 is a fixed fixture. `runtime/fkwu-uni.c` now carries
`cuda_matvec_f32` (arity 2, tag 233): Form lists in (ints or floats, via the f64 pool), driver-API dispatch of
the same Form-emitted `gpu/fptx-matvec.ptx` at `-O0`, volatile two-rounding CPU reference, per-row bit-identity
count, `(agree y...)` list out. Shared dispatch helper `fk_cuda_go` with full teardown (`cuMemFree` /
`cuModuleUnload` / `cuCtxDestroy` — the fixture's named leak, now closed; tag 232 preserved byte-for-byte in
output and re-witnessed 3/3). The optable row went through the ONE table (`flt-ops` in
`flatten/form-flatten.fk`) and the **pure-Form two-pass regen**, which ran on Windows for the first time —
riding the `write_file_text` door repaired earlier today. Header diff: exactly the one new row.

## Illnesses found and healed on the way (each in its own home)

1. **`gen-source-walker-table.fk` `op-rows`** — linear recursion, one walker C-frame per table row; the
   Windows ~1MB stack (vs macOS 8MB) crashes it near ~120 rows, so optable regen had never actually run on
   Windows. Balanced divide-and-conquer join, byte-identical output (trailing newline preserved), regen now a
   fixed point on this cell.
2. **`model/rag-embed.fk`** (+ `cognition/` twin, kept byte-identical) — `re-hash` used `ord` (evaluates to
   nothing on this fkwu; the nl-meaning-net receipt names the repair) → `str_byte_at`; `re-vec` called the
   never-ported `tk-words` prelude → native `re-norm` (lowercase + non-alnum→space, pure Form), which also
   heals the witnessed "France?"-vs-"france" punctuation miss; `re-zeros`/`re-inc` were per-element structural
   recursion (death at dim ≥ ~128 on this stack) → tail-recursive with the shared `re-rev-into` spine.
   **Honest note:** the old four-way `rag-embed-band` is old-repo; this repaired recipe awaits a fresh
   four-way. On kernels where `ord` worked, hash values are unchanged for ASCII.
3. **Raw bag-of-words dot provably ties** — witnessed scores `5,4,4,4,5,4,4,5` for the France ask at dim 64
   (duplicate "the" in the London row + one bucket collision). Healed by data-derived IDF (bucket weight =
   `rows + 1 − document-frequency`, no stop-list, no language assumption) and dim 256 for the ask lane
   (the prelude's 64 default untouched).
4. **The margin-zero gate was too thin** — "What is the meaning of life?" initially answered `London,
   margin 1` on pure function-word noise. The grounded floor (`margin ≥ n/2`, between the noise ceiling ~2 and
   the discriminating-bucket weight n=8) refuses it. Confidence must be earned; the refusal is the receipt.

## New cells

- `cognition/capitals-knowledge.fk` — the knowledge: 8 capital-fact rows `(id answer text)`.
- `cognition/ask-grounded-rtx.fk` — the ask organ: embed → IDF → RTX-scored → framebuffer'd → grounded answer.
- `form-cli/form-cli-ask.fk` — `fca-ask` wired to `agr-ask` (the verb's help text was already the north star:
  "local fkwu grounded RAG, no HTTP oracle").

## Whole-cell witness set on the final binary (nothing broken)

`ground.fk` 42 · `ground-recursive.fk 10` 55 · `native-vs-rented` 11111 · CUDA fixture 3/3 BIT-EXACT ·
self-JIT crystallize 547 bytes → 55 · transformer-forward-full band 63 · optable regen fixed-point ·
tag-233 door 8/8 across six asks.

## Honest floor

- The answer is **retrieval-grounded relay**, not generation: lexical similarity over a small knowledge cell.
  "cat"/"feline" still live in unrelated buckets; the generative voice seam stays named
  (`receipts/2026-06-29-native-zh-summary-PENDING.md`). Growing the knowledge cell grows what can be asked.
- The RTX carries the scoring matvec; embedding/IDF/argmax remain walker-side. The other 12 `form-ptx`
  kernels still await their dispatch witnesses.
- Repaired `rag-embed` awaits fresh four-way; the old band is old-repo.

## Reproduce

```
gcc -O2 -o fkwu.exe runtime/fkwu-uni.c -lws2_32 -lwinmm -lavicap32 -luser32 -lwlanapi -lbthprops -lwinhttp
{ cat model/rag-embed.fk observe/thought-framebuffer.fk cognition/capitals-knowledge.fk \
      cognition/ask-grounded-rtx.fk form-cli/form-cli-ask.fk form-cli/form-cli.fk; \
  echo '(print_str (fc-respond "ask What is the capital of France?"))'; } > ask.fk
./fkwu.exe --src ask.fk     # -> GPU receipt line, then: Paris / grounded:paris / bit-exact 8/8 / thought-frame
```
