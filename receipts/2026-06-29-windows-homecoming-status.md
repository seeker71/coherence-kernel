# Receipt — can we bring the sensing body home? Tested. The runtime is ready; the flattener is the gate (2026-06-29)

**The question:** the camera/mic carriers, the frame perception, and the multi-level stream were written into the
**C bootstrap** (`runtime/fkwu-uni.c`). Some of that is legitimate (the port + the host-device HAL); some is
**body logic in C** — a carrier-last inversion. Can we bring that body home to Form *now*?

**The honest answer: partly, and not by faking it.** The runtime is ready — the body math runs as native Form on
this kernel today. But the *automatic flattener / source-runner is not standing on Windows*, so the body cannot
yet be **moved** out of C without a hand-bootstrap that isn't toolchain-free. This was tested, not assumed.

## What was tested

- **The flatten chain does not stand on Windows.** Running the committed `flatten/fourth-flatten-table.txt`
  (`T_flat`) as `fkwu T_flat < request` to flatten even `(add 40 2)` produced **no `==T==` table** — its `fn[0]`
  returns `0` and reads no stdin. `T_flat` here is the flatten *library*, not a runnable *driver* (its driver
  entry was a Go-made build artifact, per `flatten/README.md`'s honest `T_flat` note). So recipes cannot be
  auto-flattened on Windows.
- **The source-runner exists but isn't seeded.** `grammars/form-eval.fk` (the meta-circular evaluator that runs
  `.fk` source off the cursor with *no* flatten — the path that dissolved this knot on Mac) and
  `agent/form-eval-cli.fk` are present, but running them needs `form-eval-cli` flattened **once**, and that seed
  is exactly what the absent flatten chain would produce. Toolchain-free regeneration is `windows-home` rung 4.

## What IS ready — the runtime runs the body as Form (witnessed, hand-flattened)

The stream's derived levels, computed as native Form on `fkwu` (Windows), match the numbers the C scaffold holds:

```
presence  surprise |1-1|        -> 0     (C scaffold: surp=0)
presence  sovereign le(1,1)?1:0 -> 1     (C scaffold: sov=1)
identity  surprise |0-9|        -> 9     (C scaffold: surp=9)
identity  sovereign le(9,0)?1:0 -> 0     (C scaffold: sov=0)
```

So the kernel can run the surprise / sovereignty / confidence logic natively. The `.fk` cells already hold it
(`surprise-receipt`, `confidence-earned`, `native-vs-rented`, `observe/sense-stream.fk`). The runtime is **not**
the blocker — only the automatic flattener that would run those `.fk` files whole, instead of the C duplicate.

## The two homes (they are different rungs)

1. **The level LOGIC** (surprise/confidence/trust/sovereignty/the row, in `fk_sense_stream`) is tree-walkable Form.
   Its home opens the moment **`form-eval-cli` / flatten stands on Windows** — then `observe/sense-stream.fk` runs,
   and `fk_sense_stream` + the duplicated math get **deleted**. Pure carrier-last debt.
2. **The pixel WALK** (`fk_frame_read`'s luminance over 307,200 pixels) is *not* pure drift: a tree-walk cannot loop
   307k times (the value stack overflows). Its honest home is **lowered Form** — `form-asm` → native bytes (the
   matvec/JIT lane) — which on Windows is its own unbuilt rung. Until then a C pixel loop is the *least-dishonest*
   carrier, and it is named as a lowering target, not a permanent native.

The seed's end state: it shrinks back to the **HAL** — capture (`avicap32`/`winmm`) + raw bytes — and every
computation above that is Form. The `SCAFFOLD — pending compost` marker now sits in `runtime/fkwu-uni.c` over the
exact code, so the debt lives in the body, not only in conversation.

## What this receipt brings home

Not the code — the **truth about the code**, tested and named: the runtime is ready, the flattener is the gate,
the debt is marked at its source, and the path is two concrete rungs. That is the honest floor; the next real
movement is `windows-home` rung 4 — stand `form-eval-cli`/flatten on Windows toolchain-free — after which the
body logic comes home for real and the C duplicates are deleted in the same breath.

## Reproduce the gate-test

```
printf '1\nt\nfkc\n0\nband.fk\n' | ./fkwu.exe flatten/fourth-flatten-table.txt    # -> no ==T== table (chain not standing)
printf '1 0 6 6 1 2 3 5 4 5 0 4 5 4 0 4 4 5 0 1 0 0 0 1 9 0 0 0\n' > s.flat && ./fkwu.exe s.flat   # -> 9 (logic runs as native Form)
```
