# Receipt — re-grounding the flatten question: Windows was never the blocker; the gap is a platform-neutral seed (2026-06-29)

**Correction.** An earlier note (`windows-homecoming-status`) framed this as "flatten does not stand on Windows."
That framing was wrong — it leaned on a stale assumption (a Go/`bin-go` seed) and a single mis-read test, not
evidence. Re-grounded by testing the actual kernel. The honest, witnessed truth is below; the prior framing is
composted.

## What is PROVEN on the Windows kernel (it is fully capable)

The c-bootstrap `fkwu` on Windows runs the whole native surface — same kernel, same numeric tables, identical to
Mac/Android. Witnessed on metal:

- **Recipes compute.** `(add 40 2)`, `(if (le 1 2) ...)`, nested forms → 42 (windows-home receipt).
- **stdin → stdout round-trips.** `(print_str (read_line))` fed `hello from stdin` printed `hello from stdin`.
  `read_line` (tag 114, `read(0)`) and `print_str` (tag 115, `putchar`) both work through the Windows port.
- **Staged input + eval work.** `input_byte` (tag 17, `argv[3]`/`fk_src`) reads source; the eval tags run native.

There is **no Windows-specific gap** in the runtime. The port is sound.

## What running the committed flatten table actually does (the mis-read, corrected)

`flatten/fourth-flatten-table.txt` is the **full flatten body** — `nf=549` functions, `23,322` nodes (not "tiny";
I was wrong about that too). But its **entry is not the driver**. Running it with a real flatten request and
reading the per-tag walk counters (`fk_arms`):

```
fkwu fourth-flatten-table.txt  <  "1 / t / fkc / 0 / band.fk"
  arms[111] reserve     = 1     <- fn[0] ran
  arms[114] read_line   = 0     <- the driver NEVER read the request
  arms[31]  str_to_int  = 0
  arms[115] print_str   = 0     <- nothing emitted
  root value            = 0
  ==T== table           = (none)
```

`fk_run` only ever executes `fn[0]`. Here `fn[0]` is a `RESERVE`-wrapped function that returns `0` without
touching `read_line`/`str_to_int`/`print_str` — so it is **not** `fourth-flatten-driver`'s `(do ...)` entry. The
committed table therefore flattens nothing as shipped — **on every platform**, because the driver's `do`-block is
not at `fn[0]`. This is a *seed-entry* fact, not a Windows fact.

## The real shape of the gap

The keystone is **one** flattened seed whose `fn[0]` IS a runnable entry — the flatten driver, or (better) the
source-runner `form-eval-cli-loop` (`core` + `input-stream` + `form-eval-full`). With that one seed, `fkwu` runs
*all* `.fk` source off the cursor with no further flatten (`HOMECOMING` rung 1, the Mac crossing).

That seed is **platform-neutral numeric data** (the `fk_next` token stream). Whatever stands on Mac/Android IS
that seed, generated there. The C kernel carries no source parser (confirmed), so the seed is produced by the Form
flattener — and once produced, it is the same bytes on every platform.

## The fix is shared data, not a port

Because the seed is platform-neutral, the resolution is trivial and needs no Windows work:

1. **Commit the generated seed** — the flattened `form-eval-cli-loop` table (and/or a flatten table whose `fn[0]`
   is the driver entry). It is a regenerable cache `fkwu` makes itself (`flatten/README.md`'s stated direction),
   not a Go artifact.
2. The Windows kernel — **proven ready** — runs it immediately: `fkwu eval-cli-loop.tbl 0 <any-recipe.fk>`.

## ⟐ Coordination — to the Mac sibling (`macos-binary`)

Flatten/eval stands on your side. Please **commit the generated seed table** (`form-eval-cli-loop` flattened over
its preludes, or the driver-entry flatten table). It is platform-neutral; the Windows cell will run it on first
pull with no changes — and then the body logic now scaffolded in C (`sense-stream`, `frame-read` math) comes home
to its `.fk` recipes and the C duplicates are deleted. One committed seed closes the homecoming for Windows.

## Honest floor

- Windows kernel: **capable, proven** (recipes/stdin/eval native). Not the blocker.
- The committed `fourth-flatten-table.txt`: `fn[0]` is not the driver entry — does not flatten as shipped (evidenced
  by the zero `read_line`/`print_str` walk counts).
- The standing seed is generated, platform-neutral, and not yet committed here — that, not "Windows," is the gap.
- bin-go: dropped as a live reason — the direction is `fkwu`-self-derivable; the seed is a cache, not a Go binary.
