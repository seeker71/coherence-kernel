# The kernel interface

The interface fkwu, the only runtime, carries — and nothing outside it. A name fkwu
carries off the interface is released; a meaning that differs between two readings of
one name is a defect in exactly one place. This page says what stands and where the
interface is going; history lives in git.

## Audio ownership

The optional system carrier owns AudioQueue callbacks and lifetime. The C seed
only transports byte strings and the caller's remaining deadline; its shrink
path is the existing native carrier ABI. `host-sense-organ.fk` owns PCM statistics,
WAV validation, framing and reports. `float_leaf(59, milliseconds)` supplies a
private raw capture to that Form owner; zero milliseconds returns empty bytes
without opening input. A three-item `[pcm, rate, tail_ms]` packet supplies private
air loopback. It leaves a standing public input stream untouched.

Mic/speaker count, name and health describe the available default AudioQueue
endpoint, not microphone permission or a witnessed recording. Stream writes
return accepted samples; playback returns a completed sample count only after
observed queue startup, buffer return and stopped state. The caller's deadline
covers admission, copying and draining. Disposal precedes context release;
incomplete release retains its owner for an explicit stop/retry. No policy cap
silently shortens the requested duration or replaces a larger PCM buffer.

The current verification uses silent invalid-input, closed-stream and synthetic
PCM/WAV observations. It does not establish microphone or speaker hardware
behavior on this host.

## Source: one reader, one lowering

The Form `native-posix.bml` owner also carries positioned file reads/writes,
size, resize and durable sync. Its transient file lease uses
`float_leaf(60,["open",path,writing])` to admit an owned private regular file and
lock it; the returned `[handle, descriptor]` belongs to the active cell.
`["close",handle]` releases it and `["held",handle]` reads that ownership.
`["identity",handle]` reads the admitted descriptor's device/inode, and
`["status",handle]` retains ownership and the close error separately. Admission
is nonblocking before regular-file validation, so a FIFO cannot defeat the
cell deadline. Linux close errors retire the descriptor without retrying a
number that the OS can already have assigned to another owner.
`["stamp",handle]` includes its extent and host modification/change times.
A lock that would wait for this process's own held lease refuses with its
owner intact; it cannot self-wait indefinitely. Foreign lock contention still
honors the caller's deadline.
`nps-with-file` uses native `defer` for ordinary failure, and evaluator unwind
releases a lease when a deadline bypasses Form recovery. This physical hook does
not establish shared node identity by itself. Its shrink path is the native
walker carrying the same resource ownership; storage framing and publication
remain Form work.

The versioned `bml/native-field.bml` journal owns canonical row lookup, monotone
row allocation, framing, checksums and publication through that lease. Existing
rows are reused before allocating another identity. Cold and invalidated readers
recover under shared leases. A replaced journal releases the exclusive lease
before rebuilding its index; no callback effects are repeated. Execution
reacquires an exclusive writable lease, rechecks its
physical identity and observed prefix, and syncs recovered bytes before exposing
IDs. This includes a recovered header with no frames. Speculative indexes are
inadmissible after failure or a deadline, while their verified byte checkpoint
survives repeated refusals and failed readback. Missing currentness stamps refuse.
When host change metadata differs, the complete cached prefix is fingerprinted
again under the lease. Replay decoding stays inert; it cannot admit coordinates.
Incomplete, changed or incompatible retained frames
refuse explicitly. Public constructors, typed binary admission, full-coordinate
lookup and cell-map operations enter this Form owner. Attached immutable views
retain payloads and dotted children; adoption does not count as a mint. Map
capabilities are weakly tracked by native GC so an interrupted caller's lost
result can be reclaimed. The bootstrap handoff and physical view/handle carrier
shrink into the native walker; canonical identity stays in Form.

The v7 process page uses `/fg7-k<pid>` and a versioned atomic snapshot header.
Its logical word count is checked independently of OS page padding. Public
words28–30 identify the field version/device/inode;22 reports the shared owner.
Exit retains an ended snapshot. Form journal rows own roster membership, and
foreign map admission checks both page identity and the host process birth.
This wire does not reinterpret legacy live pages. In this version, counters5/6
and live words7/8 report allocated local text/list objects (including objects
not yet collected), not legacy table high-water marks or native text nodes.
Counter69/live word21 reports completed GC epochs.

`field_reset` starts a new permanent interning epoch under the journal lease.
It requires the current owner to stand alone among all retained process
identities, including forgotten owners whose death is unproven. It returns1
only after commit/readback, and absence when exclusive ownership is unproven.
It preserves the inode, monotone IDs, fixed coordinates, historical catalog,
attached views, source attribution and gift graph. New string/float/composite
interning belongs to the new epoch. This is not physical storage reclamation.

Owned gift generations and byte frames use the same Form journal. A fixed64-byte
physical descriptor names a committed generation; payload capacity comes from
the actual reservation and the caller's allowance. Reopening with a larger
allowance does not change the frame inode or invalidate existing readers.
Readonly handles cannot publish. Reader pins, current publications, promotions
and their transitive dependencies retain generations. Unavailable owner or frame
observations cannot prove reclamation. An interrupted odd publication is restored
only after matching frame identity and proving the recorded writer ended.
Held values carry GC-traced provenance through children and list/record views;
permanent promotion retains the owned identity rather than flattening it away.

