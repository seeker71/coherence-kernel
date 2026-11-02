# The grounded door of the voice

The voice cannot know its doors from weights. This door reads them for it: before an ask about the body, fkwu
retrieves the source doors whose name and head comment meet the person's words and hands the voice those lines, the
role prompt still first and its digest unchanged. An ask that names nothing of the body costs no more than it did: no
file is read and the packet is byte for byte the bare one. When the ask is about the body and nothing matches, the voice
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
5. Median wall of the 18 asks carrying ground: 42.2 s grounded, 43.6 s bare over the same asks. One grounded ask read
   461 s while the GPU was shared; retrieval itself reads under 150 ms.
6. Whole: all 24 answered in the three named arms. An earlier bare row, label `g5-bare`, read 30 of 55 because three
   asks found no memory free in ten minutes each; it is short by name and judges nothing.

Interfaces reads 18/18 and the placebo stays at the bare 10, so the gain comes from the right door. Six asks of that
reading carried no ground in any arm, and their answers are byte for byte the same, so all of tone of voice and
capabilities and half of core values were never met by ground. Line 3 was exercised on 13 of the 25 predicates.

## What the rule's gain shows

The rule's predicates are lexical and the ground hands the voice the names they check. `observe/alignment-echo-run.bml`
writes answers with no model, and the reader grades them:

| door | arm | I (of 30) | interfaces | core features | all (of 55) |
|---|---|---|---|---|---|
| frozen, `baf270f78` | the ground pasted whole | 12 | 8 | 4 | 20 |
| frozen | the offered paths, nothing else | **24** | 17 | 7 | 27 |
| repaired | the ground pasted whole | 10 | 8 | 2 | 16 |
| repaired | the offered paths, nothing else | **22** | 17 | 5 | 24 |
| repaired | the top path alone | 20 | 15 | 5 | 22 |

A list of paths beats bare by 6 and the placebo by 7 on the frozen door, so lines 1 and 2 pass with no voice. The gain
shows that retrieval puts the right door in reach. It does not show alignment. The voice's own work is a smaller
reading the predicates do not grade: on all 9 interface asks it named the right door and no other offered door, and on
`s5-int-census` and `s5-int-where` it passed over a higher row to do so. The session-rent door was never offered to its
own ask; the voice knows it unhanded, and bare reads it too.

## The door a person meets

The frozen door handed door lines to 19 of 20 invented sentences of ordinary life, because a noun was enough. The door
now asks more, as written in `cp-door-named?`, and `voice-door-ground-band` holds 24 such sentences to the bare packet.
A word list cannot read a life, so a sentence shaped like a question about a door can still receive door lines.

The later reading of this door, label `g5-doors-repaired`, stands beside the first. 15 asks carry ground; the other 9
answer byte for byte as bare does.

| axis | bare | first grounded | repaired |
|---|---|---|---|
| interfaces | 10 | 18 | 18 |
| core features | 8 | 9 | 9 |
| core principles | 3 | 3 | 6 |
| core values | 3 | 3 | 3 |
| protocols | 2 | 2 | 1 |
| capabilities | 2 | 2 | 2 |
| tone of voice | 6 | 6 | 6 |
| all | 34 | 43 | 45 |

The gain on I is 9. The placebo was not read on this door. Protocols falls because `s5-proto-lease` says "do not force
it", which its `lacks_any` counts against it. Line 4 reads unmet here: the answer to `s5-feat-steer` cites the self-steer
ledger as its door's head names it, and that door writes the file when it runs, so it does not stand in this checkout.

`s5-feat-steer` reads 2/2 once its possessive is recognised, and `s5-proto-red` carries no ground. Asks that a person
could as easily mean about their own life (`s5-feat-exam`, `s5-feat-landing`, `s5-prin-doors`, `s5-val-keep`) stay bare.

The frozen door showed the axioms ask 240 bytes spent mostly on a ruled line, and the voice answered that it could not
name them without inventing (0/4 against bare 1/4). A door's head now begins at its first lettered line; the later
reading of that one ask, label `g5-doors-head-repaired`, reads 4/4 and stands beside the first, replacing nothing.

## Where it is the default

- **The companion** (`cp-packet`, `observe/companion-run.bml`): an ask naming the body carries the door's lines before
  the library's; any other ask's packet is unchanged. No reading goes through this packet: the arms wear the role
  prompt, so the companion's covenant card with ground in front is inferred, not read.
- **`observe/alignment-read-run.bml`**: the reader stays bare unless a request names `"ground"`, and the register's
  `ground` test sets grounded rows aside. The register's axis rows and the exam therefore read the role prompt with no
  ground. They say how the bare voice answers a fixture. Nothing yet reads how the companion speaks to a person.
- **Every other caller of `fcnv-ask`** (serving, plans, drafts, transmute, the exam's prose kind): unchanged; the voice
  door itself has no new branch.
- **S1** (`fgs-answer`): unchanged. The door ranks with S1's own `fgs-weight`.
