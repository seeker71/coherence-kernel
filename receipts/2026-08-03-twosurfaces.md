# 2026-08-03 — I deleted a real row, then a guard was built to keep it deleted

## What I got wrong

In `ced303fa4` I believed I was removing a duplicated `challenger.deepseek-ds4-metal` row. The diff
says I removed **two** rows: the ds4 row (replaced by a corrected one — fine) and
**`base.llama32-3b-local`**, which was not a duplicate of anything.

The mistake was reading `grep -n` output as if it were a second line of a `sed -n` range. Line 582
appeared twice on my screen — once from grep, once from sed — and I concluded 582 and 583 were
identical. 583 was the llama comparator row.

## Why it mattered downstream

A sub-agent, correctly refusing my handed diagnosis, probed and found `nmcp-unique?` was *not* blind
to a repeated id. What the registry had held was two rows both naming `llama3.2:3b`:

```
base.llama32-3b-metal   surface native-recipe   runtime fkwu+Metal   role champion
base.llama32-3b-local   surface local-process   runtime ollama       role shadow
```

That is **one model on two surfaces**, and the comparator row's own next-action says *"retain only as
an explicit comparator for the direct Metal lane"*. It is the point of the registry, not a defect.

The agent then widened `nmcp-unique?` to reject any two rows sharing a **model name**, and added a
band bit asserting the deleted row stays absent. That is my error hardened into a rule: the registry
could no longer express the comparator pattern it exists to hold.

## The fix

Uniqueness now keys on the pair **(model-name, runtime)** — `nmcp-same-lane?` / `nmcp-lane-in?`. Same
model reached the same way twice is a duplicate; same model reached differently earns its row.
`base.llama32-3b-local` restored, registry back to 35.

```
35 rows, comparator restored                          -> 65535
same-model + same-runtime clone, DIFFERENT id         -> 65529   (bits 2 and 4)
restored                                              -> 65535
```

The clone carries a different id, so this is the quiet duplicate — exactly the case an id-only walk
misses — and it is now caught.

## Ground stamp

```
2026-08-03, M4 Max. git show ced303fa4 -- native-model-control-plane.fk => two rows deleted.
Restored row first landed OUTSIDE the registry list (my sed matched my own comment line, not the
row); (len (nmcp-registry)) said 34 while grep said 35 — the cell was asked and disagreed with grep.
Moved inside: len 35. Band tests/native-model-control-plane-band.fk 65535; count bit re-pinned 34->35.
.fkb/.sym removed before every run (freshness is st_mtime whole seconds).
```

## The most surprising teaching

**`grep -c` said 35 and the cell said 34, and the cell was right.** I had "restored" a row into the
middle of the function definitions where it parsed fine and meant nothing. Counting occurrences of a
pattern in a file is not the same question as asking the structure how many members it has, and the
first one is the one that is always available and always tempting. The probe that settled it was four
lines: prelude the cell, print `(len (nmcp-registry))`.

## Where discomfort turned to gold

Reading a sub-agent report that corrected my diagnosis, feeling the relief of "good, it's fixed" — and
then finding the fix had encoded my own mistake as a guard. The agent did exactly what I asked and was
right about the mechanism; the error was upstream, in a claim I handed it as context. **A wrong premise
delivered confidently to a careful worker comes back as a well-built wrong thing.** I wrote "nmcp-unique?
is blind to duplicate rows" in a receipt and a commit message before probing it, and it became a
specification. The gold is narrow and usable: when handing a diagnosis to someone else, mark which
parts were witnessed and which were inferred — the agent probed anyway and caught it, which is the only
reason this is a receipt and not a regression.

## Unfinished

- Two corpus rows proposed by sub-agents and not landed (ids collide across concurrent sessions):
  `blindstamp` (a stamp identical across programs 103 functions apart) and `mintedkey` (a key that
  cannot detect a duplicate because the registry mints it). Both verified 0-hit.
- `observe/preflight-run.fk` prints a bare integer instead of its report: a string *literal* in that
  position prints text, a string *built at runtime* prints an integer, so `pf-report` and
  `vf-mirror-file` are both mute. Named by a sub-agent, unowned, unfixed.
- `bootstrap/form-cli-darwin-arm64.stamp` has disagreed with `form-cli.stamp` since before this work,
  so `FORM_STANDARD_LANE=1` needs a refreshed committed platform binary.
