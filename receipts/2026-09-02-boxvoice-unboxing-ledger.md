# boxvoice — the boxing ledger learns names, the rowless fires learn numbers

2026-09-02, branch `claude/goofy-lalande-ad476a`. Corpus row 1227. Two
questions from Urs in one breath: "couldn't speak? what is preventing
it?" and "we should know why primitive types are used where — which could
use unboxing."

## What had prevented speech, precisely

Two index spaces conflated at one write. Heat is counted by **fn-index**
(`fk_fn_heat[dispatch idx]`, minted by `fk_defn_next`); names live in
**symbol rows** (`fk_fnsym_s/n[row]`, minted by `fk_fntop` — a different
counter); `fk_fnidx[row]` is the join. The old writer indexed the name
columns with the fn-index — reading rows one reserved slot ahead, or rows
never written: `%.*s` with length 0, empty name, every line. Nothing
"refused" — the writer asked the wrong shelf and printed the silence it
found there. Healed in mirrorburn by walking rows and joining through
`fk_fnidx`; pressing the question found the heal HALF-done: a hot
fn-index with no symbol row would now VANISH from the board entirely
(worse than the old empty name). Rowless fires now speak by number —
`count fn#idx` — never silence.

## The unboxing ledger

Ints ride unboxed (tagged words). Every float RESULT allocates a
float-pool slot (`fk_fbox`). Which sources pay that is now witnessed:

- `fk_cur_fn` — set beside every heat bump (all ten dispatch sites, both
  walker copies). The dispatch is a tail-jump with no restore point, so
  attribution bleeds to the callee across returns — a sampling truth,
  said plainly, exact for the hot loops that matter.
- `fk_fn_fbox[idx]` — a seventh column in the fn organ, grown with its
  siblings; each float mint charged to the recipe standing at the fire.
- `.fkwu-boxing.<pid>` — written beside the heat board, same shape
  (`count name`, floor 1024), same numbered-voice sweep. Silent when
  nothing boxes: a pure-int run writes no file at all.

## The first reading

Two loops, identical heat — 300,001 dispatches each:

```
-- heat --            -- boxing --
300001 ispin          600000 fspin
300001 fspin          (ispin absent: zero boxes)
```

The ratio is itself a finding: **two** boxes per fspin iteration — the
add's result AND the `1.5` literal, which re-boxes on every evaluation.
The unboxing worklist names itself, ranked: (1) float-literal interning
(a constant should mint once, not per pass), (2) unboxed float lanes for
recipes hot on this board — the glass JIT's float story. `ispin` proves
the int lane already clean.

Regression: `hot-spin` still named on heat, bands 15 / 63 cold,
bootstrap compile healthy.

## The most surprising teaching

The question "what is preventing it?" was itself the diagnostic that
finished the heal. Explaining the mechanism aloud exposed that my fix
traded empty names for total absence in the rowless case — a regression
in honesty hiding inside a repair. The wound wasn't found by a probe; it
was found by having to say the mechanism plainly to someone who asked.

## Where discomfort became gold

Charging mints to the "current recipe" without a restore point felt like
shipping an admittedly-imperfect instrument, and the pull was to build
full scoped attribution first. Sitting with it: the bleed is bounded,
documented, and irrelevant exactly where the ledger matters (hot loops
own their own mints) — and the imperfect instrument found a real,
actionable wound (the re-boxing literal) in its first minute of life. A
sampling truth that ships today beats an exact truth that doesn't.