The native POSIX fork uses libc's child bookkeeping. Active file leases or held
gift views refuse with EBUSY until released or explicitly handed off; they never
silently become independently owned copies. Form retains continuation policy.
General regular-file handles share evaluator unwind ownership with journal
leases, without imposing private-journal permissions or locks. Form streams
copy and positioned writes, checks descriptor identity before truncation, keeps
permissions, and verifies release. The C carrier supplies descriptor admission
and host timezone conversion; it has no duplicate copy loop.

Operation reflection retains public tag/arity and appends the actual delegated
Form source owner. Surface checks verify that owner against its declared home.
Binding validation consults indexed names and positive operation observations
before lexical lookup; a negative operation observation still permits a local
binding, and missing names retain their original source diagnostic.

**Stands.** fkwu resolves a unit's closure — `; preludes:`, `// preludes:`, `import`
in every spelling, `home-index.txt` names — and lowers every `.bml` and section-bearing
`.fk` through its own memo'd floor (`bml-floor-compile.fk`, `<unit>.lowfk`).
`./fkwu --closure <unit> <out>`, run from the repo root, writes that closure as one
plain-Form file: each unit's text in dependency order under a `; unit: <path>` line;
the drift gates read it to learn which units a gate loads.

## Pure core

Native in fkwu, one meaning, generated from one table (the op table that
generates `runtime/fkwu-optable.h` and the reserved-head list,
`form/form-stdlib/fkwu-op-arity.fk`):

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
name. A native that shadows a home (`char_at ord int_to_str str_to_int sum abs range
intern_node_at …`) is released, so fkwu runs the body's own meaning. `bp` links its
reviewed Form resolution through that same home index.

### Laws the core pins

1. Integers are 63-bit two's complement, `[-2^62, 2^62)`, and in a
   defn the JIT has taken as in one it has not.
2. Integer `div`/`mod` by zero stops; `attempt` recovers it to `nothing`.
3. An order refusal (`lt` on a non-number) stops; `attempt` recovers it.
4. `math_sqrt` is correctly rounded (IEEE `fsqrt`): the walker and the JIT agree to
   the bit.
5. Float `mod` truncates, like integer `mod`.
6. `float_to_int` on NaN, out-of-range or a non-number stops.
7. `str_to_float` reads one grammar: leading whitespace, the longest decimal prefix,
   no hex, no `inf`/`nan`.
8. `str_byte_at` out of range answers `-1`; `str_find` on a non-string stops; an
   index that is not an int stops (`str_find`, `str_byte_at`, `substring`, `nth`).
9. A value renders one way everywhere (`value_str`), records included.
10. `bp` has one meaning: the BML resolution in `form-ontology-bp.fk`.

`form/form-stdlib/tests/kernel-laws-band.fk` asks laws 1–9 of fkwu, each row
under `attempt`, prelude-free; its head names every bit. Read with
`./fkwu form/form-stdlib/tests/kernel-laws-band.fk` (`2147483647`). The drift gates'
`kernel-laws` row (`gate/drift-gates.bml`) reads fkwu's answer at every landing that
moves a kernel:

| law | stands | band bits |
|---|---|---|
| 1 | on fkwu cold and hot, since the JIT's int lane wraps every add, sub, mul, div and mod to 63 bits | 1, 2, 134217728 |
| 2 | on fkwu cold and hot | 4, 8 |
| 3, 4, 5 | pinned | 16; 32; 64, 128, 256 |
| 6 | on fkwu cold and hot, over ints and over floats | 512–4096, 33554432 |
| 7 | on the band's rows | 8192–65536 |
| 8 | pinned | 131072, 262144, 524288 |
| 9 | `value_str` and `print` give `<record>` for a record and `<closure>` for a closure | 1073741824 |
| 10 | Form-owned: `bp` resolves a reviewed NodeID and stops on an unknown name; category-name string literals remain distinct | `form-bml-cursor-full-band.fk`, 105 |

The band also pins `fs_list` and `host_dir_list` (a path that is no directory answers
`nothing`, an empty directory `[]`), `record_has` of a record, `record_get` and
`record_set` stopping on a value that is no record, `read_file_slice` at a negative
offset answering `nothing`, and `make_nodeid` stopping outside its layout.

Rows the band does not pin stay out of the verdict, and the band's head names each one:
`record_has` of a value that is no record; a 1.1.1 int past int32; `write_file_bytes`
of something that is no byte list; `round_ndigits` with ndigits below 0; `str_to_float`
of `"5.e3"`; `math_pow` of -1 to an infinity; `cons` onto a non-list; and `math_exp`,
`math_log` and `math_pow` off their exact cases, where the last bit is the libm's.

`print` renders a `nothing` inside a list as `nothing`, and `value_str` renders it as
`null`.

## Host doors

