# Receipt — docs brought to current truth + a fresh agent builds the form-cli on bootstrap (2026-06-29)

**The ask (Urs):** update all the documentation, and ensure a new agent can build the form-cli on the bootstrap.

## A new agent's bootstrap — witnessed (fresh clone, not the working tree)

```
gh repo clone seeker71/coherence-kernel fresh-clone --depth 1
cd fresh-clone
gcc -O2 -o fkwu.exe runtime/fkwu-uni.c -lws2_32 -lwinmm -lavicap32 -luser32 -lwlanapi -lbthprops -lwinhttp
  -> build exit 0, fkwu.exe 297162 b
( cat observe/native-vs-rented.fk; echo '(native-vs-rented-check)' ) > nvr.fk ; ./fkwu.exe --src nvr.fk
  -> 11111      (a real body cell, native, no Go / no flatten / no T_flat)
```

One `cc`/`gcc` command on `runtime/fkwu-uni.c` -> the kernel; the kernel runs the Form body. macOS/Linux:
`cc -O2 -o fkwu runtime/fkwu-uni.c` (the host-carrier libs are `_WIN32`-gated). The build of the runtime touches
no Go, Rust, Python, or TypeScript.

## Docs brought to the truth this session proved

- **README.md** — added a **Build it** section (one C seed -> native, mac/linux + Windows, with the `11111`
  verify); the kernel bullet now names `fkwu --src` (its own source-runner) alongside `form-eval`, and states the
  walkers are never the runtime.
- **AGENTS.md** — added **Build the kernel first** (the entry a new agent needs): the build command, the `--src`
  verify to `11111`, and "the Go/Rust/TS walkers are four-way proof siblings only — never the runtime."
- **HOMECOMING.md** — Rung 1 now records that the source-runner runs **real body cells** (`native-vs-rented` ->
  `11111` on fkwu, bit-identical to the walkers, no Go/flatten/T_flat); remaining = strings + the interactive loop.
- **flatten/SEED-DROP.md** — **superseded for the run path**: the body already runs via `--src` with no seed; the
  doc is retained only for the optional `form-eval` door (a flatten *cache*, never a gate). The "the only missing
  piece is the seed" framing is composted.
- **walkers/README.md** — already correct ("These are NOT the runtime"); left as-is.

A scan for stale claims (body can't run on Windows / needs the seed / a walker runs the body) came back clean.

## The standard-receipt rung this documents

`c-bootstrap fkwu` + Form source + real metal (Windows witnessed; mac/linux the same one-seed build) + **no go /
rust / clang / bash / python in the run** = a real rung of the sovereignty receipt, now the documented default a
fresh agent reaches in two commands (build, run).
