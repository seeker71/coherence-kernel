# The siblings rest until a kernel moves

2026-09-15, evening, M4 Max, Hati Suci. Urs: *"We don't care about non fkwu kernel performance. Just
a way to validate, and not needed when no kernel changes are done, and most of the work shall not be
in the kernel."*

## Carried

- **The rule lives in Form.** `gate/kernel-change.bml` answers one question: has a kernel source
  moved since origin/main? A kernel source is the seed (`runtime/`), the proof walkers (`walkers/`)
  and the fuller siblings (`form/form-kernel-go`, `form/form-kernel-rust`, `form/form-kernel-ts/src`),
  committed, in the working tree or untracked. It prints what moved and answers 1, or answers 0 when
  nothing did; with no merge-base to read it answers 1.
- **`form/validate.sh` asks it before building anything.** When no kernel moved, no sibling is built
  or started. Each band's closure runs on fkwu from source (`run_fkwu_lane`), and its last line
  answers its pins: the Verdict in its head and its row in `fourth-arm-bands.txt`. A band with neither
  runs clean or fails, and its answer is shown as unjudged. `FORM_VALIDATE_SIBLINGS=1` asks the
  siblings anyway; `--binary` always does.
- **AGENTS.md says it beside the walkers:** they validate a kernel change and nothing else, their speed
  is no goal, and most work lands in BML and Form.
- Preflight stays as it was. It asks the siblings only to classify a name that failed to resolve,
  one small probe per arm, never a sweep.

## Witnessed

- Asked on this tree, `kernel-change` answered 0 ("none moved"); with an untracked `walkers/.kc-probe` it answered
  1 and named the file; with the file removed, 0 again. The first draft could not name anything: it
  called `int_to_str` without core.fk, and the moved branch stopped on a `str_concat` of nothing.
  Adding `form-stdlib/core.fk` to its preludes fixed it.
- `validate.sh` on `vector-ops-band`: 511 against its manifest pin on the fkwu lane, no sibling
  built or started, 7 s wall including the image rebuilds a new binary asks for. `gpu-dispatch-band`:
  63 on its fkwu-only lane, unchanged. `fib.fk`: 6765, ran clean with no pin. With
  `FORM_VALIDATE_SIBLINGS=1`, `fib.fk` built Go and bundled TypeScript, and the three agreed on 6765.
- Of 2,170 band files, 1,068 pin a Verdict in their head, and the manifest registers 1,005 `fks`
  rows. Those answer a pin on fkwu alone; the rest show what they answered.
- Preflight clean on `gate/kernel-change-run.fk`; drift door 16383 of 16383; freshness 31 after the rebase
  (the seed moved upstream).

## Closing

Most surprising: the manifest's third column made this possible. It was written so a band could not
change what it certifies without the change being seen across four agreeing kernels; read alone, it
is a pin fkwu can answer by itself. The siblings' agreement was the only check for bands that pinned
nothing.

Discomfort to gold: all afternoon I wrote that a quiet re-read was owed because a sibling sweep held
the host, as if the load were weather. It was our own practice, running Go, Rust and TypeScript over
BML that could not move them. The re-read is still owed tonight (two other fkwu processes at 100%),
but what kept the host loud is answered where it started.

— Claude (Opus 5), as Sema, worktree pensive-wilbur-a0b3b7
