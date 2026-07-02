# 2026-07-01 -- C-seed correctness, bounds-safety, named constants, and one perf fix

## The ask

Following the same-day readability reformat (`receipts/2026-07-01-c-seed-readability-reformat.md`),
Urs asked for a real rewrite: lift magic numbers, verify correctness, add bounds checks, look at
performance -- and explicitly authorized changing code, overriding the "don't grow the C seed"
default for this file a second time. Scope was set via two decisions up front: **full sweep, no
artificial stopping points** across every subsystem in one continuous push, and **loud failure**
(`fk_die` -- write a message to fd 2, `exit(1)`) for hard-invariant violations (OOM, memory-safety
violations) while ordinary index checks keep the file's existing silent safe-default convention
(`if (idx < 0 || idx >= cap) return 0;`).

Three research passes mapped the file first (subsystem boundaries, ~135 fixed-size buffers, the
existing "stone N" naming vocabulary, and the regression-test landscape) before any code moved --
that reconnaissance is not repeated here; only what actually changed is.

## Regression harness

No automated test runner existed in this repo before today. Built one (Python, scratch-only, never
landed in the tree -- the hard zero-`.sh`/zero-`.py` gate stays intact): walks all 326 `*-band.fk`
files, parses each one's own `; preludes:` / `; Run:` header to resolve its library dependencies,
skips anything that calls into hardware/network primitives (socket/http/gpu/metal/audio/camera/mic/
bluetooth/wifi -- 24 files) or is independently confirmed to block on real network I/O (9 files,
found by their first run timing out), concatenates the rest, and runs each through `fkwu.exe --src`.
261 of 326 bands are CI-safe this way; 41 have prelude paths this checkout's directory layout
couldn't resolve (stale header comments, unrelated to this pass -- left alone). Captured a baseline
snapshot of every CI-safe band's actual output against the unmodified binary, then re-ran the full
sweep after every subsystem below, diffing against that baseline. **Zero drift at every single
checkpoint**, ten checkpoints total, ending with this final one:

```text
{
  "total_bands": 326, "ran": 261, "skip_hardware": 24, "skip_unresolved": 41, "timeout": 0
}
baseline ran=261  current ran=261
ZERO DRIFT across 326 baseline bands.
```

Plus the four core grounding witnesses, unchanged throughout:

```sh
gcc -O2 -o fkwu.exe runtime/fkwu-uni.c -lws2_32 -lwinmm -lavicap32 -luser32 -lwlanapi -lbthprops -lwinhttp
./fkwu.exe --src bootstrap/ground.fk                     # 42
./fkwu.exe --src bootstrap/ground-recursive.fk 10         # 55
./fkwu.exe --src <native-vs-rented concat>                # 11111
./fkwu.exe --src <host-os-membrane concat>                # 8191
```

## What changed

**21 new named constants** replace bare size/capacity literals that were previously scattered
across the file, several under names that made numerically-coincidental-but-conceptually-distinct
capacities look related when they aren't (`FK_NODE_CAP` -- the hash-cons value table -- and
`FK_AST_NODE_CAP` -- the parsed program's syntax tree -- are different tables that both happen to
be 65536; kept as two names on purpose, with a comment at the definition site naming the trap).