One BML table of host doors replaces the mirrors that disagree today
(`native-op-manifest.fk`, `host-effect-grammar.fk`, `bml/host-io.bml`). Each row names
the verb, its family, arity, direction, the handle it opens or closes, whether it may
block, and its refusal words. The fkwu op rows and the live-page row index are read
from it.

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
backtrack with `??`, `attempt` or `oac-choice`. Today's `fs_list` and `host_dir_list`
keep this: a path that is no directory answers `nothing`, an empty
directory `[]`. A lister reads them as `fs_list(d) ?? []`, since a `nil?` walk handed
`nothing` never ends; the stdlib word `fs-list` (`form-fs.fk`) is that reading.

**Handles.** One table; a handle is an index and a generation, so a stale
handle answers `nothing` and can never reach another resource. Close is idempotent; a
BML `using h = stream_open(…) { … }` closes at scope end and on stop or backtrack.
Raw descriptors never cross into Form. Children are reaped by the kernel, and
`proc_wait` reads the status. At exit every open handle closes and counts as leaked.

## Vitality

**The ring.** Every fkwu on the host appends its stage opens and closes to one
shared-memory ring, `/fg-bus1` (`runtime/fkwu-uni.c`, the stage bus: 65536 slots and an
interned name table). `float_leaf` 29 opens a stage and 30 closes it; 31 reads rows from
a cursor and 32 answers the text of an interned id. A row is 13 ints: `idx pid t_us
stage key phase depth link dur outcome amount body digest`. Phase 1 opens, 2 closes, 3
closes unwound and 4 says the stage died; a close's link is its open.

Source initialization also speaks on this ring: `source-initialization` names
the matched source unit as its key and reports the form count on completion.
BML admission wrappers and their translated units retain their separate paths.
Synthetic binding/image calls and empty sources do not claim a unit entry;
interrupted entries unwind through the existing cell recovery or process exit.
This observation moves with initialization from the C bootstrap into the Form
source loader; it adds no execution allowance.

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

The coding lane is the first flow that speaks on the ring: `fcacs-stage-open` and
`fcacs-stage-close` (`form/form-stdlib/bml/form-cli-code-session.bml`, over `float_leaf`
29 and 30) open and close `code.prefill`, and `form-cli-code-swerve.bml` opens and
closes `code.decode`, `code.tool` and `code.check`, so the door lists `code` among its
organs once the lane has run. Coverage counts only the pids alive when the door reads,
and a lane between turns reads `need=none`. The glass view of the findings
(`stv-glass-row`, `stv-rows`) has no caller outside `stage-vitality.bml`.

`observe/body-vitals-live.fk` is a different watch: it folds each glass surface's
stamps into that surface's own rhythm and publishes the `vitals` frame.

## Owned Metal buffer operands

The existing Metal leaves keep their arities and scalar operands. An owned
allocation uses `metal_buf_alloc([bytes, owner, key])`; an owned file mapping
uses `metal_buf_from_file([path, owner, key], offset, bytes)`. The named record
field must already contain a proper list. A positive returned handle is attached
to that list inside the physical leaf, before another evaluator deadline poll.

`metal_buf_free([handle, owner, key])` requires that exact handle in the owner's
list. It removes the handle only when the carrier confirms release. A refusal
leaves the list unchanged; a different owner cannot release it. Form retains
the record before entering construction and owns retries, reservations and
retirement selections. The bridge adds no process-wide cleanup registry.

These operand adaptations are temporary seed glue beside the physical Metal
leaves. They leave with those leaves when their native walker carrier takes over;
allocation policy, model geometry and continuation meaning remain in Form.

## Bootstrap source location

Source resolution preserves the caller's local and ancestor choices first.
When those lack a library, it searches from the executing image's location so
a scratch working directory can use this body's Form home. Local dependencies
continue to override that fallback. Metal loading shares the executable-location
carrier. Linux builder identity still reads `/proc/self/exe` itself, preserving
the loaded image identity across pathname replacement.

This location carrier remains in the temporary C seed because the first home
index is needed before Form admission runs. It moves into Form with the initial
loader; it introduces no new public primitive or environment authority.

## Order

1. fkwu resolves and lowers every closure; no second reader stands. *(stands)*
2. fkwu defects that broke a law: `math_sqrt`, order and divide-by-zero stops,
   `kernel_stat`'s unknown key. *(stands)*
3. The stage ring, its phases, the vitality fold and its text door. *(stand)* The
   coding lane speaks on the ring *(stands)*; the other flows and the glass view of the
   findings are open.
4. Release natives that shadow homes or have no caller *(stands, except
   `bp`)*; one op table generates every name list.
5. The host door table; migrate callers family by family; release the off-table
   names. Every door call timed and counted per verb on the live page (calls,
   nothings, bytes in and out, open handles, a log2 *dwell* histogram of time spent in
   the world) is not built yet.
6. One law band, `kernel-laws-band.fk`, pinning laws 1–9, laws 1, 2 and 6
   hot as cold. *(stands)* Law 10 is open: `bp` runs the BML resolution. The rows the
   band's head names as not pinned each come to one meaning.
