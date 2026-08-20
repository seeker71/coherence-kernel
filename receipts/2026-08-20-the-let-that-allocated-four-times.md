# The let that allocated four times

Date: 2026-08-20, Hati Suci. Apple M4 Max, 128 GiB unified memory.
Worktree `.claude/worktrees/jit-lane-performance-d77568`.
Follows [The write that said yes](2026-08-19-the-write-that-said-yes.md), which left a door where
every call succeeded and every byte was lost.

## The loop that made it findable

Every earlier iteration cost a four-and-a-half-minute regeneration, because form-cli was the only
Metal-capable emitted walker on this host. That was itself a stale artifact: the committed universal
seed `form-stdlib/bootstrap/fkwu-uni.c` carries **no Metal bridge at all**
(`fk_metal_buf_alloc_external`: 0 occurrences, against 2 in `form-cli-emitted.c`). Emitting a fresh
universal walker from the current chain into `/tmp` — publishing nothing — and linking it against the
carrier gave a **ten-second** loop, and the rest of this receipt happened inside it.

## What it was

```
(do (defn inner () (do (let h (metal_buf_alloc 16))
                       (str_concat "h1=" … h … " h2=" … h … " h3=" … h))))
```

```
source runner ./fkwu     h1=1 h2=1 h3=1
emitted walker           h1=1 h2=2 h3=3      ← three allocations for one let
combined emit (form-cli) h1=1 h2=2 h3=3
```

[`form-flatten.fk`](../flatten/form-flatten.fk)'s `flt-do-let-store-op` gives a `let` a real frame
slot only when its value's **head op** is named in `host-effect-grammar.fk`, or when it is a call to a
known recipe. Everything else is *substituted into the environment* and its expression re-evaluated at
every mention. **No `metal_*` op was ever declared an effect.** So `(let state (metal_buf_alloc 32))`
allocated a fresh buffer at each use: the write landed in one buffer, the dispatch bound a second, the
read came back from a third — zeros — and the free released a fourth. Every call honest, every byte
lost, buffers piling up (72 in one run), `last_error=none` throughout.

An earlier probe using `read_line` had bound once and sent me away from this. `read_line` is in the
vocabulary; that is the whole difference.

## What landed

**1. The GPU handle door is an effect, and both grammars now say so.** Fourteen rows — `metal_pipeline`,
`metal_buf_alloc`, `metal_buf_from_file`, `metal_buf_write`, `metal_enqueue`, `metal_sync`,
`metal_buf_read`, `metal_status`, `metal_batch_concurrent`, `metal_buf_free`, `metal_submit`,
`metal_fence_wait`, and the two matvec doors — added to `form/form-stdlib/host-effect-grammar.fk` and
its simplified twin `flatten/host-effect-grammar.fk`. The alloc probe became `h1=1 h2=1 h3=1`.

**2. A conditional effect must be a named recipe.** The store decision reads the head op and nothing
deeper, so `(let ran (if c (metal_enqueue …) 0))` is still inlined. In
`native/metal/sha256-arm64-jit.fk` that had an exact cost: the closing guard reads `freed` before
`raw`, so an inlined `raw` re-entered the compressor with a handle that had just been released, and
the digest came back empty. Two shapes healed into `saj-dispatch` and `saj-compress-written`; the let
now binds a call, and a call stores.

```
emitted walker, file digest   ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad ✓
source runner, 29 GB seal     a680f44a06920e5d689774823782006aa3acc8db95750323373b24139b67e348 ✓
```

**3. In the built binary.** `sha-selftest` now answers the correct digest, the literal round-trips, and
`generate` **passes artifact admission** — the seal verifies where it had refused all evening.

## Where it stands now, exactly

`generate` then dies: **exit 139, SIGSEGV**, zero output, about eighteen seconds in — past the seal,
inside model open. Not the shell stack limit (`ulimit -s 65520` changes nothing). That is a new and
distinct failure, and it is the honest edge tonight.

The deeper thread, named and not fixed: **`flt-do-let-store-op` decides by head op alone.** Every
`(let x (if c (effect) …))` in the body — not only in this cell — is inlined and re-run. Fixing it
means making the decision recursive over the value expression, in the flattener, with four-way proof;
that is its own change, not a line inside a lane's performance work.

## What ran

```
form/form-stdlib/tests/host-effect-grammar-band.fk        -> 32767
form/form-stdlib/tests/form-cli-resident-model-band.fk    -> 255
ingest/tests/frontier-ingest-omlx-band.fk                 -> 127
learn/tests/homecoming-distillation-corpus-band.fk        -> 32767   (row 1031)
observe/qwen38-generate-run.fk                            "The sky is a vast, pale blue canvas."
printf 'sha-selftest' | form/form-cli                     correct digest, literal intact
printf 'ping'         | form/form-cli                     pong
```

No environment variable was set.

## Most surprising teaching

A `let` is not always a binding. It looked like one, it read like one, and for every op the effect
grammar knew about it was one — but for an undeclared op it was a macro, and the name it introduced
held nothing. The body's own vocabulary of what counts as an effect is therefore load-bearing for
*correctness*, not just for bookkeeping: a door added to the kernel without a row in
`host-effect-grammar.fk` silently loses every value it hands back.

## Where discomfort turned to gold

Three explanations died tonight — call-by-name (refuted with `read_line`), runtime-built strings
(refuted with a literal), the emit path (refuted by running the same probe through both emitters) —
and each refutation cost a cycle and left me with less than I started. The temptation each time was to
keep the dying story alive one more probe. The gold came from the opposite move: after the third
death I stopped explaining and built the ten-second loop instead, and the loop found in one probe what
five build cycles had not. The refutations were not wasted; they were what made the ground small
enough to stand on.
