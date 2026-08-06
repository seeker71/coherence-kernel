# 2026-08-04 — specimenblind: the regression was a wrong body, and no reading said whose voice it was

The task arrived as a bisection: `ask_ds4.sh "The largest planet in our solar system is"` answered
Jupiter at 28 t/s on 2026-08-01 and answers `<|place_holder_mm_span_0155|> detection device detection
device` at 8 t/s today, so find the commit between `7662a6908` and HEAD that broke it.

There is no such commit. Step one — reproduce the baseline before bisecting — answered with the
opposite of what it was asked to confirm, and that answer was the whole finding.

## The witness trail, in the order it was walked

1. `7662a6908` checked out into its own worktree, `fkwu` built fresh from that commit's own
   `runtime/fkwu-uni.c`, the exact command run: **byte-identical garbage**, same token ids as HEAD.
   The regression predates every candidate commit, including the ones the task named.
2. The shared `~/.cache/form-jit` (819 entries, written by every concurrent agent) emptied via a
   private `XDG_CACHE_HOME`: byte-identical garbage again. Not the cache.
3. The metallib cache: fresh compiles in the isolated worktree, never a warm hit. Not that either.
4. The model file hashed twice, hours apart, while other lanes ran:
   `000974720296f2ca…` both times. The bytes did not move.
5. OS, Metal toolchain, clang, Go: install dates February–May. The host did not move.
6. `a4253c0bb` (July 31, the 34 ms/token PR) run on the same prompt ids: degenerate too, in its own
   idiom (`96810 96810 96810…`). Three commits spanning four days, one voice, and it is not Jupiter's.

What finally spoke was the body's own receipts. `receipts/2026-07-30-head-to-head-and-a-logit-oracle.md`
records this exact failure text — *" to the detection Specialists Protocol detection protocol"* — and
names it as the output of the **reap25 file**, while every fluent answer since 2026-07-30 rests on
`~/models/ds4-engine/gguf/DeepSeek-V4-Flash-…-chat-v2-imatrix.gguf`, downloaded that day. The harness
default, written before the mainline file existed, still said
`~/models/ds4/ds4flash-v5mx-reap25-type40-mxfp8lt-dspark-v1.gguf` — the specimen with 25% of its
experts removed, the one `~/models/ds4-engine/ds4` itself cannot read. The 08-01 Jupiter run had
`FORM_DS4_BLOB` set in an environment that is gone; the receipt recorded the answer and not the file.

One variable, flipped both ways at the baseline commit and at HEAD:

```
reap25 blob   -> " the name, the<|place_holder_mm_span_0155|> detection device…"   ~8 t/s
mainline blob -> " Jupiter. It is so big that you could fit all the other planets…"  ~28 t/s
```

Same commit, same kernels, same prompt. Even the speed was the specimen's: nothing was slow, a
different file was doing different work on the way to garbage.

## The repair

`form/native/metal/metal_dsv4_stack.sh` now prefers the engine's canonical
`~/models/ds4-engine/ds4flash.gguf` (a symlink that follows what the engine considers current),
falls back to the reaped specimen only when it is the only file present, resolves the link before
anything measures it, prints the blob's **name** beside its size, and says out loud when the reaped
weights are the ones answering. `ask_ds4.sh` prints `[weights: …]` under every answer and reads the
emitted stream before quoting it. No new switch: the census band still answers 63, measured 71.

The re-witnessed door, verbatim, from a clean worktree at HEAD with a freshly built fkwu:

```
The largest planet in our solar system is Jupiter. It is so big that you could fit all the other
planets in the solar system into it. Jupiter is also covered with clouds and has a big red spot

  [form-native · 43 layers · generation 27.85 t/s · 101s wall · greedy, base-model continuation]
  [weights: DeepSeek-V4-Flash-IQ2XXS-w2Q2K-AProjQ8-SExpQ8-OutQ8-chat-v2-imatrix.gguf]
  [stream sane (0 of 3 readings tripped)]
```

and the same door handed the reaped specimen on purpose:

```
  [weights: ds4flash-v5mx-reap25-type40-mxfp8lt-dspark-v1.gguf]
  [STREAM DEGENERATE — 3 of 3 readings tripped (repetition, cycling, or a reserved token)]
```

