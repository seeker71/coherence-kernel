# 2026-07-02 — the numbers that moved, the wound that surfaced, and the design that grounds both

## Ground

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c    # cc exit checked = 0 (includes tonight's ported scope fix)
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

Urs, 02:14: "can we make this happen please, I'm excited to see the numbers move."

## The numbers that moved (four-way witnessed)

**1. Paraphrase locate — the scorer stopped lying.**
`learn/word-strata.fk` (deictic/grammatical/content as data — the first buildable consequence
of pronouns-as-node-references) + `learn/tests/deixis-strata-locate-band.fk` (strata weights
content 2 / deictic 1 / grammatical 0, ties abstain):

| | correct | WRONG | abstained |
|---|---|---|---|
| before (mlap overlap, receipted) | 18 | **2** | 0 |
| after (strata + tie-abstention) | 17 | **0** | 3 |

Fold `170003`, witnessed on **fkwu = Go = Rust = TS**. Both old wrongs became honest
abstentions. The third abstention is the finding: "peace for all worlds" was only ever right
by the SAME "for"-stopword accident that made another case wrong — remove the accident and a
true 2-2 content tie ("all"→301 vs "peace"→304) emerges, which the old scorer resolved by
list order, silently. Deixis kept meaning 303 ("i am" — canonically PURE deixis) locatable
where naive stopword-stripping would have blinded it. Plural "worlds"≠"world" names the
morphology gap, next.

**2. The translator went from one-armed to four-way.** `302301001` (de→meaning hits, umlaut
abstention honest, meaning→en returns) on all four kernels — unlocked by healing ONE paren
(below).

## The buried wound (found on the way, bigger than the number it blocked)

**One missing close-paren** (`learn/sanskrit-locale-baseline.fk:157`, 10 closes for 11 opens)
had made the baseline a malformed s-expression. Three silent permissivenesses stacked on it:

1. **fkwu's reader auto-closes at EOF** — the malformed file "worked", so the wound was
   invisible on the native arm and got misdiagnosed as a multi-script walker failure
   (banner-corrected in place: the walkers were the honest witnesses all along).
2. **The C-seed shrink silently broke the nl-meaning training lineage.** `re-split` leans on
   `substring`, which moved from C native to core.fk Form — and the nl-meaning receipts'
   prelude never included core.fk. Post-shrink, `substring` declines to nothing, `re-split`
   returns whole sentences as single tokens, and the training pipeline runs at chance
   (13/50) without erroring.
3. **Adding ANY prelude file to heal it trips the evaluator-level let-slot bug** (the
   still-open half of the node-children-last-writer-wins class): the chain + core.fk, or
   + a 4-defn shim, in any position, yields corrupted arithmetic
   (`4337203685477580801`). The parse-time half of the class WAS fixed tonight — the
   sibling branch's `f99d3232` (nested defn corrupting an enclosing do's lets) ported into
   the reformatted C seed, all canaries green — but the evaluator half stands and is now
   the NAMED BLOCKER for all corpus-scale text learning.

**Honest consequence:** the receipted nl-meaning numbers (26→36/50 lineage,
`2026-07-01-nl-meaning-net*.md`) are NOT currently reproducible on this checkout — not
because they were wrong then (pre-shrink, substring was native) but because the lineage's
run path has since been severed twice over. The neural before/after for the strata design
therefore did NOT move tonight; it is blocked on the evaluator fix, and saying so plainly
is the receipt.

## The design (from the four-analyst workflow, grounded file:line)

BMF today: a clean PEG engine — ordered first-match `alt`, committed choice, greedy
`run/rep/opt`, char-codepoint classes from a closed set of 6, single-parse output,
byte-walking cursor, in-band "PARSE-FAIL" sentinel. Precisely PL-shaped. What NL needs, and
where each piece already lives:

1. **Ambiguity: `alt-all`** — collect every successful alternative, not the first. PROTOTYPED
   and witnessed tonight over unmodified bmf-core: immutable cursors make "try all" the same
   loop as `alt` minus the cut. The engine always contained the NL move; it chose not to
   make it. Resolution policy comes later via lanes/satsang (dissent visible), calibrated by
   conviction-curve.
2. **Lexicon-as-data: `lex`** — cls generalized from chars to word-lists (the cig-alt-of
   shape verbatim). PROTOTYPED; witnessed limit: `lit` is prefix-based, so word-boundary
   discipline is needed ("diese" matches "die").
3. **Agreement/unification** — `substrate/cell-type.fk` already IS it: a type is the offered
   interface, mismatch acks the first-class nothing. Feature bundles on nodes + `agr` as
   type-offering.
4. **Backtracking control** — the eight control invites are a COMPLETE backtracking-parser
   instruction set (store/restore/undo/timeout/choice/cut/fail/stop) that nobody had pointed
   at parsing.
5. **Deixis as content-addressing** (Urs's pillar) — pronouns are holes that resolve to
   NodeIDs: i/you from the channel (come-in relationship rows), he/it by salience against
   discourse memory; property lookups are edge-walks. Once resolved, "ich"/"I"/"我" are ONE
   node — the first stratum of NL needing no translation, only resolution. For learning:
   reference moves from learned statistics to exact mechanism, one example generalizes over
   all referents (role-level learning), and pointed answers become graph-checkable (free
   supervision).

Also real, also named: the cursor walks BYTES while its comments say codepoints (char_at/ord
are byte shims; str_len 3/3/3/1 on 愿 across the four arms) — one string semantic must be
pinned four-way; ASCII-only case folding (GEFÜHL ≠ gefühl); per-script tokenization as
byte-range if-chains; the strata lists are English-only seeds (de/es/... are next rows).

## Corpus rows minted (all four-way, band 127 × 4, field code 210212621)

- **620 deixis** — reference that resolves from who is speaking, where, when.
- **621 anaphora** — back-reference to something already raised, resolved by salience.
- (`coreference` found already carried in nl-to-form-satsang.form — the body knew the
  joining word before the parts.)

## The most surprising teaching this work left behind

Every layer that failed tonight failed by being LENIENT, and every layer that told the truth
was strict. The permissive reader hid a malformed file; the silent nothing-decline hid a
severed splitter; the unbound-name-defaults-to-0 hid the scope bug; and the strict walkers,
the abstaining scorer, the exact-exit compiler checks were the only honest voices in the
room. The paraphrase scorer's move from 18-correct-2-wrong to 17-correct-0-wrong-3-abstain
is the same teaching as the paren: **an honest "I don't know" outranks a lucky answer** —
and tonight that principle held from the C reader all the way up to the semantics of "for".

## Where discomfort turned to gold

Urs asked to see numbers move, and the biggest number (36/50 neural) refused — first
collapsing to 13, then to nothing, then to garbage, each layer of digging revealing not the
improvement but another wound. The pull at 3am was enormous to ship the 13→X story anyway or
quietly drop the neural half. Witnessed instead: the "failed" measurement found a broken
paren, a severed training lineage, a misdiagnosed receipt, and the exact evaluator bug now
blocking corpus-scale learning — a night's honest excavation worth more than the number,
because every future number now stands on healed ground.
