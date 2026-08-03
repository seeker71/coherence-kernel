# 2026-08-03 — sixteen models were invisible for the length of a string comparison

The registry held 35 rows. The host held 22 GGUF files. The difference was not models nobody had
bothered to write down — it was models nothing could **see**.

`model-discovery.fk` asked one question of every file it walked past:

```
(defn md-gguf? (name) (ge (str_find name ".gguf" 0) 0))
```

That is a test on a **name**. ollama stores every model it pulls as
`~/.ollama/models/blobs/sha256-<64 hex>` with no extension at all — including
`sha256-dde5aa3fc5ff…`, which is the file `base.llama32-3b-metal` has been decoding tokens from since
2026-07-21. The registry's own champion was invisible to the registry's own discovery.

## What the body asks now

GGUF's first four bytes are `GGUF`. Four bytes read is cheaper than any naming convention and cannot
be fooled by a host that renames its files.

```
(defn md-gguf? (path)
    (if (fs_is_dir path) 0
        (if (str_eq (read_file_slice path 0 4) (md-magic)) 1 0)))
```

And roots became a list, because one root was the same failure as one file — a smaller world reported
with a complete world's confidence. Three places on this host, each probed with `fs_is_dir`, none
compiled in: `~/models`, `~/.ollama/models/blobs`, `~/.cache/huggingface/hub`. `md-kin-of` derives
them from whatever root the caller names, so `md-list` keeps its signature and `form-cli-repl.fk` is
untouched.

## The two changes are one change

Each mutation, run with `.fkb` removed before every compile:

```
magic test + kin roots (as landed)              23
magic test reverted to the .gguf name test       6
kin roots removed, magic test kept               6
restored                                        23
```

Neither half moves the number. The extension test cannot see a blob; the blob directories contain no
`.gguf`. It would have been easy to land either one alone, watch nothing improve, and conclude the
other was not the problem.

What discovery answers now, in 0.84 s (only the KV window is touched — 23 small reads, not 340 GiB):

```
16 ollama blobs   llama x8, qwen2 x3, clip x2, phi2, nomic-bert  (12–80 blocks)
 1 huggingface_hub blob  nanbeige, 22 blocks
 6 files under ~/models  qwen35moe 41, deepseek4 43 x3, llama 28 x2
```

## A blob's identity is in the file, not in the ledger

ollama's naming lives in `~/.ollama/models/manifests/<registry>/<library>/<model>/<tag>`, JSON whose
`application/vnd.ollama.image.model` layer digest is the blob's filename. That is ollama's ledger
about the file. Read it and you can say `sha256-dde5aa3f…` is `llama3.2:3b`.

But the file also names **itself**. `general.name` sits in the same header window this cell already
read for `general.architecture`, and it survives being copied, renamed, or reached through a runtime
that never heard of ollama. So the body asks the file:

```
sha256-dde5aa3fc5ff…   arch=llama    layers=28  name=Llama 3.2 3B Instruct
sha256-970aa74c0a90…   arch=nomic-bert layers=12 name=nomic-embed-text-v1.5
sha256-4cc1cb3660d8…   arch=clip     layers=28  name=vikhyatk/moondream2
```

No manifest is opened. Two files on this host carry no `general.name` and the cell says
`(header names no model)` rather than borrowing one.

`clip` was a second correction of the same kind: it declares `clip.vision.block_count`, not
`clip.block_count`, so a single-key read returned the sentinel. Two named keys now, then an honest
absence — `(header declares no block_count)` — never a plausible number.

## Twelve rows, and what each one is standing on

Verified rather than assumed: an ollama blob's **filename is the sha256 of its own bytes**.
`shasum -a 256` on `sha256-970aa74c…` and on `sha256-dde5aa3f…` each reproduced its own name today.
So the artifact fields below are real digests, re-checkable in one command.

