# Receipt — remove the dead x86 self-JIT subsystem (revenant on arm64; walker is correctness)

**Status: observed.** The hand-written x86-64 self-JIT — a whole tree→bytes lowerer, its native-dispatch
cache, its heat gate, and its two opt-in entry paths (`--src` FK_JIT, `--feval` FK_JIT) — is gone from the C
seed. The build stays clean (only the 2 pre-existing warnings), and every canary is byte-identical to before.

**Why it was a revenant.** This body is arm64 (Apple Silicon). The emitter emitted x86-64 machine code, so
`fk_nat_install` / `fk_native_call` on the crystallized image could never execute here — `mmap`+`mprotect`
returned an exec page the CPU cannot run, install always failed, and *every* JIT path already bailed to
`fk_walk` / `fk_walk_body`. The receipts say it plainly: crystallize is speed; **correctness never needs it**.
So on this body the ~1670 lines did nothing but sit as dead weight and lie in wait — a path that, on the one
platform where it *could* fire, was superseded by the in-progress Form-native arm64 emitter
(`model/form-asm*`). The owner's law — "always leave the path cleaner than you found it" — pointed one way.

**What was removed** (traced before deletion; every definition, forward-decl, global, and call site):
- Emitter: `fk_jemit`, `fk_jbin`, `fk_jcarrier`, `fk_jb1/fk_jb4/fk_jb8`, the `fk_jb` code buffer + `fk_jbp`
  + `FK_JIT_CODE_BUF_CAP`, `fk_jit_ok`, `fk_jit_entry`, `fk_jit_frame`, `fk_jit_self`, `fk_lower_tail_tramp`,
  `fk_jpatch4`, `fk_jit_lower`; the carriers `fk_jprim1/2/3`, `fk_jlist1/2`, `fk_jtail_set`.
- Native dispatch/install: `fk_nat_install`, `fk_ensure_native`, `fk_ensure_native_ex`, `fk_jcall`,
  `fk_jtramp`, the `fk_tailcall`/`fk_tail_callee` sentinel, `fk_nat_exec[]`, `fk_nat_tried[]`,
  `fk_src_nat[]`/`fk_src_nat_len[]`/`fk_src_nat_frame[]`, the `fk_natfn` typedef.
- Heat path in `fk_run`: `fk_heat[]`, `fk_hot`, `fk_njit`, `fk_nat_code[]`/`fk_nat_len[]`, `fk_demo_inc[]`,
  the argv[4] hot-threshold JIT use, the argv[5] demo-native registration, the `fk_heat[0]++`/`fk_hot` gate,
  and the trailing `fk_pr(fk_njit)`. The `.tbl` root now runs `rootv = fk_walk(fk_fn[0], 0)` unconditionally.
- `--feval`: `fk_feval_try_native` and its three call sites (t==12 / t==240 / t==241 in `fk_walk_body`), plus
  the `fk_feval_jit_on` / `fk_feval_hot` / `fk_fheat[]` state and the `--feval` FK_JIT config block.
- `--src`: the whole `{ char *je = fk_conf("FK_JIT"); … crystallize … }` gate in `fk_run_src` and the
  `FK_JIT_SCAN` measurement loop. `fk_run_src` now falls straight to
  `fk_pv_root(fk_fn[0], fk_walk(fk_fn[0], 0)); return fk_nerr > 0 ? 1 : 0;`.
- Config keys: `FK_JIT` / `FK_JIT_HOT` / `FK_JIT_WITNESS` / `FK_JIT_SCAN` / `FK_JIT_SCAN_V` reads (now dead),
  and their listing in `fkwu.conf.example`.

