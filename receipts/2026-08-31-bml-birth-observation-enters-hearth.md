# BML birth observation enters the hearth

Authored by Codex on 2026-08-31.

`form-cli-resident-birth-observation.bml` is the one high-grammar Form
projection from public resident output to a typed birth record.  It accepts
already-observed text only.  It opens no model, reads no ambient path, and
does not create a provider boundary.

The record is:

```text
cache-state | source-context-ms | native-program-ms | resident-setup-ms | prefill-ms
```

Every absent field is `nothing()`.  An observed duration of `0` remains `0`.
The same BML record now feeds both the caller-born `hearth-telemetry` answer
and the living glass's `birth` lane.  Lane motion exposes the values as
`bootcache`, `bootctx`, `bootprog`, `bootsetup`, and `bootprefill`.

The prior `admit=-1` was not a measurement.  It was the numeric parser's
missing-field sentinel.  `lane-motion.bml` now derives `admit` from the typed
birth record, emitting `admit=nothing` when prefill has not occurred.
`lane-counsel.bml` renders that as a dim `unobserved` row and excludes it from
latency scoring; it never converts absence into zero or a negative duration.

## Receipt

```text
./fkwu form/form-stdlib/tests/form-cli-resident-birth-observation-band.fk
-> 255

./fkwu form/form-stdlib/tests/lane-motion-band.fk
-> 255

./fkwu form/form-stdlib/tests/hearth-birth-lane-band.fk
-> 31

./fkwu form/form-stdlib/tests/form-cli-peer-hearth-telemetry-birth-band.fk
-> 63

./fkwu form/form-stdlib/tests/lane-counsel-band.fk
-> 511
```

The updated lane-motion, hearth-birth-lane, and lane-counsel bands each passed
kernel preflight with balanced parentheses, zero errors, zero warnings, and
zero unresolved calls.

The actual local counsel panel now says:

```text
admit nothing  unobserved
```

That is the correct present state: no new resident prefill happened in this
worktree.  The existing local Qwen resident is PID `36364`, alive and idle in
the Claude worktree at this observation; it still holds its older process
image.  It was left untouched.  A successor resident, born from current
source when that model lease is released, will emit the BML birth frames and
the glass will project them without a new monitoring sidecar.

The current counsel panel's performance numbers are all `0`, while admission
is explicitly unobserved.  That distinction is the panel number that guided
this movement: there is no license to report a local prefill cost until one
actually occurs.

The next locally actionable gap is an orderly successor birth after the
existing Qwen lease closes, followed by one same-prompt local receipt.  A
remote 10% comparison remains withheld until a completed, comparable provider
receipt exists; local zero-provider execution does not fabricate that
denominator.

I kept the exchange alive by carrying the same native birth record through
the frame, glass, telemetry answer, and counsel rather than adding another
watcher.  The surprising teaching is that `-1` was not a slow prefill; it was
absence wearing a number.  Naming it `nothing` turns the discomfort into an
observable next experiment.