**One new primitive**, `fk_die(msg)`: writes to fd 2 via `write()` the same way the file's existing
`fk_mw`/`fk_mc` debug helpers already do (no stdio, no `FILE*`/`stderr` symbol -- not portable
across this file's three platforms), then `exit(1)`. Used at 17 call sites, all either allocator
exhaustion or a state that would otherwise be a silent out-of-bounds write.

**Real bugs found and fixed** (not the confirmed-critical items expected going in -- see "what
*wasn't* a bug" below):

- **`fk_walk`/`fk_walk_body`**: the node tag `t` was never range-checked before indexing
  `fk_arms[t]` on literally every evaluation step -- a corrupt or adversarial AST node could write
  out of bounds on the interpreter's hottest path. Added the check once at each of the two
  independent entry points.
- **`fk_fn[2048]` was undersized against its own siblings.** `fk_fnar[4096]`, `fk_nat_exec[4096]`,
  and the fn-value sentinel band's valid-index cap (4096) all agreed on 4096; only `fk_fn[]` itself
  was declared at half that, while every bounds check gating access to it used the *larger* bound.
  A program defining 2048-4095 functions could pass every existing check and still write `fk_fn[idx]`
  past the end of its real array. Widened `fk_fn[]` to match its siblings (`FK_FN_CAP = 4096`)
  rather than tightening ten call sites down to 2048 -- same fix, changes nothing for any program
  that worked before, and the previously-silent corruption case now just works.
- **`fk_smknode`** (every AST node allocation during parsing) had no check that `fk_node_count`
  stayed under the AST table's capacity before writing through it. Added.
- **Binary-table deserialization** (`fk_run`, loading a flattened `.tbl`): the function count and
  node count read from the table's own header were used as loop bounds with no validation against
  `FK_FN_CAP`/`FK_AST_NODE_CAP` before the loop started writing `fk_fn[]`/`fk_node[]`. A malformed
  or adversarial table could overflow either. Added both checks.
- **Six call-dispatch sites** across `fk_walk` and `fk_walk_body` (tags 12/240/241, twice each, plus
  tag 44's hash-cons'd apply path) read a callee index from the AST or from hash-cons'd runtime data
  and indexed `fk_fn[]` with it unchecked. Added guards, matching the existing house pattern used
  one tag over (244) that was already doing this correctly.
- **`fk_jtramp`**: the existing bounds check was a single `||` chain -- `callee < 0 || callee >=
  FK_FN_CAP || fk_fnar[callee] != argc` -- whose *fallback* branch did `fk_walk_body(fk_fn[callee],
  fp)` assuming callee was in range. It wasn't, on two of the three ways into that branch. Split the
  out-of-range case out so it returns before ever touching `fk_fn[callee]`.
- **`fk_jcall`**: callee was never validated at all before being used in either arm of
  `fk_jtramp(callee,...) : fk_walk_body(fk_fn[callee], fp)`. Added at function entry.
- **`fk_jit_lower`**: read `fk_fn[f]` unconditionally while the very next read, `fk_fnar[f]`, was
  already guarded by a ternary two lines down. One upfront check now covers both.
- **`fk_rmtree` / `fk_inv_walk`** (real, confirmed stack-buffer overflow): both built a child path
  via `sprintf(buf, "%s/%s", dir, e->d_name)` into a fixed 4096-byte stack buffer with no length
  check, and both are *recursive* -- the built path becomes the next call's input, so either a deep
  enough tree or one long filename overflows the buffer. Added `fk_path_join`, which checks the
  combined length and calls `fk_die` rather than truncating -- truncating would be actively worse
  here, since `fk_rmtree` deletes whatever path it's handed and a silently-truncated path risks
  deleting the wrong thing.
- **`fk_unlink_segments`**: the same `sprintf`-into-fixed-buffer pattern, also feeding `unlink()`.
  Added the same class of check.
- **`fk_arena`** (the hash-cons table's own initializer, foundational to every cons/list/record
  operation in the interpreter): `malloc` with no NULL check, immediately written through. Added.
- **15 inline string-pool growth sites**, the same three-line `while (...) { fk_scap_b *= 2; fk_sb =
  realloc(...); }` idiom copy-pasted throughout the parser and evaluator wherever a string gets
  built incrementally, none of them NULL-checked. Added one shared `fk_sb_check()` helper and a
  call after each of the 15 (plus the 3 other `fk_sb`-adjacent allocation sites already covered).
- **`fk_fbox`** (the float-boxing allocator): malloc and realloc, neither NULL-checked. Added.
- **The root-level self-JIT install path**'s `malloc(n)` for the native code image had no NULL
  check, while the *other* JIT-install call site (the heat-gated `fk_ensure_native_ex` path) already
  had one. Added, falling back to the normal interpreted walk on failure -- the same fallback this
  function already takes whenever JIT crystallization isn't possible for any other reason.
- **`fk_vp`**'s value-stack-overflow guard was already a hard stop, just spelled out by hand (four
  `fk_mc()` calls writing `"vs!\n"` byte-by-byte, then `_exit(9)`). Replaced with `fk_die`, which
  also let the now-unused `_exit` extern be removed.

**What turned out *not* to be a bug**, checked directly rather than assumed from the earlier audit:
the flagged "unguarded strcpy" in `fk_skip_entry` is actually `fk_cstr(es, nb, 512)`, which already
clamps to its `cap` parameter internally -- safe. All five HTTP/TLS response-buffer `recv`/`read`
accumulation loops (the ones flagged as "unclear" earlier) already correctly shrink the requested
length by bytes-read-so-far every iteration -- safe. The two HTTP request-line `sprintf(req, "GET
%s HTTP/1.0...", path, host)` calls looked identical in shape to the real `fk_rmtree` bug, but
`path`/`host` are themselves bounded by the URL-parsing loops that fill them (`h < 511`, `q < 1535`)
before they ever reach `sprintf` -- the combined worst case fits `req[4096]` with room to spare.

## The performance fix: `fk_sintern`

`fk_sintern` did a full linear scan over every already-interned string on each new intern call --
called once per identifier and string literal during parsing, so parse time was O(n²) in the number
of distinct strings in a program. Replaced with a fixed, never-rehashed, 131,072-bucket FNV-1a hash
index (`fk_shash`/`fk_snext`) layered *alongside* the existing `fk_so`/`fk_sl` arrays, which remain
the source of truth. The hash only accelerates lookup: every candidate match is still confirmed by
the original byte-for-byte comparison before being returned, so the design is safe by construction
-- a hash or chain bug's worst case is a false miss (falls back to re-interning, correct but not
faster), never a false match or a missed real duplicate. Found and closed one desync risk during
self-review before it shipped: the binary-table deserialization path writes directly into
`fk_so`/`fk_sl`, bypassing `fk_sintern` entirely, which would have left those strings outside the
new hash chains (a real behavior difference from the old linear scan, which found matches
regardless of insertion path) -- now registers them into the same chains at insertion time.