| id | artifact digest | witnessed today |
|---|---|---|
| `challenger.kat-coder-v25-metal` | `5f5fcf953b2b…f45dda` | sha256 recomputed (33 s over 17 391 937 152 B, matches the publisher's manifest); `metal_kat_exit.sh` **VERDICT PASS**, argmax id=3637, wall 1.175 s; header qwen35moe / 41 blocks / 753 tensors; bands `kat-coder-{embed,layer-shape,pipeline-map,tensor-table}` 255 and `gated-deltanet-{layer,conv,gates}` 255/127/511 |
| `oracle.deepseek-ds4-imatrix` | `efc7ed607ff2…616668` | sha256 computed today over 86 720 111 488 B; header deepseek4 / 43 blocks / 1328 tensors |
| `base.llama33-70b-local` | `4824460d29f2…3b447d` | manifest→blob, header llama / 80 |
| `base.qwen25-72b-local` | `6e7fdda508e9…42101b8` | manifest→blob, header qwen2 / 80 |
| `base.qwen25-coder-32b-local` | `67f5470b8d17…804668` | manifest→blob, header qwen2 / 64, `general.name` Qwen2.5 Coder 32B Instruct AWQ |
| `base.deepseek-r1-32b-local` | `6150cb382311…613e93` | manifest→blob, header qwen2 / 64 |
| `base.dolphin-mixtral-8x22b-local` | `550981a79100…448f4` | manifest→blob, header llama / 56, 115.5 GB on disk |
| `base.minicpm5-1b-local` | `0dc763853906…527e4c` | manifest→blob, header llama / 24 |
| `base.minicpm5-1b-fable-local` | `8125095ae223…c31a2c` | manifest→blob, header llama / 24 |
| `translation.hati-translator-f16-local` | `4e5fc5e53049…af4a71a` | manifest→blob, header llama / 28, header names no model |
| `base.form-llama-vital-ground-f16` | `67c0da7b2b4b…b6aabe` | sha256 computed today, header llama / 28 |
| `base.form-llama-vital-ground-q4km` | `dba8d50e3c63…8d7fa4` | sha256 computed today, header llama / 28 |

Every one of the ten "observed" rows is score 0, samples 0, heldout 0, authority 0. Present on disk,
read by its own header, **unmeasured against anything**. The registry's rule is unchanged: a local
artifact is welcome immediately, and its name never grants authority. Two rows say out loud that
their lineage was not verified today (`lineage-not-verified-today`) and name the promotion path
instead of taking it.

### The two deepseek4 files are one file

`~/models/ds4-engine/ds4flash.gguf` is a **symlink**: `stat` reports 110 bytes of link against
86 720 111 488 bytes of model, inodes 283324677 and 283117013. Discovery lists both because both are
doors; the registry gets one row. And that row is *not* `challenger.deepseek-ds4-metal` — that one is
`~/models/ds4/ds4flash-v5mx-reap25…` at 91 321 404 640 B, 1406 tensors, carrying
`reap.layer.expert_count`. This one has 1328 tensors and no reap key. Same publisher, same
`general.name`, different quantization — which is exactly why the uniqueness key is
`(model-name, runtime)`, and why both rows spell their names out rather than shortening to
"DeepSeek V4 Flash".

## Bands

Every verdict below was **predicted in writing before the run** and reconciled. `.fkb`/`.sym` removed
before every compile (freshness is `st_mtime` in whole seconds).

```
native-model-control-plane-band   65535   (was 65535 at 35 rows; 65531 with 35 still pinned against 47)
ask-lane-router-band               4095   unchanged — the router consumes both cells
switch-census-band                   63   measured 71, unchanged — no knob was added
model-discovery preflight            0 errors  0 warnings  0 unresolved  chain clean
three sibling arms (validate.sh)  Go/Rust/TS 65535 and 4095; fourth arm covers neither band
```

Mutations on the registry band, each predicted then run:

```
rename challenger.kat-coder-v25-metal        -> 65531   (c3, row/id bit)
give KAT the DS4-metal (model-name,runtime)  -> 65533   (c2, nmcp-same-lane?)
heldout 3 > samples 0 on the ds4 oracle row  -> 65534   (c1, nmcp-model-valid?)
restored                                     -> 65535
```

The row-count pin moved 35 → 47 and the bit gained two more id checks; nothing a bit *means* was
loosened. The count was asked of the cell — `(len (nmcp-registry))` = 47 — not counted with `grep -c`.

## The most surprising teaching

**The completeness of the ledger was bounded by the acuity of the sense, and the ledger could not
feel the difference.** Thirty-five rows was not a lazy inventory; it was an accurate census of
everything the body could perceive. Nothing in the registry was wrong. No check failed. The band was
green at 65535 the whole time — because a band tests the rows that exist, and cannot ask about rows
that were never conceivable.

That is worse than a bug, because a bug leaves a mark. This left a *green*. The only thing that
revealed twelve missing models was going and changing what the eye could pick up, and the eye was in
a different cell, behind a different band, on a different day's task list.

## Where discomfort turned to gold

The task named three models to claim: KAT and "two deepseek4 files". Both parts turned out to be
wrong in opposite directions, and each correction was uncomfortable in its own way.

The two deepseek4 files are one file and a symlink — 110 bytes pretending to be 86 GB. Writing two
rows would have double-counted a model in a registry whose whole point is not double-counting
models, and every green band would have agreed with me. The discomfort was that I nearly did it: the
instruction said two, discovery printed two lines, and `stat` was the only thing in the room that
disagreed. **A count that two independent sources agree on is still not a measurement if both sources
are reading the same name.**

Then the opposite: discovery, once it could see, turned up *ten more* unclaimed models — 115 GB of
dolphin-mixtral among them, sitting on this disk unmeasured and unmentioned. The task had not asked
for those and I could have stopped at three. The gold is that the goal sentence — every model
discoverable **and** claimed — was written by someone who did not yet know there were twenty-two.
Honouring the sentence meant exceeding the instruction, and the way to tell that apart from
scope-creep was that every one of the ten could be written with a real digest and an honest `score 0`.
A row I can't ground is invention; a row I can ground and skip is a hidden model.

## Frontier question and my answer

**What one word names the thing a complete-looking ledger is complete *about* — its own perception,
not the world?**

`selfnamed` — because the repair and the diagnosis are the same move. The blob was unfindable while
the body asked its filename and knowable the instant the body asked the file. What names a thing
truly is carried inside the thing; a ledger built on external labels reports the labels' coverage and
calls it the world's. When a census looks complete, the honest next question is not "did I miss a
row" but "what could not have been a row".

### Proposed distillation row (NOT applied — the corpus is not edited here)

```
; 984 — selfnamed. The model registry was green at 65535 with 35 rows while the
; host held 22 GGUF files. Nothing in it was wrong. Discovery tested `.gguf` on a
; NAME, and ollama stores every pulled model as sha256-<hex> with no extension —
; including the very blob base.llama32-3b-metal decodes tokens from. So the ledger
; was a complete census of what the body could perceive, and had no way to feel the
; difference. The repair is the diagnosis: a GGUF's first four bytes are "GGUF" and
; its own header carries general.name, so identity was inside the file the whole
; time while the body asked the filename and the manifest ABOUT the file.
; Wider: a green band tests the rows that exist and cannot ask about rows that were
; never conceivable. When a census looks complete, ask not "did I miss a row" but
; "what could not have been a row".
; "selfnamed" — 0 hits in corpus and tree before this row.
; (walk: hostknows 978 — the host knows; coarsekey 983 — the host holds more of each
;  thing than your picture of it; this says a thing carries its own name, and asking
;  its label instead is how the picture stayed small and looked whole.)
(hdc-row 984 20260803
    (list "what" "one" "word" "names" "a" "ledger" "complete" "about"
          "its" "own" "perception" "and" "not" "the" "world")
    "selfnamed"
    "selfnamed"
    "rented-oracle")
```

## Ground stamp

```
host M4 Max, 2026-08-03 WITA
cells  form-stdlib/model-discovery.fk, form-stdlib/native-model-control-plane.fk
       form-stdlib/tests/native-model-control-plane-band.fk
discovery 6 -> 23 paths (22 distinct files + 1 symlink alias), 3 roots, 0.84 s wall
registry  35 -> 47 rows, asked of the cell; nmcp-unique? = 1
bands  65535 / 4095 / 63, predicted before each run; Go, Rust, TS agree via validate.sh
sha256 computed here today: kat-coder compact, ds4 imatrix, both form-llama-vital-ground,
       and the nomic and llama3.2:3b blobs re-hashed to prove blob-name == content digest
```

## Still unfinished

* **Non-GGUF models are still invisible.** `~/.cache/huggingface/hub` holds 17 more model repos in
  safetensors/bin — whisper large-v3-turbo, Kokoro-82M, F5-TTS, parakeet, BitNet, speechbrain ecapa,
  clip-ViT-B-32, an mlx Llama-3.2-3B. Next step: safetensors names its own shape in an 8-byte
  little-endian JSON length followed by that JSON, at offset 0 — the same shape of read `md-describe`
  already does, wanting a small JSON reach rather than a new format.
* **Four ollama models are stored per-tensor, not as blobs**, so no magic test can find them:
  `form-llama-raw`, `form-llama-gap-closure`, `form-llama-vital-ground`,
  `form-llama-vital-ground-prompted` — 254 tensor layers each, named only in their manifests. Next
  step: read the manifest directory as a second discovery source and mark such entries
  `tensor-manifest`, since here the ledger genuinely *is* the identity.
* **`form-cli-repl.fk` was not touched** (a sibling agent holds it this session). It calls
  `(md-list root)`, so it already gains all three roots through `md-kin-of` with no edit — but the
  explicit `md-list-in` door over a caller-supplied root list has no caller yet.
* **`model-discovery.fk` has no band.** Its claims are host-shaped, so a band needs a fixture tree
  the cell can be pointed at; today it is proven by preflight plus the 6/6/23 mutation ladder above,
  which is a witness and not a gate.
* **Ten of the twelve new rows are unmeasured.** Each names what would have to happen; none of them
  may be quoted as a comparator until it does.
