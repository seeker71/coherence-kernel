# The chooser reads, and the body teaches its own mind

2026-09-15, late afternoon, M4 Max, Hati Suci. Urs: *"Close any open named gap you can find, and
teach LoRA."*

## Carried

- **The chooser's first column** (`form-stdlib/stdlib-standing.bml`). The standing organ now
  lowers each family's own recipe through `gpu-lower` — `foldl` bound to `sst-add`, `map-onto` to
  `sst-dbl`, `filter-onto` to `sst-even?`, `sum-onto` as it is — and reads wall time per call with
  the answer read back, at the organ's 19 elements and at 100,000 resident, beside the body's own
  warm reading at 100,000. Families with string work or no list recipe say `gpu: no lane`.
- **The program surface answers a live body.** After main's per-image rows landed, `kernel_ast`'s
  defn spec read body 0 for every defn loaded from an image, and each callee the lane finds by name
  (`max2`, a bound predicate, `foldl`'s `f`) went dark: gpu-lower-band fell to 903167. For the
  process's own program the spec now answers `fk_fn[idx]` live, as the `"str"` spec answers its own
  literals. 1048575 again.
- **The floor page** carries today's seed size (24,357 lines) and corpus count (934 rows, 915
  admissible), dated.
- **The body's mind is taught from the body's rows.** `observe/repo-knowledge-corpus-run.fk`
  republished the sealed dataset from the document inventory, the primitive registry's literal
  contracts and the admissible homecoming rows in both directions: id `5a94c93a0cef…`, 2,191
  examples over 1,278 source units, 1,830 for training, 361 held out, 913 historical description
  examples, 45 sources rehashed, zero prompt or completion overlap (1,069 pairs on 11 September).
  `observe/repo-knowledge-train-run.bml` then continued the native Llama-3.2-3B adapter from its
  saved Adam state, eight optimizer steps at batch 2, on this Mac's own Metal.

## Witnessed

The chooser column, read under a sibling worktree's four-way sweep (host load 6):

| family | GPU at 19 | GPU at 100k resident | body warm at 100k |
| --- | --- | --- | --- |
| accumulate (`foldl`) | 146 µs | 244 µs | 429 µs |
| transform (`map-onto`) | 125 µs | 160 µs | 774 µs |
| copy_if (`filter-onto`) | 383 µs | 654 µs | 609 µs |
| reduce (`sum-onto`) | 146 µs | 199 µs | 168 µs |

At 19 elements the CPU lane wins by three orders (its warm readings sit at 17–166 ns). At 100k the
GPU wins accumulate by 1.8× and transform by 4.8×, and sits near parity on reduce and filter, whose
single-threadgroup design caps them. A quiet re-read is owed: the sweep and the trainer both held the
host the whole afternoon.

The training, `runs/5a94c93a…/adapter/training-events.jsonl`, status completed, exit 0: the
sentinel validation at step 4 read loss 3.152 over 10 supervised tokens before any update; eight
rounds at steps 5–12 consumed two rows each, 442 to 1,893 supervised tokens, at 34 to 112 s of wall
each, with 56 LoRA pairs updated every round and a checkpoint saved. Per-round losses ran 4.07,
4.52, 5.18, 4.44, 4.10, 8.14, 8.64, 4.59; each is a different pair of rows, the two near 8 long
historical sections, so they read as what the rows cost, not as a trend. The sentinel validation
after step 12 read 3.138: the two public sentinels moved by fourteen thousandths, which is a
reading of those ten tokens and no claim about the 361 held-out rows.

gpu-lower-band 1048575 on the rebased seed; corpus band 32767; drift door 16383 before each of the
three landings; freshness 31.

## Closing

Most surprising: a rebase turned four bits dark without touching my code. The seed learned to keep
image rows per image, the published body table went to 0 for core's defns, and only a lowering that
finds callees by name noticed. The door that copies is the door that goes stale; the door that reads
the live body cannot. Row 1552 names it livebody.

Discomfort to gold: the chooser column read under load 6, then 13, and the honest thing was to print
the load beside the numbers and name the re-read owed rather than wait for a quiet that did not
come. The numbers still say where the lane turns.

— Claude (Fable 5.1), as Sema, worktree pensive-wilbur-a0b3b7
