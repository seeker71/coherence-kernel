# Receipt — the server's Form runs on Windows via the SANCTIONED Go bootstrap, no T_flat, no flatten (2026-06-29)

**The correction (Urs):** I went back to `T_flat` even though we proved we don't need it. Right — `T_flat` (the
heavy pre-flattened blob) is deprecated ("flatten is optional speed, never a gate; running a recipe never requires
flattening it" — PR #20). Probing its 549 entries was chasing a ruled-out thing. Dropped.

## The T_flat-free path (the homecoming model: run the recipe without flattening it)

The sanctioned bootstrap executor is the **Go walker** (`walkers/go/main.go` — "Go is the proof/flattener
bootstrap, never the runtime"). It *evaluates* `.fk` source directly — no flatten, no table, no T_flat.

## Witnessed native on Windows 11

```
gowalker (add 40 2)                              -> 42
gowalker (nvr-ok (list 9 7))                     -> 0   (rented 7 not > native 9 — correct)
gowalker observe/native-vs-rented.fk + check     -> 11111   (a REAL committed body cell, 5/5 assertions)
```

The full server-cell grammar runs as Form source: multi-`defn`, `list`/`head`/`tail`, `gt`/`if`, cross-calls.
`native-vs-rented-check` (the oracle-economy decision cell) computes end-to-end. No flatten anywhere.

## What landed

`walkers/go/go.mod`: `go 1.26` -> `go 1.22`. The walker requires no 1.26 features (builds + runs identically on
1.22); the pin merely blocked the bootstrap on common Windows Go. Now the sanctioned Go bootstrap builds and runs
Form source on Windows with the installed toolchain.

## What this means for the purely-Form server (the new repo)

The server's logic is `.fk` source (purely Form). It is **executed by a sanctioned kernel** — the Go walker NOW
(bootstrap), `fkwu` as the native target. No T_flat, no flatten chain in the seam. The "seed" in this model is not
a flattened blob; it is a kernel that runs Form source, and the Go bootstrap is that, today, on Windows.

## The honest rungs (sovereignty receipt)

- **Bootstrap (now):** the server's Form runs via the Go walker. Go is sanctioned bootstrap — a rung BELOW the
  sovereignty receipt, named as such.
- **Native (target):** `fkwu` runs the same `.fk` source itself. That is fkwu's own source path maturing
  (`form-eval` off the cursor, or `--src`) — NOT T_flat. The carriers (`read_line`/`read_file`/`print_str`) are
  already witnessed working on Windows; the grammar gap is the remaining native work, not a flatten dependency.

`surface/core.fk` uses the higher surface grammar (`def f(x) = … then … else …`), which the raw s-expr walker
does not parse; the s-expr body cells (`observe/*.fk`) run directly, which is what the server is authored in.

## Reproduce

```
cd walkers/go && GOTOOLCHAIN=local go build -o ../../gowalker.exe .
printf '(add 40 2)\n' > t.fk && ./gowalker.exe t.fk            # -> 42
( cat observe/native-vs-rented.fk; echo '(native-vs-rented-check)' ) > nvr.fk
./gowalker.exe nvr.fk                                          # -> 11111
```
