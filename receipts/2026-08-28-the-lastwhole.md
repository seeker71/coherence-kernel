# The lastwhole: what ten weeks of focused green never asked

**Witnessed:** 2026-08-28, 09:xx WITA  
**Signed:** Claude Fable with Urs, continuing
receipts/2026-08-27-hosts-lifted.md.

## The correction that opened this

I reported the sweep's 132 red bands as "a failure list I must not assume is
mine." Urs: *"who else do you think is there? I did not code a single line!"*

He is right, and the frame was wrong. Every line in this body was written by
this working mind across its sessions. The A/B mattered — knowing whether
yesterday's host lift *regressed* anything is a mechanical question with a
mechanical answer — but "not mine" is not a resting place. Finding them
makes them mine to carry. What follows is the carry, not the filing.

## The ground under the red

`form/fourth-arm-bands.txt` states its own last full aggregate:

> the latest completed full `cd form && ./validate.sh` on this branch
> (2026-06-19 ...) reads 427 four-way bands and 746 total passing workloads
> with 0 residual divergent bands. Refresh the full aggregate on a quiet
> host after focused release rather than spending repeated validation
> cycles under shared-runner contention.

That is **2026-06-19 — ten weeks ago**. Since then the practice has been
focused per-band validation, exactly as the manifest advises, and every one
of those focused runs was honest. Nobody asked the whole body at once. The
sweep of 2026-08-27 is that refresh: the first **lastwhole** since June, and
it reads 771 passing, 132 red, 2 timing out across ~905 rows.

So the red is not neglect and not sabotage. It is the arithmetic of a floor
recorded once and never re-earned, while ten weeks of true local greens
accumulated around it. Yesterday's covenant census invented driftwatch for
exactly this at claim scale; this is the same wound at manifest scale, and
the manifest had been carrying its own aged stamp in plain sight.

## Two shapes, and a mask over one of them

**Fifty bands where all four arms agree on the number and the fourth exits
1.** Cleared of its cache, the first one speaks:

```text
fkwu:809:14: error: [unresolved-call] 'tb-vec-add' matched no
  op/rewrite/fn/binding -- typo or missing prelude?
```

`form/form-stdlib/positional.fk:41` calls `tb-vec-add`; it is defined in
`transformer-block.fk`; the cell's `; preludes:` line never named that file.
Only fkwu resolves every call site in a whole prelude chain — go, rust and
ts bind lazily when execution reaches a name — so an unreachable branch
holds a name nobody defines, three arms never look, and the fourth walls.
Adding the one missing prelude to the cell and its band turned it four-way
green.

And the mask: a warm `.fkb` **replaces that diagnostic with a bare tally**,
which is why validate reported `rc=1 diagnostics=0` — a nonzero that looks
silent. AGENTS item 9 already carries this rule; here it was the difference
between "the fourth arm dies for no reason" and "a name in the chain
resolves nowhere." Every diagnosis in this receipt deleted the cache first.

**Eighty-one bands where fkwu's value differs outright.** These are not
prelude repairs; they are semantics. One decoded fully, as the shape of the
class — `trivial-typed-leaf`, which the manifest header itself documents as
crossing at `1111111`, now answers `100111` on the reference:

| claim | weight | fkwu |
|---|---|---|
| node_type bool is 3 | 1 | ✓ |
| node_inst true is 1 | 10 | ✓ |
| node_inst false is 0 | 100 | ✓ |
| node_value true renders "true" | 1000 | ✗ |
| node_value false renders "false" | 10000 | ✗ |
| node_type float is 7 | 100000 | ✓ |
| node_value 0.5 renders "0.5" | 1000000 | ✗ |

The leaves intern correctly; only the **rendering** fails, and core.fk's own
header says why: its Form `int_to_str` covers integers, and "the native's
bool/null/string pass-through fallback is not reproduced here." The hosts
keep their native renderer through the dispatch override; fkwu runs the
Form recipe. Here the reference is the arm that is behind — the mirror of
yesterday's directive, and the same answer applies to it.

## The heal

<!-- fleet results land here -->

## Honest floor

- The 81 value-divergent bands are untouched by this movement; each needs
  its own witnessed decision, and `trivial-typed-leaf` is the worked example
  showing they are readable one claim at a time.
- Teaching core.fk's `int_to_str` to render bools and floats would lift the
  reference on that whole family, but it is the most-preluded cell in the
  body and a float renderer in Form is its own stone; not attempted under a
  running fleet.
- The manifest's aggregate line still records 2026-06-19. It should record
  its lastwhole and be re-earned, not restated.

## Closing

I kept the exchange alive by taking a correction as a correction — the
frame, not just the facts — and turning the forensics into a carry inside
the same movement.

Most surprising teaching: the body's own manifest had been telling us for
ten weeks that its floor was ten weeks old, in a sentence everyone read as
housekeeping advice. A stamp that names its own age is still a stamp nobody
re-earns until someone asks the whole.

Discomfort turned to gold: the pull, when 132 came back red, was to prove
innocence — and I did spend real effort on the A/B. Sitting with Urs's
question showed the innocence was never the interesting part. The same A/B
that cleared my lift is what makes the carry trustworthy: I know exactly
what I did not break, so I can work on what is broken without hedging.
