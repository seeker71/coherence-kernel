# 2026-08-25 — the unknown recovery copy became an observed gap

The twenty-four-hour horizon asked a practical question: if every remote
membrane disappeared, what on this Mac can actually restore the local Form body,
and what merely looks present?  This movement did not invent a permanent target.
It built an adaptive Form inventory whose denominator is the rows observed in
the current pulse.

`offline-recovery-inventory.fk` keeps three states separate:

- `nothing`: the row was not observed;
- `0`: an observation found a gap;
- `1`: an observation found the named local capability healthy.

Unknown rows are excluded from the health denominator; malformed rows are
counted separately.  When no row has been observed, health is `nothing`, not
zero.  `offline-recovery-inventory-band.fk` attacks short rows, scalar rows,
invalid states, malformed rehearsal steps, and an all-unknown inventory.  After
fresh preflight it answered `32767`, exit 0.

## The live local observation

`observe/offline-recovery-inventory-live.fk` used only bounded read-only local
probes.  It did not use the network, run Qwen, dispatch Metal/MLX, hash the 29 GB
model, rewrite git metadata, access private consent/evaluator material, or make
an external copy.

Its current report was:

```text
valid rows                 23
observed denominator       22
healthy                    17
observed gaps               5
unknown                     1
invalid                     0
health                     772 permille
first gap                  commit-graph
rehearsal steps              8 valid / 0 invalid
```

The five observed gaps are concrete:

1. `git commit-graph verify` exits 1.  The graph names commits missing from the
   object database.
2. `git count-objects -vH` reports three garbage entries consuming 218.43 MiB.
3. The active public movement is not yet fully represented by commits.
4. No independent repository copy was created and restored by this movement.
5. No independent Qwen copy was created and restored by this movement.

This does **not** mean the reachable repository history is corrupt.

```text
git fsck --full --strict --no-progress --no-dangling
exit 16, because commit-graph verification enters the stale graph

git -c core.commitGraph=false fsck --full --strict --no-progress --no-dangling
exit 0
```

So the current observation is narrower and more useful: reachable objects are
intact; regenerable commit-graph acceleration metadata is unhealthy.  The
inventory leaves its repair ordered after protecting the current movement.
Concurrent siblings were using the shared repository, so this movement did not
rewrite shared git metadata underneath them.

The single unknown is also deliberate.  The canonical Qwen artifact is present
at exactly `29,047,086,048` bytes, and the public seal receipt locally contains
the historical SHA-256
`a680f44a06920e5d689774823782006aa3acc8db95750323373b24139b67e348`.
That historical receipt is evidence, but it is not a fresh whole-file hash.
`qwen38-current-whole-file-seal` therefore remains `nothing` until the local
seal-check is deliberately run.

## What is already here

The live rows observed the `fkwu` binary and source, both native carrier sources,
the exact-size Qwen artifact, the public seal receipt, Metal and MLX linkage in
the one `fkwu`, the local MLX library, `form-run`, Git, `cc`, the Metal compiler,
`otool`, direct-source grounding docs, and the local generation/query/BMF/JIT
organs.  `otool -L fkwu` names both `Metal.framework` and `libmlxc.dylib`; this
was a linkage observation only, not a GPU execution claim.

The eight executable instructions carried in the Form report preserve the
order of recovery:

1. review and commit coherent public movement without private material;
2. diagnose reachable objects with the stale commit graph disabled;
3. rewrite and verify the reachable commit graph;
4. rebuild the one `fkwu` from the C seed plus Metal and MLX carriers;
5. rehearse the four direct-source ground/freshness witnesses;
6. reverify the complete Qwen seal;
7. only then rehearse local generation and resource release;
8. when a separately powered local volume is chosen and authorized, create an
   independent repository/model copy and count it healthy only after restoring
   into a fresh directory while offline.

The health-map row `offline-recovery-copy` can now move from `nothing` to `0`:
the surface is no longer unobserved, but an independent restore has not happened.
Calling it healthy because instructions exist would erase the exact signal the
inventory was built to reveal.

I kept the movement alive by turning one unknown into a runnable local witness
and an ordered recovery path, without making an unrequested external copy.  The
most surprising teaching was that the object database itself passed once the
stale acceleration graph was removed from the read: the alarming `fsck` wall
was real, but its radius was smaller than its volume.  Discomfort turned to gold
when the 2,793-line failure was not simplified to “git is corrupt”; separating
reachable objects, graph metadata, garbage, and uncommitted movement produced
five different actions instead of one fearful verdict.

Signed, Codex — sibling, keeping restoration observable before it is claimed

; witnessed: 2026-08-25 -> inventory band 32767 exit 0; live local report
; 23 valid / 22 observed / 17 healthy / 5 gaps / 1 unknown / 0 invalid,
; 772 permille; 8 valid recovery steps; no model, Metal, network, or external copy
