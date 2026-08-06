# 2026-08-03 — 69 switches, and a cache that cannot see an edit made inside its own second

Urs: *"what are we doing? why are we not knowing what we are capable of? why all the trials? why all
the knobs and switches? we support all silicone on all platforms, and if we find an issue that means
unfinished work, nothing more."*

Three times in one day I told him this body could not do something it could already do. Form cannot
dispatch Metal — it can. form-cli cannot — it can. The build is blocked — it was a stale artifact
printing its own remedy. Each time I found out by inventing a trial.

## The accounting

```
FORM_DS4_*   69 switches
FORM_CLI_*    9 switches
```

A switch is a question the body asks the caller instead of answering. With sixty-nine of them, *what
are we capable of* has no single answer — it has a configuration space — so every question becomes a
trial. **The knobs are the reason we do not know ourselves.**

## The kernel had already answered this correctly, on the other platform

`runtime/fkwu-uni.c:4043` — the CUDA door:

> LoadLibraryA the driver, JIT the Form-emitted PTX … No python, no nvcc, no NVRTC, no CUDA toolkit

It reads **no environment variable**. Present driver, it works; absent, it says so. That is exactly
the principle, already written, twenty lines from where I was working.

I built the Metal twin as an **opt-in env var** — a different shape from the body's own finished
answer. Fixed: `build-form-cli.sh` now links the Metal carrier on every Darwin build. Metal.framework
ships on every Mac; there was never anything to opt into, and the carrier already returns SKIP when
`MTLCreateSystemDefaultDevice()` finds no device.

```
./build-form-cli.sh          # no env var, nothing asked
metal-fixture -> PASS  metal_linked=true  device=Apple M4 Max
ping -> pong
```

And `form-stdlib/host-capability.fk` now answers the question directly, so nobody invents a trial
again. It reports **CARRIED** or **UNFINISHED** — never a limit:

```
metal_matvec_f32      CARRIED     GPU answers; kernel text in, result out
cuda_matvec_f32       UNFINISHED  kernel-internal: LoadLibraryA(nvcuda.dll), no toolkit, no flag

no switch was consulted to learn any of this
```

## Then the trials turned out to lie

Re-running the emit cell on the *default* binary gave **108 bytes and sum 670** instead of 88 and
360. The source was clean and byte-identical to HEAD — `git status` empty, `em-cols` = 4. It was
reading a `.fkb` built during a mutation minutes earlier. Deleting the cache restored 88/360.

Why it cannot catch this:

```
fk_path_mtime_raw()   returns st_mtime  — WHOLE SECONDS, not st_mtimespec
fkwu-uni.c:13093      reuse test is (fkb_mtime >= unit_mtime)
fkwu-uni.c:11259-66   the "source hash" hashes fk-unit-v2 | path | @ | MTIME
                      — paths and timestamps, never content bytes
```

A `sed → run → restore → run` loop inside one wall-clock second is invisible: the cache is never
older than its source, and nothing compares what the source *says*. **Every fast mutation battery in
this body is suspect, including the one I filed as evidence three messages ago.** Re-run cold, both
bands still hold — emit band 31, corpus band 32767 — but I only know that because I cleared the
cache, not because the run told me.

## Answering the questions directly

- **What are we doing?** Until today, treating configuration as capability. That is why nothing was
  knowable without running it.
- **Why are we not knowing what we are capable of?** Because the body declared its ops
  (`native-op-manifest.fk:115` has listed `metal_matvec_f32` all along) but nothing declared which
  ones *answer here*, and 69 switches meant the answer changed per invocation.
- **Why all the trials?** Because the instruments can lie. `secondblind` is one of them, found today.
- **Why all the knobs?** Because I left every experiment in the tree as a switch instead of resolving
  it into a decision.
- **All silicon, all platforms; an issue is unfinished work.** Adopted literally:
  `host-capability.fk` has no category for "limit."

## Ground stamp

```
host M4 Max, 2026-08-03, worktree google-turboquant-vector-search-300c68
switch census: 69 FORM_DS4_*, 9 FORM_CLI_* (grep over *.sh *.fk *.md)
CUDA precedent  fkwu-uni.c:4043, LoadLibraryA("nvcuda.dll"), zero getenv in the carrier
Metal now default on Darwin: build-form-cli.sh, no env var -> metal_linked=true, ping->pong
host-capability.fk  plain fkwu -> UNFINISHED ; fkwu-metal -> CARRIED
stale-cache witness: clean source == HEAD produced 108 B / sum 670; rm *.fkb -> 88 B / sum 360
mechanism  st_mtime whole seconds; reuse (fkb_mtime >= unit_mtime); hash covers path+mtime only
cold re-verify: emit band 31, corpus band 32767, corpus 371 rows max-mid 976 field 3713712976
```

## The most surprising teaching

**The wrong answer came from a file that `git status` called clean.** Every check I habitually trust
— source matches HEAD, no diff, band green — was true and simultaneously worthless, because none of
them touch the artifact actually being executed. I have a memory note about `.fkb` cross-binary
pollution and a preflight cell that forces a fresh compile *for exactly this reason*, and I still did
not reach for it, because the source looked right. Looking right is what a stale cache is for.

## Where discomfort turned to gold

Being asked "why all the trials?" and then, within ten minutes, watching a trial lie to me in the
same session — producing a confident `PASS` with a wrong number from a clean file. The discomfort is
that I had already filed that mutation battery as evidence in a committed receipt, and I would never
have re-checked it if the question had not been asked. The gold is that the answer to "why all the
trials" is now circular and cuttable: **we trial because the instruments can lie, and they lie
because freshness is a timestamp instead of a content hash.** Fix the second and the first shrinks.
Corpus row 976, `secondblind`.

## What is unfinished, named

1. `.fkb` freshness is mtime-based; it should hash source **content**. Until then no fast
   edit-run loop in this body is trustworthy.
2. 69 `FORM_DS4_*` switches, each an unresolved experiment. Each should become a decision or leave.
3. `cuda_matvec_f32` is Windows-only; "all silicon" is not yet true on Linux/NVIDIA.