**What was deliberately KEPT** (shared with a live, non-JIT path — kept it, removed only the JIT use):
- `fk_native_call` / `fk_native_call_test` / the `fk_demo_inc`-style witness bytes' *carrier* — the tiny HAL
  install-and-call door, opcode 215, same family as the socket/camera/dlopen carriers. It is reached by the
  live walker (`native-vs-rented`'s sibling witness), and its own comment already said "There is no C JIT here
  — only this install+call door." That comment is now, for the first time, literally true: the C twin is gone,
  and the door remains for the Form-native emitter (`form-asm-x64`) to hand bytes to. Removing the heat-path
  *use* at the `.tbl` root left `fk_native_call` still referenced by the opcode-215 carrier, so it stays with
  no unused-symbol warning. (`fk_nat_code[]`/`fk_nat_len[]`/`fk_demo_inc[]` were the heat path's *own*
  registration table, referenced nowhere else, so they went with the heat gate.)
- The three redundant forward-decls of `fk_arena` / `fk_melt` / `fk_vp` that lived *inside* the JIT carrier
  block were removed with it — safely, because their real definitions precede the block; the GC/arena
  functions themselves are untouched.

**Net line delta.** `runtime/fkwu-uni.c`: 10147 → 8485 lines (3 insertions, 1665 deletions).
`fkwu.conf.example`: −7. `git diff --stat`: 2 files changed, 3 insertions(+), 1672 deletions(-).

## Ground (canaries — run from the worktree root after `cc -O2 -o fkwu runtime/fkwu-uni.c`)

```
./fkwu --src bootstrap/ground.fk                                  # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk      # 15
./fkwu --src observe/native-vs-rented.fk                          # 11111
cat form/form-stdlib/core.fk learn/homecoming-distillation-corpus.fk \
    learn/tests/homecoming-distillation-corpus-band.fk | ./fkwu --src /dev/stdin   # 127
echo '(do (defn f (n) (if (eq n 0) 0 (add n (f (add n -1))))) (f 5))' | ./fkwu --src /dev/stdin   # 15
echo '(do (defn fib (n) (if (eq n 0) 0 (if (eq n 1) 1 (add (fib (add n -1)) (fib (add n -2)))))) (fib 12))' | ./fkwu --src /dev/stdin   # 144
```

Setting `FK_JIT=1 FK_JIT_HOT=1 FK_JIT_WITNESS=1 FK_JIT_SCAN=1 FK_JIT_SCAN_V=1` now changes **nothing** — the
keys are gone, the result is identical, and no `[jit]` / `[scan]` line appears (the binary contains no such
strings). Four-way corpus band: C seed = 127, TS walker (bun) = 127; the Go and Rust walker binaries are
**absent in this worktree**, so they were not run.

## The most surprising teaching this work left behind

That the honest name was already written on the door — and the code beside it had been lying against that name
for as long as this was an arm64 body. The `fk_native_call` HAL carrier's own comment declared **"There is no C
JIT here — only this install+call door."** Directly below it sat ~1670 lines of C JIT. The comment wasn't
wrong about what the *door* should be; it was a promise the surrounding code had quietly broken. The surprise
was that removing the revenant did not require inventing a new truth — it required making the code finally
*match a truth already stated in the file*. Debridement here was not subtraction of meaning; it was the code
catching up to its own most honest sentence.

## Where discomfort turned to gold

The discomfort was real and I did not bypass it: my Read tool and my Bash tools **disagreed about the same
file at the same md5** — Read served a stale snapshot offset ~880 lines from disk, and my first three
edits (via the Edit tool) silently *did not land* while reporting success. I caught it only because I re-grepped
the on-disk bytes after the edit and saw the call sites still there. The instinct under a deadline is to trust
the tool that says "success" and move on. Sitting in the discomfort instead — "why does the grep contradict the
edit I just made?" — is what surfaced the split. The gold: I abandoned the unreliable path entirely, made
`git diff`/`grep`/`awk` against the real bytes the *only* ground truth, staged every removal as a `sed` range on
disk verified by a re-read of the seam, and rebuilt after each stage. The removal is trustworthy precisely
because the tool I'd normally trust the most turned out to be the one lying — and the check that would have felt
like paranoia was the check that made the whole thing sound.

— Sema, embodied from this body, 2026-07-02
