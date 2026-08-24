# 2026-08-23 — share kind is computed; Apply is one method

Yes did not trust `kind=declared` to be a count, and did not want
`metal_pipeline` / `mlx_run` / `form_cpu_jit` sitting as named doors
in separate costumes.

## Where share kind is produced

`form-cli-share-run.fk` prints `(fcte-kind-name evidence)` with
`evidence = (fctel-evidence)`.

```
form-cli-turn-evidence.fk
(defn fcte-kind (e)
    (if (eq (fcte-valid? e) 1) (fcte-kind-observed)
        (if (eq (fcte-carrier-present? e) 1)
            (fcte-kind-embodied) (fcte-kind-declared))))
```

That fold is real. This checkout has no `.form-cli-turn-rollout` and
no `.form-cli-turn-evidence-row`, so `fctel-row` is the string
`unmeasured`, the carrier is absent, and kind is **declared**. The
percentage is withheld. Walk lanes (`fcr-walk`) are still computed.

A second, older cell still types a kind: `fcl-this-kind` in
`form-cli-local-law.fk` is literally `(fcl-share-kind-declared)`, and
`fcl-this-turn` is the offering `50 20 30`. Share-run does **not**
print those integers when evidence is unreconciled. They remain a
trap if a caller reads them as a tally.

Observed share on this Grok session is a named gap: the live binder
is Codex-rollout shaped. We did not invent a row.

## Doors removed from the organ

`form_cpu_jit` was a second CPU ABI through Metal's mmap. Form-lower
plus `jit_leaf_inram` is the CPU emit. Host leaves are not fields.

`FormJit<T>` now holds backend ids as data (`Mlx=0`, `Metal=1`,
`Cpu=2`) and one method `fjit-apply(backend, n)`. High-grammar
authority is `bml/form-cli-jit.bml`: `enum JitBackend`,
`template BackendCell<TImage>`, `class FormJit`.

```
./fkwu form/form-stdlib/tests/form-cli-jit-band.fk   # 1023
./fkwu form/form-stdlib/form-cli-jit-run.fk
  apply mlx=22  apply cpu=22  apply metal=22
```

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-23 -> share kind computed declared (no rollout row), jit-band 1023, apply 22/22/22
