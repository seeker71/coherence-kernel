# 2026-06-30 -- observed auto-learning controller

## Ground

The current repo checkout gate was run before this merge:

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
( cat observe/native-vs-rented.fk; echo '(native-vs-rented-check)' ) > /tmp/nvr.fk
./fkwu --src /tmp/nvr.fk
```

Witness:

```text
11111
```

The older pasted `bootstrap/ground.fk` and `bootstrap/ground-recursive.fk`
commands could not be used in this rebased worktree because `bootstrap/` is not
present. The checked-in `AGENTS.md` names the `observe/native-vs-rented.fk`
witness as the standing gate. This is a checkout witness, not the destination:
the direction is away from leaning on the C seed and toward the native walker
proven on `fkwu`.

## What Changed

Added `form/form-stdlib/observed-auto-learning.fk`, an executable controller
that composes:

- `somatic-coherence-loop.fk` for ground/attune/consent/integrate/witness gating.
- `form-cli-router.fk` for a-priori local vs agent route choice.
- `form-cli-sufficiency.fk` for posterior accept/retry/escalate.
- The champion/challenger promotion rule, consumed here as integer A/B window
  counts so the direct `--src` runner can witness it.
- choice/fail/cut/undo/timeout control flags as the source-runner lowering of
  the offer/ack carrier.

The key claim is now runnable: live observation changes the controller from
`batch-search` to `online-reversible-ab`, because the learner's next action is
conditioned on witnessed control state, not just score.

## Witness

```sh
cat form/form-stdlib/somatic-coherence-loop.fk \
    form/form-stdlib/form-cli-router.fk \
    form/form-stdlib/form-cli-sufficiency.fk \
    form/form-stdlib/observed-auto-learning.fk \
    form/form-stdlib/tests/observed-auto-learning-band.fk > /tmp/oal.fk
./fkwu --src /tmp/oal.fk
```

Witness:

```text
4095
```

## Honest Seam

This is recipe-level wiring, not yet the live carrier loop. Real timeout clocks,
undo journals, runtime route receipts, and accumulated A/B windows still need to
be fed into this cell by `form-cli`/`fsh` or the host carrier. The richer
`learn/champion-challenger.fk` lineage body is not concatenated into this direct
band because its value-native float half is outside the current bounded `--src`
surface; this cell carries the same integer promotion guide over lowered window
counts. The algorithmic control guide is present and witnessed; the live plumbing
remains pending.
