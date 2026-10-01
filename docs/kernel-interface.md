# The kernel interface

One interface every kernel carries — fkwu (the only runtime), and the Go, Rust and
TypeScript proof siblings — and nothing outside it. A name a kernel carries off the
interface is released; a meaning that differs between kernels is a defect in exactly
one place. This page says what stands and where the interface is going; history lives
in git.

## Source: one reader, one lowering

**Stands.** fkwu resolves a unit's closure — `; preludes:`, `// preludes:`, `import`
in every spelling, `home-index.txt` names — and lowers every `.bml` and section-bearing
`.fk` through its own memo'd floor (`bml-floor-compile.fk`, `<unit>.lowfk`).
`./fkwu --closure <unit> <out>`, run from the repo root, writes that closure as one
plain-Form file: each unit's text in dependency order under a `; unit: <path>` line.

A sibling reads the plain Form files named on argv, in order, joins them with one
newline, walks the result and prints its value. It follows no directive, opens no
index, lowers nothing and keeps no source cache. `validate.sh` hands the siblings the
closure fkwu walks, so four arms agreeing means four arms agreed over one input.

## Pure core

Native on every kernel, one meaning, generated from one table (the op table that
generates `runtime/fkwu-optable.h` and the reserved-head lists):

| family | names |
|---|---|
| numbers | `add sub mul div mod le lt eq` (`gt ge ne not and or` are reader rewrites) |
| floats | `float_to_int str_to_float math_sqrt make_float32 make_float64 float_value intern_trivial_float` |
| equality | `value_eq str_eq` |
| bits | `band bor bxor shl_u32 shr_u32` |
| strings | `str_len str_byte_at byte_to_str str_concat` |
| lists | `cons head tail nth len list empty` |
| absence, control | `nothing nothing? attempt form_error` |
| kind | `value_kind` |
| records | `record_new record_get record_set record_has record_keys record? record_blueprint` |
| nodes | `make_nodeid intern_trivial_int intern_trivial_string intern_trivial_bool intern_node node_children node_value node_category node_type node_inst node_pkg node_level` |

A measured native (`substring str_find scan_run value_str round_ndigits rotr_u32
add_u32 bnot_u32 method_define method_has method_invoke recipe_to_bytes
bytes_to_recipe`) stays native until a warm BML home matches it; then it becomes one
`home-index.txt` row. Everything else lives in a Form or BML home that fkwu links by
name and hands the siblings inside the closure. A sibling native that shadows a home
(`char_at ord int_to_str str_to_int sum abs range intern_node_at …`) is released,
so every kernel runs the body's own meaning. `bp` is the one shadow still native on
Go, Rust and TS (law 10).

### Laws the core pins

1. Integers are 63-bit two's complement, `[-2^62, 2^62)`, on every kernel.
2. Integer `div`/`mod` by zero stops; `attempt` recovers it to `nothing`.
3. An order refusal (`lt` on a non-number) stops; `attempt` recovers it.
4. `math_sqrt` is correctly rounded (IEEE `fsqrt`): the walker, the JIT and the
   siblings agree to the bit.
5. Float `mod` truncates, like integer `mod`.
6. `float_to_int` on NaN, out-of-range or a non-number stops.
7. `str_to_float` reads one grammar: leading whitespace, the longest decimal prefix,
   no hex, no `inf`/`nan`.
8. `str_byte_at` out of range answers `-1`; `str_find` on a non-string stops.
9. A value renders one way everywhere (`value_str`), records included.
10. `bp` has one meaning: the BML resolution in `form-ontology-bp.fk`.

One prelude-free probe, each row under `attempt`, read on all four arms (2026-10-01):

| law | Go, Rust, TS | fkwu |
|---|---|---|
| 1, 3, 4 | hold | holds |
| 2 | holds | holds cold; once the JIT has taken the defn (20000 calls), `div` by zero answers 0 |
| 5 | holds | `mod 1e19 3.0` answers -512, `mod 7.5 0.0` answers 7.5 (siblings: 1, NaN) |
| 6 | holds | NaN answers 0, `1e300` answers -1, `"x"` answers a raw word |
| 7 | holds | `"0x10"` reads 16, `"inf"` Infinity, `"nan"` NaN |
| 8 | holds | `str_byte_at` holds; `str_find` on an int answers -1 |
| 9 | `value_str` holds; `print` of a record: Go `<record @0 #0fields>`, Rust and TS `<record>` | `value_str` holds; `print` of a record answers its raw word |
| 10 | a native `bp` answers a NodeID, and an unknown name stops | tag 45 answers its own argument, a string |

On every arm `print` renders a `nothing` inside a list as `nothing`, and `value_str` renders it as
`null`. No arm runs the BML `bp` in `form-ontology-bp.fk`, so law 10 holds nowhere. No band pins laws
5–10 yet; where the kernels are going is one four-way law band that does.

## Host doors

One BML table of host doors replaces the five mirrors that disagree today
(`native-op-manifest.fk`, `host-effect-grammar.fk`, `bml/host-io.bml`,
`primitive-registry.fk`, the TS `KernelHost` field list). Each row names the verb, its
family, arity, direction, the handle it opens or closes, whether it may block, and its
refusal words. The fkwu op rows, the siblings' registration check and the live-page
row index are read from it.

