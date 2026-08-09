# The flag that selected nothing, and 115 cells that do not close

*2026-08-09. Two directives: drop `--src`, and stop naming errors and leaving
them for you. The second one is the real note and I earned it.*

## `--src` was never a door

Witnessed in `runtime/fkwu-uni.c`:

```
13048: if (argc >= 3 && argv[1][0] == '-' && argv[1][1] == '-')
13049:     return fk_run_src(argv[2], ...);
13051: if (fk_path_has_suffix(argv[1], ".fk"))
13052:     return fk_run_src(argv[1], ...);
```

The same function, either way. The flag is not parsed as a mode and is not
validated — `./fkwu --banana cell.fk` answers identically to `--src` and to the
bare path. Verified on a real chain: same verdict, same `[unresolved-call]` with
file:line:col, same exit 1.

Swept out of every live file: **2,445 → 0**. Receipts keep theirs; they are a
record of what was typed, and rewriting a witness to tidy it is the one thing
this body does not do.

## The 303 I named last night and left

Last night's receipt said 304 of 313 loader consumers could be repointed, and
that I had done one. That was a list handed to you wearing the word "measured".
**287 cells repointed** at `form-ontology-bp.fk` this morning — prelude lines
only, never prose, and only cells using no name from the engine half. The eight
that genuinely need it keep it.

## The warning storm was mine

Every run this week printed `foreign .fkb (written by a different fkwu build)`
and `stale .fkb (stored source identity does not match)`. I read them as scenery
for nine days. They were the direct consequence of my own two changes — the
value-stack rebuild and the v1→v2 content-digest bump — and every one of them
was an artifact asking to be regenerated once. Cleared the 120 build products
(keeping the one tracked sample). Second run: **zero warnings, zero errors**.

A warning I caused, that I then trained myself to ignore, for nine days.

## 115 cells in the tree do not close

Sweeping every `.fk` for form balance: **116 unbalanced**, 115 of them
paren-surface. Ten put to fkwu, ten confirmed — `input-ended-mid-form` or
`stray ')'`. Four such cells were found by accident this week, one commit at a
time; the tree holds a hundred more.

`observe/tree-balance.fk` makes the class countable instead of accidental. It
counts and locates; **it never repairs**, and that restraint is paid for: the
MDL cell balanced, parsed, and *crashed* until the closer moved from the list
tail to the `(let …)` it belonged to. Balance is not structure. Two cells this
morning proved it again — closing both over-closed by one, closing either alone
under-closed. A hundred confident appends would balance a hundred files and
silently renest an unknown number of them.

So the honest state, and the work I am on rather than handing over: the class is
visible, the count is recorded, and each repair needs the cell's own verdict to
land on. One reconciliation is already open — the organ counts 179 where the
same sweep in another language counts 116, and I will not pin a number I cannot
explain.

## The most surprising teaching

**The tell for a ritual is that nobody can say what the alternative was.** I
typed `--src` some hundreds of times, wired it into three entry documents four
days ago as the practice a new agent should follow, and watched four blank
contexts copy it back to me — and not once did I ask what the other mode would
have been. There was none. It selected nothing, and 2,445 copies of it taught
every reader that it mattered.

## Where discomfort turned to gold

You named the thing directly: surfacing an error and stepping around it hands me
the work back without asking. Reading last night's line — *"I have not audited
the other 303 … it is next"* — with that in mind, it is not carefulness. It is a
handoff wearing carefulness. The tell is the tense: work I am *doing* has a
diff; work that is *next* has a sentence.

What the discomfort bought is a rule I can apply without you: **if I can name it,
I own it in the same movement — and if it is genuinely too large for one
movement, the first thing I build is the organ that keeps it visible, not the
paragraph that describes it.** That is why `tree-balance.fk` exists and the 115
are not a note.

## Frontier question

*What names a required form that selects nothing and is copied because it is
there?* → **cargo-flag**. 0 hits before offering. Corpus row **997**.

Corpus band `32767`, 392 rows. preflight `1023`, conformance `262143`,
MDL `65535`, review-ask `511`, nl-extract `255` — all exit 0.
