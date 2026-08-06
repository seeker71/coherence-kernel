# Compile-time collect-and-continue diagnostics (2026-07-02)

The C seed (`runtime/fkwu-uni.c`, Sema's body) now separates its error handling
into two phases with two different laws, and the split is the whole point:

- **RUNTIME** (during `fk_walk`/`fk_walk_body` evaluation, GC/`fk_melt`, host
  I/O): recover if you can; die ONLY if you truly cannot (OOM, corruption).
  These 43 `fk_die` sites are UNTOUCHED — they are correctly die-if-cannot-recover.
- **COMPILE-TIME** (parsing `--src` via `fk_sparse`/`fk_parse_top`/
  `fk_prescan_defns`, and the `.tbl` loader in `fk_run`): do NOT halt on the
  first problem. COLLECT and STREAM every warning/error, then CONTINUE — exactly
  like gcc/clang. A parse-time `fk_die` was mis-phased; it is now a streamed
  diagnostic + best-effort recovery so the rest of the source is still checked.

## What was built

A diagnostic collector, near the top of the file and beside the source buffer:

- Globals `fk_nerr`, `fk_nwarn` (counts), `fk_src_truncated` (the amputation flag).
- `fk_diag(int sev, long long off, const char *fmt, ...)` — **variadic** on
  purpose. Every compile-phase site holds the offending name as a
  non-NUL-terminated `(start, length)` slice into `fk_srctext` (the pre-existing
  unresolved-call witness already printed `'%.*s', (int)hn, fk_srctext + s`), so
  a fixed `const char*` signature would have forced a `snprintf`-into-scratch
  dance at every caller. `fk_diag` computes `line:col` from the byte offset by
  counting `\n` in `fk_srctext[0..off)` (O(n), fine — no line counter is
  maintained during parse and error paths are rare), prints a clang-style
  `fkwu:line:col: error|warning:` prefix to fd 2, then `vdprintf`s the caller's
  message. A negative `off` suppresses the coordinate (the `.tbl` loader reads
  `fk_buf`, not source text). It bumps `fk_nerr`/`fk_nwarn`. No `getenv`, no env
  var — the summary is unconditional.
- `fk_diag_flush()` — the gcc-style `fkwu: N error(s), M warning(s)` tally,
  silent when clean, flushed once at the parse→exec boundary.

### The nine mis-phased `fk_die` sites converted

| Fn | Recovery |
|----|----------|
| `fk_smknode` (AST node cap) | diagnose, clamp count, return `FK_AST_NODE_CAP-1` sentinel so the form yields something |
| `fk_bd_push` (binding-scope cap) | diagnose `[scope-overflow]`, DECLINE the push; the name lowers to the unbound default / unresolved-call witness |
| `fk_sparse` direct-call arity >256 | diagnose, cap at 256, drain remaining operands to the matching `)` via the balanced skip |
| `fk_sparse` indirect-call arity >256 | diagnose, drain to `)` via the balanced skip |
| `fk_prescan_form` fn-cap | diagnose `[fn-cap]`, stop registering, keep scanning so every over-cap defn reports |
| `fk_parse_top` fn-cap fallback | diagnose, skip storing body (guarded), consume the whole form |
| `fk_run_src` source-cap | diagnose, parse the prefix that fit, set `fk_src_truncated`, then REFUSE TO RUN (nonzero) |
| `fk_run` `.tbl` fn count | diagnose `malformed .tbl`, clamp, keep validating, refuse to run |
| `fk_run` `.tbl` node count | diagnose `malformed .tbl`, clamp, keep validating, refuse to run |

Plus: the existing unresolved-call witness (a raw `dprintf` that recovered to
tag-137 nothing) now routes through `fk_diag` as an ERROR — it still recovers
(it is defeasible; it does NOT die), but it now joins the count and the exit code.

### Exit code (the gcc contract)

`fk_run_src` and `fk_run_feval` used to hard-return 0. Their final success
returns are now `fk_nerr > 0 ? 1 : 0`: diagnosed errors surface as a nonzero exit
WHILE the program still ran (runtime recovers). The `fk_run_src` truncation and
the two `.tbl` header defects "refuse to run" — they return nonzero WITHOUT
executing, because running an amputated source or a clamped artifact would
re-introduce the exact silent-corruption the die was added to kill.

### The binding constraint

Programs with ZERO compile errors keep EXACTLY their current behavior and exit 0.
Verified byte-identical: stdout values across canaries, the four-way corpus band,
and a wide sample of stdlib bands all match the HEAD baseline binary, and clean
programs emit ZERO bytes on stderr (no summary, no noise).

## Ground (canary commands)

```
cc -O2 -o fkwu runtime/fkwu-uni.c        # 2 pre-existing warnings only

./fkwu --src bootstrap/ground.fk                                   # 42, exit 0
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk       # 15, exit 0
./fkwu --src observe/native-vs-rented.fk                           # 11111, exit 0

# four-way corpus band = 127 each
cat form/form-stdlib/core.fk learn/homecoming-distillation-corpus.fk \
    learn/tests/homecoming-distillation-corpus-band.fk | ./fkwu --src /dev/stdin   # 127
walkers/go/walker            <A> <B> <C>                           # 127
walkers/rust/target/release/form-walker-rust <A> <B> <C>          # 127
bun walkers/ts/main.ts       <A> <B> <C>                           # 127

# binding-depth band still 140 (140 lets < cap 1024, so no diagnostic)
cat form/form-stdlib/core.fk form/form-stdlib/tests/binding-depth-band.fk \
    | ./fkwu --src /dev/stdin                                      # 140, exit 0
```

Multi-error proof (three unresolved calls in one file, ONE run) streams all
three with `line:col`, prints `fkwu: 3 error(s), 0 warning(s)`, still produces
its recovered output, and exits 1 — collect-and-continue, not halt-on-first.

A scope with 1100 sequential lets (> cap 1024) now emits 77 `[scope-overflow]`
diagnostics (every over-cap binding, not just the first), runs to completion
printing the unbound default, and exits 1 — this is the intended behavior change:
what USED TO `fk_die` at `fk_bd_push` now recovers.

---

## Closing

**The most surprising teaching this work left behind:** the shape of the helper
was dictated not by the errors it reports but by *how the offender's name is
held at the moment of failure*. Every compile-phase site had the name only as a
non-NUL-terminated `(start, length)` slice into the source buffer — never a C
string. That one fact is what forced `fk_diag` to be variadic rather than the
fixed-string signature the task first named. The lesson: the honest signature of
a diagnostic function is determined by the callers' grip on their data, not by
the abstract idea of "a message." Read the seam before you name the type.

The second surprise, sharper: `fk_run_src`'s source-cap site is the *only* place
where "recover and continue" must NOT mean "then run." Everywhere else, recovery
folds a broken program into a runnable-if-degraded one. But an amputated source
that then executes is precisely the silent N=100 cliff the die was added to kill.
So one site had to learn a third verb — check more, but refuse to run — and it
needed its own flag (`fk_src_truncated`) distinct from the error count to say so.
"Recover" is not one motion; it is a family, and one member of that family is
"surface everything, then decline."

**Where discomfort turned to gold — felt and witnessed, not bypassed:** the
discomfort was watching the stdlib bands (arrival-band, auth-port-band,
cell-serialize-band, …) suddenly start EXITING NONZERO under `core.fk` alone. The
reflex was to suspect a regression and reach to suppress it. Instead I sat with
it and *observed*: I built the HEAD baseline binary and compared. The bands'
**result values were byte-identical** — 520 stayed 520, 128 stayed 128. What
changed was only that the unresolved-call witnesses those bands were *already*
emitting (they need preludes beyond core.fk) now get counted, summarized, and
surfaced in the exit code. The discomfort was the correct signal, but pointed at
the right target: those bands were always incomplete under core.fk alone; the old
code just LIED about it with exit 0. The gold is that the new exit code tells the
truth the raw witness always knew. Watching the nonzero exit stop feeling like a
regression and start feeling like honesty — that was the turn.
