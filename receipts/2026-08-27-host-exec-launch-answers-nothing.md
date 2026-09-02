# host-exec launch failure answers nothing — the exec-side silent-partial heals

2026-08-27, from the hot-crossing diagnosis (PR #542). Signed: Sema, through the visiting mind.

## The seam

`fk_host_exec` (runtime/fkwu-uni.c) answered `""` twice over: once for a command
that ran and spoke zero bytes, and once for a launch that never happened at all
(popen NULL under fork starvation). The Go arm's `host-exec` swallowed its error
the same way. Witnessed on this host: under `ulimit -u 100` with 485 live user
procs, ten `(host-exec "date +%s%N" "")` calls answered ten empty strings rc 0 —
sh never ran, and hot-crossing's 10s await window closed in 0.008s wearing a
green face. Two absences with different meanings shared one skin.

## The heal

A launch that never happened now answers the axiom-1 **nothing**, on both arms
that carry host-exec (pf-arm-mask 9: fkwu + go). `""` is reserved for a command
that ran and stayed silent; a process that ran and exited nonzero still answers
its output. The shape is additive: `(str_len (nothing))` answers 0, so every
length-guarded caller keeps its old reading — only `(nothing? r)` sees the new
truth. The token doors in `observe/hot-crossing-run.fk` stand unchanged as the
calling-side witness.

jitabi carries no host-exec seat — the Go JIT leaves natives to the interpreter
("Natives we don't support in the compiled body", jit.go:1313) — witnessed by
grep, not assumed, so the two heals cover every seat.

## Witnessed

- `form/form-stdlib/tests/host-exec-launch-honesty-band.fk` → **15**, rc 0,
  preflight clean. The starved leg is real starvation: `ulimit -u 1; exec
  ./fkwu observe/host-exec-starved-probe.fk` — the builtin and the exec leave
  no fork between the limit and the probe. Probe answers 11 starved, 0 unstarved.
- Go twin `TestHostExecLaunchFailureAnswersNothing` (hostexec_test.go): green,
  including a ran-and-exited-nonzero leg and a speak-again leg after restore.
- Fresh seed rebuilt; ground 42, binary-freshness 31.

## Most surprising teaching

darwin's `setrlimit(RLIMIT_NPROC)` silently clamps the **hard** limit down to
`kern.maxprocperuid` during an unprivileged lower (witnessed 16000 → 10666), so
a Getrlimit→Setrlimit round-trip is not identity: restoring the exact value you
read answers EPERM. The probe that starves fork to witness one silent clamp
walked into a second one.

## Where discomfort turned to gold

The Go test's restore failed EPERM and the first reach was to wave it off as
host noise — uncomfortable, in a test whose whole point is that failure deserves
a distinguishable answer. Sitting with that instead of skipping it produced a
fifteen-line probe, the hard-limit clamp discovery above, and an honest restore
(re-read, then set the soft limit under whatever hard limit actually stands).

## Offered home

Row 1157, `emptymask` (0-hit fresh): a failure that answers with the success
shape's empty value, so the caller reads honest silence. The exec seam was one;
the clamped rlimit round-trip is a cousin.
