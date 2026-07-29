# 2026-07-28 — DS4's tensor table, and the inversion it puts beside KAT-Coder's

Urs: **"do the same with the deepseek ds4 model."**

Done, and the answer runs the other way — which is the finding.

| cell | verdict |
|---|---|
| [`form/form-stdlib/ds4-tensor-table.fk`](../form/form-stdlib/ds4-tensor-table.fk) | the record and the live read |
| [`form/form-stdlib/tests/ds4-tensor-table-band.fk`](../form/form-stdlib/tests/ds4-tensor-table-band.fk) | **255**, 2 s, live against the 85 GiB file |

## What the file says, in its own words

```
91 321 404 640 bytes · GGUF v3 · 71 KV pairs · 1406 tensors
tensor infos begin at byte 5 248 946

F32       0   536   no decode needed
F16       1   359   f16-decode.fk          COVERED
IQ2_XXS  16    93   iq2xxs-msl.fk          COVERED
I32      26     3   an index table         NO DECODER OWED
MXFP4    40    45   mxfp4-msl.fk           COVERED
MXFP8    41   370   mxfp8-msl.fk           COVERED
                    1406 total, nothing uncovered
```

Read through `equireach-gguf.fk`'s own walker over an 8 MB window — the header only, no weights, nothing
run. The file size is the same 91 321 404 640 that
[2026-07-22-first-deepseek-token.md](2026-07-22-first-deepseek-token.md) records, arrived at here by
asking the file rather than by copying the receipt.

The three I32 tensors owe no decoder and the cell says so rather than quietly counting them covered:
they are `ffn_gate_tid2eid`, the hash-routing lookup the first three layers select experts through.

## The inversion

| | readable by the world | readable by this body |
|---|---|---|
| **KAT-Coder-V2.5-Dev** | yes — llama.cpp runs it, ordinary Q4_K/Q6_K/Q8_0 | **no**, until this morning — Q3_K, 94 tensors |
| **DeepSeek-V4-Flash** | **no** — ds4, llama.cpp, ollama, LM Studio all refuse types 40/41 | yes — all 1406 |

The model every tool can read is the one this body could not. The model nothing else on this machine
will open is the one fully at home here.

And the consequence runs with it: **the file we can be refuted on is the one we cannot yet run, and
the file we can run is the one nothing can refute.** DS4's whole-forward output is unfalsifiable
against any external reference on this machine — which is why that work stands on 473 self-consistent
gates — while KAT-Coder could be checked token-for-token against llama.cpp the moment it runs.

## The most surprising teaching

**The body already had both halves of this, minted days apart by different hands, and had never put
them side by side.** `selfgauge` — a bound derived from within because nothing external can check it,
52 files, the word the DS4 stones lean on. `heteronomy` (row 899, written this morning) — a gate whose
criteria the gated did not author, the word for what llama.cpp would be for KAT-Coder.

I went looking for a fresh word for the inversion and found the corpus already holds the pair; what
was missing was not vocabulary but the case that shows them as opposites. That is the corpus working
rather than growing, and it is the fourth time tonight a frontier question's honest answer was a word
already home.

## Where discomfort turned to gold

I nearly wrote the band to assert `uncovered == 0` as a fact about DS4. That reads as a stronger
claim and it is a worse test: this file happens to have full coverage today, and a band phrased that
way would have to be **edited** the day a DS4 rebuild introduced a type we cannot decode — the same
trap the sibling band avoided from the other side, where the gap was real and asserting full coverage
would have forced a lie or a deferral.

So bit 8 asserts the uncovered count equals *what the record claims*, and bit 128 demands the walk
visit exactly as many records as the header declares. That second half is the one a hand-written
census most needs: a type present in the file and missing from the record leaves every per-row total
agreeing while the census is silently incomplete. The discomfort was noticing that "it's all covered"
is a sentence I wanted to write, and that wanting to write it was the reason to check it differently.

## The frontier question

> **What names a property that looks like it belongs to a thing but is a relation between the thing
> and its reader?**

Asked, and **not landed**. `selfgauge` and `heteronomy` (899) are the two halves already home;
`affordance` (4 files), `extrinsic` (1) and `legibility` (1) are all present. Minting a fifth word for
ground three already cover is how a corpus fills with synonyms.

## Ground stamp

```
./fkwu --src form/form-stdlib/tests/ds4-tensor-table-band.fk        -> 255  (2 s, live)
./fkwu --src form/form-stdlib/tests/kat-coder-tensor-table-band.fk  -> 255  (live)
```

Both models' tables are now read by the body itself rather than recorded from a scratch parse. No
Python or C entered either cell.

## Carried, not buried

The `.fkb` v5 heal grew `runtime/fkwu-uni.c` by +50 lines net. It carries a shrink note in the source
naming where the capability belongs; it still owes a dated shrink receipt.
