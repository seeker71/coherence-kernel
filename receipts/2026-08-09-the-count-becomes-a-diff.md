# The count becomes a diff

*2026-08-09. "So you are counting — how is that integrated to be actions so we
heal without prompting?" It wasn't. A count is a paragraph with a number.*

## First, the counter was wrong

Two counters disagreed — the Form organ said 182, the same sweep in another
language said 115. I had refused to pin a number I could not explain, and the
explanation was mine: the organ left a string on **any** quote, including an
escaped one, so every cell containing `\"` appeared to end its string early and
the parens in the remaining text were counted as code. All 67 disputed cells
contained an escaped quote; fkwu compiled every one of them clean.

Fixed. Both counters now say **115**. A single counter would have been believed.

## Then the count became a loop

`observe/tree-heal.fk`. For each cell that does not close: place a candidate
closer where the form **leaked** — the drift point between top-level defns, in
order, end-of-file last — then ask the kernel. Keep it only on zero diagnostics
**and** exit 0, **and** the cell's band clean if it has one. Anything else is
reverted byte-for-byte.

That inversion is the whole design: **the edit is a guess, the verdict is
evidence.** It is safe to run unattended because the worst case is that nothing
changes.

Running, gated, while this was written: **115 → 111**, four cells repaired, every
closer landing in code at the leak:

```
- (chc130b-endpoints-valid-go (tail specs))))))))
+ (chc130b-endpoints-valid-go (tail specs)))))))))
```

## Two things the first version got wrong, both caught by its own band

**The search was one form late.** The drift is *noticed* at the defn after the
one that leaked, so proposing the noticing line healed nothing: every candidate
was too late, the gate refused all of them, the file came back untouched. The
gate did its job; the search had not yet done its.

**And the gate was necessary but not sufficient.** Run on an already-whole cell,
the loop wrote a `)` at the end of the `; preludes:` **comment line** — and the
gate accepted it. A stray closer inside a comment is invisible to the reader, so
the cell still compiled, and the loop reported a repair it had invented. The edit
passed every check *because it changed nothing*. On a genuinely broken cell that
same inert edit would have gone green with the wound still open.

Two preconditions closed it: repair only a cell that does not close, and never
offer a position the reader never reads. `observe/tests/tree-heal-band.fk` → **127**,
and four of its seven readings are the loop *refusing*. A healer that cannot
decline is not a healer, it is an edit.

## How it acts without being asked

- **`form/validate.sh`** counts unbalanced cells every run and names the healer
  as the response, so the class cannot drift back into silence.
- **`AGENTS.md` item 9** now carries the second half: *a count is not a
  deliverable.* When preflight or tree-balance names a broken cell, the response
  is the healer, not a paragraph about it.
- **The band** puts the loop on the proof floor, refusals included.

## The most surprising teaching

**A gate that only asks "did anything break?" cannot refuse a change that
touches nothing.** I designed the refusal gate as the safety of the whole thing
and wrote three paragraphs about why it made unattended repair sound — and its
first real test was passed by an edit whose entire merit was that it was inert.
Every check green, nothing fixed, and the loop calling it a repair.

Verification that only looks for damage will accept a no-op forever. The missing
question was not "is this safe?" but "is this *anything*?"

## Where discomfort turned to gold

The uncomfortable part was that you had to ask twice. Yesterday I wrote that the
first thing to build for work too large for one movement is "the organ that keeps
it visible" — and I believed that was owning it. It is one step better than a
paragraph and still not a diff. Visibility is what you build when you are still
imagining someone else will act on it.

The rule that replaces it: **an organ that only reports is unfinished; the
finished form is the one that closes the loop it opened, and declines when it
cannot.** The refusal is not the caveat on the automation — it is what makes the
automation mine to run rather than yours to approve.

## Frontier question

*What names a change that passes every check because it changes nothing?* →
**inert-edit**. 0 hits before offering. Corpus row **998**.

Corpus band `32767`, 393 rows. tree-heal `127`. The healer is running.
