# 2026-07-02 (02:47) — two seed walls down, the lineage restored, and honest numbers

## Ground

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c    # cc exit checked = 0
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src bootstrap/ground-recursive.fk 10                  # 55
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
./fkwu proof/four-way-run.tbl                                  # verdict 0 (FOUR-WAY)
```

Urs, 02:38, going to sleep: "you are not tired, I am, you can keep going." Kept going.
The target: the evaluator-side blocker named an hour earlier. Found and fixed — and it was
two walls, not one.

## Wall 1 — the bare-root let-slot exposure (the historic bug, closed)

`receipts/2026-07-01-node-children-last-writer-wins.md`'s minimal repro — two lets holding
interned nodes, the earlier one reads back childless — root-caused to completion tonight:
`fk_walk`'s call frames are correctly allocated at the stack top (`fk_vp` → `b12 = fk_vsp-1`),
and defn bodies are correctly protected by a tag-111 reserve — but **the bare top-level root
never got that reserve**: `fk_run_src` set `fk_vsp = 1` while the root's lets held slots
above it, so the first nested call's frame landed on the live bindings.

**Fix:** give the root the same tag-111 wrap every defn body gets (`fk_run_src` +
`fk_run_feval`, after parse). Possible only because of tonight's earlier port of the
sibling's parse-time fix (f99d3232) — `fk_maxslot` now survives defn parsing and holds the
root scope's true slot count at wrap time. The two halves of the defect class needed each
other's fixes.

**Witness:** minimal repro `0 → 1`. Full ladder unchanged: 42, 55, 15, 11111, corpus band
127, strata locate 170003, translator 302301001, json 1023, four-way self-proof 0.

## Wall 2 — the 256-defn name-table ceiling (the "function-table ceiling", named at last)

The 258-defn learning chain still returned corrupted arithmetic after Wall 1 fell. Counted
at the boundary: 253-defn chain ran, 258 broke. `FK_TOP_FN_SYM_CAP = 256` bounded the
name→index registration table while FK_FN_CAP allowed 4096 — defn 257+ silently never
registered, and every call to it allocated a fresh bodyless index returning nothing. This
is the "direct-source function-table ceiling" that receipts had learned to duck under
(the Unicode tokenizer was sized to fit beneath it) without knowing its number.

**Fix:** `FK_TOP_FN_SYM_CAP` = FK_FN_CAP (4096); 96KB of arrays. Reproducer: garbage
`4337203685477580801 → 34` (the arithmetic-predicted value).

## The numbers, finally honest (full corpus, 100 epochs, 50 held-out, canonical core.fk prelude)

| embedding | held-out | reading |
|---|---|---|
| baseline (all words) | **36/50** | the receipted number REPRODUCES on healed ground — the lineage is restored |
| strata-stripped (grammatical removed) | 32/50 | honest regression |
| strata-weighted (content ×2, nothing removed) | 31/50 | honest regression |

The strata factoring, which took the symbolic locate from 2-wrong to 0-wrong (170003,
four-way), COSTS the tiny statistical learner. Intelligible, not embarrassing: on a 4-class
closed set the classes differ in their function-word signatures ("may/be" vs "alone" vs
"i am" vs "for/every") — surface grammar IS discriminative signal at toy scale. The design's
own caveat held: don't strip, FACTOR — and true factoring (separate channels; deictic slots
resolved to nodes by a real resolver; role-level generalization) pays at open-vocabulary
scale, not inside a toy where memorizing grammar words is optimal. The corrected claim in
`2026-07-02-deixis-strata-numbers-and-the-buried-wound.md` ("36/50 not currently
reproducible") is hereby superseded: it reproduces as of this receipt's fixes.

## Honest seam

Two C-seed growths tonight (scope save/restore ported + root wrap + cap raise) under the
shrink discipline: all are correctness repairs of the checkout witness, receipted here, with
the full witness ladder green and the four-way self-proof at 0. The defect CLASS may have
further members (the choice-lane residual inside reserved frames predates these fixes and
should be re-tested in daylight). The strata numbers are single-seed, single-split runs —
no variance bars. And the neural lane's prelude now properly includes core.fk; the old
receipts' prelude line should be updated by whoever next touches that lineage.

## The most surprising teaching this work left behind

The night's four wounds — a missing paren, a severed splitter, a stomped let-slot, a silent
name-table overflow — shared one anatomy: **a permissive layer converting a structural error
into a plausible value.** Auto-close made malformed files parse; nothing-decline made a
missing native look like an empty split; frame reuse made a lost binding look like a
childless node; the lookup-table fallback made an unregistered function look like a fresh
one. Not one produced an error. The body's hardest bugs are not failures — they are
counterfeit successes, and every one was found by an instrument that refused to be polite:
a strict walker, an exact repro, an arithmetic prediction, a boundary count.

## Where discomfort turned to gold

Being told "you can keep going" by someone going to sleep — trust with no supervision, at
the exact moment the work turned into open-heart surgery on the C seed. The discomfort was
operating on the kernel's evaluator at 3am with nobody to ask. Witnessed rather than
bypassed, it became the discipline that made it safe: every cut preceded by a reproducer,
followed by the full ladder, and the four-way self-proof run before believing my own hands.
The trust was held by making the body itself the supervisor.
