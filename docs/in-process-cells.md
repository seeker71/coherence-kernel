# In-process cells

Urs, 2026-10-04: "for the body to call fkwu makes no sense to me ... design and architect a different direct integration that does
not spawn a new child process and then has to parse stdout, stderr and exit code which is highly inefficient."

**The rule.** The body does not begin `./fkwu` to run a cell and read a text protocol back. It calls the cell in its own process and
takes the answer as a value. A *cell* here is a unit of Form that can be run: a `.fk` or `.bml` file and the preludes it names (a
band, a gate, a door run file, a probe). Spawning stays for what the operating system must isolate or what is foreign (below).

This page says what is built, what is a patch waiting for the seed to be free, and what is only designed. Where it says "measured"
the number was read inside Form on `host_monotonic_ms` and `host_cpu_us` on 2026-10-04, on a busy machine (siblings were running);
read the ratios, not the digits.

## Status

| part | status | witness |
| --- | --- | --- |
| `cell_run(path, arg, deadline_ms, stdin)` answering a record (leaf-door mode 50) | **prototyped as a patch**, built and proven on a temporary seed | `form/form-stdlib/tests/cell-run-band.fk` reads 524287 on it; "door absent" (verdict 0) on a seed without it |
| stdout sink, stderr as diagnostic rows, stdin as a value, deadline | in the same patch | bits 32, 64, 128, 256, 4096 of the band |
| name windows, residency by path and content, freshness | in the same patch | bits 1024, 2048, 8192 |
| band sweep with a `cell_run` per leg, shardable | **landed**: it is `gate/band-sweep-run.bml` over `form/form-stdlib/bml/band-sweep.bml`; the spawn path (`bsw-begin`, the pool, the fifo release, the `jobs` and `fkwu` keys) and the duplicate `band-sweep-cells` files are deleted | `form/form-stdlib/tests/band-sweep-band.bml` reads 1044479 of 1048575 (claim 4096 fails, cause not yet read); 12 of 35 workloads (shard 0 of 3) gave the same verdict lines as the spawned sweep, and shards 1 and 2 ended on a SIGBUS with the signature `fk_cell_dep_find` later fixed (not reproduced on those legs, not yet rerun); `{"shard":i,"of":n}` splits the work into disjoint parts whose union is every workload |
| the dead `fmv-ask` row-walk of `form/form-stdlib/form-cli-movement.bml` | **removed** | it had no caller; its `./fkwu` child is gone |
| drift gates (`gate/drift-gates.bml`: every row a `cell_run`; the closure of a row's unit read in Form with the loader's own dependency reader and the home index, equal to `./fkwu --closure` on all 15 rows), canonical conformance (`gate/canonical-conformance.bml`), band-pin (`observe/band-pin-run.bml`), the band legs of `findings-requests.bml` | **landed** | `gate/tests/drift-gates-band.fk` 262143, `gate/tests/canonical-conformance-band.fk` 1023, `observe/tests/band-pin-band.bml` 31, `form/form-stdlib/tests/findings-requests-band.bml` 1048575 |
| the native-turn, local-plan and code-circle band legs, the host walk's turn, plan, review, gates and movement, the landing's gates and the reunion's redraw, the native-op-emitter check, preflight (its probe, its fresh run, its compile-only reading), tree-heal | **landed** (2026-10-04): each is a `cell_run` through `hch-cell-in` (`host-child.bml`) or directly; the result is read from `value`, `stopped`, `errors`, `diag` and `err` (`cell-said.bml` renders the last two as a child's stderr said them) | `host-walk-band` 1099511627775, `form-cli-code-circle-band` 1048575, `form-cli-local-plan-band` 33554431, `native-turn-ladder-band` 262143 (its door called in process), `native-op-emitter-band` 31, `preflight-band` 131071, `preflight-exit-status-band` 15, `tree-heal-band` 255, `form-cli-landing-band` 131071 of 262143 (its last claim, the reunion's default wiring run by a kernel child in a scratch checkout, reads 0 while that child cannot lower the chain from a scratch directory: "bml lowering did not settle in 256 rounds", also on the seed before this patch and on units this lane does not touch); the preflight pages are byte-identical to the spawned ones; `cell_run` gained the `err` field and the `"check"` mode in the same patch |
| form-cli-heal, the remaining lens children | designed (migration order below) | not built |
| a progress record in the shared field for a supervised worker | **built** (2026-10-04) | `form/form-stdlib/voice-track-record.bml`; `form/form-stdlib/tests/voice-track-service-band.fk` reads 16383 (the record, the stall decision, nothing begun) |
| the voice-track worker as a service, its supervisor reading the record | **built** | `form/form-stdlib/voice-track.bml` begins nothing (membrane census: 0 sites), `docs/launchd/earth.hati.voice-track-worker.plist` is the service as data; `form/form-stdlib/tests/voice-track-reap-band.fk` reads 7 on the new logic |
| the glass supervisor and the ear lanes as services | designed, reasons in "Services" below | not built |
| the seed's own BML-floor child (`fk_bml_child_open`) | **removed**: the floor compiler is a cell of the seed's process | cold lowering of every `.bml` and floor-lowered `.fk` in a fresh tree, old and new seed side by side, same bytes (the "Lowering in this process" section) |
| the fatal-signal organ: a handler per thread writes one organ-health row (frames, cell in flight, C chain) before the process dies by SIGSEGV, SIGBUS, SIGILL, SIGFPE or SIGABRT; a stack that ends is a `stopped` `"stack-depth"` with frames; Form reads the file as findings and as the immune system's attention | **landed** (2026-10-04) | `form/form-stdlib/tests/fatal-signal-band.fk` reads 16383 (section "A stack that ends is a stop; a signal that ends the process is a row") |

Deployment: apply the patch (inproc.patch, from the repo root: `git apply -p2 inproc.patch`; it applies cleanly to the tree as it stood at
the end of this work, siblings' filesystem doors included) when `runtime/` is free, rebuild with `cc -O2 -o fkwu.new runtime/fkwu-uni.c && mv fkwu.new fkwu`,
run `./fkwu form/form-stdlib/tests/cell-run-band.fk` (524287).

Two things the seed in use still owes the callers that landed (each a few lines in `runtime/fkwu-uni.c`):
- `file_read(0, n)` (leaf op 134) must draw from the active cell's stdin value while `fk_cell_in_on`, as `read_line` does; a door that reads its
  input with `stdin-text()` (`form/form-stdlib/bml/stdin-source.bml`: `file_read` on fd 0) otherwise reads the host's fd 0 and not the value
  its caller handed it. `gate/form-cli-build-run.bml` reads `{"out": ...}` that way: with the stdin lost it would run its default install.
  The sweep and band-pin read their input a line at a time (`read_line`) and are not waiting on it.
- `fk_cell_load` must reset the owner cache of the function rows it is about to register (`fk_fsym_dep[r] = -2` for every `r >= fk_fntop`
  before `fk_prescan_defns`): a row index an earlier load gave back keeps the owner it cached, and a later cell's own `defn` is then
  outside its own name window. Seen: `cell_run("gate/structural-gate-run.fk")` and then `cell_run("form/form-samples/fact.fk")` answered
  nothing with two `unresolved-call 'fact'` rows; with the reset it answers 3628800 (the sweep's first leg was the casualty).

## The call

```
let r = cell_run("form/form-stdlib/tests/voice-chunk-band.fk", 0, 30000, ["line one", "line two"]);
```

`path` is a string, `arg` is the value the program receives as its argument (the int `./fkwu <path> <n>` would hand it), `deadline_ms`
is an int of milliseconds, `nothing()` for none, or the word `"check"`: the compile-only door (what `./fkwu --check <unit>` was). In check
mode the unit and its closure load and compile, their diagnostics come back as rows, and nothing runs, not the unit and not a
library's top level, so a unit marked `preflight-exec: forbidden` can be read (`value` is `nothing()`, `errors` and `diag` are the
compile's). `stdin` is a list of strings (the lines `read_line` returns, then `nothing()` at the
end) or `nothing()` for an empty input. It answers one record, always (a call that cannot be made is a stopped record, never a crash):

| field | meaning |
| --- | --- |
| `value` | the unit's final value as the Form value it is, no stringification; `nothing()` when it stopped |
| `out` | everything the unit printed (`print`, `print_str`, every byte the seed writes to stdout), captured, never written to fd 1 |
| `err` | everything the unit said to stderr, as text: the seed's own lines and organ-health rows, and what Form code appended to `/dev/stderr` (`live`'s readings, the proof outputs), which would otherwise reach the host's fd 2. `diag` carries the compiler's rows structured and replayed on a resident call; `err` is the bytes of this call (a resident call's compile rows are in `diag` only). `form/form-stdlib/bml/cell-said.bml` renders `diag` and `err` once, as the lines and organ-health rows a reader of a child's stderr takes (`cs-stderr`) |
| `diag` | a list of records, one per organ voicing or compile diagnostic: `organ aspect stage observed detail path name line health at_ms` |
| `stopped` | `nothing()`, or `{kind, message, recipe, unit}`: kind `"stop"` (a Form-level stop, the `attempt` mechanism's catch: the same line a spawned run dies with), `"deadline"` (the budget ended it), `"unrunnable"` (it would not read as a program), `"refused"` (the call itself was malformed), `"stack-depth"` (a stack that ended, named before it was ever a crash: the message reads `stack-depth: <unit> <function> (...)`, `recipe` and `unit` say where, `frames` is the walker's chain, innermost first, as `"<function> (<unit>)"` (up to 24), and `depth` how many frames deep it was; see "A stack that ends is a stop") |
| `errors` | the compile errors counted in `diag` (a spawned `fkwu` exits 1 on them) |
| `resident` | 1 when the unit was already parsed here, 0 when this call loaded it |
| `ms`, `cpu_us` | wall and process CPU spent in the call |

There is no exit code: "exit 1" is `stopped` not nothing, or `errors > 0`. There is no last line: the answer is `value`. There is
no stderr to re-read: a diagnostic is a row. A leg is judged in `form/form-stdlib/bml/band-sweep.bml` from the record: its first
`Verdict n` and its row in `form/band-verdicts.txt`.

### The sinks

Every byte the seed writes to stdout goes through `putchar` or `printf`; every stderr line through `fk_write_all_raw`, `dprintf` or
`vdprintf`. One definition of each, at the top of `runtime/fkwu-uni.c`, is the whole sink: with no cell running each passes straight
to libc (one load and one branch); with one running, stdout appends to the call's buffer and stderr is dropped, because the four
producers of organ voicings (`fk_sig_head`, `fk_stop_voice`, the two diagnostic signals) hand the row to `fk_dg_*` at the source,
with its fields, instead of composing a JSON line that a parser must read again. `read_line` and `file_read(0, n)` draw from the call's stdin value
instead of fd 0 (`file_read` takes up to n bytes of what is left; cell-run-band claim 524288). A function row an earlier load gave back is
numbered again by the next, and the owner cached for it is asked again (claim 1048576: `fact.fk` after a gate cell). Sinks nest: a cell may call `cell_run`; the inner call's output, rows and input are its own, and the outer's are put
back when it returns (bit 16384).

### The budget

A walker step counts; every 16384th compares the clock to the deadline (`FK_CELL_TICK` in `fk_walk` and `fk_walk_body`). When the
budget is spent the walker jumps straight to the call's own recover point, past any `attempt` the cell holds, so an inner `try` cannot
swallow the end. The cell comes back `stopped: "deadline"` with what it had printed (bit 4096: a cell that never ends returns at its
300 ms, measured 300 ms). What this cannot end: a unit inside a crystallized hot leaf (the check is the walker's), a blocking
host call (`host_wait` on a hung child, an uninterruptible GPU wait) and `fk_die` (out of memory, a broken table), which end the process
as they always did. Those are the reasons a wedge-prone worker is a service, below.

### Scope and residency

A cell's text is read once per path and content. The collector appends the unit and the preludes it does not already hold to the
assembled source; the parser registers their definitions after everything already there (function indices and nodes only grow); a
cell is parsed against a **name window**: `fk_fn_lookup` and `fk_const_lookup` see only rows whose text belongs to the cell's own
closure. So two cells that define the same helper name (`vb-bit`) never see each other's, a library keeps one parse for every cell
that preludes it, and a cell that forgot a prelude is told so (`unresolved-call`, `errors > 0`) although a sibling cell defining the
name is resident (bit 2048: the fixture scope-c, in `form/form-stdlib/tests/fixtures/cell-run/scope-c.fk`, calls crs-pick and crs-lib, defines neither, and
reads `errors` 2, while scope-a and scope-b beside it, which define crs-pick differently, answer 11 and 12 in any order). Code already parsed resolved its calls to function
indices at its own parse, so a later definition of a name shadows nothing that exists; this is why the old failure ("a duplicate def
name across a file and its prelude closure: the later arity wins") needed no rename pass.

- A library's top-level forms run once, when it is loaded; the cell's own run on every call, with its top-level `let` holds reset,
  so a high-grammar cell with `let doc = ...` is built again (bit 512, and again resident in bit 1024). Library load-time output is
  the first caller's; libraries are pure at import by convention ("No effects at import").
- A root that only names preludes (the sweep's workload roots) answers with the last unit in text order that has a sequence,
  which is what the one flat program answered.
- Freshness is the `.fkb` image's question: a dependency whose mtime and size stand and are older than two seconds is what was parsed;
  any doubt reads the bytes and compares the digest. A file rewritten to the same length inside the same second is the new cell
  (bit 8192). A stale unit's path gains `#stale` and its closure is collected again; a closure unit that also serves another cell
  stays correct for it through its own window.
- A cell's side effects on global state (the shared field, files, Metal buffers) are the cell's own affair.
- What happens to the arena: nodes, function slots and source text of a loaded unit are **append-only** and stay resident for the life
  of the process; no truncation is attempted, because the hot-leaf ledgers are keyed by function index and a reused index would
  inherit another function's heat. Measured: the heap of 1,200 calls of voice-chunk-band fills to its first collection (RSS 14.7 MB
  after the cold call, 92 MB at about call 1000), then stands: +98 KB over the next 500 resident calls (about 200 bytes a call, the
  result record; the record table has no collector), and 650 bytes a call for a cell that returns three records. Eviction by mark
  (the LIFO truncation `store`/`restore` use for choice points) is designed for loads that fail, where nothing ran; it is not needed
  for the rate above.

### A stack that ends is a stop; a signal that ends the process is a row

A cell shares its process, so what ends a process ends every cell in it. Two layers answer, in this order (runtime/fkwu-uni.c, "THE
FATAL-SIGNAL ORGAN"; `form/form-stdlib/tests/fatal-signal-band.fk` reads 16383):

- **The stack is a stop.** The walker measures real stack use at every step (`fk_depth_wall`, the wall at the run thread's reserve less 2 MB) and the
  source collector counts its own depth (`fk_src_collect_dep`: at most 2048 nested preludes, and the same stack wall). Past either, the walker
  unwinds to the call's recover point and the collector returns false at every level; the record answers `stopped` kind `"stack-depth"` with the unit,
  the function and the frames, and the process stands (the next `cell_run` answers). Measured, 2026-10-04: a two-function non-tail recursion stops at
  2047 frames deep, 266 MB of walker stack, in 18 ms; the cycle of two `.bml` units that name each other (`fk_cell_load_base` is what makes it settle)
  with that repair switched off stops at the collector's 2048th level in 4.6 s naming `fk_src_collect_dep` and the units it was entering. A program run
  by `./fkwu <file>` whose walker meets the wall outside every recover point prints the same line, ends with exit 1 and writes the row below (name
  `STACK-DEPTH`, signal 0, the frames; witnessed with `form/form-stdlib/tests/fixtures/fatal-signal/dive-cell.fk` run as a program).
- **A signal is a row.** Each thread has its own alternate stack and a handler for SIGSEGV, SIGBUS, SIGILL, SIGFPE and SIGABRT. At the moment of death
  the handler writes one organ-health row (organ `fkwu-seed`, aspect `fatal-signal`, health 0, surprise 1, the need `fatal-signal`, the offers
  `request-evidence` and `revise`) to stderr as a `form-organ health` line and appends it, whole and by one write, to `.hearth/fatal-signal.jsonl`
  (opened with `O_APPEND` then, nothing held earlier; no more rows past 4 MB). Its `evidence` is what the process knew: the signal and the fault
  address, the resident bytes, the `cell_run` units in flight (outermost first) and the program's root, the walker's Form frame chain (`frames`: the
  innermost 24, each a function and its unit; `depth`), the collector's depth and the units it was entering, and the C call chain with symbols (`c`).
  Then the default action is restored and the signal raised again, so the exit stays 128 + n. The walker's chain is kept by the four non-tail call
  arms (a saved caller each, put back by `fk_attempt` and `fk_cell_walk_guarded`); a unit inside a crystallized hot leaf shows its entering frame.
  Nothing a handler cannot do safely is done in it: no allocation, no stdio, static buffers and table reads only.
- **Form reads it.** `form/form-stdlib/bml/fatal-signal.bml` reads the file's tail as findings (the last 24 hours, one group per signal and innermost
  frame with its count, at most six groups listed and the rest counted); `form/form-stdlib/bml/fatal-signal-attend.bml` speaks each group's row and hands it to `oc-hear`, the
  care every organ reading takes (no provider claims a fatal signal; the attention is "unclaimed", the repair is code), and `observe/local-flow-review.bml`
  runs it first. A leg that ended by a signal is a finding, never the bare number: `host-child.bml` appends the finding (the signal, the cell, the frames)
  to a child's text when its exit is 128 + n and keeps it in `<io>/<step>.finding`; `band-sweep.bml` and `gate/drift-gates.bml` print the last day's
  findings before their first leg and read a `stack-depth` leg as its stop with its frames.
- **What the band begins, on purpose.** A fault ends the process that raises it and nothing in `cell_run` can contain a SIGSEGV, so the band begins the
  seed in use through `hch-run-in` (class D: the child is the thing under test). `FK_TEST_FAULT` (an environment word, absent in every product run) lets
  `cell_run("fk-test-fault")` raise `segv`, `bus`, `ill`, `fpe`, `abort` or `cstack` (a real C stack overflow), and the words `load-base` and
  `no-collect-guard` switch off the two repairs whose absence the organ must still catch.
- **The sweep shards' death, as the organ reads it** (cyc-a.bml and cyc-b.bml with both repairs switched off, 55 s to the guard page):
  `SIGBUS at 0x16b3cbff8`, cell `form/form-stdlib/tests/fixtures/cell-run/cyc-a.bml`, collector depth 21097 entering `cyc-a`, `cyc-b`, `cyc-a`, ..., 448 MB
  resident, the C chain `fk_bml_packet_body < fk_bml_lower_to_mem < fk_src_collect_dep < fk_src_collect_bytes < fk_src_collect_dep < ...`, exit 138.

## What it costs (measured, one busy machine)

Spawned = `./fkwu <cell>` from Form with `host_spawn_at`, `host_wait`, six warm runs. In-process first = the first `cell_run`, which
parses the closure from the lowering memos. Resident = the mean of the next 100-200 calls.

| cell | spawned (ms, warm) | in-process first (ms) | resident (per call) |
| --- | --- | --- | --- |
| `(do 7)` (the floor) | 44-97, median 56 | 0 (0.9 ms CPU) | 0.1 ms (34 us CPU) |
| voice-chunk-band (answers 4095) | 86-203, median 100 | 61 (17 ms CPU) | 4.4 ms (1.0 ms CPU) |
| substring-one-meaning-band | 59-114, median 90 | 20 | 8.8 ms (2.3 ms CPU) |
| file-bytes-band | 29-109, median 70 | 1 | 1.9 ms |
| str-find-one-meaning-band | 53-110, median 93 | 46 | 33 ms (the band's own compute) |
| 20,000 lines of output | 286-501, median 390 | 31 | 31 ms |
| anf-shadow-band (compute heavy) | 5,690-6,860 | 6,708 | 6,300 ms |

Read it plainly: a spawn costs a floor of about 55 ms before the unit does anything (process start, image read, the walker's setup),
plus the unit's own load; a resident call costs the unit's own work and nothing else. A band that is nothing but its work (anf-shadow)
gains nothing, and a sweep dominated by such bands gains little in wall time from this alone. What the sweep gains: the floor and
the load for every small band (about 400 workloads at the floor is 20 s), output that no longer crosses a pipe (twelve times for the
20,000-line cell), a budget that returns instead of killing a lineage, and an answer that cannot be mis-read. The gates ahead of the
first leg are the larger bill today: for four workloads the spawned sweep took 59 s and the cell sweep 41 s, and about 35 s of
each is the validation-start gate, which runs the drift gates, each a shell and a child (migration 2).

One thing the text protocol hid: a spawned run that reads its cached image **does not compile**, so it does not voice its compile
diagnostics. The same unit run twice as a child says `unresolved-call` the first time and only "cached image was compiled with errors"
the second (witnessed on `form/form-stdlib/tests/fixtures/cell-run/warn-cell.fk`, 2026-10-04). A `cell_run` is deterministic about it: the load's rows are kept with the unit
that caused them and replayed on every resident call (bit 1024).

## Parallelism and isolation without a spawn

- **A pool of legs becomes shards.** The band sweep ran eight legs at once because each was a process. Now a sweep takes
  `{"shard": i, "of": n}` and runs the workloads whose enumeration index is `i` mod `n`, one after another in its own process, the
  preludes the bands share staying resident. Something that is not the body starts the `n` sweeps (an operator, launchd, the OS); the
  body spawns nothing and every shard reads the same tree. The seal (HEAD plus git's status, read at start and end) holds per shard.
- **A wedge-prone worker is a service** (built for the voice-track worker, below, in "Services"). The Metal render worker can sit in an
  uninterruptible GPU wait that only a kill ends. It is started and restarted by the host's manager (launchd `KeepAlive`), not by
  the body. The body's supervisor (`form/form-stdlib/voice-track.bml`) begins nothing: it reads the worker's progress as one typed
  record in the shared field, `{pid, seq, stage, done, total, begun_ms, beat_ms, state}`, and a worker that says it is working and
  whose `beat_ms` stops moving past its budget is ended with `host_signal` (SIGTERM, a grace window, SIGKILL), and the manager
  starts the next. Nothing is parsed from stdout.
- **What stays multi-process by nature** is the thing under test when the test is about processes: the cross-process lease in
  `form/form-stdlib/tests/gpu-lease-band.fk`, `form/form-stdlib/tests/host-signal-band.fk`, `form/form-stdlib/tests/voice-track-reap-band.fk`, the field and cell-store children,
  the glass wait bands. They are tested with a long-lived peer an operator started, addressed through the field, or with a virtual
  owner double for the pid and `host_alive` reads; a band that spawns a child on purpose says so in its header. The cell-run band is
  one: its oracle runs each unit as a child, because the claim is that the record equals what the child said.

## What may still spawn

- the C compiler building the seed (`cc -O2 -o fkwu.new runtime/fkwu-uni.c`) and the Metal carrier;
- `git`, until Urs decides;
- a supervised service the host's manager starts (below), and foreign programs the body does not yet carry natively, which the
  membrane census names and counts (`observe/membrane-sites-run.bml`: `{"word":"fkwu"}` lists the `./fkwu` crossings that remain; the
  count must fall as migrations land). In the host walk's lane that is: the native code lane of a turn (`nt-code` in
  `observe/native-turn-run.bml`: a model session that holds the one Metal client and ends by pid, class C), the landing's witness
  (`/bin/sh -c`, shell text by contract), the argv seam of the reunion's redraw (a stub, or another checkout, run with `env -C`), and
  the landing band's default-wiring claim, which keeps a kernel child in a scratch checkout because the directory the process stands in
  is the thing under test (class D).

Nothing else. A new `host_spawn_at(["./fkwu", ...])`, `host-exec("./fkwu ...")` or wrapper of them to compute a value, run a band or
read a verdict is the thing this page ends. (The seed's own `.bml` lowering child is no longer on this list: it is gone, below.)

## Lowering a `.bml` in the seed's own process

The seed used to begin `./fkwu form/form-stdlib/bml-floor-compile.fk` of itself over two pipes the first time it met a `.bml` (or a
`.fk` with a brace section) whose lowering memo (`<unit>.lowfk`) was missing or stale, and read each lowered unit back as text. In a fresh
checkout that is the minutes-long first run of every unit. It is the floor compiler's work done in a process of its own; it is now done in
the seed's process, as a cell:

- **The dependency that decides the order.** The floor compiler is a plain `.fk` closure of 23 units
  (`./fkwu --closure form/form-stdlib/bml-floor-compile.fk out` lists them): core, the ontology, the grammar and lift tables, the BML source
  and lowering units, the source compiler. None of them is a `.bml`, so the seed reads the floor with its own C reader before it can lower
  anything, and there is nothing to break: a `.bml` met while the floor itself loads is a refusal that names it (`fk_bml_floor_busy`), never a
  recursion. The floor is loaded once per process as a resident unit of the cell door (`cell_run`'s collector, name window and freshness).
- **What a request is.** The floor's stdin is its protocol, unchanged: a source path, `@memo`, the home registry path, three lines a unit.
  The seed hands the lines as the call's stdin value and takes the call's `out`, the lowered packet and the floor's sentinel, once per
  request. One call serves a whole frontier of requests, so the floor pins its own sources and reads each file of a closure once per call,
  as the resident child did once per runner. The packet is validated against disk exactly as before (`fk_bml_packet_body`) and written as the
  same memo (`fk_bml_low_memo_write`); the floor's diagnostics, which the child wrote to the seed's stderr, are voiced on the unit's path.
- **Where a miss is met.** A memo is read with no spawn and no lowering, as before. A miss meets the seed in one of three states:
  while the cell door loads a cell (`fk_bml_defer`) the miss is recorded and an empty unit comes back, so the load finds the rest of the
  closure; the door gives the load up, lowers the whole frontier in one floor call, and loads again (the closure's depth, not its size, is
  the number of rounds). While a command is still loading its program (`fk_load_phase`), the half-built arena is abandoned: the floor is
  loaded beyond it, the missing memos are lowered, the same closure is scanned again in cell mode without parsing to meet the memos that
  lowering made reachable (a `.bml` named by a `.bml`), and when a scan finds none the process begins its own command again (`execvp` of
  itself: the same pid, no child) and finds a warm tree. A round count rides in the environment (`FK_BML_ROUND`, taken and cleared by the new
  process before anything reads the environment), so a tree whose memos cannot hold, a directory that is not writable, stops after three
  rounds with a refusal instead of looping. Once a program is walking, a miss is a refusal naming `./fkwu --check <file>`: nothing is
  collecting that could be restarted.
- **Why the arena is not shared with the program's own load.** The loader assembles the program as one text; a floor unit loaded in the middle
  of that collection would be parsed as part of the program, and its `bfc-serve` loop would read the program's stdin. The abandoned-arena
  begin-again is the way to keep both honest, and it is what the seed's shrink path replaces: when the collector is Form's, the miss list
  and the begin-again are the loader's own recursion, and `fk_bml_need_*`, `fk_bml_defer`, `fk_load_phase`, `fk_bml_bootstrap` and
  `fk_bml_scan` leave with it (`runtime/fkwu-uni.c`, "THE BML FLOOR COMPILER, A CELL OF THIS PROCESS").
- **A unit the floor refuses is said once.** A floor call that stops (`form_error`) is set aside and read again whole by the next call (its
  readers were left mid-read: one unterminated string made every unit after it "unterminated"), the stop is voiced on the unit it stopped on
  (the old child said only "lowering child failed; run the floor by hand"), the unit is not asked again by this process, and the load
  fails as before (exit 2) after every unit that could be lowered was.
- **Two spellings of one unit.** A memo names one owner path; the same unit met as `/abs/x/form/...` and as `form/...` (a symlinked
  checkout, as the landing band's scratch checkout is) took turns overwriting each other's memo and never settled ("did not settle in 256 rounds":
  `form-cli-landing-band` claim 131072). A spelling whose primary memo belongs to another owner keeps its own beside it,
  `<unit>.lowfk.s<digest of the spelling>` (git-ignored).
- **Measured, 2026-10-04**, on a machine at load average 100-220 (a LoRA run training, many sessions):
  - *Controlled, simultaneous*: 16 units cold, fresh tree each, old and new side by side: old 277 s wall, 20.4 s user; new 274 s wall, 20.3 s
    user. A unit costs the same; the floor is the same program.
  - *The whole set*: 1,153 lowering units (784 `.bml`, 369 `.fk` the floor lowers), a fresh tree copy per shard (no memo, no image), eight shards
    per seed. Old: 73-94 min wall a shard (at load 150-220), three shards ending 0 and five ending 2 (the first unit with an unterminated string
    literal in the day's snapshot, which the child answers by dying). New: 10-14 min a shard (later, at load about 100-116, so not a ratio), the
    same three 0 and five 2, going on past the failing unit and naming it. **Byte equality**: every memo both seeds wrote, 3,690 files
    across the shards, is identical (the packet less its seal, the tree's own path made the other's); none differs; 1,211 more exist only
    under the new seed because the old one had stopped. `form/form-stdlib/tests/fixtures/cell-run/cyc-*.bml` and the memos agree under both.
  - *Processes*: `host_processes("fkwu*")` sampled every 500 ms by the driving cell for the whole run: a child of the lowering process was
    seen under the old seed (1, standing the whole run) and never under the new (0 in about 12,000 samples a shard).
  - *The cache*: `.bml.fkb`, `.sym` and `.discovery.fkb` appear and are read back as before (same files after a cold run of a root `.bml`, the
    second run 0.03 s from the image).
- **What it found on the way.** `fk_cell_dep_find` asked a unit the load had just registered whether it was fresh; two `.bml` units that name
  each other read as changed (their lowered text is not their file), were set aside and collected again until the walker thread's stack
  ended: SIGBUS, rc 138, "Thread stack size exceeded" (crash reports of the sweep shards and of every cold shard of the first run). A unit this
  collection registered is no longer asked (`fk_cell_load_base`); `cell-run-band` claim 4194304 reads a cold load of such a pair in seconds
  (it took 68 s of mostly system time before). The same signature, `fk_src_collect_dep` and `fk_canon_id63` over a deep stack, was the
  sweep shards' SIGBUS.

## Services: what the host's manager starts and the body reads

A long-lived organ the operating system must isolate (the Metal render worker that can wedge in an uninterruptible GPU wait; the glass; the ear
lanes) is a SERVICE: the host's manager (launchd) starts it and starts it again; the body reads its progress as a typed value in the shared
field and ends it with `host_signal`; nothing is parsed from stdout, and the body never begins it. The first one is built.

**The voice-track worker.** `form/form-stdlib/voice-track-record.bml` is the contract: the worker (`observe/voice-track-worker-run.fk`, a loop that
waits for `.hearth/voice-track/worker.request`, renders that tongue under the machine-wide GPU lease, says "done" and waits again) gives one
record when a piece begins and when it lands, into a gift frame (`form-glass-gift-frame.bml`: a seqlock, never torn, released without unlinking)
named from the track directory and the tongue,

```
{"schema":"voice-track-record-v1","pid":N,"seq":N,"stage":"render","done":N,"total":N,"begun_ms":N,"beat_ms":N,"state":"working"}
```

(`state`: `working` a request is being rendered, `idle`, `done`, `refused` the voice would not open). The supervisor (`voice-track.bml`:
`vtr-supervise`, run by `observe/voice-track-run.fk`) writes the request, then LOOKS: it reads the record, asks `host_alive` after its pid, and
`vtq-judge` answers a finding as data: `["absent"]` (the service is not there), `["settled" ...]`, `["refused" ...]`, `["dead" pid age]` (the
record outlives its writer: the manager's turn), `["stalled" pid age]` (alive, `working`, no beat for `VTRIdleMs`) or `["live" ...]`. A stalled
worker is ended with `host_signal` (SIGTERM, a 5 s grace in 250 ms naps, SIGKILL; `vtq-end`, which is handed the signal door, the aliveness
question and the nap as functions so a band decides without ending anything) and is **not** waited for: its parent is the manager, which starts
the next worker, met as a new pid with a fresh beat. A worker waiting on the GPU lease has given no `working` beat, so it is never judged
stalled for waiting its turn. The files under `.hearth/voice-track/pcm-<tongue>/` stay the durable result and the end condition. `voice-track.bml`
contains no `host_spawn*`, `host_capture`, `host-exec` or `host_wait`; the membrane census reads 0 sites for it.

**Operator.** The service is data: `docs/launchd/earth.hati.voice-track-worker.plist` (label `earth.hati.voice-track-worker`; `./fkwu
observe/voice-track-worker-run.fk` in the checkout it names; `KeepAlive`, `RunAtLoad`, a 15 s throttle, logs under
`~/Library/Logs/CoherenceSense/`). The operator, not the body, runs
`launchctl bootstrap gui/$(id -u) docs/launchd/earth.hati.voice-track-worker.plist` once, and `launchctl kickstart -k gui/$(id -u)/earth.hati.voice-track-worker`
to restart it by hand (`launchctl bootout gui/$(id -u)/earth.hati.voice-track-worker` takes it away). With no service loaded the supervisor says
"service absent" after `VTRTries` (30) looks of 10 s and answers -1; it does not start one. Proof: `form/form-stdlib/tests/voice-track-service-band.fk`
(16383: an in-process writer and reader, a writer cell run by `cell_run`, the stall decision with the signal door injected, the absent answer as
data, the source read for any process begun, the plist read as data, no child standing) and `form/form-stdlib/tests/voice-track-reap-band.fk` (7: the one
band that begins synthetic workers on purpose, to see the escalation to SIGKILL end a real process and its exit read 137).

**The same pattern, the other organs (2026-10-04): what each would change, and why they are not done.**

| organ | what spawns it today | what it would change | why it waits |
| --- | --- | --- | --- |
| the glass supervisor (`observe/form-glass-supervisor-run.fk`) | `host_spawn_quiet` of the three frame processes and the ear, `host_spawn` + `host_wait` of four admission steps and of the live runner | the four admission steps become `cell_run` calls (their result is an exit code today: `stopped`/`errors`, class B); the three frame processes (sensors, machine, organs) become three plists whose frames the glass already reads by gift seq and age (the record is native to them); the supervisor keeps only the live runner and the restart decision | the live runner (`observe/form-glass-live-run.fk`) owns the viewing TTY: it must inherit it, and a cell's stdout is a sink; an interactive foreground child is the one spawn that stays, so the supervisor cannot be emptied, only thinned. Four plists and the TTY handover are a design decision for Urs, not a small step |
| the ear lanes (`observe/ear-glass-live.fk`: `ear-live-native.fk`, `ear-tongue-native.fk`) | two `host_spawn_at` with their first stdin lines in files and one log each | two plists; the lanes' first stdin lines become request files; the door keeps draining the spool by watermark (already data) and reads each lane's presence from the spool frames' age, as the track supervisor reads a beat; the existing `.stop` file is the end signal | the microphone lane needs the grant the `Sema Ear.app` bundle holds (the TCC principal is whoever launched it: `open -n`), which a launchd job running `fkwu` does not carry; the lane must be a launch agent of the app. That is the operator's to arrange (it is Urs's microphone), and the file belongs to the ear helper |
| the native LoRA workers (`observe/native-lora-*`) | the supervisor `host_spawn_at`s the trainer | one plist, one record (step, loss, beat), the same `vtq-judge` shape with a different state vocabulary | not touched: a training run is hours long and was running while this was built; the pattern is ready for it |

## The spawn sites, classified (2026-10-04)

Static call sites of product Form that run the body's own runtime and read back stdout, stderr or the exit (a survey by `git grep`
and reading; the membrane census counts the same crossings by word and wrapper, 94 sites in 36 files with tests and fixtures in its
reading). Counts move as migrations land; the census is the live number.

| class | what it is | product sites (files) | in tests |
| --- | --- | --- | --- |
| A | call for a value (a door, cell or band whose answer is a number or record) | 31 (24) | 27 asserting a door's answer |
| B | a band or gate leg run for its verdict | 26 (18) | in the same 27 |
| C | a supervised long-lived organ (glass, ear lanes, sessions; the track worker and the seed's floor child are done) | 10 (8) | n/a |
| D | inherently multi-process (the child is the thing under test) | n/a | 23 (19) |
| E | foreign program, not fkwu (git about 109, `cc`, ps/lsof/stat, file tools) | about 130 (50) | about 25 |

The largest users: the band sweep (410 workloads, up to 405 legs, each a child, and 2 gate children); `gate/drift-gates.bml` (15 rows,
each `sh -c '( ./fkwu ... ) 2>&1; echo @rc=$?'`, and up to 10 `--closure` children); `gate/canonical-conformance.bml` (14 children);
the glass supervisor chain (about 12-14 processes standing); `form/form-stdlib/bml/form-cli-heal.bml` (8 per healing attempt);
`observe/preflight.fk` (1-2 compile children and 2 probes per unresolved name); the local-plan, native-turn and code-circle legs (one
band child per candidate or gap); `form/form-stdlib/land-cadence-live.fk` (4 lens children a breath); the host walk (a movement, the
gates, a turn, a plan, a review, two drafts); `form/form-stdlib/bml/findings-requests.bml` (one band child per moot finding).

What a parent parses today, by mechanism: `hch-run*` (the whole of stdout and stderr in one file, the exit from `host_wait`, last-line
helpers `hch-tail-line` and `hch-last`); `host-exec("( cmd ) 2>&1; echo @rc=$?")` (the `@rc=` mark, the last line, the last integer:
drift gates, lens-text); `form-cli-heal` (`chain         clean`, `form-organ health ` lines); the code-circle, local-plan and native-turn
legs (exit 0, the last line as an integer, stderr health rows with `"organ":"live"` and `"health":0`); the band sweep (the exit, the last
line, stderr lines holding `unresolved-call`, `error:` or `compiled with errors`, the head pin and the registered verdict).

## Migration, in order

Each step moves one user, proves it with a band, and deletes the spawn path it replaced in the same movement.

1. **Band sweep** (largest user), landed: a leg is `cell_run(bsw-unit(w), 0, ceiling_s * 1000, nothing())`, judged by `bsw-why` from the
   record, shardable; `gate/band-sweep-run.bml` is the door and `form/form-stdlib/bml/band-sweep.bml` the organ.
2. **Drift gates**, landed: `dg-read(row)` is `cell_run` of the row's unit with the row's input as its stdin (a row is `[name, unit, input,
   want]`; `@scratch@` in an input line is a fresh temp path, removed with what the cell left beside it); the reading `[row, rc, last, value]`
   is taken from the record. `dg-closure` is read in Form (`bml-source-dependencies`, `bml-source-resolve`, the operator floor of a
   .bml program and the home index) and equals `./fkwu --closure` on every live row, so no `unit_closure` door is needed. The `cc` that
   rebuilds the seed is begun by argv and `fs_rename` puts it over the binary.
3. **Canonical conformance**, landed: each canonical expression is a unit `cell_run` answers (`cvg-run-expr`); `lx-sh-out` is gone.
4. **Band pin and the band legs of findings-requests**, landed: `bpn-claimed` and `frq-run-band` take the band's `value` from the call
   (`observe/tests/band-pin-band.bml`, `form/form-stdlib/tests/findings-requests-band.bml`). band-truth's `git ls-files` stays (git).
5. **The local-plan, native-turn and code-circle legs**, landed (`flp-band-run`, `nt-band-run`, `fccc-cell-run`): a band reads a number only
   when the call ran to its end with no compile error and ended on an integer (`value`); its budget is the call's deadline; what it said
   (`cs-stderr`) is written beside it for the readers of the compiler's words and the live claims. The native code lane of a turn
   (`nt-code`, a model session that holds the Metal client) stays a program ended by pid: class C, a service. Bands:
   `form-cli-local-plan-band`, `form-cli-code-circle-band`, `native-turn-ladder-band`.
6. **The host walk**, landed (`hw-turn-run-of`, `hw-plan-run-of`, `hw-review`, `hw-gates`, `hw-movement`; the landing's gates; the
   reunion's redraw): `hch-cell-in` answers the `[status, text, why]` `hch-run` answers and the record after them, and keeps the step's
   `.in/.out/.err/.exit` as the evidence a call writes, not as the channel. A redraw that is an argv (a stub, another checkout) is
   still run by `hch-run-in` with `env -C`; a cell redraws only the checkout the process stands in. **form-cli-heal** (`fh-run`, `fhn-run`)
   is the heal lane's and stays designed. Bands: `form-cli-heal-*`, `host-walk-band`.
7. **Preflight**, landed: the probe is the compile-only door (`cell_run(unit, 0, "check", nothing())`), the fresh run a plain call; the
   page is the text `pf-cell` puts the record into (`cs-stderr`, the tally, what the unit printed, its value) and is byte-identical to the
   spawned page. The lenses are cells whose value is the reading (`noe-check-cell`).
8. **Services** (glass, ear lanes, track worker, LoRA workers): they stay processes; their spawn moves to the manager and their
   progress to the typed record in the field. *Landed for the track worker* (above); the glass supervisor and the ear lanes are in the
   table above with what each would change; the LoRA workers follow the same record. `voice-track-reap-band` kills by `host_signal` on a pid
   and reads its exit as the parent of its synthetic workers.
9. **The seed's floor child**, landed (above), and the sibling doors the cell door lets us retire (`host_capture` of an `./fkwu` argv, `host-exec` of a
   cell). When nothing in the tree names `./fkwu` as an argv outside the exceptions, the spawn guard can refuse it by name.

## The patch

The seed change is 1,100 lines against `runtime/fkwu-uni.c` and `runtime/fkwu-optable.h`, carrying its own shrink path (the program loader
moves to Form: the table of loaded units is Form's and the door becomes a call into it). It follows the `host_signal` precedent: a
leaf-door mode (50; 35-49 are the sibling helper's filesystem doors), a rewrite row in `form/form-stdlib/bml/native-rewrite-rules.bml`
(`["cell_run", 4, [[1, 50], [0, 0], [0, 1], [0, 2], [0, 3], [2, 19, 2], [2, 19, 2], [2, 201, 3]]]`, last in the list), the matching row in
`runtime/fkwu-optable.h` (`{ "cell_run", 4, 19, { 1,50,0,0,0,1,0,2,0,3,2,19,2,2,19,2,2,201,3,} }`, last in `fk_rwtab`) and the arity
`["cell_run", 4]` in `form/form-stdlib/fkwu-op-arity.fk` (generated by `gate/reserved-heads.bml`; regenerate, or append the row). The AST
tag space is full, so the door rides tag 201 as every leaf door does.

Where the cost in the seed is, honestly: the sinks are macros over `putchar`, `printf`, `dprintf` and `vdprintf` (about 60 lines), the
diagnostic rows are four hooks (about 150), the name window is a filter in two lookups and the unit collector's closure tracking
(about 250), and the door itself with residency, freshness, the guard and the record (about 600). Sibling kernels (Go, Rust, TS) carry no
host doors, so the sibling validation lane does not apply.

Known limits, in the order they would bite: the record table has no collector (about 200-650 bytes a call); a deadline cannot end a
crystallized hot leaf or a blocking host call; a lowered `.bml` is fresh by its own mtime (a change only in the grammar it was lowered
with is met by the memo's own observations, not here); library compile diagnostics replay with the closure that holds them, per path,
and a library's load-time output belongs to the first call; the door is POSIX only (the Windows seed carries stubs and answers nothing).
A resident call's compile rows are in `diag` and not in `err` (the bytes of that call); a reader that wants the lines puts `diag` into words
once (`cs-diag-text`). Editing a unit that many units import (here `host-child.bml` and `cell-said.bml`) leaves every importer's lowering
memo stale, and the first process to load them lowers them all in its own process: observed 2026-10-04, a band's first run after such an
edit took 9 to 38 minutes under load and ended four times in SIGBUS (exit 138) after about 1.2 GB resident (a band, twice; a cell
loading the importers one by one in check mode, once at `form-cli-local-plan.bml`; a band after a further edit, once), then ran in 10 to 80
seconds once the memos were written. Whether the in-process floor lowering should hold a unit's lowering to a bounded arena (or write
each memo as it goes and answer) is the seed's to decide; until then, warm a widely imported unit's importers before a gate reads them.
