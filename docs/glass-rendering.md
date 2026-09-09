# Native buffered Glass

The renderer retains its front/back semantic frames. Successful output commits
the back frame; failed output retains the front frame for retry. An unchanged
frame emits no bytes. Equal-width ASCII segments update in place; changed widths
and non-ASCII text use a whole-row fallback. Styles reset before changing so
background and weight do not leak between segments.

A frame is built as Form bytes before writing. It uses absolute row positions,
not line feeds, and a fresh renderer paints over the previous screen without a
separate blank-screen clear. Cursor movement is bracketed inside a
[DEC 2026 synchronized update](https://gist.github.com/christianparpart/d8a62cc1ab659194337d73e399004036).
Terminal support determines whether intermediate writes are visually batched;
the Form buffer and changed-region suppression work independently of that support.
No Rich, Python renderer, shell renderer, or new external service is involved.

Run `./fkwu observe/form-glass-run.fk` in the viewing terminal. After a native
binary repair, relaunch that carrier once to replace its already-running children.
Thereafter a dependency change renews the renderer and the three sensor processes;
a renderer-only pool renewal keeps sensors and skips repeated admission output.

Bounded checks (each also has its printed verdict):

```text
./fkwu form/form-stdlib/tests/form-glass-render-batch-band.fk  # 4095
./fkwu form/form-stdlib/tests/form-glass-heal-band.fk          # 262143
./fkwu observe/form-glass-jit-hold-current-run.fk             # checks=4095/4095
./fkwu observe/form-glass-frame-budget-run.fk                 # measured, not fixed
```

`DOING dsk=` is placed first so a narrow view retains it. `cpuT` is cumulative CPU
time, not utilization. Owner CPU carries microseconds; absent or stale values
show `?`. Retained detailed tiles still carry last-observed evidence. Organ
coverage requires the exact census identities, not just an equal row count.

## Frame-local metric memory

`fgd-metric-index(rows)` builds a reclaimable native keyed trie. Its depth follows
the row count; keys are exact bytes, not a fixed field list. The first row for an
ID wins, including an unavailable row; absent IDs return an empty list. Old index
snapshots stay unchanged when another frame is built. Dropping a frame lets its
index and rows be reclaimed, unlike a kernel record whose fields remain rooted
for the process lifetime.

```text
./fkwu form/form-stdlib/tests/form-glass-metric-index-band.fk  # 65535
./fkwu observe/form-glass-metric-index-measure.bml
```

At the measurement door, enter `map` followed by Enter. Repeat in a separate
invocation with `record` to measure the previous implementation. Each bounded
run checks 256,000 lookups over 1,000 changing frames and prints elapsed time,
heap capacity, reclamations, and permanent-record allocations. No live sensor,
microphone, or renderer is started, and no transcript is read. Compare repeated
warm runs: bounded memory does not by itself establish lower CPU cost.

Transcript bodies now pass directly into `ftc-byte-text` segments. The native
wrapper walks only visible spans, retains UTF-8 character boundaries, replaces
terminal controls with spaces, and leaves off-screen suffixes untouched. It does
not build every growing text prefix as an interned string. The string-returning
`fglm-clean` remains available to bounded callers, outside the live render path.
`./fkwu form/form-stdlib/tests/form-glass-transcript-bytes-band.fk` checks this
contract, including unchanged-frame suppression; the expected verdict is 1023.
