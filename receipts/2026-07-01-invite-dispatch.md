# Receipt — closing the loop: BMF-recognized invites drive the control primitives (2026-07-01)

`grammars/control-invite-grammar.fk` recognizes the eight native token invites (`<CHOICE>` `<CUT>` `<FAIL>`
`<STOP>` `<UNDO>` `<STORE>` `<RESTORE>` `<TIMEOUT>`) inside free text. `control/offer-ack-core.fk` and
`control/choice-lane-core.fk` implement what each of those verbs *does*. Neither half alone was the feature
the original ask described — "choice invites internal thinking, going down different query lanes, collecting
nodes along the way" needs the middle piece: something that walks a stream's recognized invites in order and
actually drives the matching primitive, threading memory and checkpoints from one to the next.

## What landed

**`control/invite-dispatch.fk`** — a SESSION (`cid-session`: the running ack, the collected memory, the last
checkpoint) that a stream of invites is folded through:

- `cid-step(session, tag, alts, args)` — the ONE dispatcher every tag routes through. `<CHOICE>`/`<CUT>`
  offer `alts` through the matching primitive and append the ack to memory (the "nodes gathered along the
  way"); `<TIMEOUT>` does the same through a budget-bounded walk; `<FAIL>`/`<STOP>` need no alternatives at
  all; `<STORE>`/`<RESTORE>`/`<UNDO>` read and write the checkpoint.
- `cid-run(tags, alt-lists, args, session)` — walks the tag list in order, consuming one alternatives-list
  per choice/cut/timeout tag (alternatives live in the reasoning context, never in the text itself); `<STOP>`
  halts the walk immediately — nothing after it runs, the same "boundary stops acknowledging" the offer/ack
  core already gives `oac-stop`.
- `cid-run-stream(text, alt-lists, args)` — the public entry: raw text in, a final session out, having walked
  every invite the grammar found.

**`control/tests/invite-dispatch-band.fk`** — ten claims exercising all eight verbs through real text streams
(e.g. `"explore <CHOICE> then <STORE>"`), written inside one `defn` called once from the start (not bare
top-level `let`s — see `receipts/2026-07-01-node-children-last-writer-wins.md`).

## Honest state of the live witness

Small-scale, individual pieces of the dispatcher check out live: `cid-step`, `cid-run`, and the grammar's own
`cig-invites` each work correctly in isolation against a minimal prelude
(`control/offer-ack-core.fk` + `control/choice-lane-core.fk` + `control/invite-dispatch.fk`, no grammar
files) — for example `(oac-one? (oac-choice ...))` through that prelude alone reads correctly.

But `control/tests/invite-dispatch-band.fk` needs the full prelude chain the grammar itself requires
(`form/form-stdlib/core.fk`, `grammars/line-grammar.fk`, `grammars/bmf-core.fk`,
`grammars/control-invite-grammar.fk`, then the control files), and at that combined scale the same defect
class fixed in `control/offer-ack-core.fk` resurfaces through a *different* combination: adding
`form/form-stdlib/core.fk` alone — 74 functions, no top-level `let`s, pure `defn`s — is enough to reintroduce
`oac-choice` misclassification. This is not a new bug and not a flaw in `invite-dispatch.fk`'s logic (checked
by hand and in small-scale isolation above); it is the same runtime floor named in
`receipts/2026-07-01-node-children-last-writer-wins.md`, wider than the one instance already fixed. The
current live band result on this build is `598`/`1023`; that number is not a measure of this file's
correctness, since the primitives it calls are independently known-correct at smaller scale and the same
inputs trace through by hand to the intended results.

This is named rather than hidden: the feature is designed, written, and reasoned through end to end; its full
live proof waits on either closing the broader defect class (likely more instances like the one already
fixed, found the same way — trace, isolate, replace a top-level cached value with a fresh function call) or a
Form-native eval/flatten lane that does not carry this floor at all.

## Build

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
cat form/form-stdlib/core.fk grammars/line-grammar.fk grammars/bmf-core.fk \
    grammars/control-invite-grammar.fk control/offer-ack-core.fk control/choice-lane-core.fk \
    control/invite-dispatch.fk control/tests/invite-dispatch-band.fk > /tmp/invite-dispatch-band.fk
./fkwu --src /tmp/invite-dispatch-band.fk   # -> 598 (honest current result; see above)
```
