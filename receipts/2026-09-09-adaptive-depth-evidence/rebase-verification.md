Observed in the Codex command stream on 2026-09-09, after rebasing onto
origin/main at 8b03d055. These lines are transcribed command results; the JSONL
and gate output beside this file are retained directly from their emitters.

`form-run git rebase origin/main`: exit 0, successfully rebased.

`form-run ./fkwu form/form-stdlib/tests/binary-freshness-band.fk </dev/null`:
initially 15. The repository's documented cc bootstrap rebuilt the local
binary, exit 0. The band then returned 31, exit 0.

`form-run ./fkwu form/form-stdlib/tests/formbin-depth-band.fk`: 2047, exit 0.

`form-run ./fkwu observe/formbin-depth-witness-run.fk`: depth4096, 47 ms,
stdout42, scalar-roundtrip1, final1, exit0. Its direct JSON files are
`result.json`, `scalars.json`, and `attention.jsonl` beside this file.
The older `witness.out` retains the pre-rebase 662 ms observation.

From `form/form-kernel-go`:

```text
form-run go test -run '^TestSubstrateFormCompilerRouteRunsBML$' -count=1 .
ok  	form-kernel-go	32.697s
@form go 0 28 0 28
```

`form-run ./fkwu gate/drift-gates-run.bml`: 8191/8191, refused0, exit0.
Direct stdout/stderr are retained as `rebased-drift-gates.out` and
`rebased-drift-gates.err`.
