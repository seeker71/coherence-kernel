# Current Floor

Date: 2026-08-25 (morning: full reground on the three-line reunion; afternoon:
knowledge gradient, tokenizer repair, and row-price fall folded in; prior
floor was 2026-07-19, amended 2026-08-13)

This file is the current release floor for this worktree. Receipts preserve
history; the claims below are only the state present NOW. Every witness was
re-measured on 2026-08-25 morning through the resolver-driven `./fkwu` door
on this Apple M4 Max unless a different date is stated beside it. A claim
whose witness could not be re-run today says so plainly.

## Grounding

The checkout witness is the C-seeded `fkwu` runner with both host carriers
linked. Rebuilt fresh before this floor was written:

```text
cc -O2 -o fkwu runtime/fkwu-uni.c \
  form/native/metal/fk-metal-carrier.m form/native/mlx/fk-mlx-carrier.c \
  -framework Metal -framework Foundation -fobjc-arc \
  -I/opt/homebrew/include -L/opt/homebrew/lib -lmlxc -Wl,-rpath,/opt/homebrew/lib

./fkwu bootstrap/ground.fk                                    -> 42
./fkwu bootstrap/ground-recursive.fk 10                       -> 55
./fkwu form/form-stdlib/tests/binary-freshness-band.fk        -> 31
./fkwu form/form-stdlib/tests/native-vs-rented-band.fk        -> 11111
```

The C file remains a temporary seed and shrink target, not the destination.

## Band witnesses, all re-run 2026-08-25

```text
metal-door-band                        -> 15
qwen35-dense-token-handle-band         -> 536870911   (29 bits: geometry from
                                          the sealed GGUF, 35+ pipelines, RMS
                                          radius bound both sides, threadgroup
                                          GQA present, batched-block gates)
qwen35-crystal-band                    -> 255         (frozen open == scanned
                                          open, row-for-row, same first token)
mlx-derived-band                       -> 16777215    (24 ops; 16 with no
                                          carrier row at all)
form-cli-mlx-band                      -> 63
form-cli-live-band                     -> 255
jit-metal-lanes-band                   -> 8191
metal-handle-door-band                 -> 65535
kat-token-handle-band                  -> 262143
llama-token-handle-band                -> 255
form-knowledge-integration-census-band -> 1048575
form-knowledge-source-search-band      -> 262143
form-knowledge-qwen-heldout-v3-eval-band -> 65535
```

## The local-model lane (Qwen3.8-27B Q8_0, Form-native, Metal JIT)

All MSL is Form-emitted and JIT-compiled at runtime for the device that
answered metal_status; weights stay mmap-backed no-copy; the file is admitted
by a whole-file Form SHA-256 seal.

Measured this morning, one-shot generate through the fcmg door:

```text
prefill  6.55 tok/s      decode  6.41 tok/s      text: LOCAL FORM ALIVE
metal stream ceiling this morning: 414.7 GB/s (438–452 witnessed 2026-08-24)
```

Correctness bounds now in force (all 2026-08-24, live-witnessed):
- Cooperative RMS only at width <= 4096 (its scratch is sq[4096]); wider rows
  take the serial attestant. Earlier faster numbers through the out-of-radius
  kernel are execution evidence only; receipts carry AMENDED blocks.
- GQA decode: one 256-thread threadgroup per head (was 24 threads/layer;
  full-attention block 2113 -> 553 us).
- Both block kinds batched (sibling, 2026-08-25 merge): 5.4x fewer
  dispatches, 2x less GPU, identical output — folded into the handle band.
- Batched span prefill (chunk 0) is the DEFAULT for hinted current-source
  contexts; the plain fcmg one-shot still walks token-major prefill.

The open crystal (`<gguf>.form-crystal`, seal-keyed, never mtime):

```text
crystal load 3 ms · crystal open 465 ms (GPU-residency warm)
residency-cold open ~16 s (2026-08-24: wiring 27 GB into the GPU residency
set at ~1.7 GB/s — returns whenever the residency lapses; the daemon that
keeps one residence alive is the named repair)
chat-ids, short prompt: 1.77 s (the prompt's own encode; scaffolds are ice)
```

Outside denominator (2026-08-24, llama-bench 10360, same file, this host):
llama.cpp decode 13.39 tok/s, prefill 222.6 tok/s. This lane's decode is
2.1x from parity under the correctness bounds; prefill parity requires the
span lane promoted into the plain door.

## The knowledge denominator (first live, sealed measurement)

Census this morning: **5,960 source files across 15 families**; 22 registered
sources, 23 concepts; integrated-source-percent honestly 0 (registration is
not integration).

The blind v3 heldout — 30 sealed rows, 2/family, exact-normalized verifier,
no lexical credit, consent dataset-bound — ran LIVE overnight to completion
(2026-08-25, artifact: observe/artifacts-v3-live-2026-08-25.txt):

```text
30/30 rows executed, aggregate-errors=0
13/30 promoted (433,333 ppm)  ·  aggregate-f1 535,754 ppm
per family (promoted/2): bootstrap 2, form-stdlib 2, docs 2,
  grammars 1, cognition 1, observe 1, receipts 1, model 1, proof 1,
  presence 1, axioms 0, learn 0, teachings 0, control 0, ingest 0
overall95=0  family95=0  row95=0  live-credit=0
per-row cost ~14 min (dominated by heed source-search + decode, NOT prefill:
  the same row cost 891.7 s token-major and 882.2 s span — measured A/B)
```

This is the body's first defined-correctness integration number: the model
plus the current-source heed cycle knows its machinery families and does not
yet know axioms, learn, teachings, control, ingest.