| family | verbs |
|---|---|
| file | `file_read(path, off, len)` · `file_write(path, bytes)` · `file_append(path, bytes)` |
| path | `path_stat(p)` → `(kind size mtime-ms)` · `path_list(dir)` · `path_make(p, "dir"\|"fifo")` · `path_remove(p, "one"\|"tree")` · `path_rename(a, b)` · `host_where("cwd"\|"temp"\|"root"\|"home")` |
| process | `proc_spawn(argv, redirects)` · `proc_run(argv, stdin)` → `(status stdout)` · `proc_wait(pid, deadline-ms)` · `proc_signal(pid, sig)` · `proc_info(pid\|pattern)` · `host_pid` |
| stream | `stream_open(kind, spec)` · `stream_read(h, max, deadline-ms)` · `stream_write(h, bytes)` · `stream_close(h)` · `stream_info(h)` — kinds `stdin stdout stderr tcp-listen tcp-accept tcp-connect tls-connect fifo mic speaker camera terminal` |
| clock | `clock_now("wall-ms"\|"mono-ms"\|"mono-ns"\|"cpu-us")` · `await(deadline-ms, handles)` — the one place the body blocks |
| shared | `shared_map(name, bytes, "offer"\|"receive")` · `shared_words` · `shared_read` · `shared_write` · `shared_seq` · `shared_unmap` · `shared_unlink` |
| gpu | the Metal handle doors (`metal_pipeline buf_alloc buf_from_file buf_write enqueue sync buf_read status batch_concurrent buf_free`) |
| device | `device_list(kind)` · `host_metrics(kind)` |
| entropy | `entropy_bytes(n)` |
| self | `fb_record framebuffer-events framebuffer-clear node_source kernel_stat jit_leaf_inram` |

`tcp-listen` binds loopback unless its spec names an address; the public plugin door
names `0.0.0.0` because its container is reached over the Docker network.

**Failure.** A door answers its value or `nothing`. The reason is a word
(`no-such-path exists refused too-long deadline argv fork exec unavailable too-many`)
the caller reads with `host_why(verb)`. No negative codes, no `""` or `[]` standing
for absence, no `0` standing for refusal, and no door that ends the process: callers
backtrack with `??`, `attempt` or `oac-choice`.

**Handles.** One table per kernel; a handle is an index and a generation, so a stale
handle answers `nothing` and can never reach another resource. Close is idempotent; a
BML `using h = stream_open(…) { … }` closes at scope end and on stop or backtrack.
Raw descriptors never cross into Form. Children are reaped by the kernel, and
`proc_wait` reads the status. At exit every open handle closes and counts as leaked.

## Vitality

**The ring.** Every kernel on the host appends its stage opens and closes to one
shared-memory ring, `/fg-bus1` (`runtime/fkwu-uni.c`, the stage bus: 65536 slots and an
interned name table). `float_leaf` 29 opens a stage and 30 closes it; 31 reads rows from
a cursor and 32 answers the text of an interned id. A row is 13 ints: `idx pid t_us
stage key phase depth link dur outcome amount body digest`. Phase 1 opens, 2 closes, 3
closes unwound and 4 says the stage died; a close's link is its open.

**The fold.** `form/form-stdlib/bml/stage-vitality.bml` folds the rows without calling a
door. Each stage key learns its own bound (its p95, then the stage's, from a log2
histogram), and the fold finds seven kinds: **overstay** (an open past its bound),
**stall**, **died**, **lockstep** (one stage and key open in several pids of one body),
**contention** (the same across bodies), **retread** (the same close again with nothing
differing between) and **loop** (a period repeated whole in one pid). Each finding's
stake, in microseconds of body time, accrues to its organ (the stage name's prefix
before `.`). An organ's wane is that stake per second over the window, and the highest
wane is the body's need.

**The door.** `./fkwu observe/fb-findings-run.bml </dev/null` prints the bus reading,
the ranked findings, the organs by wane and the coverage (pids that spoke / pids alive),
and answers `need=<organ> <wane>ms/s <finding> <stage> <key>`, or `need=none`.
`form/form-stdlib/tests/stage-vitality-band.bml` witnesses the fold and
`form/form-stdlib/tests/fb-bus-band.fk` the ring.

No flow speaks on the ring yet: outside `fb-bus-band.fk` nothing calls `float_leaf` 29
or 30, so the door reads silence as `need=none` (coverage 0/17 on 2026-10-01). The
glass view of the findings (`stv-glass-row`, `stv-rows`) has no caller.

`observe/body-vitals-live.fk` is a different watch: it folds each glass surface's
stamps into that surface's own rhythm and publishes the `vitals` frame.

## Order

1. Release kernel-side lowering; siblings read fkwu's closure. *(stands)*
2. fkwu defects that broke a law: `math_sqrt`, order and divide-by-zero stops,
   `kernel_stat`'s unknown key. *(stands)*
3. The stage ring, its phases, the vitality fold and its text door. *(stand)* Flows
   that speak on the ring and the glass view of the findings are open.
4. Release sibling natives that shadow homes or have no caller *(stands, except
   `bp`)*; one op table generates every name list.
5. The host door table; migrate callers family by family; release the off-table
   names. Every door call timed and counted per verb on the live page (calls,
   nothings, bytes in and out, open handles, a log2 *dwell* histogram of time spent in
   the world) is not built yet.
6. One four-way law band pinning laws 1–10; fkwu brought to law 2 on its JIT path
   and to laws 5–8, `print` to law 9 on fkwu and Go, and every arm to law 10.
