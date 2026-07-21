# The 94 cat-pipeline jit bands regain live preludes; the verdict alone was never the witness

2026-07-18, heuristic-morse worktree, branch `claude/dreamy-poincare-de4c1a`.
fkwu built from `runtime/fkwu-uni.c` (`cc -O2`), all runs from repo root.

## What was asked, what was found

The assignment named 25 jit band files with no live `; preludes:` header, only a
manual `( cat ... ) > /tmp/x.fk` pipeline in comments, failing axiom-5 numb
(`[unresolved-call]`, exit 1) under `./fkwu --src <band>`. Grounding here found
the true count: **94** bands in `model/tests/` and `observe/tests/` in exactly
that state — the 25 was the witnessed subset (the 24 whose cat lines spell the
moved `model/form-asm-x64.fk`, plus one). All 94 failed identically when probed
fresh. The standing directive (cat-pipeline-only bands were left out of the
PR #290 header rejoin) covers the whole set, so the whole set was healed.

## The heal, in two layers

1. **94 band headers.** Each band got a one-line `; preludes:` derived from its
   documented cat line — every file before the band itself, in order;
   `model/form-asm-x64.fk` respelled to its live home `form-stdlib/form-asm-x64.fk`
   (the resolver finds it under `form/`); all other deps repo-root-relative.
   The cat lines stay as prose, untouched.

2. **28 dep headers.** First verification pass: all 94 bands answered their
   documented verdicts (fresh runs, `.fkb`/`.sym` deleted first) — but 22 bands
   still shouted hundreds of `[unresolved-call]` diagnostics while exiting 0
   with the right number. Each compilation unit resolves only against its *own*
   prelude closure, not against siblings the band loaded first; deps whose only
   header was a dead capital `; Prelude:` block went numb mid-run. Their capital
   blocks document their true dep lists, so each of the 28 got a live lowercase
   line derived from its own block (moved paths respelled; prose left as prose).

## Verification

- A/B witness: every band's documented cat pipeline was rebuilt (in scratchpad,
  not the shared `/tmp` names) and run against the preludes-run. **94/94 verdicts
  identical** — every band answers its documented number (jit-carrier-abi 32767,
  jit-native-admission 262143, jit-carrier-current-cache-gate 8191, …).
- After dep healing, second fresh sweep: **94/94 rc=0, zero `[unresolved-call]`
  diagnostics, verdicts unchanged** — the per-unit closure now resolves exactly
  what the one-text cat resolves.
- Corpus row 807 landed (fresh word `parity`, 0-hit before landing); the corpus
  band asked of the body ([203, 807, 1, 2032032807]) and re-run fresh: 4095.

## Most surprising teaching

**Verdict equality is not resolution equality.** After the first heal, 22 bands
produced their exact documented all-ones masks while hundreds of their calls
were numb — axiom-5 recovery to nothing let `nothing == nothing` comparisons
stay green. The same number, different truth-content: the T_flat aphonia
pattern wearing a passing test's face. The only honest witness was diagnostic
silence *plus* the verdict, proven by diffing unresolved-call sets between the
preludes world and the cat world (cat: zero, preludes: hundreds). A band's
number can be right for the wrong reason; parity is the property to demand.

## Where discomfort turned to gold

The moment the first sweep came back `rc=127` on all 94, the easy read was
"bands are broken worse than reported." Sitting with the wrongness instead of
narrating it: 127 is the shell's *command not found* — macOS has no `timeout`.
The discomfort of a perfectly uniform failure was the tell (real failures vary);
witnessing it produced the perl-alarm wrapper and, more importantly, the habit
of distrusting any result too uniform to be news. The second discomfort — 22
bands green-but-shouting — was the one that mattered: not bypassing it bought
the dep-layer heal and row 807.

## Honest seams left open

- The other `; Prelude:` capital blocks that ARE complete stayed prose, per the
  directive; only files that demonstrably went numb got live lines.
- The moved-five respell of the *cat prose lines* (dead `model/form-asm-x64.fk`
  spellings in comments) lives on the vigorous-bouman branch; this work did not
  duplicate it — prose stays as-is here, reunion will reconcile.
