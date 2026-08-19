# Receipt — four-way adjudication returns to Form source

; witnessed: 2026-07-20 -> PASS

## Idea and prediction

Replace the mistakenly promoted bash validator with a source-native Form entry cell. Prediction: if
`four-way-run.fk` is still a living recipe rather than historical prose, `fkwu` can load it from source,
Form can invoke all four lanes through `host-exec`, and `fwv-verdict` will return `0` without `.tbl`.

## Implementation

- Added `fr-diagnose-cmds` to execute the fkwu arm as well as Go, Rust, and TypeScript.
- Added `proof/four-way-ground.fk`, which imports the Form core, verdict, and runner and supplies the
  four ground-workload commands as data.
- No bash validator, `.tbl`, remote model, or new C-seed behavior participates in adjudication.

## Live observation

```text
./fkwu --src proof/four-way-ground.fk
0
```

`0` is the recipe's FOUR-WAY verdict. The host command launches only `fkwu`; command execution,
integer parsing, comparison, and diagnosis occur inside the Form program.

## Adjudication and learning

Accepted. The predicted Form-source route works. The surprise was that the native proof body was not
missing: only its executable source carrier had been lost when `.tbl` retired. `form/validate.sh` remains
useful bulk scaffolding, but its shell-side comparison must not be cited as proof that the body adjudicated
itself. The next lift is a parameterized Form CLI request surface so this source-native runner can prove
arbitrary workloads rather than the bounded ground cell alone.