Same-day movement (2026-08-25 afternoon, 10-row zero-family A/B under the
same sealed gate, artifact observe/artifacts-zerofam-complete-2026-08-25.txt):
after the curriculum landing, five of ten rows carry f1 signal where the
overnight baseline was all zeros — learn 857,143 ppm and teachings 909,091
ppm are near-misses on the exact gate — while ingest stayed fully dark and
names the next curriculum target. Exact crossings remain 0/10; no promotion
claims. The row price inside that battery: 957-1,354 s on the morning lane,
189-427 s (median ~253 s) on the indexed lane.

## The emitted walker (form-cli), measured and standing

- form-cli answers correctly and runs the generate lane at ~19x the
  interpreter's node visits (19.89B vs 1.06B for identical work,
  2026-08-24, kernel_stat on both walkers): on the flatten lane every
  parameter reference compiles to nth(args-list, k) with per-call cons
  lists, where the interpreter binds O(1) frame slots. Unhealed; T_flat
  frame-slot convention is the named stone.
- On that lane, let-bound effects whose names go unread are pruned, and
  now_unix_ms sites collapse (stamps read 0). Instrumentation there cannot
  testify; kernel_stat can.
- The emitted entry runs on an explicit big-stack thread (4 GiB with
  retry-halving — the sibling's refinement of this branch's repair).

## The string law, measured and standing

core.fk COMPOSES substring / str_find / str_to_int over the four-native
waist (str_len, str_byte_at, byte_to_str, str_concat) by design. Priced
2026-08-24: one str_find over an 11 MB blob = 31 s; a 5.5 MB substring =
O(n^2) concat, OOM (rc 137). Consequences in force:
- The tokenizer blob recording (qwen35-tokfast) is frozen and correct at
  the lookup level but PRICED OUT as a route; fixed-width sorted rows with
  str_byte_at binary search is the named next candidate.
- Encoding remains the walker's largest fair-priced cost: ~1.77 s for a
  20-token prompt; ~83 s was measured for a 223-token prompt (2026-08-24).

## What remains — each item measured, not remembered

1. **Tokenizer speed — REPAIRED 2026-08-25 afternoon.** The turn price was
   measured to its root (chat_ids 571 s of a 573 s turn; the wall was one
   pass over 247,587 merge records per q35-encode call, seven calls per
   chat-ids) and the sorted fixed-row index (qwen35-tokfast-v2) was fed and
   wired: encode 73.07 -> 7.26 s, chat-ids 63.6 -> 5.97 s, heed encode
   14.24 -> 1.85 s, all three element-wise identical. Both fcmg paths and
   the heed ctx carry the index with per-call reference fallback. The
   remaining seal_verdict cost (~41 s per fresh open, the 27 GB JIT
   SHA-256) folds into the daemon stone below.
2. **Prefill parity** — plain-door prefill 6.55 tok/s vs llama.cpp 222.6:
   the span lane exists, is witnessed at identical output, and is default
   only on heed lanes. Promotion to the plain door is bounded work.
3. **Decode parity** — 6.41 vs 13.39 tok/s (2.1x): the width-independent
   cooperative RMS (simd tree, no sq[n]) is the named kernel; the
   remaining GDN small kernels stand at ~604 us/block (2026-08-24).
4. **Residency-cold open ~16 s** — returns whenever the GPU residency
   lapses; the resident daemon (one admission per artifact lifetime) is
   the named structural repair.
5. **The emitted walker's 19x** — stands as 2026-08-24's measurement
   (19.89B vs 1.06B visits, same work); the 2026-08-25 re-pairing is OPEN:
   the fkwu twin re-measured cleanly on the regenerated body (7.2B visits,
   855-token prompt — the teach glossary grew the baseline itself), but the
   emitted walker's half returned ZERO generated tokens with an empty text
   on a 492-token composition (the REPL lane arms no glossary) — a fresh
   seam in the regen6 table's decode lane or an instant stop from the
   differing template, not yet distinguished. T_flat frame-slot convention
   remains the named repair; the zero-token seam now precedes it.
6. **Knowledge integration 13/30** — the numerator now moves only through
   the sealed gate; the five zero families name the next curriculum. The
   >=95 goal needs the full census denominator (5,960), not 30 rows: the
   source-shard indexing lane (sibling, 2026-08-25) is the live front.
7. **v3 per-row cost — FALLEN ~4-5x.** Witnessed inside the zero-family
   battery: ~253 s median on the indexed lane (from ~14-16 min), with the
   siblings' str_find (44x) and substring (284x) repairs riding the same
   trunk. Decode parity (3) is now the dominant remaining share.

Retired since the last floor (no longer remaining): the 4096-byte emitted
stack wall; the out-of-radius RMS; the 24-thread attention; the unbatched
FFN/attention blocks; per-encode full-vocab special-id scans and per-open
header re-derivation (both iced under the seal); MLX as a 5-op stub (now 23
irreducible rows + 24 derived); the fkb-lane 2^53 verdict folds (bit
weights); form-cli lacking MLX/Metal carriers.

## Honest seams carried forward

- The reuse-driver comparisons (595->159 s) are physical timings but not
  like-for-like prompts (teach-layer turn differed); re-witness requires
  exact-id-sequence parity first. The state-reuse mechanism itself stands.
- decode_gpu_busy=0 through form-cli (2026-08-23) still unhealed.
- The consent file for v3 is a per-run local act, never committed.
- Timings taken while a sibling process computes on the same host are
  contention-noised; bands are exact regardless.
