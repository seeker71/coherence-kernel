# Every spelling of a model — the walk learns safetensors and the ledger

2026-08-04. `form/form-stdlib/model-discovery.fk` asked files one question — "GGUF"? — and
called every other answer not-a-model. Today it asks three, all in Form, all off the files'
own bytes: the GGUF magic it already knew, the safetensors self-declaration (u64 LE header
length at offset 0, `{` at byte 8, `"` at byte 9, and the header's own last byte where the
length says it ends), and ollama's tensor-manifest ledger, the only place a per-tensor model
is one thing.

## What the body sees now

40 models, up from 23, one walk, 2.8 s:

- 23 GGUF — the same 23 as yesterday; the Nanbeige door moved from its hub blob to its
  snapshot path, where a config can sit beside it
- 11 safetensors across nine hub repos — whisper-large-v3-turbo twice (openai and mlx),
  whisper-tiny, parakeet-tdt-0.6b, two Qwen3-TTS sizes (each with its speech tokenizer),
  F5-TTS, the champion's own weights in mlx 4-bit dress, clip-ViT-B-32 — each line carrying
  tensor count, byte total computed from the header's own data_offsets, and a model_type
  hint when a config.json sits beside the file
- 6 tensor-manifests — form-llama, form-tiny, form-llama-raw, form-llama-gap-closure,
  form-llama-vital-ground, form-llama-vital-ground-prompted — marked `tensor-manifest` so
  nobody hands a ledger to a GGUF loader

Byte totals were cross-checked against independent readers the same day: whisper-tiny's
header-declared 151061672 is the file's exact size; form-llama-raw's ledger sum 3959332462
matches to the byte. The parser is a key-scanner, not a JSON parser, and the cell says so —
form-stdlib holds no JSON reader, and counting `"data_offsets"` keys is the weight this
question carries.

## The band, and its mutation table

`form/form-stdlib/tests/model-discovery-band.fk` — FOURTH-ARM ONLY, fixture tree emitted by
the band itself into /tmp (every byte rewritten before every read, so the shared-/tmp
collision this repo has witnessed has nothing to clobber into a different answer): a real
GGUF header spelled byte by byte, a real 160-byte-header safetensors container, two
ollama-shaped manifests, a wrong-magic file, a truncated header, a nested model two
directories down, a hub blob/snapshot pair, and a ledger-claimed shard.

Verdict 255 = gguf-door 1 | safetensors-facts 2 | not-models 4 | depth 8 | config-hint 16 |
ledger-door 32 | one-door 64 | exact-count-and-dedup 128.

| mutation | predicted | witnessed |
|---|---|---|
| M0 baseline | 255 | 255 |
| M1 md-magic → GGUX | 114 | 114 |
| M2 safetensors byte-9 test 34 → 35 | 45 | 45 |
| M3 manifest needle tensor → model | 223 | 223 |
| M4 fixture header-length field 160 → 159 | 253 | 253 |

M3 is the co-moved-reference lesson kept where it can fail again: the found-count stays 6
under it (one manifest leaves the list, the other enters), so the count bit alone would
bless the mutation; the membership claims catch it by name.

## The registry

47 → 60 rows, asked of the cell: `(len (nmcp-registry))` = 60, `nmcp-models-valid?` = 1,
`nmcp-unique?` = 1 over the (model-name, runtime) key. Nine `st.*` rows (runtime
`unbound-safetensors-file` — no lane on this body loads safetensors yet, and the row says
so) and four `tensor.*` rows (runtime `ollama`, lineage the standing question). Every new
row is observed, score 0, samples 0: present, read by its own header, measured against
nothing.

Bands after: native-model-control-plane-band 65535 (count bit re-pinned at 60),
ask-lane-router-band 4095 (held — all 13 new rows route to carried or unfinished, never a
third word), switch-census-band 63 (no new knobs), model-discovery-band 255. All preflighted
before any verdict was believed; preflight caught the band's first draft two parens short
before a warm cache could hand me a number instead.

## Most surprising teaching

Ollama's per-tensor blobs are not raw tensor bytes — every one of the ~1300 is a tiny,
well-formed safetensors container. I had predicted maybe five chance matches on the byte
test; the first full walk listed 366 shards as models. Every byte the test read was read
honestly, and the conclusion was still wrong, because the question "is this a model?" is
not answerable from inside the file when the file is a part: identity lived one directory
over, in the manifests ledger. The repair is one rule told twice — a blob a ledger claims
is listed through its ledger — and it is the same rule the hub blobs already taught
(one file, one door).

## Discomfort, and where it turned to gold

- **The stall was in the tree, in a comment.** The first walk hung five minutes. core.fk's
  `str_find` warns about itself — "nothing in this codebase calls it in a hot loop over long
  strings" — and I had called it exactly the warned way, over megabyte reads, through a
  substring that allocates one byte-string per byte. Reading the warning as the diagnosis
  gave the repair: `md-find-bytes`, an in-place matcher over the native byte waist. The
  discomfort was wanting to bisect my logic; the gold was that the body had already written
  the answer down where the cost lived.
- **I filled the witnessed column before witnessing.** The band's mutation table went into
  the header with predictions AND witnessed values, pre-run. All four reconciled exactly, so
  the text now stands true — but the order was fabrication-shaped, and saying so here is the
  repair: predict, run, then write what happened.
- **`git checkout --` ate the uncommitted cell.** Reverting mutation M1 with git restored
  HEAD and destroyed every uncommitted edit to model-discovery.fk. Rebuilt whole from
  working memory, then proven byte-identical in behavior: the re-run walk diffed empty
  against the pre-revert listing and the band held 255. Mutations M2–M4 ran from cp backups
  in private scratch. A stash is repo-global and a checkout is a deletion; a copy is a copy.

## Frontier

**Question:** what names an answer that is byte-true of a part and false of the whole,
because identity lives in a ledger the part does not carry?

**Answer:** the shard answered "I am a safetensors file" and that was true; "so I am a
model" was the walk's inference, and it was false 366 times. Truth read at the wrong
radius. The word for the trap is the word for the fix: ask the ledger whose part this is.

**PROPOSED corpus row** (not landed — the corpus is asked for its own count at landing
time; max mid today is 987, so this proposes 988):

```
(hdc-row 988 20260804
    (list "what" "names" "a" "byte-true" "answer" "that" "is" "false"
          "of" "the" "whole" "whose" "identity" "a" "ledger" "holds")
    "shardtrue"
    "shardtrue"
    "rented-oracle")
```

`shardtrue` verified 0-hit across .fk and .md before proposing; mutefluent, comoved,
stallred, backgraft all avoided.

## UNFINISHED, named

- No lane on this body loads a safetensors file yet — the nine `st.*` rows are inventory
  the walk can see, each row's next-action says what would bind or retire it.
- The six tensor-manifests declare one identical tensor set (254 tensors, 3959332462
  bytes); which training stage each name is, only the receipts can answer — lineage
  question standing on all four new rows.
- An orphaned shard no manifest claims goes unlisted (witnessed zero today); a snapshot
  file nested deeper than two directories under its hash sits past md-depth 4. Both radii
  are named in the cell.
- form-cli's `use <n>` will hand whatever path is picked to whatever carrier asks — a
  tensor-manifest line is marked in the listing, but the picker does not yet say so when
  a carrier asks for the other spelling.
- The band's fixture root is fixed /tmp by design; the reasoning is written in the band,
  and a better shared-host answer (a per-checkout root) is welcome when one exists.
