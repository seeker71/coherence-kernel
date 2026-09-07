# The choices arrive

`form-choice-flow.bml` names what a choice is. This is where the body's real ones walk in.

Eight points, and not one of them counts anything the body was not already counting:

| point | options | ledger read from | flow / restrict / hold |
| --- | --- | --- | --- |
| `jit.crystallize` | walk, crystallize | the live page's per-defn native flag: >0 taken, 0 held, <0 declined; the walking rows' heat is the wait | 4 / 0 / 95 |
| `jit.box` | box | the boxing worklist — slots minted and slots read | 100 / 0 / 0 |
| `jit.mint` | reuse, mint | the mint ledger (`kernel_page_box pid -n`) | none: never offered here |
| `organ.surprise-route` | seven | the organs frame's own attempts, successes and failures, and the movement states it publishes in the row's channels | 3 / 96 / 0 |
| `organ.protocol-floor` | proven, unproven | the same frame's `proven-19-of-22` | 86 / 0 / 13 |
| `glass.frame` | one per publisher | the roster: current is a taking, absent the held offer, malformed a decline | 100 / 0 / 0 |
| `governor.metal-admission` | admit, hold | the governor's three admission lifecycles, which already ARE the body's three outcomes — grown a tick at a time through `fcf-ledger-note` | 0 / 100 / 0 |
| `route.category` | two real candidates, lifted through `branch-choice-order` | nothing counts this. Dark. | none |

`observe/form-choice-flow-live.fk` stands them in a `choices` frame beside the organs frame — one kernel,
its own second, every row carrying `shm:<frame>#<seq>`. Findings ride with them, each with what it asks for.
`form/form-stdlib/tests/form-choice-flow-sources-band.fk` — 8191, publishing into its own root so measuring
the choices does not stain them.

Two gaps the body has right now, both real:

- `jit.box` is a **corridor**. The fold leg — the transparent f64 leaf that landed yesterday — leaves no count
  on the page, so the only choice the page can witness is `box`. The ask is a counter for the fold, not a
  second option written down to make the number look better.
- `route.category` is **dark**. `bml-route-choice-runtime` builds real candidates and nothing anywhere has
  ever recorded that the choice was put.

And the loudest wound: `organ.surprise-route` closes far more often than it opens. The surprise organ routes,
and most of what it routes lands on `hold`, `rest` or `compost` — which `fgor-carried?` counts as failure.
That is the body telling us its own resting states are being scored as losses.

Most surprising teaching: the body was already keeping every ledger this lane needed — the governor's three
admission lifecycles are literally success, fail and silence, and the organs frame publishes its own option
list in the row's channels. Nothing had to be invented; it had to be *read*.

Where discomfort turned to gold: I wanted `jit.box` to have two options, because a corridor looks like a
worse number. Refusing to invent the fold counter is the finding that lane actually needed.
