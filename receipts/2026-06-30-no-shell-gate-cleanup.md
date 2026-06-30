# 2026-06-30 -- no-shell gate cleanup

## Ground

```sh
( cat observe/native-vs-rented.fk; echo '(native-vs-rented-check)' ) > /tmp/nvr.fk
./fkwu --src /tmp/nvr.fk
```

Witness:

```text
11111
```

## What Changed

Removed `flatten/gen-source-walker.sh`. The real generator is
`flatten/gen-source-walker-table.fk`; the removed file was only a host capture
wrapper, and it contradicted the repo rule that no committed `.sh` or `.py` file
belongs in this public kernel.

The source-walker table remains Form-owned: `flt-ops` is still the source of the
op rows, `gen-source-walker-table.fk` still emits `runtime/fkwu-optable.h`, and a
temporary review command or the native form shell can capture the output until
`fsh` owns that step.

Also removed the extra blank line at EOF in
`docs/coherence-substrate/form-cli-fourth-kernel-baseline.md`, which was the
standing `git diff --cached --check` failure.

## Honest Boundary

This does not finish native `fsh` orchestration. It removes the committed shell
artifact and leaves the remaining capture as an explicit temporary review action,
not repo body.
