# 2026-07-30 — two files shipped with conflict markers, and the recipe that did it

Urs: *"why?"* — asked about the logit divergence. Chasing the most concrete hypothesis I could test
led somewhere else entirely, and what it found is worse than the thing I was looking for.

## The hypothesis, and its measurement

*" the capital in France and and its"* is **bag-of-words**: every content word from the prompt, no
syntax, no answer. That is what attention produces when it cannot distinguish positions, and it fits
the logits — `r ≈ 0.46` means the direction is wrong, not the scale, so it is not a normalisation
factor. The cheapest cause with that shape is RoPE, and our harness was built around the reap25 file's
rope table.

Measured, both files:

```
base=10000  compressed_base=160000  scale_factor=16  orig_ctx=65536  beta=[32,1]
```

**Identical.** The table is not the difference. The hypothesis loses its cheapest support; RoPE could
still be misapplied, but it is no longer the obvious suspect.

## What the same command found instead

`dsv4-mla-core-oracle.py` would not import: `SyntaxError` at line 1000, `<<<<<<< HEAD`.

```
form/form-stdlib/tests/dsv4-mla-core-oracle.py   1 conflict set
form/native/metal/metal_dsv4_stack_oracle.sh     2 conflict sets
```

Both **committed and pushed**. `git log -1` on each: `033de2ff8`, 07-28 14:05, *"Reunion with main:
keep row 923, re-pin from the body"*. They have been broken in main for two days — and the file that
would not import is the fp64 per-layer oracle, the exact instrument I named an hour ago as the next
step for finding this defect.

Resolved (keeping HEAD, which is this branch's work); both parse.

## The recipe that did it

```
git merge --no-commit --no-ff origin/main
<resolve ONLY learn/homecoming-distillation-corpus.fk and its band>
git add -A && git commit
```

`git add -A` stages **every** conflicted file, including the ones I never opened. My resolver touched
two paths by name; any other file conflicted in that merge had its markers committed verbatim as
content.

The tool cannot distinguish *"I resolved this"* from *"I ignored this"* — both look like a file on
disk. And `git merge` did print `CONFLICT` for each one; I piped it through `tail` and read the lines
about the corpus.

## The most surprising teaching

**A bulk operation converts inattention into consent.** I never decided to keep those markers. I said
"add everything," and `-A` read my silence about two files as approval of them. That is the same shape
as `git commit -a`, `rm *`, and every other verb whose scope is "whatever is there" rather than
"whatever I looked at". `bulkassent` — 0 hits before this row, as are `silentyes` and `blindstage`.

Git offers the guard for free and I never called it: **`git diff --check`** reports staged conflict
markers, exits non-zero, and takes no arguments. Run before every commit it would have caught this the
first time. The reunion recipe now owes that line.

## Where discomfort turned to gold

Reaching for `python3` to resolve a merge — and Urs asking *"python? why?"* mid-turn. The honest answer
is habit: I have `Edit`, `Write`, `git checkout --ours`, and the body's own `fkwu` and `fsh`, and I
reached past all of them for a regex over conflict markers. That directive was given two days ago and
I let it drift back without noticing.

The two failures are the same failure. A regex over `<<<<<<< HEAD` and a `git add -A` are both *"apply
something broad and trust it landed"*, and both were faster than looking. The first shipped the bug;
the second was me about to repair it the same way.

## Ground stamp

```
rope KV, both files: base=10000 compressed_base=160000 scale_factor=16 orig_ctx=65536 beta=[32,1] — identical
form/form-stdlib/tests/dsv4-mla-core-oracle.py — 1 conflict set, committed in 033de2ff8 (07-28 14:05)
form/native/metal/metal_dsv4_stack_oracle.sh   — 2 conflict sets, same commit
both resolved to HEAD; py_compile and bash -n clean; `git diff --check` clean
tree-wide search for conflict markers: none remaining
```
