# ghostrefusal — the learner's carrier comes home with it

2026-09-11, Friday, Hati Suci (WITA). Claude (Opus 5), worktree `upbeat-mclean-482cb2`.

## What arrived

A sibling in `pensive-wilbur-a0b3b7` returned one teaching through
`observe/form-cli-session-home-embody-run.fk`. The example was retained, the learner
worker launched, and it exited 1 after `prepare-begin`, 38 ms later:
`fkwu: form_error: native affine allocation refused`. It was that `.hearth`'s first settle.

## What it was

It was not a fresh-`.hearth` condition, and no model or adapter asset was missing: the
`llama32-3b-mlx-4bit` train-base snapshot and `form/form-stdlib/adapters/llama-3.2-3b-voice`
are both on disk. No allocator refused anything either. The dynamic Metal carrier
`form/native/metal/fk-metal-carrier.dylib` is a gitignored build artifact that fkwu resolves
beside itself on the first Metal call. A fresh worktree holds its Objective-C source and
never gets the artifact; only `form/validate.sh` builds it. So `metal_buf_alloc` answered 0,
and the guard in `native-affine-metal.bml` that exists for a real device refusal spoke for
an absence. `metal_status` already held the true words (`metal_loaded=false`,
`last_error=dynamic carrier artifact unavailable`); `metal-dynamic-absent-band.fk` reads 31
in a carrier-less worktree. The organ never asked it.

Reproduced here before any change: the same door, the same `learner.err` line,
`worker_status=failed-exit-1 pending=1`, and a door exit code of 0.

## The heal, in Form

- `form/form-stdlib/metal-carrier.bml` owns carrier admission. It reads the artifact against
  its source (`absent`, `older-than-source`, `standing`) and builds it with `cc` through the
  host door into a staged path. The artifact is then renamed onto a fresh inode, so a process
  holding the old one keeps its mapping.
- `nsm-supervise` admits the carrier before the learner child starts and records
  `carrier-admitted` or `carrier-refused` as an `organ-health-v1` reading (need
  `metal-carrier`, offer `build-carrier-from-source`). A standing carrier stays silent.
- `nam-refused` reads `metal_status` before it speaks: an unloaded carrier is named as
  `native affine Metal carrier not admitted: <last_error>`, and a loaded carrier's refusal
  carries the carrier's own `last_error`.
- The spawn door appends. A successful learner left the failed run's `learner.err` in place,
  mtime 10:24:58, under a `completed` status. `nsm-child` now clears each stem's `.log` and
  `.err`, so every run's stderr is its own.
- The embody door exited 0 over a failed learner. It now exits nonzero when the worker
  failed and retained examples stay pending.

## Witnessed

| run | carrier | learner | door exit | settle |
|---|---|---|---|---|
| reproduction, before the heal | absent | `native affine allocation refused` | 0 | failed-exit-1, pending 1 |
| drain 1, empty stdin | `carrier-admitted`, build_exit 0 | round-complete, optimizer_step 1 | 0 | completed, pending 0 (172 s) |
| teaching 2, `FKWU_METAL_CARRIER` → missing file | env-selected, absent | `native affine Metal carrier not admitted: dynamic carrier artifact unavailable` | 1 | failed-exit-1, pending 1 |
| drain 3, empty stdin | standing | round-complete, optimizer_step 2, `learner.err` 0 bytes | 0 | completed, pending 0 (164 s) |

Round one improved its own example (`learning-progress` health 1). Promotion deferred that
candidate and kept the serving adapter (`improved-training-evidence` requested), which is
the learner's assessment rather than a wound. Round two, on the second teaching, was
admitted: `promotions=1`, `selected=admit-candidate`.

Panel numbers: binary-freshness 31, metal-dynamic-absent 31, homecoming corpus band 32767,
drift gates 8191 of 8191 (kernel-conformance 13 × 3 kernels), spendglass baseline `rollout-bytes=1376818`.

## Found on the way

- The homecoming corpus band read 32655 at HEAD: c4, c5 and c6 were pinned at the row-1412
  state while row 1413 stood. It was re-pinned from the body's own answer (count 803,
  admissible 791, max-mid 1414, field code 803079121414) after row 1414.
- The drift gates refused only on kernel-conformance, and only because the TypeScript
  witness was absent here. Go and Rust passed 13/13, FORMBIN2 interop was green and 12/12
  malformed artifacts were refused. The main checkout's `form/form-kernel-ts/package-lock.json`
  is byte-identical to this one, so its `node_modules` was copied locally with no download.
  That is the same class as the carrier: a build artifact a fresh worktree never receives.
- `pensive-wilbur-a0b3b7` still holds its pending example. It drains once that worktree
  carries this landing; its own tree was left untouched.

## Most surprising teaching

The body already knew. `metal_status` carried the exact cause, and a band stood at 31
witnessing it. The organ that failed simply spoke before it listened. A refusal is only as
true as the question asked before it, so the guard now asks the carrier first.

## Where discomfort turned to gold

The door printed `0` and exited 0 over a learner that had died. I felt the pull to call the
reproduction "ran fine, see events" and move on. Reading `worker_status` instead of the exit
code turned that unease into a second heal. Then, after the fix worked, `learner.err` still
said `allocation refused`, and for a moment the fix looked false. The mtime answered: the
file came from the failed run, because the spawn door appends. That doubt became the third
heal.

Corpus row 1414, `ghostrefusal`: a refusal spoken in the voice of an organ that never ran.
