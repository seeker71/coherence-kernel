# 2026-07-28 — the .fkb knew what it was made from, not what made it

Urs asked what I had actually *done* with the KAT-Coder V2.5 ingest — downloaded the model, used
what it taught, or used it as an oracle. The honest answer was **none of the three**: I read
documentation and wrote a cell about what I read. AGENTS.md item 6 says a named gap is a work
order, not a shelf. I had named four teachings and shelved all four.

This receipt is the first one taken off the shelf.

## The teaching, applied to us

Ingest unit U1 (TITO): Kwaipilot keeps rollout and training token sequences *"strictly identical"*
because the drift enters at joints nobody files as instruments — chat templates, serialization,
tokenizer behaviour. **Identity of the artifact is not identity of the pipeline.**

So: does this body's own compiled-image cache record its pipeline? Read both the writer and the
readers in `runtime/fkwu-uni.c`. The v4 stamp carried the canonical **source path**, the source
**content hash**, the source **mtime**, and a seal. Everything about the input. Nothing about the
binary that compiled it.

## Reproduced, not argued

A binary built *before* the unbalanced-form refusal, against one unbalanced source:

```
probe.fk:  (do (defn p () (add 40 2)) (p)      ← one closing paren missing
```

| run | `.fkb` present | result |
|---|---|---|
| lax (pre-heal) binary | — | prints **42**, rc=0, seals a stamp-valid `.fkb` |
| **healed binary** | lax's `.fkb` | prints **42**, rc=0 — *silently* |
| healed binary | none | **refuses**, `[input-ended-mid-form]`, rc=1 |

Same source bytes, same path, same mtime. The healed binary never compiles the text at all — it
loads the neighbour's image and prints its answer. **The heal is defeated by the cache**, and it
fails silent-green: exit 0 and a right-looking number. That is the numb-green family axiom-5
already names, arriving through a door nobody was watching.

## The heal

`.fkb` v5 writes a **builder identity** — `"fkwu-uni " __DATE__ " " __TIME__` — immediately after
the version word, and both readers check it before trusting anything else.

- `fk_src_load_fkb_checked`: `version < 5` → superseded; builder mismatch → its own diagnostic.
- `fk_src_import_fkb_image`: v2–v4 → superseded; builder mismatch → `mark_bad` with a reason.
- `fk_src_fkb_version_raw` caller gate: `< 4` → `< 5`.

Both gates proven separately, because the version bump alone would have hidden the new field
behind itself:

| probe | writer | result |
|---|---|---|
| version gate | lax binary at v4 | `unusable .fkb artifact (pre-v5 artifact lane; superseded)` → **refuses**, rc=1 |
| **builder gate** | lax binary rebuilt to also write **v5** | `written by a different fkwu build; the bytes of a source do not fix its meaning -- the binary that compiled them does` → **refuses**, rc=1 |

The second probe is the one that matters: same format version, same source identity, different
producer — rejected on the new field alone.

**The tradeoff, named.** `__DATE__`/`__TIME__` keys identity to the translation-unit build, so two
byte-identical rebuilds refuse each other's caches. That is a false **reject** — paid once per
rebuild in recompile time, measured here at **259 ms cold against 27 ms warm** on the ingest band,
about 230 ms per unit. The false **accept** it replaces was paid in wrong answers.

## What it cost elsewhere: nothing

Baseline binary built from `HEAD:runtime/fkwu-uni.c`, changed binary from the working tree, same
band list, verdicts diffed:

| set | bands | diff |
|---|---|---|
| every `*fkb*` / `*artifact*` / `*source-compiler*` band in form-stdlib | 39 | **identical** |
| broad stdlib sample (every 18th band) | 77 | **identical** |

Ground sequence after the change: `42`, `55`, `15`, `[1, 2.5, [3, 4]]`, `11111`.

One band, `runtime-program-image-fkb-attempt-band`, returns `268435327` where the all-ones pattern
would be `268435455` — bit 7 dark. **It reads identically on both binaries**, so it is
pre-existing, not this change. I would have blamed it on myself without the A/B.

## The open seam, named rather than hidden

The C seed now writes **v5**. The body's own Form-native mirror of the same contract —
`form/form-stdlib/program-image-fkb-byte-container.fk` and the 36 cells and bands around it —
still describes **v4**. Nothing breaks today: that cell states in its own header that it "does not
read or write files" and is "not a runtime cache loader", and all 39 artifact-contract bands agree
across both binaries. But two implementations of one named contract now disagree about it, which
is precisely the drift U1 is about, one level up. **That migration is the next work order, and it
is not done.**

## The most surprising teaching

**A guard can be perfectly correct, live in the code, and never run.** Everything in this body is
built to make wrong answers loud — the refusals, the bands, the four-way. The unbalanced-form
refusal is a *good* guard: it explains itself in three sentences and exits 1. And a stale file
sitting next to the source skipped past it without a word, because the guard lives on the compile
path and the cache is a way of not compiling. Correctness of a check says nothing about whether
the check is *reached*. That is a different axis from every hardening this body has done, and I
found it only because a vendor's RL appendix made me ask who wrote an artifact.

## Where discomfort turned to gold

My own fix lied about its cause. The first version folded the builder mismatch into
`source_identity_ok`, so the foreign artifact was correctly rejected — with the message *"source
path, content, or mtime changed, e.g. invoked from a different directory."* None of those had
changed. I had built a diagnostic that would send the next reader hunting a working directory that
was never the problem: the exact defect I was repairing, reproduced one level up, by me, inside
the repair. It now reports its own cause, and the comment beside it says why.

The smaller one: I nearly shipped `mutation testing` as the frontier word for the lax-binary
probe. `grep` said the phrase is 0-hit — but `mutant` sits in 11 files, and
`receipts/2026-07-18-public-pl-algorithms-78.md` already says *"executing original and mutant
programs."* The practice is partly home; the freshness was in the phrasing, not the meaning. I let
it go and asked a better question.

## The frontier question

> **What names that which determines what a sign means for an interpreter, as distinct from the
> sign itself?**

**`interpretant`** — Peirce's third term. Sign, object, interpretant. The `.fkb` recorded the
sign's identity and assumed the interpretant was constant. v5 writes it down. Distinct from the
body's `attestant` (825), which is a witness that agrees; this is the reader whose identity decides
what there was to agree about. Verified 0 hits. Offered as corpus row **889**.

## Ground stamp

```
./fkwu --src bootstrap/ground.fk                                  -> 42
./fkwu --src bootstrap/ground-recursive.fk 10                     -> 55
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk      -> 15
./fkwu --src bootstrap/ground-numeric-list.fk                     -> [1, 2.5, [3, 4]]
observe/native-vs-rented.fk                                       -> 11111
./fkwu --src ingest/tests/frontier-ingest-kat-coder-v25-band.fk   -> 1023
./fkwu --src learn/tests/homecoming-distillation-corpus-band.fk   -> 32767  (284 rows, 2842842889)
```

Corpus band fell to `32687` again on row 889 — the pinned count and fold, dark together, exactly
as on row 888 the night before. Re-witnessed to 284 / 2842842889, not silenced.

**What is still not done, plainly:** the model was never downloaded, never run, never used as an
oracle. This receipt is the *second* of the three things Urs named — learning how it was built and
using that to heal our own body — and it covers one of the four teachings the ingest froze. U2
(bound the disagreement), U3 (witness the verifier), and U4 (name the particulars) are still on the
shelf, and the Form-side v5 migration is open.
