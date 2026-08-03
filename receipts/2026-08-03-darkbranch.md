# 2026-08-03 — "REFUTED, and kept as the falsifier" was written in my own hand

Urs: *"clean up all the switches and knobs and knowledge base and memory to set the record straight
and help ground that the only limit are the physical hardware and our imagination."*

## The record, set straight

**Memory.** `MEMORY.md` said the `.fkb` cache bug was *"FIXED 2026-07-28 (.fkb v5 carries a builder
identity)"*. False, and I proved it false yesterday without updating it: v5 keys the **builder**, not
the **source bytes**, so it cannot see a same-second edit at all. Corrected, with the mechanism
(`st_mtime` whole seconds; reuse test `fkb_mtime >= unit_mtime`; "source hash" covers path+mtime,
never content) and the consequence: **every fast mutation battery in this body is suspect.**

**New standing memory** `capability-not-configuration` — all silicon, all platforms; an issue is
unfinished work, never a limit; probe before writing "cannot"; don't add switches.

**The body now states its own knob count.** `form-stdlib/switch-census.fk` carries the census, the
disposition rule, and a *ratchet* — `switch-census-band.fk` (verdict 63) fails if the count rises.
A target is a wish; a ratchet makes an addition cost a deliberate edit with a reason beside it.

```
opened 2026-08-03   78    (69 FORM_DS4_* + 9 FORM_CLI_*)
after first pass    71
  input      (keep)  13   which model, which prompt, how many steps — arguments, not knobs
  instrument (fold)  10   changes what is REPORTED, never what is COMPUTED
  experiment (delete) 48  ← the work
```

## What the first four deletions found

Two of them carried comments saying, **in as many words**, *"REFUTED, and kept as the falsifier"*:

- `HZ_SCOPED` — `memoryBarrier(resources:)`: 1844 barriers/token against 1510, 39.14 ms against
  38.17. The finer claim is true and the hardware does not reward it.
- `PAD` — N empty dispatches per layer to price an ask: slope **1.1 µs**, which settled that dispatch
  *count* is not what a token costs (removing 129 made it slower; adding 86 made it faster).

Both findings are real and both belong in the record. **Neither needed to stay executable.** I had
told myself a losing branch is a falsifier — keep it and the refutation can be re-run. But the
refutation lives in the *numbers*. The branch is a second program maintained in the dark: never
exercised, never verified, rotting against every change to the path that won, and counted forever in
the census of questions this body asks its caller.

The other two were quieter and worse. `MOE_FUSE2` and `Q80_CPU_ORDER` were *comments promising an A/B
whose implementation had already been deleted* — the tree offered a switch that did not exist.

## The deletion changed nothing, which is the proof

```
                    baseline    after
PASS lines            113        113
gates            VERDICT PASS 101   VERDICT PASS 101
decoded stream        STREAM IDENTICAL (diff empty)
```

That is the disposition rule demonstrated: **the default already IS the decision.** Whatever a switch
does when unset is what we ship, every day, to everyone — so the other branch is a decision made and
never written down. Inlining the default and deleting the loser is behaviour-preserving by
construction, and the run proves it rather than the argument.

## Ground stamp

```
host M4 Max, 2026-08-03, worktree google-turboquant-vector-search-300c68
census   grep -rhoE 'FORM_(DS4|CLI)_[A-Z0-9_]+' --include=*.sh --include=*.fk | sort -u | wc -l
         78 -> 71 ; 57 of the DS4 set actually read by metal_dsv4_stack.sh
removed  FORM_DS4_HZ_SCOPED (env read + else-branch), FORM_DS4_PAD (env read, padBuf,
         padDispatches, call site, 2 stale comment cites), FORM_DS4_MOE_FUSE2 and
         FORM_DS4_Q80_CPU_ORDER (comment-only promises of deleted code)
verify   FORM_DS4_GATES=1 KV_SEQUENCE=1 KV_STEPS=2 KV_CAP=8, before and after
         113 PASS both; 101 gates VERDICT PASS both; decoded continuation diff empty
cells    switch-census.fk (ratchet 71), tests/switch-census-band.fk 63,
         host-capability.fk CARRIED/UNFINISHED, no category for "limit"
memory   MEMORY.md fkb line corrected from "FIXED"; new capability-not-configuration
corpus   372 rows, max-mid 977, field 3723722977, 0 duplicate ids, band 32767
```

## The most surprising teaching

**My own band was wrong and the cell was right.** I wrote a bit asserting "no sentence in the law
contains the word *limit*" — and the law violates it twice on purpose, because *"the only limits are
the physical hardware"* and *"never a limit"* are the entire point. The band scored 31 instead of 63
and for a moment I read that as the cell being off-message. The test encoded a slogan
("avoid the word") where the cell encoded a meaning ("limits are hardware and imagination"). A check
that pattern-matches vocabulary will fail exactly the writing that states the principle most
directly.

## Where discomfort turned to gold

Reading *"REFUTED, and kept as the falsifier"* in a comment I wrote, and recognising it as a rule I
had invented to avoid deleting my own work. It reads as rigour — the losing branch preserved, the
numbers cited, the A/B reproducible. What it actually produced was 48 remaining experiments, a
configuration space instead of a body, and a day in which I told Urs three times that we could not do
things we could already do. **A falsifier you cannot afford to run is not a falsifier; it is a costume
worn by dead code.** Corpus row 977, `darkbranch`.

## Unfinished, named and sequenced

1. **48 experiment switches remain.** Method proven and behaviour-preserving; harness is
   `FORM_DS4_GATES=1 KV_SEQUENCE=1 KV_STEPS=2 KV_CAP=8` against 113 PASS / 101 gates / identical
   stream. Each pass: inline the default, delete the loser, re-run, lower `sc-ceiling`.
2. **10 instruments want one door**, not ten flags — and two of them (`SKIP`, `DOUBLE`) are known
   liars that should carry their own warning or go.
3. **`.fkb` freshness must hash content**, not mtime. Until then no fast edit-run loop here is
   trustworthy — this is the one that makes every other verification honest.
4. **`cuda_matvec_f32` is Windows-only.** "All silicon" is not yet true on Linux/NVIDIA.
