# 2026-07-02 — silent nil, made loud: seven degradation paths the four-way proof was already voting against

## Ground

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
./fkwu --src observe/native-vs-rented.fk                       # 11111
# corpus band, four-way (fkwu = Go = Rust = TS)                # 127
```

Urs: *"xx silently returns nil when yy: no alarm bells? no let's address this right now?"* — pointing
at `fk_cons_val` returning nil (handle 1) on a full heap, and at the whole class. The rebuke landed:
the receipt for the last cliff fix even *names* the disease ("a partial thing accepted as whole")
while leaving instances of it alive in the seed. I had fixed only the two sites on my own critical
path and walked past the rest.

## The audit, and its honest limit

A five-lens workflow swept `runtime/fkwu-uni.c` and catalogued ~140 candidate silent-degradation
sites (capacity exhaustion, allocation failure, syscall-failure-as-data, truncation, skipped work).
The **verify phase was killed wholesale by a Fable-5 rate limit** after 1.8M tokens — every adversarial
verifier died with "reached your limit," and the script crashed dereferencing the null verdicts. So
the ~140-item list is **Find-phase only, unverified**. Switched to Opus 4.8 and verified the *core
evaluator/parser* paths by direct grounded reading — because fixing an unverified finding is itself
accepting a partial thing (an unproven claim) as whole, the exact disease.

A parallel recon agent read the three proof-walkers: **Go panics, Rust `panic!`s, TypeScript throws —
and none has a fixed cap at all** (host GC, dynamic maps/vectors). The silent-nil / silent-overwrite /
silent-truncation shapes live *only* in the C seed. The four-way proof was already voting 3-to-1
against silence; we just hadn't read its disagreements as a bug-finder.

## The seven fixes — every one the same disease

| # | site | was | now |
|---|------|-----|-----|
| 1 | `fk_cons_val` @3620 | heap-full → `return 1` (nil) | **`fk_die`** — cannot melt here (live C-local intermediates aren't on the value stack to trace) |
| 2 | `fk_list_push` @2337 | heap-full → `return acc` (drops element) | **`fk_die`** |
| 3 | `fk_walk` tag 129 @6739 | drops roots past cap after melt (no-else `if`) | **`fk_die`** if still full after melt |
| 4 | `fk_walk` tag 64 @6128 | `fk_rp = FK_RECORD_CAP-1` — **aliases two distinct records onto one slot** | **`fk_die`** |
| 5 | `fk_walk` tag 64 @6146 | drops keys past `FK_RECORD_MAX_KEYS` | **`fk_die`** |
| 6 | `fk_bd_push` @7094 | binding stack `fk_bd_s[128]`, silently drops bindings past 128 | **raised 128→1024** (`FK_BD_STACK_CAP`, raisable-constant class) **+ `fk_die`** past 1024 |
| 7 | `argn[256]`/`iargn[256]` @7330/@7361 | >256-arity call → truncates args, desyncs parser | **`fk_die`** |

Kept documented, not changed: `fk_mem` tags 13/14 mask indices `& (FK_MEM_CELL_CAP-1)` — that is
deliberate ring-addressing (O(1) mutable cells), not an exhaustion path.

`fk_bd_push` was the one that earned a *raise*, not just a death: generated classifier programs
nest many sequential `let`s in one scope, and 128 is genuinely too low — the same "`--src` is not a
gate / this is a raisable constant" lesson as `FK_NODE_CAP` and `FK_AST_NODE_CAP`.

## Witnessed

- Canaries after rebuild: **42 / 15 / 11111**, corpus band **127 four-way**.
- New permanent body cell `form/form-stdlib/tests/binding-depth-band.fk`: a single scope of **140
  chained bindings** resolves to **140 across all four** (fkwu = Go = Rust = TS). Under the old
  128-slot cap, bindings v128..v139 were silently dropped and the deep value would have been wrong
  with no witness — this is the regression that would have caught that silence, now embodied.
- Loud-death demonstration: a **1100-deep scope** (past the 1024 cap) prints
  `fk_bd_push: parser binding-scope stack exhausted ...` and exits 1 — a loud death, not a silent
  wrong number.

## Honest floor (named, not bypassed)

- **The audio regressions could not be run.** The scratchpad was cleared at 15:19 (at the model
  switch); the ~1,700 wavs behind `full12=40096` and `p100run=171` lived only in `/tmp` and are gone.
  The changed paths the audio pipeline *shares* — heap melt / fbroots drain — are exercised by the
  canaries and corpus band (which allocate and melt) and by the new binding-depth cell; the rest of
  that pipeline is host-I/O + arithmetic these changes don't touch. But the literal numbers are
  unreproduced this session, and I won't report them as re-verified.
- **~140 catalogued sites remain unverified.** The audit's verify phase was rate-limited. Most are a
  *different class* — host-carrier I/O (network/sensor/file) returning empty on failure, which is
  arguably "pending is honest," not silent corruption — and must not be blanket-killed. That triage
  is a named follow-up, not done here.

## The most surprising teaching this work left behind

The seed was the only one of four bodies that lied. The walkers — written later, on host runtimes —
never *had* the option to silently degrade: panic/throw is the host default, and they have no fixed
caps to overflow. The C seed's silence wasn't a design choice; it was an artifact of hand-writing
bounds checks that returned *a* value because returning was the easy branch to type. And the cure had
been sitting in plain sight the whole time: every silent path is exactly a place where fkwu would
disagree with three walkers that refuse. The four-way discipline, built to prove *agreement*, was
already a silence-detector — we had just never read its votes that way.

## Where discomfort turned to gold

Two discomforts, both wanting to be bypassed. First: the audit's verify phase died after 1.8M tokens,
and the pull was to trust the unverified list and fix all ~140 sites blindly — which would have been
accepting an unproven claim as whole, the very disease. Witnessed instead: I fixed only what I could
ground by reading, and named the remaining ~140 as unverified. Second: the scratchpad wipe erased the
regressions Urs asked for, and the pull was to quietly skip them and report green — a skipped check
reported as done is itself a changeling. Witnessed instead: I said plainly the audio numbers can't be
reproduced, and *built a better, permanent regression* for the exact paths I changed. The loss forced
the proof out of a `/tmp` number and into a committed four-way cell — the wipe is why the guard is now
durable.

## Corpus

Row 645 **changeling** — a whole quietly swapped for a partial thing read as if complete (fresh; the
returned nil, the aliased record, the truncated list — every silent-degradation path this work made
loud). It follows row 644 **apocope** (silent truncation): *apocope* is losing the ending and reading
the remainder as whole; *changeling* is the whole itself secretly replaced. The disease has its two
names now.
