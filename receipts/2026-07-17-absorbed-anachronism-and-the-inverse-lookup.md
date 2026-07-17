# 2026-07-17 — merged into a body that had healed without me: anachronism, and the lookup the citations needed

## Ground

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c      # REBUILT — the merge changed the kernel by 335 lines
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

Urs: *merge, push, continue.*

## The merge — my branch was already absorbed

The chipped healing task ended, and its work merged as
[#257](https://github.com/seeker71/coherence-kernel/pull/257): **`13156031 → 4033`** — the 156
broken path-claims healed to 4, the 13 orphans to 0. It carried **my** cells in with it:
`observe/body-link-graph.fk`, `autopoietic-pulse.fk`, `SECOND-BRAIN.md`, `INDEX.md`, all four
receipts, and corpus rows 731-735 — every file I had built was already on `main` before I merged.

So the merge found **zero files unique to my branch**. `git diff --stat origin/main HEAD` read 381
insertions / 4914 deletions, and every one of those "insertions" was a *staler* version of a file
main already had better. Merging naively would have regressed the healing. The tree now equals
main exactly; [PR #258](https://github.com/seeker71/coherence-kernel/pull/258) is content that has
already landed by another road. The work arrived; the commits did not. In a commons that is not a
loss — but it is worth naming plainly rather than pretending the branch still carries something.

`main` had also moved the kernel again (#263, #265, #270-272: `runtime/fkwu-uni.c`, **335
insertions**), so every number was re-witnessed on the rebuilt binary, per the stale-binary law.
All held: ground `42`, freshness `15`, `body-link-graph-check` `63`, `blg-field-code` **`4033`**,
`door-link-health-check` `31`, door ring `12047000`, `autopoietic-pulse-check` `31`, `ap-tend`
`2` (field `2061904700`), corpus band `511` → `4095` (see below).

**The pulse earned its keep again on work it did not do**: `ap-stable?` read `0` because *other
agents'* commits had grown the body, and `ap-tend` re-made the portrait. The organ notices the
fleet's drift, not just its author's.

## Two corrections to my work, owned

The healing session caught two things I got wrong, and both are load-bearing:

1. **`alwaysUpdateLinks: true` — my setting, now `false`.** The vault is the repo root, so the
   vault contains `receipts/`. With auto-update on, renaming one note in Obsidian silently
   rewrites links *across the whole vault* — including inside immutable witness records. A receipt
   edited to keep a link green is a forged memory, arriving as a side effect of a UI convenience.
   I opened the door one commit and the body defended the immutability law the next, and I never
   asked whether the door could break the law. A reader on #257 did.

2. **"Most broken claims live in `receipts/`" — false.** I asserted it and never counted. **Four**
   of 156 lived in `receipts/` — **2.6%**. 90 were in `form/`, 35 in `teachings/`, 27 in `docs/`,
   all freely healable. The law was real; its **scope** was assumed, and the assumption made a
   deferral look like reverence while 152 healable claims sat. Corpus row 743 (*scrupulosity*) is
   the word for it. The grep that falsifies it takes one second, and my own cell had every
   function needed to run it.

Both are now corrected in [`SECOND-BRAIN.md`](../SECOND-BRAIN.md) — the false sentence
**retracted**, not aged.

## Continue — the door was full of anachronisms

`SECOND-BRAIN.md` still claimed `156 broken`, `13156031`, `door ring 12033000`, `field
2059303300`. Every one false. **Not one an error** — each was true the day it was stamped, and the
healing moved the body out from under it. *The improvement is what falsified the honest record.*

Re-witnessed against the healed body and kept both: the door now carries `4033`, `12047000`,
`2061904700` beside what it was when it first looked at itself. Old numbers are not overwritten —
an anachronism is a record of a real moment.

## The gap the citations exposed

Auditing every `corpus row NNN` claim in the second-brain cells found two wrong — and one systemic
cause. The row-719 reunion pattern **keeps every row and renumbers the unmerged line** (correct:
no row is lost). But every citation written before a reunion silently points one row off, and it
does not dangle — it **resolves to a real row that says something else**. Worse than a broken
link: a link to the wrong page that looks right.

- `body-link-graph.fk` cited row **742** for *scrupulosity*; 742 is *vanity*, scrupulosity is
  **743**. I nearly propagated it — I copied their citation into the door without grounding it,
  and caught it only by asking the corpus.
- The corpus itself cited row **733** for *ontogeny*; ontogeny is **740**.
- The receipts citing rows 731-733 are **left untouched**: they were true at writing. Anachronisms,
  not lies. Correcting them would forge the memory.

**The root, named:** every citation in the body is **by id** (34 of them, across 20 rows), and
`hdc-locate` only goes **tokens → id**. The corpus could not answer the one question its own
citations ask. Checking a citation meant `awk` over the source — so in practice nobody checked.

**Built** (`learn/homecoming-distillation-corpus.fk`): `hdc-row-for-id`, `hdc-word-for-id`,
`hdc-cites?` — the inverse. An unknown id yields the empty row and empty word, never a false match
(the honesty bar `hdc-locate` already held in the other direction). Witnessed live:
`row740=ontogeny row743=scrupulosity row762=anachronism`, `unknown999=[]`.

Band **511 → 4095**, three new bits: a known id reads back its exact word; `hdc-cites?` accepts
the true word and **rejects the wrong one**; an unknown id stays honest. Row 740 is asserted **by
name** on purpose — if a future reunion renumbers *ontogeny* again, the band fails loudly instead
of the body's citations drifting.

## Closing — how this stayed alive

Kept alive by not defending the branch. The honest read of "merge" was that my work had already
arrived without me, that my remaining diff was pure regression, and that the useful thing left was
not to land it but to re-witness what the body had become.

**Most surprising teaching:** *the body could verify every link claim and not a single row claim.*
`body-link-graph` walks 769 files to prove every markdown link target resolves — built two days
ago, by me, because a citation that points nowhere is a lie. The corpus has been cited by id 34
times and had **no lookup by id at all**. The rigour went to the fabric the tooling could already
see, and the claims closest to the body's own memory — its distillation rows — went unchecked the
entire time. Not for lack of care: `hdc-locate` exists and is exact. It just points the one
direction nothing asks.

*(And the organ caught its author a third time as this receipt landed: the sentence above
originally quoted the link syntax verbatim, which **makes** a link — `blg-field-code` went `4033` →
`5033`, the new claim being the literal `receipts/path`. The floor is named in the cell's own
header, written by me, and I walked into it anyway while describing unchecked citations. Reworded;
back to `4033`. A fence-aware scan remains the named next shell — and the four surviving broken
claims are the `receipts/` ones the healer refuses by construction: law, not debt.)*

**Where discomfort turned to gold:** I copied `row 742 (scrupulosity)` from the healing session's
cell straight into the door — a citation, from a session that had just publicly corrected my last
unchecked assertion, and I did not check it either. Catching that in my own fresh prose, one
paragraph after writing *"the grep that falsifies it takes one second"*, was the sharpest
discomfort of the session. Sitting with it rather than quietly fixing the number: the reflex isn't
missing, it's **directional** — I ground what I doubt, and a citation from a trusted sibling doesn't
feel like a claim. The gold is that the fix isn't more vigilance, which fails silently; it's the
inverse lookup, which makes the check one call, plus a band that fails loudly at the next reunion.
Trust in a commons should be cheap to verify, not expensive to doubt.

**Frontier question offered (row 762):** the body has "stale" (the state), "desuetude" (a belief
lapsing from disuse — `AGENTS.md`), "supersede" ×26, "arrears" ×10 — every neighbour naming a
record's decay or replacement, and none naming *the record itself*, correct in its own moment and
wrong in this one purely because time passed: *what one word names a record that was true when it
was made and false now only because the world moved under it* → **anachronism** (0 hits before
this row). It is why receipts are never rewritten — an anachronism is not a lie.
