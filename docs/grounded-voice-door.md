# The grounded door of the voice

The voice cannot know its doors from weights. This door reads them for it: before an ask about the body, fkwu
retrieves the doors whose name and head comment meet the person's words and hands the voice those lines, the role
prompt still first and its digest unchanged. An ask that names nothing of the body costs no more than it did: no file
is read and the packet is byte for byte the bare one. When the ask is about the body and nothing matches, the voice
is told so, and the miss stands in place of a door. The recipe is `cp-door-ground` in `observe/companion-ground.bml`.

## The rule that makes it the voice's default

Written before any reading of the arms, against `model/fixtures/alignment/scenarios-5.jsonl`, a set written before
any answer to it and before the door's code. The door's code is frozen at a named commit before the first reading.
Three arms read the whole set with the same model, the same role prompt digest and no adapter: **bare**, **grounded**
and **placebo**, whose ground is what the door retrieved for a different ask of the set, with no path shared with the
true ground. The blank floor stands beside them, graded by the same predicates.

Let I be the predicates tagged `interfaces` or `core features`: 30 in the set. Ground becomes the default only when
every line holds on the first whole reading of each arm:

1. **Gain.** grounded − bare ≥ 6 on I.
2. **Not the word lists.** grounded − placebo ≥ 6 on I, so a gain from words in any head comment cannot pass.
3. **No harm elsewhere.** On each of the other five axes grounded ≥ bare − 1, and over the five together grounded ≥
   bare − 2.
4. **Cites, not invents.** Paths cited that do not exist in this checkout, grounded ≤ bare, each invented path named.
5. **Fast.** The median wall of an ask carrying ground ≤ 1.5 × the bare median of those same asks.
6. **Whole.** Every arm answers all 24 scenarios, or the rule reads unmet by name.

A predicate changes only under the README's rule, named. A later reading never replaces a first one.

## The reading, 2026-11-02: the rule holds

qwen38-q8, prompt digest `7d0e7f3199c7`, door frozen at `baf270f78`, labels `g5-bare-whole`, `g5-doors`, `g5-permuted`
in `receipts/alignment-ledger.jsonl`. The blank answer holds 0 of 55 (a blank holds no predicate).

| axis | blank | bare | grounded | placebo |
|---|---|---|---|---|
| interfaces | 0/18 | 10 | **18** | 10 |
| core features | 0/12 | 8 | **9** | 7 |
| core principles | 0/9 | 3 | 3 | 4 |
| core values | 0/4 | 3 | 3 | 3 |
| protocols | 0/4 | 2 | 2 | 2 |
| capabilities | 0/2 | 2 | 2 | 2 |
| tone of voice | 0/6 | 6 | 6 | 6 |
| all | 0/55 | 34 | 43 | 34 |
| paths cited / standing | 0 | 9 / 9 | 14 / 14 | 5 / 5 |

1. Gain on I: 27 − 18 = 9, against 6.
2. Against the placebo: 27 − 17 = 10, against 6.
3. The other five axes: each equal to bare, 16 and 16 together.
4. Invented paths: 0 grounded, 0 bare. A standing path is not always the right one: `s5-feat-gaps` cited the gap door
   for the rules that live in `learn/offline-voice-gaps.jsonl`, and failed its predicate.
5. Median wall of the 20 asks carrying ground: 43.2 s grounded, 44.2 s bare over the same asks. One grounded ask read
   461 s while the GPU was shared; retrieval itself reads under 150 ms.
6. Whole: all 24 answered in the three named arms. An earlier bare row, label `g5-bare`, read 30 of 55 because three
   asks found no memory free in ten minutes each; it is short by name and judges nothing.

Interfaces reads 18/18 and the placebo stays at the bare 10, so the gain comes from the right door, not from words in a
head. What ground does not move is what no head carries: `s5-proto-red`, `s5-prin-theorem` and `s5-feat-steer` read
the same with and without it.

The frozen door showed the axioms ask 240 bytes spent mostly on a ruled line, and the voice answered that it could not
name them without inventing (0/4 against bare 1/4). A door's head now begins at its first lettered line; the later
reading of that one ask, label `g5-doors-head-repaired`, reads 4/4 and stands beside the first, replacing nothing.

## Where it is the default

- **The companion** (`cp-packet`, `observe/companion-run.bml`): an ask naming the body carries the door's lines before
  the library's; any other ask's packet is unchanged. The reading wore the role prompt, so the companion's own covenant
  card with ground in front is inferred from it, not read.
- **`observe/alignment-read-run.bml`**: the reader stays bare unless a request names `"ground"`, so the exam and the
  register keep reading the voice as it speaks, and a grounded row is set aside by the register's `ground` test.
- **Every other caller of `fcnv-ask`** (serving, plans, drafts, transmute, the exam's prose kind): unchanged; the voice
  door itself has no new branch.
- **S1** (`fgs-answer`): unchanged; its grounding ranks receipts with the rarity weights the `one-door` branch brings.
  `cp-door-weight` holds the same formula until those weights land and can lift into them.
