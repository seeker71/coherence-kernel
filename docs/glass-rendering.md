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