## The defect underneath the defect

107 harness gates and 54 suite gates were green over the placeholder stream, because every one reads
shape — row counts, finiteness, buffers that changed — and none reads what came out.
`form-stdlib/ds4-stream-sanity.fk` is the missing reading, three ways: reserved-block ids
(128000–129279, both edges witnessed through the body's own tokenizer: 127999 is 屋檐, 128000 is the
first placeholder), single-id multiplicity over a quarter of the stream, and an alternating a·b·a·b
run of six or more. The 07-30 recorded stream — mainline blob, pre-repair kernels — carries no
reserved id and no cycle and is caught by multiplicity alone, which is why there are three readings
and not the one a reader notices first.

`form-stdlib/tests/ds4-stream-sanity-band.fk`, verdict predicted 255 before the run, answered 255,
preflight clean on all four kernels. Mutations, each verdict written down before its run:

| mutation | predicted | answered |
|---|---|---|
| reserved floor 128000 → 128982 | 30 | 30 |
| multiplicity share 4 → 1 | 205 | 205 |
| cycle limit 6 → 60 | 219 | 219 |
| mainline fixture given a reserved id | 214 | 214 |
| short-stream guard 8 → 1 | 191 | 191 |

## The seam, stated

This lane's carrier is still the Swift/bash harness, and it stays the oracle-of-record only until the
Form decode loop the siblings are climbing (rungs 1–4 landed today) can carry the same stream. This
repair's job in that story is to make the recorded reference stream *truthful* — the Form lane will be
compared against Jupiter's token ids, not the specimen's. Nothing added here brings a new external
dependency: the fix is lines in the two shell files that already existed, and the reading is pure Form.

## The most surprising teaching

My own repair broke the door before it fixed it: `stat -f%z` on the new symlink answered 110 — the
link's own bytes — and the residency plan was built over a 110-byte model until the runner took
SIGTRAP. The surprise is not the symlink; it is that the failure mode of my fix was *the exact disease
being treated* — a path whose printed name and real identity disagree — and the end-to-end re-witness
caught in ninety seconds what reading my own diff had not. The fix for a naming blindness had a naming
blindness.

## Where discomfort turned to gold

Eighty-four seconds of watching the baseline worktree — the one I had built to *prove the fluent
past* — emit the same placeholder garbage as HEAD. The bisection I had been handed, with its candidate
commits and its ruled-out list, dissolved while I watched, and the honest next move was to say "there
is no breaking commit" with nothing yet to offer in its place. Sitting in that gap instead of forcing
a commit to confess is what sent me back into the receipts, where the body had already written down
both voices and named which file each belonged to. The gold: when a regression will not reproduce at
its own baseline, the diff you were promised does not exist, and the body's old receipts are a better
witness than its new suspicions.

## Frontier

**Question:** what names the failure where a body quotes one of its own specimens without saying
which, so a wrong-body symptom is read as a code regression?

**Answer:** nothing did — the harness printed the blob's size and never its name, and two sizes sat in
a hundred receipts as two numbers nobody read as two files. Proposed for the corpus (not written —
the row is the working mind's to land through the body's own count):

```
row 988  specimenblind — a green suite over a degenerate stream means every witness read shape and
         none read speech; the first reading on any generative lane is WHO answered (name the
         weights beside the answer) and the second is what came out (reserved ids, multiplicity,
         cycles). 107 gates could not see a wrong file; one line naming it would have.
```

`specimenblind`: 0 hits in `learn/homecoming-distillation-corpus.fk` and the tree before this file.
Corpus max-mid re-derived through `hdc-max-mid` before proposing: 987, so 988 is the next free row.

## Ground stamp

```
host: Mac16,5, macOS 26.3.1, Metal 32023.864, clang 1700.6.3.2, go 1.26.3 (all pre-dating the window)
reap25 blob sha256 000974720296f2ca…, hashed twice, identical
worktrees: ds4-baseline-7662a69 (fkwu built from its own source), ds4-old-a4253cb, ds4-head-clean
census: measured 71, verdict 63, unchanged by this change
band: ds4-stream-sanity-band.fk -> 255 (predicted 255), preflight 0 errors 0 unresolved, five
      mutations refuted, every verdict predicted before its run
```