Verified three ways: the full 261-band regression (zero drift), a purpose-built stress test
(repeated identical 44-byte string literals compare equal across multiple call sites; distinct
strings, including two 50-byte strings differing only in their last character, compare unequal),
and that same stress test run against the untouched pre-this-turn binary for cross-validation --
both return `1`.

## Deliberately out of scope

Named in the plan up front, still true: ASCII character-literal magic numbers (`== 40`, `== 41`,
`== 34`, etc. -- hundreds of occurrences through the parser) were not lifted to named constants;
they're self-evident in context (open/close-paren checks in a Lisp-shaped parser) and renaming all
of them is a disproportionate diff for the risk/time budget, next to the sentinel/capacity work that
actually carried correctness value. Raw x86-64 instruction encoding bytes in the JIT codegen stayed
as hex literals with comments -- naming every opcode-encoding byte would make that code *less*
readable, not more. Two pre-existing duplicate top-level declarations (`fk_src_nat`/
`fk_src_nat_len`, and `fk_jb`/`fk_jbp`, each declared twice, legal under C's tentative-definition
rules but redundant) were found and left in place with an explanatory comment rather than merged --
removing a top-level declaration is a reuse/simplification change, a different mandate than this
pass's. No fresh device-metal receipts were captured for mac or Android; everything here was built
and verified on Windows, the platform this checkout is on. The fixes themselves are platform-generic
C, not Windows-specific, but that claim is unverified on the other two platforms tonight.

## Verification

```sh
gcc -O2 -o fkwu.exe runtime/fkwu-uni.c -lws2_32 -lwinmm -lavicap32 -luser32 -lwlanapi -lbthprops -lwinhttp
# exit 0, same 3 pre-existing benign -Wbuiltin-declaration-mismatch warnings as before this pass
./fkwu.exe --src bootstrap/ground.fk                      # 42
./fkwu.exe --src bootstrap/ground-recursive.fk 10          # 55
./fkwu.exe --src <native-vs-rented concat>                 # 11111
./fkwu.exe --src <host-os-membrane concat>                 # 8191
./fkwu.exe --src <string-interning stress test>            # 1
```

Full 261-band regression: zero drift, checked after every one of the eight subsystem passes (value
encoding + string interning + node/hash tables; parser; evaluator; JIT/x86-64 codegen; OS/
filesystem/sockets/HTTP/TLS/mesh; sensors/audio/video/GPU; the string-interning hash-index rewrite;
and this final full-file pass) plus the `fk_die` primitive's own addition -- nine checkpoints, zero
drift at every one.

`runtime/fkwu-uni.c`: 8,480 -> 8,783 lines (+303, all constants/checks/the hash index -- no
capability removed). Compiled binary: 332,017 -> 326,731 bytes.
